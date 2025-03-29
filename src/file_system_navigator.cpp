#include "file_system_navigator.h"
#include <iostream> // Добавить в начало файла
#include <fstream>
#include <algorithm>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

FileSystemNavigator::FileSystemNavigator(TerminalUI& ui, AudioManager& audio)
    : ui_(ui), audio_(audio) {}

std::vector<MenuItem> FileSystemNavigator::create_menu_items(const std::string& path) const {
    std::vector<MenuItem> items;
    
    try {
        if (path != "/") {
            MenuItem parent_dir;
            parent_dir.name = "..";
            parent_dir.function = "filesystem " + std::filesystem::path(path).parent_path().string();
            items.push_back(parent_dir);
        }

        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            MenuItem item;
            item.name = entry.path().filename().string();
            
            if (entry.is_directory()) {
                item.function = "filesystem " + entry.path().string();
            } else {
                item.function = "view_file " + entry.path().string();
            }
            
            items.push_back(item);
        }
        
        std::sort(items.begin(), items.end(), [](const MenuItem& a, const MenuItem& b) {
            bool a_is_dir = a.function.find("filesystem ") == 0;
            bool b_is_dir = b.function.find("filesystem ") == 0;
            return a_is_dir > b_is_dir || (a_is_dir == b_is_dir && a.name < b.name);
        });

        if (items.empty()) {
            MenuItem empty_item;
            empty_item.name = "Директория пуста";
            items.push_back(empty_item);
        }
        
    } catch (...) {
        items.clear();
        MenuItem error_item;
        error_item.name = "Ошибка доступа";
        items.push_back(error_item);
    }
    
    return items;
}

void FileSystemNavigator::navigate(const std::string& path) {
    size_t selected = 0;
    bool exit = false;
    auto last_redraw = std::chrono::steady_clock::now();
    std::vector<MenuItem> menu_items;
    
    while (!exit) {
        const auto now = std::chrono::steady_clock::now();
        
        if (now - last_redraw > 33ms) {
            menu_items = create_menu_items(path);
            ui_.draw_menu("Файловая система: " + path, menu_items, selected);
            last_redraw = now;
        }
        
        switch(ui_.get_key_input()) {
            case 'U': 
                if (selected > 0) {
                    selected--;
                    audio_.play_tick();
                }
                break;
                
            case 'D':
                if (selected < menu_items.size() - 1) {
                    selected++;
                    audio_.play_tick();
                }
                break;
                
            case '\n': {
                const auto& item = menu_items[selected];
                if (item.function.find("filesystem ") == 0) {
                    navigate(item.function.substr(11));
                } else if (item.function.find("view_file ") == 0) {
                    show_content(item.function.substr(10));
                }
                exit = true;
                break;
            }
                
            case 127:
                exit = true;
                break;
        }
        std::this_thread::sleep_for(10ms);
    }
}

void FileSystemNavigator::show_content(const std::string& path) const {
    std::ifstream file(path);
    if (!file) {
        show_error("Ошибка открытия файла");
        return;
    }
    
    ui_.clear_screen();
    std::cout << Constants::GREEN << "Содержимое файла:\n\n" << Constants::RESET;
    std::cout << file.rdbuf() << "\n\nНажмите любую клавишу...";
    getchar();
}

void FileSystemNavigator::show_error(const std::string& message) const {
    ui_.clear_screen();
    std::cerr << Constants::GREEN << "Ошибка: " 
             << Constants::RESET << message << "\n";
    getchar();
}
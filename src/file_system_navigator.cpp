#include "file_system_navigator.h"
#include <iostream> 
#include <fstream>
#include <algorithm>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

FileSystemNavigator::FileSystemNavigator(TerminalUI& ui, AudioManager& audio)
    : ui_(ui), audio_(audio) {}

    std::vector<MenuItem> FileSystemNavigator::get_menu_items(const std::string& path) const {
        return create_menu_items(path);
    }

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


void FileSystemNavigator::show_content(const std::string& path) const {
    std::ifstream file(path);
    std::stringstream buffer;
    
    buffer << Constants::GREEN << "Содержимое файла:\n\n" << Constants::RESET;
    
    if (file) {
        buffer << file.rdbuf();
    } else {
        buffer << "Ошибка открытия файла.\n";
    }
    
    buffer << "\n\nНажмите любую клавишу..." << std::flush;
    
    ui_.draw_menu("Просмотр файла", {MenuItem{path}}, 0, 0);

    std::cout << buffer.str();
    
    while(ui_.get_key_input() == -1) {
        std::this_thread::sleep_for(10ms);
    }
}

void FileSystemNavigator::show_error(const std::string& message) const {
    ui_.draw_menu("Ошибка", {MenuItem{message}}, 0, 0);
    std::this_thread::sleep_for(2s);
}
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
        if (auto it = menu_cache_.find(path); it != menu_cache_.end()) {
            lru_order_.remove(path);
            lru_order_.push_front(path);
            return it->second;
        }
        
        auto items = create_menu_items(path);
        update_cache(path, std::move(items));
        return menu_cache_[path];
    }

    std::vector<MenuItem> FileSystemNavigator::create_menu_items(const std::string& path) const {
        std::vector<MenuItem> items;
        
        try {
            size_t entry_count = 0;
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                ++entry_count;
            }
    
            items.reserve(entry_count + (path != "/" ? 1 : 0));
    
            if (path != "/") {
                items.emplace_back(
                    "..", 
                    "filesystem " + std::filesystem::path(path).parent_path().string()
                );
            }
    
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                MenuItem item;
                item.name = entry.path().filename().string();
    
                if (entry.is_directory()) {
                    item.function = "filesystem " + entry.path().string();
                } else {
                    item.function = "view_file " + entry.path().string();
                }
    
                items.emplace_back(std::move(item));
            }
    
            std::sort(items.begin(), items.end(), [](const MenuItem& a, const MenuItem& b) {
                bool a_is_dir = a.function.find("filesystem ") == 0;
                bool b_is_dir = b.function.find("filesystem ") == 0;
                return a_is_dir > b_is_dir || (a_is_dir == b_is_dir && a.name < b.name);
            });
    
            if (items.empty()) {
                items.emplace_back("Директория пуста");
            }
    
        } catch (...) {
            items.clear();
            items.emplace_back("Ошибка доступа");
        }
        
        return items;
    }


void FileSystemNavigator::show_content(const std::string& path) const {
    ui_.needs_initial_draw_ = true; // reset
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

void FileSystemNavigator::update_cache(const std::string& path, std::vector<MenuItem>&& items) const {
    if (menu_cache_.size() >= max_cache_size_) {
        const auto& lru = lru_order_.back();
        menu_cache_.erase(lru);
        lru_order_.pop_back();
    }
    
    menu_cache_.emplace(path, std::move(items));
    lru_order_.push_front(path);
}
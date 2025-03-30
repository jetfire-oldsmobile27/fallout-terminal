#pragma once
#include "terminal_ui.h"
#include "audio_manager.h"
#include <filesystem>
#include <vector>

class FileSystemNavigator
{
public:
    FileSystemNavigator(TerminalUI &ui, AudioManager &audio);
    std::vector<MenuItem> get_menu_items(const std::string &path) const;
    void show_content(const std::string &path) const;

private:
    TerminalUI &ui_;
    AudioManager &audio_;

    void show_error(const std::string &message) const;
    std::vector<MenuItem> create_menu_items(const std::string& path) const;
    void update_cache(const std::string& path, std::vector<MenuItem>&& items) const;

    mutable std::unordered_map<std::string, std::vector<MenuItem>> menu_cache_;
    mutable std::list<std::string> lru_order_;
    mutable size_t max_cache_size_ = 50;
};
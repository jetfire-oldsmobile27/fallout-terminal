#pragma once
#include "terminal_ui.h"
#include "audio_manager.h"
#include <filesystem>
#include <vector>

class FileSystemNavigator {
public:
    FileSystemNavigator(TerminalUI& ui, AudioManager& audio);
    void navigate(const std::string& path);

private:
    TerminalUI& ui_;
    AudioManager& audio_;
    
    std::vector<MenuItem> create_menu_items(const std::string& path) const;
    void show_content(const std::string& path) const;
    void show_error(const std::string& message) const;
};
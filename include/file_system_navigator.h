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
};
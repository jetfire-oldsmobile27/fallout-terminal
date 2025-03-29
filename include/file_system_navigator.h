#pragma once
#include "terminal_ui.h"
#include "audio_manager.h"
#include <filesystem>

class FileSystemNavigator {
public:
    FileSystemNavigator(TerminalUI& ui, AudioManager& audio);
    void navigate(const std::string& path);

private:
    TerminalUI& ui;
    AudioManager& audio;
    
    void showContent(const std::string& path) const;
    void showError(const std::string& message) const;
};
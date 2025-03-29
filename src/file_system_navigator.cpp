#include "file_system_navigator.h"
#include <fstream>

FileSystemNavigator::FileSystemNavigator(TerminalUI& ui, AudioManager& audio) 
    : ui(ui), audio(audio) {}

void FileSystemNavigator::navigate(const std::string& path) {
    std::vector<std::filesystem::directory_entry> entries;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path))
            entries.push_back(entry);
    } catch (...) {
        showError("Ошибка доступа к директории");
        return;
    }

    size_t selected = 0;
    while (true) {
        ui.clearScreen();
        std::cout << Constants::GREEN << "Файловая система: " << path << "\n\n";
        
        for (size_t i = 0; i < entries.size(); ++i) {
            std::cout << (i == selected ? "> " : "  ") 
                     << entries[i].path().filename().string() << "\n";
        }

        switch(ui.getKeyInput()) {
            case 'U': if (selected > 0) { selected--; audio.playTick(); } break;
            case 'D': if (selected < entries.size()-1) { selected++; audio.playTick(); } break;
            case '\n': 
                if (entries[selected].is_directory()) {
                    navigate(entries[selected].path().string());
                } else {
                    showContent(entries[selected].path().string());
                }
                break;
            case 127: return;
        }
    }
}

void FileSystemNavigator::showContent(const std::string& path) const {
    std::ifstream file(path);
    if (!file) {
        showError("Ошибка открытия файла");
        return;
    }
    
    ui.clearScreen();
    std::cout << Constants::GREEN << "Содержимое файла:\n\n" << Constants::RESET;
    std::cout << file.rdbuf() << "\n\nНажмите любую клавишу...";
    getchar();
}

void FileSystemNavigator::showError(const std::string& message) const {
    ui.clearScreen();
    std::cerr << Constants::GREEN << "Ошибка: " 
             << Constants::RESET << message << "\n";
    getchar();
}
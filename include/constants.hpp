#pragma once
#include <string>
#include <vector>

namespace Constants {
    // Цвета
    constexpr const char* GREEN       = "\033[32m";
    constexpr const char* YELLOW      = "\033[33m";
    constexpr const char* RESET       = "\033[0m";
    
    // Тексты
    const std::string HEADER          = "=== Терминальный интерфейс ===";
    
    // Тайминги
    constexpr int ANIMATION_DELAY_MS  = 35;
    constexpr int MENU_PADDING        = 4;
    
    // Файлы
    constexpr const char* CONFIG_FILE = "config.json";
}

struct MenuItem {
    std::string name;
    std::string function;
    std::vector<MenuItem> submenu;
};
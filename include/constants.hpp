#pragma once
#include <string>
#include <vector>

namespace Constants {
    constexpr const char* GREEN       = "\033[32m";
    constexpr const char* YELLOW      = "\033[33m";
    constexpr const char* RESET       = "\033[0m";
    
    const std::string HEADER          = "=== Терминальный интерфейс ===";
    
    constexpr int ANIMATION_DELAY_MS  = 35;
    constexpr int MENU_PADDING        = 4;
    
    constexpr const char* CONFIG_FILE = "config.json";
}

struct MenuItem {
    MenuItem() = default;
    MenuItem(std::string n, std::string f = "") 
        : name(std::move(n)), function(std::move(f)) {}
    
    std::string name;
    std::string function;
    std::vector<MenuItem> submenu;
};
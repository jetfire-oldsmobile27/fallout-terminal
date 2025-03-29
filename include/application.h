#pragma once
#include "terminal_ui.h"
#include "audio_manager.h"
#include "menu_builder.h"
#include "file_system_navigator.h"
#include <stack>
#include <fstream>
#include <atomic>

class Application {
public:
    Application();
    void run();

private:
    TerminalUI ui;
    AudioManager audio;
    FileSystemNavigator fsNav;
    std::stack<std::pair<std::string, std::vector<MenuItem>>> menuStack;
    MenuItem rootMenu;
    size_t selectedIndex = 0;
    bool running = true;

    void loadConfig();
    void generateDefaultConfig() const;
    void mainLoop();
    void processInput();
    void moveUp();
    void moveDown();
    void selectItem();
    void goBack();
    void handleFunction(const std::string& func);
    
    const std::string& currentTitle() const;
    const std::vector<MenuItem>& currentItems() const;
    std::atomic<bool> needsRedraw{true};
};
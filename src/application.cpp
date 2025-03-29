#include "application.h"
#include <boost/json/src.hpp>
#include <filesystem>

Application::Application() : fsNav(ui, audio) {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    ui.setupTerminal();  
}

void Application::run() {
    loadConfig();
    mainLoop();
}

void Application::loadConfig() {
    std::ifstream ifs(Constants::CONFIG_FILE);
    if (!ifs) {
        generateDefaultConfig();
        ifs.open(Constants::CONFIG_FILE);
    }
    
    std::string configStr((std::istreambuf_iterator<char>(ifs)),
                         std::istreambuf_iterator<char>());
    try {
        rootMenu = MenuBuilder::buildFromJSON(json::parse(configStr));
    } catch (std::exception& e) {
        std::cerr << "Config error: " << e.what() << "\n";
        exit(1);
    }
}

void Application::generateDefaultConfig() const {
    json::object root;
    root["title"] = "Меню";
    json::array items;

    // Настройки
    json::object settings;
    settings["name"] = "Настройки";
    settings["function"] = "settings";
    items.push_back(settings);

    // Дверь
    json::object openDoorConfirm;
    openDoorConfirm["name"] = "Точно открыть?";
    openDoorConfirm["function"] = "openDoor";
    
    json::array confirmItems;
    confirmItems.push_back(openDoorConfirm);
    
    json::object openDoor;
    openDoor["name"] = "Открыть";
    openDoor["submenu"] = confirmItems;
    
    json::array doorSubmenu;
    doorSubmenu.push_back(openDoor);
    
    json::object door;
    door["name"] = "Дверь";
    door["submenu"] = doorSubmenu;
    items.push_back(door);

    // Файловая система
    json::object fsNav;
    fsNav["name"] = "Просмотр файлов";
    fsNav["function"] = "filesystem /";
    items.push_back(fsNav);

    // Выход
    json::object exitItem;
    exitItem["name"] = "Выход";
    exitItem["function"] = "exit";
    items.push_back(exitItem);

    root["items"] = items;
    std::ofstream ofs(Constants::CONFIG_FILE);
    ofs << json::serialize(root);
}

void Application::mainLoop() {
    menuStack.push({rootMenu.name, rootMenu.submenu});
    
    while (running) {
        ui.drawMenu(currentTitle(), currentItems(), selectedIndex);
        processInput();
    }
}

void Application::processInput() {
    switch(ui.getKeyInput()) {
        case 'U': moveUp(); break;
        case 'D': moveDown(); break;
        case '\n': selectItem(); break;
        case 127: goBack(); break;
    }
}

void Application::moveUp() {
    if (selectedIndex > 0) {
        selectedIndex--;
        audio.playTick();
    }
}

void Application::moveDown() {
    if (selectedIndex + 1 < currentItems().size()) {
        selectedIndex++;
        audio.playTick();
    }
}

void Application::selectItem() {
    audio.playSelect();
    const auto& item = currentItems()[selectedIndex];
    
    if (!item.submenu.empty()) {
        menuStack.push({currentTitle(), currentItems()});
        menuStack.top().second = item.submenu;
        selectedIndex = 0;
    } else if (!item.function.empty()) {
        if (item.function == "exit") running = false;
        else handleFunction(item.function);
    }
}

void Application::goBack() {
    audio.playBack();
    if (menuStack.size() > 1) {
        menuStack.pop();
        selectedIndex = 0;
    }
}

void Application::handleFunction(const std::string& func) {
    if (func.find("filesystem ") == 0) {
        fsNav.navigate(func.substr(11));
    } else if (func.find("cmd ") == 0) {
        ui.clearScreen();
        std::string command = func.substr(4);
        
        #ifdef WINDOWS
            command = "cmd /c \"" + command + "\"";
        #else
            command = "sh -c \"" + command + "\"";
        #endif
        
        int result = system(command.c_str());
        std::cout << "\nКоманда выполнена с кодом: " << result 
                 << "\nНажмите любую клавишу...";
        getchar();
    } else if (func == "exit") {
        running = false;
    } else {
        // Обработка других функций
    }
}

const std::string& Application::currentTitle() const { 
    return menuStack.top().first; 
}

const std::vector<MenuItem>& Application::currentItems() const { 
    return menuStack.top().second; 
}
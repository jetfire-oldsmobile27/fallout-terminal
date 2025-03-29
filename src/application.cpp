#include "application.h"
#include <iostream> 
#include <boost/json/src.hpp>
#include <filesystem>
#include <thread>
#include <fstream>

Application::Application() 
    : file_system_navigator_(ui_, audio_),
      last_redraw_(std::chrono::steady_clock::now()) 
{
    setlocale(LC_ALL, "ru_RU.UTF-8");
    load_config();
    
    menu_stack_.push({"Главное меню", root_menu_.submenu});
}

void Application::main_loop() {
    using namespace std::chrono;
    auto last_frame = steady_clock::now();
    
    while (is_running_) {
        const auto now = steady_clock::now();
        const auto delta = now - last_frame;
        
        if (delta > 16ms) {
            if (needs_redraw_) {
                ui_.draw_menu(current_title(), current_items(), selected_index_);
                needs_redraw_ = false;
            }
            last_frame = now;
        }
        
        process_input();
        std::this_thread::sleep_for(1ms);
    }
}

void Application::run() {
    main_loop();
}

void Application::load_config() {
    std::ifstream config_file(Constants::CONFIG_FILE);
    if (!config_file) {
        generate_default_config();
        config_file.open(Constants::CONFIG_FILE);
    }
    
    std::string config_content((std::istreambuf_iterator<char>(config_file)),
                         std::istreambuf_iterator<char>());
    try {
        root_menu_ = MenuBuilder::build_from_json(json::parse(config_content));
    } catch (std::exception& error) {
        std::cerr << "Config error: " << error.what() << "\n";
        exit(1);
    }
}

void Application::generate_default_config() const {
    json::object root;
    root["name"] = "Меню";
    json::array items;

    // Настройки
    json::object settings;
    settings["name"] = "Настройки";
    settings["function"] = "settings";
    items.push_back(settings);

    // Файловая система
    json::object file_system;
    file_system["name"] = "Просмотр файлов";
    file_system["function"] = "filesystem /";
    items.push_back(file_system);

    // Выход
    json::object exit_item;
    exit_item["name"] = "Выход";
    exit_item["function"] = "exit";
    items.push_back(exit_item);

    root["items"] = items;
    std::ofstream output_file(Constants::CONFIG_FILE);
    output_file << json::serialize(root);
}

void Application::process_input() {
    switch(ui_.get_key_input()) {
        case 'U': 
            if (selected_index_ > 0) {
                selected_index_--;
                audio_.play_tick_async();
                needs_redraw_ = true;
            }
            break;
            
        case 'D':
            if (selected_index_ + 1 < current_items().size()) {
                selected_index_++;
                audio_.play_tick_async();
                needs_redraw_ = true;
            }
            break;
            
        case '\n': {
            const auto& current_item = current_items()[selected_index_];
            if (!current_item.submenu.empty()) {
                menu_stack_.push({current_title(), current_items()});
                menu_stack_.top().second = current_item.submenu;
                selected_index_ = 0;
                needs_redraw_ = true;
            } else if (!current_item.function.empty()) {
                if (current_item.function == "exit") {
                    is_running_ = false;
                } else {
                    handle_function(current_item.function);
                    needs_redraw_ = true;
                }
            }
            break;
        }
            
        case 127: // Backspace
            if (menu_stack_.size() > 1) {
                menu_stack_.pop();
                selected_index_ = 0;
                audio_.play_back_async();
                needs_redraw_ = true;
            }
            break;
    }
}

void Application::handle_function(const std::string& function_name) {
    if (function_name.find("filesystem ") == 0) {
        file_system_navigator_.navigate(function_name.substr(11));
    } else if (function_name.find("cmd ") == 0) {
        ui_.clear_screen();
        std::string command = function_name.substr(4);
        
        #ifdef WINDOWS
            command = "cmd /c \"" + command + "\"";
        #else
            command = "sh -c \"" + command + "\"";
        #endif
        
        int result = system(command.c_str());
        std::cout << "\nКоманда выполнена с кодом: " << result 
                 << "\nНажмите любую клавишу...";
        getchar();
    }
}

const std::string& Application::current_title() const { 
    static std::string default_title = "Главное меню";
    return menu_stack_.empty() ? default_title : menu_stack_.top().first;
}

const std::vector<MenuItem>& Application::current_items() const { 
    static std::vector<MenuItem> empty_items;
    return menu_stack_.empty() ? empty_items : menu_stack_.top().second;
}
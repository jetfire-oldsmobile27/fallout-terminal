#pragma once
#include "terminal_ui.h"
#include "audio_manager.h"
#include "menu_builder.h"
#include "file_system_navigator.h"
#include <atomic>
#include <chrono>
#include <stack>

#ifdef _WIN32
#define APP_EXPORT __declspec(dllexport)
#else
#define APP_EXPORT
#endif

class APP_EXPORT Application {
public:
    Application();
    void run();

private:
    TerminalUI ui_;
    AudioManager audio_;
    FileSystemNavigator file_system_navigator_;
    std::stack<std::pair<std::string, std::vector<MenuItem>>> menu_stack_;
    MenuItem root_menu_;
    size_t selected_index_ = 0;
    std::atomic<bool> is_running_{true};
    std::atomic<bool> needs_redraw_{true};
    std::chrono::steady_clock::time_point last_redraw_;
    size_t prev_selected_index_ = 0;

    void load_config();
    void generate_default_config() const;
    void main_loop();
    void process_input();
    void handle_function(const std::string& function_name);
    
    const std::string& current_title() const;
    const std::vector<MenuItem>& current_items() const;
};
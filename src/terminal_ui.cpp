#include "terminal_ui.h"
#include <chrono>
#include <thread>
#include <iostream>

TerminalUI::TerminalUI() {
#ifdef WINDOWS
    hconsole_ = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(hconsole_, &original_mode_);
#else
    tcgetattr(STDIN_FILENO, &original_settings_);
#endif
    setup_terminal();
}

TerminalUI::~TerminalUI() {
#ifdef WINDOWS
    SetConsoleMode(hconsole_, original_mode_);
#else
    tcsetattr(STDIN_FILENO, TCSANOW, &original_settings_);
#endif
}

void TerminalUI::setup_terminal() const {
#ifdef WINDOWS
    DWORD new_mode = original_mode_;
    new_mode &= ~ENABLE_ECHO_INPUT;
    SetConsoleMode(hconsole_, new_mode);
#else
    termios new_settings = original_settings_;
    new_settings.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
#endif
}

void TerminalUI::clear_screen() const {
    std::cout << "\033[2J\033[1;1H" << std::flush;
}

void TerminalUI::draw_menu(const std::string& title,
                         const std::vector<MenuItem>& items,
                         size_t selected) const {
    
    if(items.empty()) return;
    selected = std::min(selected, items.size() - 1);
    std::stringstream buffer;
    buffer << "\033[2J\033[1;1H";
    
    buffer << Constants::YELLOW
          << std::string(get_terminal_width(), '=')
          << Constants::RESET << "\n";
    
    const int header_pad = (get_terminal_width() - Constants::HEADER.size()) / 2;
    buffer << std::string(header_pad, ' ')
          << Constants::GREEN << Constants::HEADER << Constants::RESET << "\n";
    
    buffer << Constants::YELLOW
          << std::string(get_terminal_width(), '=')
          << Constants::RESET << "\n\n";
    
    buffer << Constants::GREEN << "> " << title << "\n\n";
    
    for (size_t i = 0; i < items.size(); ++i) {
        buffer << (i == selected ? "  > " : "    ")
              << items[i].name << "\n";
    }
    
    buffer << "\n" << Constants::YELLOW
          << std::string(get_terminal_width(), '=')
          << Constants::RESET << "\n";
    
    std::cout << buffer.str() << std::flush;
}

int TerminalUI::get_terminal_width() const {
#ifdef WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hconsole_, &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    return size.ws_col;
#endif
}

int TerminalUI::get_key_input() const {
#ifdef WINDOWS
    if (_kbhit()) {
        int ch = _getch();
        if (ch == 0xE0) {
            switch (_getch()) {
                case 72: return 'U';
                case 80: return 'D';
            }
        }
        return ch;
    }
#else
    fd_set set;
    timeval timeout{0, 0};
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    
    if (select(1, &set, NULL, NULL, &timeout) > 0) {
        int ch = getchar();
        if (ch == 27) {
            getchar(); // Skip '['
            switch(getchar()) {
                case 'A': return 'U';
                case 'B': return 'D';
            }
        }
        return ch;
    }
#endif
    return -1;
}
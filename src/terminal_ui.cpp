#include "terminal_ui.h"
#include <chrono>
#include <thread>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

TerminalUI::TerminalUI()
{
#ifdef _WIN32
    hconsole_ = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD mode = 0;
    GetConsoleMode(hconsole_, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hconsole_, mode);

    GetConsoleMode(hconsole_, &original_mode_);
#else
    tcgetattr(STDIN_FILENO, &original_settings_);
#endif
    setup_terminal();
}

TerminalUI::~TerminalUI()
{
#ifdef _WIN32
    SetConsoleMode(hconsole_, original_mode_);
#else
    tcsetattr(STDIN_FILENO, TCSANOW, &original_settings_);
#endif
}

void TerminalUI::setup_terminal() const
{
#ifdef _WIN32
    DWORD new_mode = original_mode_;
    new_mode &= ~ENABLE_ECHO_INPUT;
    SetConsoleMode(hconsole_, new_mode);
#else
    termios new_settings = original_settings_;
    new_settings.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
#endif
}

void TerminalUI::clear_screen() const
{
    std::cout << "\033[2J\033[1;1H" << std::flush;
}

void TerminalUI::draw_menu(const std::string &title,
    const std::vector<MenuItem> &items,
    size_t selected,
    size_t prev_selected) const {

const int current_width = get_terminal_width();
if (last_term_width_ != current_width) {
needs_initial_draw_ = true;
last_term_width_ = current_width;
}

std::stringstream buffer;
const int& term_width = current_width;
const size_t menu_lines = items.size() + 6; 

if (needs_initial_draw_ || items.size() != last_item_count_) {
buffer << "\033[2J\033[H"; 
needs_initial_draw_ = false;
last_item_count_ = items.size();
} else {
buffer << "\033[H"; 
for (size_t i = 0; i < menu_lines; ++i) {
buffer << "\033[K\n"; 
}
buffer << "\033[" << menu_lines << "A"; 
} 

buffer << Constants::YELLOW << std::string(term_width, '=') << Constants::RESET << "\n"
<< std::string((term_width - Constants::HEADER.size()) / 2, ' ')
<< Constants::GREEN << Constants::HEADER << Constants::RESET << "\n"
<< Constants::YELLOW << std::string(term_width, '=') << Constants::RESET << "\n\n"
<< Constants::GREEN << "> " << title << "\n\n";

for (size_t i = 0; i < items.size(); ++i) {
buffer << (i == selected ? "  > " : "    ")
<< items[i].name << "\033[K\n";
}

buffer << "\n"
<< Constants::YELLOW
<< std::string(term_width, '=') << Constants::RESET << "\n";

std::cout << buffer.str() << std::flush;
}

int TerminalUI::get_terminal_width() const
{
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hconsole_, &csbi);
    return std::max(80, csbi.srWindow.Right - csbi.srWindow.Left + 1);
#else
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    return size.ws_col;
#endif
}

int TerminalUI::get_key_input() const
{
#ifdef _WIN32
    if (_kbhit())
    {
        int ch = _getch();
        if (ch == 0xE0 || ch == 0x00)
        {
            switch (_getch())
            {
            case 72:
                return 'U';
            case 80:
                return 'D';
            case 75:
                return 'L';
            case 77:
                return 'R';
            }
        }
        if (ch == 13)
            return '\n';
        if (ch == 8)
            return 0x7F;
        return ch;
    }
#else
    fd_set set;
    timeval timeout{0, 0};
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);

    if (select(1, &set, NULL, NULL, &timeout) > 0)
    {
        int ch = getchar();
        if (ch == 27)
        {
            getchar();
            switch (getchar())
            {
            case 'A':
                return 'U';
            case 'B':
                return 'D';
            }
        }
        return ch;
    }
#endif
    return -1;
}
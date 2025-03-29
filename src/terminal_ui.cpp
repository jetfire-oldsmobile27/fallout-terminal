#include "terminal_ui.h"
#include "audio_manager.h"
#include <iostream>
#include <thread>

TerminalUI::TerminalUI() {
#ifdef WINDOWS
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(hConsole, &oldMode);
    SetConsoleMode(hConsole, oldMode & ~ENABLE_ECHO_INPUT);
#else
    tcgetattr(STDIN_FILENO, &originalSettings);
    termios newSettings = originalSettings;
    newSettings.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
#endif
}

TerminalUI::~TerminalUI() {
#ifdef WINDOWS
    SetConsoleMode(hConsole, oldMode);
#else
    tcsetattr(STDIN_FILENO, TCSANOW, &originalSettings);
#endif
}

int TerminalUI::getTerminalWidth() const {
    #ifdef WINDOWS
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        return csbi.dwSize.X;
    #else
        winsize size{};
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
        return size.ws_col > 0 ? size.ws_col : 80;
    #endif
    }
    
    void TerminalUI::drawMenu(const std::string& title, 
                            const std::vector<MenuItem>& items, 
                            size_t selected) const {
        clearScreen();
        printHeader();
        
        // Вывод заголовка
        std::cout << Constants::GREEN << "> " << title << "\n\n" << Constants::RESET;
        
        // Вывод пунктов меню
        for(size_t i = 0; i < items.size(); ++i) {
            std::cout << (i == selected ? "  > " : "    ")
                     << items[i].name << "\n";
        }
        
        printBorder();
    }

    void TerminalUI::setupTerminal() {
        // Реализация для Windows
        #ifdef WINDOWS
        GetConsoleMode(hConsole, &oldMode);
        SetConsoleMode(hConsole, oldMode & ~ENABLE_ECHO_INPUT);
        
        // Реализация для Linux
        #else
        termios newSettings = originalSettings;
        newSettings.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
        #endif
    }

void TerminalUI::printBorder() const {
    #ifdef WINDOWS
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        std::cout << std::string(csbi.dwSize.X, '=') << '\n';
    #else
        std::cout << Constants::YELLOW 
                 << std::string(getTerminalWidth(), '=') 
                 << Constants::RESET << '\n';
    #endif
    }
    
    void TerminalUI::printHeader() const {
    #ifdef WINDOWS
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        int pad = (csbi.dwSize.X - Constants::HEADER.size()) / 2;
        std::cout << std::string(pad, ' ') << Constants::HEADER << '\n';
    #else
        int pad = (getTerminalWidth() - Constants::HEADER.size()) / 2;
        std::cout << std::string(pad, ' ') << Constants::GREEN 
                 << Constants::HEADER << Constants::RESET << '\n';
    #endif
    }
    
    int TerminalUI::getKeyInput() const {
    #ifdef WINDOWS
        if(_kbhit()) {
            int ch = _getch();
            if(ch == 0xE0) {
                switch(_getch()) {
                    case 72: return 'U';
                    case 80: return 'D';
                }
            }
            return ch;
        }
        return -1;
    #else
        fd_set set;
        struct timeval timeout;
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
    
        if (select(1, &set, NULL, NULL, &timeout) > 0) {
            int ch = getchar();
            if (ch == 27) {
                getchar(); // Пропускаем '['
                switch(getchar()) {
                    case 'A': return 'U';
                    case 'B': return 'D';
                }
            }
            return ch;
        }
        return -1;
    #endif
    }

void TerminalUI::clearScreen() const {
#ifdef WINDOWS
    system("cls");
#else
    std::cout << "\033[2J\033[H";
#endif
}
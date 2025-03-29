#pragma once
#include "constants.hpp"
#include <string>
#include <vector>
#include <sstream>

#ifdef WINDOWS
#include <conio.h>
#else
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/select.h>
#endif

class TerminalUI {
public:
    TerminalUI();
    ~TerminalUI();
    
    void draw_menu(const std::string& title, 
                 const std::vector<MenuItem>& items,
                 size_t selected) const;
    int get_key_input() const;
    void clear_screen() const;

private:
    void setup_terminal() const;
    int get_terminal_width() const;
    
#ifdef WINDOWS
    HANDLE hconsole_;
    DWORD original_mode_;
#else
    termios original_settings_;
#endif
};
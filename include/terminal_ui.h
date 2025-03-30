#pragma once
#include "constants.hpp"
#include <string>
#include <vector>
#include <sstream>

#if defined(_WIN32) || defined(_WIN64)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <conio.h>
#define STDIN_FILENO 0 
#define STDOUT_FILENO 1 
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
                 size_t selected,
                 size_t prev_selected) const;
    int get_key_input() const;
    void clear_screen() const;

    mutable bool needs_initial_draw_ = true;
private:
    void setup_terminal() const;
    int get_terminal_width() const;
    
#if defined(_WIN32) || defined(_WIN64)
    HANDLE hconsole_;
    DWORD original_mode_;
#else
    termios original_settings_;
#endif
};
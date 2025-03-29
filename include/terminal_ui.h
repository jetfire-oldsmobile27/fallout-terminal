#pragma once
#include "constants.hpp"
#include <vector>
#include <string>

#ifdef WINDOWS
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#endif

class AudioManager;

class TerminalUI {
public:
    TerminalUI();
    ~TerminalUI();
    
    void setupTerminal();  // Перемещено в public секцию
    void animatePrint(const std::string& text, const AudioManager& audio) const;
    void drawMenu(const std::string& title, 
                const std::vector<MenuItem>& items, 
                size_t selected) const;
    int getKeyInput() const;
    int getTerminalWidth() const;
    void clearScreen() const;
    void printBorder() const;  // Добавлено
    void printHeader() const;  // Добавлено

private:
#ifdef WINDOWS
    HANDLE hConsole;
    DWORD oldMode;
#else
    termios originalSettings;
#endif
};
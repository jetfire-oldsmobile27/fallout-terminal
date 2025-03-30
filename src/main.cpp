#include "application.h"
#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char* argv[]) {
    (void)argc; (void)argv; // no warnings(!)
    #ifdef _WIN32
    SetConsoleOutputCP(65001); // UTF-8
    SetConsoleCP(65001);
    #endif
    Application app;
    app.run();
    return 0;
}
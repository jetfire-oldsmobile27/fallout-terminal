#include "application.h"
#include "verifier.hpp"
#include <iostream>
#include <fstream>
#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char* argv[]) {
    try {
        const std::vector<unsigned char> public_key = {}; 
        
        std::ifstream self_exe("/proc/self/exe", std::ios::binary);
        std::vector<unsigned char> exe_data(
            (std::istreambuf_iterator<char>(self_exe)),
            std::istreambuf_iterator<char>()
        );
        
        const size_t signature_size = &__stop_signature - &__start_signature;
        const std::vector<unsigned char> signature(
            reinterpret_cast<const unsigned char*>(&__start_signature),
            reinterpret_cast<const unsigned char*>(&__stop_signature)
        );
        
        SignatureVerifier verifier(public_key);
        if (verifier.Verify(exe_data, signature)) {
            std::cout << "Integrity check passed!\n";
            return 0;
        } else {
            std::cerr << "BINARY TAMPERED!\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }


    (void)argc; (void)argv; // no warnings(!)
    #ifdef _WIN32
    SetConsoleOutputCP(65001); // UTF-8
    SetConsoleCP(65001);
    #endif
    Application app;
    app.run();
    return 0;
}
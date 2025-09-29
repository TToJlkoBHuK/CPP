#include "Game.h"
#include <iostream>

int main() {
    try {
        Game game;
        game.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
#ifdef _WIN32
        system("pause");
#endif
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "An unknown error occurred." << std::endl;
#ifdef _WIN32
        system("pause");
#endif
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
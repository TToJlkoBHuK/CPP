// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#pragma once

#include <SFML/Graphics.hpp>
#include "Board.h"
#include "Config.h"
#include <memory>

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    sf::RenderWindow window;
    sf::Texture gemTextureSheet;
    std::unique_ptr<Board> board;
    void loadResources();

};
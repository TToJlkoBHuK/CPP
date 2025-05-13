#include "Game.h"
#include "Config.h"
#include <stdexcept>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Mouse.hpp>

Game::Game()
    : window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "SFML Gems Game", sf::Style::Default)
{
    window.setFramerateLimit(60);
    loadResources();
    auto newBoard = std::make_unique<Board>(GRID_WIDTH, GRID_HEIGHT, gemTextureSheet);
    board = std::move(newBoard);
}

void Game::loadResources() {
    if (!gemTextureSheet.loadFromFile("gems.png")) {
        throw std::runtime_error("Failed to load gems.png texture!");
    }
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    while (const auto event = window.pollEvent()) {
        event->visit([this](auto&& e) {
            using T = std::decay_t<decltype(e)>;

            if constexpr (std::is_same_v<T, sf::Event::Closed>) {
                window.close();
            }
            else if constexpr (std::is_same_v<T, sf::Event::MouseButtonPressed>) {
                if (e.button == sf::Mouse::Button::Left) {
                    board->handleMouseClick({ e.position.x, e.position.y });
                }
            }
            });
    }
}

void Game::update(sf::Time deltaTime) {
    while (board->update()) {}
}

void Game::render() {
    window.clear(sf::Color::Black);
    if (board) {
        board->draw(window);
    }
    window.display();
}
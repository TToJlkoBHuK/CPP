// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#ifndef PADDLE_H
#define PADDLE_H

#include <SFML/Graphics.hpp>
#include "constants.h"

struct Paddle {
    sf::RectangleShape shape;
    float speed;
    bool isStickyActive;
    float currentWidth;

    Paddle(float width = PADDLE_START_WIDTH, float height = PADDLE_HEIGHT, sf::Color color = sf::Color::Cyan);
    void update(sf::Time dt);
    void draw(sf::RenderWindow& window);
    void changeSize(float factor, bool absolute = false, float value = 0.f);
    void reset();
    sf::FloatRect getBounds() const;
};

#endif // PADDLE_H
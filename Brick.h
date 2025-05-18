#ifndef BRICK_H
#define BRICK_H

#include <SFML/Graphics.hpp>
#include "constants.h"

struct Brick {
    sf::RectangleShape shape;
    BlockType type;
    int health;
    bool hasBonus;
    BonusType bonusToDrop;
    bool isVisible;
    sf::Color originalColor;

    Brick(float x, float y, float width, float height, BlockType type, sf::Color color);
    bool hit();
    void draw(sf::RenderWindow& window) const;
    sf::FloatRect getBounds() const;
};

#endif // BRICK_H
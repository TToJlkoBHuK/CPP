#ifndef BONUS_H
#define BONUS_H

#include <SFML/Graphics.hpp>
#include "constants.h" 

struct Bonus {
    sf::RectangleShape shape;
    BonusType type;
    float fallSpeed;
    bool isActive;


    Bonus(float x, float y, float size, BonusType type, sf::Color color);
    void update(sf::Time dt);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
};

#endif // BONUS_H
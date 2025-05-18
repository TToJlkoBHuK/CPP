#include "Bonus.h"

Bonus::Bonus(float x, float y, float size, BonusType bt, sf::Color color)
    : type(bt), fallSpeed(BONUS_FALL_SPEED), isActive(true) {
    shape.setPosition(sf::Vector2f(x, y));
    shape.setSize(sf::Vector2f(size, size));
    shape.setFillColor(color);
}

void Bonus::update(sf::Time dt) {
    if (isActive) {
        shape.move(sf::Vector2f(0.f, fallSpeed * dt.asSeconds()));
        if (shape.getPosition().y > SCREEN_HEIGHT) {
            isActive = false;
        }
    }
}

void Bonus::draw(sf::RenderWindow& window) {
    if (isActive) {
        window.draw(shape);
    }
}

sf::FloatRect Bonus::getBounds() const {
    return shape.getGlobalBounds();
}
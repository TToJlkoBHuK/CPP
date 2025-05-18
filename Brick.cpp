#include "Brick.h"
#include <cstdlib>

Brick::Brick(float x, float y, float width, float height, BlockType t, sf::Color color)
    : type(t), hasBonus(false), bonusToDrop(BonusType::NONE), isVisible(true) {
    shape.setPosition(sf::Vector2f(x, y));
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(color);
    shape.setOutlineThickness(1.f);
    shape.setOutlineColor(sf::Color(color.r / 2, color.g / 2, color.b / 2));
    originalColor = color;

    switch (type) {
    case BlockType::NORMAL_1HP: health = 1; break;
    case BlockType::NORMAL_2HP: health = 2; break;
    case BlockType::NORMAL_3HP: health = 3; break;
    case BlockType::INDESTRUCTIBLE: health = -1; break;
    case BlockType::BONUS_DROPPER:
        health = 1;
        hasBonus = true;
        {
            int randomBonus = std::rand() % 7;
            switch (randomBonus) {
            case 0: bonusToDrop = BonusType::PADDLE_WIDE; break;
            case 1: bonusToDrop = BonusType::PADDLE_NARROW; break;
            case 2: bonusToDrop = BonusType::BALL_FAST; break;
            case 3: bonusToDrop = BonusType::BALL_SLOW; break;
            case 4: bonusToDrop = BonusType::STICKY_PADDLE; break;
            case 5: bonusToDrop = BonusType::SAFETY_NET; break;
            case 6: bonusToDrop = BonusType::MULTI_BALL; break;
            default: bonusToDrop = BonusType::PADDLE_WIDE; break;
            }
        }
        break;
    case BlockType::SPEED_UP_BALL: health = 1; break;
    default: health = 1; break;
    }
}

bool Brick::hit() {
    if (type == BlockType::INDESTRUCTIBLE || !isVisible) {
        return false;
    }

    health--;
    if (health > 0) {
        float factor = static_cast<float>(health) / (type == BlockType::NORMAL_2HP ? 2.f : (type == BlockType::NORMAL_3HP ? 3.f : 1.f));
        shape.setFillColor(sf::Color(
            static_cast<uint8_t>(originalColor.r * factor),
            static_cast<uint8_t>(originalColor.g * factor),
            static_cast<uint8_t>(originalColor.b * factor)
        ));
        return false;
    }
    else {
        isVisible = false;
        return true;
    }
}

void Brick::draw(sf::RenderWindow& window) const {
    if (isVisible) {
        window.draw(shape);
    }
}

sf::FloatRect Brick::getBounds() const {
    return shape.getGlobalBounds();
}
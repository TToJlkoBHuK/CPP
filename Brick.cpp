#include "Brick.h"
#include "Game.h"
#include "Ball.h"
#include "Bonus.h"
#include <cstdlib>
Brick::Brick(float x, float y, float width, float height, sf::Color color)
    : isVisible(true), originalColor(color) {
    shape.setPosition(sf::Vector2f(x, y));
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(color);
    shape.setOutlineThickness(1.f);
    shape.setOutlineColor(sf::Color(
        static_cast<uint8_t>(color.r * 0.7f),
        static_cast<uint8_t>(color.g * 0.7f),
        static_cast<uint8_t>(color.b * 0.7f)
    ));
}
void Brick::draw(sf::RenderWindow& window) const {
    if (isVisible) {
        window.draw(shape);
    }
}
sf::FloatRect Brick::getBounds() const {
    return shape.getGlobalBounds();
}
void Brick::updateColorBasedOnHealth(int currentHealth, int maxHealth) {
    if (maxHealth <= 0 || currentHealth <= 0) return;
    float factor = static_cast<float>(currentHealth) / static_cast<float>(maxHealth);
    shape.setFillColor(sf::Color(
        static_cast<uint8_t>(originalColor.r * factor),
        static_cast<uint8_t>(originalColor.g * factor),
        static_cast<uint8_t>(originalColor.b * factor)
    ));
}
NormalBrick::NormalBrick(float x, float y, sf::Color color, int initialHealth)
    : Brick(x, y, BRICK_WIDTH, BRICK_HEIGHT, color), health(initialHealth), maxHealth(initialHealth) {
    if (health <= 0) health = 1;
}

bool NormalBrick::onHit(Game& game, Ball& ballHit) {
    if (!isVisible) return false;

    health--;
    game.addScore(SCORE_BRICK_HIT_NORMAL);

    if (health <= 0) {
        isVisible = false;
        game.addScore(SCORE_BRICK_DESTROYED_NORMAL);
        return true;
    }
    else {
        if (maxHealth > 1) {
            updateColorBasedOnHealth(health, maxHealth);
        }
        return false;
    }
}
IndestructibleBrick::IndestructibleBrick(float x, float y, sf::Color color)
    : Brick(x, y, BRICK_WIDTH, BRICK_HEIGHT, color) {}

bool IndestructibleBrick::onHit(Game& game, Ball& ballHit) {
    if (!isVisible) return false;
    return false;
}
BonusDropperBrick::BonusDropperBrick(float x, float y, sf::Color color, int initialHealth)
    : Brick(x, y, BRICK_WIDTH, BRICK_HEIGHT, color), health(initialHealth) {
    if (health <= 0) health = 1;
}

bool BonusDropperBrick::onHit(Game& game, Ball& ballHit) {
    if (!isVisible) return false;

    health--;
    game.addScore(SCORE_BRICK_HIT_NORMAL);

    if (health <= 0) {
        isVisible = false;
        game.addScore(SCORE_BRICK_DESTROYED_BONUS);
        return true;
    }
    else {
        if (health < 2) {
            updateColorBasedOnHealth(health, 2);
        }
        return false;
    }
}

std::unique_ptr<Bonus> BonusDropperBrick::createBonus(float x_pos, float y_pos) {
    int randomBonusType = std::rand() % 7;
    switch (randomBonusType) {
    case 0: return std::make_unique<WidenPaddleBonus>(x_pos, y_pos);
    case 1: return std::make_unique<NarrowPaddleBonus>(x_pos, y_pos);
    case 2: return std::make_unique<FastBallBonus>(x_pos, y_pos);
    case 3: return std::make_unique<SlowBallBonus>(x_pos, y_pos);
    case 4: return std::make_unique<StickyPaddleBonus>(x_pos, y_pos);
    case 5: return std::make_unique<SafetyNetBonus>(x_pos, y_pos);
    case 6: return std::make_unique<MultiBallBonus>(x_pos, y_pos);
    default: return std::make_unique<WidenPaddleBonus>(x_pos, y_pos);
    }
}
SpeedUpBrick::SpeedUpBrick(float x, float y, sf::Color color, int initialHealth)
    : Brick(x, y, BRICK_WIDTH, BRICK_HEIGHT, color), health(initialHealth) {
    if (health <= 0) health = 1;
}

bool SpeedUpBrick::onHit(Game& game, Ball& ballHit) {
    if (!isVisible) return false;

    health--;
    game.addScore(SCORE_BRICK_HIT_NORMAL);

    if (health <= 0) {
        isVisible = false;
        ballHit.applySpeedChange(BALL_SPEED_CHANGE_FACTOR);
        game.addScore(SCORE_BRICK_DESTROYED_SPEED);
        return true;
    }
    else {
        return false;
    }
}

#ifndef BRICK_H
#define BRICK_H

#include <SFML/Graphics.hpp>
#include "constants.h"
#include <memory>
class Game;
class Ball;
class Bonus;
class Brick {
public:
    sf::RectangleShape shape;
    bool isVisible;
    sf::Color originalColor;
    Brick(float x, float y, float width, float height, sf::Color color);
    virtual ~Brick() = default;
    virtual bool onHit(Game& game, Ball& ballHit) = 0;
    virtual bool isDestructible() const = 0;
    virtual std::unique_ptr<Bonus> createBonus(float x_pos, float y_pos) {
        return nullptr;
    }
    void draw(sf::RenderWindow& window) const;
    sf::FloatRect getBounds() const;

protected:
    void updateColorBasedOnHealth(int currentHealth, int maxHealth);
};
class NormalBrick : public Brick {
private:
    int health;
    const int maxHealth;
public:
    NormalBrick(float x, float y, sf::Color color, int initialHealth);
    bool onHit(Game& game, Ball& ballHit) override;
    bool isDestructible() const override { return true; }
};
class IndestructibleBrick : public Brick {
public:
    IndestructibleBrick(float x, float y, sf::Color color);
    bool onHit(Game& game, Ball& ballHit) override;
    bool isDestructible() const override { return false; }
};
class BonusDropperBrick : public Brick {
private:
    int health;
public:
    BonusDropperBrick(float x, float y, sf::Color color, int initialHealth = 1);
    bool onHit(Game& game, Ball& ballHit) override;
    bool isDestructible() const override { return true; }
    std::unique_ptr<Bonus> createBonus(float x_pos, float y_pos) override;
};
class SpeedUpBrick : public Brick {
private:
    int health;
public:
    SpeedUpBrick(float x, float y, sf::Color color, int initialHealth = 1);
    bool onHit(Game& game, Ball& ballHit) override;
    bool isDestructible() const override { return true; }
};

#endif //BRICK_H

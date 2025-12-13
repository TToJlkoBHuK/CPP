// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#ifndef BONUS_H
#define BONUS_H

#include <SFML/Graphics.hpp>
#include "constants.h" 
#include <memory>

class Game;

class Bonus {
public:
    sf::RectangleShape shape;
    float fallSpeed;
    bool isActive;

    Bonus(float x, float y, float size, sf::Color color);
    virtual ~Bonus() = default;
    virtual void applyEffect(Game& game) = 0;

    void update(sf::Time dt);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
};

class WidenPaddleBonus : public Bonus {
public:
    WidenPaddleBonus(float x, float y);
    void applyEffect(Game& game) override;
};

class NarrowPaddleBonus : public Bonus {
public:
    NarrowPaddleBonus(float x, float y);
    void applyEffect(Game& game) override;
};

class FastBallBonus : public Bonus {
public:
    FastBallBonus(float x, float y);
    void applyEffect(Game& game) override;
};

class SlowBallBonus : public Bonus {
public:
    SlowBallBonus(float x, float y);
    void applyEffect(Game& game) override;
};

class StickyPaddleBonus : public Bonus {
public:
    StickyPaddleBonus(float x, float y);
    void applyEffect(Game& game) override;
};

class SafetyNetBonus : public Bonus {
public:
    SafetyNetBonus(float x, float y);
    void applyEffect(Game& game) override;
};

class MultiBallBonus : public Bonus {
public:
    MultiBallBonus(float x, float y);
    void applyEffect(Game& game) override;
};


#endif // BONUS_H
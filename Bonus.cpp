// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "Bonus.h"
#include "Game.h"
#include "Paddle.h"
#include "Ball.h"
Bonus::Bonus(float x, float y, float size, sf::Color color)
    : fallSpeed(BONUS_FALL_SPEED), isActive(true) {
    shape.setPosition(sf::Vector2f(x, y));
    shape.setSize(sf::Vector2f(size, size));
    shape.setFillColor(color);
    shape.setOutlineThickness(1.f);
    shape.setOutlineColor(sf::Color::White);
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
WidenPaddleBonus::WidenPaddleBonus(float x, float y)
    : Bonus(x, y, BONUS_SIZE, sf::Color::Green) {}

void WidenPaddleBonus::applyEffect(Game& game) {
    game.getPaddle().changeSize(PADDLE_WIDTH_CHANGE_FACTOR_LARGE);
}
NarrowPaddleBonus::NarrowPaddleBonus(float x, float y)
    : Bonus(x, y, BONUS_SIZE, sf::Color::Red) {}

void NarrowPaddleBonus::applyEffect(Game& game) {
    game.getPaddle().changeSize(PADDLE_WIDTH_CHANGE_FACTOR_SMALL);
}
FastBallBonus::FastBallBonus(float x, float y)
    : Bonus(x, y, BONUS_SIZE, sf::Color(255, 165, 0)) {}

void FastBallBonus::applyEffect(Game& game) {
    for (auto& ball : game.getBalls()) {
        ball.applySpeedChange(BALL_SPEED_CHANGE_FACTOR);
    }
}
SlowBallBonus::SlowBallBonus(float x, float y)
    : Bonus(x, y, BONUS_SIZE, sf::Color::Blue) {}

void SlowBallBonus::applyEffect(Game& game) {
    for (auto& ball : game.getBalls()) {
        ball.applySpeedChange(1.f / BALL_SPEED_CHANGE_FACTOR);
    }
}
StickyPaddleBonus::StickyPaddleBonus(float x, float y)
    : Bonus(x, y, BONUS_SIZE, sf::Color::Cyan) {}

void StickyPaddleBonus::applyEffect(Game& game) {
    game.setPaddleSticky(true);
}
SafetyNetBonus::SafetyNetBonus(float x, float y)
    : Bonus(x, y, BONUS_SIZE, sf::Color::White) {}

void SafetyNetBonus::applyEffect(Game& game) {
    game.setSafetyNetActive(true);
}
MultiBallBonus::MultiBallBonus(float x, float y)
    : Bonus(x, y, BONUS_SIZE, sf::Color(255, 255, 0, 180)) {}

void MultiBallBonus::applyEffect(Game& game) {
    game.spawnAdditionalBall();
}

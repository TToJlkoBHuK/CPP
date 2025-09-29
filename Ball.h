// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#ifndef BALL_H
#define BALL_H

#include <SFML/Graphics.hpp>
#include "constants.h"

struct Paddle;
 
struct Ball {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    bool attachedToPaddle;
    float currentSpeed;

    Ball(float radius = BALL_RADIUS, sf::Color color = sf::Color::Yellow, float speed = BALL_START_SPEED);
    void reset(const sf::Vector2f& paddlePos, const sf::Vector2f& paddleSize);
    void update(sf::Time dt, const sf::RectangleShape& paddleShape, bool isPaddleSticky);
    void draw(sf::RenderWindow& window);
    void applySpeedChange(float factor);
    sf::FloatRect getBounds() const;
};

#endif // BALL_H
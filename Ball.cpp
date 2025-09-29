// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "Ball.h"
#include "Paddle.h"
#include <cmath>    
#include <cstdlib>

Ball::Ball(float radius, sf::Color color, float speed)
    : attachedToPaddle(true), currentSpeed(speed) {
    shape.setRadius(radius);
    shape.setFillColor(color);
    shape.setOrigin(sf::Vector2f(radius, radius));
}

void Ball::reset(const sf::Vector2f& paddlePos, const sf::Vector2f& paddleSize) {
    attachedToPaddle = true;
    shape.setPosition(sf::Vector2f(paddlePos.x + paddleSize.x / 2.f, paddlePos.y - shape.getRadius() - 1.f));
    velocity = sf::Vector2f(0.f, 0.f);
}

void Ball::update(sf::Time dt, const sf::RectangleShape& paddleShape, bool isPaddleSticky) {
    if (attachedToPaddle) {
        shape.setPosition(sf::Vector2f(paddleShape.getPosition().x + paddleShape.getSize().x / 2.f, paddleShape.getPosition().y - shape.getRadius() - 1.f));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) || sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            attachedToPaddle = false;
            float angle = (std::rand() % 60 + 60) * (3.14159f / 180.f);
            velocity.x = currentSpeed * std::cos(angle);
            velocity.y = -currentSpeed * std::sin(angle);
        }
    }
    else {
        shape.move(velocity * dt.asSeconds());
    }
}

void Ball::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

void Ball::applySpeedChange(float factor) {
    currentSpeed *= factor;
    float magnitude = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (magnitude > 0) {
        velocity.x = (velocity.x / magnitude) * currentSpeed;
        velocity.y = (velocity.y / magnitude) * currentSpeed;
    }
    else {
        velocity.y = -currentSpeed;
    }
}

sf::FloatRect Ball::getBounds() const {
    return shape.getGlobalBounds();
}
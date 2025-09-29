// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "Paddle.h"

Paddle::Paddle(float width, float height, sf::Color color)
    : speed(PADDLE_SPEED), isStickyActive(false), currentWidth(width) {
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(color);
    shape.setPosition(sf::Vector2f((SCREEN_WIDTH - width) / 2.f, SCREEN_HEIGHT - height - 10.f));
}

void Paddle::update(sf::Time dt) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        shape.move(sf::Vector2f(-speed * dt.asSeconds(), 0.f));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        shape.move(sf::Vector2f(speed * dt.asSeconds(), 0.f));
    }

    if (shape.getPosition().x < 0.f) {
        shape.setPosition(sf::Vector2f(0.f, shape.getPosition().y));
    }
    if (shape.getPosition().x + shape.getSize().x > SCREEN_WIDTH) {
        shape.setPosition(sf::Vector2f(SCREEN_WIDTH - shape.getSize().x, shape.getPosition().y));
    }
}

void Paddle::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

void Paddle::changeSize(float factor, bool absolute, float value) {
    float oldWidth = currentWidth;
    if (absolute) {
        currentWidth = value;
    }
    else {
        currentWidth *= factor;
    }

    if (currentWidth < PADDLE_MIN_WIDTH) currentWidth = PADDLE_MIN_WIDTH;
    if (currentWidth > SCREEN_WIDTH * 0.8f) currentWidth = SCREEN_WIDTH * 0.8f;

    float currentX = shape.getPosition().x;
    shape.setSize(sf::Vector2f(currentWidth, PADDLE_HEIGHT));
    shape.setPosition(sf::Vector2f(currentX + (oldWidth - currentWidth) / 2.f, shape.getPosition().y));

    if (shape.getPosition().x < 0.f) shape.setPosition(sf::Vector2f(0.f, shape.getPosition().y));
    if (shape.getPosition().x + shape.getSize().x > SCREEN_WIDTH) {
        shape.setPosition(sf::Vector2f(SCREEN_WIDTH - shape.getSize().x, shape.getPosition().y));
    }
}

void Paddle::reset() {
    currentWidth = PADDLE_START_WIDTH;
    shape.setSize(sf::Vector2f(currentWidth, PADDLE_HEIGHT));
    shape.setPosition(sf::Vector2f((SCREEN_WIDTH - currentWidth) / 2.f, SCREEN_HEIGHT - PADDLE_HEIGHT - 10.f));
    isStickyActive = false;
}

sf::FloatRect Paddle::getBounds() const {
    return shape.getGlobalBounds();
}
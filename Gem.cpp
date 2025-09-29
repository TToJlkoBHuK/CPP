#include "Gem.h"
#include "Config.h"
#include <stdexcept>
#include <SFML/Graphics.hpp>
Gem::Gem(std::unique_ptr<GemColorBase> c, int x, int y, sf::Texture& textureSheet)
    : color(std::move(c)), gridPosition(x, y), textureSheetRef(textureSheet), sprite(textureSheet)
{
    if (color && color->getIndex() != -1) {
        updateTextureRect();
    }
    setGridPosition(x, y);
}

void Gem::setGridPosition(int gridX, int gridY) {
    gridPosition.x = gridX;
    gridPosition.y = gridY;
    sprite.setPosition(sf::Vector2f(
        static_cast<float>(gridX * GEM_SIZE),
        static_cast<float>(gridY * GEM_SIZE)
    ));
}

void Gem::updateTextureRect() {
    if (!color || color->getIndex() == -1) return;

    int colorIndex = color->getIndex();
    if (colorIndex < 0 || colorIndex >= NUM_GEM_COLORS) {
        throw std::runtime_error("Invalid GemColor index in updateTextureRect");
    }
    sprite.setTexture(textureSheetRef);
    sprite.setTextureRect(sf::IntRect(
        sf::Vector2i(colorIndex * GEM_SIZE, 0),
        sf::Vector2i(GEM_SIZE, GEM_SIZE)
    ));
}
const GemColorBase* Gem::getColor() const {
    return color.get();
}

void Gem::draw(sf::RenderWindow& window) {
    if (color && color->getIndex() != -1) {
        window.draw(sprite);
    }
}
void Gem::setColor(std::unique_ptr<GemColorBase> newColor) {
    color = std::move(newColor);
    updateTextureRect();
}

sf::Vector2f Gem::getVisualPosition() const {
    return sprite.getPosition();
}

void Gem::setVisualPosition(float x, float y) {
    sprite.setPosition(sf::Vector2f(x, y));
}
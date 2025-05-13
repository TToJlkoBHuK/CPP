#include "Gem.h"
#include "Config.h"
#include <stdexcept>
#include <SFML/Graphics.hpp>

Gem::Gem(GemColor c, int x, int y, sf::Texture& textureSheet)
    : color(c), gridPosition(x, y), textureSheetRef(textureSheet), sprite(textureSheet)
{
    if (color != GemColor::None) {
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
    if (color == GemColor::None) return;

    int colorIndex = static_cast<int>(color);
    if (colorIndex < 0 || colorIndex >= NUM_GEM_COLORS) {
        throw std::runtime_error("Invalid GemColor index in updateTextureRect");
    }
    sprite.setTexture(textureSheetRef);
    sprite.setTextureRect(sf::IntRect(
         sf::Vector2i(colorIndex * GEM_SIZE, 0),    
         sf::Vector2i(GEM_SIZE, GEM_SIZE)           
    ));
}

GemColor Gem::getColor() const {
    return color;
}

void Gem::draw(sf::RenderWindow& window) {
    if (color != GemColor::None) {
        window.draw(sprite);
    }
}

void Gem::setColor(GemColor newColor) {
    color = newColor;
    updateTextureRect();
}
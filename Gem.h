
#pragma once

#include <SFML/Graphics.hpp>
#include "Config.h"

class Gem {
public:
    Gem(GemColor color, int gridX, int gridY, sf::Texture& textureSheet);
    void draw(sf::RenderWindow& window);
    void setGridPosition(int gridX, int gridY);
    GridCoord getGridPosition() const;
    GemColor getColor() const;
    void setColor(GemColor newColor);
    sf::Vector2f getVisualPosition() const;
    void setVisualPosition(float x, float y);

private:
    sf::Sprite sprite;
    GemColor color;
    GridCoord gridPosition;
    sf::Texture& textureSheetRef;
    void updateTextureRect();
};
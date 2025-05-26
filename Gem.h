#pragma once

#include <SFML/Graphics.hpp>
#include "Config.h"
#include <memory>

class Gem {
public:
    Gem(std::unique_ptr<GemColorBase> c, int gridX, int gridY, sf::Texture& textureSheet);
    void draw(sf::RenderWindow& window);
    void setGridPosition(int gridX, int gridY);
    GridCoord getGridPosition() const;
    const GemColorBase* getColor() const;
    void setColor(std::unique_ptr<GemColorBase> newColor);
    sf::Vector2f getVisualPosition() const;
    void setVisualPosition(float x, float y);

private:
    sf::Sprite sprite;
    std::unique_ptr<GemColorBase> color;
    GridCoord gridPosition;
    sf::Texture& textureSheetRef;
    void updateTextureRect();
};
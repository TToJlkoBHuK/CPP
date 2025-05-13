#pragma once

#include <vector>
#include <set>
#include <random>
#include <SFML/Graphics.hpp>
#include "Gem.h"
#include "Config.h"

class Board {
public:
    Board(int width, int height, sf::Texture& textureSheet);
    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;
    void draw(sf::RenderWindow& window);
    bool update();
    bool handleMouseClick(sf::Vector2i pixelCoords);
    bool isIdle() const;


private:
    int width;
    int height;
    sf::Texture& textureSheetRef;
    std::vector<std::vector<std::unique_ptr<Gem>>> grid;
    GridCoord selectedGemCoords;
    bool gemSelected;
    std::mt19937 rng;
    std::uniform_int_distribution<int> gemColorDist;
    std::uniform_real_distribution<float> bonusChanceDist;
    std::uniform_int_distribution<int> bonusTypeDist;
    std::uniform_int_distribution<int> gridXDist;
    std::uniform_int_distribution<int> gridYDist;
    bool isValidCoords(int r, int c) const;
    bool isValidCoords(GridCoord coords) const;
    bool areAdjacent(GridCoord p1, GridCoord p2) const;
    void swapGems(GridCoord p1, GridCoord p2);
    std::set<GridCoord> findMatches();
    std::pair<int, std::vector<std::pair<GridCoord, GemColor>>> destroyGems(const std::set<GridCoord>& coordsToDestroy);
    bool applyGravity();
    bool refillBoard();
    void trySpawnBonus(const std::vector<std::pair<GridCoord, GemColor>>& destroyedGemsInfo);
    void applyRepaintBonus(GridCoord targetPos, GemColor sourceColor);
    void applyBombBonus(GridCoord targetPos);
    void initializeBoard();
    bool hasPossibleMoves() const;
    void createGem(int r, int c, GemColor color);
    void createRandomGem(int r, int c);
    bool needsMatchCheck;
    bool needsGravityCheck;
    bool needsRefillCheck;
    bool needsBonusProcessing;
    std::vector<std::pair<GridCoord, GemColor>> recentlyDestroyedGems;

};
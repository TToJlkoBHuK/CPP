#pragma once

#include <vector>
#include <set>
#include <random>
#include <SFML/Graphics.hpp>
#include "Gem.h"
#include "Config.h"
#include <memory>
class IdleState : public GameStateBase {
public:
    void enter(Board& board) override;
    GameStateBase* update(Board& board) override;
    void exit(Board& board) override;
    bool isIdle() const override;
};

class GemSelectedState : public GameStateBase {
public:
    void enter(Board& board) override;
    GameStateBase* update(Board& board) override;
    void exit(Board& board) override;
};

class SwappingState : public GameStateBase {
public:
    void enter(Board& board) override;
    GameStateBase* update(Board& board) override;
    void exit(Board& board) override;
};

class CheckingState : public GameStateBase {
public:
    void enter(Board& board) override;
    GameStateBase* update(Board& board) override;
    void exit(Board& board) override;
};

class DestroyingState : public GameStateBase {
public:
    void enter(Board& board) override;
    GameStateBase* update(Board& board) override;
    void exit(Board& board) override;
};

class FallingState : public GameStateBase {
public:
    void enter(Board& board) override;
    GameStateBase* update(Board& board) override;
    void exit(Board& board) override;
};

class RefillingState : public GameStateBase {
public:
    void enter(Board& board) override;
    GameStateBase* update(Board& board) override;
    void exit(Board& board) override;
};

class ApplyingBonusState : public GameStateBase {
public:
    void enter(Board& board) override;
    GameStateBase* update(Board& board) override;
    void exit(Board& board) override;
};


class Board {
public:
    Board(int width, int height, sf::Texture& textureSheet);
    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;

    void draw(sf::RenderWindow& window);
    bool update();
    bool handleMouseClick(sf::Vector2i pixelCoords);
    bool isIdle() const;
    void setState(std::unique_ptr<GameStateBase> newState);
    GridCoord getSelectedGemCoords() const { return selectedGemCoords; }
    void setSelectedGemCoords(GridCoord coords) { selectedGemCoords = coords; }
    bool isGemSelected() const { return gemSelected; }
    void setGemSelected(bool selected) { gemSelected = selected; }

    bool isValidCoords(int r, int c) const;
    bool isValidCoords(GridCoord coords) const;
    bool areAdjacent(GridCoord p1, GridCoord p2) const;
    void swapGems(GridCoord p1, GridCoord p2);
    std::set<GridCoord> findMatches();
    std::pair<int, std::vector<std::pair<GridCoord, int>>> destroyGems(const std::set<GridCoord>& coordsToDestroy);
    bool applyGravity();
    bool refillBoard();
    void trySpawnBonus(const std::vector<std::pair<GridCoord, int>>& destroyedGemsInfo);
    void applyRepaintBonus(GridCoord targetPos, int sourceColorIndex);
    void applyBombBonus(GridCoord targetPos);
    void createGem(int r, int c, std::unique_ptr<GemColorBase> color);
    void createRandomGem(int r, int c);
    std::vector<std::vector<std::unique_ptr<Gem>>>& getGrid() { return grid; }
    const std::vector<std::vector<std::unique_ptr<Gem>>>& getGrid() const { return grid; }
    sf::Texture& getTextureSheet() { return textureSheetRef; }
    std::mt19937& getRNG() { return rng; }
    std::uniform_int_distribution<int>& getGemColorDist() { return gemColorDist; }
    std::uniform_real_distribution<float>& getBonusChanceDist() { return bonusChanceDist; }
    std::uniform_int_distribution<int>& getBonusTypeDist() { return bonusTypeDist; }
    std::uniform_int_distribution<int>& getGridXDist() { return gridXDist; }
    std::uniform_int_distribution<int>& getGridYDist() { return gridYDist; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    std::vector<std::pair<GridCoord, int>> recentlyDestroyedGems;

    bool hasPossibleMoves() const;


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

    std::unique_ptr<GameStateBase> currentState;

    void initializeBoard();
};
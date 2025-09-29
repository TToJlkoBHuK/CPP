#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <map>
#include <memory>

constexpr int GRID_WIDTH = 8;
constexpr int GRID_HEIGHT = 8;
constexpr int GEM_SIZE = 64;
constexpr unsigned int WINDOW_WIDTH = GRID_WIDTH * GEM_SIZE;
constexpr unsigned int WINDOW_HEIGHT = GRID_HEIGHT * GEM_SIZE;
const int NUM_GEM_COLORS = 6;
const float BONUS_CHANCE = 0.1f;
const int REPAINT_BONUS_RADIUS = 3;
const int BOMB_BONUS_COUNT = 5;
class GemColorBase {
public:
    virtual ~GemColorBase() = default;
    virtual int getIndex() const = 0;
    virtual sf::Color getSFMLColor() const = 0;
};
class RedGemColor : public GemColorBase {
public:
    int getIndex() const override { return 0; }
    sf::Color getSFMLColor() const override { return sf::Color::Red; }
};

class GreenGemColor : public GemColorBase {
public:
    int getIndex() const override { return 1; }
    sf::Color getSFMLColor() const override { return sf::Color::Green; }
};

class BlueGemColor : public GemColorBase {
public:
    int getIndex() const override { return 2; }
    sf::Color getSFMLColor() const override { return sf::Color::Blue; }
};

class YellowGemColor : public GemColorBase {
public:
    int getIndex() const override { return 3; }
    sf::Color getSFMLColor() const override { return sf::Color::Yellow; }
};

class PurpleGemColor : public GemColorBase {
public:
    int getIndex() const override { return 4; }
    sf::Color getSFMLColor() const override { return sf::Color::Magenta; }
};

class OrangeGemColor : public GemColorBase {
public:
    int getIndex() const override { return 5; }
    sf::Color getSFMLColor() const override { return sf::Color(255, 165, 0); }
};

class NoneGemColor : public GemColorBase {
public:
    int getIndex() const override { return -1; }
    sf::Color getSFMLColor() const override { return sf::Color::Black; }
};
class GameStateBase {
public:
    virtual ~GameStateBase() = default;
    virtual void enter(class Board& board) = 0;
    virtual GameStateBase* update(class Board& board) = 0;
    virtual void exit(class Board& board) = 0;
    virtual bool isIdle() const { return false; }
};
struct GridCoord {
    int x;
    int y;

    GridCoord() : x(0), y(0) {}
    GridCoord(int x, int y) : x(x), y(y) {}
    bool operator<(const GridCoord& other) const {
        if (y == other.y) {
            return x < other.x;
        }
        return y < other.y;
    }

    bool operator==(const GridCoord& other) const {
        return x == other.x && y == other.y;
    }
};
namespace sf {
    inline bool operator<(const Vector2i& a, const Vector2i& b) {
        if (a.x == b.x) {
            return a.y < b.y;
        }
        return a.x < b.x;
    }
}
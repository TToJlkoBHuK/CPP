#pragma once

#include <SFML/System/Vector2.hpp>
constexpr int GRID_WIDTH = 8;
constexpr int GRID_HEIGHT = 8;
constexpr int GEM_SIZE = 64;
constexpr unsigned int WINDOW_WIDTH = GRID_WIDTH * GEM_SIZE;
constexpr unsigned int WINDOW_HEIGHT = GRID_HEIGHT * GEM_SIZE;
const int NUM_GEM_COLORS = 6;
const float BONUS_CHANCE = 0.1f;
const int REPAINT_BONUS_RADIUS = 3;
const int BOMB_BONUS_COUNT = 5;
enum class GemColor {
    Red = 0,
    Green,
    Blue,
    Yellow,
    Purple,
    Orange,
    None
};
enum class GameState {
    Idle,
    GemSelected,
    Swapping,
    Checking,
    Destroying,
    Falling,
    Refilling,
    ApplyingBonus
};

namespace sf {
    inline bool operator<(const Vector2i& a, const Vector2i& b) {
        if (a.x == b.x) {
            return a.y < b.y;
        }
        return a.x < b.x;
    }
}
struct GridCoord {
    int x;
    int y;

    GridCoord() : x(0), y(0) {}

    GridCoord(int x, int y) : x(x), y(y) {}

    bool operator<(const GridCoord& other) const {
        return (y < other.y) || (y == other.y && x < other.x);
    }

    bool operator==(const GridCoord& other) const {
        return x == other.x && y == other.y;
    }
};
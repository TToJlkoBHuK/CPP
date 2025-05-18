#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SFML/Graphics.hpp>

const float SCREEN_WIDTH = 800.f;
const float SCREEN_HEIGHT = 600.f;
const float PADDLE_START_WIDTH = 100.f;
const float PADDLE_HEIGHT = 20.f;
const float PADDLE_SPEED = 400.f;
const float PADDLE_MIN_WIDTH = 40.f;
const float PADDLE_WIDTH_CHANGE_FACTOR = 0.5f;
const float BALL_RADIUS = 8.f;
const float BALL_START_SPEED = 250.f;
const float BALL_SPEED_CHANGE_FACTOR = 1.2f;
const float BRICK_WIDTH = 60.f;
const float BRICK_HEIGHT = 20.f;
const int BRICK_COLS = 10;
const int BRICK_ROWS = 5;
const float BRICK_START_X = (SCREEN_WIDTH - (BRICK_COLS * (BRICK_WIDTH + 5.f) - 5.f)) / 2.f;
const float BRICK_START_Y = 50.f;
const float BONUS_SIZE = 20.f;
const float BONUS_FALL_SPEED = 150.f;

enum class BlockType {
    NORMAL_1HP,
    NORMAL_2HP,
    NORMAL_3HP,
    INDESTRUCTIBLE,
    BONUS_DROPPER,
    SPEED_UP_BALL
};

enum class BonusType {
    NONE,
    PADDLE_WIDE,
    PADDLE_NARROW,
    BALL_FAST,
    BALL_SLOW,
    STICKY_PADDLE,
    SAFETY_NET,
    MULTI_BALL
};

enum class GameState {
    PLAYING,
    PAUSED,
    GAME_OVER,
    LEVEL_CLEAR
};

#endif // CONSTANTS_H
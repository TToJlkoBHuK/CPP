// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SFML/Graphics.hpp>
const float SCREEN_WIDTH = 800.f;
const float SCREEN_HEIGHT = 600.f;
const float PADDLE_START_WIDTH = 100.f;
const float PADDLE_HEIGHT = 20.f;
const float PADDLE_SPEED = 400.f;
const float PADDLE_MIN_WIDTH = 40.f;
const float PADDLE_MAX_WIDTH = 200.f;
const float PADDLE_WIDTH_CHANGE_FACTOR_SMALL = 0.7f;
const float PADDLE_WIDTH_CHANGE_FACTOR_LARGE = 1.3f;
const float BALL_RADIUS = 8.f;
const float BALL_START_SPEED = 250.f;
const float BALL_SPEED_CHANGE_FACTOR = 1.2f;
const float BALL_MIN_SPEED = 150.f;
const float BALL_MAX_SPEED = 500.f;
const float BRICK_WIDTH = 60.f;
const float BRICK_HEIGHT = 20.f;
const int BRICK_COLS = 10;
const int BRICK_ROWS = 5;
const float BRICK_SPACING = 5.f;
const float BRICK_START_X = (SCREEN_WIDTH - (BRICK_COLS * (BRICK_WIDTH + BRICK_SPACING) - BRICK_SPACING)) / 2.f;
const float BRICK_START_Y = 50.f;
const float BONUS_SIZE = 20.f;
const float BONUS_FALL_SPEED = 150.f;
enum class GameState {
    PLAYING,
    PAUSED,
    GAME_OVER,
    LEVEL_CLEAR,
    MAIN_MENU
};
const int SCORE_BRICK_HIT_NORMAL = 10;
const int SCORE_BRICK_DESTROYED_NORMAL = 50;
const int SCORE_BRICK_DESTROYED_BONUS = 70;
const int SCORE_BRICK_DESTROYED_SPEED = 60;
const int SCORE_LOST_LIFE = -50;

#endif // CONSTANTS_H

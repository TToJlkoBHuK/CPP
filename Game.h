#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include "constants.h"
#include "Paddle.h"
#include "Ball.h"

class Brick;
class Bonus;

class Game {
public:
    Game();
    ~Game();

    void run();

    Paddle& getPaddle();
    std::vector<Ball>& getBalls();

    void addScore(int points);
    void loseLife();

    void setPaddleSticky(bool sticky);
    void setSafetyNetActive(bool active);
    void spawnAdditionalBall();
    void addActiveBonus(std::unique_ptr<Bonus> bonus);


private:
    sf::RenderWindow window;
    Paddle paddle;
    std::vector<Ball> balls;
    std::vector<std::unique_ptr<Brick>> bricks;
    std::vector<std::unique_ptr<Bonus>> activeBonuses;

    int score;
    int lives;
    GameState gameState;
    bool safetyNetActive;

    sf::Font font;
    sf::Text scoreText;
    sf::Text livesText;
    sf::Text messageText;

    sf::Clock deltaClock;

    void setupText(sf::Text& text, unsigned int charSize, sf::Color color, float x, float y);
    void loadLevel(int levelNumber = 1);
    void resetLevelState();
    void resetGame();

    void processEvents();
    void update(sf::Time dt);
    void render();

    void handleCollisions();
    void handleBallWallCollision(Ball& ball);
    void handleBallPaddleCollision(Ball& ball);
    void handleBallBrickCollision(Ball& ball, Brick& brick);
    void handlePaddleBonusCollision(Paddle& paddle, Bonus& bonus);
    void handleBallBallCollision(Ball& ball1, Ball& ball2);

    void updateUI();
};

#endif // GAME_H

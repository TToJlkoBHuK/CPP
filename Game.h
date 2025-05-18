#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "constants.h"
#include "Paddle.h"
#include "Ball.h"
#include "Brick.h"
#include "Bonus.h"

class Game {
public:
    Game();
    void run();

private:
    sf::RenderWindow window;
    Paddle paddle;
    std::vector<Ball> balls;
    std::vector<Brick> bricks;
    std::vector<Bonus> activeBonuses;

    int score;
    int lives;
    GameState gameState;
    bool safetyNetActive;

    sf::Font font;
    sf::Text scoreText;
    sf::Text livesText;
    sf::Text messageText;

    sf::Clock deltaClock;

    void processEvents();
    void update(sf::Time dt);
    void render();

    void loadLevel();    
    void resetLevel();   
    void resetGame();    

    void handleCollisions();
    void handleBallWallCollision(Ball& ball);
    void handleBallPaddleCollision(Ball& ball);
    void handleBallBrickCollision(Ball& ball, Brick& brick, size_t brickIndex);
    void handlePaddleBonusCollision(Bonus& bonus, size_t bonusIndex);
    void handleBallBallCollision(Ball& ball1, Ball& ball2); 

    void spawnBonusItem(const Brick& brick); 
    void applyBonusEffect(BonusType type);   

    void updateUI(); 
    void setupText(sf::Text& text, int charSize, sf::Color color, float x, float y);
};

#endif // GAME_H
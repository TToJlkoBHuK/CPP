#include "Game.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

Game::Game()
    : window(sf::VideoMode({ static_cast<unsigned int>(SCREEN_WIDTH), static_cast<unsigned int>(SCREEN_HEIGHT) }), "SFML Arkanoid"),
    paddle(PADDLE_START_WIDTH, PADDLE_HEIGHT, sf::Color::Cyan),
    score(0), lives(3), gameState(GameState::PLAYING), safetyNetActive(false), font(), scoreText(font, "", 20), livesText(font, "", 20), messageText(font, "", 20) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    if (!font.openFromFile("arial.ttf")) {
    }
    setupText(scoreText, 20, sf::Color::White, 10.f, 10.f);
    setupText(livesText, 20, sf::Color::White, SCREEN_WIDTH - 100.f, 10.f);
    setupText(messageText, 40, sf::Color::Yellow, SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f);
    sf::FloatRect localBounds = messageText.getLocalBounds();
    setupText(messageText, 40, sf::Color::Yellow, SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f);
    messageText.setOrigin(sf::Vector2f(messageText.getLocalBounds().size.x / 2.f, messageText.getLocalBounds().size.y / 2.f));
    loadLevel();
    balls.emplace_back(BALL_RADIUS, sf::Color::Yellow, BALL_START_SPEED);
    resetLevel();
}
void Game::setupText(sf::Text& text, int charSize, sf::Color color, float x, float y) {
    text.setFont(font);
    text.setCharacterSize(charSize);
    text.setFillColor(color);
    text.setPosition(sf::Vector2f(x, y));
}
void Game::loadLevel() {
    bricks.clear();
    for (int i = 0; i < BRICK_ROWS; ++i) {
        for (int j = 0; j < BRICK_COLS; ++j) {
            BlockType type = BlockType::NORMAL_1HP;
            sf::Color color = sf::Color::Green;
            int randType = std::rand() % 100;

            if (randType < 5) {
                type = BlockType::INDESTRUCTIBLE;
                color = sf::Color(100, 100, 100);
            }
            else if (randType < 15) {
                type = BlockType::BONUS_DROPPER;
                color = sf::Color::Magenta;
            }
            else if (randType < 25) {
                type = BlockType::SPEED_UP_BALL;
                color = sf::Color::Red;
            }
            else if (randType < 45) {
                type = BlockType::NORMAL_2HP;
                color = sf::Color(255, 165, 0);
            }
            else if (randType < 60) {
                type = BlockType::NORMAL_3HP;
                color = sf::Color(255, 69, 0);
            }
            else {
                type = BlockType::NORMAL_1HP;
                color = sf::Color(0, 128, 0);
            }
            if (i % 2 == 0) color = sf::Color(color.r * 0.8f, color.g * 0.8f, color.b * 0.8f);


            bricks.emplace_back(BRICK_START_X + j * (BRICK_WIDTH + 5.f),
                BRICK_START_Y + i * (BRICK_HEIGHT + 5.f),
                BRICK_WIDTH, BRICK_HEIGHT, type, color);
        }
    }
}
void Game::resetLevel() {
    paddle.reset();
    for (auto& ball : balls) {
        ball.reset(paddle.shape.getPosition(), paddle.shape.getSize());
        ball.currentSpeed = BALL_START_SPEED;
    }
    if (balls.size() > 1) {
        balls.erase(balls.begin() + 1, balls.end());
    }
    if (balls.empty()) {
        balls.emplace_back(BALL_RADIUS, sf::Color::Yellow, BALL_START_SPEED);
    }
    balls[0].reset(paddle.shape.getPosition(), paddle.shape.getSize());
    activeBonuses.clear();
    safetyNetActive = false;
    gameState = GameState::PLAYING;
}
void Game::resetGame() {
    score = 0;
    lives = 3;
    loadLevel();
    resetLevel();
}
void Game::run() {
    while (window.isOpen()) {
        sf::Time dt = deltaClock.restart();
        processEvents();
        if (gameState == GameState::PLAYING) {
            update(dt);
        }
        render();
    }
}
void Game::processEvents() {
    while (const auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                if (gameState == GameState::PLAYING)
                    gameState = GameState::PAUSED;
                else if (gameState == GameState::PAUSED)
                    gameState = GameState::PLAYING;
            }
            if ((gameState == GameState::GAME_OVER || gameState == GameState::LEVEL_CLEAR)
                && keyPressed->code == sf::Keyboard::Key::Enter) {
                resetGame();
            }
            if (gameState == GameState::PAUSED && keyPressed->code == sf::Keyboard::Key::Enter) {
                gameState = GameState::PLAYING;
            }
        }
    }
}
void Game::update(sf::Time dt) {
    paddle.update(dt);
    for (auto& ball : balls) {
        ball.update(dt, paddle.shape, paddle.isStickyActive);
    }
    for (auto& bonus : activeBonuses) {
        bonus.update(dt);
    }
    activeBonuses.erase(std::remove_if(activeBonuses.begin(), activeBonuses.end(),
        [](const Bonus& b) { return !b.isActive; }),
        activeBonuses.end());
    handleCollisions();
    bool destructibleBricksLeft = false;
    for (const auto& brick : bricks) {
        if (brick.isVisible && brick.type != BlockType::INDESTRUCTIBLE) {
            destructibleBricksLeft = true;
            break;
        }
    }
    if (!destructibleBricksLeft && !bricks.empty()) {
        gameState = GameState::LEVEL_CLEAR;
    }
    updateUI();
}
void Game::updateUI() {
    scoreText.setString("Score: " + std::to_string(score));
    livesText.setString("Lives: " + std::to_string(lives));
    if (gameState == GameState::GAME_OVER) {
        messageText.setString("GAME OVER\nPress Enter to Restart");
        sf::FloatRect bounds = messageText.getLocalBounds();
        messageText.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f));
        messageText.setPosition(sf::Vector2f(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f));
    }
    else if (gameState == GameState::LEVEL_CLEAR) {
        messageText.setString("LEVEL CLEAR!\nPress Enter to Play Again");
        sf::FloatRect bounds = messageText.getLocalBounds();
        messageText.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f));
        messageText.setPosition(sf::Vector2f(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f));
    }
    else if (gameState == GameState::PAUSED) {
        messageText.setString("PAUSED\nPress Escape to Resume");
        sf::FloatRect bounds = messageText.getLocalBounds();
        messageText.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f));
        messageText.setPosition(sf::Vector2f(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f));
    }
    else {
        messageText.setString("");
    }
}
void Game::handleCollisions() {
    for (size_t i = 0; i < balls.size(); ++i) {
        handleBallWallCollision(balls[i]);
        handleBallPaddleCollision(balls[i]);
        for (size_t j = 0; j < bricks.size(); ++j) {
            if (bricks[j].isVisible) {
                handleBallBrickCollision(balls[i], bricks[j], j);
            }
        }
    }
    if (balls.size() > 1) {
        for (size_t i = 0; i < balls.size(); ++i) {
            for (size_t j = i + 1; j < balls.size(); ++j) {
                if (balls[i].getBounds().findIntersection(balls[j].getBounds()).has_value()) {
                    handleBallBallCollision(balls[i], balls[j]);
                }
            }
        }
    }
    for (size_t i = 0; i < activeBonuses.size(); ++i) {
        if (paddle.getBounds().findIntersection(activeBonuses[i].getBounds()).has_value()) {
            handlePaddleBonusCollision(activeBonuses[i], i);
        }
    }
    for (size_t i = 0; i < balls.size(); ) {
        if (balls[i].shape.getPosition().y - balls[i].shape.getRadius() > SCREEN_HEIGHT) {
            if (safetyNetActive) {
                balls[i].velocity.y = -std::abs(balls[i].velocity.y);
                balls[i].shape.setPosition(sf::Vector2f(balls[i].shape.getPosition().x, SCREEN_HEIGHT - balls[i].shape.getRadius() - 1.f));
                safetyNetActive = false;
                i++;
            }
            else {
                balls.erase(balls.begin() + i);
            }
        }
        else {
            i++;
        }
    }
    if (balls.empty() && gameState == GameState::PLAYING) {
        lives--;
        score -= 50;
        if (score < 0) score = 0;
        paddle.changeSize(1.0f - PADDLE_WIDTH_CHANGE_FACTOR);
        if (lives <= 0) {
            gameState = GameState::GAME_OVER;
        }
        else {
            balls.emplace_back(BALL_RADIUS, sf::Color::Yellow, BALL_START_SPEED);
            balls.back().reset(paddle.shape.getPosition(), paddle.shape.getSize());
        }
    }
}
void Game::handleBallWallCollision(Ball& ball) {
    sf::Vector2f pos = ball.shape.getPosition();
    float r = ball.shape.getRadius();
    if (ball.shape.getPosition().x - ball.shape.getRadius() < 0.f) {
        ball.velocity.x = std::abs(ball.velocity.x);
        ball.shape.setPosition(sf::Vector2f(r, pos.y));
    }
    else if (pos.x + r > SCREEN_WIDTH) {
        ball.velocity.x = -std::abs(ball.velocity.x);
        ball.shape.setPosition(sf::Vector2f(SCREEN_WIDTH - r, pos.y));
    }
    if (pos.y - r < 0.f) {
        ball.velocity.y = std::abs(ball.velocity.y);
        ball.shape.setPosition(sf::Vector2f(pos.x, r));
    }
}
void Game::handleBallPaddleCollision(Ball& ball) {
    if (ball.attachedToPaddle) return;
    sf::FloatRect ballBounds = ball.getBounds();
    sf::FloatRect paddleBounds = paddle.getBounds();
    if (ballBounds.findIntersection(paddleBounds).has_value()) {
        if (ball.velocity.y > 0 && ball.shape.getPosition().y < paddle.shape.getPosition().y) {
            if (paddle.isStickyActive) {
                ball.attachedToPaddle = true;
                paddle.isStickyActive = false;
                ball.velocity = sf::Vector2f(0, 0);
            }
            else {
                ball.velocity.y = -std::abs(ball.velocity.y);
                float hitPos = (ball.shape.getPosition().x - paddle.shape.getPosition().x) / paddle.shape.getSize().x;
                float influence = (hitPos - 0.5f) * 2.f;
                ball.velocity.x = influence * ball.currentSpeed * 0.8f;
                float magnitude = std::sqrt(ball.velocity.x * ball.velocity.x + ball.velocity.y * ball.velocity.y);
                if (magnitude > 0) {
                    ball.velocity = (ball.velocity / magnitude) * ball.currentSpeed;
                }
            }
            ball.shape.setPosition(sf::Vector2f(ball.shape.getPosition().x, paddle.shape.getPosition().y - ball.shape.getRadius() - 0.1f));
        }
    }
}
void Game::handleBallBrickCollision(Ball& ball, Brick& brick, size_t brickIndex) {
    if (ball.attachedToPaddle || !brick.isVisible) return;
    sf::FloatRect ballBounds = ball.getBounds();
    sf::FloatRect brickBounds = brick.getBounds();
    if (ballBounds.findIntersection(brickBounds).has_value()) {
        float overlapLeft = (ballBounds.position.x + ballBounds.size.x) - brickBounds.position.x;
        float overlapRight = (brickBounds.position.x + brickBounds.size.x) - ballBounds.position.x;
        float overlapTop = (ballBounds.position.y + ballBounds.size.y) - brickBounds.position.y;
        float overlapBottom = (brickBounds.position.y + brickBounds.size.y) - ballBounds.position.y;
        bool ballFromLeft = std::abs(overlapLeft) < std::abs(overlapRight);
        bool ballFromTop = std::abs(overlapTop) < std::abs(overlapBottom);
        float minOverlapX = ballFromLeft ? overlapLeft : overlapRight;
        float minOverlapY = ballFromTop ? overlapTop : overlapBottom;
        if (minOverlapX < minOverlapY) {
            ball.velocity.x = (ballFromLeft ? -1 : 1) * std::abs(ball.velocity.x);
        }
        else {
            ball.velocity.y = (ballFromTop ? -1 : 1) * std::abs(ball.velocity.y);
        }
        if (brick.type != BlockType::INDESTRUCTIBLE) {
            score += 10;
            if (brick.hit()) {
                score += 50;
                if (brick.hasBonus) {
                    spawnBonusItem(brick);
                }
            }
            if (brick.type == BlockType::SPEED_UP_BALL) {
                ball.applySpeedChange(BALL_SPEED_CHANGE_FACTOR);
            }
        }
    }
}
void Game::handlePaddleBonusCollision(Bonus& bonus, size_t bonusIndex) {
    applyBonusEffect(bonus.type);
    activeBonuses[bonusIndex].isActive = false;
}
void Game::handleBallBallCollision(Ball& ball1, Ball& ball2) {
    if (ball1.attachedToPaddle || ball2.attachedToPaddle) return;
    sf::Vector2f delta = ball2.shape.getPosition() - ball1.shape.getPosition();
    float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    float minDistance = ball1.shape.getRadius() + ball2.shape.getRadius();
    if (distance < minDistance) {
        float overlap = 0.5f * (minDistance - distance);
        sf::Vector2f separationVector = (delta / distance) * overlap;
        ball1.shape.move(-separationVector);
        ball2.shape.move(separationVector);
        sf::Vector2f normal = delta / distance;
        sf::Vector2f tangent(-normal.y, normal.x);
        float v1n = ball1.velocity.x * normal.x + ball1.velocity.y * normal.y;
        float v1t = ball1.velocity.x * tangent.x + ball1.velocity.y * tangent.y;
        float v2n = ball2.velocity.x * normal.x + ball2.velocity.y * normal.y;
        float v2t = ball2.velocity.x * tangent.x + ball2.velocity.y * tangent.y;
        std::swap(v1n, v2n);
        ball1.velocity = v1n * normal + v1t * tangent;
        ball2.velocity = v2n * normal + v2t * tangent;
    }
}
void Game::spawnBonusItem(const Brick& brick) {
    sf::Color bonusColor = sf::Color::Yellow;
    switch (brick.bonusToDrop) {
    case BonusType::PADDLE_WIDE: bonusColor = sf::Color::Green; break;
    case BonusType::PADDLE_NARROW: bonusColor = sf::Color::Red; break;
    case BonusType::BALL_FAST: bonusColor = sf::Color(255, 165, 0); break;
    case BonusType::BALL_SLOW: bonusColor = sf::Color::Blue; break;
    case BonusType::STICKY_PADDLE: bonusColor = sf::Color::Cyan; break;
    case BonusType::SAFETY_NET: bonusColor = sf::Color::White; break;
    case BonusType::MULTI_BALL: bonusColor = sf::Color(255, 255, 0, 128); break;
    default: break;
    }
    activeBonuses.emplace_back(brick.shape.getPosition().x + BRICK_WIDTH / 2.f - BONUS_SIZE / 2.f,
        brick.shape.getPosition().y + BRICK_HEIGHT / 2.f,
        BONUS_SIZE, brick.bonusToDrop, bonusColor);
}
void Game::applyBonusEffect(BonusType type) {
    switch (type) {
    case BonusType::PADDLE_WIDE:
        paddle.changeSize(1.3f);
        break;
    case BonusType::PADDLE_NARROW:
        paddle.changeSize(0.7f);
        break;
    case BonusType::BALL_FAST:
        for (auto& ball : balls) ball.applySpeedChange(BALL_SPEED_CHANGE_FACTOR);
        break;
    case BonusType::BALL_SLOW:
        for (auto& ball : balls) ball.applySpeedChange(1.f / BALL_SPEED_CHANGE_FACTOR);
        break;
    case BonusType::STICKY_PADDLE:
        paddle.isStickyActive = true;
        break;
    case BonusType::SAFETY_NET:
        safetyNetActive = true;
        break;
    case BonusType::MULTI_BALL:
        if (!balls.empty()) {
            Ball newBall = balls[0];
            newBall.shape.move(sf::Vector2f(std::rand() % 20 - 10, std::rand() % 10 - 5));
            newBall.velocity.x *= (std::rand() % 2 == 0 ? 1.f : -1.f);
            newBall.velocity.y *= (std::rand() % 2 == 0 ? 0.8f : -0.8f);
            if (newBall.velocity.x == 0 && newBall.velocity.y == 0) {
                newBall.velocity.y = -newBall.currentSpeed;
            }
            newBall.attachedToPaddle = false;
            balls.push_back(newBall);
        }
        break;
    case BonusType::NONE: default: break;
    }
}
void Game::render() {
    window.clear(sf::Color::Black);
    for (const auto& brick : bricks) {
        brick.draw(window);
    }
    paddle.draw(window);
    for (auto& ball : balls) {
        ball.draw(window);
    }
    for (auto& bonus : activeBonuses) {
        bonus.draw(window);
    }
    if (safetyNetActive) {
        sf::RectangleShape netShape(sf::Vector2f(SCREEN_WIDTH, 5.f));
        netShape.setFillColor(sf::Color(0, 255, 255, 100));
        netShape.setPosition(sf::Vector2f(0, SCREEN_HEIGHT - 5.f));
        window.draw(netShape);
    }
    window.draw(scoreText);
    window.draw(livesText);
    if (gameState != GameState::PLAYING) {
        window.draw(messageText);
    }
    window.display();
}
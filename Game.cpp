#include "Game.h"
#include "Brick.h"
#include "Bonus.h"
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

    loadLevel(1);
    resetLevelState();
}

Game::~Game() {

}
void Game::setupText(sf::Text& text, unsigned int charSize, sf::Color color, float x, float y) {
    text.setFont(font);
    text.setCharacterSize(charSize);
    text.setFillColor(color);
    text.setPosition(sf::Vector2f(x, y));
}
void Game::loadLevel(int levelNumber) {
    bricks.clear();
    for (int i = 0; i < BRICK_ROWS; ++i) {
        for (int j = 0; j < BRICK_COLS; ++j) {
            float brickX = BRICK_START_X + j * (BRICK_WIDTH + BRICK_SPACING);
            float brickY = BRICK_START_Y + i * (BRICK_HEIGHT + BRICK_SPACING);
            int randType = std::rand() % 100;
            sf::Color brickColor = sf::Color::Green;

            if (randType < 5) {
                brickColor = sf::Color(100, 100, 100);
                bricks.emplace_back(std::make_unique<IndestructibleBrick>(brickX, brickY, brickColor));
            }
            else if (randType < 15) {
                brickColor = sf::Color::Magenta;
                bricks.emplace_back(std::make_unique<BonusDropperBrick>(brickX, brickY, brickColor));
            }
            else if (randType < 25) {
                brickColor = sf::Color::Red;
                bricks.emplace_back(std::make_unique<SpeedUpBrick>(brickX, brickY, brickColor));
            }
            else if (randType < 45) {
                brickColor = sf::Color(255, 165, 0);
                bricks.emplace_back(std::make_unique<NormalBrick>(brickX, brickY, brickColor, 2));
            }
            else if (randType < 60) {
                brickColor = sf::Color(255, 69, 0);
                bricks.emplace_back(std::make_unique<NormalBrick>(brickX, brickY, brickColor, 3));
            }
            else {
                brickColor = sf::Color(0, 128, 0);
                bricks.emplace_back(std::make_unique<NormalBrick>(brickX, brickY, brickColor, 1));
            }
        }
    }
    gameState = GameState::PLAYING;
}
void Game::resetLevelState() {
    paddle.reset();

    balls.clear();
    balls.emplace_back(BALL_RADIUS, sf::Color::Yellow, BALL_START_SPEED);
    balls[0].reset(paddle.shape.getPosition(), paddle.shape.getSize());

    activeBonuses.clear();
    safetyNetActive = false;
}
void Game::resetGame() {
    score = 0;
    lives = 3;
    loadLevel(1);
    resetLevelState();
    gameState = GameState::PLAYING;
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

    for (auto& bonus_ptr : activeBonuses) {
        bonus_ptr->update(dt);
    }
    activeBonuses.erase(std::remove_if(activeBonuses.begin(), activeBonuses.end(),
        [](const std::unique_ptr<Bonus>& b) { return !b->isActive; }),
        activeBonuses.end());

    handleCollisions();
    bool destructibleBricksLeft = false;
    for (const auto& brick_ptr : bricks) {
        if (brick_ptr->isVisible && brick_ptr->isDestructible()) {
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

    messageText.setString("");
    if (gameState == GameState::GAME_OVER) {
        messageText.setString("GAME OVER\nPress Enter to Restart");
    }
    else if (gameState == GameState::LEVEL_CLEAR) {
        messageText.setString("LEVEL CLEAR!\nPress Enter for Next Level");
    }
    else if (gameState == GameState::PAUSED) {
        messageText.setString("PAUSED\nPress Escape to Resume");
    }
    if (!messageText.getString().isEmpty()) {
        sf::FloatRect textBounds = messageText.getLocalBounds();
        messageText.setOrigin(sf::Vector2f(textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f));
        messageText.setPosition(sf::Vector2f(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f));
    }
}
void Game::handleCollisions() {
    for (size_t i = 0; i < balls.size(); ++i) {
        handleBallWallCollision(balls[i]);
        handleBallPaddleCollision(balls[i]);
        for (const auto& brick_ptr : bricks) {
            if (brick_ptr->isVisible) {
                handleBallBrickCollision(balls[i], *brick_ptr);
            }
        }
    }
    if (balls.size() > 1) {
        for (size_t i = 0; i < balls.size(); ++i) {
            for (size_t j = i + 1; j < balls.size(); ++j) {
                if (balls[i].getBounds().findIntersection(balls[j].getBounds())) {
                    handleBallBallCollision(balls[i], balls[j]);
                }
            }
        }
    }
    for (const auto& bonus_ptr : activeBonuses) {
        if (bonus_ptr->isActive && paddle.getBounds().findIntersection(bonus_ptr->getBounds())) {
            handlePaddleBonusCollision(paddle, *bonus_ptr);
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
        loseLife();
        if (lives > 0) {
            resetLevelState();
        }
        else {
            gameState = GameState::GAME_OVER;
        }
    }
}

void Game::loseLife() {
    lives--;
    addScore(SCORE_LOST_LIFE);
    if (score < 0) score = 0;
}
void Game::handleBallWallCollision(Ball& ball) {
    sf::Vector2f pos = ball.shape.getPosition();
    float r = ball.shape.getRadius();

    if (pos.x - r < 0.f) {
        ball.velocity.x = std::abs(ball.velocity.x);
        ball.shape.setPosition(sf::Vector2f(r + 0.1f, pos.y));
    }
    else if (pos.x + r > SCREEN_WIDTH) {
        ball.velocity.x = -std::abs(ball.velocity.x);
        ball.shape.setPosition(sf::Vector2f(SCREEN_WIDTH - r - 0.1f, pos.y));
    }

    if (pos.y - r < 0.f) {
        ball.velocity.y = std::abs(ball.velocity.y);
        ball.shape.setPosition(sf::Vector2f(pos.x, r + 0.1f));
    }
}
void Game::handleBallPaddleCollision(Ball& ball) {
    if (ball.attachedToPaddle) return;

    sf::FloatRect ballBounds = ball.getBounds();
    sf::FloatRect paddleBounds = paddle.getBounds();

    if (ballBounds.findIntersection(paddleBounds)) {
        if (ball.velocity.y > 0 && ball.shape.getPosition().y < paddle.shape.getPosition().y) {
            if (paddle.isStickyActive) {
                ball.attachedToPaddle = true;
                paddle.isStickyActive = false;
                ball.velocity = sf::Vector2f(0.f, 0.f);
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
void Game::handleBallBrickCollision(Ball& ball, Brick& brick) {
    if (ball.attachedToPaddle || !brick.isVisible) return;

    sf::FloatRect ballBounds = ball.getBounds();
    sf::FloatRect brickBounds = brick.getBounds();

    if (ballBounds.findIntersection(brickBounds)) {
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
            if (ballFromLeft) ball.shape.setPosition(sf::Vector2f(brickBounds.position.x - ball.shape.getRadius() - 0.1f, ball.shape.getPosition().y));
            else ball.shape.setPosition(sf::Vector2f(brickBounds.position.x + brickBounds.size.x + ball.shape.getRadius() + 0.1f, ball.shape.getPosition().y));
        }
        else {
            ball.velocity.y = (ballFromTop ? -1 : 1) * std::abs(ball.velocity.y);
            if (ballFromTop) ball.shape.setPosition(sf::Vector2f(ball.shape.getPosition().x, brickBounds.position.y - ball.shape.getRadius() - 0.1f));
            else ball.shape.setPosition(sf::Vector2f(ball.shape.getPosition().x, brickBounds.position.y + brickBounds.size.y + ball.shape.getRadius() + 0.1f));
        }
        if (brick.onHit(*this, ball)) {
            std::unique_ptr<Bonus> newBonus = brick.createBonus(
                brick.shape.getPosition().x + brick.shape.getSize().x / 2.f - BONUS_SIZE / 2.f,
                brick.shape.getPosition().y + brick.shape.getSize().y / 2.f
            );
            if (newBonus) {
                addActiveBonus(std::move(newBonus));
            }
        }
    }
}
void Game::handlePaddleBonusCollision(Paddle& paddle_ref, Bonus& bonus) {
    bonus.applyEffect(*this);
    bonus.isActive = false;
}
void Game::handleBallBallCollision(Ball& b1, Ball& b2) {
    if (b1.attachedToPaddle || b2.attachedToPaddle) return;

    sf::Vector2f delta = b2.shape.getPosition() - b1.shape.getPosition();
    float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    float minDistance = b1.shape.getRadius() + b2.shape.getRadius();

    if (distance < minDistance && distance > 0.001f) {
        float overlap = 0.5f * (minDistance - distance);
        sf::Vector2f separationVector = (delta / distance) * overlap;
        b1.shape.move(-separationVector);
        b2.shape.move(separationVector);
        sf::Vector2f normal = delta / distance;
        sf::Vector2f tangent(-normal.y, normal.x);
        float v1n_scalar = b1.velocity.x * normal.x + b1.velocity.y * normal.y;
        float v1t_scalar = b1.velocity.x * tangent.x + b1.velocity.y * tangent.y;
        float v2n_scalar = b2.velocity.x * normal.x + b2.velocity.y * normal.y;
        float v2t_scalar = b2.velocity.x * tangent.x + b2.velocity.y * tangent.y;
        std::swap(v1n_scalar, v2n_scalar);
        b1.velocity = v1n_scalar * normal + v1t_scalar * tangent;
        b2.velocity = v2n_scalar * normal + v2t_scalar * tangent;
    }
}
void Game::render() {
    window.clear(sf::Color::Black);
    for (const auto& brick_ptr : bricks) {
        brick_ptr->draw(window);
    }
    paddle.draw(window);
    for (auto& ball : balls) {
        ball.draw(window);
    }
    for (const auto& bonus_ptr : activeBonuses) {
        bonus_ptr->draw(window);
    }
    if (safetyNetActive) {
        sf::RectangleShape netShape(sf::Vector2f(SCREEN_WIDTH, 5.f));
        netShape.setFillColor(sf::Color(0, 255, 255, 100));
        netShape.setPosition(sf::Vector2f(0.f, SCREEN_HEIGHT - netShape.getSize().y));
        window.draw(netShape);
    }
    window.draw(scoreText);
    window.draw(livesText);
    if (!messageText.getString().isEmpty()) {
        window.draw(messageText);
    }

    window.display();
}
Paddle& Game::getPaddle() { return paddle; }
std::vector<Ball>& Game::getBalls() { return balls; }

void Game::addScore(int points) {
    score += points;
    if (score < 0) score = 0;
}

void Game::setPaddleSticky(bool sticky) { paddle.isStickyActive = sticky; }
void Game::setSafetyNetActive(bool active) { safetyNetActive = active; }

void Game::spawnAdditionalBall() {
    if (balls.empty()) return;
    Ball newBall = balls[0];
    newBall.shape.move(sf::Vector2f((std::rand() % 20 - 10) * 0.1f, (std::rand() % 10 - 5) * 0.1f));
    float angle_offset = (std::rand() % 90 - 45) * (3.14159f / 180.f);
    float current_angle = std::atan2(newBall.velocity.y, newBall.velocity.x);
    float new_angle = current_angle + angle_offset;

    newBall.velocity.x = newBall.currentSpeed * std::cos(new_angle);
    newBall.velocity.y = newBall.currentSpeed * std::sin(new_angle);
    newBall.attachedToPaddle = false;

    balls.push_back(newBall);
}

void Game::addActiveBonus(std::unique_ptr<Bonus> bonus) {
    if (bonus) {
        activeBonuses.push_back(std::move(bonus));
    }
}


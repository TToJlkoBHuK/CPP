// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "Board.h"
#include <vector>
#include <set>
#include <algorithm>
#include <chrono>
#include <memory>
#include <iostream>
#include <functional>
// IdleState
void IdleState::enter(Board& board) { /* ... */ }
GameStateBase* IdleState::update(Board& board) {
    return nullptr;
}
void IdleState::exit(Board& board) { /* ... */ }
bool IdleState::isIdle() const { return true; }

// GemSelectedState
void GemSelectedState::enter(Board& board) { /* ... */ }
GameStateBase* GemSelectedState::update(Board& board) {
    return nullptr;
}
void GemSelectedState::exit(Board& board) { /* ... */ }
// SwappingState
void SwappingState::enter(Board& board) { /* ... */ }
GameStateBase* SwappingState::update(Board& board) {
    std::set<GridCoord> matchesAfterSwap = board.findMatches();
    if (matchesAfterSwap.empty()) {
        board.swapGems(board.getSelectedGemCoords(), board.getSelectedGemCoords());
        return new IdleState();
    }
    else {
        return new CheckingState();
    }
}
void SwappingState::exit(Board& board) { /* ... */ }

// CheckingState
void CheckingState::enter(Board& board) { /* ... */ }
GameStateBase* CheckingState::update(Board& board) {
    std::set<GridCoord> matches = board.findMatches();
    if (!matches.empty()) {
        auto destructionResult = board.destroyGems(matches);
        board.recentlyDestroyedGems = destructionResult.second;
        return new DestroyingState();
    }
    else {
        return new IdleState();
    }
}
void CheckingState::exit(Board& board) { /* ... */ }

// DestroyingState
void DestroyingState::enter(Board& board) { /* ... */ }
GameStateBase* DestroyingState::update(Board& board) {
    return new ApplyingBonusState();
}
void DestroyingState::exit(Board& board) { /* ... */ }

// FallingState
void FallingState::enter(Board& board) { /* ... */ }
GameStateBase* FallingState::update(Board& board) {
    if (board.applyGravity()) {
        return new RefillingState();
    }
    else {
        return new CheckingState();
    }
}
void FallingState::exit(Board& board) { /* ... */ }

// RefillingState
void RefillingState::enter(Board& board) { /* ... */ }
GameStateBase* RefillingState::update(Board& board) {
    if (board.refillBoard()) {
        return new CheckingState();
    }
    else {
        return new IdleState();
    }
}
void RefillingState::exit(Board& board) { /* ... */ }

// ApplyingBonusState
void ApplyingBonusState::enter(Board& board) { /* ... */ }
GameStateBase* ApplyingBonusState::update(Board& board) {
    board.trySpawnBonus(board.recentlyDestroyedGems);
    board.recentlyDestroyedGems.clear();
    return new FallingState();
}
void ApplyingBonusState::exit(Board& board) { /* ... */ }


Board::Board(int w, int h, sf::Texture& textureSheet)
    : width(w),
    height(h),
    textureSheetRef(textureSheet),
    selectedGemCoords(-1, -1),
    gemSelected(false),
    rng(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count())),
    gemColorDist(0, NUM_GEM_COLORS - 1),
    bonusChanceDist(0.0f, 1.0f),
    bonusTypeDist(0, 1),
    gridXDist(0, w - 1),
    gridYDist(0, h - 1),
    currentState(std::make_unique<IdleState>())
{
    grid.resize(height);
    for (int r = 0; r < height; ++r) {
        grid[r].resize(width);
    }

    initializeBoard();
}

void Board::setState(std::unique_ptr<GameStateBase> newState) {
    if (currentState) {
        currentState->exit(*this);
    }
    currentState = std::move(newState);
    if (currentState) {
        currentState->enter(*this);
    }
}

void Board::initializeBoard() {
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            do {
                createRandomGem(r, c);
            } while (
                (c >= 2 &&
                    grid[r][c]->getColor()->getIndex() == grid[r][c - 1]->getColor()->getIndex() &&
                    grid[r][c]->getColor()->getIndex() == grid[r][c - 2]->getColor()->getIndex())
                ||
                (r >= 2 &&
                    grid[r][c]->getColor()->getIndex() == grid[r - 1][c]->getColor()->getIndex() &&
                    grid[r][c]->getColor()->getIndex() == grid[r - 2][c]->getColor()->getIndex())
                );
        }
    }
    setState(std::make_unique<IdleState>());
}
void Board::createGem(int r, int c, std::unique_ptr<GemColorBase> color) {
    if (isValidCoords(r, c)) {
        grid[r][c] = std::make_unique<Gem>(std::move(color), c, r, textureSheetRef);
    }
}

void Board::createRandomGem(int r, int c) {
    if (isValidCoords(r, c)) {
        int randomColorIndex = gemColorDist(rng);
        std::unique_ptr<GemColorBase> randomColor;
        switch (randomColorIndex) {
        case 0: randomColor = std::make_unique<RedGemColor>(); break;
        case 1: randomColor = std::make_unique<GreenGemColor>(); break;
        case 2: randomColor = std::make_unique<BlueGemColor>(); break;
        case 3: randomColor = std::make_unique<YellowGemColor>(); break;
        case 4: randomColor = std::make_unique<PurpleGemColor>(); break;
        case 5: randomColor = std::make_unique<OrangeGemColor>(); break;
        default: randomColor = std::make_unique<NoneGemColor>(); break;
        }
        grid[r][c] = std::make_unique<Gem>(std::move(randomColor), c, r, textureSheetRef);
        grid[r][c]->setGridPosition(c, r);
    }
}


void Board::draw(sf::RenderWindow& window) {
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            if (grid[r][c]) {
                grid[r][c]->draw(window);
            }
        }
    }
}

bool Board::update() {
    GameStateBase* nextState = currentState->update(*this);
    if (nextState != nullptr) {
        setState(std::unique_ptr<GameStateBase>(nextState));
        return true;
    }
    return false;
}


bool Board::handleMouseClick(sf::Vector2i pixelCoords) {
    if (!isIdle() && !gemSelected)
        return false;

    int gridX = pixelCoords.x / GEM_SIZE;
    int gridY = pixelCoords.y / GEM_SIZE;
    GridCoord clickedCoords(gridX, gridY);

    if (!isValidCoords(clickedCoords)) {
        setState(std::make_unique<IdleState>());
        return false;
    }

    if (!gemSelected) {
        selectedGemCoords = clickedCoords;
        gemSelected = true;
        setState(std::make_unique<GemSelectedState>());
        return true;
    }
    else {
        GridCoord secondClickCoords = clickedCoords;

        if (secondClickCoords == selectedGemCoords) {
            gemSelected = false;
            setState(std::make_unique<IdleState>());
            return true;
        }

        if (areAdjacent(selectedGemCoords, secondClickCoords)) {
            swapGems(selectedGemCoords, secondClickCoords);
            gemSelected = false;
            setState(std::make_unique<SwappingState>());
            return true;
        }
        else {
            selectedGemCoords = secondClickCoords;
            setState(std::make_unique<GemSelectedState>());
            return true;
        }
    }
}

bool Board::isIdle() const {
    return currentState->isIdle();
}

bool Board::isValidCoords(int r, int c) const {
    return r >= 0 && r < height && c >= 0 && c < width;
}

bool Board::isValidCoords(GridCoord coords) const {
    return isValidCoords(coords.y, coords.x);
}

bool Board::areAdjacent(GridCoord p1, GridCoord p2) const {
    int dx = std::abs(p1.x - p2.x);
    int dy = std::abs(p1.y - p2.y);
    return (dx == 1 && dy == 0) || (dx == 0 && dy == 1);
}

void Board::swapGems(GridCoord p1, GridCoord p2) {
    if (!isValidCoords(p1) || !isValidCoords(p2)) return;

    auto temp = std::move(grid[p1.y][p1.x]);
    grid[p1.y][p1.x] = std::move(grid[p2.y][p2.x]);
    grid[p2.y][p2.x] = std::move(temp);

    if (grid[p1.y][p1.x]) {
        grid[p1.y][p1.x]->setGridPosition(p1.x, p1.y);
    }
    if (grid[p2.y][p2.x]) {
        grid[p2.y][p2.x]->setGridPosition(p2.x, p2.y);
    }
}

std::set<GridCoord> Board::findMatches() {
    std::set<GridCoord> matches;
    for (int r = 0; r < height; ++r) {
        int consecutive = 1;
        const GemColorBase* currentColor = nullptr;
        for (int c = 0; c < width; ++c) {
            if (grid[r][c] && grid[r][c]->getColor() && grid[r][c]->getColor()->getIndex() == (currentColor ? currentColor->getIndex() : -2)) {
                consecutive++;
            }
            else {
                if (consecutive >= 3) {
                    for (int i = c - consecutive; i < c; ++i) {
                        matches.insert({ i, r });
                    }
                }
                currentColor = grid[r][c] ? grid[r][c]->getColor() : nullptr;
                consecutive = 1;
            }
        }
        if (consecutive >= 3) {
            for (int i = width - consecutive; i < width; ++i) {
                matches.insert({ i, r });
            }
        }
    }

    for (int c = 0; c < width; ++c) {
        int consecutive = 1;
        const GemColorBase* currentColor = nullptr;
        for (int r = 0; r < height; ++r) {
            if (grid[r][c] && grid[r][c]->getColor() && grid[r][c]->getColor()->getIndex() == (currentColor ? currentColor->getIndex() : -2)) {
                consecutive++;
            }
            else {
                if (consecutive >= 3) {
                    for (int i = r - consecutive; i < r; ++i) {
                        matches.insert({ c, i });
                    }
                }
                currentColor = grid[r][c] ? grid[r][c]->getColor() : nullptr;
                consecutive = 1;
            }
        }
        if (consecutive >= 3) {
            for (int i = height - consecutive; i < height; ++i) {
                matches.insert({ c, i });
            }
        }
    }

    return matches;
}

std::pair<int, std::vector<std::pair<GridCoord, int>>> Board::destroyGems(const std::set<GridCoord>& coordsToDestroy) {
    int count = 0;
    std::vector<std::pair<GridCoord, int>> destroyedInfo;

    for (const auto& coord : coordsToDestroy) {
        if (isValidCoords(coord) && grid[coord.y][coord.x]) {
            destroyedInfo.push_back({ coord, grid[coord.y][coord.x]->getColor()->getIndex() });
            grid[coord.y][coord.x].reset();
            count++;
        }
    }
    return { count, destroyedInfo };
}

bool Board::applyGravity() {
    bool gemsFell = false;
    for (int c = 0; c < width; ++c) {
        int emptyRow = height - 1;
        for (int r = height - 1; r >= 0; --r) {
            if (grid[r][c]) {
                if (r != emptyRow) {
                    grid[emptyRow][c] = std::move(grid[r][c]);
                    grid[emptyRow][c]->setGridPosition(c, emptyRow);
                    gemsFell = true;
                }
                emptyRow--;
            }
        }
        while (emptyRow >= 0) {
            grid[emptyRow][c].reset();
            emptyRow--;
        }
    }
    return gemsFell;
}

bool Board::refillBoard() {
    bool newGemsAdded = false;
    for (int c = 0; c < width; ++c) {
        for (int r = 0; r < height; ++r) {
            if (!grid[r][c]) {
                createRandomGem(r, c);
                grid[r][c]->setGridPosition(c, r);
                newGemsAdded = true;
            }
            else {
                break;
            }
        }
    }
    return newGemsAdded;
}

void Board::trySpawnBonus(const std::vector<std::pair<GridCoord, int>>& destroyedGemsInfo) {
    for (const auto& info : destroyedGemsInfo) {
        if (bonusChanceDist(rng) <= BONUS_CHANCE) {
            int bonusType = bonusTypeDist(rng);
            GridCoord bonusOrigin = info.first;
            int bonusOriginColorIndex = info.second;
            int targetR, targetC;
            int attempts = 0;
            const int maxAttempts = 20;
            do {
                int dr = rng() % (2 * REPAINT_BONUS_RADIUS + 1) - REPAINT_BONUS_RADIUS;
                int dc = rng() % (2 * REPAINT_BONUS_RADIUS + 1) - REPAINT_BONUS_RADIUS;
                targetR = bonusOrigin.y + dr;
                targetC = bonusOrigin.x + dc;
                attempts++;
            } while (!isValidCoords(targetR, targetC) && attempts < maxAttempts);

            if (isValidCoords(targetR, targetC)) {
                GridCoord targetPos = { targetC, targetR };
                if (bonusType == 0) {
                    applyRepaintBonus(targetPos, bonusOriginColorIndex);
                }
                else {
                    applyBombBonus(targetPos);
                }
            }
        }
    }
}

void Board::applyRepaintBonus(GridCoord targetPos, int sourceColorIndex) {
    if (!isValidCoords(targetPos) || sourceColorIndex == -1) return;

    if (grid[targetPos.y][targetPos.x]) {
        std::unique_ptr<GemColorBase> newColor;
        switch (sourceColorIndex) {
        case 0: newColor = std::make_unique<RedGemColor>(); break;
        case 1: newColor = std::make_unique<GreenGemColor>(); break;
        case 2: newColor = std::make_unique<BlueGemColor>(); break;
        case 3: newColor = std::make_unique<YellowGemColor>(); break;
        case 4: newColor = std::make_unique<PurpleGemColor>(); break;
        case 5: newColor = std::make_unique<OrangeGemColor>(); break;
        default: newColor = std::make_unique<NoneGemColor>(); break;
        }
        grid[targetPos.y][targetPos.x]->setColor(std::move(newColor));
    }

    std::vector<GridCoord> candidates;
    for (int dr = -REPAINT_BONUS_RADIUS; dr <= REPAINT_BONUS_RADIUS; ++dr) {
        for (int dc = -REPAINT_BONUS_RADIUS; dc <= REPAINT_BONUS_RADIUS; ++dc) {
            GridCoord candidatePos(targetPos.x + dc, targetPos.y + dr);
            if (isValidCoords(candidatePos) &&
                grid[candidatePos.y][candidatePos.x] &&
                !areAdjacent(targetPos, candidatePos)) {
                candidates.push_back(candidatePos);
            }
        }
    }

    std::shuffle(candidates.begin(), candidates.end(), rng);
    int count = 0;
    for (const auto& pos : candidates) {
        if (count >= 2) break;
        std::unique_ptr<GemColorBase> newColor;
        switch (sourceColorIndex) {
        case 0: newColor = std::make_unique<RedGemColor>(); break;
        case 1: newColor = std::make_unique<GreenGemColor>(); break;
        case 2: newColor = std::make_unique<BlueGemColor>(); break;
        case 3: newColor = std::make_unique<YellowGemColor>(); break;
        case 4: newColor = std::make_unique<PurpleGemColor>(); break;
        case 5: newColor = std::make_unique<OrangeGemColor>(); break;
        default: newColor = std::make_unique<NoneGemColor>(); break;
        }
        grid[pos.y][pos.x]->setColor(std::move(newColor));
        count++;
    }
}

void Board::applyBombBonus(GridCoord targetPos) {
    std::set<GridCoord> gemsToDestroy;

    if (isValidCoords(targetPos) && grid[targetPos.y][targetPos.x]) {
        gemsToDestroy.insert(targetPos);
    }

    std::vector<GridCoord> allGems;
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            if (grid[r][c] && !gemsToDestroy.count({ c, r })) {
                allGems.push_back({ c, r });
            }
        }
    }

    std::shuffle(allGems.begin(), allGems.end(), rng);
    int needed = BOMB_BONUS_COUNT - gemsToDestroy.size();
    for (int i = 0; i < needed && i < allGems.size(); ++i) {
        gemsToDestroy.insert(allGems[i]);
    }

    if (!gemsToDestroy.empty()) {
        destroyGems(gemsToDestroy);
    }
}

bool Board::hasPossibleMoves() const {
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            if (!grid[r][c]) continue;

            const GemColorBase* current = grid[r][c]->getColor();
            if (!current || current->getIndex() == -1) continue;
            if (c + 1 < width && grid[r][c + 1] && grid[r][c + 1]->getColor() && grid[r][c + 1]->getColor()->getIndex() != -1) {
                const GemColorBase* right = grid[r][c + 1]->getColor();
                if (c + 2 < width && grid[r][c + 2] && grid[r][c + 2]->getColor() && grid[r][c + 2]->getColor()->getIndex() == current->getIndex()) return true;
                if (r - 1 >= 0 && r + 1 < height && grid[r - 1][c + 1] && grid[r - 1][c + 1]->getColor() && grid[r - 1][c + 1]->getColor()->getIndex() == current->getIndex() &&
                    grid[r + 1][c + 1] && grid[r + 1][c + 1]->getColor() && grid[r + 1][c + 1]->getColor()->getIndex() == current->getIndex()) return true;
                if (r + 2 < height && grid[r + 1][c + 1] && grid[r + 1][c + 1]->getColor() && grid[r + 1][c + 1]->getColor()->getIndex() == current->getIndex() &&
                    grid[r + 2][c + 1] && grid[r + 2][c + 1]->getColor() && grid[r + 2][c + 1]->getColor()->getIndex() == current->getIndex()) return true;

                if (r - 2 >= 0 && grid[r - 1][c + 1] && grid[r - 1][c + 1]->getColor() && grid[r - 1][c + 1]->getColor()->getIndex() == current->getIndex() &&
                    grid[r - 2][c + 1] && grid[r - 2][c + 1]->getColor() && grid[r - 2][c + 1]->getColor()->getIndex() == current->getIndex()) return true;
                if (c - 1 >= 0 && grid[r][c - 1] && grid[r][c - 1]->getColor() && grid[r][c - 1]->getColor()->getIndex() == right->getIndex()) return true;
                if (r - 1 >= 0 && r + 1 < height && grid[r - 1][c] && grid[r - 1][c]->getColor() && grid[r - 1][c]->getColor()->getIndex() == right->getIndex() &&
                    grid[r + 1][c] && grid[r + 1][c]->getColor() && grid[r + 1][c]->getColor()->getIndex() == right->getIndex()) return true;
                if (r + 2 < height && grid[r + 1][c] && grid[r + 1][c]->getColor() && grid[r + 1][c]->getColor()->getIndex() == right->getIndex() &&
                    grid[r + 2][c] && grid[r + 2][c]->getColor() && grid[r + 2][c]->getColor()->getIndex() == right->getIndex()) return true;

                if (r - 2 >= 0 && grid[r - 1][c] && grid[r - 1][c]->getColor() && grid[r - 1][c]->getColor()->getIndex() == right->getIndex() &&
                    grid[r - 2][c] && grid[r - 2][c]->getColor() && grid[r - 2][c]->getColor()->getIndex() == right->getIndex()) return true;
            }
            if (r + 1 < height && grid[r + 1][c] && grid[r + 1][c]->getColor() && grid[r + 1][c]->getColor()->getIndex() != -1) {
                const GemColorBase* down = grid[r + 1][c]->getColor();
                if (r + 2 < height && grid[r + 2][c] && grid[r + 2][c]->getColor() && grid[r + 2][c]->getColor()->getIndex() == current->getIndex()) return true;
                if (c - 1 >= 0 && c + 1 < width && grid[r + 1][c - 1] && grid[r + 1][c - 1]->getColor() && grid[r + 1][c - 1]->getColor()->getIndex() == current->getIndex() &&
                    grid[r + 1][c + 1] && grid[r + 1][c + 1]->getColor() && grid[r + 1][c + 1]->getColor()->getIndex() == current->getIndex()) return true;
                if (c + 2 < width && grid[r + 1][c + 1] && grid[r + 1][c + 1]->getColor() && grid[r + 1][c + 1]->getColor()->getIndex() == current->getIndex() &&
                    grid[r + 1][c + 2] && grid[r + 1][c + 2]->getColor() && grid[r + 1][c + 2]->getColor()->getIndex() == current->getIndex()) return true;

                if (c - 2 >= 0 && grid[r + 1][c - 1] && grid[r + 1][c - 1]->getColor() && grid[r + 1][c - 1]->getColor()->getIndex() == current->getIndex() &&
                    grid[r + 1][c - 2] && grid[r + 1][c - 2]->getColor() && grid[r + 1][c - 2]->getColor()->getIndex() == current->getIndex()) return true;
                if (r - 1 >= 0 && grid[r - 1][c] && grid[r - 1][c]->getColor() && grid[r - 1][c]->getColor()->getIndex() == down->getIndex()) return true;
                if (c - 1 >= 0 && c + 1 < width && grid[r][c - 1] && grid[r][c - 1]->getColor() && grid[r][c - 1]->getColor()->getIndex() == down->getIndex() &&
                    grid[r][c + 1] && grid[r][c + 1]->getColor() && grid[r][c + 1]->getColor()->getIndex() == down->getIndex()) return true;
                if (c + 2 < width && grid[r][c + 1] && grid[r][c + 1]->getColor() && grid[r][c + 1]->getColor()->getIndex() == down->getIndex() &&
                    grid[r][c + 2] && grid[r][c + 2]->getColor() && grid[r][c + 2]->getColor()->getIndex() == down->getIndex()) return true;

                if (c - 2 >= 0 && grid[r][c - 1] && grid[r][c - 1]->getColor() && grid[r][c - 1]->getColor()->getIndex() == down->getIndex() &&
                    grid[r][c - 2] && grid[r][c - 2]->getColor() && grid[r][c - 2]->getColor()->getIndex() == down->getIndex()) return true;
            }
        }
    }
    return false;
}
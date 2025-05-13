#include "Board.h"
#include <vector>
#include <set>
#include <algorithm>
#include <chrono>
#include <memory>
#include <iostream>

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
    needsMatchCheck(true),
    needsGravityCheck(false),
    needsRefillCheck(false),
    needsBonusProcessing(false)
{
    grid.resize(height);
    for (int r = 0; r < height; ++r) {
        grid[r].resize(width);
    }

    initializeBoard();
}
void Board::initializeBoard() {
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            do {
                createRandomGem(r, c);
            } while (
                (c >= 2 &&
                    grid[r][c]->getColor() == grid[r][c - 1]->getColor() &&
                    grid[r][c]->getColor() == grid[r][c - 2]->getColor())
                ||
                (r >= 2 &&
                    grid[r][c]->getColor() == grid[r - 1][c]->getColor() &&
                    grid[r][c]->getColor() == grid[r - 2][c]->getColor())
                );
        }
    }
    needsMatchCheck = false;
    needsGravityCheck = false;
    needsRefillCheck = false;
    needsBonusProcessing = false;

}
void Board::createGem(int r, int c, GemColor color) {
    if (isValidCoords(r, c)) {
        grid[r][c] = std::make_unique<Gem>(color, c, r, textureSheetRef);
    }
}
void Board::createRandomGem(int r, int c) {
    if (isValidCoords(r, c)) {
        GemColor randomColor = static_cast<GemColor>(gemColorDist(rng));
        grid[r][c] = std::make_unique<Gem>(randomColor, c, r, textureSheetRef);
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
    bool changed = false;

    if (needsBonusProcessing) {
        trySpawnBonus(recentlyDestroyedGems);
        recentlyDestroyedGems.clear();
        needsBonusProcessing = false;
        needsMatchCheck = true;
        changed = true;
    }
    else if (needsMatchCheck) {
        std::set<GridCoord> matches = findMatches();
        if (!matches.empty()) {
            auto destructionResult = destroyGems(matches);
            int destroyedCount = destructionResult.first;
            recentlyDestroyedGems = destructionResult.second;

            if (destroyedCount > 0) {
                needsGravityCheck = true;
                needsBonusProcessing = true;
                changed = true;
            }
            needsMatchCheck = false;
        }
        else {
            needsMatchCheck = false;
        }
    }
    else if (needsGravityCheck) {
        if (applyGravity()) {
            needsRefillCheck = true;
            changed = true;
        }
        needsGravityCheck = false;
    }
    else if (needsRefillCheck) {
        if (refillBoard()) {
            needsMatchCheck = true;
            changed = true;
        }
        needsRefillCheck = false;
    }

    return changed;
}


bool Board::handleMouseClick(sf::Vector2i pixelCoords) {
    if (!isIdle()) return false;

    int gridX = pixelCoords.x / GEM_SIZE;
    int gridY = pixelCoords.y / GEM_SIZE;
    GridCoord clickedCoords(gridX, gridY);

    if (!isValidCoords(clickedCoords)) {
        gemSelected = false;
        return false;
    }

    if (!gemSelected) {
        selectedGemCoords = clickedCoords;
        gemSelected = true;
        return true;
    }
    else {
        GridCoord secondClickCoords = clickedCoords;

        if (secondClickCoords == selectedGemCoords) {
            gemSelected = false;
            return true;
        }

        if (areAdjacent(selectedGemCoords, secondClickCoords)) {
            swapGems(selectedGemCoords, secondClickCoords);
            std::set<GridCoord> matchesAfterSwap = findMatches();

            if (matchesAfterSwap.empty()) {
                swapGems(selectedGemCoords, secondClickCoords);
                gemSelected = false;
                return false;
            }
            else {
                needsMatchCheck = true;
                gemSelected = false;
                return true;
            }
        }
        else {
            selectedGemCoords = secondClickCoords;
            gemSelected = true;
            return true;
        }
    }
}
bool Board::isIdle() const {
    return !needsMatchCheck && !needsGravityCheck && !needsRefillCheck && !needsBonusProcessing;
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
        GemColor currentColor = GemColor::None;
        for (int c = 0; c < width; ++c) {
            if (grid[r][c] && grid[r][c]->getColor() == currentColor) {
                consecutive++;
            }
            else {
                if (consecutive >= 3) {
                    for (int i = c - consecutive; i < c; ++i) {
                        matches.insert({ i, r });
                    }
                }
                currentColor = grid[r][c] ? grid[r][c]->getColor() : GemColor::None;
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
        GemColor currentColor = GemColor::None;
        for (int r = 0; r < height; ++r) {
            if (grid[r][c] && grid[r][c]->getColor() == currentColor) {
                consecutive++;
            }
            else {
                if (consecutive >= 3) {
                    for (int i = r - consecutive; i < r; ++i) {
                        matches.insert({ c, i });
                    }
                }
                currentColor = grid[r][c] ? grid[r][c]->getColor() : GemColor::None;
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
std::pair<int, std::vector<std::pair<GridCoord, GemColor>>> Board::destroyGems(const std::set<GridCoord>& coordsToDestroy) {
    int count = 0;
    std::vector<std::pair<GridCoord, GemColor>> destroyedInfo;

    for (const auto& coord : coordsToDestroy) {
        if (isValidCoords(coord) && grid[coord.y][coord.x]) {
            destroyedInfo.push_back({ coord, grid[coord.y][coord.x]->getColor() });
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
void Board::trySpawnBonus(const std::vector<std::pair<GridCoord, GemColor>>& destroyedGemsInfo) {
    for (const auto& info : destroyedGemsInfo) {
        if (bonusChanceDist(rng) <= BONUS_CHANCE) {
            int bonusType = bonusTypeDist(rng);
            GridCoord bonusOrigin = info.first;
            GemColor bonusOriginColor = info.second;
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
                    applyRepaintBonus(targetPos, bonusOriginColor);
                }
                else {
                    applyBombBonus(targetPos);
                }
                needsMatchCheck = true;
            }
        }
    }
}
void Board::applyRepaintBonus(GridCoord targetPos, GemColor sourceColor) {
    if (!isValidCoords(targetPos) || sourceColor == GemColor::None) return;

    if (grid[targetPos.y][targetPos.x]) {
        grid[targetPos.y][targetPos.x]->setColor(sourceColor);
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
        grid[pos.y][pos.x]->setColor(sourceColor);
        count++;
    }

    needsMatchCheck = true;
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
        needsGravityCheck = true;
    }
}
bool Board::hasPossibleMoves() const {
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            if (!grid[r][c]) continue;

            GemColor current = grid[r][c]->getColor();
            if (c + 1 < width && grid[r][c + 1]) {
                GemColor right = grid[r][c + 1]->getColor();
                if (c + 2 < width && grid[r][c + 2] && grid[r][c + 2]->getColor() == current) return true;
                if (r - 1 >= 0 && r + 1 < height && grid[r - 1][c + 1] && grid[r - 1][c + 1]->getColor() == current && grid[r + 1][c + 1] && grid[r + 1][c + 1]->getColor() == current) return true;
                if (r + 2 < height && grid[r + 1][c + 1] && grid[r + 1][c + 1]->getColor() == current && grid[r + 2][c + 1] && grid[r + 2][c + 1]->getColor() == current) return true;
                if (r - 2 >= 0 && grid[r - 1][c + 1] && grid[r - 1][c + 1]->getColor() == current && grid[r - 2][c + 1] && grid[r - 2][c + 1]->getColor() == current) return true;
                if (c - 1 >= 0 && grid[r][c - 1] && grid[r][c - 1]->getColor() == right) return true;
                if (r - 1 >= 0 && r + 1 < height && grid[r - 1][c] && grid[r - 1][c]->getColor() == right && grid[r + 1][c] && grid[r + 1][c]->getColor() == right) return true;
                if (r + 2 < height && grid[r + 1][c] && grid[r + 1][c]->getColor() == right && grid[r + 2][c] && grid[r + 2][c]->getColor() == right) return true;
                if (r - 2 >= 0 && grid[r - 1][c] && grid[r - 1][c]->getColor() == right && grid[r - 2][c] && grid[r - 2][c]->getColor() == right) return true;

            }
            if (r + 1 < height && grid[r + 1][c]) {
                GemColor down = grid[r + 1][c]->getColor();
                if (r + 2 < height && grid[r + 2][c] && grid[r + 2][c]->getColor() == current) return true;
                if (c - 1 >= 0 && c + 1 < width && grid[r + 1][c - 1] && grid[r + 1][c - 1]->getColor() == current && grid[r + 1][c + 1] && grid[r + 1][c + 1]->getColor() == current) return true;
                if (c + 2 < width && grid[r + 1][c + 1] && grid[r + 1][c + 1]->getColor() == current && grid[r + 1][c + 2] && grid[r + 1][c + 2]->getColor() == current) return true;
                if (c - 2 >= 0 && grid[r + 1][c - 1] && grid[r + 1][c - 1]->getColor() == current && grid[r + 1][c - 2] && grid[r + 1][c - 2]->getColor() == current) return true;

                if (r - 1 >= 0 && grid[r - 1][c] && grid[r - 1][c]->getColor() == down) return true;
                if (c - 1 >= 0 && c + 1 < width && grid[r][c - 1] && grid[r][c - 1]->getColor() == down && grid[r][c + 1] && grid[r][c + 1]->getColor() == down) return true;
                if (c + 2 < width && grid[r][c + 1] && grid[r][c + 1]->getColor() == down && grid[r][c + 2] && grid[r][c + 2]->getColor() == down) return true;
                if (c - 2 >= 0 && grid[r][c - 1] && grid[r][c - 1]->getColor() == down && grid[r][c - 2] && grid[r][c - 2]->getColor() == down) return true;
            }
        }
    }
    return false;
}
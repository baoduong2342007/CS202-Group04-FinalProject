/**
 * @file TileMap.cpp
 * @author TV4 (Vy)
 * @brief Implementation of TileMap loading, rendering, queries, and physics
 * @note Sprint 4 - validates level data and renders tiles from a tileset
 */

#include "level/TileMap.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <string_view>
#include <utility>

#include <box2d/box2d.h>
#include <SFML/System/Exception.hpp>

#include "physics/PhysicsEngine.h"
#include "level/TileCollisionSpans.h"
#include "level/TileSemantics.h"
#include "entities/Entity.h"
#include "entities/Enemy.h"
#include "entities/BlockDebris.h"
#include "entities/Mario.h"
#include "entities/QuestionBlock.h"
#include "core/SpriteFrames_ovw.h"
#include "core/SpriteFrames_udg.h"
#include "core/SpriteFrames_castle.h"
#include "core/SpriteFrames_udw.h"
#include "core/SpriteFrames_shared.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "level/TileFrames.h"
#include "core/LevelCatalog.h"
#include "physics/CollisionManager.h"

namespace {

constexpr std::string_view VALID_TILE_SYMBOLS = ".0123456789B?CGKMFS|RUEOfhuoecx[]{}prJTLHV^~mWbkydqlPDtsn=XAN";
constexpr float TILE_SIZE_PIXELS = 32.f;
constexpr float TILE_FRICTION = 0.6f;
constexpr float FLAG_WAVE_SPEED = 7.0f;
constexpr float FLAG_WAVE_AMPLITUDE = 1.5f;
constexpr float FLAG_WAVE_PHASE_OFFSET = 0.75f;
constexpr float TWO_PI = 6.28318530718f;

bool isSolidTileSymbol(char tile) {
    return (tile == '0' || tile == '1') || tile == 'B' || tile == 'E' || tile == 'S' ||
           tile == '[' || tile == ']' ||
           tile == 'p' || tile == 'r' || tile == 'q' ||
           tile == '{' || tile == '}' ||
           tile == '=';
}

bool isForegroundTile(char symbol) {
    return symbol == 'B' || symbol == '?' ||
           symbol == 'U' || symbol == 'O' ||
           symbol == 'F' || symbol == 'T' ||
           symbol == '|' || symbol == 'H' ||
           symbol == '[' || symbol == ']' ||
           symbol == 'p' || symbol == 'r' || symbol == 'q' ||
           symbol == '{' || symbol == '}';
}

bool isEnemySupportTileSymbol(char tile) {
    return isSolidTileSymbol(tile) || tile == '?' ||
           tile == 'U' || tile == 'u' ||
           tile == 'f' || tile == 'h' ||
           tile == 'O' || tile == 'o';
}

int worldToGridCoordinate(float coordinate) {
    return static_cast<int>(std::lround(coordinate / TILE_SIZE_PIXELS));
}

QuestionBlock* findQuestionBlockAt(std::vector<std::unique_ptr<Entity>>& entities,
                                   int column,
                                   int row) {
    for (auto& entity : entities) {
        if (!entity || !entity->isQuestionBlock() || !entity->isActive() || entity->isPendingDestroy()) {
            continue;
        }

        sf::Vector2f position = entity->getPosition();
        if (worldToGridCoordinate(position.x) == column && worldToGridCoordinate(position.y) == row) {
            return static_cast<QuestionBlock*>(entity.get());
        }
    }

    return nullptr;
}

void defeatEnemiesAboveBlock(
    std::vector<std::unique_ptr<Entity>>& entities,
    const sf::Vector2f& blockPosition,
    const sf::Vector2f& blockSize,
    Mario* mario) {
    if (!mario || blockSize.x <= 0.f || blockSize.y <= 0.f) {
        return;
    }

    constexpr float DIRECT_SUPPORT_EPSILON_PIXELS = 8.f;
    const float blockLeft = blockPosition.x;
    const float blockRight = blockPosition.x + blockSize.x;
    const float blockTop = blockPosition.y;

    for (const auto& entity : entities) {
        if (!entity || !entity->isEnemy()) {
            continue;
        }

        Enemy* enemy = static_cast<Enemy*>(entity.get());
        b2Body* enemyBody = enemy->getBody();
        if (enemy->isDying() || enemy->shouldRemove() ||
            enemy->isPendingDestroy() || !enemy->isActive() ||
            !enemyBody || !enemyBody->IsEnabled()) {
            continue;
        }

        // Entity render positions can lag one frame behind the post-Step
        // physics state. Read the Box2D center and convert it once to pixels.
        const sf::Vector2f enemyCenter =
            PhysicsEngine::metersToPixels(enemyBody->GetPosition());
        const sf::Vector2f enemySize = enemy->getSize();
        const float enemyLeft = enemyCenter.x - enemySize.x / 2.f;
        const float enemyRight = enemyCenter.x + enemySize.x / 2.f;
        const float enemyBottom = enemyCenter.y + enemySize.y / 2.f;
        const float horizontalOverlap =
            std::min(enemyRight, blockRight) - std::max(enemyLeft, blockLeft);

        if (horizontalOverlap <= 0.f ||
            std::abs(enemyBottom - blockTop) > DIRECT_SUPPORT_EPSILON_PIXELS) {
            continue;
        }

        CollisionManager::defeatEnemy(*enemy, DefeatCause::BLOCK_BUMP, mario);
    }
}

bool isHorizontalPipeFootprint(const std::vector<std::string>& grid,
                               int column, int row) {
    if (column < 0 || row < 0) {
        return false;
    }

    // H is the bottom-left anchor of a 3x2 pipe.
    //
    //     XXX
    //     HXX
    //
    // Therefore a queried cell can belong to an H whose anchor is:
    // - on this row or one row below it
    // - this column, one column left, or two columns left
    for (int anchorRow = row; anchorRow <= row + 1; ++anchorRow) {
        if (anchorRow < 0 || anchorRow >= static_cast<int>(grid.size())) {
            continue;
        }

        for (int anchorColumn = column - 2; anchorColumn <= column; ++anchorColumn) {

            if (anchorColumn < 0 || anchorColumn >= static_cast<int>(grid[anchorRow].size())) {
                continue;
            }

            if (grid[anchorRow][anchorColumn] == 'H') {
                return true;
            }
        }
    }

    return false;
}

} // namespace

void TileMap::queueTileHit(int column, int row, float overlap) {
    for (auto& pendingHit : m_pendingTileHits) {
        if (pendingHit.gridPosition.x == column && pendingHit.gridPosition.y == row) {
            pendingHit.overlap = std::max(pendingHit.overlap, overlap);
            return;
        }
    }

    m_pendingTileHits.push_back({{column, row}, overlap});
}

void TileMap::removeTile(int column, int row) {
    if (row < 0 || row >= static_cast<int>(m_grid.size())) {
        return;
    }
    if (column < 0 || column >= static_cast<int>(m_grid[row].size())) {
        return;
    }
    if (m_grid[row][column] == '.') {
        return;
    }

    m_grid[row][column] = '.';
    buildVertices();

    if (m_physicsWorld && !m_physicsWorld->IsLocked()) {
        // One tile may live inside a merged span, so rebuild the static
        // terrain from the updated grid instead of destroying one body.
        createPhysicsBodies(m_physicsWorld);
    }
}

void TileMap::processPendingHits(std::vector<std::unique_ptr<Entity>>& entities, TextureManager& textureManager, bool isBigMario, Mario* mario) {
    if (m_pendingTileHits.empty()) {
        return;
    }

    PendingTileHit bestHit = m_pendingTileHits.front();
    for (const auto& pendingHit : m_pendingTileHits) {
        if (pendingHit.overlap > bestHit.overlap) {
            bestHit = pendingHit;
        }
    }

    const sf::Vector2i bestGridPos = bestHit.gridPosition;
    if (mario) {
        if (QuestionBlock* block = findQuestionBlockAt(entities, bestGridPos.x, bestGridPos.y)) {
            defeatEnemiesAboveBlock(entities,
                                    block->getPosition(),
                                    block->getSize(),
                                    mario);
            block->onHit(*mario, &entities, &textureManager);
            m_pendingTileHits.clear();
            return;
        }
    }

    if (TileSemantics::isBreakable(
            getTileAt(bestGridPos.x, bestGridPos.y))) {
        defeatEnemiesAboveBlock(
            entities,
            gridToWorldPosition(bestGridPos),
            {TILE_SIZE_PIXELS, TILE_SIZE_PIXELS},
            mario);
    }

    hitTile(bestGridPos.x, bestGridPos.y,
            isBigMario, entities, &textureManager);

    m_pendingTileHits.clear();
}

namespace {

bool isBlankLine(const std::string& line) {
    return line.find_first_not_of(" \t") == std::string::npos;
}

bool isCommentLine(const std::string& line) {
    const std::size_t firstCharacter = line.find_first_not_of(" \t");

    return firstCharacter != std::string::npos && line[firstCharacter] == '#';
}

bool isValidTileSymbol(char symbol) {
    return VALID_TILE_SYMBOLS.find(symbol) != std::string_view::npos;
}

bool isRenderableTile(char symbol) {
    return (symbol == '0' || symbol == '1') || symbol == 'B' ||
           symbol == 'F' || symbol == 'T' ||
           symbol == 'L' || symbol == 'H' ||
           symbol == 'S' || symbol == '|' || symbol == 'E' ||
           symbol == '[' || symbol == ']' ||
           symbol == 'p' || symbol == 'r' || symbol == 'q' ||
           symbol == '{' || symbol == '}' ||
           symbol == '=';
}

bool isWarpId(char symbol) {
    return symbol >= '1' && symbol <= '9';
}

bool parseAndNormalizeWarps(std::vector<std::string>& grid,
                            std::vector<TileMap::WarpEntry>& entries,
                            std::vector<TileMap::WarpReturn>& returns,
                            const std::string& path
                            ) {
    entries.clear();
    returns.clear();

    const auto hasEntry = [&entries](char id) {
        return std::any_of(entries.begin(), entries.end(),
                           [id](const TileMap::WarpEntry& entry) {
                                return entry.id == id;
                           }
                           );
    };

    const auto hasReturn = [&returns](char id) {
        return std::any_of(returns.begin(), returns.end(),
                           [id](const TileMap::WarpReturn& destination) {
                                return destination.id == id;
                           }
                           );
    };

    for (std::size_t row = 0; row < grid.size(); ++row) {
        for (std::size_t column = 0; column < grid[row].size(); ++column) {
            const char symbol = grid[row][column];

            // --------------------------------------------
            // R<id> = invisible return/spawn point
            // --------------------------------------------
            if (symbol == 'R') {
                if (column + 1 >= grid[row].size() ||
                    !isWarpId(grid[row][column + 1])) {
                    std::cerr << "Warp return R must be followed by ID 1-9 in " << path << std::endl;
                    return false;
                }

                const char id = grid[row][column + 1];

                if (hasReturn(id)) {
                    std::cerr << "Duplicate warp return R" << id << " in " << path << std::endl;
                    return false;
                }

                returns.push_back( { id, {static_cast<int>(column), static_cast<int>(row)} } );

                // Rn is metadata only.
                grid[row][column] = '.';
                grid[row][column + 1] = '.';

                ++column;
                continue;
            }

            // Only H, [ and p may act as warp entries.
            if (symbol != 'H' && symbol != '[' && symbol != 'p') {
                continue;
            }

            // H / [] / pr without index remain normal objects.
            if (column + 1 >= grid[row].size() || !isWarpId(grid[row][column + 1])) {
                continue;
            }

            const char id = grid[row][column + 1];

            if (hasEntry(id)) {
                std::cerr << "Duplicate warp entry " << id << " in " << path << std::endl;
                return false;
            }

            TileMap::WarpEntryType type;

            if (symbol == 'H') {
                type = TileMap::WarpEntryType::HORIZONTAL;

                // H already renders/collides as a composite 3x2 pipe.
                grid[row][column + 1] = '.';
            } else if (symbol == '[') {
                type = TileMap::WarpEntryType::VERTICAL;

                // [2 -> [] at runtime.
                grid[row][column + 1] = ']';
            } else {
                type = TileMap::WarpEntryType::PIRANHA;

                // p2 -> pr at runtime.
                grid[row][column + 1] = 'r';
            }

            entries.push_back( { id, {static_cast<int>(column), static_cast<int>(row)}, type } );
            ++column;
        }
    }

    // After normalization no number or R may remain.
    for (std::size_t row = 0; row < grid.size(); ++row) {
        for (std::size_t column = 0; column < grid[row].size(); ++column) {
            const char symbol = grid[row][column];

            // `1` is also the legacy ground symbol used by TV5's existing
            // levels, so only 2-9 are ambiguous orphan warp IDs.
            if (symbol == 'R' || (isWarpId(symbol) && symbol != '1')) {
                std::cerr << "Orphan warp metadata '" << symbol << "' at row " << row + 1 << ", column " << column + 1 << " in " << path << std::endl;

                return false;
            }
        }
    }

    // Every entry needs one return.
    for (const auto& entry : entries) {
        if (!hasReturn(entry.id)) {
            std::cerr << "Warp entry " << entry.id << " has no matching R" << entry.id << " in " << path << std::endl;

            return false;
        }
    }

    // Every return needs one entry.
    for (const auto& destination : returns) {
        if (!hasEntry(destination.id)) {
            std::cerr << "Warp return R" << destination.id << " has no matching entry in " << path << std::endl;

            return false;
        }
    }

    return true;
}

// Parses '^' (start) and '~' (end) elevator markers from the grid.
// A route is VERTICAL when the end marker shares the start's column,
// HORIZONTAL when it shares the start's row.  When both apply, the
// vertical route wins.  When several end markers are visible along the
// route axis, the nearest one is paired with the start.
bool parseAndNormalizeElevators(std::vector<std::string>& grid,
                                std::vector<TileMap::ElevatorRoute>& routes,
                                const std::string& path) {
    routes.clear();

    std::vector<sf::Vector2i> starts;
    for (std::size_t row = 0; row < grid.size(); ++row) {
        for (std::size_t column = 0; column < grid[row].size(); ++column) {
            if (grid[row][column] == '^') {
                starts.push_back({static_cast<int>(column), static_cast<int>(row)});
            }
        }
    }

    std::vector<sf::Vector2i> consumedEnds;

    const auto isConsumedEnd = [&consumedEnds](int column, int row) {
        return std::any_of(consumedEnds.begin(), consumedEnds.end(),
                           [column, row](const sf::Vector2i& end) {
                               return end.x == column && end.y == row;
                           });
    };

    for (const sf::Vector2i& start : starts) {
        int endColumn = -1;
        int endRow = -1;

        // Vertical candidates: unconsumed '~' markers in the start column.
        {
            int bestDistance = std::numeric_limits<int>::max();
            for (std::size_t row = 0; row < grid.size(); ++row) {
                if (grid[row][start.x] != '~' || isConsumedEnd(start.x, static_cast<int>(row))) {
                    continue;
                }
                const int distance = std::abs(static_cast<int>(row) - start.y);
                if (distance < bestDistance) {
                    bestDistance = distance;
                    endColumn = start.x;
                    endRow = static_cast<int>(row);
                }
            }
        }

        // Horizontal candidates: unconsumed '~' markers in the start row.
        if (endColumn < 0) {
            int bestDistance = std::numeric_limits<int>::max();
            for (std::size_t column = 0; column < grid[start.y].size(); ++column) {
                if (grid[start.y][column] != '~' || isConsumedEnd(static_cast<int>(column), start.y)) {
                    continue;
                }
                const int distance = std::abs(static_cast<int>(column) - start.x);
                if (distance < bestDistance) {
                    bestDistance = distance;
                    endColumn = static_cast<int>(column);
                    endRow = start.y;
                }
            }
        }

        if (endColumn < 0) {
            std::cerr << "Elevator start '^' at row " << start.y + 1
                      << ", column " << start.x + 1
                      << " has no matching '~' end marker in " << path << std::endl;
            return false;
        }

        if (endColumn == start.x && endRow == start.y) {
            std::cerr << "Elevator route must span at least one tile in " << path << std::endl;
            return false;
        }

        consumedEnds.push_back({endColumn, endRow});
        routes.push_back({start, {endColumn, endRow}, endColumn == start.x});
    }

    // Every '~' must belong to a start.
    for (std::size_t row = 0; row < grid.size(); ++row) {
        for (std::size_t column = 0; column < grid[row].size(); ++column) {
            if (grid[row][column] == '~' && !isConsumedEnd(static_cast<int>(column), static_cast<int>(row))) {
                std::cerr << "Orphan elevator end marker '~' at row " << row + 1
                          << ", column " << column + 1 << " in " << path << std::endl;
                return false;
            }
        }
    }

    // Markers are metadata only; strip them from the renderable grid.
    for (const sf::Vector2i& start : starts) {
        grid[start.y][start.x] = '.';
    }
    for (const sf::Vector2i& end : consumedEnds) {
        grid[end.y][end.x] = '.';
    }

    return true;
}

// Parses pairs of 'c' (start) and 'x' (end) route markers for flying/swimming Cheep Cheeps.
// If 'c' has a matching 'x' in the same column (vertical) or same row (horizontal),
// a route is created and both markers are converted to '.'. If 'c' has no matching 'x',
// it stays as 'c' for single-point spawning.
bool parseAndNormalizeCheepCheeps(std::vector<std::string>& grid,
                                  std::vector<TileMap::CheepCheepRoute>& routes,
                                  const std::string& path) {
    routes.clear();

    std::vector<sf::Vector2i> starts;
    for (std::size_t row = 0; row < grid.size(); ++row) {
        for (std::size_t column = 0; column < grid[row].size(); ++column) {
            if (grid[row][column] == 'c') {
                starts.push_back({static_cast<int>(column), static_cast<int>(row)});
            }
        }
    }

    std::vector<sf::Vector2i> consumedEnds;

    const auto isConsumedEnd = [&consumedEnds](int column, int row) {
        return std::any_of(consumedEnds.begin(), consumedEnds.end(),
                           [column, row](const sf::Vector2i& end) {
                               return end.x == column && end.y == row;
                           });
    };

    for (const sf::Vector2i& start : starts) {
        int endColumn = -1;
        int endRow = -1;

        // Vertical candidates: unconsumed 'x' markers in the start column.
        {
            int bestDistance = std::numeric_limits<int>::max();
            for (std::size_t row = 0; row < grid.size(); ++row) {
                if (grid[row][start.x] != 'x' || isConsumedEnd(start.x, static_cast<int>(row))) {
                    continue;
                }
                const int distance = std::abs(static_cast<int>(row) - start.y);
                if (distance < bestDistance) {
                    bestDistance = distance;
                    endColumn = start.x;
                    endRow = static_cast<int>(row);
                }
            }
        }

        // Horizontal candidates: unconsumed 'x' markers in the start row.
        if (endColumn < 0) {
            int bestDistance = std::numeric_limits<int>::max();
            for (std::size_t column = 0; column < grid[start.y].size(); ++column) {
                if (grid[start.y][column] != 'x' || isConsumedEnd(static_cast<int>(column), start.y)) {
                    continue;
                }
                const int distance = std::abs(static_cast<int>(column) - start.x);
                if (distance < bestDistance) {
                    bestDistance = distance;
                    endColumn = static_cast<int>(column);
                    endRow = start.y;
                }
            }
        }

        // If a matching 'x' was found, this 'c' is the start of a route!
        if (endColumn >= 0) {
            consumedEnds.push_back({endColumn, endRow});
            routes.push_back({start, {endColumn, endRow}, endColumn == start.x});

            // Erase both markers from grid so they don't block physics or render as solid
            grid[start.y][start.x] = '.';
            grid[endRow][endColumn] = '.';
        }
        // Otherwise, if no 'x' was found, 'c' remains in the grid as a standalone swimming fish.
    }

    // Every 'x' must have belonged to a 'c'. Check for orphan 'x' markers.
    for (std::size_t row = 0; row < grid.size(); ++row) {
        for (std::size_t column = 0; column < grid[row].size(); ++column) {
            if (grid[row][column] == 'x' && !isConsumedEnd(static_cast<int>(column), static_cast<int>(row))) {
                std::cerr << "Orphan Cheep Cheep route end marker 'x' at row " << row + 1
                          << ", column " << column + 1 << " in " << path << std::endl;
                return false;
            }
        }
    }

    return true;
}

constexpr std::string_view TILESET_PATH = "assets/textures/tiles/tileset.png";
constexpr std::string_view OBJECTS_TILESET_PATH = "assets/textures/items/items_objects.png";

struct DebrisFrames {
    const sf::IntRect& topLeft;
    const sf::IntRect& topRight;
    const sf::IntRect& bottomLeft;
    const sf::IntRect& bottomRight;
};

const sf::IntRect& flagFrameForTheme(LevelTheme theme) {
    switch (theme) {
        case LevelTheme::UNDERGROUND:
            return SpriteFrames::udg::Items::FLAGPOLE_FLAG;
        case LevelTheme::CASTLE:
            return SpriteFrames::castle::Items::FLAGPOLE_FLAG;
        case LevelTheme::UNDERWATER:
            return SpriteFrames::udw::Items::FLAGPOLE_FLAG;
        case LevelTheme::OVERWORLD:
        default:
            return SpriteFrames::ovw::Items::FLAGPOLE_FLAG;
    }
}

const DebrisFrames& debrisFramesForTheme(LevelTheme theme) {
    static const DebrisFrames overworld{
        SpriteFrames::ovw::Blocks::DEBRIS_TOP_LEFT,
        SpriteFrames::ovw::Blocks::DEBRIS_TOP_RIGHT,
        SpriteFrames::ovw::Blocks::DEBRIS_BOTTOM_LEFT,
        SpriteFrames::ovw::Blocks::DEBRIS_BOTTOM_RIGHT};
    static const DebrisFrames underground{
        SpriteFrames::udg::Blocks::DEBRIS_TOP_LEFT,
        SpriteFrames::udg::Blocks::DEBRIS_TOP_RIGHT,
        SpriteFrames::udg::Blocks::DEBRIS_BOTTOM_LEFT,
        SpriteFrames::udg::Blocks::DEBRIS_BOTTOM_RIGHT};
    static const DebrisFrames castle{
        SpriteFrames::castle::Blocks::DEBRIS_TOP_LEFT,
        SpriteFrames::castle::Blocks::DEBRIS_TOP_RIGHT,
        SpriteFrames::castle::Blocks::DEBRIS_BOTTOM_LEFT,
        SpriteFrames::castle::Blocks::DEBRIS_BOTTOM_RIGHT};
    static const DebrisFrames underwater{
        SpriteFrames::legacy::Blocks::UW_DEBRIS_TOP_LEFT,
        SpriteFrames::legacy::Blocks::UW_DEBRIS_TOP_RIGHT,
        SpriteFrames::legacy::Blocks::UW_DEBRIS_BOTTOM_LEFT,
        SpriteFrames::legacy::Blocks::UW_DEBRIS_BOTTOM_RIGHT};
    switch (theme) {
        case LevelTheme::UNDERGROUND: return underground;
        case LevelTheme::UNDERWATER:  return underwater;
        case LevelTheme::CASTLE:      return castle;
        case LevelTheme::OVERWORLD:
        default:                      return overworld;
    }
}

} // namespace

void TileMap::setTheme(LevelTheme theme) {
    if (m_theme == theme) {
        return;
    }

    m_theme = theme;

    if (!m_grid.empty()) {
        buildVertices();
    }
}

namespace {
sf::IntRect getTilesetRect(char symbol, LevelTheme theme) {
    switch (symbol) {
        case '0':
        case '1':
            if (theme == LevelTheme::UNDERWATER) return TileFrames::GROUND_UNDERWATER;
            if (theme == LevelTheme::UNDERGROUND) return TileFrames::GROUND_UNDERGROUND;
            if (theme == LevelTheme::CASTLE) return TileFrames::GROUND_CASTLE;
            return TileFrames::GROUND;

        case 'S':
            if (theme == LevelTheme::UNDERWATER) return TileFrames::STONE_UNDERWATER;
            if (theme == LevelTheme::UNDERGROUND) return TileFrames::STONE_UNDERGROUND;
            if (theme == LevelTheme::CASTLE) return TileFrames::STONE_CASTLE;
            return TileFrames::STONE;

        case 'B':
            if (theme == LevelTheme::UNDERWATER) return TileFrames::BRICK_UNDERWATER;
            if (theme == LevelTheme::UNDERGROUND) return TileFrames::BRICK_UNDERGROUND;
            if (theme == LevelTheme::CASTLE) return TileFrames::BRICK_CASTLE;
            return TileFrames::BRICK;

        case '?':
        case 'U':
        case 'u':
        case 'O':
        case 'o':
        case 'f':
        case 'h':
            if (theme == LevelTheme::UNDERWATER) return TileFrames::QUESTION_UNDERWATER;
            if (theme == LevelTheme::UNDERGROUND) return TileFrames::QUESTION_UNDERGROUND;
            return TileFrames::QUESTION;

        case 'E':
            if (theme == LevelTheme::UNDERWATER) return TileFrames::USED_BLOCK_UNDERWATER;
            if (theme == LevelTheme::UNDERGROUND) return TileFrames::USED_BLOCK_UNDERGROUND;
            if (theme == LevelTheme::CASTLE) return TileFrames::USED_BLOCK_CASTLE;
            return TileFrames::USED_BLOCK;

        case '[':
        case 'p':
        case 'q':
            if (theme == LevelTheme::UNDERWATER) return TileFrames::PIPE_TOP_LEFT_UNDERWATER;
            if (theme == LevelTheme::UNDERGROUND) return TileFrames::PIPE_TOP_LEFT_UNDERGROUND;
            if (theme == LevelTheme::CASTLE) return TileFrames::PIPE_TOP_LEFT_CASTLE;
            return TileFrames::PIPE_TOP_LEFT;

        case ']':
        case 'r':
            if (theme == LevelTheme::UNDERWATER) return TileFrames::PIPE_TOP_RIGHT_UNDERWATER;
            if (theme == LevelTheme::UNDERGROUND) return TileFrames::PIPE_TOP_RIGHT_UNDERGROUND;
            if (theme == LevelTheme::CASTLE) return TileFrames::PIPE_TOP_RIGHT_CASTLE;
            return TileFrames::PIPE_TOP_RIGHT;

        case '{':
            if (theme == LevelTheme::UNDERWATER) return TileFrames::PIPE_BODY_LEFT_UNDERWATER;
            if (theme == LevelTheme::UNDERGROUND) return TileFrames::PIPE_BODY_LEFT_UNDERGROUND;
            if (theme == LevelTheme::CASTLE) return TileFrames::PIPE_BODY_LEFT_CASTLE;
            return TileFrames::PIPE_BODY_LEFT;

        case '}':
            if (theme == LevelTheme::UNDERWATER) return TileFrames::PIPE_BODY_RIGHT_UNDERWATER;
            if (theme == LevelTheme::UNDERGROUND) return TileFrames::PIPE_BODY_RIGHT_UNDERGROUND;
            if (theme == LevelTheme::CASTLE) return TileFrames::PIPE_BODY_RIGHT_CASTLE;
            return TileFrames::PIPE_BODY_RIGHT;

        case 'F':
        case 'T':
            if (theme == LevelTheme::UNDERWATER) return TileFrames::FINISH_TOP_UNDERWATER;
            if (theme == LevelTheme::UNDERGROUND) return TileFrames::FINISH_TOP_UNDERGROUND;
            // The Castle palette supplies the stone/gray cap used by the
            // finale, matching the Castle body and the rest of Level 3.
            if (theme == LevelTheme::CASTLE) return TileFrames::FINISH_TOP_CASTLE;
            return TileFrames::FINISH_TOP;

        case '|':
            if (theme == LevelTheme::UNDERWATER) return TileFrames::FINISH_POLE_UNDERWATER;
            if (theme == LevelTheme::UNDERGROUND) return TileFrames::FINISH_POLE_UNDERWATER;
            if (theme == LevelTheme::CASTLE) return TileFrames::FINISH_POLE_CASTLE;
            return TileFrames::FINISH_POLE;

        case '=':
            // Bowser's bridge: the Castle Platform cell of the tileset atlas
            // (docs/tileset_coordinate.md #191). One palette serves every theme.
            return sf::IntRect({558, 33}, {16, 16});

        default:
            return TileFrames::GROUND;
    }
}

sf::IntRect getHorizontalPipeRect(LevelTheme theme) {
    switch (theme) {
        case LevelTheme::UNDERGROUND:
            return TileFrames::HORIZONTAL_PIPE_UNDERGROUND;

        case LevelTheme::CASTLE:
            return TileFrames::HORIZONTAL_PIPE_CASTLE;

        case LevelTheme::UNDERWATER:
            return TileFrames::HORIZONTAL_PIPE_UNDERWATER;

        case LevelTheme::OVERWORLD:
        default:
            return TileFrames::HORIZONTAL_PIPE;
    }
}

sf::IntRect getWaterSurfaceRect(LevelTheme theme) {
    switch (theme) {
        case LevelTheme::UNDERGROUND:
            return TileFrames::WATER_SURFACE_UNDERGROUND;

        case LevelTheme::CASTLE:
            return TileFrames::WATER_SURFACE_CASTLE;

        case LevelTheme::UNDERWATER:
            return TileFrames::WATER_SURFACE_UNDERWATER;

        case LevelTheme::OVERWORLD:
        default:
            return TileFrames::WATER_SURFACE;
    }
}

sf::IntRect getWaterBodyRect(LevelTheme theme) {
    switch (theme) {
        case LevelTheme::UNDERGROUND:
            return TileFrames::WATER_BODY_UNDERGROUND;

        case LevelTheme::CASTLE:
            return TileFrames::WATER_BODY_CASTLE;

        case LevelTheme::UNDERWATER:
            return TileFrames::WATER_BODY_UNDERWATER;

        case LevelTheme::OVERWORLD:
        default:
            return TileFrames::WATER_BODY;
    }
}

void appendTexturedVertex(sf::VertexArray& vertices,
                          float x, float y,
                          float textureX, float textureY){
    sf::Vertex vertex;
    vertex.position = {x, y};
    vertex.color = sf::Color::White;
    vertex.texCoords = {textureX, textureY};

    vertices.append(vertex);
}

void appendTileQuad(sf::VertexArray& vertices,
                    int column, int row,
                    const sf::IntRect& rect) {
    const float left = static_cast<float>(column) * TILE_SIZE_PIXELS;
    const float top = static_cast<float>(row) * TILE_SIZE_PIXELS;
    const float right = left + TILE_SIZE_PIXELS;
    const float bottom = top + TILE_SIZE_PIXELS;

    constexpr float texEpsilon = 0.02f;

    const float textureLeft = static_cast<float>(rect.position.x) + texEpsilon;
    const float textureTop = static_cast<float>(rect.position.y) + texEpsilon;
    const float textureRight = static_cast<float>(rect.position.x + rect.size.x) - texEpsilon;
    const float textureBottom = static_cast<float>(rect.position.y + rect.size.y) - texEpsilon;

    appendTexturedVertex(vertices,
                         left, top,
                         textureLeft, textureTop
                         );

    appendTexturedVertex(vertices,
                         left, bottom,
                         textureLeft, textureBottom
                         );

    appendTexturedVertex(vertices,
                         right, bottom,
                         textureRight, textureBottom
                         );

    appendTexturedVertex(vertices,
                         left, top,
                         textureLeft, textureTop
                         );

    appendTexturedVertex(vertices,
                         right, bottom,
                         textureRight, textureBottom
                         );

    appendTexturedVertex(vertices,
                         right, top,
                         textureRight, textureTop
                         );
}

struct LevelValidationState {
    std::size_t expectedWidth{0};
    std::size_t marioSpawnCount{0};
    std::size_t secondSpawnCount{0};
    std::size_t flowerPedestalCount{0};
    std::size_t finishCount{0};
    std::size_t flagpoleTopCount{0};
    std::size_t toadCount{0};
};

bool validateRow(const std::string& row,
                 std::size_t lineNumber,
                 const std::string& path,
                 TileMap::LayoutMode mode,
                 LevelValidationState& state) {
    if (state.expectedWidth == 0) {
        state.expectedWidth = row.size();
    } else if (row.size() != state.expectedWidth) {
        const std::size_t mismatchColumn = row.size() < state.expectedWidth ? row.size() + 1 : state.expectedWidth + 1;

        std::cerr << "Invalid level file: row " << lineNumber << " has width " << row.size() << ", expected " << state.expectedWidth << "; first mismatch at column " << mismatchColumn << " in " << path << std::endl;

        return false;
    }
    
    for (std::size_t column = 0; column < row.size(); ++column) {
        const char symbol = row[column];

        if (!isValidTileSymbol(symbol)) {
            std::cerr << "Invalid tile symbol '" << symbol << "' at row " << lineNumber << ", column " << column + 1 << " in " << path << std::endl;

            return false;
        }

        // The PvP-only markers must not leak into campaign layouts, and vice
        // versa the campaign finish markers are rejected by the arena rules.
        const bool pvpOnlySymbol = (symbol == 'm');
        if (pvpOnlySymbol && mode != TileMap::LayoutMode::PVP_ARENA) {
            std::cerr << "PvP-only tile symbol '" << symbol << "' at row " << lineNumber << ", column " << column + 1 << " is not allowed in this layout mode in " << path << std::endl;

            return false;
        }

        if (symbol == 'M') {
            ++state.marioSpawnCount;
        } else if (symbol == 'm') {
            ++state.secondSpawnCount;
        } else if (symbol == 'W' && mode == TileMap::LayoutMode::PVP_ARENA) {
            ++state.flowerPedestalCount;
        } else if (symbol == 'F') {
            ++state.finishCount;
        } else if (symbol == 'T') {
            ++state.flagpoleTopCount;
        } else if (symbol == 'N') {
            ++state.toadCount;
        }
    }
    
    return true;
}

bool validateCampaignMarkers(const LevelValidationState& state, const std::string& path) {
    if (state.marioSpawnCount != 1) {
        std::cerr << "Invalid level file: expected exactly one Mario spawn but found " << state.marioSpawnCount << " in " << path << std::endl;

        return false;
    }

    const bool hasFlagEnding = state.finishCount == 1 &&
                               state.flagpoleTopCount == 1 &&
                               state.toadCount == 0;

    const bool hasToadEnding = state.finishCount == 0 &&
                               state.flagpoleTopCount == 0 &&
                               state.toadCount == 1;

    if (!hasFlagEnding && !hasToadEnding) {
        std::cerr << "Invalid level file: expected either one flag ending " << "(T + F) or one Toad ending (N) in " << path << std::endl;

        return false;
    }
    return true;
}

bool validatePvpArenaMarkers(const LevelValidationState& state, const std::string& path) {
    if (state.marioSpawnCount != 1) {
        std::cerr << "Invalid PvP arena file: expected exactly one player one spawn ('M') but found " << state.marioSpawnCount << " in " << path << std::endl;

        return false;
    }

    if (state.secondSpawnCount != 1) {
        std::cerr << "Invalid PvP arena file: expected exactly one player two spawn ('m') but found " << state.secondSpawnCount << " in " << path << std::endl;

        return false;
    }

    if (state.flowerPedestalCount != 1) {
        std::cerr << "Invalid PvP arena file: expected exactly one fire flower pedestal ('W') but found " << state.flowerPedestalCount << " in " << path << std::endl;

        return false;
    }

    if (state.finishCount != 0) {
        std::cerr << "Invalid PvP arena file: finish markers are not allowed but found " << state.finishCount << " in " << path << std::endl;

        return false;
    }
    
    return true;
}

bool validateLevelMarkers(const LevelValidationState& state,
                          const std::string& path,
                          TileMap::LayoutMode mode) {
    if (mode == TileMap::LayoutMode::PVP_ARENA) {
        return validatePvpArenaMarkers(state, path);
    }

    return validateCampaignMarkers(state, path);
}

bool validateFlagPole(const std::vector<std::string>& grid, const std::string& path) {
    std::size_t finishRow = 0;
    std::size_t finishColumn = 0;
    bool finishFound = false;
    std::size_t topRow = 0;
    std::size_t topColumn = 0;
    bool topFound = false;

    for (std::size_t row = 0; row < grid.size(); ++row) {
        const std::size_t column = grid[row].find('F');

        if (column != std::string::npos) {
            finishRow = row;
            finishColumn = column;
            finishFound = true;
            break;
        }
    }

    for (std::size_t row = 0; row < grid.size(); ++row) {
        const std::size_t column = grid[row].find('T');

        if (column != std::string::npos) {
            topRow = row;
            topColumn = column;
            topFound = true;
            break;
        }
    }

    if (!finishFound) {
        return false;
    }

    if (!topFound || finishRow == 0 || topRow != finishRow - 1 ||
        topColumn != finishColumn) {
        std::cerr << "Invalid level file: flagpole top marker must be directly above the finish point in " << path << std::endl;

        return false;
    }

    const std::size_t firstPoleRow = finishRow + 1;

    if (firstPoleRow >= grid.size() || grid[firstPoleRow][finishColumn] != '|') {
        std::cerr << "Invalid level file: finish point at row " << finishRow + 1 << ", column " << finishColumn + 1 << " must have a pole tile directly below it in " << path << std::endl;

        return false;
    }

    std::size_t row = firstPoleRow;

    while (row < grid.size() && grid[row][finishColumn] == '|') {
        ++row;
    }

    if (row >= grid.size() || !isSolidTileSymbol(grid[row][finishColumn])) {
        std::cerr << "Invalid level file: flag pole at column " << finishColumn + 1 << " must end on a solid tile in " << path << std::endl;

        return false;
    }

    for (std::size_t currentRow = 0; currentRow < grid.size(); ++currentRow) {
        for (std::size_t currentColumn = 0; currentColumn < grid[currentRow].size(); ++currentColumn) {
            if (grid[currentRow][currentColumn] != '|') {
                continue;
            }

            const bool belongsToPole = currentColumn == finishColumn && currentRow >= firstPoleRow && currentRow < row;

            if (!belongsToPole) {
                std::cerr << "Invalid level file: disconnected pole tile at row " << currentRow + 1 << ", column " << currentColumn + 1 << " in " << path << std::endl;

                return false;
            }
        }
    }

    return true;
}

} // namespace

TileMap::~TileMap() {
    clearPhysicsBodies();
}

bool TileMap::loadFromFile(const std::string& path, LayoutMode mode) {
    std::ifstream inputFile(path);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open level file: " << path << std::endl;
        return false;
    }

    std::vector<std::string> loadedGrid;
    std::vector<WarpEntry> loadedWarpEntries;
    std::vector<WarpReturn> loadedWarpReturns;
    
    LevelValidationState validationState;
    std::string line;
    std::size_t lineNumber = 0;

    while(std::getline(inputFile, line)) {
        ++lineNumber;
        // Remove the carriage-return character from Windows CRLF files.
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (isBlankLine(line) || isCommentLine(line)) {
            continue;
        }

        if (!validateRow(line, lineNumber, path, mode, validationState)) {
            return false;
        }

        loadedGrid.push_back(line);
    }

    if (loadedGrid.empty()) {
        std::cerr << "Level file contains no map data: " << path << std::endl;
        return false;
    }
    
    if (!validateLevelMarkers(validationState, path, mode)) {
        return false;
    }
    
    if (!parseAndNormalizeWarps(loadedGrid,
                                loadedWarpEntries,
                                loadedWarpReturns,
                                path)) {
        return false;
    }

    std::vector<ElevatorRoute> loadedElevatorRoutes;
    if (!parseAndNormalizeElevators(loadedGrid, loadedElevatorRoutes, path)) {
        return false;
    }

    std::vector<CheepCheepRoute> loadedCheepCheepRoutes;
    if (!parseAndNormalizeCheepCheeps(loadedGrid, loadedCheepCheepRoutes, path)) {
        return false;
    }

    // Only flag-ending campaign levels need flagpole validation.
    // Toad-ending levels do not contain F/T/| markers.
    if (mode == LayoutMode::CAMPAIGN && validationState.finishCount > 0 && !validateFlagPole(loadedGrid, path)) {
        return false;
    }
    
    sf::Texture loadedTileset;
    sf::Texture loadedObjectsTileset;
    try {
        sf::Image tilesetImage;
        if (!tilesetImage.loadFromFile(std::string(TILESET_PATH))) {
            std::cerr << "Failed to load TileMap tileset image: " << TILESET_PATH << std::endl;
            return false;
        }

        // The reference sheet is an opaque compositing sheet.  Its three
        // backdrop colors are not gameplay pixels; key them out before the
        // atlas is uploaded so pipes, poles, and assembled structures do not
        // carry lavender/blue rectangles into the level.
        tilesetImage.createMaskFromColor(sf::Color(146, 144, 255));
        tilesetImage.createMaskFromColor(sf::Color(148, 148, 255));
        tilesetImage.createMaskFromColor(sf::Color(0, 41, 140));
        loadedTileset = sf::Texture(tilesetImage);

        sf::Image objectsImage;
        if (!objectsImage.loadFromFile(std::string(OBJECTS_TILESET_PATH))) {
            std::cerr << "Failed to load TileMap objects tileset image: "
                      << OBJECTS_TILESET_PATH << std::endl;
            return false;
        }
        objectsImage.createMaskFromColor(sf::Color(146, 144, 255));
        objectsImage.createMaskFromColor(sf::Color(148, 148, 255));
        objectsImage.createMaskFromColor(sf::Color(0, 41, 140));
        loadedObjectsTileset = sf::Texture(objectsImage);
    } catch (const sf::Exception& exception) {
        std::cerr << "Failed to load TileMap tileset: " << TILESET_PATH << std::endl;
        std::cerr << "Reason: " << exception.what() << std::endl;
        return false;
    }

    loadedTileset.setSmooth(false);
    loadedObjectsTileset.setSmooth(false);

    const sf::Vector2u tilesetSize = loadedTileset.getSize();

    if (tilesetSize.x == 0 || tilesetSize.y == 0) {
        std::cerr << "Invalid TileMap tileset dimensions: " << tilesetSize.x << 'x' << tilesetSize.y << std::endl;
        return false;
    }

    m_pendingTileHits.clear();

    m_warpEntries = std::move(loadedWarpEntries);
    m_warpReturns = std::move(loadedWarpReturns);
    m_elevatorRoutes = std::move(loadedElevatorRoutes);
    m_cheepCheepRoutes = std::move(loadedCheepCheepRoutes);

    m_layoutMode = mode;

    m_grid = std::move(loadedGrid);
    m_tileset = std::move(loadedTileset);
    m_objectsTileset = std::move(loadedObjectsTileset);

    m_flagAnimationTime = 0.0f;
    m_flagDropDistance = 0.0f;
    buildVertices();
    return true;
}

void TileMap::render(sf::RenderTarget& target) const {
    sf::RenderStates states;
    states.texture = &m_tileset;

    // Terrain only.
    target.draw(m_vertices, states);

    sf::RenderStates objectStates;
    objectStates.texture = &m_objectsTileset;
    target.draw(m_objectVertices, objectStates);
}

void TileMap::renderWater(sf::RenderTarget& target) const {
    sf::RenderStates states;
    states.texture = &m_tileset;

    target.draw(m_waterVertices, states);
}

void TileMap::renderFlags(sf::RenderTarget& target) const {
    sf::RenderStates flagStates;
    flagStates.texture = &m_objectsTileset;
    target.draw(m_flagVertices, flagStates);
}

void TileMap::renderForeground(sf::RenderTarget& target) const {
    sf::RenderStates states;
    states.texture = &m_tileset;

    // The pole shaft/cap stays above the flag cloth. This restores the small
    // pole-colored connector at the flag edge without putting the cloth above
    // Mario while he slides.
    target.draw(m_foregroundVertices, states);
}

char TileMap::getTileAt(int column, int row) const {
    if (column < 0 || row < 0){
        return '.';
    }

    const std::size_t gridRow = static_cast<std::size_t>(row);
    const std::size_t gridColumn = static_cast<std::size_t>(column);

    if (gridRow >= m_grid.size()){
        return '.';
    }

    if (gridColumn >= m_grid[gridRow].size()){
        return '.';
    }

    return m_grid[gridRow][gridColumn];
}

bool TileMap::isEnemySupport(int column, int row) const {
    return isEnemySupportTileSymbol(getTileAt(column, row)) || isHorizontalPipeFootprint(m_grid, column, row);
}

bool TileMap::isSolid(int column, int row) const {
    return isSolidTileSymbol(getTileAt(column, row)) || isHorizontalPipeFootprint(m_grid, column, row);
}

bool TileMap::isClimbable(int column, int row) const {
    return getTileAt(column, row) == 'V';
}

std::size_t TileMap::getWidth() const {
    if (m_grid.empty()){
        return 0;
    }

    return m_grid.front().size();
}

std::size_t TileMap::getHeight() const {
    return m_grid.size();
}

void TileMap::triggerTileBump(int column, int row) {
    for (const auto& bump : m_bumpAnimations) {
        if (bump.column == column && bump.row == row) return;
    }
    m_bumpAnimations.push_back({column, row, 0.f, 0.16f, -12.f});
}

void TileMap::setFlagDropDistance(float distancePixels) {
    const float clampedDistance =
        std::clamp(distancePixels, 0.0f, getFlagMaxDropDistance());
    if (std::abs(clampedDistance - m_flagDropDistance) < 0.01f) {
        return;
    }

    m_flagDropDistance = clampedDistance;
    buildFlagVertices();
}

float TileMap::getFlagMaxDropDistance() const {
    float maxDrop = 0.0f;
    for (std::size_t row = 0; row < m_grid.size(); ++row) {
        for (std::size_t col = 0; col < m_grid[row].size(); ++col) {
            if (m_grid[row][col] != 'F') {
                continue;
            }

            int bottomRow = static_cast<int>(row);
            while (bottomRow + 1 < static_cast<int>(m_grid.size()) &&
                   getTileAt(static_cast<int>(col), bottomRow + 1) == '|') {
                ++bottomRow;
            }

            maxDrop = std::max(
                maxDrop,
                std::max(0.0f,
                         static_cast<float>(bottomRow - static_cast<int>(row)) *
                             TILE_SIZE_PIXELS));
        }
    }
    return maxDrop;
}

bool TileMap::isFlagFullyDropped() const {
    return m_flagDropDistance >= getFlagMaxDropDistance() - 0.01f;
}

void TileMap::update(float dt) {
    const bool flagChanged = dt > 0.0f;
    if (flagChanged) {
        m_flagAnimationTime = std::fmod(
            m_flagAnimationTime + dt * FLAG_WAVE_SPEED, TWO_PI);
        if (m_flagAnimationTime < 0.0f) {
            m_flagAnimationTime += TWO_PI;
        }
    }

    bool changed = false;
    for (auto it = m_bumpAnimations.begin(); it != m_bumpAnimations.end(); ) {
        it->timer += dt;
        changed = true;
        if (it->timer >= it->maxDuration) {
            it = m_bumpAnimations.erase(it);
        } else {
            ++it;
        }
    }

    if (changed) {
        buildVertices();
    } else if (flagChanged) {
        buildFlagVertices();
    }
}

void TileMap::buildVertices() {
    m_vertices.clear();
    m_waterVertices.clear();
    m_foregroundVertices.clear();
    m_objectVertices.clear();
    m_flagVertices.clear();

    for (std::size_t row = 0; row < m_grid.size(); ++row) {
        for (std::size_t column = 0; column < m_grid[row].size(); ++column) {

            const char symbol = m_grid[row][column];

            // L = bottom-left anchor of a 5x5 castle.
            if (symbol == 'L') {
                const sf::IntRect rect = (m_theme == LevelTheme::UNDERGROUND)
                                            ? TileFrames::CASTLE_UNDERGROUND
                                            : TileFrames::CASTLE;

                const float left = static_cast<float>(column * TILE_SIZE);
                const float top = static_cast<float>((static_cast<int>(row) - 4) * TILE_SIZE);
                const float right = left + static_cast<float>(5 * TILE_SIZE);
                const float bottom = top + static_cast<float>(5 * TILE_SIZE);

                const float texEpsilon = 0.02f;

                const float textureLeft = static_cast<float>(rect.position.x) + texEpsilon;
                const float textureTop = static_cast<float>(rect.position.y) + texEpsilon;
                const float textureRight = static_cast<float>(rect.position.x + rect.size.x) - texEpsilon;
                const float textureBottom = static_cast<float>(rect.position.y + rect.size.y) - texEpsilon;

                appendTexturedVertex(m_vertices,
                                     left, top,
                                     textureLeft, textureTop
                                     );
                appendTexturedVertex(m_vertices,
                                     left, bottom,
                                     textureLeft, textureBottom
                                     );
                appendTexturedVertex(m_vertices,
                                     right, bottom,
                                     textureRight, textureBottom
                                     );

                appendTexturedVertex(m_vertices,
                                     left, top,
                                     textureLeft, textureTop
                                     );

                appendTexturedVertex(m_vertices,
                                     right, bottom,
                                     textureRight, textureBottom
                                     );
                appendTexturedVertex(m_vertices,
                                     right, top,
                                     textureRight, textureTop
                                     );

                continue;
            }

            // H = bottom-left anchor of a 3x2 horizontal pipe.
            if (symbol == 'H') {
                const sf::IntRect rect = getHorizontalPipeRect(m_theme);

                const float left = static_cast<float>(column * TILE_SIZE);
                const float top = static_cast<float>((static_cast<int>(row) - 1) * TILE_SIZE);
                const float right = left + static_cast<float>(3 * TILE_SIZE);
                const float bottom = top + static_cast<float>(2 * TILE_SIZE);

                const float texEpsilon = 0.02f;

                const float textureLeft = static_cast<float>(rect.position.x) + texEpsilon;
                const float textureTop = static_cast<float>(rect.position.y) + texEpsilon;
                const float textureRight = static_cast<float>(rect.position.x + rect.size.x) - texEpsilon;
                const float textureBottom = static_cast<float>(rect.position.y + rect.size.y) - texEpsilon;

                appendTexturedVertex(m_foregroundVertices,
                                     left, top,
                                     textureLeft, textureTop
                                     );
                appendTexturedVertex(m_foregroundVertices,
                                     left, bottom,
                                     textureLeft, textureBottom
                                     );
                appendTexturedVertex(m_foregroundVertices,
                                     right, bottom,
                                     textureRight, textureBottom
                                     );

                appendTexturedVertex(m_foregroundVertices,
                                     left, top,
                                     textureLeft, textureTop
                                     );

                appendTexturedVertex(m_foregroundVertices,
                                     right, bottom,
                                     textureRight, textureBottom
                                     );

                appendTexturedVertex(m_foregroundVertices,
                                     right, top,
                                     textureRight, textureTop
                                     );

                continue;
            }
            
            if (symbol == 'W') {
                // handled separately by buildWaterVertices()
                continue;
            }

            if (symbol == 'e') {
                appendTileQuad(m_vertices,
                               static_cast<int>(column), static_cast<int>(row),
                               TileFrames::HARD_BLOCK_CASTLE);

                continue;
            }

            // F rendered as flag in m_flagVertices — skip from tile grid.
            if (symbol == 'F') {
                // Keep the map representation vertical (T/F/|), but render
                // the first shaft segment in the F cell as well.  The flag
                // cloth is anchored to the pole center, so this closes the
                // small visual gap at the top without changing the validated
                // trigger column or Mario's climb position.
                const sf::IntRect poleRect = getTilesetRect('|', m_theme);
                const float x = static_cast<float>(column * TILE_SIZE);
                const float y = static_cast<float>(row * TILE_SIZE);
                const float right = x + static_cast<float>(TILE_SIZE);
                const float bottom = y + static_cast<float>(TILE_SIZE);
                const float texEpsilon = 0.02f;
                const float textureLeft = static_cast<float>(poleRect.position.x) + texEpsilon;
                const float textureTop = static_cast<float>(poleRect.position.y) + texEpsilon;
                const float textureRight = static_cast<float>(poleRect.position.x + poleRect.size.x) - texEpsilon;
                const float textureBottom = static_cast<float>(poleRect.position.y + poleRect.size.y) - texEpsilon;

                appendTexturedVertex(m_foregroundVertices, x, y, textureLeft, textureTop);
                appendTexturedVertex(m_foregroundVertices, x, bottom, textureLeft, textureBottom);
                appendTexturedVertex(m_foregroundVertices, right, bottom, textureRight, textureBottom);
                appendTexturedVertex(m_foregroundVertices, x, y, textureLeft, textureTop);
                appendTexturedVertex(m_foregroundVertices, right, bottom, textureRight, textureBottom);
                appendTexturedVertex(m_foregroundVertices, right, y, textureRight, textureTop);
                continue;
            }

            // V = vine tile (climbable)
            if (symbol == 'V') {
                const sf::IntRect textureRect =
                    (row == 0 || getTileAt(static_cast<int>(column), static_cast<int>(row - 1)) != 'V')
                        ? SpriteFrames::shared::Items::VINE_TOP
                        : ((row % 2u) == 0u ? SpriteFrames::shared::Items::VINE_STEM_1
                                            : SpriteFrames::shared::Items::VINE_STEM_2);
                const float left = static_cast<float>(column * TILE_SIZE);
                const float top = static_cast<float>(row * TILE_SIZE);
                const float right = left + static_cast<float>(TILE_SIZE);
                const float bottom = top + static_cast<float>(TILE_SIZE);
                const float textureLeft = static_cast<float>(textureRect.position.x) + 0.02f;
                const float textureTop = static_cast<float>(textureRect.position.y) + 0.02f;
                const float textureRight = static_cast<float>(textureRect.position.x + textureRect.size.x) - 0.02f;
                const float textureBottom = static_cast<float>(textureRect.position.y + textureRect.size.y) - 0.02f;
                appendTexturedVertex(m_objectVertices, left, top, textureLeft, textureTop);
                appendTexturedVertex(m_objectVertices, left, bottom, textureLeft, textureBottom);
                appendTexturedVertex(m_objectVertices, right, bottom, textureRight, textureBottom);
                appendTexturedVertex(m_objectVertices, left, top, textureLeft, textureTop);
                appendTexturedVertex(m_objectVertices, right, bottom, textureRight, textureBottom);
                appendTexturedVertex(m_objectVertices, right, top, textureRight, textureTop);
                continue;
            }

            if (!isRenderableTile(symbol)) {
                continue;
            }

            const float x = static_cast<float>(column * TILE_SIZE);
            const float y = static_cast<float>(row * TILE_SIZE);

            const sf::IntRect textureRect = getTilesetRect(symbol, m_theme);

            float offsetY = 0.f;

            for (const auto& bump : m_bumpAnimations) {
                if (bump.column == static_cast<int>(column) && bump.row == static_cast<int>(row)) {
                    const float progress = bump.timer / bump.maxDuration;

                    offsetY = std::sin(progress * 3.14159265f) * bump.maxOffset;

                    break;
                }
            }

            const float left = x;
            const float top = y + offsetY;
            const float right = left + static_cast<float>(TILE_SIZE);
            const float bottom = top + static_cast<float>(TILE_SIZE);

            const float texEpsilon = 0.02f;

            const float textureLeft = static_cast<float>(textureRect.position.x) + texEpsilon;
            const float textureTop = static_cast<float>(textureRect.position.y) + texEpsilon;
            const float textureRight = static_cast<float>(textureRect.position.x + textureRect.size.x) - texEpsilon;
            const float textureBottom = static_cast<float>(textureRect.position.y + textureRect.size.y) - texEpsilon;

            sf::VertexArray& targetArray = isForegroundTile(symbol)
                                           ? m_foregroundVertices : m_vertices;

            appendTexturedVertex(targetArray,
                                 left, top,
                                 textureLeft, textureTop
                                 );
            appendTexturedVertex(targetArray,
                                 left, bottom,
                                 textureLeft, textureBottom
                                 );
            appendTexturedVertex(targetArray,
                                 right, bottom,
                                 textureRight, textureBottom
                                 );

            appendTexturedVertex(targetArray,
                                 left, top,
                                 textureLeft, textureTop
                                 );
            appendTexturedVertex(targetArray,
                                 right, bottom,
                                 textureRight, textureBottom
                                 );
            appendTexturedVertex(targetArray,
                                 right, top,
                                 textureRight, textureTop
                                 );
        }
    }

    buildWaterVertices();
    buildFlagVertices();
}

void TileMap::buildFlagVertices() {
    m_flagVertices.clear();

    for (std::size_t row = 0; row < m_grid.size(); ++row) {
        for (std::size_t col = 0; col < m_grid[row].size(); ++col) {
            if (m_grid[row][col] != 'F') {
                continue;
            }

            const sf::IntRect textureRect = flagFrameForTheme(m_theme);

            int bottomRow = static_cast<int>(row);
            while (bottomRow + 1 < static_cast<int>(m_grid.size()) &&
                   getTileAt(static_cast<int>(col), bottomRow + 1) == '|') {
                ++bottomRow;
            }

            // Let the flag occupy the pole body all the way to the solid base.
            // The bottom edge reaches the base without entering its tile, so
            // the flag can keep the same downward displacement as Mario.
            const float maxDropDistance = std::max(
                0.0f,
                static_cast<float>(bottomRow - static_cast<int>(row)) * TILE_SIZE_PIXELS);
            const float dropDistance = std::min(m_flagDropDistance, maxDropDistance);

            const float poleCenterX = static_cast<float>(col * TILE_SIZE) + TILE_SIZE_PIXELS / 2.0f;
            // The right edge of the triangular flag is the pole-side edge.
            // Move the 32px world quad half a tile left so it meets the pole
            // center rather than the right edge of the F marker cell.
            const float left = poleCenterX - TILE_SIZE_PIXELS;
            const float top = static_cast<float>(row * TILE_SIZE) + dropDistance;
            const float right = poleCenterX;
            const float bottom = top + TILE_SIZE_PIXELS;

            // Keep the pole-side edge anchored on the right and gently shear
            // only the free edge on the left. This animates the objects-atlas
            // flag without changing collision geometry.
            const float topWave = std::sin(m_flagAnimationTime) * FLAG_WAVE_AMPLITUDE;
            const float bottomWave = std::sin(m_flagAnimationTime + FLAG_WAVE_PHASE_OFFSET) * FLAG_WAVE_AMPLITUDE;
            const float texEpsilon = 0.02f;
            const float textureLeft = static_cast<float>(textureRect.position.x) + texEpsilon;
            const float textureTop = static_cast<float>(textureRect.position.y) + texEpsilon;
            const float textureRight = static_cast<float>(textureRect.position.x + textureRect.size.x) - texEpsilon;
            const float textureBottom = static_cast<float>(textureRect.position.y + textureRect.size.y) - texEpsilon;

            appendTexturedVertex(m_flagVertices, left + topWave, top, textureLeft, textureTop);
            appendTexturedVertex(m_flagVertices, left + bottomWave, bottom, textureLeft, textureBottom);
            appendTexturedVertex(m_flagVertices, right, bottom,
                                 textureRight, textureBottom);
            appendTexturedVertex(m_flagVertices, left + topWave, top, textureLeft, textureTop);
            appendTexturedVertex(m_flagVertices, right, bottom,
                                 textureRight, textureBottom);
            appendTexturedVertex(m_flagVertices, right, top,
                                 textureRight, textureTop);
        }
    }
}

std::vector<sf::Vector2i> TileMap::findTiles(char symbol) const {
    std::vector<sf::Vector2i> positions;

    for (std::size_t row = 0; row < m_grid.size(); ++row) {
        for (std::size_t column = 0; column < m_grid[row].size(); ++column) {
            if (m_grid[row][column] == symbol) {
                positions.emplace_back(static_cast<int>(column), static_cast<int>(row));
            }
        }
    }

    return positions;
}

std::optional<sf::Vector2i> TileMap::findWarpReturn(char id) const {
    for (const auto& destination : m_warpReturns) {
        if (destination.id == id) {
            return destination.position;
        }
    }

    return std::nullopt;
}

sf::Vector2f TileMap::gridToWorldPosition(const sf::Vector2i& gridPosition) {
    const float tileSize = static_cast<float>(TILE_SIZE);

    return {static_cast<float>(gridPosition.x) * tileSize,
        static_cast<float>(gridPosition.y) * tileSize
    };
}

void TileMap::createPhysicsBodies(b2World* world) {
    if (!world) {
        std::cerr << "Cannot create TileMap physics bodies: world is null" << std::endl;
        return;
    }
    
    clearPhysicsBodies();
    m_physicsWorld = world;
    
    const float tileSize = static_cast<float>(TILE_SIZE);
    const std::vector<TileCollisionSpan> spans =
        buildHorizontalTileCollisionSpans(m_grid, isSolidTileSymbol);

    for (const TileCollisionSpan& span : spans) {
        const float spanWidthPixels = static_cast<float>(span.length) * tileSize;
        const sf::Vector2f centerPixels(
            static_cast<float>(span.column) * tileSize + spanWidthPixels / 2.f,
            static_cast<float>(span.row) * tileSize + tileSize / 2.f
        );
        const b2Vec2 centerMeters = PhysicsEngine::pixelsToMeters(centerPixels);

        b2BodyDef bodyDefinition;
        bodyDefinition.type = b2_staticBody;
        bodyDefinition.position.Set(centerMeters.x, centerMeters.y);

        // Coordinates identify the span start for diagnostics only. Contact points
        // determine the exact tile when Mario hits a block from below.
        bodyDefinition.userData.pointer = TILE_USERDATA_FLAG |
                                        (static_cast<uintptr_t>(span.row) << 16) |
                                        static_cast<uintptr_t>(span.column);

        b2Body* body = world->CreateBody(&bodyDefinition);

        b2PolygonShape shape;
        shape.SetAsBox(PhysicsEngine::pixelsToMeters(spanWidthPixels / 2.f),
                       PhysicsEngine::pixelsToMeters(tileSize / 2.f));

        b2FixtureDef fixtureDefinition;
        fixtureDefinition.shape = &shape;
        fixtureDefinition.friction = TILE_FRICTION;

        body->CreateFixture(&fixtureDefinition);
        m_physicsBodies.push_back(body);
    }
    
    for (std::size_t row = 0; row < m_grid.size(); ++row) {
        for (std::size_t column = 0; column < m_grid[row].size(); ++column) {
            if (m_grid[row][column] != 'H') {
                continue;
            }

            const float widthPixels = 3.f * tileSize;
            const float heightPixels = 2.f * tileSize;

            const float left = static_cast<float>(column) * tileSize;
            const float top = static_cast<float>(static_cast<int>(row) - 1) * tileSize;

            const sf::Vector2f centerPixels(left + widthPixels / 2.f,
                                            top + heightPixels / 2.f
                                            );

            const b2Vec2 centerMeters = PhysicsEngine::pixelsToMeters(centerPixels);

            b2BodyDef bodyDefinition;
            bodyDefinition.type = b2_staticBody;
            bodyDefinition.position.Set(centerMeters.x, centerMeters.y);

            // H is a TileMap terrain body.
            // Stored coordinates are the H bottom-left anchor.
            bodyDefinition.userData.pointer = TILE_USERDATA_FLAG | (static_cast<uintptr_t>(row) << 16) | static_cast<uintptr_t>(column);

            b2Body* body = world->CreateBody(&bodyDefinition);

            b2PolygonShape shape;
            shape.SetAsBox(PhysicsEngine::pixelsToMeters(widthPixels / 2.f),
                           PhysicsEngine::pixelsToMeters(heightPixels / 2.f)
            );

            b2FixtureDef fixtureDefinition;
            fixtureDefinition.shape = &shape;
            fixtureDefinition.friction = TILE_FRICTION;

            body->CreateFixture(&fixtureDefinition);
            m_physicsBodies.push_back(body);
        }
    }
}

void TileMap::clearPhysicsBodies() {
    if (!m_physicsWorld) {
        m_physicsBodies.clear();
        return;
    }

    for (b2Body* body : m_physicsBodies) {
        if (body) {
            m_physicsWorld->DestroyBody(body);
        }
    }

    m_physicsBodies.clear();
    m_physicsWorld = nullptr;
}

void TileMap::destroyPhysicsBodies() {
    clearPhysicsBodies();
}

bool TileMap::hitTile(int column, int row, bool isBigMario,
                      std::vector<std::unique_ptr<Entity>>& entities,
                      TextureManager* textureManager) {
    if (row < 0 || row >= static_cast<int>(m_grid.size())) {
        return false;
    }
    if (column < 0 || column >= static_cast<int>(m_grid[row].size())) {
        return false;
    }

    char symbol = m_grid[row][column];

    // --- Breakable Brick Block ('B') ---
    if (TileSemantics::isBreakable(symbol)) {
        if (isBigMario) {
            // Shatter Brick Block
            m_grid[row][column] = '.';
            buildVertices();

            sf::Vector2f tileWorldPos = gridToWorldPosition({column, row});
            if (m_physicsWorld && !m_physicsWorld->IsLocked()) {
                // One tile may be inside a merged span, so rebuild static terrain
                // from the updated grid instead of destroying a single body.
                createPhysicsBodies(m_physicsWorld);
            }

            // Spawn 4 flying debris particles (4 corners)
            sf::Vector2f center = tileWorldPos + sf::Vector2f(8.f, 8.f);
            const DebrisFrames& debris = debrisFramesForTheme(m_theme);
            auto d1 = std::make_unique<BlockDebris>(center, sf::Vector2f(-120.f, -380.f), debris.topLeft);
            auto d2 = std::make_unique<BlockDebris>(center, sf::Vector2f(120.f, -380.f), debris.topRight);
            auto d3 = std::make_unique<BlockDebris>(center, sf::Vector2f(-80.f, -220.f), debris.bottomLeft);
            auto d4 = std::make_unique<BlockDebris>(center, sf::Vector2f(80.f, -220.f), debris.bottomRight);

            if (textureManager) {
                d1->setTextureManager(*textureManager);
                d2->setTextureManager(*textureManager);
                d3->setTextureManager(*textureManager);
                d4->setTextureManager(*textureManager);
            }

            entities.push_back(std::move(d1));
            entities.push_back(std::move(d2));
            entities.push_back(std::move(d3));
            entities.push_back(std::move(d4));

            EventBus::getInstance().notify(EventType::BRICK_BROKEN);

            return true;
        } else {
            // Small Mario bump: Block bounces up slightly
            triggerTileBump(column, row);
            EventBus::getInstance().notify(EventType::BLOCK_BUMPED);
            return true;
        }
    }

    return false;
}

void TileMap::buildWaterVertices() {
    m_waterVertices.clear();

    // In PvP, W keeps its existing meaning: fire-flower pedestal, not liquid.
    if (m_layoutMode != LayoutMode::CAMPAIGN) {
        return;
    }
    
    const sf::IntRect surfaceRect = getWaterSurfaceRect(m_theme);
    const sf::IntRect bodyRect = getWaterBodyRect(m_theme);

    for (std::size_t row = 0; row < m_grid.size(); ++row) {
        for (std::size_t col = 0; col < m_grid[row].size(); ++col) {
            if (m_grid[row][col] != 'W') {
                continue;
            }

            // W itself is the liquid surface.
            appendTileQuad(m_waterVertices,
                           static_cast<int>(col), static_cast<int>(row),
                           surfaceRect
                           );

            // Fill the same column down to the bottom of the map.
            // Terrain is rendered afterward and naturally covers liquid where solid blicks exist.
            for (std::size_t waterRow = row + 1; waterRow < m_grid.size(); ++waterRow) {
                // If another explicit surface exists lower in this column, let that marker own the remaining region.
                if (m_grid[waterRow][col] == 'W') {
                    break;
                }

                appendTileQuad(m_waterVertices,
                               static_cast<int>(col), static_cast<int>(waterRow),
                               bodyRect
                               );
            }
        }
    }
}

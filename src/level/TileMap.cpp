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
#include <string_view>
#include <utility>

#include <box2d/box2d.h>
#include <SFML/System/Exception.hpp>

#include "physics/PhysicsEngine.h"
#include "level/TileCollisionSpans.h"
#include "level/TileSemantics.h"
#include "entities/Entity.h"
#include "entities/BlockDebris.h"
#include "entities/Mario.h"
#include "entities/QuestionBlock.h"
#include "core/SpriteFrames.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "level/TileFrames.h"

namespace {

constexpr std::string_view VALID_TILE_SYMBOLS = ".1B?CGKMFS|UEOfhuo[]{}";
constexpr float TILE_SIZE_PIXELS = 32.f;
constexpr float TILE_FRICTION = 0.6f;

bool isSolidTileSymbol(char tile) {
    return tile == '1' || tile == 'B' || tile == 'E' || tile == 'S' ||
           tile == '[' || tile == ']' || tile == '{' || tile == '}';
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
            block->onHit(*mario, &entities, &textureManager);
            m_pendingTileHits.clear();
            return;
        }
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
    return symbol == '1' || symbol == 'B' || symbol == 'F' || symbol == 'S' || symbol == '|' || symbol == 'E' ||
           symbol == '[' || symbol == ']' || symbol == '{' || symbol == '}';
}

bool isForegroundTile(char symbol) {
    // Blocks, flagpoles, and pipes go to the foreground so items spawn behind them and Mario goes behind pipes
    return symbol == 'B' || symbol == '?' || symbol == 'U' || symbol == 'O' ||
           symbol == 'F' || symbol == '|' ||
           symbol == '[' || symbol == ']' || symbol == '{' || symbol == '}';
}

constexpr std::string_view TILESET_PATH = "assets/textures/tiles/tileset.png";

sf::IntRect getTilesetRect(char symbol) {
    switch (symbol) {
        case '1':
            return TileFrames::GROUND;

        case 'S':
            return TileFrames::STONE;

        case 'B':
            return TileFrames::BRICK;

        case '?':
        case 'U':
        case 'u':
        case 'O':
        case 'o':
        case 'f':
        case 'h':
            return TileFrames::QUESTION;

        case 'E':
            return TileFrames::USED_BLOCK;

        case '[':
            return TileFrames::PIPE_TOP_LEFT;

        case ']':
            return TileFrames::PIPE_TOP_RIGHT;

        case '{':
            return TileFrames::PIPE_BODY_LEFT;

        case '}':
            return TileFrames::PIPE_BODY_RIGHT;

        case 'F':
            return TileFrames::FINISH_TOP;

        case '|':
            return TileFrames::FINISH_POLE;

        default:
            return TileFrames::GROUND;
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

struct LevelValidationState {
    std::size_t expectedWidth{0};
    std::size_t marioSpawnCount{0};
    std::size_t finishCount{0};
};

bool validateRow(const std::string& row,
                 std::size_t lineNumber,
                 const std::string& path,
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

        if (symbol == 'M') {
            ++state.marioSpawnCount;
        } else if (symbol == 'F') {
            ++state.finishCount;
        }
    }
    
    return true;
}

bool validateLevelMarkers(const LevelValidationState& state, const std::string& path) {
    if (state.marioSpawnCount != 1) {
        std::cerr << "Invalid level file: expected exactly one Mario spawn but found " << state.marioSpawnCount << " in " << path << std::endl;

        return false;
    }

    if (state.finishCount != 1) {
        std::cerr << "Invalid level file: expected exactly one finish point but found " << state.finishCount << " in " << path << std::endl;

        return false;
    }
    
    return true;
}

bool validateFlagPole(const std::vector<std::string>& grid, const std::string& path) {
    std::size_t finishRow = 0;
    std::size_t finishColumn = 0;
    bool finishFound = false;

    for (std::size_t row = 0; row < grid.size(); ++row) {
        const std::size_t column = grid[row].find('F');

        if (column != std::string::npos) {
            finishRow = row;
            finishColumn = column;
            finishFound = true;
            break;
        }
    }

    if (!finishFound) {
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

bool TileMap::loadFromFile(const std::string& path) {
    std::ifstream inputFile(path);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open level file: " << path << std::endl;
        return false;
    }

    std::vector<std::string> loadedGrid;
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

        if (!validateRow(line, lineNumber, path, validationState)) {
            return false;
        }

        loadedGrid.push_back(line);
    }

    if (loadedGrid.empty()) {
        std::cerr << "Level file contains no map data: " << path << std::endl;
        return false;
    }
    
    if (!validateLevelMarkers(validationState, path)) {
        return false;
    }

    if (!validateFlagPole(loadedGrid, path)) {
        return false;
    }
    
    sf::Texture loadedTileset;
    try {
        loadedTileset = sf::Texture(std::string(TILESET_PATH));
    } catch (const sf::Exception& exception) {
        std::cerr << "Failed to load TileMap tileset: " << TILESET_PATH << std::endl;
        std::cerr << "Reason: " << exception.what() << std::endl;
        return false;
    }

    loadedTileset.setSmooth(false);

    const sf::Vector2u tilesetSize = loadedTileset.getSize();

    if (tilesetSize.x == 0 || tilesetSize.y == 0) {
        std::cerr << "Invalid TileMap tileset dimensions: " << tilesetSize.x << 'x' << tilesetSize.y << std::endl;
        return false;
    }

    m_pendingTileHits.clear();

    m_grid = std::move(loadedGrid);
    m_tileset = std::move(loadedTileset);
    buildVertices();
    return true;
}

void TileMap::render(sf::RenderTarget& target) const {
    sf::RenderStates states;
    states.texture = &m_tileset;

    target.draw(m_vertices, states);
}

void TileMap::renderForeground(sf::RenderTarget& target) const {
    sf::RenderStates states;
    states.texture = &m_tileset;

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
    return isEnemySupportTileSymbol(getTileAt(column, row));
}

bool TileMap::isSolid(int column, int row) const {
    return isSolidTileSymbol(getTileAt(column, row));
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

void TileMap::update(float dt) {
    if (m_bumpAnimations.empty()) return;

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
    }
}

void TileMap::buildVertices() {
    m_vertices.clear();
    m_foregroundVertices.clear();

    for (std::size_t row = 0; row < m_grid.size(); ++row) {
        for (std::size_t column = 0; column < m_grid[row].size(); ++column) {
            const char symbol = m_grid[row][column];

            if (!isRenderableTile(symbol)) {
                continue;
            }

            float offsetY = 0.f;
            for (const auto& bump : m_bumpAnimations) {
                if (bump.column == static_cast<int>(column) && bump.row == static_cast<int>(row)) {
                    float progress = bump.timer / bump.maxDuration;
                    offsetY = std::sin(progress * 3.14159265f) * bump.maxOffset;
                    break;
                }
            }

            const float left = static_cast<float>(column * TILE_SIZE);
            const float top = static_cast<float>(row * TILE_SIZE) + offsetY;

            const float right = left + static_cast<float>(TILE_SIZE);
            const float bottom = top + static_cast<float>(TILE_SIZE);

            const sf::IntRect rect = getTilesetRect(symbol);

            const float textureLeft = static_cast<float>(rect.position.x);
            const float textureTop = static_cast<float>(rect.position.y);

            const float textureRight = textureLeft + static_cast<float>(rect.size.x);
            const float textureBottom = textureTop + static_cast<float>(rect.size.y);

            // Route to correct vertex array based on layer classification
            sf::VertexArray& targetArray = isForegroundTile(symbol) ? m_foregroundVertices : m_vertices;

            // First triangle: top-left, bottom-left, bottom-right.
            appendTexturedVertex(targetArray, left, top,
                                 textureLeft, textureTop);

            appendTexturedVertex(targetArray, left, bottom,
                                 textureLeft, textureBottom);
            
            appendTexturedVertex(targetArray, right, bottom,
                                 textureRight, textureBottom);

            // Second triangle: top-left, bottom-right, top-right.
            appendTexturedVertex(targetArray, left, top,
                                 textureLeft, textureTop);
            
            appendTexturedVertex(targetArray, right, bottom,
                                 textureRight, textureBottom);
            
            appendTexturedVertex(targetArray, right, top,
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
            auto d1 = std::make_unique<BlockDebris>(center, sf::Vector2f(-120.f, -380.f), SpriteFrames::Blocks::DEBRIS_TOP_LEFT);
            auto d2 = std::make_unique<BlockDebris>(center, sf::Vector2f(120.f, -380.f), SpriteFrames::Blocks::DEBRIS_TOP_RIGHT);
            auto d3 = std::make_unique<BlockDebris>(center, sf::Vector2f(-80.f, -220.f), SpriteFrames::Blocks::DEBRIS_BOTTOM_LEFT);
            auto d4 = std::make_unique<BlockDebris>(center, sf::Vector2f(80.f, -220.f), SpriteFrames::Blocks::DEBRIS_BOTTOM_RIGHT);

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

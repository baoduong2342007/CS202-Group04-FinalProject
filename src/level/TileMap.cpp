/**
 * @file TileMap.cpp
 * @author TV4 (Vy)
 * @brief Implementation of TileMap loading, rendering, queries, and physics
 * @note Sprint 4 - validates level data and renders tiles from a tileset
 */

#include "level/TileMap.h"

#include <fstream>
#include <iostream>
#include <string_view>
#include <utility>

#include <box2d/box2d.h>
#include <SFML/System/Exception.hpp>

#include "physics/PhysicsEngine.h"
#include "entities/Entity.h"
#include "entities/BlockDebris.h"
#include "items/Mushroom.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "core/SpriteFrames.h"

namespace {

constexpr std::string_view VALID_TILE_SYMBOLS = ".1B?CGKMFS|[]{}UEO";
constexpr float TILE_FRICTION = 0.6f;
constexpr unsigned int TILESET_TILE_COUNT = 4;

std::vector<sf::Vector2i> s_pendingTileHits;
} // namespace

void TileMap::queueTileHit(int column, int row) {
    s_pendingTileHits.emplace_back(column, row);
}

void TileMap::processPendingHits(std::vector<std::unique_ptr<Entity>>& entities, TextureManager& textureManager, bool isBigMario, const sf::Vector2f& marioPos, float marioWidth) {
    if (s_pendingTileHits.empty()) return;

    sf::Vector2i bestGridPos = s_pendingTileHits.front();

    if (marioWidth > 0.f) {
        float marioLeft = marioPos.x;
        float marioRight = marioPos.x + marioWidth;
        float maxOverlap = -1.f;

        for (const auto& gridPos : s_pendingTileHits) {
            float tileLeft = static_cast<float>(gridPos.x * TILE_SIZE);
            float tileRight = tileLeft + static_cast<float>(TILE_SIZE);

            float overlap = std::max(0.f, std::min(marioRight, tileRight) - std::max(marioLeft, tileLeft));
            if (overlap > maxOverlap) {
                maxOverlap = overlap;
                bestGridPos = gridPos;
            }
        }
    }

    hitTile(bestGridPos.x, bestGridPos.y, isBigMario, entities, textureManager);
    s_pendingTileHits.clear();
}

namespace {

bool isBlankLine(const std::string& line){
    return line.find_first_not_of(" \t") == std::string::npos;
}

bool isCommentLine(const std::string& line){
    const std::size_t firstCharacter = line.find_first_not_of(" \t");

    return firstCharacter != std::string::npos && line[firstCharacter] == '#';
}

bool isValidTileSymbol(char symbol){
    return VALID_TILE_SYMBOLS.find(symbol) != std::string_view::npos;
}

bool isRenderableTile(char symbol){
    return symbol == '1' || symbol == 'B' || symbol == 'F' ||
           symbol == 'S' || symbol == '[' || symbol == ']' || symbol == '{' || symbol == '}' || symbol == '|' || symbol == 'E' || symbol == 'O';
}


constexpr std::string_view TILESET_PATH = "assets/textures/items/items_blocks.png";

sf::IntRect getTilesetRect(char symbol){
    switch (symbol){
        case '1':
        case 'S':
            return SpriteFrames::Blocks::BRICK;

        case 'B':
        case '[':
        case ']':
        case '{':
        case '}':
            return SpriteFrames::Blocks::BRICK;

        case '?':
        case 'U':
        case 'O':
            return SpriteFrames::Blocks::QUESTION1;

        case 'E':
            return SpriteFrames::Blocks::EMPTY;

        case 'F':
        case '|':
            return SpriteFrames::Blocks::EMPTY;

        default:
            return SpriteFrames::Blocks::BRICK;
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
                 LevelValidationState& state){
    if (state.expectedWidth == 0){
        state.expectedWidth = row.size();
    } else if (row.size() != state.expectedWidth){
        std::cerr << "Invalid level file: inconsistent row width at line " << lineNumber << " in " << path << std::endl;
        
        return false;
    }
    
    for (std::size_t column = 0; column < row.size(); ++column){
        const char symbol = row[column];

        if (!isValidTileSymbol(symbol)){
            std::cerr << "Invalid tile symbol '" << symbol << "' at line " << lineNumber << ", column " << column + 1 << " in " << path << std::endl;

            return false;
        }

        if (symbol == 'M'){
            ++state.marioSpawnCount;
        } else if (symbol == 'F'){
            ++state.finishCount;
        }
    }
    
    return true;
}

bool validateLevelMarkers(const LevelValidationState& state, const std::string& path){
    if (state.marioSpawnCount != 1){
        std::cerr << "Invalid level file: expected exactly one Mario spawn but found " << state.marioSpawnCount << " in " << path << std::endl;

        return false;
    }

    if (state.finishCount == 0){
        std::cerr << "Invalid level file: no finish flag found in " << path << std::endl;

        return false;
    }
    
    return true;
}

} // namespace

TileMap::~TileMap(){
    clearPhysicsBodies();
}

bool TileMap::loadFromFile(const std::string& path){
    std::ifstream inputFile(path);
    if (!inputFile.is_open()){
        std::cerr << "Failed to open level file: " << path << std::endl;
        return false;
    }

    std::vector<std::string> loadedGrid;
    LevelValidationState validationState;
    std::string line;
    std::size_t lineNumber = 0;

    while(std::getline(inputFile, line)){
        ++lineNumber;
        // Remove the carriage-return character from Windows CRLF files.
        if (!line.empty() && line.back() == '\r'){
            line.pop_back();
        }

        if (isBlankLine(line) || isCommentLine(line)){
            continue;
        }

        if (!validateRow(line, lineNumber, path, validationState)){
            return false;
        }

        loadedGrid.push_back(line);
    }

    if (loadedGrid.empty()){
        std::cerr << "Level file contains no map data: " << path << std::endl;
        return false;
    }
    
    if (!validateLevelMarkers(validationState, path)){
        return false;
    }
    sf::Texture loadedTileset;
    try {
        loadedTileset = sf::Texture(std::string(TILESET_PATH));
    } catch (const sf::Exception& exception){
        std::cerr << "Failed to load TileMap tileset: " << TILESET_PATH << std::endl;
        std::cerr << "Reason: " << exception.what() << std::endl;
        return false;
    }

    loadedTileset.setSmooth(false);

    const sf::Vector2u tilesetSize = loadedTileset.getSize();
    const unsigned int expectedWidth = TILE_SIZE * TILESET_TILE_COUNT;
    const unsigned int expectedHeight = TILE_SIZE;

    if (tilesetSize.x < expectedWidth || tilesetSize.y < expectedHeight){
#ifdef DEBUG
        std::cerr << "[TileMap] Warning: Tileset image size is " << tilesetSize.x << 'x' << tilesetSize.y << std::endl;
#endif
    }

    m_grid = std::move(loadedGrid);
    m_tileset = std::move(loadedTileset);
    buildVertices();
    return true;
}

void TileMap::render(sf::RenderWindow& window) const {
    sf::RenderStates states;
    states.texture = &m_tileset;

    window.draw(m_vertices, states);
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

bool TileMap::isSolid(int column, int row) const {
    const char tile = getTileAt(column, row);

    return tile == '1' || tile == 'B' || tile == 'U' || tile == 'O' || tile == 'E' ||
           tile == 'S' || tile == '[' || tile == ']' || tile == '{' || tile == '}';
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

void TileMap::buildVertices(){
    m_vertices.clear();

    for (std::size_t row = 0; row < m_grid.size(); ++row){
        for (std::size_t column = 0; column < m_grid[row].size(); ++column){
            const char symbol = m_grid[row][column];

            if (!isRenderableTile(symbol)){
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

            // First triangle: top-left, bottom-left, bottom-right.
            appendTexturedVertex(m_vertices, left, top,
                                 textureLeft, textureTop);

            appendTexturedVertex(m_vertices, left, bottom,
                                 textureLeft, textureBottom);
            
            appendTexturedVertex(m_vertices, right, bottom,
                                 textureRight, textureBottom);

            // Second triangle: top-left, bottom-right, top-right.
            appendTexturedVertex(m_vertices, left, top,
                                 textureLeft, textureTop);
            
            appendTexturedVertex(m_vertices, right, bottom,
                                 textureRight, textureBottom);
            
            appendTexturedVertex(m_vertices, right, top,
                                 textureRight, textureTop);
        }
    }
}

std::vector<sf::Vector2i> TileMap::findTiles(char symbol) const {
    std::vector<sf::Vector2i> positions;

    for (std::size_t row = 0; row < m_grid.size(); ++row){
        for (std::size_t column = 0; column < m_grid[row].size(); ++column){
            if (m_grid[row][column] == symbol){
                positions.emplace_back(static_cast<int>(column), static_cast<int>(row));
            }
        }
    }

    return positions;
}

sf::Vector2f TileMap::gridToWorldPosition(const sf::Vector2i& gridPosition){
    const float tileSize = static_cast<float>(TILE_SIZE);

    return {static_cast<float>(gridPosition.x) * tileSize,
        static_cast<float>(gridPosition.y) * tileSize
    };
}

void TileMap::createPhysicsBodies(b2World* world){
    if (!world){
        std::cerr << "Cannot create TileMap physics bodies: world is null" << std::endl;
        return;
    }
    
    clearPhysicsBodies();
    m_physicsWorld = world;
    
    const float tileSize = static_cast<float>(TILE_SIZE);
    
    for (std::size_t row = 0; row < m_grid.size(); ++row){
        for (std::size_t column = 0; column < m_grid[row].size(); ++column){
            if (!isSolid(static_cast<int>(column), static_cast<int>(row))){
                continue;
            }
            
            sf::Vector2f centerPixels(
                static_cast<float>(column) * tileSize + tileSize / 2.f,
                static_cast<float>(row) * tileSize + tileSize / 2.f
            );
            b2Vec2 centerMeters = PhysicsEngine::pixelsToMeters(centerPixels);

            b2BodyDef bodyDefinition;
            bodyDefinition.type = b2_staticBody;
            bodyDefinition.position.Set(centerMeters.x, centerMeters.y);

            uintptr_t packedData = TILE_USERDATA_FLAG |
                                   (static_cast<uintptr_t>(row) << 16) |
                                   static_cast<uintptr_t>(column);
            bodyDefinition.userData.pointer = packedData;
            
            b2Body* body = world->CreateBody(&bodyDefinition);
            
            float halfSizeMeters = PhysicsEngine::pixelsToMeters(tileSize / 2.f);
            b2PolygonShape shape;
            shape.SetAsBox(halfSizeMeters, halfSizeMeters);
            
            b2FixtureDef fixtureDefinition;
            fixtureDefinition.shape = &shape;
            fixtureDefinition.friction = TILE_FRICTION;

            body->CreateFixture(&fixtureDefinition);
            m_physicsBodies.push_back(body);
        }
    }
}

void TileMap::clearPhysicsBodies(){
    if (!m_physicsWorld){
        m_physicsBodies.clear();
        return;
    }

    for (b2Body* body : m_physicsBodies){
        if (body){
            m_physicsWorld->DestroyBody(body);
        }
    }

    m_physicsBodies.clear();
    m_physicsWorld = nullptr;
}

bool TileMap::hitTile(int column, int row, bool isBigMario, std::vector<std::unique_ptr<Entity>>& entities, TextureManager& textureManager) {
    if (row < 0 || row >= static_cast<int>(m_grid.size())) return false;
    if (column < 0 || column >= static_cast<int>(m_grid[row].size())) return false;

    char symbol = m_grid[row][column];

    // --- Question Block or Mushroom Block ('?' or 'U' or 'O') ---
    if (symbol == '?' || symbol == 'U' || symbol == 'O') {
        // Change tile to Empty Block 'E'
        m_grid[row][column] = 'E';
        buildVertices();
        triggerTileBump(column, row);

        // Spawn Mushroom above the block
        sf::Vector2f spawnPos = gridToWorldPosition({column, row - 1});
        MushroomType mushType = (symbol == 'O') ? MushroomType::ONE_UP : MushroomType::SUPER;
        auto mushroom = std::make_unique<Mushroom>(spawnPos, m_physicsWorld, mushType);
        mushroom->setTextureManager(textureManager);
        entities.push_back(std::move(mushroom));

        EventBus::getInstance().notify(EventType::PLAYER_POWER_UP);
        return true;
    }

    // --- Brick Block ('1' or 'B') ---
    if (symbol == '1' || symbol == 'B') {
        if (isBigMario) {
            // Shatter Brick Block
            m_grid[row][column] = '.';
            buildVertices();

            // Destroy physics body if world is unlocked
            sf::Vector2f tileWorldPos = gridToWorldPosition({column, row});
            if (m_physicsWorld && !m_physicsWorld->IsLocked()) {
                for (auto it = m_physicsBodies.begin(); it != m_physicsBodies.end(); ++it) {
                    b2Body* b = *it;
                    if (b) {
                        int cCol, cRow;
                        unpackTileCoords(b->GetUserData().pointer, cCol, cRow);
                        if (cCol == column && cRow == row) {
                            m_physicsWorld->DestroyBody(b);
                            m_physicsBodies.erase(it);
                            break;
                        }
                    }
                }
            }

            // Spawn 4 flying debris particles
            sf::Vector2f center = tileWorldPos + sf::Vector2f(8.f, 8.f);
            entities.push_back(std::make_unique<BlockDebris>(center, sf::Vector2f(-120.f, -380.f)));
            entities.push_back(std::make_unique<BlockDebris>(center, sf::Vector2f(120.f, -380.f)));
            entities.push_back(std::make_unique<BlockDebris>(center, sf::Vector2f(-80.f, -220.f)));
            entities.push_back(std::make_unique<BlockDebris>(center, sf::Vector2f(80.f, -220.f)));

            return true;
        } else {
            // Small Mario bump: Block bounces up slightly
            triggerTileBump(column, row);
            return true;
        }
    }

    return false;
}

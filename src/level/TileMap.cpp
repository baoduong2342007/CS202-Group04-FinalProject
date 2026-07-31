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

namespace {

constexpr std::string_view VALID_TILE_SYMBOLS = ".1B?SF|[]{}CGKM";

constexpr std::string_view TILESET_PATH = "assets/textures/tiles/tileset.png";

constexpr float TILE_FRICTION = 0.6f;
constexpr unsigned int TILESET_TILE_COUNT = 10;

constexpr unsigned int GROUND_TILE_INDEX = 0;
constexpr unsigned int BRICK_TILE_INDEX = 1;
constexpr unsigned int QUESTION_TILE_INDEX = 2;
constexpr unsigned int STONE_TILE_INDEX = 3;
constexpr unsigned int FLAG_TOP_TILE_INDEX = 4;
constexpr unsigned int FLAG_POLE_TILE_INDEX = 5;
constexpr unsigned int PIPE_TOP_LEFT_TILE_INDEX = 6;
constexpr unsigned int PIPE_BODY_LEFT_TILE_INDEX = 7;
constexpr unsigned int PIPE_TOP_RIGHT_TILE_INDEX = 8;
constexpr unsigned int PIPE_BODY_RIGHT_TILE_INDEX = 9;

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

bool isRenderableTile(char symbol) {
    return symbol == '1'
    || symbol == 'B'
    || symbol == '?'
    || symbol == 'S'
    || symbol == 'F'
    || symbol == '|'
    || symbol == '['
    || symbol == '{'
    || symbol == ']'
    || symbol == '}';
}

unsigned int getTilesetIndex(char symbol){
    switch (symbol){
        case '1':
            return GROUND_TILE_INDEX;

        case 'B':
            return BRICK_TILE_INDEX;

        case '?':
            return QUESTION_TILE_INDEX;

        case 'S':
            return STONE_TILE_INDEX;

        case 'F':
            return FLAG_TOP_TILE_INDEX;

        case '|':
            return FLAG_POLE_TILE_INDEX;

        case '[':
            return PIPE_TOP_LEFT_TILE_INDEX;

        case '{':
            return PIPE_BODY_LEFT_TILE_INDEX;

        case ']':
            return PIPE_TOP_RIGHT_TILE_INDEX;

        case '}':
            return PIPE_BODY_RIGHT_TILE_INDEX;

        default:
            return GROUND_TILE_INDEX;
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
        if (!line.empty() && line.back() == '\r'){line.pop_back();}
        if (isBlankLine(line) || isCommentLine(line)){continue;}
        if (!validateRow(line, lineNumber, path, validationState)){return false;}
        
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

    if (tilesetSize.x != expectedWidth || tilesetSize.y != expectedHeight){
        std::cerr << "Invalid TileMap tileset size: expected " << expectedWidth << 'x' << expectedHeight << ", but found " << tilesetSize.x << 'x' << tilesetSize.y << std::endl;
        return false;
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

    return tile == '1'
    || tile == 'B'
    || tile == '?'
    || tile == 'S'
    || tile == '['
    || tile == '{'
    || tile == ']'
    || tile == '}';
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

void TileMap::buildVertices(){
    m_vertices.clear();

    for (std::size_t row = 0; row < m_grid.size(); ++row){
        for (std::size_t column = 0; column < m_grid[row].size(); ++column){
            const char symbol = m_grid[row][column];

            if (!isRenderableTile(symbol)){
                continue;
            }

            const float left = static_cast<float>(column * TILE_SIZE);
            const float top = static_cast<float>(row * TILE_SIZE);

            const float right = left + static_cast<float>(TILE_SIZE);
            const float bottom = top + static_cast<float>(TILE_SIZE);

            const unsigned int tileIndex = getTilesetIndex(symbol);

            const float textureLeft = static_cast<float>(tileIndex * TILE_SIZE);
            const float textureTop = 0.f;

            const float textureRight = textureLeft + static_cast<float>(TILE_SIZE);
            const float textureBottom = static_cast<float>(TILE_SIZE);

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

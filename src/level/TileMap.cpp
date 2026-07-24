/**
 * @file TileMap.cpp
 * @author TV4 (Vy)
 * @brief Implementation of TileMap file loading and validation
 * @note Sprint 4 - validates ASCII level data before updating TileMap
 */

#include "level/TileMap.h"

#include <fstream>
#include <iostream>
#include <string_view>
#include <utility>

namespace {

constexpr std::string_view VALID_TILE_SYMBOLS = ".1B?CGKMF";

bool isBlankLine(const std::string& line){
    return line.find_first_not_of(" \t") == std::string::npos;
    
}

bool isCommentLine(const std::string& line){
    const std::size_t firstCharacter = line.find_first_not_of(" \t");
    
    return firstCharacter != std::string::npos and line[firstCharacter] == '#';
}

bool isValidTileSymbol(char symbol){
    return VALID_TILE_SYMBOLS.find(symbol) != std::string_view::npos;
}

bool isRenderableTile(char symbol){
    return symbol == '1' or symbol == 'B' or symbol == '?' or symbol == 'F';
}

sf::Color getPlaceholderColor(char symbol){
    switch (symbol){
        case '1': // Ground
            return sf::Color(120, 75, 40);
        
        case 'B': // Brick
            return sf::Color(190, 80, 45);
            
        case '?': // Question block
            return sf::Color(235, 180, 30);
        
        case 'F': // Finish flag
            return sf::Color(50, 190, 70);
            
        default:
            return sf::Color::Transparent;
    }
}

void appendColoredVertex(sf::VertexArray& vertices,
                         float x, float y,
                         const sf::Color& color){
    sf::Vertex vertex;
    vertex.position = {x, y};
    vertex.color = color;
    
    vertices.append(vertex);
}

}

bool TileMap::loadFromFile(const std::string& path){
    std::ifstream inputFile(path);
    
    if (!inputFile.is_open()){
        std::cerr << "Failed to open level file: " << path << std::endl;
        return false;
    }
    
    std::vector<std::string> loadedGrid;
    
    std::size_t expectedWidth = 0;
    std::size_t finishCount = 0;
    std::size_t lineNumber = 0;
    std::size_t marioSpawnCount = 0;
        
    std::string line;
    
    while(std::getline(inputFile, line)){
        ++lineNumber;
        
        // Remove the carriage-return character from Windows CRLF files.
        if (!line.empty() and line.back() == '\r'){
            line.pop_back();
        }
        
        if (isBlankLine(line) or isCommentLine(line)){
            continue;
        }
        
        if (expectedWidth == 0){
            expectedWidth = line.size();
        } else if (line.size() != expectedWidth){
            std::cerr << "Invalid level file: inconsistent row width at line " << lineNumber << " in " << path << std::endl;
            
            return false;
        }
        
        for (std::size_t column = 0; column < line.size(); ++ column){
            const char symbol = line[column];
            
            if (!isValidTileSymbol(symbol)){
                std::cerr << "Invalid tile symbol '" << symbol << "' at line " << lineNumber << ", column " << column + 1 << " in " << path << std::endl;
                
                return false;
            }
            
            if (symbol == 'M'){
                ++marioSpawnCount;
            } else if (symbol == 'F'){
                ++finishCount;
            }
        }
        
        loadedGrid.push_back(line);
    }
    
    if (loadedGrid.empty()){
        std::cerr << "Level file contains no map data: " << path << std::endl;
        
        return false;
    }
    
    if (marioSpawnCount != 1){
        std::cerr << "Invalid level file: expected exactly one Mario spawn but found " << marioSpawnCount << " in " << path << std::endl;
        
        return false;
    }
    
    if (finishCount == 0){
        std::cerr << "Invalid level file: no finish flag found in " << path << std::endl;
        
        return false;
    }
    
    m_grid = std::move(loadedGrid);
    buildVertices();
    
    return true;
}

void TileMap::render(sf::RenderWindow& window) const {
    window.draw(m_vertices);
}

char TileMap::getTileAt(int column, int row) const {
    if (column < 0 or row < 0){
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
    
    return tile == '1' or tile == 'B' or tile == '?';
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
            
            const sf::Color tileColor = getPlaceholderColor(symbol);
            
            // First triangle: top-left, bottom-left, bottom-right.
            appendColoredVertex(m_vertices, left, top, tileColor);
            appendColoredVertex(m_vertices, left, bottom, tileColor);
            appendColoredVertex(m_vertices, right, bottom, tileColor);

            // Second triangle: top-left, bottom-right, top-right.
            appendColoredVertex(m_vertices, left, top, tileColor);
            appendColoredVertex(m_vertices, right, bottom, tileColor);
            appendColoredVertex(m_vertices, right, top, tileColor);
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

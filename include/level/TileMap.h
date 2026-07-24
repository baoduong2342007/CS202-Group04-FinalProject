/**
 * @file TileMap.h
 * @author TV4 (Vy)
 * @brief TileMap management: load, query, and render level tiles
 * @note Week 2 skeleton setup
 */

#pragma once

#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

class TileMap {
public:
    TileMap() = default;
    ~TileMap() = default;
    
    bool loadFromFile(const std::string& path);
    void render(sf::RenderWindow& window);
    
    char getTileAt(int column, int row) const;
    bool isSolid(int column, int row) const;
    
private:
    std::vector<std::string> m_grid;
    sf::Texture m_tileset;
};

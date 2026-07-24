/**
 * @file TileMap.h
 * @author TV4 (Vy)
 * @brief TileMap management: load, query, and render level tiles
 * @note Week 2 skeleton setup
 */

#pragma once

#include <string>
#include <vector>
#include <cstddef>

#include <SFML/Graphics.hpp>

class TileMap {
public:
    TileMap() = default;
    ~TileMap() = default;
    
    bool loadFromFile(const std::string& path);
    
    void render(sf::RenderWindow& window) const;
    
    char getTileAt(int column, int row) const;
    bool isSolid(int column, int row) const;
    
    std::size_t getWidth() const;
    std::size_t getHeight() const;
    
private:
    static constexpr unsigned int TILE_SIZE = 32;
    
    void buildVertices();

    std::vector<std::string> m_grid;
    sf::VertexArray m_vertices{sf::PrimitiveType::Triangles};
    sf::Texture m_tileset;
};

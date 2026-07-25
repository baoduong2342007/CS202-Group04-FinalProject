/**
 * @file TileMap.h
 * @author TV4 (Vy)
 * @brief TileMap management: load, query, and render level tiles
 * @note Sprint 4 - level loading, validation, queries, and placeholder rendering
 */

#pragma once

#include <cstddef>
#include <string>
#include <vector>

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

    std::vector<sf::Vector2i> findTiles(char symbol) const;

    static sf::Vector2f gridToWorldPosition(const sf::Vector2i& gridPosition);

private:
    static constexpr unsigned int TILE_SIZE = 32;

    void buildVertices();

    std::vector<std::string> m_grid;
    sf::VertexArray m_vertices{sf::PrimitiveType::Triangles};
    sf::Texture m_tileset;
};

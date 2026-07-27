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

class b2Body;
class b2World;

class TileMap {
public:
    TileMap() = default;
    ~TileMap();
    TileMap(const TileMap&) = delete;
    TileMap& operator=(const TileMap&) = delete;
    TileMap(TileMap&&) = delete;
    TileMap& operator=(TileMap&&) = delete;

    bool loadFromFile(const std::string& path);

    void render(sf::RenderWindow& window) const;

    char getTileAt(int column, int row) const;
    bool isSolid(int column, int row) const;

    std::size_t getWidth() const;
    std::size_t getHeight() const;

    std::vector<sf::Vector2i> findTiles(char symbol) const;

    static sf::Vector2f gridToWorldPosition(const sf::Vector2i& gridPosition);
    
    void createPhysicsBodies(b2World* world);
    
private:
    static constexpr unsigned int TILE_SIZE = 32;

    void buildVertices();
    void clearPhysicsBodies();

    std::vector<std::string> m_grid;
    sf::VertexArray m_vertices{sf::PrimitiveType::Triangles};
    sf::Texture m_tileset;
    b2World* m_physicsWorld{nullptr};
    std::vector<b2Body*> m_physicsBodies;
};

/**
 * @file TileMap.h
 * @author TV4 (Vy)
 * @brief TileMap management: load, validate, query, render, and create physics
 * @note Sprint 4 - textured tile rendering using a 10-frame tileset
 */

#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

class b2Body;
class b2World;
class Mario;

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
    
    // UserData packing constants for Box2D tile bodies
    static constexpr uintptr_t TILE_USERDATA_FLAG = 0x8000000000000000ULL;
    static bool isTileUserData(uintptr_t ptr) { return (ptr & TILE_USERDATA_FLAG) != 0; }
    static void unpackTileCoords(uintptr_t ptr, int& outCol, int& outRow) {
        outRow = static_cast<int>((ptr >> 16) & 0xFFFF);
        outCol = static_cast<int>(ptr & 0xFFFF);
    }

    void queueTileHit(int column, int row, float overlap = 0.f);
    void processPendingHits(std::vector<std::unique_ptr<class Entity>>& entities, class TextureManager& textureManager, bool isBigMario, Mario* mario = nullptr);

    bool hitTile(int column, int row, bool isBigMario, std::vector<std::unique_ptr<class Entity>>& entities, class TextureManager& textureManager);
    
    void update(float dt);
    void triggerTileBump(int column, int row);

private:
    static constexpr unsigned int TILE_SIZE = 32;

    struct PendingTileHit {
        sf::Vector2i gridPosition;
        float overlap;
    };

    struct TileBump {
        int column;
        int row;
        float timer{0.f};
        float maxDuration{0.16f};
        float maxOffset{-12.f};
    };

    void buildVertices();
    void clearPhysicsBodies();

    std::vector<std::string> m_grid;
    sf::VertexArray m_vertices{sf::PrimitiveType::Triangles};
    sf::Texture m_tileset;
    b2World* m_physicsWorld{nullptr};
    std::vector<b2Body*> m_physicsBodies;
    std::vector<TileBump> m_bumpAnimations;
    std::vector<PendingTileHit> m_pendingTileHits;
};

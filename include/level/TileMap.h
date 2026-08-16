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
#include <optional>

#include <SFML/Graphics.hpp>
#include "core/LevelCatalog.h"

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

    enum class WarpEntryType {
        HORIZONTAL,
        VERTICAL,
        PIRANHA
    };

    struct WarpEntry {
        char id;
        sf::Vector2i position;
        WarpEntryType type;
    };

    struct WarpReturn {
        char id;
        sf::Vector2i position;
    };

    /// One moving elevator declared directly in a level map.
    /// Use '^' for the start marker and '~' for the end marker.
    /// Markers must share a column for vertical movement or a row for
    /// horizontal movement; both markers are converted to empty tiles after
    /// loading. The active level theme supplies the platform appearance.
    /// Do not duplicate the same route in levels/elevators.txt.
    /// vertical == true when both markers share the same column.
    struct ElevatorRoute {
        sf::Vector2i start;
        sf::Vector2i end;
        bool vertical;
    };

    /// One moving Cheep Cheep route declared directly in a level map.
    /// Use 'c' for the start marker and 'x' for the end marker.
    /// Markers must share a column for vertical movement (up & down) or a row
    /// for horizontal movement (left & right); both markers are converted to
    /// empty tiles after loading.
    struct CheepCheepRoute {
        sf::Vector2i start;
        sf::Vector2i end;
        bool vertical;
    };

    const std::vector<WarpEntry>& getWarpEntries() const {
        return m_warpEntries;
    }

    const std::vector<ElevatorRoute>& getElevatorRoutes() const {
        return m_elevatorRoutes;
    }

    const std::vector<CheepCheepRoute>& getCheepCheepRoutes() const {
        return m_cheepCheepRoutes;
    }

    std::optional<sf::Vector2i> findWarpReturn(char id) const;

    void setTheme(LevelTheme theme);

    /// Validation contract of the layout being loaded.
    /// CAMPAIGN requires exactly one 'M', one 'F' and one 'T' with a valid
    /// flagpole. PVP_ARENA requires exactly one 'M' (player one spawn), one
    /// 'm' (player two spawn) and one 'W' (fire flower pedestal), and allows
    /// no finish markers.
    enum class LayoutMode {
        CAMPAIGN,
        PVP_ARENA
    };

    bool loadFromFile(const std::string& path,
                      LayoutMode mode = LayoutMode::CAMPAIGN);

    void render(sf::RenderTarget& target) const;
    /// Draw the flag cloth between the world layer and Mario/entities.
    void renderFlags(sf::RenderTarget& target) const;
    void renderForeground(sf::RenderTarget& target) const;

    char getTileAt(int column, int row) const;
    bool isEnemySupport(int column, int row) const;
    bool isSolid(int column, int row) const;
    bool isClimbable(int column, int row) const;

    std::size_t getWidth() const;
    std::size_t getHeight() const;

    std::vector<sf::Vector2i> findTiles(char symbol) const;
    float getFlagDropDistance() const { return m_flagDropDistance; }

    static sf::Vector2f gridToWorldPosition(const sf::Vector2i& gridPosition);
    
    void createPhysicsBodies(b2World* world);
    /// Release tile bodies while their owning Box2D world is still alive.
    void destroyPhysicsBodies();
    
    // UserData packing constants for Box2D tile bodies
    static constexpr uintptr_t TILE_USERDATA_FLAG = 0x8000000000000000ULL;
    static bool isTileUserData(uintptr_t ptr) {
        return (ptr & TILE_USERDATA_FLAG) != 0;
    }
    static void unpackTileCoords(uintptr_t ptr, int& outCol, int& outRow) {
        outRow = static_cast<int>((ptr >> 16) & 0xFFFF);
        outCol = static_cast<int>(ptr & 0xFFFF);
    }

    void queueTileHit(int column, int row, float overlap = 0.f);
    void processPendingHits(std::vector<std::unique_ptr<class Entity>>& entities,
                            class TextureManager& textureManager,
                            bool isBigMario,
                            Mario* mario = nullptr
                            );

    bool hitTile(int column, int row, bool isBigMario,
                 std::vector<std::unique_ptr<class Entity>>& entities,
                 class TextureManager* textureManager = nullptr
                 );
    
    void update(float dt);
    void triggerTileBump(int column, int row);

    /// Move the rendered flag down the validated pole without changing map
    /// markers or collision geometry. The stored value is clamped to the
    /// maximum validated drop for the pole.
    void setFlagDropDistance(float distancePixels);
    /// Maximum downward travel (px) the flag can reach on the validated pole.
    float getFlagMaxDropDistance() const;
    /// True once the flag cloth has reached the bottom of the pole body.
    bool isFlagFullyDropped() const;

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
    void buildFlagVertices();
    void clearPhysicsBodies();

    std::vector<std::string> m_grid;
    sf::VertexArray m_vertices{sf::PrimitiveType::Triangles};
    sf::VertexArray m_foregroundVertices{sf::PrimitiveType::Triangles};
    sf::VertexArray m_objectVertices{sf::PrimitiveType::Triangles};
    sf::VertexArray m_flagVertices{sf::PrimitiveType::Triangles};
    sf::Texture m_tileset;
    sf::Texture m_objectsTileset;
    float m_flagAnimationTime{0.0f};
    float m_flagDropDistance{0.0f};
    b2World* m_physicsWorld{nullptr};
    std::vector<b2Body*> m_physicsBodies;
    std::vector<TileBump> m_bumpAnimations;
    std::vector<PendingTileHit> m_pendingTileHits;
    LevelTheme m_theme{LevelTheme::OVERWORLD};
    
    std::vector<WarpEntry> m_warpEntries;
    std::vector<WarpReturn> m_warpReturns;
    std::vector<ElevatorRoute> m_elevatorRoutes;
    std::vector<CheepCheepRoute> m_cheepCheepRoutes;
};

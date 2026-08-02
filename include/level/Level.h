/**
 * @file Level.h
 * @author TV1 (Dương)
 * @brief Manages all entities within a single game level
 * @note Sprint 4 — owns TileMap, Mario, Camera, and all spawned entities
 */

#pragma once

#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "entities/Entity.h"
#include "entities/Mario.h"
#include "level/TileMap.h"
#include "level/Camera.h"

class Level {
public:
    // 1. Constructor / Destructor
    Level();
    ~Level() = default;

    // 3. Public methods
    bool loadFromFile(const std::string& path);
    void update(float dt);
    void render(sf::RenderWindow& window);

    // 4. Getters / Setters
    Mario& getMario();
    const Mario& getMario() const;
    TileMap& getTileMap();
    Camera& getCamera();

private:
    // 5. Private methods
    void spawnEntitiesFromTileMap();
    void removeDeadEntities();

    // 6. Private members
    TileMap m_tileMap;
    Camera m_camera;
    std::unique_ptr<Mario> m_mario;
    std::vector<std::unique_ptr<Entity>> m_entities;
};

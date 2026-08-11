/**
 * @file Level.h
 * @author TV1 (Dương)
 * @brief Manages all entities within a single game level
 * @note Sprint 4 — owns TileMap, Mario, Camera, TextureManager, and all spawned entities
 */

#pragma once

#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "core/TextureManager.h"
#include "entities/Entity.h"
#include "entities/Mario.h"
#include "level/Camera.h"
#include "level/TileMap.h"

#include <box2d/box2d.h>
class ContactListener;

#include "entities/FireBallPool.h"

class Level {
public:
    // 1. Constructor / Destructor
    Level();
    ~Level();

    // 2. Override methods
    // (None — Level does not inherit from a base class)

    // 3. Public methods
    void setTheme(LevelTheme theme);
    bool loadFromFile(const std::string& path);
    void update(float dt);
    void render(sf::RenderTarget& target);
    void shootFireBall();

    /// Spawn one FireBall owned by Mario. Returns false when FIRE/cooldown/pool
    /// rules reject the request.
    bool spawnFireBall();

    /// Spawn a FireballExplosion particle effect at the specified position.
    void spawnFireballExplosion(const sf::Vector2f& position);

    // 4. Getters / Setters
    Mario* getMario();
    const Mario* getMario() const;
    TileMap& getTileMap();
    Camera& getCamera();
    TextureManager& getTextureManager();
    std::vector<std::unique_ptr<Entity>>& getEntities() { return m_entities; }
    FireBallPool& getFireBallPool() { return m_fireBallPool; }
    bool isLevelCompleted() const;


private:
    struct FireBallSpawnRequest {
        sf::Vector2f position;
        Direction direction;
    };

    // 5. Private methods
    void spawnEntitiesFromTileMap();
    void removeDeadEntities();
    void checkItemCollisions();
    void checkFinishFlag();
    void processPendingFireballs();

    // 6. Private members
    std::unique_ptr<b2World> m_world;
    std::unique_ptr<ContactListener> m_contactListener;
    TileMap m_tileMap;
    Camera m_camera;
    TextureManager& m_textureManager;
    std::unique_ptr<Mario> m_mario;
    std::vector<std::unique_ptr<Entity>> m_entities;
    FireBallPool m_fireBallPool{2};
    std::vector<FireBallSpawnRequest> m_pendingFireBallRequests;
    bool m_levelCompleted = false;
    std::string m_levelPath;
    float m_physicsAccumulator = 0.0f;
    LevelTheme m_theme{LevelTheme::OVERWORLD};
};

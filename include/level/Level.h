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

class Level {
public:
    // 1. Constructor / Destructor
    Level();
    ~Level();

    // 2. Override methods
    // (None — Level does not inherit from a base class)

    // 3. Public methods
    void setTheme(LevelTheme theme);
    void setCameraVerticalMode(CameraVerticalMode mode);
    bool loadFromFile(const std::string& path);
    void update(float dt);
    void render(sf::RenderTarget& target);
    /// Consolidated production entry point for FireBall shooting.
    /// Checks FIRE state, cooldown, hard limit of two, and safe world-lock queueing.
    bool requestFireBallShot(Mario& mario);
    bool requestFireBallShot();

    /// Spawn a FireballExplosion particle effect at the specified position.
    void spawnFireballExplosion(const sf::Vector2f& position);

    // 4. Getters / Setters
    Mario* getMario();
    const Mario* getMario() const;
    TileMap& getTileMap();
    Camera& getCamera();
    TextureManager& getTextureManager();
    std::vector<std::unique_ptr<Entity>>& getEntities() { return m_entities; }
    std::size_t getActiveFireBallCount() const;
    std::size_t getPendingFireBallCount() const {
        return m_pendingFireBallRequests.size();
    }
    bool isLevelCompleted() const;
    /// True while the flagpole slide and the automatic walk into the exit are running.
    bool isFlagSequenceActive() const { return m_flagSequenceActive; }


private:
    /// Flagpole outcome sequence driven by actual Mario/flag state instead of
    /// a rough time estimate (so a large dt spike cannot skip the slide).
    enum class FlagPhase {
        NONE,
        SLIDING,
        WAITING_FLAG_DROP,
        WALKING
    };

    struct FireBallSpawnRequest {
        sf::Vector2f position;
        Direction direction;
        Mario* owner;
    };

    // 5. Private methods
    void spawnEntitiesFromTileMap();
    void removeDeadEntities();
    void checkItemCollisions();
    void checkFinishFlag();
    void processPendingFireballs();
    
    void checkPipeWarps();
    void warpMarioToReturn(char warpId);

    bool isPiranhaAliveAt(const sf::Vector2i& pipePosition) const;
    void suppressPiranhaAt(const sf::Vector2i& pipePosition);

    // 6. Private members
    std::unique_ptr<b2World> m_world;
    std::unique_ptr<ContactListener> m_contactListener;
    TileMap m_tileMap;
    Camera m_camera;
    TextureManager& m_textureManager;
    std::unique_ptr<Mario> m_mario;
    std::vector<std::unique_ptr<Entity>> m_entities;
    std::vector<FireBallSpawnRequest> m_pendingFireBallRequests;
    bool m_levelCompleted = false;
    bool m_flagSequenceActive = false;
    bool m_flagWalkActive = false;
    FlagPhase m_flagPhase = FlagPhase::NONE;
    float m_flagWalkTargetX = 0.0f;
    float m_flagSlideStartMarioY = 0.0f;
    float m_flagSlideStartDropDistance = 0.0f;
    std::string m_levelPath;
    float m_physicsAccumulator = 0.0f;
    LevelTheme m_theme{LevelTheme::OVERWORLD};
    CameraVerticalMode m_cameraVerticalMode{CameraVerticalMode::LOCKED};
    float m_pipeWarpCooldown = 0.0f;
};

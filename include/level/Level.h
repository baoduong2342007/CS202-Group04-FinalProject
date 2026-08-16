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
#include "level/CheepCheepConfig.h"
#include "physics/CollisionManager.h"

#include <box2d/box2d.h>
class ContactListener;
class FireFlower;

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
    bool loadFromFile(const std::string& path,
                      CharacterType characterType = CharacterType::MARIO);
    /// Load a two-fighter duel arena ('M' = player one, 'm' = player two,
    /// 'W' = fire flower pedestal). Switches this Level into PvP mode, where
    /// both fighters are simulated, deferred duel contacts are applied, and
    /// the campaign-only systems (flagpole, pipes, generators) stay dormant.
    bool loadPvpArena(const std::string& path,
                      CharacterType playerOne,
                      CharacterType playerTwo);
    void update(float dt);
    void render(sf::RenderTarget& target);
    /// Consolidated production entry point for FireBall shooting.
    /// Checks FIRE state, cooldown, hard limit of two, and safe world-lock queueing.
    bool requestFireBallShot(Mario& mario);
    bool requestFireBallShot();

    /// Spawn a FireballExplosion particle effect at the specified position.
    void spawnFireballExplosion(const sf::Vector2f& position);
    /// Spawn a deferred world-space stomp-score popup.
    void spawnScorePopup(const StompScoreAward& award);

    // --- PvP duel support -------------------------------------------------
    /// Place the contested fire flower on the arena. Only one flower may be
    /// on the field at a time; returns nullptr when one already is.
    FireFlower* spawnPvpFireFlower(const sf::Vector2f& position);
    /// Remove the on-field fire flower (round reset). No-op when absent.
    void clearPvpFireFlower();
    bool hasPvpFireFlowerOnField() const { return m_pvpFireFlower != nullptr; }
    /// Drain fireball hits forwarded from the deferred duel-contact queue.
    /// The round state machine owns the knockback/stun policy for these.
    std::vector<PvpHit> consumePvpFireballHits();

    // 4. Getters / Setters
    Mario* getMario();
    const Mario* getMario() const;
    Mario* getMario2();
    const Mario* getMario2() const;
    bool isPvpMode() const { return m_pvpMode; }
    CharacterType getCharacterType() const { return m_characterType; }
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
        TURNING_RIGHT,
        WALKING
    };

    enum class PipeWarpPhase {
        NONE,
        ENTERING_VERTICAL,
        ENTERING_HORIZONTAL,
        WARPING_DELAY,
        EXITING_VERTICAL
    };

    struct FireBallSpawnRequest {
        sf::Vector2f position;
        Direction direction;
        Mario* owner;
    };

    // 5. Private methods
    void spawnEntitiesFromTileMap();
    void spawnElevatorsFromTileMap();
    void spawnCheepCheepRoutesFromTileMap();
    void spawnCheepCheepsFromConfig();
    void updateCheepCheepGenerators(float dt);
    void updateFlagSequence(float dt);
    void updatePvp(float dt);
    void processPendingPvpHits();
    void updateEntities(float dt);
    void updateExplosions();
    void removeDeadEntities();
    void checkItemCollisions();
    void checkItemCollisionFor(Mario& player);
    void checkFinishFlag();
    void processPendingFireballs();
    void processPendingStompScorePopups();
    
    void checkPipeWarps();
    void startPipeWarp(char warpId, PipeWarpPhase phase, const sf::Vector2i& pipeTile);
    void updatePipeWarp(float dt);
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
    /// Second PvP fighter; null in the campaign single-player flow.
    std::unique_ptr<Mario> m_mario2;
    std::vector<std::unique_ptr<Entity>> m_entities;
    std::vector<FireBallSpawnRequest> m_pendingFireBallRequests;
    bool m_pvpMode = false;
    /// Non-owning handle to the contested fire flower (owned by m_entities).
    FireFlower* m_pvpFireFlower = nullptr;
    /// Fireball hits drained from the duel queue, forwarded to the round FSM.
    std::vector<PvpHit> m_pvpFireballHits;
    bool m_levelCompleted = false;
    bool m_flagSequenceActive = false;
    bool m_flagWalkActive = false;
    FlagPhase m_flagPhase = FlagPhase::NONE;
    float m_flagWalkTargetX = 0.0f;
    float m_flagPoleCenterX = 0.0f;
    float m_flagTurnTargetX = 0.0f;
    float m_flagSlideStartMarioY = 0.0f;
    float m_flagSlideStartDropDistance = 0.0f;
    std::string m_levelPath;
    float m_physicsAccumulator = 0.0f;
    CharacterType m_characterType = CharacterType::MARIO;
    LevelTheme m_theme{LevelTheme::OVERWORLD};
    CameraVerticalMode m_cameraVerticalMode{CameraVerticalMode::LOCKED};
    float m_pipeWarpCooldown = 0.0f;
    PipeWarpPhase m_pipeWarpPhase = PipeWarpPhase::NONE;
    float m_pipeWarpTimer = 0.0f;
    float m_pipeWarpExitTargetY = 0.0f;
    char m_pendingWarpId = '0';
    sf::Vector2i m_pendingPipeTile{0, 0};
    std::vector<CheepCheepGeneratorConfig> m_activeGenerators;
    std::vector<float> m_generatorTimers;
};

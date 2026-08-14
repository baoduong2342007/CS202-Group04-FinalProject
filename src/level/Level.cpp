/**
 * @file Level.cpp
 * @author TV1 (Dương)
 * @brief Level implementation — loads map, spawns entities, orchestrates update/render
 * @note Sprint 4 fix: TextureManager wired to entities, item collision, dead entity cleanup
 */

#include "level/Level.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <cassert>

#include "items/Item.h"
#include "items/Coin.h"
#include "patterns/EntityFactory.h"
#include "patterns/EventBus.h"
#include "physics/PhysicsEngine.h"
#include "physics/ContactListener.h"
#include "entities/Enemy.h"
#include "entities/Elevator.h"
#include "level/ElevatorConfig.h"
#include "level/CheepCheepConfig.h"
#include "entities/PiranhaPlant.h"
#include "entities/CheepCheep.h"
#include "entities/FireBall.h"
#include "entities/FireballExplosion.h"
#include "core/SpriteFrames_ovw.h"
#include "core/LevelCatalog.h"
#include "core/SoundManager.h"

#include "core/DisplayConfig.h"

namespace {

constexpr unsigned int TILE_SIZE = 32;
// Elevator markers occupy one 32 px map cell, while the platform is 64 px
// wide. Treat the marker as the platform's center anchor instead of its left
// edge so a lift sits centered in the opening shown by the level layout.
constexpr float ELEVATOR_MARKER_X_OFFSET =
    -static_cast<float>(TILE_SIZE) / 2.0f;
constexpr float FLAGPOLE_WALK_SPEED = 150.0f;
constexpr float ENEMY_ACTIVATION_MARGIN = 64.f;
constexpr float ENTITY_CLEANUP_MARGIN = 64.f;
const sf::Color UNDERGROUND_BACKGROUND_COLOR(0, 0, 128);
const sf::Color UNDERWATER_BACKGROUND_COLOR(0, 48, 112);
const sf::Color CASTLE_BACKGROUND_COLOR(28, 8, 36);

bool shouldActivateEnemy(const Enemy& enemy, const sf::View& cameraView) {
    const sf::Vector2f cameraCenter = cameraView.getCenter();

    const sf::Vector2f cameraSize = cameraView.getSize();

    const float cameraLeft = cameraCenter.x - cameraSize.x / 2.f;
    const float activationRight = cameraCenter.x + cameraSize.x / 2.f + ENEMY_ACTIVATION_MARGIN;

    const sf::FloatRect enemyBounds = enemy.getBoundingBox();

    const float enemyLeft = enemyBounds.position.x;
    const float enemyRight = enemyBounds.position.x + enemyBounds.size.x;

    return enemyRight >= cameraLeft && enemyLeft <= activationRight;
}

bool isEntityOutsideLevelBounds(const Entity& entity, float levelWidth, float levelHeight) {
    const sf::FloatRect bounds = entity.getBoundingBox();

    const float left = bounds.position.x;
    const float right = bounds.position.x + bounds.size.x;

    const float top = bounds.position.y;
    const float bottom = bounds.position.y + bounds.size.y;

    return right < -ENTITY_CLEANUP_MARGIN ||
           left > levelWidth + ENTITY_CLEANUP_MARGIN ||
           bottom < -ENTITY_CLEANUP_MARGIN ||
           top > levelHeight + ENTITY_CLEANUP_MARGIN;
}

// Tile codes that represent spawnable standalone entities (Goomba, Koopa, PiranhaPlant, CheepCheep, Coin, QuestionBlock, Springboard)
constexpr char SPAWN_CODES[] = {'G', 'K', 'p', 'c', 'C', '?', 'f', 'h', 'U', 'u', 'O', 'o', 'J', 'e'};

std::size_t findGroundSurfaceRow(const TileMap& tileMap) {
    const std::size_t height = tileMap.getHeight();
    const std::size_t width = tileMap.getWidth();
    if (height == 0 || width == 0) {
        return 0;
    }

    // The floor can be one or several rows thick. A floor row is identified by
    // its dominant ground-tile coverage; sparse platforms above it must not
    // move the background down.
    const auto isFloorRow = [&tileMap, width](std::size_t row) {
        std::size_t groundTiles = 0;
        for (std::size_t column = 0; column < width; ++column) {
            const char tile = tileMap.getTileAt(static_cast<int>(column), static_cast<int>(row));
            if (tile == '0' || tile == '1') {
                ++groundTiles;
            }
        }
        return groundTiles * 2 >= width;
    };

    std::size_t surfaceRow = height - 1;
    while (surfaceRow > 0 && isFloorRow(surfaceRow - 1)) {
        --surfaceRow;
    }
    return surfaceRow;
}

float calculateBackgroundTop(const TileMap& tileMap) {
    const float groundTop = static_cast<float>(findGroundSurfaceRow(tileMap) * TILE_SIZE);
    const float backgroundHeight = static_cast<float>(DisplayConfig::LOGICAL_HEIGHT);
    // Do not clamp this to zero: short levels can legitimately place the full
    // background frame above world Y=0 while the camera is already inside it.
    return groundTop - backgroundHeight;
}

} // namespace


Level::Level() : m_textureManager(TextureManager::getInstance()) {}

Level::~Level() {
    // Entity and tile bodies must be destroyed before the Box2D world.  This
    // also keeps TileMap's destructor from observing a dead world pointer.
    m_entities.clear();
    m_mario.reset();
    m_tileMap.destroyPhysicsBodies();
    m_contactListener.reset();
    m_world.reset();
}
void Level::setTheme(LevelTheme theme) {
    m_theme = theme;
    m_tileMap.setTheme(theme);
}

void Level::setCameraVerticalMode(CameraVerticalMode mode) {
    m_cameraVerticalMode = mode;
    m_camera.setVerticalMode(mode);
}

bool Level::loadFromFile(const std::string& path, CharacterType characterType) {
    m_characterType = characterType;
    m_entities.clear();
    m_pendingFireBallRequests.clear();
    m_mario.reset();
    m_tileMap.destroyPhysicsBodies();
    m_contactListener.reset();
    m_world.reset();
    m_levelCompleted = false;
    m_flagSequenceActive = false;
    m_flagWalkActive = false;
    m_flagPhase = FlagPhase::NONE;
    m_flagWalkTargetX = 0.0f;
    m_flagSlideStartMarioY = 0.0f;
    m_flagSlideStartDropDistance = 0.0f;
    m_physicsAccumulator = 0.0f;
    m_levelPath = path;
    
    if (!m_tileMap.loadFromFile(path)) {
        std::cerr << "Level: Failed to load TileMap from " << path << std::endl;
        return false;
    }
    
    // Initialize camera with screen size and level pixel-bounds
    const float levelWidth = static_cast<float>(m_tileMap.getWidth() * TILE_SIZE);
    const float levelHeight = static_cast<float>(m_tileMap.getHeight() * TILE_SIZE);
    
    m_camera.init(
        sf::Vector2f(static_cast<float>(DisplayConfig::LOGICAL_WIDTH),
                     static_cast<float>(DisplayConfig::LOGICAL_HEIGHT)),
        sf::FloatRect(sf::Vector2f(0.f, 0.f),
                      sf::Vector2f(levelWidth, levelHeight))
    );
    m_camera.setVerticalMode(m_cameraVerticalMode);
    // Must be called BEFORE spawnEntitiesFromTileMap() so entities have ground to land on
    const float gravity = (m_theme == LevelTheme::UNDERWATER) ? 8.0f : 25.0f;
    m_world = std::make_unique<b2World>(b2Vec2(0.f, gravity));
    m_contactListener = std::make_unique<ContactListener>(m_tileMap);
    m_world->SetContactListener(m_contactListener.get());
    
    if (m_world) {
        m_tileMap.createPhysicsBodies(m_world.get());
    }
    
    // Spawn Mario and all entities from tile codes
    spawnEntitiesFromTileMap();

    m_pipeWarpCooldown = 0.0f;
    m_activeGenerators = CheepCheepConfig::generatorsFor(m_levelPath);
    m_generatorTimers.assign(m_activeGenerators.size(), 0.0f);
    
    // Initialize camera and entity sprite frames before first render
    update(0.f);
    
    return true;
}

void Level::spawnEntitiesFromTileMap() {
    const float levelHeight = static_cast<float>(m_tileMap.getHeight() * TILE_SIZE);

    // --- Spawn Mario from 'M' tile code (see level1.txt) ---
    auto marioSpawns = m_tileMap.findTiles('M');

    if (!marioSpawns.empty()) {
        sf::Vector2f spawnPos = TileMap::gridToWorldPosition(marioSpawns.front());
        m_mario = std::make_unique<Mario>(spawnPos, sf::Vector2f(32.f, 32.f));
        m_mario->setCharacterType(m_characterType);
        m_mario->setRespawnPosition(spawnPos);
        m_mario->setPitThreshold(levelHeight + 64.f);
    } else {
        std::cerr << "Level: No Mario spawn point ('M') found! " << "Defaulting to (100, 100)" << std::endl;
        m_mario = std::make_unique<Mario>();
        m_mario->setCharacterType(m_characterType);
    }

    // Wire TextureManager to Mario so setSprite() works
    m_mario->setTextureManager(m_textureManager);

    // Initialize Mario physics body
    m_mario->initPhysics(m_world.get(), b2_dynamicBody, sf::Vector2f(32.f, 32.f));

    // Wire underwater mode so Mario uses swim mechanics
    if (m_theme == LevelTheme::UNDERWATER) {
        m_mario->setUnderwater(true);
        if (m_mario->getBody()) {
            m_mario->getBody()->SetLinearDamping(1.5f);
        }
    }

    // --- Spawn enemies and items via Factory ---
    for (char code : SPAWN_CODES) {
        auto positions = m_tileMap.findTiles(code);
        for (const auto& gridPos : positions) {
            sf::Vector2f worldPos =
                TileMap::gridToWorldPosition(gridPos);
            auto entity =
                EntityFactory::createFromTileCode(code, worldPos, m_world.get(), m_theme);
            if (entity) {
                // Wire TextureManager so entity sprites can load
                entity->setTextureManager(m_textureManager);
                if (entity->isEnemy()) {
                    static_cast<Enemy*>(entity.get())->setTileMap(&m_tileMap);
                }
                m_entities.push_back(std::move(entity));
            }
        }
    }

    // --- Spawn elevators from '^' / '~' route markers ---
    spawnElevatorsFromTileMap();

    // --- Spawn Cheep Cheep routes from 'c' / 'x' route markers ---
    spawnCheepCheepRoutesFromTileMap();

    // --- Spawn Cheep Cheeps from external config (levels/cheep_cheep.txt) ---
    spawnCheepCheepsFromConfig();
}

void Level::spawnElevatorsFromTileMap() {
    const auto elevatorPositionFromMarker = [](const sf::Vector2i& marker) {
        const sf::Vector2f position = TileMap::gridToWorldPosition(marker);
        return sf::Vector2f(position.x + ELEVATOR_MARKER_X_OFFSET, position.y);
    };

    for (const auto& route : m_tileMap.getElevatorRoutes()) {
        const sf::Vector2f start = elevatorPositionFromMarker(route.start);
        const sf::Vector2f end = elevatorPositionFromMarker(route.end);

        auto elevator = std::make_unique<Elevator>(
            start,
            sf::Vector2f(end.x, end.y),
            Elevator::DEFAULT_SPEED,
            route.vertical ? Elevator::Axis::VERTICAL : Elevator::Axis::HORIZONTAL,
            Elevator::DEFAULT_PAUSE,
            m_theme);

        // Wire TextureManager so the elevator sprite can load
        elevator->setTextureManager(m_textureManager);
        elevator->initPhysics(m_world.get(), b2_kinematicBody, elevator->getSize());
        m_entities.push_back(std::move(elevator));
    }

    // External registry (levels/elevators.txt) — the primary way to add
    // elevators without modifying the level map file.
    for (const auto& route : ElevatorConfig::routesFor(m_levelPath)) {
        const auto inBounds = [this](const sf::Vector2i& point) {
            return point.x >= 0 && point.y >= 0 &&
                   point.x < static_cast<int>(m_tileMap.getWidth()) &&
                   point.y < static_cast<int>(m_tileMap.getHeight());
        };
        const bool validAxis = (route.start.x == route.end.x) !=
                               (route.start.y == route.end.y);
        const bool validNumbers = std::isfinite(route.speedPixelsPerSecond) &&
                                  std::isfinite(route.pauseSeconds) &&
                                  route.speedPixelsPerSecond > 0.0f &&
                                  route.pauseSeconds >= 0.0f;
        const bool validEndpoints = inBounds(route.start) && inBounds(route.end) &&
                                    !m_tileMap.isSolid(route.start.x, route.start.y) &&
                                    !m_tileMap.isSolid(route.end.x, route.end.y) &&
                                    !m_tileMap.isClimbable(route.start.x, route.start.y) &&
                                    !m_tileMap.isClimbable(route.end.x, route.end.y);
        if (!validAxis || !validNumbers || !validEndpoints) {
            std::cerr << "Level: skipping invalid elevator route in " << m_levelPath
                      << " from (" << route.start.x << ',' << route.start.y << ") to ("
                      << route.end.x << ',' << route.end.y << ")" << std::endl;
            continue;
        }

        const sf::Vector2f start = elevatorPositionFromMarker(route.start);
        const sf::Vector2f end = elevatorPositionFromMarker(route.end);
        const bool vertical = (route.start.x == route.end.x);

        auto elevator = std::make_unique<Elevator>(
            start,
            sf::Vector2f(end.x, end.y),
            route.speedPixelsPerSecond,
            vertical ? Elevator::Axis::VERTICAL : Elevator::Axis::HORIZONTAL,
            route.pauseSeconds,
            m_theme);

        elevator->setTextureManager(m_textureManager);
        elevator->initPhysics(m_world.get(), b2_kinematicBody, elevator->getSize());
        m_entities.push_back(std::move(elevator));
    }
}

void Level::spawnCheepCheepRoutesFromTileMap() {
    for (const auto& route : m_tileMap.getCheepCheepRoutes()) {
        const sf::Vector2f startPos = TileMap::gridToWorldPosition(route.start);
        const sf::Vector2f endPos = TileMap::gridToWorldPosition(route.end);
        auto cheep = std::make_unique<CheepCheep>(
            startPos,
            m_world.get(),
            m_theme,
            CheepCheepBehavior::SWIMMING,
            CheepCheepColor::GREEN
        );
        cheep->setRoute(startPos, endPos, 75.0f);
        cheep->setTextureManager(m_textureManager);
        cheep->setTileMap(&m_tileMap);
        m_entities.push_back(std::move(cheep));
    }
}

void Level::spawnCheepCheepsFromConfig() {
    for (const auto& spawn : CheepCheepConfig::spawnsFor(m_levelPath)) {
        const sf::Vector2f worldPos = TileMap::gridToWorldPosition(spawn.gridPosition);
        auto cheep = std::make_unique<CheepCheep>(
            worldPos,
            m_world.get(),
            m_theme,
            spawn.behavior,
            spawn.color
        );
        cheep->setFacingDirection(spawn.direction);
        cheep->setTextureManager(m_textureManager);
        cheep->setTileMap(&m_tileMap);
        m_entities.push_back(std::move(cheep));
    }

    for (const auto& route : CheepCheepConfig::routesFor(m_levelPath)) {
        const sf::Vector2f startPos = TileMap::gridToWorldPosition(route.start);
        const sf::Vector2f endPos = TileMap::gridToWorldPosition(route.end);
        auto cheep = std::make_unique<CheepCheep>(
            startPos,
            m_world.get(),
            m_theme,
            CheepCheepBehavior::SWIMMING,
            route.color
        );
        cheep->setRoute(startPos, endPos, route.speedPixelsPerSecond);
        cheep->setTextureManager(m_textureManager);
        cheep->setTileMap(&m_tileMap);
        m_entities.push_back(std::move(cheep));
    }
}

void Level::updateCheepCheepGenerators(float dt) {
    if (!m_mario || m_activeGenerators.empty() || m_flagSequenceActive || m_levelCompleted || !m_world) {
        return;
    }

    const float marioX = m_mario->getPosition().x;
    static std::mt19937 rng(std::random_device{}());

    for (std::size_t i = 0; i < m_activeGenerators.size(); ++i) {
        const auto& gen = m_activeGenerators[i];
        const float startX = static_cast<float>(gen.startColumn * TILE_SIZE);
        const float endX = static_cast<float>(gen.endColumn * TILE_SIZE);

        if (marioX < startX || marioX > endX) {
            continue;
        }

        m_generatorTimers[i] += dt;
        if (m_generatorTimers[i] >= gen.intervalSeconds) {
            m_generatorTimers[i] = 0.0f;

            const sf::View& view = m_camera.getView();
            const sf::Vector2f center = view.getCenter();
            const sf::Vector2f size = view.getSize();

            const float left = center.x - size.x / 2.0f;
            const float right = center.x + size.x / 2.0f;
            const float bottom = center.y + size.y / 2.0f;

            std::uniform_real_distribution<float> distX(left + 32.f, right - 32.f);
            std::uniform_real_distribution<float> distVx(gen.minVx, gen.maxVx);
            std::uniform_real_distribution<float> distVy(gen.minVy, gen.maxVy);

            const float spawnX = distX(rng);
            const float spawnY = bottom + 16.f;
            const float vx = distVx(rng);
            const float vy = distVy(rng);

            auto cheep = std::make_unique<CheepCheep>(
                sf::Vector2f(spawnX, spawnY),
                m_world.get(),
                m_theme,
                CheepCheepBehavior::JUMPING,
                gen.color,
                sf::Vector2f(vx, vy)
            );
            cheep->setTextureManager(m_textureManager);
            cheep->setTileMap(&m_tileMap);
            cheep->activate();
            m_entities.push_back(std::move(cheep));
        }
    }
}

void Level::update(float dt) {
    if (m_flagSequenceActive && m_mario) {
        switch (m_flagPhase) {
        case FlagPhase::SLIDING:
        case FlagPhase::WAITING_FLAG_DROP: {
            m_mario->setMoveIntent(0.0f);
            m_mario->setRunIntent(false);

            if (m_flagPhase == FlagPhase::SLIDING) {
                // Scripted descent: Mario slides down the pole while the flag
                // follows his displacement exactly.
                m_mario->updateFlagpoleSlide(dt);
                const float marioDisplacement =
                    m_mario->getPosition().y - m_flagSlideStartMarioY;
                m_tileMap.setFlagDropDistance(
                    std::max(0.0f,
                             m_flagSlideStartDropDistance + marioDisplacement));
            } else {
                // Mario already reached the pole base but the flag still needs
                // to descend to its validated maximum. Hold Mario still and
                // finish the flag drop independently (no rough time estimate).
                if (m_mario->getBody()) {
                    m_mario->getBody()->SetLinearVelocity(b2Vec2(0.f, 0.f));
                }
                m_tileMap.setFlagDropDistance(
                    m_tileMap.getFlagDropDistance() +
                    std::min(Mario::FLAGPOLE_SLIDE_SPEED * dt,
                             m_tileMap.getFlagMaxDropDistance() -
                                 m_tileMap.getFlagDropDistance()));
            }

            if (m_mario->isFlagpoleSlideComplete() &&
                m_tileMap.isFlagFullyDropped()) {
                // Once the flag reaches the bottom, Mario must leave the pole
                // toward the castle.  If he is still on the pole's left side,
                // first move him around to the right side of the pole.  This
                // is a real short transition, rather than only changing the
                // sprite direction, so the two starting sides behave visibly
                // differently.
                const float marioCenterX =
                    m_mario->getPosition().x + m_mario->getSize().x / 2.0f;
                if (marioCenterX < m_flagPoleCenterX) {
                    m_flagTurnTargetX =
                        m_flagPoleCenterX - m_mario->getSize().x / 2.0f + 14.0f;
                    m_mario->setFlagpoleSliding(false);
                    m_mario->setAutomaticWalkSpeed(FLAGPOLE_WALK_SPEED);
                    m_mario->setMoveIntent(1.0f);
                    m_flagPhase = FlagPhase::TURNING_RIGHT;
                    break;
                }

                // The original game gives Mario a short pause at the bottom,
                // then takes control and walks him right into the castle.
                m_mario->setFlagpoleSliding(false);
                m_mario->setAutomaticWalkSpeed(FLAGPOLE_WALK_SPEED);
                m_mario->setMoveIntent(1.0f);
                m_mario->setRunIntent(false);
                m_flagWalkActive = true;
                m_flagPhase = FlagPhase::WALKING;
            } else if (m_mario->isFlagpoleSlideComplete() &&
                       !m_tileMap.isFlagFullyDropped()) {
                m_flagPhase = FlagPhase::WAITING_FLAG_DROP;
            }
            break;
        }
        case FlagPhase::TURNING_RIGHT: {
            m_mario->setMoveIntent(1.0f);
            m_mario->setRunIntent(false);

            if (m_mario->getPosition().x >= m_flagTurnTargetX) {
                m_mario->setPosition({m_flagTurnTargetX,
                                      m_mario->getPosition().y});
                m_mario->stopMoving();
                m_mario->setVelocity({0.0f, 0.0f});
                m_mario->setMoveIntent(1.0f);
                m_flagWalkActive = true;
                m_flagPhase = FlagPhase::WALKING;
            }
            break;
        }
        case FlagPhase::WALKING: {
            m_mario->setMoveIntent(1.0f);
            m_mario->setRunIntent(false);

            // The walk is driven by Mario's actual physics position.  A timer
            // based on targetDistance / walkSpeed finishes too early because
            // Mario accelerates from rest; snapping here would skip the last
            // part of the walk into the castle.
            if (m_mario->getPosition().x >= m_flagWalkTargetX) {
                const sf::Vector2f stopPosition(m_flagWalkTargetX,
                                                m_mario->getPosition().y);
                m_mario->setPosition(stopPosition);
                m_mario->stopMoving();
                m_mario->setVelocity({0.0f, 0.0f});
                m_mario->setAutomaticWalkSpeed(0.0f);
                m_flagSequenceActive = false;
                m_flagWalkActive = false;
                m_flagPhase = FlagPhase::NONE;
                m_levelCompleted = true;
                EventBus::getInstance().notify(EventType::LEVEL_COMPLETED);
            }
            break;
        }
        case FlagPhase::NONE:
            break;
        }
    }

    if (m_pipeWarpPhase != PipeWarpPhase::NONE) {
        updatePipeWarp(dt);
        if (m_mario) {
            m_mario->updateVisuals(dt);
            const sf::Vector2f centerPos = m_mario->getPosition() + (m_mario->getSize() / 2.0f);
            m_camera.update(dt, centerPos);
        }
        return;
    }
    
    if (m_mario) {
        const sf::Vector2f center = m_mario->getPosition() + m_mario->getSize() / 2.0f;
        const int column = static_cast<int>(center.x / static_cast<float>(TILE_SIZE));
        const int row = static_cast<int>(center.y / static_cast<float>(TILE_SIZE));
        const bool onVine = m_tileMap.isClimbable(column, row);
        const float vineCenterX = static_cast<float>(column * TILE_SIZE) + TILE_SIZE / 2.0f;
        m_mario->setClimbContext(onVine, vineCenterX);
        m_mario->preparePhysics(dt);
    }

    if (m_world) {
        const bool physicsStepped = PhysicsEngine::update(*m_world, dt, m_physicsAccumulator);

        if (physicsStepped && m_mario) {
            m_mario->refreshGroundedState();
        }
    }

    if (m_pipeWarpCooldown > 0.0f) {
        m_pipeWarpCooldown = std::max(0.0f, m_pipeWarpCooldown - dt);
    }

    checkPipeWarps();

    // Flush any pending fireball creation requests queued while Box2D world was locked
    processPendingFireballs();

    // Update tilemap bump animations
    m_tileMap.update(dt);

    // Process queued tile hits (bumping Question blocks & shattering Brick blocks).
    // Mario owns the capability rule so Star power applies to every body tier.
    const bool canBreakBlocks = m_mario && m_mario->canBreakBricks();
    m_tileMap.processPendingHits(m_entities, m_textureManager,
                                 canBreakBlocks, m_mario.get());

    // Update Mario (the flag follows his displacement inside the flag sequence
    // handler above, driven by actual slide state rather than a timer).
    if (m_mario) {
        m_mario->update(dt);
    }

    // Update dynamic leaping Cheep Cheep bridge generators
    updateCheepCheepGenerators(dt);

    // Update all entities (enemies, items)
    for (auto& entity : m_entities) {
        if (!entity) {
            continue;
        }

        if (!entity->isEnemy()) {
            entity->update(dt);
            continue;
        }

        Enemy* enemy = static_cast<Enemy*>(entity.get());

        const sf::View& cameraView = m_camera.getView();

        if (!enemy->isActivated()) {
            if (!shouldActivateEnemy(*enemy, cameraView)) {
                continue;
            }

            enemy->activate();
        }

        if (enemy->isPiranhaPlant() && m_mario) {
            static_cast<PiranhaPlant*>(enemy)->updateMarioProximity(m_mario->getPosition());
        }

        enemy->update(dt);
    }

    const float levelWidth = static_cast<float>(m_tileMap.getWidth() * TILE_SIZE);

    const float levelHeight = static_cast<float>(m_tileMap.getHeight() * TILE_SIZE);

    for (auto& entity : m_entities) {
        if (!entity || entity->shouldRemove() || entity->isPendingDestroy()) {
            continue;
        }

        if (isEntityOutsideLevelBounds(*entity, levelWidth, levelHeight)) {
            entity->markForRemoval();
        }
    }

    // Check item-Mario collisions
    checkItemCollisions();
    checkFinishFlag();

    // Spawn explosion particle for any deactivated fireball requesting explosion
    std::vector<sf::Vector2f> explosionPositions;
    for (auto& entity : m_entities) {
        if (entity && entity->isFireBall()) {
            auto* fb = static_cast<FireBall*>(entity.get());
            if (fb->shouldSpawnExplosion() && (fb->shouldRemove() || fb->isPendingDestroy() || !fb->isActive())) {
                explosionPositions.push_back(fb->getPosition());
                fb->clearExplosionFlag();
            }
        }
    }
    for (const auto& pos : explosionPositions) {
        spawnFireballExplosion(pos);
    }

    // Remove dead entities
    removeDeadEntities();

    // Update camera to follow Mario's center
    if (m_mario) {
        sf::Vector2f centerPos = m_mario->getPosition() + (m_mario->getSize() / 2.0f);
        m_camera.update(dt, centerPos);
    }
}

void Level::spawnFireballExplosion(const sf::Vector2f& position) {
    auto explosion = std::make_unique<FireballExplosion>(position);
    explosion->setTextureManager(m_textureManager);
    // The effect is inserted after the entity-update loop, so initialize its
    // position/scale immediately for the frame in which it is spawned.
    explosion->update(0.f);
    m_entities.push_back(std::move(explosion));
}

void Level::render(sf::RenderTarget& target) {
    // Apply camera view
    target.setView(m_camera.getView());

    const float backgroundHeight = static_cast<float>(DisplayConfig::LOGICAL_HEIGHT);
    const float levelWidth = static_cast<float>(m_tileMap.getWidth() * TILE_SIZE);
    const float levelHeight = static_cast<float>(m_tileMap.getHeight() * TILE_SIZE);

    if (m_theme != LevelTheme::OVERWORLD) {
        const sf::Color themeColor =
            m_theme == LevelTheme::UNDERGROUND
                ? UNDERGROUND_BACKGROUND_COLOR
                : (m_theme == LevelTheme::UNDERWATER
                       ? UNDERWATER_BACKGROUND_COLOR
                       : CASTLE_BACKGROUND_COLOR);
        // Each non-overworld theme has a deliberate palette. Extend it beyond
        // the map so shake/dead-zone motion cannot expose the clear color.
        sf::RectangleShape themeBackground(
            sf::Vector2f(levelWidth, levelHeight + backgroundHeight * 2.f));
        themeBackground.setPosition({0.f, -backgroundHeight});
        themeBackground.setFillColor(themeColor);
        target.draw(themeBackground);
    } else {
        // Draw the cheerful pixel-art world background behind the tilemap.
        const sf::Texture& bgTex = m_textureManager.getTexture(
            std::string(SpriteFrames::ovw::Backgrounds::WORLD_PATH));
        sf::Sprite bgSprite(bgTex);
        bgSprite.setTextureRect(SpriteFrames::ovw::Backgrounds::WORLD);

        const float backgroundScale =
            backgroundHeight / static_cast<float>(SpriteFrames::ovw::Backgrounds::WORLD.size.y);
        const float stripWidth =
            static_cast<float>(SpriteFrames::ovw::Backgrounds::WORLD.size.x) * backgroundScale;
        const float backgroundTop = calculateBackgroundTop(m_tileMap);

        std::size_t stripIndex = 0;
        for (float x = 0; x < levelWidth + stripWidth; x += stripWidth, ++stripIndex) {
            const bool mirrored = (stripIndex % 2u) != 0u;
            bgSprite.setScale(mirrored ? sf::Vector2f(-backgroundScale, backgroundScale)
                                       : sf::Vector2f(backgroundScale, backgroundScale));
            bgSprite.setPosition(mirrored ? sf::Vector2f(x + stripWidth, backgroundTop)
                                         : sf::Vector2f(x, backgroundTop));
            target.draw(bgSprite);
        }
    }

    // Draw tilemap background
    m_tileMap.render(target);

    // The cloth belongs behind the actors, while the pole shaft/cap remains
    // in the foreground layer below. This keeps the pole connector visible
    // and prevents the flag quad from hiding Mario during the slide.
    m_tileMap.renderFlags(target);

    // Draw all entities (enemies, items)
    for (const auto& entity : m_entities) {
        target.draw(*entity);
    }

    // Draw Mario on top
    if (m_mario) {
        target.draw(*m_mario);
    }
    
    // Draw foreground tiles (blocks, flagpoles, pipes) on top of Mario and entities
    m_tileMap.renderForeground(target);
}

void Level::checkItemCollisions() {
    if (!m_mario || m_mario->isCollisionLocked()) return;

    for (auto& entity : m_entities) {
        // Use virtual isItem() instead of dynamic_cast to avoid RTTI overhead
        if (!entity->isItem()) continue;

        Item* item = static_cast<Item*>(entity.get());
        if (item->isCollected()) {
            if (const auto* coin = dynamic_cast<const Coin*>(item);
                coin && coin->getCoinType() == CoinType::QUESTION_POPUP &&
                !item->shouldRemove()) {
                continue;
            }
            if (!item->shouldRemove()) {
                item->markForRemoval();
            }
            continue;
        }

        if (item->checkOverlap(*m_mario)) {
            item->onCollect(*m_mario);
            item->markForRemoval();
        }
    }
}

void Level::checkFinishFlag() {
    if (!m_mario || m_mario->isCollisionLocked() || m_levelCompleted ||
        m_flagSequenceActive) {
        return;
    }

    const auto finishTiles = m_tileMap.findTiles('F');

    if (finishTiles.empty()) {
        return;
    }

    const auto& finishPosition = finishTiles.front();

    int bottomRow = finishPosition.y;

    const auto poleTiles = m_tileMap.findTiles('|');

    for (const auto& polePosition : poleTiles) {
        if (polePosition.x == finishPosition.x && polePosition.y > bottomRow) {
            bottomRow = polePosition.y;
        }
    }

    const sf::Vector2f triggerPosition = TileMap::gridToWorldPosition(finishPosition);

    const float triggerHeight = static_cast<float>(bottomRow - finishPosition.y + 1) * static_cast<float>(TILE_SIZE);

    const sf::FloatRect finishTrigger(triggerPosition, sf::Vector2f(static_cast<float>(TILE_SIZE),
                                                                    triggerHeight
                                                                    )
                                      );

    const sf::FloatRect marioBounds = m_mario->getBoundingBox();

    if (marioBounds.findIntersection(finishTrigger)) {
        // The flag is a one-shot gameplay sequence.  Keep the level active
        // until Mario has finished the climb and walked into the exit.
        m_flagSequenceActive = true;
        m_flagWalkActive = false;
        m_flagPhase = FlagPhase::SLIDING;
        m_flagSlideStartMarioY = m_mario->getPosition().y;
        m_flagSlideStartDropDistance = 0.0f;
        const float poleCenterX = triggerPosition.x + TILE_SIZE / 2.0f;
        m_flagPoleCenterX = poleCenterX;
        const float targetTopY = static_cast<float>(bottomRow + 1) * TILE_SIZE -
                                 m_mario->getSize().y;
        m_mario->beginFlagpoleSlide(poleCenterX, targetTopY);
        m_tileMap.setFlagDropDistance(m_flagSlideStartDropDistance);

        // `L` is the bottom-left anchor of the five-tile-wide castle. Aim for
        // its center door. Levels without a castle still get a short,
        // authentic-looking walk toward the right edge before transitioning.
        const auto castleAnchors = m_tileMap.findTiles('L');
        const auto destinationCastle = std::find_if(
            castleAnchors.begin(), castleAnchors.end(),
            [&finishPosition](const sf::Vector2i& anchor) {
                return anchor.x > finishPosition.x;
            });
        if (destinationCastle != castleAnchors.end()) {
            // Some levels, notably Level 2, contain a decorative castle at
            // the spawn and the real exit castle after the flag.  The first
            // anchor is not necessarily the destination.
            const float castleLeft = static_cast<float>(destinationCastle->x) * TILE_SIZE;
            const float castleCenter = castleLeft + 2.5f * TILE_SIZE;
            m_flagWalkTargetX = castleCenter - m_mario->getSize().x / 2.0f;
        } else {
            const float levelRight = static_cast<float>(m_tileMap.getWidth()) * TILE_SIZE;
            const float rightEdgeTarget = levelRight - m_mario->getSize().x - 8.0f;
            const float shortWalkTarget = m_mario->getPosition().x + 4.0f * TILE_SIZE;
            m_flagWalkTargetX = std::min(rightEdgeTarget, shortWalkTarget);
        }
        m_flagWalkTargetX = std::max(m_flagWalkTargetX,
                                     m_mario->getPosition().x + TILE_SIZE);
    }
}

void Level::removeDeadEntities() {
    m_entities.erase(
        std::remove_if(m_entities.begin(), m_entities.end(),
            [](const std::unique_ptr<Entity>& e) {
                return !e || e->shouldRemove() || !e->isActive() || e->isPendingDestroy();
            }),
        m_entities.end()
    );
}

// --- Getters ---
Mario* Level::getMario() {
    return m_mario.get();
}
const Mario* Level::getMario() const {
    return m_mario.get();
}
bool Level::isLevelCompleted() const {
    return m_levelCompleted;
}


TileMap& Level::getTileMap() { return m_tileMap; }
Camera& Level::getCamera() { return m_camera; }
TextureManager& Level::getTextureManager() { return m_textureManager; }

bool Level::requestFireBallShot(Mario& mario) {
    if (!m_world) {
        return false;
    }

    constexpr std::size_t MAX_ACTIVE_FIREBALLS = 2;
    if (getActiveFireBallCount() + m_pendingFireBallRequests.size() >=
        MAX_ACTIVE_FIREBALLS) {
        return false;
    }

    // Reserve the cooldown before checking IsLocked so accepted deferred shots
    // cannot bypass cadence or be accepted twice during one physics step.
    if (!mario.tryStartFireBallShot()) {
        return false;
    }

    const float spawnX = mario.getPosition().x +
        (mario.getFacingDirection() == Direction::RIGHT
             ? mario.getSize().x + 4.f
             : -16.f);
    const sf::Vector2f spawnPosition(spawnX, mario.getPosition().y + 4.f);
    const FireBallSpawnRequest request{
        spawnPosition, mario.getFacingDirection(), &mario};

    if (m_world->IsLocked()) {
        m_pendingFireBallRequests.push_back(request);
        return true;
    }

    auto fireball = std::make_unique<FireBall>(
        request.position, request.direction, m_world.get());
    fireball->setOwner(request.owner);
    fireball->setTextureManager(m_textureManager);
    m_entities.push_back(std::move(fireball));
    EventBus::getInstance().notify(EventType::FIREBALL_SHOT);
    return true;
}

bool Level::requestFireBallShot() {
    if (!m_mario) return false;
    return requestFireBallShot(*m_mario);
}

void Level::processPendingFireballs() {
    if (!m_world || m_world->IsLocked() || m_pendingFireBallRequests.empty()) return;

    for (const auto& req : m_pendingFireBallRequests) {
        auto fireball = std::make_unique<FireBall>(req.position, req.direction, m_world.get());
        fireball->setOwner(req.owner);
        fireball->setTextureManager(m_textureManager);
        m_entities.push_back(std::move(fireball));
        EventBus::getInstance().notify(EventType::FIREBALL_SHOT);
    }
    m_pendingFireBallRequests.clear();
}

std::size_t Level::getActiveFireBallCount() const {
    return static_cast<std::size_t>(std::count_if(
        m_entities.begin(), m_entities.end(),
        [](const std::unique_ptr<Entity>& entity) {
            return entity && entity->isFireBall() && entity->isActive();
        }));
}

bool Level::isPiranhaAliveAt(const sf::Vector2i& pipePosition) const {
    const sf::Vector2f pipeWorld = TileMap::gridToWorldPosition(pipePosition);

    const float expectedX = pipeWorld.x + TILE_SIZE / 2.0f;

    for (const auto& entity : m_entities) {
        if (!entity || !entity->isPiranhaPlant() || entity->shouldRemove() || entity->isPendingDestroy()) {
            continue;
        }

        const auto* plant = static_cast<const PiranhaPlant*>(entity.get());

        // Dying Piranha already counts as defeated,
        // no need to wait for death animation to disappear.
        if (plant->isDying()) {
            continue;
        }

        const sf::Vector2f position = plant->getPosition();

        const bool samePipeX = std::abs(position.x - expectedX) <= static_cast<float>(TILE_SIZE);
        const bool nearPipeY = std::abs(position.y - pipeWorld.y) <= 3.0f * TILE_SIZE;

        if (samePipeX && nearPipeY) {
            return true;
        }
    }

    return false;
}

void Level::suppressPiranhaAt(const sf::Vector2i& pipePosition) {
    const sf::Vector2f pipeWorld = TileMap::gridToWorldPosition(pipePosition);
    const float expectedX = pipeWorld.x + TILE_SIZE / 2.0f;

    for (auto& entity : m_entities) {
        if (!entity || !entity->isPiranhaPlant() || entity->shouldRemove() || entity->isPendingDestroy()) {
            continue;
        }

        const sf::Vector2f position = entity->getPosition();

        const bool samePipeX = std::abs(position.x - expectedX) <= static_cast<float>(TILE_SIZE);
        const bool nearPipeY = std::abs(position.y - pipeWorld.y) <= 3.0f * TILE_SIZE;

        if (samePipeX && nearPipeY) {
            entity->setActive(false);
            entity->markForRemoval();
            return;
        }
    }
}

void Level::startPipeWarp(char warpId, PipeWarpPhase phase, const sf::Vector2i& pipeTile) {
    if (!m_mario) {
        return;
    }

    m_pendingWarpId = warpId;
    m_pendingPipeTile = pipeTile;
    m_pipeWarpPhase = phase;
    m_pipeWarpTimer = 0.5f; // 0.5s smooth slide duration

    SoundManager::getInstance().playSound("powerdown");

    if (m_mario->getBody()) {
        m_mario->getBody()->SetGravityScale(0.0f);
        m_mario->getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
    }
    m_mario->setVelocity({0.0f, 0.0f});

    if (phase == PipeWarpPhase::ENTERING_VERTICAL) {
        if (m_mario->getMarioState() == MarioState::SUPER ||
            m_mario->getMarioState() == MarioState::FIRE_SUPER) {
            m_mario->playAnimation("crouch");
        } else {
            m_mario->playAnimation("idle");
        }
    } else if (phase == PipeWarpPhase::ENTERING_HORIZONTAL) {
        m_mario->playAnimation("walk");
    }
}

void Level::updatePipeWarp(float dt) {
    if (!m_mario || m_pipeWarpPhase == PipeWarpPhase::NONE) {
        return;
    }

    if (m_mario->getBody()) {
        m_mario->getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
        m_mario->getBody()->SetGravityScale(0.0f);
    }
    m_mario->setVelocity({0.0f, 0.0f});

    constexpr float PIPE_SLIDE_SPEED = 48.0f; // px/sec

    if (m_pipeWarpPhase == PipeWarpPhase::ENTERING_VERTICAL) {
        m_pipeWarpTimer -= dt;
        const sf::Vector2f pos = m_mario->getPosition();
        const float pipeCenterX = static_cast<float>(m_pendingPipeTile.x * TILE_SIZE) + static_cast<float>(TILE_SIZE);
        const float targetX = pipeCenterX - m_mario->getSize().x / 2.0f;
        const float newX = pos.x + (targetX - pos.x) * std::min(1.0f, dt * 10.0f);
        m_mario->setPosition({newX, pos.y + PIPE_SLIDE_SPEED * dt});

        if (m_mario->getMarioState() == MarioState::SUPER ||
            m_mario->getMarioState() == MarioState::FIRE_SUPER) {
            m_mario->playAnimation("crouch");
        } else {
            m_mario->playAnimation("idle");
        }

        if (m_pipeWarpTimer <= 0.0f) {
            m_pipeWarpPhase = PipeWarpPhase::WARPING_DELAY;
            m_pipeWarpTimer = 0.35f; // 350ms transition delay
            warpMarioToReturn(m_pendingWarpId);
        }
    } else if (m_pipeWarpPhase == PipeWarpPhase::ENTERING_HORIZONTAL) {
        m_pipeWarpTimer -= dt;
        const sf::Vector2f pos = m_mario->getPosition();
        m_mario->setPosition({pos.x + 60.0f * dt, pos.y});
        m_mario->playAnimation("walk");

        if (m_pipeWarpTimer <= 0.0f) {
            m_pipeWarpPhase = PipeWarpPhase::WARPING_DELAY;
            m_pipeWarpTimer = 0.35f; // 350ms transition delay
            warpMarioToReturn(m_pendingWarpId);
        }
    } else if (m_pipeWarpPhase == PipeWarpPhase::WARPING_DELAY) {
        m_pipeWarpTimer -= dt;
        if (m_pipeWarpTimer <= 0.0f) {
            warpMarioToReturn(m_pendingWarpId);
        }
    } else if (m_pipeWarpPhase == PipeWarpPhase::EXITING_VERTICAL) {
        m_pipeWarpTimer -= dt;
        const sf::Vector2f pos = m_mario->getPosition();
        const float newY = std::max(m_pipeWarpExitTargetY, pos.y - PIPE_SLIDE_SPEED * dt);
        m_mario->setPosition({pos.x, newY});
        m_mario->playAnimation("idle");

        if (m_pipeWarpTimer <= 0.0f || pos.y <= m_pipeWarpExitTargetY) {
            m_mario->setPosition({pos.x, m_pipeWarpExitTargetY});
            m_pipeWarpPhase = PipeWarpPhase::NONE;
            m_pipeWarpCooldown = 0.5f;
            if (m_mario->getBody()) {
                m_mario->getBody()->SetGravityScale(1.0f);
            }
            m_mario->playAnimation("idle");
        }
    }
}

void Level::warpMarioToReturn(char warpId) {
    if (!m_mario) {
        return;
    }

    const auto destination = m_tileMap.findWarpReturn(warpId);

    if (!destination) {
        return;
    }

    const sf::Vector2i returnPosition = *destination;

    // Special rule:
    //
    // R3
    // pr
    //
    // Mario return and Piranha cannot use
    // the same pipe simultaneously.
    const sf::Vector2i tileBelow{returnPosition.x, returnPosition.y + 1};

    if (m_tileMap.getTileAt(tileBelow.x, tileBelow.y) == 'p') {
        suppressPiranhaAt(tileBelow);
    }

    const sf::Vector2f marioSize = m_mario->getSize();

    // RN marks the tile where Mario's feet should end.
    // Works for both Small and Big Mario.
    const sf::Vector2f target{static_cast<float>(returnPosition.x * TILE_SIZE),
                              static_cast<float>((returnPosition.y + 1) * TILE_SIZE) - marioSize.y
                              };

    m_mario->setVelocity({0.0f, 0.0f});
    if (m_mario->getBody()) {
        m_mario->getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
    }

    // Check if the tile below is a pipe (so Mario should emerge upwards out of the pipe)
    const char symBelow = m_tileMap.getTileAt(tileBelow.x, tileBelow.y);
    const bool isPipeReturn = (symBelow == '[' || symBelow == ']' || symBelow == 'p' ||
                               symBelow == 'r' || symBelow == '{' || symBelow == '}');

    if (isPipeReturn) {
        // Start Mario positioned down inside the pipe, and slide upwards
        const sf::Vector2f startPos{target.x, target.y + marioSize.y};
        m_mario->setPosition(startPos);
        m_pipeWarpExitTargetY = target.y;
        m_pipeWarpPhase = PipeWarpPhase::EXITING_VERTICAL;
        m_pipeWarpTimer = 0.45f;
        SoundManager::getInstance().playSound("powerdown");
    } else {
        m_mario->setPosition(target);
        m_pipeWarpPhase = PipeWarpPhase::NONE;
        m_pipeWarpCooldown = 0.5f;
        if (m_mario->getBody()) {
            m_mario->getBody()->SetGravityScale(1.0f);
        }
        m_mario->playAnimation("idle");
    }

    // Immediately snap camera to destination
    const sf::Vector2f centerPos = target + (marioSize / 2.0f);
    m_camera.update(0.0f, centerPos);
}

void Level::checkPipeWarps() {
    if (!m_mario || m_pipeWarpCooldown > 0.0f ||
        m_pipeWarpPhase != PipeWarpPhase::NONE ||
        m_mario->isCollisionLocked() || m_flagSequenceActive ||
        m_levelCompleted) {
        return;
    }

    const sf::Vector2f marioPos = m_mario->getPosition();

    const sf::Vector2f marioSize = m_mario->getSize();

    for (const auto& entry : m_tileMap.getWarpEntries()) {
        const sf::Vector2i pipe = entry.position;

        // ==============================================
        // Hn — horizontal pipe
        // Walk RIGHT into it.
        // ==============================================
        if (entry.type == TileMap::WarpEntryType::HORIZONTAL) {
            if (m_mario->getHorizontalIntent() <= 0.5f) {
                continue;
            }

            const float pipeLeft = static_cast<float>(pipe.x * TILE_SIZE);
            const float pipeTop = static_cast<float>((pipe.y - 1) * TILE_SIZE);
            const float pipeBottom = static_cast<float>((pipe.y + 1) * TILE_SIZE);
            const float marioRight = marioPos.x + marioSize.x;

            const bool touchingPipe = std::abs(marioRight - pipeLeft) <= 12.0f;

            const bool verticallyAligned = marioPos.y < pipeBottom && marioPos.y + marioSize.y > pipeTop;

            if (touchingPipe && verticallyAligned) {
                startPipeWarp(entry.id, PipeWarpPhase::ENTERING_HORIZONTAL, entry.position);
                return;
            }

            continue;
        }

        // ==============================================
        // [n / pn — vertical pipe
        // Stand on it + press Down.
        // ==============================================
        if (m_mario->getVerticalIntent() <= 0.5f || !m_mario->isGrounded()) {
            continue;
        }

        // pn is locked until its Piranha dies.
        if (entry.type == TileMap::WarpEntryType::PIRANHA && isPiranhaAliveAt(pipe)) {
            continue;
        }

        const float pipeLeft = static_cast<float>(pipe.x * TILE_SIZE);
        const float pipeRight = pipeLeft + 2.0f * TILE_SIZE;
        const float pipeTop = static_cast<float>(pipe.y * TILE_SIZE);

        const float marioCenterX = marioPos.x + marioSize.x / 2.0f;
        const float marioFeetY = marioPos.y + marioSize.y;

        const bool centeredOnPipe = marioCenterX >= pipeLeft + 8.0f && marioCenterX <= pipeRight - 8.0f;
        const bool standingOnPipe = std::abs(marioFeetY - pipeTop) <= 8.0f;

        if (centeredOnPipe && standingOnPipe) {
            startPipeWarp(entry.id, PipeWarpPhase::ENTERING_VERTICAL, entry.position);
            return;
        }
    }
}

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
#include "items/FireFlower.h"
#include "patterns/EntityFactory.h"
#include "patterns/EventBus.h"
#include "physics/PhysicsEngine.h"
#include "physics/ContactListener.h"
#include "entities/Enemy.h"
#include "entities/Elevator.h"
#include "level/ElevatorConfig.h"
#include "level/CheepCheepConfig.h"
#include "entities/PiranhaPlant.h"
#include "entities/Blooper.h"
#include "entities/Lakitu.h"
#include "entities/HammerBro.h"
#include "entities/Bowser.h"
#include "entities/BowserAxe.h"
#include "entities/Firebar.h"
#include "entities/BulletBillLauncher.h"
#include "entities/CheepCheep.h"
#include "entities/FireBall.h"
#include "entities/FireballExplosion.h"
#include "entities/ScorePopup.h"
#include "core/SpriteFrames_ovw.h"
#include "core/LevelCatalog.h"
#include "core/ScoreRules.h"
#include "core/SoundManager.h"

#include "core/DisplayConfig.h"

namespace {

constexpr unsigned int TILE_SIZE = 32;

/// Box2D collision group for the second PvP fighter. Player one keeps the
/// campaign projectile group (-1) while player two gets his own negative
/// group: each fighter's fireballs share his group and therefore pass through
/// their shooter while still reaching the opponent.
constexpr int16_t PVP_PLAYER_TWO_COLLISION_GROUP = -2;

/// Upward bounce given to the winner of a PvP head-stomp, mirroring the
/// enemy-stomp bounce of the campaign flow.
constexpr float PVP_STOMP_BOUNCE_SPEED = 300.f;
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

// Tile codes that represent spawnable standalone entities (Goomba, Koopa, PiranhaPlant, CheepCheep, Coin, QuestionBlock, Springboard,
// BuzzyBeetle 'b', RedKoopa 'k', Paratroopa 'y'/'d', Red PiranhaPlant 'q', Blooper 'l', Podoboo 'P', Lakitu 't', Spiny 's',
// Bullet Bill launcher 'D')
constexpr char SPAWN_CODES[] = {'G', 'K', 'p', 'c', 'C', '?', 'f', 'h', 'U', 'u', 'O', 'o', 'J', 'e',
                                'b', 'k', 'y', 'd', 'q', 'l', 'P', 't', 's', 'D', 'n', 'X', 'A'};

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
    m_mario2.reset();
    m_tileMap.destroyPhysicsBodies();
    m_contactListener.reset();
    m_world.reset();
}
void Level::setTheme(LevelTheme theme) {
    m_theme = theme;
    m_tileMap.setTheme(theme);
}

void Level::applyAreaTheme(LevelTheme theme) {
    if (m_theme == theme) {
        return;
    }

    m_theme = theme;
    m_tileMap.setTheme(theme);

    const bool underwater = theme == LevelTheme::UNDERWATER;

    // Global Box2D gravity.
    if (m_world) {
        m_world->SetGravity(b2Vec2(0.f, underwater ? 8.0f : 25.0f));
    }

    const auto configureMario = [underwater](Mario* mario) {
        if (!mario) {
            return;
        }

        mario->setUnderwater(underwater);

        if (b2Body* body = mario->getBody()) {
            body->SetLinearDamping(underwater ? 1.5f : 0.0f);
        }
    };

    configureMario(m_mario.get());
    configureMario(m_mario2.get());

    auto& sound = SoundManager::getInstance();

    const MusicId music = underwater ? MusicId::UNDERWATER : MusicId::OVERWORLD;

    sound.setLevelMusic(music);
    sound.playMusic(music);
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
    m_mario2.reset();
    m_coopMode = false;
    m_flagPlayer = nullptr;
    m_warpPlayer = nullptr;
    m_pipeWarpPhase = PipeWarpPhase::NONE;
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
    m_flagPlayer = m_mario.get();

    m_pipeWarpCooldown = 0.0f;
    m_activeGenerators = CheepCheepConfig::generatorsFor(m_levelPath);
    m_generatorTimers.assign(m_activeGenerators.size(), 0.0f);

    // Initialize camera and entity sprite frames before first render
    update(0.f);

    return true;
}

void Level::beginBridgeCollapse(Mario* scorer) {
    if (m_bridgeCollapseActive || m_levelCompleted) {
        return;
    }

    m_bridgeTilesRemaining = m_tileMap.findTiles('=');
    // The collapse ripples outward from the axe, so fall axe-side first:
    // sort descending by column (the axe sits at the bridge's right end).
    std::sort(m_bridgeTilesRemaining.begin(), m_bridgeTilesRemaining.end(),
              [](const sf::Vector2i& a, const sf::Vector2i& b) {
                  return a.x > b.x;
              });

    m_bridgeCollapseActive = true;
    m_bridgeCollapseTimer = 0.f;
    m_bridgeCompletionDelay = 0.f;
    m_bridgeCollapseScorer = scorer;
}

void Level::updateBridgeCollapse(float dt) {
    if (!m_bridgeCollapseActive) {
        return;
    }

    if (m_bridgeCompletionDelay > 0.f) {
        m_bridgeCompletionDelay -= dt;
        if (m_bridgeCompletionDelay <= 0.f) {
            m_bridgeCollapseActive = false;
            m_levelCompleted = true;
            EventBus::getInstance().notify(EventType::LEVEL_COMPLETED);
        }
        return;
    }

    m_bridgeCollapseTimer += dt;
    if (m_bridgeCollapseTimer < 0.08f) {
        return;
    }
    m_bridgeCollapseTimer = 0.f;

    if (!m_bridgeTilesRemaining.empty()) {
        const sf::Vector2i tile = m_bridgeTilesRemaining.back();
        m_bridgeTilesRemaining.pop_back();
        m_tileMap.removeTile(tile.x, tile.y);
        return;
    }

    // Bridge gone: every Bowser sinks into the lava and the siege pays out.
    for (auto& entity : m_entities) {
        if (!entity || !entity->isBowser()) {
            continue;
        }
        Bowser* bowser = static_cast<Bowser*>(entity.get());
        bowser->collapseIntoLava();
        if (m_bridgeCollapseScorer) {
            m_bridgeCollapseScorer->queueScoreAward(
                bowser->getPosition() + sf::Vector2f(bowser->getSize().x / 2.f, 0.f),
                ScoreRules::BOWSER_DEFEATED, false);
        }
    }

    m_bridgeCompletionDelay = 1.2f;
}

bool Level::loadFromFile(const std::string& path,
                         CharacterType playerOne,
                         CharacterType playerTwo) {
    if (!loadFromFile(path, playerOne)) {
        return false;
    }

    m_coopMode = true;

    // Player two spawns beside the campaign 'M' tile so both players start
    // the level together. The adjacent free tile is preferred; the exact spot
    // is acceptable because the solver separates overlapping bodies.
    const float levelHeight = static_cast<float>(m_tileMap.getHeight() * TILE_SIZE);
    const auto marioSpawns = m_tileMap.findTiles('M');
    const sf::Vector2f basePos = marioSpawns.empty()
        ? m_mario->getPosition()
        : TileMap::gridToWorldPosition(marioSpawns.front());

    const auto findFreeAdjacentTile = [this](const sf::Vector2f& base,
                                             sf::Vector2f& out) -> bool {
        const int column = static_cast<int>(base.x / static_cast<float>(TILE_SIZE));
        const int row = static_cast<int>(base.y / static_cast<float>(TILE_SIZE));
        for (int dc : {1, -1}) {
            const int c = column + dc;
            if (c >= 0 && c < static_cast<int>(m_tileMap.getWidth()) &&
                !m_tileMap.isSolid(c, row)) {
                out = sf::Vector2f(base.x + static_cast<float>(dc) * TILE_SIZE, base.y);
                return true;
            }
        }
        return false;
    };

    sf::Vector2f partnerPos = basePos;
    findFreeAdjacentTile(basePos, partnerPos);

    m_mario2 = std::make_unique<Mario>(partnerPos, sf::Vector2f(32.f, 32.f));
    m_mario2->setCharacterType(playerTwo);
    m_mario2->setRespawnPosition(partnerPos);
    m_mario2->setPitThreshold(levelHeight + 64.f);
    m_mario2->setTextureManager(m_textureManager);
    m_mario2->initPhysics(m_world.get(), b2_dynamicBody, sf::Vector2f(32.f, 32.f));

    // Wire underwater swim mechanics for both players, matching player one.
    if (m_theme == LevelTheme::UNDERWATER) {
        m_mario2->setUnderwater(true);
        if (m_mario2->getBody()) {
            m_mario2->getBody()->SetLinearDamping(1.5f);
        }
    }

    // Re-run the zero-dt update so player two's sprite transforms and the
    // midpoint camera are initialized before the first render.
    updateCoop(0.f);

    return true;
}

bool Level::loadPvpArena(const std::string& path,
                          CharacterType playerOne,
                          CharacterType playerTwo) {
    m_pvpMode = true;
    m_characterType = playerOne;
    m_entities.clear();
    m_pendingFireBallRequests.clear();
    m_mario.reset();
    m_mario2.reset();
    m_pvpFireFlower = nullptr;
    m_pvpFireballHits.clear();
    CollisionManager::clearPendingPvpHits();
    m_tileMap.destroyPhysicsBodies();
    m_contactListener.reset();
    m_world.reset();
    m_levelCompleted = false;
    m_flagSequenceActive = false;
    m_flagWalkActive = false;
    m_flagPhase = FlagPhase::NONE;
    m_physicsAccumulator = 0.0f;
    m_levelPath = path;
    m_activeGenerators.clear();
    m_generatorTimers.clear();
    m_pipeWarpPhase = PipeWarpPhase::NONE;

    if (!m_tileMap.loadFromFile(path, TileMap::LayoutMode::PVP_ARENA)) {
        std::cerr << "Level: Failed to load PvP arena TileMap from " << path << std::endl;
        return false;
    }

    const float levelWidth = static_cast<float>(m_tileMap.getWidth() * TILE_SIZE);
    const float levelHeight = static_cast<float>(m_tileMap.getHeight() * TILE_SIZE);

    m_camera.init(
        sf::Vector2f(static_cast<float>(DisplayConfig::LOGICAL_WIDTH),
                     static_cast<float>(DisplayConfig::LOGICAL_HEIGHT)),
        sf::FloatRect(sf::Vector2f(0.f, 0.f),
                      sf::Vector2f(levelWidth, levelHeight))
    );
    m_camera.setVerticalMode(m_cameraVerticalMode);
    const float gravity = (m_theme == LevelTheme::UNDERWATER) ? 8.0f : 25.0f;
    m_world = std::make_unique<b2World>(b2Vec2(0.f, gravity));
    m_contactListener = std::make_unique<ContactListener>(m_tileMap);
    m_world->SetContactListener(m_contactListener.get());
    m_tileMap.createPhysicsBodies(m_world.get());

    // Player one spawns from 'M' (same rules as the campaign flow).
    const auto playerOneSpawns = m_tileMap.findTiles('M');
    const auto playerTwoSpawns = m_tileMap.findTiles('m');
    if (playerOneSpawns.empty() || playerTwoSpawns.empty()) {
        std::cerr << "Level: PvP arena is missing a player spawn point in " << path << std::endl;
        return false;
    }

    const sf::Vector2f playerOnePos = TileMap::gridToWorldPosition(playerOneSpawns.front());
    m_mario = std::make_unique<Mario>(playerOnePos, sf::Vector2f(32.f, 32.f));
    m_mario->setCharacterType(playerOne);
    m_mario->setRespawnPosition(playerOnePos);
    m_mario->setPitThreshold(levelHeight + 64.f);
    m_mario->setTextureManager(m_textureManager);
    m_mario->initPhysics(m_world.get(), b2_dynamicBody, sf::Vector2f(32.f, 32.f));

    // Player two spawns from 'm'. A distinct negative collision group makes
    // his own fireballs pass through him while reaching the opponent.
    const sf::Vector2f playerTwoPos = TileMap::gridToWorldPosition(playerTwoSpawns.front());
    m_mario2 = std::make_unique<Mario>(playerTwoPos, sf::Vector2f(32.f, 32.f));
    m_mario2->setCharacterType(playerTwo);
    m_mario2->setRespawnPosition(playerTwoPos);
    m_mario2->setPitThreshold(levelHeight + 64.f);
    m_mario2->setTextureManager(m_textureManager);
    m_mario2->initPhysics(m_world.get(), b2_dynamicBody, sf::Vector2f(32.f, 32.f));
    m_mario2->setFixtureCollisionGroup(PVP_PLAYER_TWO_COLLISION_GROUP);

    // A duel arena spawns no enemies, items, elevators, or generators.

    // Initialize fighter sprite frames and the pinned camera before render.
    updatePvp(0.f);

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

    const auto playerInRange = [](const Mario* player, float startX, float endX) {
        if (!player) {
            return false;
        }
        const float x = player->getPosition().x;
        return x >= startX && x <= endX;
    };
    static std::mt19937 rng(std::random_device{}());

    for (std::size_t i = 0; i < m_activeGenerators.size(); ++i) {
        const auto& gen = m_activeGenerators[i];
        const float startX = static_cast<float>(gen.startColumn * TILE_SIZE);
        const float endX = static_cast<float>(gen.endColumn * TILE_SIZE);

        // In co-op the generator reacts while either player is inside the gate.
        const bool anyPlayerInRange =
            playerInRange(m_mario.get(), startX, endX) ||
            (m_coopMode && playerInRange(m_mario2.get(), startX, endX));
        if (!anyPlayerInRange) {
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

void Level::updateFlagSequence(float dt) {
    // The sequence is driven by the player who touched the pole
    // (m_flagPlayer == m_mario in single player).
    if (!m_flagSequenceActive || !m_flagPlayer) {
        return;
    }
    Mario* const mario = m_flagPlayer;

    switch (m_flagPhase) {
    case FlagPhase::SLIDING:
    case FlagPhase::WAITING_FLAG_DROP: {
        mario->setMoveIntent(0.0f);
        mario->setRunIntent(false);

        if (m_flagPhase == FlagPhase::SLIDING) {
            // Scripted descent: Mario slides down the pole while the flag
            // follows his displacement exactly.
            mario->updateFlagpoleSlide(dt);
            const float marioDisplacement =
                mario->getPosition().y - m_flagSlideStartMarioY;
            m_tileMap.setFlagDropDistance(
                std::max(0.0f,
                         m_flagSlideStartDropDistance + marioDisplacement));
        } else {
            // Mario already reached the pole base but the flag still needs
            // to descend to its validated maximum. Hold Mario still and
            // finish the flag drop independently (no rough time estimate).
            if (mario->getBody()) {
                mario->getBody()->SetLinearVelocity(b2Vec2(0.f, 0.f));
            }
            m_tileMap.setFlagDropDistance(
                m_tileMap.getFlagDropDistance() +
                std::min(Mario::FLAGPOLE_SLIDE_SPEED * dt,
                         m_tileMap.getFlagMaxDropDistance() -
                             m_tileMap.getFlagDropDistance()));
        }

        if (mario->isFlagpoleSlideComplete() &&
            m_tileMap.isFlagFullyDropped()) {
            // Once the flag reaches the bottom, Mario must leave the pole
            // toward the castle. If he is still on the pole's left side,
            // first move him around to the right side of the pole.
            const float marioCenterX =
                mario->getPosition().x + mario->getSize().x / 2.0f;
            if (marioCenterX < m_flagPoleCenterX) {
                m_flagTurnTargetX =
                    m_flagPoleCenterX - mario->getSize().x / 2.0f + 14.0f;
                mario->setFlagpoleSliding(false);
                mario->setAutomaticWalkSpeed(FLAGPOLE_WALK_SPEED);
                mario->setMoveIntent(1.0f);
                m_flagPhase = FlagPhase::TURNING_RIGHT;
                break;
            }

            // The original game gives Mario a short pause at the bottom,
            // then takes control and walks him right into the castle.
            mario->setFlagpoleSliding(false);
            mario->setAutomaticWalkSpeed(FLAGPOLE_WALK_SPEED);
            mario->setMoveIntent(1.0f);
            mario->setRunIntent(false);
            m_flagWalkActive = true;
            m_flagPhase = FlagPhase::WALKING;
        } else if (mario->isFlagpoleSlideComplete() &&
                   !m_tileMap.isFlagFullyDropped()) {
            m_flagPhase = FlagPhase::WAITING_FLAG_DROP;
        }
        break;
    }
    case FlagPhase::TURNING_RIGHT: {
        mario->setMoveIntent(1.0f);
        mario->setRunIntent(false);

        if (mario->getPosition().x >= m_flagTurnTargetX) {
            mario->setPosition({m_flagTurnTargetX,
                                  mario->getPosition().y});
            mario->stopMoving();
            mario->setVelocity({0.0f, 0.0f});
            mario->setMoveIntent(1.0f);
            m_flagWalkActive = true;
            m_flagPhase = FlagPhase::WALKING;
        }
        break;
    }
    case FlagPhase::WALKING: {
        mario->setMoveIntent(1.0f);
        mario->setRunIntent(false);

        // The walk is driven by Mario's actual physics position.
        if (mario->getPosition().x >= m_flagWalkTargetX) {
            const sf::Vector2f stopPosition(m_flagWalkTargetX,
                                            mario->getPosition().y);
            mario->setPosition(stopPosition);
            mario->stopMoving();
            mario->setVelocity({0.0f, 0.0f});
            mario->setAutomaticWalkSpeed(0.0f);
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

void Level::updateEntities(float dt) {
    const sf::View& cameraView = m_camera.getView();

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

        if (!enemy->isActivated()) {
            if (!shouldActivateEnemy(*enemy, cameraView)) {
                continue;
            }

            enemy->activate();
        }

        if ((enemy->isPiranhaPlant() || enemy->isBlooper() || enemy->isLakitu() ||
             enemy->isHammerBro() || enemy->isBowser()) && m_mario) {
            // These enemies react to the closest player so they do not
            // misbehave around a co-op partner.
            sf::Vector2f nearestPos = m_mario->getPosition();
            if (m_coopMode && m_mario2) {
                const float playerOneDistance =
                    std::abs(m_mario->getPosition().x - enemy->getPosition().x);
                const float playerTwoDistance =
                    std::abs(m_mario2->getPosition().x - enemy->getPosition().x);
                if (playerTwoDistance < playerOneDistance) {
                    nearestPos = m_mario2->getPosition();
                }
            }
            if (enemy->isPiranhaPlant()) {
                static_cast<PiranhaPlant*>(enemy)->updateMarioProximity(nearestPos);
            } else if (enemy->isBlooper()) {
                static_cast<Blooper*>(enemy)->updateMarioPosition(nearestPos);
            } else if (enemy->isLakitu()) {
                static_cast<Lakitu*>(enemy)->updateMarioPosition(nearestPos);
            } else if (enemy->isHammerBro()) {
                static_cast<HammerBro*>(enemy)->updateMarioPosition(nearestPos);
            } else {
                static_cast<Bowser*>(enemy)->updateMarioPosition(nearestPos);
            }
        }

        enemy->update(dt);
    }

    // Non-enemy entities that need player awareness (Bullet Bill launchers
    // only fire while someone stands in range).
    if (m_mario) {
        for (auto& entity : m_entities) {
            if (entity && entity->isBulletBillLauncher()) {
                static_cast<BulletBillLauncher*>(entity.get())
                    ->updateMarioPosition(m_mario->getPosition());
            }
        }
    }

    // Spawner entities (Lakitu, launchers) hand their children over through
    // an outbox; Level stays the single owner of the entity list. Collect
    // first, then append - never mutate m_entities while iterating it.
    std::vector<std::unique_ptr<Entity>> adoptedSpawns;
    for (auto& entity : m_entities) {
        if (!entity) {
            continue;
        }
        for (auto& child : entity->takePendingSpawns()) {
            child->setTextureManager(m_textureManager);
            if (child->isEnemy()) {
                auto* childEnemy = static_cast<Enemy*>(child.get());
                childEnemy->setTileMap(&m_tileMap);
                childEnemy->activate();
            }
            adoptedSpawns.push_back(std::move(child));
        }
    }
    for (auto& child : adoptedSpawns) {
        m_entities.push_back(std::move(child));
    }

    // Bowser's axe: a body-less pickup, so the overlap is watched here
    // rather than routed through the contact listener.
    if (!m_bridgeCollapseActive && !m_levelCompleted && m_mario) {
        for (auto& entity : m_entities) {
            if (!entity || !entity->isBowserAxe()) {
                continue;
            }
            if (m_mario->getBoundingBox().findIntersection(entity->getBoundingBox())) {
                beginBridgeCollapse(m_mario.get());
                break;
            }
        }
    }

    // Firebar rotating hazard overlap
    for (auto& entity : m_entities) {
        if (!entity || !entity->isFirebar()) {
            continue;
        }
        Firebar* firebar = static_cast<Firebar*>(entity.get());
        if (m_mario && !m_mario->isDead() && !m_mario->isInvincible()) {
            if (firebar->checkMarioCollision(m_mario->getBoundingBox())) {
                m_mario->loseLife();
            }
        }
        if (m_mario2 && !m_mario2->isDead() && !m_mario2->isInvincible()) {
            if (firebar->checkMarioCollision(m_mario2->getBoundingBox())) {
                m_mario2->loseLife();
            }
        }
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
}

void Level::updateExplosions() {
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
}

void Level::update(float dt) {
    if (m_pvpMode) {
        updatePvp(dt);
        return;
    }

    if (m_coopMode) {
        updateCoop(dt);
        return;
    }

    updateFlagSequence(dt);

    if (m_pipeWarpPhase != PipeWarpPhase::NONE) {
        updatePipeWarp(dt);
        if (m_warpPlayer) {
            m_warpPlayer->updateVisuals(dt);
            const sf::Vector2f centerPos = m_warpPlayer->getPosition() + (m_warpPlayer->getSize() / 2.0f);
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

    processPendingStompScorePopups();

    if (m_pipeWarpCooldown > 0.0f) {
        m_pipeWarpCooldown = std::max(0.0f, m_pipeWarpCooldown - dt);
    }

    checkPipeWarps();

    // Flush any pending fireball creation requests queued while Box2D world was locked
    processPendingFireballs();

    // Update tilemap bump animations
    m_tileMap.update(dt);

    // Bowser arena: advance the bridge-collapse sequence once started.
    updateBridgeCollapse(dt);

    // Process queued tile hits (bumping Question blocks & shattering Brick blocks).
    const bool canBreakBlocks = m_mario && m_mario->canBreakBricks();
    m_tileMap.processPendingHits(m_entities, m_textureManager,
                                 canBreakBlocks, m_mario.get());

    if (m_mario) {
        m_mario->update(dt);
    }

    updateCheepCheepGenerators(dt);
    updateEntities(dt);

    checkItemCollisions();
    checkFinishFlag();
    updateExplosions();
    removeDeadEntities();
    processPendingStompScorePopups();

    // Update camera to follow Mario's center
    if (m_mario) {
        sf::Vector2f centerPos = m_mario->getPosition() + (m_mario->getSize() / 2.0f);
        m_camera.update(dt, centerPos);
    }
}

void Level::updatePvp(float dt) {
    Mario* fighters[2] = {m_mario.get(), m_mario2.get()};

    for (Mario* fighter : fighters) {
        if (fighter) {
            fighter->preparePhysics(dt);
        }
    }

    if (m_world) {
        const bool physicsStepped =
            PhysicsEngine::update(*m_world, dt, m_physicsAccumulator);

        if (physicsStepped) {
            for (Mario* fighter : fighters) {
                if (fighter) {
                    fighter->refreshGroundedState();
                }
            }
        }
    }

    // Apply deferred duel contacts (stomp KOs, fireball hits) after the step.
    processPendingPvpHits();

    processPendingFireballs();

    // Update tilemap bump animations (head bumps on stone stay cosmetic).
    m_tileMap.update(dt);

    for (Mario* fighter : fighters) {
        if (fighter) {
            fighter->update(dt);
        }
    }

    updateEntities(dt);
    checkItemCollisions();
    updateExplosions();

    // Reconcile the on-field flower handle before dead entities are erased.
    if (m_pvpFireFlower &&
        (m_pvpFireFlower->isCollected() || m_pvpFireFlower->shouldRemove() ||
         m_pvpFireFlower->isPendingDestroy() || !m_pvpFireFlower->isActive())) {
        m_pvpFireFlower = nullptr;
    }

    removeDeadEntities();

    for (Mario* fighter : fighters) {
        if (fighter) {
            for (const StompScoreAward& award :
                 fighter->consumePendingStompScoreAwards()) {
                spawnScorePopup(award);
            }
        }
    }

    // The arena is smaller than the logical view, so the camera stays pinned
    // to the arena center; the update still runs to decay shake effects.
    if (m_mario) {
        m_camera.update(dt, m_mario->getPosition() + m_mario->getSize() / 2.0f);
    }
}

void Level::updateCoop(float dt) {
    Mario* players[2] = {m_mario.get(), m_mario2.get()};

    updateFlagSequence(dt);

    if (m_pipeWarpPhase != PipeWarpPhase::NONE) {
        updatePipeWarp(dt);
        if (m_warpPlayer) {
            m_warpPlayer->updateVisuals(dt);
            const sf::Vector2f centerPos =
                m_warpPlayer->getPosition() + (m_warpPlayer->getSize() / 2.0f);
            m_camera.update(dt, centerPos);
        }
        return;
    }

    for (Mario* player : players) {
        if (!player) {
            continue;
        }
        const sf::Vector2f center = player->getPosition() + player->getSize() / 2.0f;
        const int column = static_cast<int>(center.x / static_cast<float>(TILE_SIZE));
        const int row = static_cast<int>(center.y / static_cast<float>(TILE_SIZE));
        const bool onVine = m_tileMap.isClimbable(column, row);
        const float vineCenterX = static_cast<float>(column * TILE_SIZE) + TILE_SIZE / 2.0f;
        player->setClimbContext(onVine, vineCenterX);
        player->preparePhysics(dt);
    }

    if (m_world) {
        const bool physicsStepped =
            PhysicsEngine::update(*m_world, dt, m_physicsAccumulator);

        if (physicsStepped) {
            for (Mario* player : players) {
                if (player) {
                    player->refreshGroundedState();
                }
            }
        }
    }

    processPendingStompScorePopups();

    if (m_pipeWarpCooldown > 0.0f) {
        m_pipeWarpCooldown = std::max(0.0f, m_pipeWarpCooldown - dt);
    }

    checkPipeWarps();

    // Flush any pending fireball creation requests queued while Box2D world was locked
    processPendingFireballs();

    // Update tilemap bump animations
    m_tileMap.update(dt);

    // Either player may break brick blocks; block rewards still credit
    // player one, which the shared team HUD reports as the team total.
    const bool canBreakBlocks = (m_mario && m_mario->canBreakBricks()) ||
                                (m_mario2 && m_mario2->canBreakBricks());
    m_tileMap.processPendingHits(m_entities, m_textureManager,
                                 canBreakBlocks, m_mario.get());

    for (Mario* player : players) {
        if (player) {
            player->update(dt);
        }
    }

    updateCheepCheepGenerators(dt);
    updateEntities(dt);

    checkItemCollisions();
    checkFinishFlag();
    updateExplosions();
    removeDeadEntities();
    processPendingStompScorePopups();

    // The team shares one view: track the midpoint of both players. During
    // the flagpole cinematic the camera follows the player on the pole.
    if (m_flagSequenceActive && m_flagPlayer) {
        m_camera.update(dt, m_flagPlayer->getPosition() + m_flagPlayer->getSize() / 2.0f);
    } else if (m_mario && m_mario2) {
        const sf::Vector2f center1 = m_mario->getPosition() + m_mario->getSize() / 2.0f;
        const sf::Vector2f center2 = m_mario2->getPosition() + m_mario2->getSize() / 2.0f;
        m_camera.update(dt, (center1 + center2) / 2.0f);
    } else if (m_mario) {
        m_camera.update(dt, m_mario->getPosition() + m_mario->getSize() / 2.0f);
    }
}

void Level::processPendingPvpHits() {
    for (const PvpHit& hit : CollisionManager::consumePendingPvpHits()) {
        if (!hit.attacker || !hit.victim || hit.victim->isDying()) {
            continue;
        }

        if (hit.cause == PvpHit::Cause::FIREBALL) {
            // Knockback/stun policy belongs to the round state machine.
            m_pvpFireballHits.push_back(hit);
            continue;
        }

        // Head-stomp KO: the loser plays the death sequence while the winner
        // bounces off, exactly like a defeated-enemy stomp.
        hit.victim->loseLife();
        if (b2Body* attackerBody = hit.attacker->getBody()) {
            attackerBody->SetLinearVelocity(
                b2Vec2(attackerBody->GetLinearVelocity().x,
                       -PhysicsEngine::pixelsToMeters(PVP_STOMP_BOUNCE_SPEED)));
        }
        hit.attacker->clearGroundedState();
        m_camera.shake(0.5f, 12.f);
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

void Level::processPendingStompScorePopups() {
    if (!m_mario) return;

    for (const StompScoreAward& award :
         m_mario->consumePendingStompScoreAwards()) {
        spawnScorePopup(award);
    }

    if (m_coopMode && m_mario2) {
        for (const StompScoreAward& award :
             m_mario2->consumePendingStompScoreAwards()) {
            spawnScorePopup(award);
        }
    }
}

void Level::spawnScorePopup(const StompScoreAward& award) {
    auto popup =
        std::make_unique<ScorePopup>(award.position, award.points, award.grantsLife, m_theme);
    popup->setTextureManager(m_textureManager);
    popup->update(0.f);
    m_entities.push_back(std::move(popup));
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

    // Podoboo belongs behind the liquid layer.
    for (const auto& entity : m_entities) {
        if (!entity || !entity->isPodoboo()) {
            continue;
        }

        target.draw(*entity);
    }

    // Water in normal themes, lava in Castle.
    // Drawn over Podoboo so it appears to emerge from the liquid.
    m_tileMap.renderWater(target);

    // Terrain stays above liquid.
    m_tileMap.render(target);

    // All remaining entities stay above terrain/liquid.
    for (const auto& entity : m_entities) {
        if (!entity || entity->isPodoboo()) {
            continue;
        }

        target.draw(*entity);
    }

    // Draw Mario on top
    if (m_mario) {
        target.draw(*m_mario);
    }
    if (m_mario2) {
        target.draw(*m_mario2);
    }
    
    // Draw foreground tiles (blocks, flagpoles, pipes) on top of Mario and entities
    m_tileMap.renderForeground(target);
}

void Level::checkItemCollisions() {
    if (m_pvpMode || m_coopMode) {
        // Both fighters can contest the fire flower; in co-op both players
        // pick up coins, power-ups, and stars.
        if (m_mario) {
            checkItemCollisionFor(*m_mario);
        }
        if (m_mario2) {
            checkItemCollisionFor(*m_mario2);
        }
        return;
    }

    if (!m_mario) {
        return;
    }
    checkItemCollisionFor(*m_mario);
}

void Level::checkItemCollisionFor(Mario& player) {
    if (player.isCollisionLocked()) return;

    for (auto& entity : m_entities) {
        // Use virtual isItem() instead of dynamic_cast to avoid RTTI overhead
        if (!entity->isItem()) continue;

        Item* item = static_cast<Item*>(entity.get());
        if (item->isCollected()) {
            if (item->isCoin()) {
                const auto* coin = static_cast<const Coin*>(item);
                if (coin->getCoinType() == CoinType::QUESTION_POPUP &&
                    !item->shouldRemove()) {
                    continue;
                }
            }
            if (!item->shouldRemove()) {
                item->markForRemoval();
            }
            continue;
        }

        if (item->checkOverlap(player)) {
            item->onCollect(player);
            item->markForRemoval();
        }
    }
}

void Level::checkFinishFlag() {
    if (!m_mario || m_levelCompleted || m_flagSequenceActive) {
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

    Mario* players[2] = {m_mario.get(), m_coopMode ? m_mario2.get() : nullptr};

    // In co-op the first player to reach the pole claims the exit sequence.
    Mario* triggeringPlayer = nullptr;
    for (Mario* player : players) {
        if (player && !player->isCollisionLocked() &&
            player->getBoundingBox().findIntersection(finishTrigger)) {
            triggeringPlayer = player;
            break;
        }
    }

    if (!triggeringPlayer) {
        return;
    }

    Mario* const mario = triggeringPlayer;
    m_flagPlayer = mario;

    // The flag is a one-shot gameplay sequence.  Keep the level active
    // until Mario has finished the climb and walked into the exit.
    m_flagSequenceActive = true;
    m_flagWalkActive = false;
    m_flagPhase = FlagPhase::SLIDING;
    m_flagSlideStartMarioY = mario->getPosition().y;
    m_flagSlideStartDropDistance = 0.0f;
    const float poleCenterX = triggerPosition.x + TILE_SIZE / 2.0f;
    m_flagPoleCenterX = poleCenterX;
    const float targetTopY = static_cast<float>(bottomRow + 1) * TILE_SIZE -
                             mario->getSize().y;
    mario->beginFlagpoleSlide(poleCenterX, targetTopY);
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
        m_flagWalkTargetX = castleCenter - mario->getSize().x / 2.0f;
    } else {
        const float levelRight = static_cast<float>(m_tileMap.getWidth()) * TILE_SIZE;
        const float rightEdgeTarget = levelRight - mario->getSize().x - 8.0f;
        const float shortWalkTarget = mario->getPosition().x + 4.0f * TILE_SIZE;
        m_flagWalkTargetX = std::min(rightEdgeTarget, shortWalkTarget);
    }
    m_flagWalkTargetX = std::max(m_flagWalkTargetX,
                                 mario->getPosition().x + TILE_SIZE);
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
Mario* Level::getMario2() {
    return m_mario2.get();
}
const Mario* Level::getMario2() const {
    return m_mario2.get();
}
bool Level::isLevelCompleted() const {
    return m_levelCompleted;
}

FireFlower* Level::spawnPvpFireFlower(const sf::Vector2f& position) {
    if (!m_world || m_pvpFireFlower) {
        return nullptr;
    }

    auto flower = std::make_unique<FireFlower>(position, m_world.get());
    flower->setTextureManager(m_textureManager);
    flower->update(0.f);
    m_pvpFireFlower = flower.get();
    m_entities.push_back(std::move(flower));
    return m_pvpFireFlower;
}

void Level::clearPvpFireFlower() {
    if (!m_pvpFireFlower) {
        return;
    }

    for (auto& entity : m_entities) {
        if (entity.get() == m_pvpFireFlower) {
            entity->markForDestroy();
            break;
        }
    }
    m_pvpFireFlower = nullptr;
}

std::vector<PvpHit> Level::consumePvpFireballHits() {
    std::vector<PvpHit> drained;
    drained.swap(m_pvpFireballHits);
    return drained;
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
    if (request.owner) {
        // The projectile shares its shooter's collision group so it passes
        // through him while still reaching the opposing fighter (PvP).
        fireball->setCollisionGroup(request.owner->getFixtureCollisionGroup());
    }
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
        if (req.owner) {
            fireball->setCollisionGroup(req.owner->getFixtureCollisionGroup());
        }
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

void Level::startPipeWarp(Mario* player, char warpId, PipeWarpPhase phase, const sf::Vector2i& pipeTile) {
    if (!player) {
        return;
    }

    m_warpPlayer = player;
    m_pendingWarpId = warpId;
    m_pendingPipeTile = pipeTile;
    m_pipeWarpPhase = phase;
    m_pipeWarpTimer = 0.5f; // 0.5s smooth slide duration

    SoundManager::getInstance().playSound("powerdown");

    if (player->getBody()) {
        player->getBody()->SetGravityScale(0.0f);
        player->getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
    }
    player->setVelocity({0.0f, 0.0f});

    if (phase == PipeWarpPhase::ENTERING_VERTICAL) {
        if (player->getMarioState() == MarioState::SUPER ||
            player->getMarioState() == MarioState::FIRE_SUPER) {
            player->playAnimation("crouch");
        } else {
            player->playAnimation("idle");
        }
    } else if (phase == PipeWarpPhase::ENTERING_HORIZONTAL) {
        player->playAnimation("walk");
    }
}

void Level::updatePipeWarp(float dt) {
    if (!m_warpPlayer || m_pipeWarpPhase == PipeWarpPhase::NONE) {
        return;
    }
    Mario* const mario = m_warpPlayer;

    if (mario->getBody()) {
        mario->getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
        mario->getBody()->SetGravityScale(0.0f);
    }
    mario->setVelocity({0.0f, 0.0f});

    constexpr float PIPE_SLIDE_SPEED = 48.0f; // px/sec

    if (m_pipeWarpPhase == PipeWarpPhase::ENTERING_VERTICAL) {
        m_pipeWarpTimer -= dt;
        const sf::Vector2f pos = mario->getPosition();
        const float pipeCenterX = static_cast<float>(m_pendingPipeTile.x * TILE_SIZE) + static_cast<float>(TILE_SIZE);
        const float targetX = pipeCenterX - mario->getSize().x / 2.0f;
        const float newX = pos.x + (targetX - pos.x) * std::min(1.0f, dt * 10.0f);
        mario->setPosition({newX, pos.y + PIPE_SLIDE_SPEED * dt});

        if (mario->getMarioState() == MarioState::SUPER ||
            mario->getMarioState() == MarioState::FIRE_SUPER) {
            mario->playAnimation("crouch");
        } else {
            mario->playAnimation("idle");
        }

        if (m_pipeWarpTimer <= 0.0f) {
            m_pipeWarpPhase = PipeWarpPhase::WARPING_DELAY;
            m_pipeWarpTimer = 0.35f; // 350ms transition delay
            warpMarioToReturn(m_pendingWarpId);
        }
    } else if (m_pipeWarpPhase == PipeWarpPhase::ENTERING_HORIZONTAL) {
        m_pipeWarpTimer -= dt;
        const sf::Vector2f pos = mario->getPosition();
        mario->setPosition({pos.x + 60.0f * dt, pos.y});
        mario->playAnimation("walk");

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
        const sf::Vector2f pos = mario->getPosition();
        const float newY = std::max(m_pipeWarpExitTargetY, pos.y - PIPE_SLIDE_SPEED * dt);
        mario->setPosition({pos.x, newY});
        mario->playAnimation("idle");

        if (m_pipeWarpTimer <= 0.0f || pos.y <= m_pipeWarpExitTargetY) {
            mario->setPosition({pos.x, m_pipeWarpExitTargetY});
            m_pipeWarpPhase = PipeWarpPhase::NONE;
            m_pipeWarpCooldown = 0.5f;
            if (mario->getBody()) {
                mario->getBody()->SetGravityScale(1.0f);
            }
            mario->playAnimation("idle");
            // Co-op: the partner travels along so the team shares one area.
            teleportCoopPartner(*mario);
            m_warpPlayer = nullptr;
        }
    }
}

void Level::teleportCoopPartner(const Mario& warpingPlayer) {
    if (!m_coopMode || !m_mario || !m_mario2) {
        return;
    }

    Mario* partner = (m_warpPlayer == m_mario.get()) ? m_mario2.get() : m_mario.get();

    // Stand beside the warp exit when the adjacent tile is free; otherwise
    // share the exact spot and let the solver separate the two bodies.
    const sf::Vector2f destination = warpingPlayer.getPosition();
    const int column = static_cast<int>(destination.x / static_cast<float>(TILE_SIZE));
    const int row = static_cast<int>(destination.y / static_cast<float>(TILE_SIZE));
    sf::Vector2f partnerPos = destination;
    for (int dc : {1, -1}) {
        const int c = column + dc;
        if (c >= 0 && c < static_cast<int>(m_tileMap.getWidth()) &&
            !m_tileMap.isSolid(c, row)) {
            partnerPos = sf::Vector2f(destination.x + static_cast<float>(dc) * TILE_SIZE,
                                      destination.y);
            break;
        }
    }

    partner->setPosition(partnerPos);
    partner->setVelocity({0.0f, 0.0f});
    if (b2Body* body = partner->getBody()) {
        body->SetLinearVelocity(b2Vec2(0.f, 0.f));
        body->SetGravityScale(1.0f);
    }
    partner->playAnimation("idle");
}

void Level::warpMarioToReturn(char warpId) {
    if (!m_warpPlayer) {
        return;
    }
    Mario* const mario = m_warpPlayer;

    const auto destination = m_tileMap.findWarpReturn(warpId);

    if (!destination) {
        return;
    }

    const sf::Vector2i returnPosition = *destination;

    // Level 3 constains multiple enviromental areas:
    //
    // H1 -> R1 : Overworld -> Underwater
    // H2 -> R2 : Underwater -> Overworld
    if (m_levelPath.find("level3.txt") != std::string::npos) {
        if (warpId == '1') {
            applyAreaTheme(LevelTheme::UNDERWATER);
        } else if (warpId == '2') {
            applyAreaTheme(LevelTheme::OVERWORLD);
        }
    }

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

    const sf::Vector2f marioSize = mario->getSize();

    // RN marks the tile where Mario's feet should end.
    // Works for both Small and Big Mario.
    const sf::Vector2f target{static_cast<float>(returnPosition.x * TILE_SIZE),
                              static_cast<float>((returnPosition.y + 1) * TILE_SIZE) - marioSize.y
                              };

    mario->setVelocity({0.0f, 0.0f});
    if (mario->getBody()) {
        mario->getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
    }

    // Check if the tile below is a pipe (so Mario should emerge upwards out of the pipe)
    const char symBelow = m_tileMap.getTileAt(tileBelow.x, tileBelow.y);
    const bool isPipeReturn = (symBelow == '[' || symBelow == ']' || symBelow == 'p' ||
                               symBelow == 'r' || symBelow == '{' || symBelow == '}');

    if (isPipeReturn) {
        // Start Mario positioned down inside the pipe, and slide upwards
        const sf::Vector2f startPos{target.x, target.y + marioSize.y};
        mario->setPosition(startPos);
        m_pipeWarpExitTargetY = target.y;
        m_pipeWarpPhase = PipeWarpPhase::EXITING_VERTICAL;
        m_pipeWarpTimer = 0.45f;
        SoundManager::getInstance().playSound("powerdown");
    } else {
        mario->setPosition(target);
        m_pipeWarpPhase = PipeWarpPhase::NONE;
        m_pipeWarpCooldown = 0.5f;
        if (mario->getBody()) {
            mario->getBody()->SetGravityScale(1.0f);
        }
        mario->playAnimation("idle");
        // Co-op: the partner travels along so the team shares one area.
        teleportCoopPartner(*mario);
        m_warpPlayer = nullptr;
    }

    // Immediately snap camera to destination
    const sf::Vector2f centerPos = target + (marioSize / 2.0f);
    m_camera.update(0.0f, centerPos);
}

void Level::checkPipeWarps() {
    if (!m_mario || m_pipeWarpCooldown > 0.0f ||
        m_pipeWarpPhase != PipeWarpPhase::NONE ||
        m_flagSequenceActive ||
        m_levelCompleted) {
        return;
    }

    Mario* players[2] = {m_mario.get(), m_coopMode ? m_mario2.get() : nullptr};

    for (Mario* mario : players) {
        if (!mario || mario->isCollisionLocked()) {
            continue;
        }

        const sf::Vector2f marioPos = mario->getPosition();

        const sf::Vector2f marioSize = mario->getSize();

        for (const auto& entry : m_tileMap.getWarpEntries()) {
            const sf::Vector2i pipe = entry.position;

            // ==============================================
            // Hn — horizontal pipe
            // Walk RIGHT into it.
            // ==============================================
            if (entry.type == TileMap::WarpEntryType::HORIZONTAL) {
                if (mario->getHorizontalIntent() <= 0.5f) {
                    continue;
                }

                const float pipeLeft = static_cast<float>(pipe.x * TILE_SIZE);
                const float pipeTop = static_cast<float>((pipe.y - 1) * TILE_SIZE);
                const float pipeBottom = static_cast<float>((pipe.y + 1) * TILE_SIZE);
                const float marioRight = marioPos.x + marioSize.x;

                const bool touchingPipe = std::abs(marioRight - pipeLeft) <= 12.0f;

                const bool verticallyAligned = marioPos.y < pipeBottom && marioPos.y + marioSize.y > pipeTop;

                if (touchingPipe && verticallyAligned) {
                    startPipeWarp(mario, entry.id, PipeWarpPhase::ENTERING_HORIZONTAL, entry.position);
                    return;
                }

                continue;
            }

            // ==============================================
            // [n / pn — vertical pipe
            // Stand on it + press Down.
            // ==============================================
            if (mario->getVerticalIntent() <= 0.5f || !mario->isGrounded()) {
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
                startPipeWarp(mario, entry.id, PipeWarpPhase::ENTERING_VERTICAL, entry.position);
                return;
            }
        }
    }
}

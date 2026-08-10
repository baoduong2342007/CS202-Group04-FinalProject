/**
 * @file Level.cpp
 * @author TV1 (Dương)
 * @brief Level implementation — loads map, spawns entities, orchestrates update/render
 * @note Sprint 4 fix: TextureManager wired to entities, item collision, dead entity cleanup
 */

#include "level/Level.h"

#include <algorithm>
#include <iostream>
#include <cassert>

#include "items/Item.h"
#include "items/Coin.h"
#include "patterns/EntityFactory.h"
#include "patterns/EventBus.h"
#include "physics/PhysicsEngine.h"
#include "physics/ContactListener.h"
#include "entities/Enemy.h"
#include "entities/FireBall.h"
#include "core/SpriteFrames_ovw.h"
#include "core/SoundManager.h"
#include "core/LevelCatalog.h"

#include "core/DisplayConfig.h"

namespace {
constexpr unsigned int TILE_SIZE = 32;
constexpr float ENEMY_ACTIVATION_MARGIN = 64.f;
constexpr float ENTITY_CLEANUP_MARGIN = 64.f;
const sf::Color UNDERGROUND_BACKGROUND_COLOR(0, 0, 128);


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

// Tile codes that represent spawnable standalone entities (Goomba, Koopa, Coin, QuestionBlock)
constexpr char SPAWN_CODES[] = {'G', 'K', 'C', '?', 'f', 'h', 'U', 'u', 'O', 'o'};

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
            if (tileMap.getTileAt(static_cast<int>(column), static_cast<int>(row)) == '1') {
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
Level::~Level() = default;

void Level::setTheme(LevelTheme theme) {
    m_theme = theme;
    m_tileMap.setTheme(theme);
}

bool Level::loadFromFile(const std::string& path) {
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

    // Must be called BEFORE spawnEntitiesFromTileMap() so entities have ground to land on
    m_world = std::make_unique<b2World>(b2Vec2(0.f, 25.0f));
    m_contactListener = std::make_unique<ContactListener>(m_tileMap);
    m_world->SetContactListener(m_contactListener.get());

    if (m_world) {
        m_tileMap.createPhysicsBodies(m_world.get());
    }

    // Spawn Mario and all entities from tile codes
    spawnEntitiesFromTileMap();

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
        m_mario->setRespawnPosition(spawnPos);
        m_mario->setPitThreshold(levelHeight + 64.f);
    } else {
        std::cerr << "Level: No Mario spawn point ('M') found! " << "Defaulting to (100, 100)" << std::endl;
        m_mario = std::make_unique<Mario>();
    }

    // Wire TextureManager to Mario so setSprite() works
    m_mario->setTextureManager(m_textureManager);

    // Initialize Mario physics body
    m_mario->initPhysics(m_world.get(), b2_dynamicBody, sf::Vector2f(32.f, 32.f));

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

}

void Level::update(float dt) {
    if (m_mario) {
        m_mario->preparePhysics(dt);
    }

    if (m_world) {
        const bool physicsStepped = PhysicsEngine::update(*m_world, dt);
        if (physicsStepped && m_mario) {
            m_mario->refreshGroundedState();
        }
    }

    // Flush any pending fireball creation requests queued while Box2D world was locked
    processPendingFireballs();

    // Update tilemap bump animations
    m_tileMap.update(dt);

    // Process queued tile hits (bumping Question blocks & shattering Brick blocks)
    bool isBigMario = (m_mario && m_mario->getMarioState() != MarioState::SMALL);
    m_tileMap.processPendingHits(m_entities, m_textureManager, isBigMario, m_mario.get());

    // Update Mario
    if (m_mario) {
        m_mario->update(dt);
    }

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

    // Remove dead entities
    removeDeadEntities();

    // Update camera to follow Mario's center
    if (m_mario) {
        sf::Vector2f centerPos = m_mario->getPosition() + (m_mario->getSize() / 2.0f);
        m_camera.update(dt, centerPos);
    }
}

bool Level::spawnFireBall() {
    if (!m_mario || !m_world) {
        return false;
    }

    std::unique_ptr<FireBall> fireBall = m_mario->shootFireBall(m_world.get());
    if (!fireBall) {
        return false;
    }

    fireBall->setOwner(m_mario.get());
    fireBall->setTextureManager(m_textureManager);
    m_entities.push_back(std::move(fireBall));
    EventBus::getInstance().notify(EventType::FIREBALL_SHOT);
    return true;
}

void Level::render(sf::RenderTarget& target) {
    // Apply camera view
    target.setView(m_camera.getView());

    const float backgroundHeight = static_cast<float>(DisplayConfig::LOGICAL_HEIGHT);
    const float levelWidth = static_cast<float>(m_tileMap.getWidth() * TILE_SIZE);
    const float levelHeight = static_cast<float>(m_tileMap.getHeight() * TILE_SIZE);

    if (m_theme == LevelTheme::UNDERGROUND) {
        // SMB underground stages use a flat dark-blue field behind the same
        // tile geometry. Extend it beyond the level bounds so camera motion
        // cannot reveal the sky-blue render-texture clear color.
        sf::RectangleShape undergroundBackground(
            sf::Vector2f(levelWidth, levelHeight + backgroundHeight * 2.f));
        undergroundBackground.setPosition({0.f, -backgroundHeight});
        undergroundBackground.setFillColor(UNDERGROUND_BACKGROUND_COLOR);
        target.draw(undergroundBackground);
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
    if (!m_mario) return;

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
    if (!m_mario || m_levelCompleted) {
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
        m_levelCompleted = true;

        EventBus::getInstance().notify(EventType::LEVEL_COMPLETED);
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

constexpr int MAX_ACTIVE_FIREBALLS = 4;

void Level::shootFireBall() {
    if (!m_mario || !m_world || !m_mario->canShootFireBall()) return;

    int activeFireballs = 0;
    for (const auto& entity : m_entities) {
        if (entity && entity->isFireBall() && entity->isActive()) {
            activeFireballs++;
        }
    }
    activeFireballs += static_cast<int>(m_pendingFireBallRequests.size());

    if (activeFireballs >= MAX_ACTIVE_FIREBALLS) {
        return;
    }

    if (m_world->IsLocked()) {
        float spawnX = m_mario->getPosition().x + (m_mario->getFacingDirection() == Direction::RIGHT ? m_mario->getSize().x + 4.f : -16.f);
        float spawnY = m_mario->getPosition().y + 4.f;
        m_pendingFireBallRequests.push_back({sf::Vector2f(spawnX, spawnY), m_mario->getFacingDirection()});
        return;
    }

    auto fireball = m_mario->shootFireBall(m_world.get());
    if (fireball) {
        fireball->setTextureManager(m_textureManager);
        m_entities.push_back(std::move(fireball));
        SoundManager::getInstance().playSound("fireball");
    }
}

void Level::processPendingFireballs() {
    if (!m_world || m_world->IsLocked() || m_pendingFireBallRequests.empty()) return;

    for (const auto& req : m_pendingFireBallRequests) {
        auto fireball = std::make_unique<FireBall>(req.position, req.direction, m_world.get());
        if (fireball) {
            fireball->setTextureManager(m_textureManager);
            m_entities.push_back(std::move(fireball));
            SoundManager::getInstance().playSound("fireball");
        }
    }
    m_pendingFireBallRequests.clear();
}

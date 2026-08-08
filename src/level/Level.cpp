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
#include "core/SpriteFrames.h"
#include "core/SoundManager.h"
#include "entities/FireBall.h"

#include "core/DisplayConfig.h"

namespace {
constexpr unsigned int TILE_SIZE = 32;

// Tile codes that represent spawnable standalone entities (Goomba, Koopa, Coin, QuestionBlock)
constexpr char SPAWN_CODES[] = {'G', 'K', 'C', '?', 'f', 'h', 'U', 'u', 'O', 'o'};

float calculateBackgroundTop(std::size_t levelHeightInTiles) {
    const float levelHeight = static_cast<float>(levelHeightInTiles * TILE_SIZE);
    const float groundTop = std::max(0.f, levelHeight - static_cast<float>(TILE_SIZE));
    const float backgroundHeight = static_cast<float>(SpriteFrames::Backgrounds::OVERWORLD.size.y);
    return std::max(0.f, groundTop - backgroundHeight);
}
} // namespace


Level::Level() : m_textureManager(TextureManager::getInstance()) {}
Level::~Level() = default;

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
                EntityFactory::createFromTileCode(code, worldPos, m_world.get());
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
        entity->update(dt);
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

    // Draw Mountain Background for main levels (temporarily disabled for level0)
    if (m_levelPath.find("level0") == std::string::npos) {
        const sf::Texture& bgTex = m_textureManager.getTexture(std::string(SpriteFrames::Backgrounds::MOUNTAINS_PATH));
        sf::Sprite bgSprite(bgTex);
        bgSprite.setTextureRect(SpriteFrames::Backgrounds::OVERWORLD);

        float stripWidth = static_cast<float>(SpriteFrames::Backgrounds::OVERWORLD.size.x);
        float levelWidth = static_cast<float>(m_tileMap.getWidth() * TILE_SIZE);
        float backgroundTop = calculateBackgroundTop(m_tileMap.getHeight());

        for (float x = 0; x < levelWidth + stripWidth; x += stripWidth) {
            bgSprite.setPosition(sf::Vector2f(x, backgroundTop));
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

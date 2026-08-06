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
#include "patterns/EntityFactory.h"
#include "patterns/EventBus.h"
#include "physics/PhysicsEngine.h"
#include "physics/ContactListener.h"
#include "entities/Goomba.h"
#include "core/SpriteFrames.h"

namespace {
constexpr unsigned int SCREEN_WIDTH = 1280;
constexpr unsigned int SCREEN_HEIGHT = 720;
constexpr unsigned int TILE_SIZE = 32;

// Tile codes that represent spawnable standalone entities (Goomba, Koopa, Coin, QuestionBlock)
constexpr char SPAWN_CODES[] = {'G', 'K', 'C', '?', 'U', 'O'};

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
    float levelWidth = static_cast<float>(m_tileMap.getWidth() * TILE_SIZE);
    float levelHeight = static_cast<float>(m_tileMap.getHeight() * TILE_SIZE);

    m_camera.init(
        sf::Vector2f(static_cast<float>(SCREEN_WIDTH),
                     static_cast<float>(SCREEN_HEIGHT)),
        sf::FloatRect(sf::Vector2f(0.f, 0.f),
                      sf::Vector2f(levelWidth, levelHeight))
    );

    // Must be called BEFORE spawnEntitiesFromTileMap() so entities have ground to land on
    m_world = std::make_unique<b2World>(b2Vec2(0.f, 25.0f));
    m_contactListener = std::make_unique<ContactListener>();
    m_world->SetContactListener(m_contactListener.get());

    if (m_world) {
        m_tileMap.createPhysicsBodies(m_world.get());
    }

    // Spawn Mario and all entities from tile codes
    spawnEntitiesFromTileMap();

    return true;
}

void Level::spawnEntitiesFromTileMap() {
    // --- Spawn Mario from 'M' tile code (see level1.txt) ---
    auto marioSpawns = m_tileMap.findTiles('M');

    if (!marioSpawns.empty()) {
        sf::Vector2f spawnPos =
            TileMap::gridToWorldPosition(marioSpawns.front());
        m_mario = std::make_unique<Mario>(spawnPos,
                                          sf::Vector2f(32.f, 32.f));
        m_mario->setRespawnPosition(spawnPos);
    } else {
        std::cerr << "Level: No Mario spawn point ('M') found! "
                  << "Defaulting to (100, 100)" << std::endl;
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
                    Goomba* goomba = dynamic_cast<Goomba*>(entity.get());
                    if (goomba) goomba->setTileMap(&m_tileMap);
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

void Level::render(sf::RenderWindow& window) {
    // Apply camera view
    window.setView(m_camera.getView());

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
            window.draw(bgSprite);
        }
    }



    // Draw tilemap background
    m_tileMap.render(window);

    // Draw all entities (enemies, items)
    for (const auto& entity : m_entities) {
        window.draw(*entity);
    }

    // Draw Mario on top
    if (m_mario) {
        window.draw(*m_mario);
    }
}

void Level::checkItemCollisions() {
    if (!m_mario) return;

    for (auto& entity : m_entities) {
        // Use virtual isItem() instead of dynamic_cast to avoid RTTI overhead
        if (!entity->isItem()) continue;

        Item* item = static_cast<Item*>(entity.get());
        if (!item->isCollected()) {
            if (item->checkOverlap(*m_mario)) {
                item->onCollect(*m_mario);
                item->markForRemoval();
            }
        }
    }
}

void Level::checkFinishFlag() {
    if (!m_mario || m_levelCompleted) return;

    auto flags = m_tileMap.findTiles('F');
    sf::FloatRect marioBounds = m_mario->getBoundingBox();

    for (const auto& gridPos : flags) {
        sf::Vector2f worldPos = TileMap::gridToWorldPosition(gridPos);
        sf::FloatRect flagBounds(worldPos, sf::Vector2f(32.f, 32.f));

        if (marioBounds.findIntersection(flagBounds)) {
            m_levelCompleted = true;
            EventBus::getInstance().notify(EventType::LEVEL_COMPLETED);
            break;
        }
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

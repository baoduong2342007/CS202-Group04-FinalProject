/**
 * @file Level.cpp
 * @author TV1 (Dương)
 * @brief Level implementation — loads map, spawns entities, orchestrates update/render
 * @note Sprint 4 — uses EntityFactory for spawning
 */

#include "level/Level.h"

#include <algorithm>
#include <iostream>

#include "patterns/EntityFactory.h"
#include "physics/PhysicsEngine.h"

namespace {
constexpr unsigned int SCREEN_WIDTH = 1280;
constexpr unsigned int SCREEN_HEIGHT = 720;
constexpr unsigned int TILE_SIZE = 32;

// Tile codes that represent spawnable entities
constexpr char SPAWN_CODES[] = {'G', 'K', 'C'};
} // namespace

Level::Level() = default;

bool Level::loadFromFile(const std::string& path) {
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

    // Create Box2D static bodies for solid tiles (ground, bricks, question blocks)
    // Must be called BEFORE spawnEntitiesFromTileMap() so entities have ground to land on
    b2World* world = PhysicsEngine::getInstance().getWorld();
    if (world) {
        m_tileMap.createPhysicsBodies(world);
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
    } else {
        std::cerr << "Level: No Mario spawn point ('M') found! "
                  << "Defaulting to (100, 100)" << std::endl;
        m_mario = std::make_unique<Mario>();
    }

    // Initialize Mario physics body
    m_mario->initPhysics(b2_dynamicBody, sf::Vector2f(32.f, 32.f));

    // --- Spawn enemies and items via Factory ---
    for (char code : SPAWN_CODES) {
        auto positions = m_tileMap.findTiles(code);
        for (const auto& gridPos : positions) {
            sf::Vector2f worldPos =
                TileMap::gridToWorldPosition(gridPos);
            Entity* raw =
                EntityFactory::createFromTileCode(code, worldPos);
            if (raw) {
                m_entities.emplace_back(raw);
            }
        }
    }
}

void Level::update(float dt) {
    // Update Mario
    if (m_mario) {
        m_mario->update(dt);
    }

    // Update all entities (enemies, items)
    for (auto& entity : m_entities) {
        entity->update(dt);
    }

    // Remove dead entities
    removeDeadEntities();

    // Update camera to follow Mario
    if (m_mario) {
        m_camera.update(m_mario->getPosition());
    }
}

void Level::render(sf::RenderWindow& window) {
    // Apply camera view
    window.setView(m_camera.getView());

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

void Level::removeDeadEntities() {
    m_entities.erase(
        std::remove_if(m_entities.begin(), m_entities.end(),
            [](const std::unique_ptr<Entity>& e) {
                return !e || !e->isActive() || e->isPendingDestroy();
            }),
        m_entities.end()
    );
}

// --- Getters ---
Mario& Level::getMario() { return *m_mario; }
const Mario& Level::getMario() const { return *m_mario; }
TileMap& Level::getTileMap() { return m_tileMap; }
Camera& Level::getCamera() { return m_camera; }

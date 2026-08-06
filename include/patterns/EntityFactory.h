/**
 * @file EntityFactory.h
 * @author TV1 (Dương)
 * @brief Simple Factory for instantiating polymorphic game entities
 * @note Week 4 — provides creation methods for enemies and items based on types or tile codes
 * @note Sprint 6 — S6-TV1-23: this is a Simple Factory (static utility with switch),
 *       NOT the canonical Factory Method pattern, which requires subclassable
 *       creator classes with virtual factory methods.
 */

#pragma once

// 2. SFML
#include <SFML/Graphics.hpp>

// 3. Project headers
#include "entities/Entity.h"

enum class EnemyType {
    GOOMBA,
    KOOPA
};

enum class ItemType {
    COIN,
    MUSHROOM,
    FIRE_FLOWER,
    STAR
};

// ============================================================
// PATTERN: Simple Factory
// Reason: avoids hardcoded new Goomba(), new Koopa() across TileMap or Level;
//         allows adding new enemy and item types cleanly without modifying level loading code.
// Note:   EntityFactory::create() methods are the ONLY exception to the raw pointer rule in coding.md.
//         They return raw Entity* so the caller (e.g. TileMap) can immediately wrap into std::unique_ptr<Entity>.
// ============================================================
class EntityFactory {
public:
    // 1. Constructor / Destructor
    EntityFactory() = delete; // Static utility class
    ~EntityFactory() = delete;

    // 3. Public static methods
    /// Create an enemy by type (caller owns returned memory via unique_ptr)
    static Entity* createEnemy(EnemyType type, const sf::Vector2f& position, b2World* world);

    /// Create an item by type (caller owns returned memory via unique_ptr)
    static Entity* createItem(ItemType type, const sf::Vector2f& position, b2World* world);

    /// Helper for TV4's TileMap parser: maps character code ('G', 'K', 'C', '?') to exact entity
    static Entity* createFromTileCode(char tileCode, const sf::Vector2f& position, b2World* world);
};
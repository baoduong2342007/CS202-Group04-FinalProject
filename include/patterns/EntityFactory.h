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

// 1. Standard library
#include <memory>

// 2. SFML
#include <SFML/Graphics.hpp>

// 3. Project headers
#include "core/LevelCatalog.h"
#include "entities/Entity.h"

enum class EnemyType {
    GOOMBA,
    KOOPA,
    PIRANHA_PLANT,
    CHEEP_CHEEP,
    BUZZY_BEETLE,
    RED_KOOPA,
    PARATROOPA_HOP,
    PARATROOPA_FLY,
    PIRANHA_PLANT_RED,
    BLOOPER,
    PODOBOO,
    BULLET_BILL,
    LAKITU,
    SPINY,
    HAMMER_BRO,
    BOWSER,
    FIREBAR
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
// Note:   S6-TV1-22 — all create() methods return std::unique_ptr<Entity> so the
//         caller never owns a raw pointer. Raw pointers are only used as
//         non-owning references elsewhere in the codebase.
// ============================================================
class EntityFactory {
public:
    // 1. Constructor / Destructor
    EntityFactory() = delete; // Static utility class
    ~EntityFactory() = delete;

    // 3. Public static methods
    /// Create an enemy by type (caller owns the returned unique_ptr)
    static std::unique_ptr<Entity> createEnemy(EnemyType type,
                                               const sf::Vector2f& position,
                                               b2World* world,
                                               LevelTheme theme = LevelTheme::OVERWORLD);

    /// Create an item by type (caller owns the returned unique_ptr)
    static std::unique_ptr<Entity> createItem(ItemType type,
                                              const sf::Vector2f& position,
                                              b2World* world,
                                              LevelTheme theme = LevelTheme::OVERWORLD);

    /// Helper for TV4's TileMap parser: maps character code ('G', 'K', 'C', '?') to exact entity
    static std::unique_ptr<Entity> createFromTileCode(char tileCode,
                                                      const sf::Vector2f& position,
                                                      b2World* world,
                                                      LevelTheme theme = LevelTheme::OVERWORLD);
};

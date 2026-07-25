/**
 * @file EntityFactory.cpp
 * @author TV1 (Dương)
 * @brief Factory Method pattern implementation for instantiating polymorphic game entities
 * @note Week 4 — uses hook points for TV4 (Enemy) and TV5 (Item) concrete classes
 */

#include "patterns/EntityFactory.h"

// ============================================================
// TV4 & TV5 HOOK POINTS:
// When your concrete entity classes (Goomba.h, Coin.h, etc.) are merged into develop,
// uncomment the includes below and update the return statements inside createEnemy/createItem!
// ============================================================
// #include "entities/Goomba.h"      // TV4 (Sprint 4)
// #include "entities/Koopa.h"       // TV4 (Sprint 5)
#include "items/Coin.h"              // TV5 (Sprint 4)
// #include "items/Mushroom.h"       // TV5 (Sprint 5)
// #include "items/FireFlower.h"     // TV5 (Sprint 5)
// #include "items/Star.h"           // TV5 (Sprint 5)

// ============================================================
// PATTERN: Factory Method Implementation
// ============================================================
Entity* EntityFactory::createEnemy(EnemyType type, const sf::Vector2f& position) {
    (void)position; // Suppress unused parameter warning until concrete classes are linked
    switch (type) {
        case EnemyType::GOOMBA:
            // TODO (TV4): Uncomment when Goomba.h is merged into develop:
            // return new Goomba(position);
            return nullptr;
        case EnemyType::KOOPA:
            // TODO (TV4): Sprint 5 - return new Koopa(position);
            return nullptr;
        default:
            return nullptr;
    }
}

Entity* EntityFactory::createItem(ItemType type, const sf::Vector2f& position) {
    switch (type) {
        case ItemType::COIN:
            return new Coin(position);
        case ItemType::MUSHROOM:
            // TODO (TV5): Sprint 5 - return new Mushroom(position);
            return nullptr;
        case ItemType::FIRE_FLOWER:
            // TODO (TV5): Sprint 5 - return new FireFlower(position);
            return nullptr;
        case ItemType::STAR:
            // TODO (TV5): Sprint 5 - return new Star(position);
            return nullptr;
        default:
            return nullptr;
    }
}

Entity* EntityFactory::createFromTileCode(char tileCode, const sf::Vector2f& position) {
    switch (tileCode) {
        case 'G':
            return createEnemy(EnemyType::GOOMBA, position);
        case 'K':
            return createEnemy(EnemyType::KOOPA, position);
        case 'C':
            return createItem(ItemType::COIN, position);
        case '?':
            return createItem(ItemType::MUSHROOM, position);
        default:
            return nullptr;
    }
}

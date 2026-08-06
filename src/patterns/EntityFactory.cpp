/**
 * @file EntityFactory.cpp
 * @author TV1 (Dương)
 * @brief Simple Factory implementation for instantiating polymorphic game entities
 * @note Week 4 — uses hook points for TV4 (Enemy) and TV5 (Item) concrete classes
 * @note Sprint 6 — S6-TV1-23: this is a Simple Factory, not canonical Factory Method
 */

#include "patterns/EntityFactory.h"

// ============================================================
// TV4 & TV5 HOOK POINTS:
// When your concrete entity classes (Goomba.h, Coin.h, etc.) are merged into develop,
// uncomment the includes below and update the return statements inside createEnemy/createItem!
// ============================================================
#include "entities/Goomba.h"          // TV4 (Sprint 4)
#include "entities/Koopa.h"           // TV4 (Sprint 5)
#include "items/Coin.h"              // TV5 (Sprint 4)
#include "items/Mushroom.h"         // TV5 (Sprint 5)
#include "items/FireFlower.h"       // TV5 (Sprint 5)
#include "items/Star.h"             // TV5 (Sprint 5)
#include "entities/QuestionBlock.h"    // TV5 (Sprint 4)

// ============================================================
// PATTERN: Simple Factory Implementation
// ============================================================
Entity* EntityFactory::createEnemy(EnemyType type, const sf::Vector2f& position, b2World* world) {
    switch (type) {
        case EnemyType::GOOMBA:
            return new Goomba(position, world);
        case EnemyType::KOOPA:
            return new Koopa(position, world);
        default:
            return nullptr;
    }
}

Entity* EntityFactory::createItem(ItemType type, const sf::Vector2f& position, b2World* world) {
    switch (type) {
        case ItemType::COIN:
            return new Coin(position, world);
        case ItemType::MUSHROOM:
            return new Mushroom(position, world);
        case ItemType::FIRE_FLOWER:
            return new FireFlower(position, world);
        case ItemType::STAR:
            return new Star(position, world);
        default:
            return nullptr;
    }
}

Entity* EntityFactory::createFromTileCode(char tileCode, const sf::Vector2f& position, b2World* world) {
    switch (tileCode) {
        case 'G':
            return createEnemy(EnemyType::GOOMBA, position, world);
        case 'K':
            return createEnemy(EnemyType::KOOPA, position, world);
        case 'C':
            return createItem(ItemType::COIN, position, world);
        case '?':
            return new QuestionBlock(position, world, QuestionBlockContent::COIN);
        case 'U':
            return new QuestionBlock(position, world, QuestionBlockContent::SUPER_MUSHROOM);
        case 'O':
            return new QuestionBlock(position, world, QuestionBlockContent::ONEUP_MUSHROOM);
        default:
            return nullptr;
    }
}

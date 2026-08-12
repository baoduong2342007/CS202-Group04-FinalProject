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
#include "entities/PiranhaPlant.h"    // TV4 (Sprint 7)
#include "items/Coin.h"              // TV5 (Sprint 4)
#include "items/Mushroom.h"         // TV5 (Sprint 5)
#include "items/FireFlower.h"       // TV5 (Sprint 5)
#include "items/Star.h"             // TV5 (Sprint 5)
#include "entities/Springboard.h"     // TV1 (Sprint 7)
#include "entities/QuestionBlock.h"    // TV5 (Sprint 4)

namespace {

BlockTheme toBlockTheme(LevelTheme theme) {
    switch (theme) {
        case LevelTheme::UNDERGROUND:
            return BlockTheme::UNDERGROUND;
        case LevelTheme::CASTLE:
            return BlockTheme::CASTLE;
        case LevelTheme::UNDERWATER:
            return BlockTheme::UNDERWATER;
        case LevelTheme::OVERWORLD:
        default:
            return BlockTheme::OVERWORLD;
    }
}

} // namespace

// ============================================================
// PATTERN: Simple Factory Implementation
// ============================================================
std::unique_ptr<Entity> EntityFactory::createEnemy(EnemyType type,
                                                   const sf::Vector2f& position,
                                                   b2World* world,
                                                   LevelTheme theme) {
    switch (type) {
        case EnemyType::GOOMBA:
            return std::make_unique<Goomba>(position, world, theme);
        case EnemyType::KOOPA:
            return std::make_unique<Koopa>(position, world, theme);
        case EnemyType::PIRANHA_PLANT:
            return std::make_unique<PiranhaPlant>(position, world, theme);
        default:
            return nullptr;
    }
}

std::unique_ptr<Entity> EntityFactory::createItem(ItemType type, const sf::Vector2f& position, b2World* world) {
    switch (type) {
        case ItemType::COIN:
            return std::make_unique<Coin>(position, world);
        case ItemType::MUSHROOM:
            return std::make_unique<Mushroom>(position, world);
        case ItemType::FIRE_FLOWER:
            return std::make_unique<FireFlower>(position, world);
        case ItemType::STAR:
            return std::make_unique<Star>(position, world);
        default:
            return nullptr;
    }
}

std::unique_ptr<Entity> EntityFactory::createFromTileCode(char tileCode,
                                                          const sf::Vector2f& position,
                                                          b2World* world,
                                                          LevelTheme theme) {
    switch (tileCode) {
        case 'G':
            return createEnemy(EnemyType::GOOMBA, position, world, theme);
        case 'K':
            return createEnemy(EnemyType::KOOPA, position, world, theme);
        case 'p':
        case 'r':
            return createEnemy(EnemyType::PIRANHA_PLANT, position, world, theme);
        case 'C':
            return createItem(ItemType::COIN, position, world);
        case '?':
            // Normal '?' blocks resolve once on hit: mostly Coin, otherwise a
            // random Mushroom or FireFlower.
            return std::make_unique<QuestionBlock>(position,
                                                   world,
                                                   QuestionBlockContent::ADAPTIVE,
                                                   toBlockTheme(theme));
        case 'f':
        case 'h':
            // Explicit flower routes always spawn a FireFlower. Mario keeps
            // the current Small/Super body tier when collecting it.
            return std::make_unique<QuestionBlock>(position,
                                                   world,
                                                   QuestionBlockContent::FIRE_FLOWER,
                                                   toBlockTheme(theme));
        case 'U':
        case 'u':
            return std::make_unique<QuestionBlock>(position,
                                                   world,
                                                   QuestionBlockContent::ONEUP_MUSHROOM,
                                                   toBlockTheme(theme));
        case 'O':
        case 'o':
            return std::make_unique<QuestionBlock>(position,
                                                   world,
                                                   QuestionBlockContent::STAR,
                                                   toBlockTheme(theme));
        case 'J':
        case 'S':
            {
                auto springboard = std::make_unique<Springboard>(position, theme);
                springboard->initPhysics(world, b2_staticBody, {32.f, 32.f});
                return springboard;
            }
        default:
            return nullptr;
    }
}

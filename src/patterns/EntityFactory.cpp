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
#include "entities/CheepCheep.h"       // TV4 (Cheep Cheep)
#include "entities/BuzzyBeetle.h"     // Enemy expansion
#include "entities/RedKoopa.h"        // Enemy expansion
#include "entities/Paratroopa.h"      // Enemy expansion
#include "entities/Blooper.h"         // Enemy expansion
#include "entities/Podoboo.h"         // Enemy expansion
#include "entities/BulletBill.h"      // Enemy expansion
#include "entities/BulletBillLauncher.h" // Enemy expansion
#include "entities/Lakitu.h"          // Enemy expansion
#include "entities/SpinyEgg.h"        // Enemy expansion
#include "entities/Spiny.h"           // Enemy expansion
#include "entities/HammerBro.h"       // Enemy expansion
#include "entities/Bowser.h"          // Enemy expansion
#include "entities/BowserAxe.h"       // Enemy expansion
#include "entities/Firebar.h"         // Enemy expansion
#include "items/Coin.h"              // TV5 (Sprint 4)
#include "items/Mushroom.h"         // TV5 (Sprint 5)
#include "items/FireFlower.h"       // TV5 (Sprint 5)
#include "items/Star.h"             // TV5 (Sprint 5)
#include "entities/Springboard.h"     // TV1 (Sprint 7)
#include "entities/QuestionBlock.h"    // TV5 (Sprint 4)
#include "entities/Toad.h"

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
        case EnemyType::CHEEP_CHEEP:
            return std::make_unique<CheepCheep>(position, world, theme,
                                                CheepCheepBehavior::SWIMMING,
                                                CheepCheepColor::GREEN);
        case EnemyType::BUZZY_BEETLE:
            return std::make_unique<BuzzyBeetle>(position, world, theme);
        case EnemyType::RED_KOOPA:
            return std::make_unique<RedKoopa>(position, world, theme);
        case EnemyType::PARATROOPA_HOP:
            return std::make_unique<Paratroopa>(position, world, theme,
                                                ParatroopaMode::HOP);
        case EnemyType::PARATROOPA_FLY:
            return std::make_unique<Paratroopa>(position, world, theme,
                                                ParatroopaMode::FLY_VERTICAL);
        case EnemyType::PIRANHA_PLANT_RED:
            return std::make_unique<PiranhaPlant>(position, world, theme,
                                                  PiranhaPlant::Color::RED);
        case EnemyType::BLOOPER:
            return std::make_unique<Blooper>(position, world, theme);
        case EnemyType::PODOBOO:
            return std::make_unique<Podoboo>(position, world, theme);
        case EnemyType::BULLET_BILL:
            return std::make_unique<BulletBill>(position, world, theme,
                                                Direction::LEFT);
        case EnemyType::LAKITU:
            return std::make_unique<Lakitu>(position, world, theme);
        case EnemyType::SPINY:
            return std::make_unique<Spiny>(position, world, theme);
        case EnemyType::HAMMER_BRO:
            return std::make_unique<HammerBro>(position, world, theme);
        case EnemyType::BOWSER:
            return std::make_unique<Bowser>(position, world, theme);
        case EnemyType::FIREBAR:
            return std::make_unique<Firebar>(position, world, theme);
        default:
            return nullptr;
    }
}

std::unique_ptr<Entity> EntityFactory::createItem(ItemType type,
                                                   const sf::Vector2f& position,
                                                   b2World* world,
                                                   LevelTheme theme) {
    switch (type) {
        case ItemType::COIN:
            return std::make_unique<Coin>(position, world, CoinType::COLLECTIBLE, theme);
        case ItemType::MUSHROOM:
            return std::make_unique<Mushroom>(position, world, MushroomType::SUPER, theme);
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
        case 'c':
            return std::make_unique<CheepCheep>(position, world, theme,
                                                CheepCheepBehavior::SWIMMING,
                                                CheepCheepColor::GREEN);
        case 'b':
            return createEnemy(EnemyType::BUZZY_BEETLE, position, world, theme);
        case 'k':
            return createEnemy(EnemyType::RED_KOOPA, position, world, theme);
        case 'y':
            return createEnemy(EnemyType::PARATROOPA_HOP, position, world, theme);
        case 'd':
            return createEnemy(EnemyType::PARATROOPA_FLY, position, world, theme);
        case 'q':
            return createEnemy(EnemyType::PIRANHA_PLANT_RED, position, world, theme);
        case 'l':
            return createEnemy(EnemyType::BLOOPER, position, world, theme);
        case 'P':
            return createEnemy(EnemyType::PODOBOO, position, world, theme);
        case 't':
            return createEnemy(EnemyType::LAKITU, position, world, theme);
        case 's':
            return createEnemy(EnemyType::SPINY, position, world, theme);
        case 'n':
            return createEnemy(EnemyType::HAMMER_BRO, position, world, theme);
        case 'X':
            return createEnemy(EnemyType::BOWSER, position, world, theme);
        case 'e':
        case 'E':
            return createEnemy(EnemyType::FIREBAR, position, world, theme);
        case 'A':
            return std::make_unique<BowserAxe>(position, theme);
        case 'N':
            return std::make_unique<Toad>(position);
        case 'D':
            return std::make_unique<BulletBillLauncher>(position, world, theme);
        case 'C':
            return createItem(ItemType::COIN, position, world, theme);
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

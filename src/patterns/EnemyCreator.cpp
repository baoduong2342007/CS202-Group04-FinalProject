/**
 * @file EnemyCreator.cpp
 * @brief EnemyType-to-concrete-entity Factory Method mappings.
 */

#include "patterns/EnemyCreator.h"

#include "entities/Blooper.h"
#include "entities/Bowser.h"
#include "entities/BulletBill.h"
#include "entities/BuzzyBeetle.h"
#include "entities/CheepCheep.h"
#include "entities/Firebar.h"
#include "entities/Goomba.h"
#include "entities/HammerBro.h"
#include "entities/Koopa.h"
#include "entities/Lakitu.h"
#include "entities/Paratroopa.h"
#include "entities/PiranhaPlant.h"
#include "entities/Podoboo.h"
#include "entities/RedKoopa.h"
#include "entities/Spiny.h"

std::unique_ptr<Entity> EnemyCreator::create(const SpawnRequest& request,
                                             const SpawnContext& context) const {
    const auto type = request.enemyType();
    if (!type) {
        return nullptr;
    }

    const sf::Vector2f& position = request.position;
    b2World* const world = context.world;
    const LevelTheme theme = context.theme;

    switch (*type) {
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

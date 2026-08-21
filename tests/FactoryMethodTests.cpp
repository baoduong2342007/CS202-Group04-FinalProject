/**
 * @file FactoryMethodTests.cpp
 * @brief Contract tests for the canonical Factory Method entity creation seam.
 */

#include <cassert>
#include <iostream>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <utility>

#include <box2d/box2d.h>

#include "entities/Blooper.h"
#include "entities/Bowser.h"
#include "entities/BowserAxe.h"
#include "entities/BulletBill.h"
#include "entities/BulletBillLauncher.h"
#include "entities/BuzzyBeetle.h"
#include "entities/CheepCheep.h"
#include "entities/Firebar.h"
#include "entities/Goomba.h"
#include "entities/HammerBro.h"
#include "entities/Lakitu.h"
#include "entities/Paratroopa.h"
#include "entities/PiranhaPlant.h"
#include "entities/Podoboo.h"
#include "entities/QuestionBlock.h"
#include "entities/RedKoopa.h"
#include "entities/Spiny.h"
#include "entities/Springboard.h"
#include "entities/Toad.h"
#include "items/Coin.h"
#include "items/FireFlower.h"
#include "items/Mushroom.h"
#include "items/Star.h"
#include "patterns/EnemyCreator.h"
#include "patterns/EntityFactory.h"
#include "patterns/ItemCreator.h"
#include "patterns/SpawnContext.h"
#include "patterns/SpawnRequest.h"
#include "patterns/WorldObjectCreator.h"

namespace {

constexpr sf::Vector2f POSITION{96.f, 128.f};

using CanonicalFactoryResult = decltype(
    std::declval<const EntityFactory&>().create(
        std::declval<const SpawnRequest&>(),
        std::declval<const SpawnContext&>()));
static_assert(std::is_same_v<CanonicalFactoryResult, std::unique_ptr<Entity>>);

void testCanonicalEnemyAndItemRequests() {
    std::cout << "[RUNNING] testCanonicalEnemyAndItemRequests..." << std::endl;

    b2World world(b2Vec2(0.f, 25.f));
    const SpawnContext context{&world, LevelTheme::UNDERGROUND};
    const EntityFactory factory;

    struct EnemyCase {
        EnemyType type;
        bool (Entity::*predicate)() const;
        bool hasPhysics;
    };
    const EnemyCase enemies[] = {
        {EnemyType::GOOMBA, &Entity::isEnemy, true},
        {EnemyType::KOOPA, &Entity::isKoopa, true},
        {EnemyType::PIRANHA_PLANT, &Entity::isPiranhaPlant, true},
        {EnemyType::CHEEP_CHEEP, &Entity::isCheepCheep, true},
        {EnemyType::BUZZY_BEETLE, &Entity::isBuzzyBeetle, true},
        {EnemyType::RED_KOOPA, &Entity::isRedKoopa, true},
        {EnemyType::PARATROOPA_HOP, &Entity::isParatroopa, true},
        {EnemyType::PARATROOPA_FLY, &Entity::isParatroopa, true},
        {EnemyType::PIRANHA_PLANT_RED, &Entity::isPiranhaPlant, true},
        {EnemyType::BLOOPER, &Entity::isBlooper, true},
        {EnemyType::PODOBOO, &Entity::isPodoboo, true},
        {EnemyType::BULLET_BILL, &Entity::isBulletBill, true},
        {EnemyType::LAKITU, &Entity::isLakitu, true},
        {EnemyType::SPINY, &Entity::isSpiny, true},
        {EnemyType::HAMMER_BRO, &Entity::isHammerBro, true},
        {EnemyType::BOWSER, &Entity::isBowser, true},
        // Firebar is an intentional body-less rotating hazard; its `world`
        // input is retained for compatibility but is not used by the class.
        {EnemyType::FIREBAR, &Entity::isFirebar, false},
    };

    for (const auto& test : enemies) {
        auto entity = factory.create(SpawnRequest::enemy(test.type, POSITION), context);
        assert(entity != nullptr);
        assert(entity->getType() == Entity::EntityType::ENEMY);
        assert((entity.get()->*test.predicate)());
        assert(entity->getBody() != nullptr || !test.hasPhysics);

        switch (test.type) {
            case EnemyType::GOOMBA:
                assert(dynamic_cast<Goomba*>(entity.get()) != nullptr);
                break;
            case EnemyType::KOOPA:
                assert(dynamic_cast<Koopa*>(entity.get()) != nullptr);
                break;
            case EnemyType::PIRANHA_PLANT:
            case EnemyType::PIRANHA_PLANT_RED:
                assert(dynamic_cast<PiranhaPlant*>(entity.get()) != nullptr);
                break;
            case EnemyType::CHEEP_CHEEP:
                assert(dynamic_cast<CheepCheep*>(entity.get()) != nullptr);
                break;
            case EnemyType::BUZZY_BEETLE:
                assert(dynamic_cast<BuzzyBeetle*>(entity.get()) != nullptr);
                break;
            case EnemyType::RED_KOOPA:
                assert(dynamic_cast<RedKoopa*>(entity.get()) != nullptr);
                break;
            case EnemyType::PARATROOPA_HOP:
            case EnemyType::PARATROOPA_FLY:
                assert(dynamic_cast<Paratroopa*>(entity.get()) != nullptr);
                break;
            case EnemyType::BLOOPER:
                assert(dynamic_cast<Blooper*>(entity.get()) != nullptr);
                break;
            case EnemyType::PODOBOO:
                assert(dynamic_cast<Podoboo*>(entity.get()) != nullptr);
                break;
            case EnemyType::BULLET_BILL:
                assert(dynamic_cast<BulletBill*>(entity.get()) != nullptr);
                break;
            case EnemyType::LAKITU:
                assert(dynamic_cast<Lakitu*>(entity.get()) != nullptr);
                break;
            case EnemyType::SPINY:
                assert(dynamic_cast<Spiny*>(entity.get()) != nullptr);
                break;
            case EnemyType::HAMMER_BRO:
                assert(dynamic_cast<HammerBro*>(entity.get()) != nullptr);
                break;
            case EnemyType::BOWSER:
                assert(dynamic_cast<Bowser*>(entity.get()) != nullptr);
                break;
            case EnemyType::FIREBAR:
                assert(dynamic_cast<Firebar*>(entity.get()) != nullptr);
                break;
        }
    }

    struct ItemCase {
        ItemType type;
        bool (Entity::*predicate)() const;
    };
    const ItemCase items[] = {
        {ItemType::COIN, &Entity::isItem},
        {ItemType::MUSHROOM, &Entity::isMushroom},
        {ItemType::FIRE_FLOWER, &Entity::isItem},
        {ItemType::STAR, &Entity::isStar},
    };

    for (const auto& test : items) {
        auto entity = factory.create(SpawnRequest::item(test.type, POSITION), context);
        assert(entity != nullptr);
        assert(entity->getType() == Entity::EntityType::ITEM);
        assert((entity.get()->*test.predicate)());
        assert(entity->getBody() != nullptr);

        switch (test.type) {
            case ItemType::COIN:
                assert(dynamic_cast<Coin*>(entity.get()) != nullptr);
                break;
            case ItemType::MUSHROOM:
                assert(dynamic_cast<Mushroom*>(entity.get()) != nullptr);
                assert(static_cast<Mushroom*>(entity.get())->getTheme() ==
                       LevelTheme::UNDERGROUND);
                break;
            case ItemType::FIRE_FLOWER:
                assert(dynamic_cast<FireFlower*>(entity.get()) != nullptr);
                break;
            case ItemType::STAR:
                assert(dynamic_cast<Star*>(entity.get()) != nullptr);
                break;
        }
    }

    std::cout << "[PASSED] testCanonicalEnemyAndItemRequests" << std::endl;
}

void testCreatorCategoryAndNullWorldContracts() {
    std::cout << "[RUNNING] testCreatorCategoryAndNullWorldContracts..." << std::endl;

    const SpawnContext nullContext{nullptr, LevelTheme::CASTLE};
    const EnemyCreator enemyCreator;
    const ItemCreator itemCreator;
    const WorldObjectCreator objectCreator;

    assert(enemyCreator.create(SpawnRequest::item(ItemType::COIN, POSITION), nullContext) == nullptr);
    assert(enemyCreator.create(SpawnRequest::tile('G', POSITION), nullContext) == nullptr);
    assert(itemCreator.create(SpawnRequest::enemy(EnemyType::GOOMBA, POSITION), nullContext) == nullptr);
    assert(itemCreator.create(SpawnRequest::tile('C', POSITION), nullContext) == nullptr);
    assert(objectCreator.create(SpawnRequest::enemy(EnemyType::GOOMBA, POSITION), nullContext) == nullptr);
    assert(objectCreator.create(SpawnRequest::item(ItemType::COIN, POSITION), nullContext) == nullptr);

    const EntityFactory factory;
    auto enemy = factory.create(SpawnRequest::enemy(EnemyType::GOOMBA, POSITION), nullContext);
    auto item = factory.create(SpawnRequest::item(ItemType::COIN, POSITION), nullContext);
    assert(enemy != nullptr);
    assert(item != nullptr);
    assert(enemy->getBody() == nullptr);
    assert(item->getBody() == nullptr);

    auto axe = factory.create(SpawnRequest::tile('A', POSITION), nullContext);
    auto toad = factory.create(SpawnRequest::tile('N', POSITION), nullContext);
    auto spring = factory.create(SpawnRequest::tile('J', POSITION), nullContext);
    assert(axe != nullptr && axe->isBowserAxe() && axe->getBody() == nullptr);
    assert(toad != nullptr && toad->isToad() && toad->getBody() == nullptr);
    assert(spring != nullptr && spring->isSpringboard() && spring->getBody() == nullptr);

    std::cout << "[PASSED] testCreatorCategoryAndNullWorldContracts" << std::endl;
}

void testTileCodeMappingAndShimParity() {
    std::cout << "[RUNNING] testTileCodeMappingAndShimParity..." << std::endl;

    b2World world(b2Vec2(0.f, 25.f));
    const SpawnContext context{&world, LevelTheme::CASTLE};
    const EntityFactory factory;

    struct TileCase {
        char code;
        bool (Entity::*predicate)() const;
    };
    const TileCase tiles[] = {
        {'G', &Entity::isEnemy}, {'K', &Entity::isKoopa},
        {'p', &Entity::isPiranhaPlant}, {'r', &Entity::isPiranhaPlant},
        {'c', &Entity::isCheepCheep}, {'b', &Entity::isBuzzyBeetle},
        {'k', &Entity::isRedKoopa}, {'y', &Entity::isParatroopa},
        {'d', &Entity::isParatroopa}, {'q', &Entity::isPiranhaPlant},
        {'l', &Entity::isBlooper}, {'P', &Entity::isPodoboo},
        {'t', &Entity::isLakitu}, {'s', &Entity::isSpiny},
        {'n', &Entity::isHammerBro}, {'X', &Entity::isBowser},
        {'e', &Entity::isFirebar}, {'E', &Entity::isFirebar},
        {'A', &Entity::isBowserAxe}, {'N', &Entity::isToad},
        {'D', &Entity::isBulletBillLauncher}, {'C', &Entity::isItem},
        {'?', &Entity::isQuestionBlock}, {'f', &Entity::isQuestionBlock},
        {'h', &Entity::isQuestionBlock}, {'U', &Entity::isQuestionBlock},
        {'u', &Entity::isQuestionBlock}, {'O', &Entity::isQuestionBlock},
        {'o', &Entity::isQuestionBlock}, {'J', &Entity::isSpringboard},
        {'S', &Entity::isSpringboard},
    };

    for (const auto& test : tiles) {
        auto canonical = factory.create(SpawnRequest::tile(test.code, POSITION), context);
        assert(canonical != nullptr);
        assert((canonical.get()->*test.predicate)());

        // The predicate checks the public entity contract; these additional
        // observations pin the variant-specific constructor arguments that
        // used to be easy to lose while moving the tile switch.
        switch (test.code) {
            case 'p':
            case 'r':
                assert(static_cast<PiranhaPlant*>(canonical.get())->getColor() ==
                       PiranhaPlant::Color::GREEN);
                break;
            case 'q':
                assert(static_cast<PiranhaPlant*>(canonical.get())->getColor() ==
                       PiranhaPlant::Color::RED);
                break;
            case 'c':
                assert(static_cast<CheepCheep*>(canonical.get())->getBehavior() ==
                       CheepCheepBehavior::SWIMMING);
                assert(static_cast<CheepCheep*>(canonical.get())->getColor() ==
                       CheepCheepColor::GREEN);
                break;
            case 'y':
                assert(static_cast<Paratroopa*>(canonical.get())->getMode() ==
                       ParatroopaMode::HOP);
                break;
            case 'd':
                assert(static_cast<Paratroopa*>(canonical.get())->getMode() ==
                       ParatroopaMode::FLY_VERTICAL);
                break;
            case 'e':
            case 'E':
                assert(static_cast<Firebar*>(canonical.get())->getBallCount() == 5);
                assert(canonical->getBody() == nullptr);
                break;
            case '?':
                assert(static_cast<QuestionBlock*>(canonical.get())->getContent() ==
                       QuestionBlockContent::ADAPTIVE);
                assert(static_cast<QuestionBlock*>(canonical.get())->getTheme() ==
                       BlockTheme::CASTLE);
                break;
            case 'f':
            case 'h':
                assert(static_cast<QuestionBlock*>(canonical.get())->getContent() ==
                       QuestionBlockContent::FIRE_FLOWER);
                break;
            case 'U':
            case 'u':
                assert(static_cast<QuestionBlock*>(canonical.get())->getContent() ==
                       QuestionBlockContent::ONEUP_MUSHROOM);
                break;
            case 'O':
            case 'o':
                assert(static_cast<QuestionBlock*>(canonical.get())->getContent() ==
                       QuestionBlockContent::STAR);
                break;
            case 'C':
                assert(static_cast<Coin*>(canonical.get())->getCoinType() ==
                       CoinType::COLLECTIBLE);
                assert(static_cast<Coin*>(canonical.get())->getTheme() ==
                       LevelTheme::CASTLE);
                assert(canonical->getBody() != nullptr);
                break;
            case 'D':
                assert(canonical->getBody() == nullptr);
                break;
            case 'J':
            case 'S':
                assert(canonical->getBody() != nullptr);
                assert(canonical->getBody()->GetType() == b2_staticBody);
                break;
            default:
                break;
        }

        auto shim = EntityFactory::createFromTileCode(test.code, POSITION, &world,
                                                       LevelTheme::CASTLE);
        assert(shim != nullptr);
        assert(typeid(*shim) == typeid(*canonical));
        assert(shim->getType() == canonical->getType());
        assert((shim.get()->*test.predicate)());
    }

    assert(factory.create(SpawnRequest::tile('?', POSITION),
                          SpawnContext{&world, LevelTheme::UNDERGROUND})
               ->getType() == Entity::EntityType::TERRAIN);
    auto unsupported = factory.create(SpawnRequest::tile('@', POSITION), context);
    assert(unsupported == nullptr);
    assert(factory.create(
               SpawnRequest::enemy(static_cast<EnemyType>(999), POSITION), context) == nullptr);
    assert(factory.create(
               SpawnRequest::item(static_cast<ItemType>(999), POSITION), context) == nullptr);
    assert(factory.create(SpawnRequest{}, context) == nullptr);
    assert(EntityFactory::createFromTileCode('@', POSITION, &world, LevelTheme::CASTLE) == nullptr);

    auto enemyShim = EntityFactory::createEnemy(EnemyType::PARATROOPA_FLY, POSITION,
                                                &world, LevelTheme::CASTLE);
    auto enemyCanonical = factory.create(
        SpawnRequest::enemy(EnemyType::PARATROOPA_FLY, POSITION), context);
    assert(enemyShim != nullptr && enemyCanonical != nullptr);
    assert(enemyShim->isParatroopa() && enemyCanonical->isParatroopa());
    assert(static_cast<Paratroopa*>(enemyShim.get())->getMode() ==
           static_cast<Paratroopa*>(enemyCanonical.get())->getMode());

    auto itemShim = EntityFactory::createItem(ItemType::MUSHROOM, POSITION,
                                              &world, LevelTheme::CASTLE);
    auto itemCanonical = factory.create(SpawnRequest::item(ItemType::MUSHROOM, POSITION),
                                        context);
    assert(itemShim != nullptr && itemCanonical != nullptr);
    assert(itemShim->isMushroom() && itemCanonical->isMushroom());

    std::cout << "[PASSED] testTileCodeMappingAndShimParity" << std::endl;
}

} // namespace

int main() {
    std::cout << "=== Running Factory Method Tests ===" << std::endl;
    testCanonicalEnemyAndItemRequests();
    testCreatorCategoryAndNullWorldContracts();
    testTileCodeMappingAndShimParity();
    std::cout << "All Factory Method tests PASSED successfully!" << std::endl;
    return 0;
}

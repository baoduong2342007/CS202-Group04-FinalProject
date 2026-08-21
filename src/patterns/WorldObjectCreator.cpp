/**
 * @file WorldObjectCreator.cpp
 * @brief Tile-code-to-entity Factory Method mappings.
 */

#include "patterns/WorldObjectCreator.h"

#include "entities/BowserAxe.h"
#include "entities/BulletBillLauncher.h"
#include "entities/QuestionBlock.h"
#include "entities/Springboard.h"
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

std::unique_ptr<Entity> WorldObjectCreator::create(const SpawnRequest& request,
                                                   const SpawnContext& context) const {
    const auto tile = request.tileCode();
    if (!tile) {
        return nullptr;
    }

    const sf::Vector2f& position = request.position;
    b2World* const world = context.world;
    const LevelTheme theme = context.theme;

    switch (*tile) {
        case 'G':
            return m_enemyCreator.create(SpawnRequest::enemy(EnemyType::GOOMBA, position), context);
        case 'K':
            return m_enemyCreator.create(SpawnRequest::enemy(EnemyType::KOOPA, position), context);
        case 'p':
        case 'r':
            return m_enemyCreator.create(SpawnRequest::enemy(EnemyType::PIRANHA_PLANT, position), context);
        case 'c':
            return m_enemyCreator.create(SpawnRequest::enemy(EnemyType::CHEEP_CHEEP, position), context);
        case 'b':
            return m_enemyCreator.create(SpawnRequest::enemy(EnemyType::BUZZY_BEETLE, position), context);
        case 'k':
            return m_enemyCreator.create(SpawnRequest::enemy(EnemyType::RED_KOOPA, position), context);
        case 'y':
            return m_enemyCreator.create(SpawnRequest::enemy(EnemyType::PARATROOPA_HOP, position), context);
        case 'd':
            return m_enemyCreator.create(SpawnRequest::enemy(EnemyType::PARATROOPA_FLY, position), context);
        case 'q':
            return m_enemyCreator.create(SpawnRequest::enemy(EnemyType::PIRANHA_PLANT_RED, position), context);
        case 'l':
            return m_enemyCreator.create(SpawnRequest::enemy(EnemyType::BLOOPER, position), context);
        case 'P':
            return m_enemyCreator.create(SpawnRequest::enemy(EnemyType::PODOBOO, position), context);
        case 't':
            return m_enemyCreator.create(SpawnRequest::enemy(EnemyType::LAKITU, position), context);
        case 's':
            return m_enemyCreator.create(SpawnRequest::enemy(EnemyType::SPINY, position), context);
        case 'n':
            return m_enemyCreator.create(SpawnRequest::enemy(EnemyType::HAMMER_BRO, position), context);
        case 'X':
            return m_enemyCreator.create(SpawnRequest::enemy(EnemyType::BOWSER, position), context);
        case 'e':
        case 'E':
            return m_enemyCreator.create(SpawnRequest::enemy(EnemyType::FIREBAR, position), context);
        case 'A':
            return std::make_unique<BowserAxe>(position, theme);
        case 'N':
            return std::make_unique<Toad>(position);
        case 'D':
            return std::make_unique<BulletBillLauncher>(position, world, theme);
        case 'C':
            return m_itemCreator.create(SpawnRequest::item(ItemType::COIN, position), context);
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

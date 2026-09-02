/**
 * @file EntityFactory.cpp
 * @brief Factory Method orchestration and compatibility forwarding.
 */

#include "patterns/EntityFactory.h"

#include <type_traits>

namespace {

EntityFactory& defaultFactory() {
    static EntityFactory factory;
    return factory;
}

} // namespace

std::unique_ptr<Entity> EntityFactory::create(const SpawnRequest& request,
                                              const SpawnContext& context) const {
    return std::visit(
        [this, &request, &context](const auto& payload) -> std::unique_ptr<Entity> {
            using Payload = std::decay_t<decltype(payload)>;
            if constexpr (std::is_same_v<Payload, EnemyType>) {
                return m_enemyCreator.create(request, context);
            } else if constexpr (std::is_same_v<Payload, ItemType>) {
                return m_itemCreator.create(request, context);
            } else if constexpr (std::is_same_v<Payload, char>) {
                return m_worldObjectCreator.create(request, context);
            } else {
                return nullptr;
            }
        },
        request.payload);
}

std::unique_ptr<Entity> EntityFactory::createEnemy(EnemyType type,
                                                   const sf::Vector2f& position,
                                                   b2World* world,
                                                   LevelTheme theme) {
    return defaultFactory().create(SpawnRequest::enemy(type, position),
                                   SpawnContext{world, theme});
}

std::unique_ptr<Entity> EntityFactory::createItem(ItemType type,
                                                  const sf::Vector2f& position,
                                                  b2World* world,
                                                  LevelTheme theme) {
    return defaultFactory().create(SpawnRequest::item(type, position),
                                   SpawnContext{world, theme});
}

std::unique_ptr<Entity> EntityFactory::createFromTileCode(char tileCode,
                                                          const sf::Vector2f& position,
                                                          b2World* world,
                                                          LevelTheme theme) {
    return defaultFactory().create(SpawnRequest::tile(tileCode, position),
                                   SpawnContext{world, theme});
}

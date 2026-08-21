/**
 * @file ItemCreator.cpp
 * @brief ItemType-to-concrete-entity Factory Method mappings.
 */

#include "patterns/ItemCreator.h"

#include "items/Coin.h"
#include "items/FireFlower.h"
#include "items/Mushroom.h"
#include "items/Star.h"

std::unique_ptr<Entity> ItemCreator::create(const SpawnRequest& request,
                                            const SpawnContext& context) const {
    const auto type = request.itemType();
    if (!type) {
        return nullptr;
    }

    const sf::Vector2f& position = request.position;
    b2World* const world = context.world;
    const LevelTheme theme = context.theme;

    switch (*type) {
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

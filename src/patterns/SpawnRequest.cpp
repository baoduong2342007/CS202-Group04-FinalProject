/**
 * @file SpawnRequest.cpp
 * @brief SpawnRequest value helpers.
 */

#include "patterns/SpawnRequest.h"

#include <utility>

SpawnRequest::SpawnRequest()
    : position{}, payload('\0') {}

SpawnRequest::SpawnRequest(Payload requestPayload,
                           const sf::Vector2f& spawnPosition)
    : position(spawnPosition), payload(std::move(requestPayload)) {}

SpawnRequest::SpawnRequest(EnemyType type, const sf::Vector2f& spawnPosition)
    : position(spawnPosition), payload(type) {}

SpawnRequest::SpawnRequest(ItemType type, const sf::Vector2f& spawnPosition)
    : position(spawnPosition), payload(type) {}

SpawnRequest::SpawnRequest(char tile, const sf::Vector2f& spawnPosition)
    : position(spawnPosition), payload(tile) {}

SpawnRequest::SpawnRequest(const sf::Vector2f& spawnPosition, EnemyType type)
    : SpawnRequest(type, spawnPosition) {}

SpawnRequest::SpawnRequest(const sf::Vector2f& spawnPosition, ItemType type)
    : SpawnRequest(type, spawnPosition) {}

SpawnRequest::SpawnRequest(const sf::Vector2f& spawnPosition, char tile)
    : SpawnRequest(tile, spawnPosition) {}

SpawnRequest::SpawnRequest(const sf::Vector2f& spawnPosition,
                           Payload requestPayload)
    : SpawnRequest(std::move(requestPayload), spawnPosition) {}

SpawnRequest SpawnRequest::enemy(EnemyType type, const sf::Vector2f& position) {
    return SpawnRequest(type, position);
}

SpawnRequest SpawnRequest::item(ItemType type, const sf::Vector2f& position) {
    return SpawnRequest(type, position);
}

SpawnRequest SpawnRequest::tile(char tileCode, const sf::Vector2f& position) {
    return SpawnRequest(tileCode, position);
}

SpawnRequest SpawnRequest::fromEnemy(EnemyType type, const sf::Vector2f& position) {
    return enemy(type, position);
}

SpawnRequest SpawnRequest::fromItem(ItemType type, const sf::Vector2f& position) {
    return item(type, position);
}

SpawnRequest SpawnRequest::fromTileCode(char tileCode, const sf::Vector2f& position) {
    return tile(tileCode, position);
}

bool SpawnRequest::isEnemy() const noexcept {
    return std::holds_alternative<EnemyType>(payload);
}

bool SpawnRequest::isItem() const noexcept {
    return std::holds_alternative<ItemType>(payload);
}

bool SpawnRequest::isTileCode() const noexcept {
    return std::holds_alternative<char>(payload);
}

SpawnKind SpawnRequest::kind() const noexcept {
    if (isEnemy()) {
        return SpawnKind::ENEMY;
    }
    if (isItem()) {
        return SpawnKind::ITEM;
    }
    return SpawnKind::TILE_CODE;
}

std::optional<EnemyType> SpawnRequest::enemyType() const noexcept {
    if (!isEnemy()) {
        return std::nullopt;
    }
    return std::get<EnemyType>(payload);
}

std::optional<ItemType> SpawnRequest::itemType() const noexcept {
    if (!isItem()) {
        return std::nullopt;
    }
    return std::get<ItemType>(payload);
}

std::optional<char> SpawnRequest::tileCode() const noexcept {
    if (!isTileCode()) {
        return std::nullopt;
    }
    return std::get<char>(payload);
}

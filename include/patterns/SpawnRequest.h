/**
 * @file SpawnRequest.h
 * @brief Closed, typed value request consumed by the entity factory method.
 */

#pragma once

#include <optional>
#include <variant>

#include <SFML/System/Vector2.hpp>

#include "core/LevelCatalog.h"

/// Enemy identifiers accepted by the canonical factory method.
enum class EnemyType {
    GOOMBA,
    KOOPA,
    PIRANHA_PLANT,
    CHEEP_CHEEP,
    BUZZY_BEETLE,
    RED_KOOPA,
    PARATROOPA_HOP,
    PARATROOPA_FLY,
    PIRANHA_PLANT_RED,
    BLOOPER,
    PODOBOO,
    BULLET_BILL,
    LAKITU,
    SPINY,
    HAMMER_BRO,
    BOWSER,
    FIREBAR
};

/// Item identifiers accepted by the canonical factory method.
enum class ItemType {
    COIN,
    MUSHROOM,
    FIRE_FLOWER,
    STAR
};

enum class SpawnKind {
    ENEMY,
    ITEM,
    TILE_CODE
};

/**
 * A spawn request has exactly one discriminated payload: an enemy type, an
 * item type, or a tile code.  Keeping the alternatives in a variant prevents
 * contradictory requests such as an enemy and item type being set together.
 */
struct SpawnRequest {
    using Payload = std::variant<EnemyType, ItemType, char>;
    using Kind = SpawnKind;

    sf::Vector2f position{};
    Payload payload{};

    /// An empty request is represented by an unsupported NUL tile code.
    SpawnRequest();
    SpawnRequest(Payload payload, const sf::Vector2f& position);
    SpawnRequest(EnemyType type, const sf::Vector2f& position);
    SpawnRequest(ItemType type, const sf::Vector2f& position);
    SpawnRequest(char tileCode, const sf::Vector2f& position);

    // Position-first overloads make aggregate-style call sites read naturally
    // while retaining the enum-first form used by the compatibility shims.
    SpawnRequest(const sf::Vector2f& position, EnemyType type);
    SpawnRequest(const sf::Vector2f& position, ItemType type);
    SpawnRequest(const sf::Vector2f& position, char tileCode);
    SpawnRequest(const sf::Vector2f& position, Payload payload);

    static SpawnRequest enemy(EnemyType type, const sf::Vector2f& position);
    static SpawnRequest item(ItemType type, const sf::Vector2f& position);
    static SpawnRequest tile(char tileCode, const sf::Vector2f& position);

    static SpawnRequest fromEnemy(EnemyType type, const sf::Vector2f& position);
    static SpawnRequest fromItem(ItemType type, const sf::Vector2f& position);
    static SpawnRequest fromTileCode(char tileCode, const sf::Vector2f& position);

    bool isEnemy() const noexcept;
    bool isItem() const noexcept;
    bool isTileCode() const noexcept;
    bool isTile() const noexcept { return isTileCode(); }

    SpawnKind kind() const noexcept;
    SpawnKind getKind() const noexcept { return kind(); }
    const sf::Vector2f& getPosition() const noexcept { return position; }
    const Payload& getPayload() const noexcept { return payload; }

    std::optional<EnemyType> enemyType() const noexcept;
    std::optional<ItemType> itemType() const noexcept;
    std::optional<char> tileCode() const noexcept;
    std::optional<EnemyType> getEnemyType() const noexcept { return enemyType(); }
    std::optional<ItemType> getItemType() const noexcept { return itemType(); }
    std::optional<char> getTileCode() const noexcept { return tileCode(); }
};

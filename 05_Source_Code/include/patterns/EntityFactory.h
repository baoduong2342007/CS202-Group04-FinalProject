/**
 * @file EntityFactory.h
 * @brief Factory Method orchestrator for polymorphic game entities.
 *
 * The non-static create() method is the canonical production seam.  The
 * legacy static helpers remain as migration shims for existing callers.
 */

#pragma once

#include <memory>

#include "entities/Entity.h"
#include "patterns/EnemyCreator.h"
#include "patterns/ItemCreator.h"
#include "patterns/SpawnContext.h"
#include "patterns/SpawnRequest.h"
#include "patterns/WorldObjectCreator.h"

class EntityFactory {
public:
    EntityFactory() = default;
    ~EntityFactory() = default;

    /// Canonical Factory Method entry point. The request payload selects one
    /// creator category; unsupported payloads return nullptr.
    std::unique_ptr<Entity> create(const SpawnRequest& request,
                                   const SpawnContext& context) const;

    // Compatibility shims. Each forwards to the same default orchestrator so
    // there is one mapping authority and no legacy switch duplicate.
    static std::unique_ptr<Entity> createEnemy(EnemyType type,
                                               const sf::Vector2f& position,
                                               b2World* world,
                                               LevelTheme theme = LevelTheme::OVERWORLD);

    static std::unique_ptr<Entity> createItem(ItemType type,
                                              const sf::Vector2f& position,
                                              b2World* world,
                                              LevelTheme theme = LevelTheme::OVERWORLD);

    static std::unique_ptr<Entity> createFromTileCode(char tileCode,
                                                      const sf::Vector2f& position,
                                                      b2World* world,
                                                      LevelTheme theme = LevelTheme::OVERWORLD);

private:
    EnemyCreator m_enemyCreator;
    ItemCreator m_itemCreator;
    WorldObjectCreator m_worldObjectCreator;
};

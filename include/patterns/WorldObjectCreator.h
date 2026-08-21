/**
 * @file WorldObjectCreator.h
 * @brief Factory Method creator for tile-code/world-object requests.
 */

#pragma once

#include "patterns/EntityCreator.h"

#include "patterns/EnemyCreator.h"
#include "patterns/ItemCreator.h"

class WorldObjectCreator : public EntityCreator {
public:
    std::unique_ptr<Entity> create(const SpawnRequest& request,
                                   const SpawnContext& context) const override;

private:
    // These creators are stateless; keeping them as members makes tile-code
    // delegation explicit without duplicating enum mapping authority.
    EnemyCreator m_enemyCreator;
    ItemCreator m_itemCreator;
};

/**
 * @file EnemyCreator.h
 * @brief Factory Method creator for EnemyType requests.
 */

#pragma once

#include "patterns/EntityCreator.h"

class EnemyCreator : public EntityCreator {
public:
    std::unique_ptr<Entity> create(const SpawnRequest& request,
                                   const SpawnContext& context) const override;
};

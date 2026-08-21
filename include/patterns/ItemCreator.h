/**
 * @file ItemCreator.h
 * @brief Factory Method creator for ItemType requests.
 */

#pragma once

#include "patterns/EntityCreator.h"

class ItemCreator : public EntityCreator {
public:
    std::unique_ptr<Entity> create(const SpawnRequest& request,
                                   const SpawnContext& context) const override;
};

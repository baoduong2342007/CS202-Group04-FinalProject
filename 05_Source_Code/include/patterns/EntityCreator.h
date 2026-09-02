/**
 * @file EntityCreator.h
 * @brief Abstract creator seam for Factory Method entity construction.
 */

#pragma once

#include <memory>

#include "entities/Entity.h"
#include "patterns/SpawnContext.h"
#include "patterns/SpawnRequest.h"

class EntityCreator {
public:
    virtual ~EntityCreator();

    /// Factory Method implemented by each creator category.
    virtual std::unique_ptr<Entity> create(const SpawnRequest& request,
                                           const SpawnContext& context) const = 0;
};

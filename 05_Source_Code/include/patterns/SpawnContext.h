/**
 * @file SpawnContext.h
 * @brief Non-owning world/theme inputs for entity creation.
 */

#pragma once

#include <box2d/box2d.h>

#include "core/LevelCatalog.h"

struct SpawnContext {
    b2World* world = nullptr;
    LevelTheme theme = LevelTheme::OVERWORLD;

    constexpr SpawnContext() noexcept = default;
    constexpr SpawnContext(b2World* world,
                           LevelTheme theme = LevelTheme::OVERWORLD) noexcept
        : world(world), theme(theme) {}

    constexpr SpawnContext(LevelTheme theme, b2World* world = nullptr) noexcept
        : world(world), theme(theme) {}

    b2World* getWorld() const noexcept { return world; }
    LevelTheme getTheme() const noexcept { return theme; }
};

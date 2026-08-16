/**
 * @file RedKoopa.h
 * @brief Red-shelled Koopa Troopa variant
 * @note Enemy expansion - palette variant per the team decision: movement,
 *       ledge, and shell behaviour are identical to the green Koopa.
 */

#pragma once

#include "core/LevelCatalog.h"
#include "entities/Koopa.h"

class RedKoopa : public Koopa {
public:
    explicit RedKoopa(const sf::Vector2f& position,
                      b2World* world,
                      LevelTheme theme = LevelTheme::OVERWORLD);
    ~RedKoopa() override = default;

    bool isRedKoopa() const override { return true; }
};

/**
 * @file RedKoopa.h
 * @brief Red-shelled Koopa Troopa variant
 * @note Enemy expansion - palette variant per the team decision: the shell
 *       state machine is identical to the green Koopa, but the patrol is
 *       canonical SMB1: the Red Koopa is ledge-aware and turns at ledges
 *       while the green Koopa walks off them.
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

    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::RED_KOOPA; }

    /// Canonical SMB1 ledge guard: the Red Koopa reverses at ledges and stays
    /// on its platform instead of walking off.
    bool turnsAtLedge() const override { return true; }
};

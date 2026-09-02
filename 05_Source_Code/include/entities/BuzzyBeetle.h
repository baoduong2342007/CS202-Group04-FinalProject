/**
 * @file BuzzyBeetle.h
 * @brief Fireproof shelled walker reusing the Koopa shell state machine
 * @note Enemy expansion - SMB1-accurate: shells block fireballs and the
 *       shell re-emerges without the legs-out warning shake.
 */

#pragma once

#include "core/LevelCatalog.h"
#include "entities/Koopa.h"

class BuzzyBeetle : public Koopa {
public:
    explicit BuzzyBeetle(const sf::Vector2f& position,
                         b2World* world,
                         LevelTheme theme = LevelTheme::OVERWORLD);
    ~BuzzyBeetle() override = default;

    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::BUZZY_BEETLE; }
    Capabilities getCapabilities() const noexcept override {
        return Koopa::getCapabilities() | capability(Capability::FIREPROOF);
    }

protected:
    /// A Buzzy Beetle's shell gives no visual warning before it wakes up.
    bool showsWakeWarning() const override { return false; }
};

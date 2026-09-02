/**
 * @file BowserAxe.h
 * @brief The axe behind Bowser's bridge: touching it collapses the bridge
 * @note Enemy expansion - a body-less pickup entity; Level watches the
 *       overlap with Mario and runs the collapse sequence.
 */

#pragma once

#include "core/LevelCatalog.h"
#include "entities/Entity.h"

class BowserAxe : public Entity {
public:
    BowserAxe(const sf::Vector2f& position, LevelTheme theme);
    ~BowserAxe() override = default;

    void update(float dt) override;

    EntityType getType() const override {
        return EntityType::TERRAIN;
    }

    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::BOWSER_AXE; }

private:
    float m_blinkTimer{0.f};
};

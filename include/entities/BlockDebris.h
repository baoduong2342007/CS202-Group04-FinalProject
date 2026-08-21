/**
 * @file BlockDebris.h
 * @author TV4 & TV1
 * @brief Particle debris effect spawned when Big Mario shatters a Brick Block
 */

#pragma once

#include "entities/Entity.h"

#include "core/SpriteFrames_ovw.h"

class BlockDebris : public Entity {
public:
    BlockDebris(const sf::Vector2f& position, const sf::Vector2f& velocity, const sf::IntRect& frame = SpriteFrames::ovw::Blocks::DEBRIS_TOP_LEFT);
    ~BlockDebris() override = default;

    void update(float dt) override;
    EntityType getType() const override { return EntityType::TERRAIN; }
    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::BLOCK_DEBRIS; }

private:
    sf::Vector2f m_vel;
    float m_rotation = 0.f;
    float m_lifetime = 1.0f; // Seconds before self-removal
};

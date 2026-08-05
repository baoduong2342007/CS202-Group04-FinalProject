/**
 * @file BlockDebris.h
 * @author TV4 & TV1
 * @brief Particle debris effect spawned when Big Mario shatters a Brick Block
 */

#pragma once

#include "entities/Entity.h"

class BlockDebris : public Entity {
public:
    BlockDebris(const sf::Vector2f& position, const sf::Vector2f& velocity);
    ~BlockDebris() override = default;

    void update(float dt) override;

private:
    sf::Vector2f m_vel;
    float m_rotation = 0.f;
    float m_lifetime = 1.0f; // Seconds before self-removal
};

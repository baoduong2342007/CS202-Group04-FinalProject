/**
 * @file FireballExplosion.h
 * @author TV3 & TV1
 * @brief Visual particle effect entity spawned when a FireBall explodes on impact
 * @note Week 7 implementation for Fire Mario fireball pop effect
 */

#pragma once

#include "entities/Entity.h"

class FireballExplosion : public Entity {
public:
    explicit FireballExplosion(const sf::Vector2f& position);
    ~FireballExplosion() override = default;

    void update(float dt) override;
    EntityType getType() const override { return EntityType::TERRAIN; }

private:
    float m_lifetime{0.15f};
};

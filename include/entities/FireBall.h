/**
 * @file FireBall.h
 * @author TV3 (Bảo)
 * @brief Fireball projectile entity for Mario FIRE power-up state
 * @note Week 7 implementation
 */

#pragma once

#include "entities/Entity.h"
#include "entities/Character.h" // For Direction enum

class FireBall : public Entity {
public:
    // 1. Constructor / Destructor
    FireBall();
    FireBall(const sf::Vector2f& position, Direction direction);
    ~FireBall() override = default;

    // 2. Override methods
    void update(float dt) override;

    // 3. Public methods
    void bounce(const sf::Vector2f& surfaceNormal = sf::Vector2f(0.f, -1.f));
    void deactivate();

    // 4. Getters / Setters
    int getBounceCount() const;
    Direction getDirection() const;

private:
    // 5. Private members
    Direction m_direction;
    int m_bounceCount;
    float m_lifetime;
};

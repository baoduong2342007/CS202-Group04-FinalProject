/**
 * @file Star.h
 * @author TV5 (Truyền)
 * @brief Starman — bouncing item that grants temporary invincibility
 * @note Week 5 — spawned by EntityFactory; publishes PLAYER_POWER_UP on collect.
 *       Invincibility is time-limited; the item tracks its own spawn lifetime.
 */

#pragma once

#include "items/Item.h"

class Star : public Item {
public:
    // 1. Constructor / Destructor
    Star();
    Star(const sf::Vector2f& position, b2World* world);
    ~Star() override = default;

    // 2. Override methods
    void update(float dt) override;
    void onCollect(Mario& mario) override;

    // 3. Public methods
    /// Reverse horizontal direction when hitting a wall
    void onWallCollision();
    /// Apply upward bounce when hitting the ground
    void onGroundCollision();

private:
    // 5. Private methods
    void patrol();

    // 6. Private members
    float m_patrolSpeed;
    float m_bounceVelocity;
    /// +1 = moving right, -1 = moving left
    int m_patrolDirection;
};

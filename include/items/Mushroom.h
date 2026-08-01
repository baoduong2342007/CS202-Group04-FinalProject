/**
 * @file Mushroom.h
 * @author TV5 (Truyền)
 * @brief Super Mushroom — moves horizontally, grows Mario from SMALL to SUPER
 * @note Week 5 — spawned by EntityFactory; power-up logic uses Mario's current state
 *       (no EventBus payload needed — the item queries Mario directly)
 */

#pragma once

#include "items/Item.h"

class Mushroom : public Item {
public:
    // 1. Constructor / Destructor
    Mushroom();
    Mushroom(const sf::Vector2f& position, b2World* world);
    ~Mushroom() override = default;

    // 2. Override methods
    void update(float dt) override;
    void onCollect(Mario& mario) override;

    // 3. Public methods
    /// Reverse horizontal direction when hitting a wall
    void onWallCollision();

private:
    // 5. Private methods
    void patrol();

    // 6. Private members
    float m_patrolSpeed;
    /// +1 = moving right, -1 = moving left
    int m_patrolDirection;
};

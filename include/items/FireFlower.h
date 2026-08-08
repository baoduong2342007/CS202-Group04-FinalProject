/**
 * @file FireFlower.h
 * @author TV5 (Truyền)
 * @brief Fire Flower — stationary item that turns Mario into FIRE state
 * @note Week 5 — spawned by EntityFactory; power-up logic queries Mario's state
 *       (no EventBus payload needed — the item decides the target state itself)
 */

#pragma once

#include "items/Item.h"

class FireFlower : public Item {
public:
    // 1. Constructor / Destructor
    FireFlower();
    FireFlower(const sf::Vector2f& position, b2World* world);
    ~FireFlower() override = default;

    // 2. Override methods
    void update(float dt) override;
    void onCollect(Mario& mario) override;

};

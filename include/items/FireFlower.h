/**
 * @file FireFlower.h
 * @author TV5 (Truyền)
 * @brief Fire Flower — stationary item that applies FIRE to the current body tier
 * @note Small Mario becomes Small Fire Mario; Super Mario becomes Super Fire Mario
 */

#pragma once

#include "items/Item.h"

class FireFlower : public Item {
public:
    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::FIRE_FLOWER; }
    // 1. Constructor / Destructor
    FireFlower();
    FireFlower(const sf::Vector2f& position, b2World* world);
    ~FireFlower() override = default;

    // 2. Override methods
    void update(float dt) override;
    void onCollect(Mario& mario) override;

};

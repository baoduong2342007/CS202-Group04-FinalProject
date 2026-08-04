/**
 * @file Coin.h
 * @author TV5 (Truyền)
 * @brief Concrete collectible item — awards score and publishes COIN_COLLECTED
 * @note Week 4 — spawned by EntityFactory or TileMap parser
 */

#pragma once

#include "items/Item.h"

class Coin : public Item {
public:
    // 1. Constructor / Destructor
    Coin();
    Coin(const sf::Vector2f& position, b2World* world);
    ~Coin() override = default;

    // 2. Override methods
    void update(float dt) override;
    void onCollect(Mario& mario) override;

private:
    // 6. Private members
    static constexpr int COIN_SCORE_VALUE = 200;
};

/**
 * @file Coin.h
 * @author TV5 (Truyền)
 * @brief Concrete collectible item — awards score and publishes COIN_COLLECTED
 * @note Week 4 — spawned by EntityFactory or TileMap parser
 */

#pragma once

#include "items/Item.h"

enum class CoinType {
    COLLECTIBLE,    // Static collectible coin placed on map
    QUESTION_POPUP  // Animated pop-up coin spawned from QuestionBlock
};

class Coin : public Item {
public:
    // 1. Constructor / Destructor
    Coin();
    Coin(const sf::Vector2f& position, b2World* world, CoinType type = CoinType::COLLECTIBLE);
    ~Coin() override = default;

    // 2. Override methods
    void update(float dt) override;
    void onCollect(Mario& mario) override;

    CoinType getType() const { return m_type; }

private:
    // 6. Private members
    CoinType m_type = CoinType::COLLECTIBLE;
    float m_popupTimer = 0.f;
    float m_initialY = 0.f;

    static constexpr float POPUP_DURATION = 0.45f;
    static constexpr float POPUP_HEIGHT = 48.f;
    static constexpr int COIN_SCORE_VALUE = 200;
};

/**
 * @file Coin.h
 * @author TV5 (Truyen)
 * @brief Concrete collectible item — awards score and publishes COIN_COLLECTED
 * @note Week 4 — spawned by EntityFactory or TileMap parser
 */

#pragma once

#include "core/LevelCatalog.h"
#include "items/Item.h"

enum class CoinType {
    COLLECTIBLE,    // Static collectible coin placed on map
    QUESTION_POPUP  // Animated pop-up coin spawned from QuestionBlock
};

class Coin : public Item {
public:
    // 1. Constructor / Destructor
    Coin(LevelTheme theme = LevelTheme::OVERWORLD);
    Coin(const sf::Vector2f& position,
         b2World* world,
         CoinType type = CoinType::COLLECTIBLE,
         LevelTheme theme = LevelTheme::OVERWORLD);
    ~Coin() override = default;

    // 2. Override methods
    void update(float dt) override;
    void onCollect(Mario& mario) override;

    /// Award one coin and convert every complete 100-coin group into a life.
    /// Keeping this rule here makes normal coins and QuestionBlock coins use
    /// exactly the same scoring path.
    static void awardTo(Mario& mario);

    CoinType getCoinType() const { return m_type; }
    LevelTheme getTheme() const { return m_theme; }
    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::COIN; }

private:
    // 6. Private members
    CoinType m_type = CoinType::COLLECTIBLE;
    LevelTheme m_theme = LevelTheme::OVERWORLD;
    float m_popupTimer = 0.f;
    float m_initialY = 0.f;

    static constexpr float POPUP_DURATION = 0.45f;
    static constexpr float POPUP_HEIGHT = 48.f;
    static constexpr int COIN_SCORE_VALUE = 100;
    static constexpr int COINS_PER_LIFE = 100;
};

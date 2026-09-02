/**
 * @file QuestionBlock.h
 * @author TV5 (Truyen)
 * @brief Animated Question block entity — 3-frame flashing & 12px bump animation on hit
 * @note Sprint 4 — spawned by EntityFactory from '?' tile code
 */

#pragma once

#include "entities/Entity.h"

class Mario;
class TextureManager;

enum class QuestionBlockContent {
    ADAPTIVE,       // Normal '?' block; resolved randomly when hit.
    COIN,
    SUPER_MUSHROOM,
    FIRE_FLOWER,
    ONEUP_MUSHROOM,
    STAR
};

enum class BlockTheme {
    OVERWORLD,
    UNDERGROUND,
    CASTLE,
    UNDERWATER
};

class QuestionBlock : public Entity {
public:
    // 1. Constructor / Destructor
    QuestionBlock(const sf::Vector2f& position, b2World* world, QuestionBlockContent content = QuestionBlockContent::ADAPTIVE, BlockTheme theme = BlockTheme::OVERWORLD);
    ~QuestionBlock() override = default;

    // 2. Override methods
    void update(float dt) override;
    EntityType getType() const override { return EntityType::TERRAIN; }
    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::QUESTION_BLOCK; }

    // 3. Public methods
    /// Triggers block hit reaction: 12px bump animation, spawns item & changes sprite to empty block
    void onHit(Mario& mario, std::vector<std::unique_ptr<Entity>>* entities = nullptr, TextureManager* textureManager = nullptr);
    /// Resolve a normal '?' result from a deterministic 0-199 roll.
    /// 0-139 = Coin, 140-169 = Mushroom, 170-199 = FireFlower.
    static QuestionBlockContent chooseRandomContent(unsigned int roll);
    /// Resolve a normal '?' result using the gameplay random source.
    static QuestionBlockContent chooseRandomContent();
    bool isHit() const { return m_isHit; }
    QuestionBlockContent getContent() const { return m_content; }
    BlockTheme getTheme() const { return m_theme; }

    static constexpr unsigned int CONTENT_ROLL_RANGE = 200;
    static constexpr unsigned int COIN_DROP_RATE_PERCENT = 70;

private:
    QuestionBlockContent m_content = QuestionBlockContent::ADAPTIVE;
    BlockTheme m_theme = BlockTheme::OVERWORLD;
    bool m_contentResolved = false;
    bool m_isHit = false;
    bool m_isBumping = false;
    float m_bumpTimer = 0.f;
    float m_bumpOffsetY = 0.f;

    static constexpr float BUMP_DURATION = 0.16f;
    static constexpr float MAX_BUMP_OFFSET = -12.f;
    static constexpr float ITEM_EMERGE_DELAY = 0.18f;
};

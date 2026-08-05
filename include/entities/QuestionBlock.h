/**
 * @file QuestionBlock.h
 * @author TV5 (Truyền)
 * @brief Animated Question block entity — 3-frame flashing & 12px bump animation on hit
 * @note Sprint 4 — spawned by EntityFactory from '?' tile code
 */

#pragma once

#include "entities/Entity.h"

class Mario;
class TextureManager;

enum class QuestionBlockContent {
    COIN,
    SUPER_MUSHROOM,
    ONEUP_MUSHROOM
};

class QuestionBlock : public Entity {
public:
    // 1. Constructor / Destructor
    QuestionBlock(const sf::Vector2f& position, b2World* world, QuestionBlockContent content = QuestionBlockContent::COIN);
    ~QuestionBlock() override = default;

    // 2. Override methods
    void update(float dt) override;
    bool isQuestionBlock() const override { return true; }

    // 3. Public methods
    /// Triggers block hit reaction: 12px bump animation, spawns item & changes sprite to empty block
    void onHit(Mario& mario, std::vector<std::unique_ptr<Entity>>* entities = nullptr, TextureManager* textureManager = nullptr);
    bool isHit() const { return m_isHit; }


private:
    QuestionBlockContent m_content = QuestionBlockContent::COIN;
    bool m_isHit = false;
    bool m_isBumping = false;
    float m_bumpTimer = 0.f;
    float m_bumpOffsetY = 0.f;

    static constexpr float BUMP_DURATION = 0.16f;
    static constexpr float MAX_BUMP_OFFSET = -12.f;
};

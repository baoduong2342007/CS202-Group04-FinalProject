/**
 * @file QuestionBlock.cpp
 * @author TV5 (Truyền)
 * @brief Animated Question block — 3-frame animation from items_blocks.png
 * @note Sprint 4 — uses AnimationSystem, same pattern as Coin
 */

#include "level/QuestionBlock.h"
#include "core/AnimationSystem.h"
#include "core/SpriteFrames.h"

namespace {
constexpr float BLOCK_SIZE = 32.f;
constexpr const char* QUESTION_BLOCK_TEXTURE = "assets/textures/items/items_blocks.png";
constexpr float ANIM_FRAME_DURATION = 0.2f;
} // namespace

QuestionBlock::QuestionBlock(const sf::Vector2f& position, b2World* world)
    : Entity(position, sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE)) {
    initPhysics(world, b2_staticBody, sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE), false);
    setSprite(QUESTION_BLOCK_TEXTURE);
    m_animationSystem->addAnimation("idle",
        AnimationSystem::createManualAnimation(
            SpriteFrames::Blocks::questionBlockFrames(), ANIM_FRAME_DURATION));
    playAnimation("idle");
}

void QuestionBlock::update(float dt) {
    syncPhysics();
    updateAnimation(dt);
}

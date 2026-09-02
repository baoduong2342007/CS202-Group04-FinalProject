/**
 * @file BowserAxe.cpp
 * @brief Bowser axe implementation - blinking pickup at the bridge's end
 */

#include "entities/BowserAxe.h"

#include "core/AnimationSystem.h"
#include "core/SpriteFrames_shared.h"

namespace {

const sf::Vector2f AXE_SIZE{32.f, 32.f};
constexpr const char* TILESET_TEXTURE_PATH = "assets/textures/tiles/tileset.png";
constexpr float AXE_FRAME_DURATION = 0.25f;
constexpr const char* BLINK_ANIMATION = "blink";

} // namespace

BowserAxe::BowserAxe(const sf::Vector2f& position, LevelTheme theme)
    : Entity(position, AXE_SIZE) {
    (void)theme;

    // The axe cells live in the tileset atlas (see SMB1 sprite resources
    // for exact cell mapping).
    setSprite(TILESET_TEXTURE_PATH);

    m_animationSystem = std::make_unique<AnimationSystem>();
    m_animationSystem->addAnimation(
        BLINK_ANIMATION,
        AnimationSystem::createManualAnimation(
            {SpriteFrames::legacy::LevelEntities::BOWSER_AXE,
             sf::IntRect({315, 129}, {16, 16})},
            AXE_FRAME_DURATION, true));
    playAnimation(BLINK_ANIMATION);
}

void BowserAxe::update(float dt) {
    m_blinkTimer += dt;
    updateAnimation(dt);

    if (m_sprite) {
        m_sprite->setPosition(m_position);
        m_sprite->setScale({2.f, 2.f});
    }
}

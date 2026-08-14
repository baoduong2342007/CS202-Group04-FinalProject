/**
 * @file ScorePopup.cpp
 * @brief Score popup animation and shared-atlas frame mapping.
 */

#include "entities/ScorePopup.h"

#include "core/AnimationSystem.h"
#include "core/SpriteFrames_shared.h"

namespace {
constexpr const char* SCORE_TEXTURE_PATH =
    "assets/textures/items/items_objects.png";
constexpr float POPUP_WIDTH = 32.f;
constexpr float POPUP_HEIGHT = 16.f;
constexpr float POPUP_LIFETIME = 0.8f;
constexpr float RISE_SPEED = 36.f;
}

ScorePopup::ScorePopup(const sf::Vector2f& position, int points, bool oneUp)
    : Entity(position, {POPUP_WIDTH, POPUP_HEIGHT}),
      m_points(points),
      m_oneUp(oneUp),
      m_lifetime(POPUP_LIFETIME) {
    setSprite(SCORE_TEXTURE_PATH);
    m_animationSystem->addAnimation(
        "score",
        AnimationSystem::createManualAnimation(
            std::vector<sf::IntRect>{frameFor(points, oneUp)}, 1.f));
    playAnimation("score");
}

const sf::IntRect& ScorePopup::frameFor(int points, bool oneUp) {
    using namespace SpriteFrames::shared::Items;
    if (oneUp) return SCORE_1UP;

    switch (points) {
        case 100: return SCORE_100;
        case 200: return SCORE_200;
        case 400: return SCORE_400;
        case 800: return SCORE_800;
        case 1000: return SCORE_1000;
        case 2000: return SCORE_2000;
        case 4000: return SCORE_4000;
        case 5000: return SCORE_5000;
        case 8000: return SCORE_8000;
        default: return SCORE_100;
    }
}

void ScorePopup::update(float dt) {
    if (!m_active) return;

    m_lifetime -= dt;
    if (m_lifetime <= 0.f) {
        markForRemoval();
        return;
    }

    m_position.y -= RISE_SPEED * dt;
    updateAnimation(dt);

    if (m_sprite) {
        const sf::IntRect rect = m_sprite->getTextureRect();
        if (rect.size.x > 0 && rect.size.y > 0) {
            m_sprite->setOrigin({rect.size.x / 2.f, rect.size.y / 2.f});
            m_sprite->setScale({2.f, 2.f});
        }
        m_sprite->setPosition(m_position);
    }
}

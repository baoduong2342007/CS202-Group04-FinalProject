/**
 * @file ScorePopup.cpp
 * @brief Score popup animation and theme-aware shared-atlas frame mapping.
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
} // namespace

ScorePopup::ScorePopup(const sf::Vector2f& position, int points, bool oneUp, LevelTheme theme)
    : Entity(position, {POPUP_WIDTH, POPUP_HEIGHT}),
      m_points(points),
      m_oneUp(oneUp),
      m_theme(theme),
      m_lifetime(POPUP_LIFETIME) {
    setSprite(SCORE_TEXTURE_PATH);
    m_animationSystem->addAnimation(
        "score",
        AnimationSystem::createManualAnimation(
            std::vector<sf::IntRect>{frameFor(points, oneUp, theme)}, 1.f));
    playAnimation("score");
}

const sf::IntRect& ScorePopup::frameFor(int points, bool oneUp, LevelTheme theme) {
    using namespace SpriteFrames::shared::Items;
    // Theme-specific frames:
    // Index: 0=100, 1=200, 2=400, 3=500, 4=800, 5=1000, 6=2000, 7=4000, 8=5000, 9=8000, 10=1UP
    static const sf::IntRect themedFrames[4][11] = {
        // OVERWORLD (theme 0, offset 0)
        { SCORE_100, SCORE_200, SCORE_400, SCORE_500, SCORE_800, SCORE_1000, SCORE_2000, SCORE_4000, SCORE_5000, SCORE_8000, SCORE_1UP },
        // UNDERGROUND (theme 1, offset 108)
        { sf::IntRect({234, 134}, {16, 8}), sf::IntRect({234, 144}, {16, 8}), sf::IntRect({234, 154}, {16, 8}), sf::IntRect({234, 164}, {16, 8}), sf::IntRect({234, 174}, {16, 8}), sf::IntRect({252, 134}, {16, 8}), sf::IntRect({252, 144}, {16, 8}), sf::IntRect({252, 154}, {16, 8}), sf::IntRect({252, 164}, {16, 8}), sf::IntRect({252, 174}, {16, 8}), sf::IntRect({252, 184}, {16, 8}) },
        // CASTLE (theme 2, offset 216)
        { sf::IntRect({234, 242}, {16, 8}), sf::IntRect({234, 252}, {16, 8}), sf::IntRect({234, 262}, {16, 8}), sf::IntRect({234, 272}, {16, 8}), sf::IntRect({234, 282}, {16, 8}), sf::IntRect({252, 242}, {16, 8}), sf::IntRect({252, 252}, {16, 8}), sf::IntRect({252, 262}, {16, 8}), sf::IntRect({252, 272}, {16, 8}), sf::IntRect({252, 282}, {16, 8}), sf::IntRect({252, 292}, {16, 8}) },
        // UNDERWATER (theme 3, offset 324)
        { sf::IntRect({234, 350}, {16, 8}), sf::IntRect({234, 360}, {16, 8}), sf::IntRect({234, 370}, {16, 8}), sf::IntRect({234, 380}, {16, 8}), sf::IntRect({234, 390}, {16, 8}), sf::IntRect({252, 350}, {16, 8}), sf::IntRect({252, 360}, {16, 8}), sf::IntRect({252, 370}, {16, 8}), sf::IntRect({252, 380}, {16, 8}), sf::IntRect({252, 390}, {16, 8}), sf::IntRect({252, 400}, {16, 8}) }
    };

    int themeIdx = 0;
    switch (theme) {
        case LevelTheme::UNDERGROUND: themeIdx = 1; break;
        case LevelTheme::CASTLE:      themeIdx = 2; break;
        case LevelTheme::UNDERWATER:  themeIdx = 3; break;
        case LevelTheme::OVERWORLD:
        default:                      themeIdx = 0; break;
    }

    if (oneUp) return themedFrames[themeIdx][10];

    int pointIdx = 0;
    switch (points) {
        case 100:  pointIdx = 0; break;
        case 200:  pointIdx = 1; break;
        case 400:  pointIdx = 2; break;
        case 500:  pointIdx = 3; break;
        case 800:  pointIdx = 4; break;
        case 1000: pointIdx = 5; break;
        case 2000: pointIdx = 6; break;
        case 4000: pointIdx = 7; break;
        case 5000: pointIdx = 8; break;
        case 8000: pointIdx = 9; break;
        default:   pointIdx = 0; break;
    }
    return themedFrames[themeIdx][pointIdx];
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

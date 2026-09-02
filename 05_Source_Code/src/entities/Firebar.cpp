/**
 * @file Firebar.cpp
 * @author TV4 (Vy)
 * @brief Firebar entity implementation - rotating chain of fireballs
 * @note Sprint 7 - authentic SMB1 castle obstacle
 */

#include "entities/Firebar.h"

#include <cmath>
#include "core/SpriteFrames_castle.h"
#include "core/SpriteFrames_shared.h"

namespace {

constexpr const char* ITEMS_OBJECTS_TEXTURE_PATH = "assets/textures/items/items_objects.png";
constexpr sf::Vector2f FIREBAR_MOUNT_SIZE{32.f, 32.f};
constexpr float FRAME_DURATION = 0.08f;

const std::vector<sf::IntRect>& getFireballFrames(LevelTheme theme) {
    static const std::vector<sf::IntRect> castleFrames = {
        sf::IntRect({180, 270}, {8, 8}),
        sf::IntRect({190, 270}, {8, 8}),
        sf::IntRect({200, 270}, {8, 8}),
        sf::IntRect({210, 270}, {8, 8})
    };
    static const std::vector<sf::IntRect> defaultFrames = {
        SpriteFrames::legacy::Items::FIREBALL_1,
        SpriteFrames::legacy::Items::FIREBALL_2,
        SpriteFrames::legacy::Items::FIREBALL_3,
        SpriteFrames::legacy::Items::FIREBALL_4
    };

    if (theme == LevelTheme::CASTLE) {
        return castleFrames;
    }
    return defaultFrames;
}

} // namespace

Firebar::Firebar(const sf::Vector2f& position,
                 b2World* world,
                 LevelTheme theme,
                 float initialAngle,
                 float rotationSpeed,
                 int ballCount)
    : Enemy(position, FIREBAR_MOUNT_SIZE, 1),
      m_angle(initialAngle),
      m_rotationSpeed(rotationSpeed),
      m_ballCount(ballCount),
      m_center(position + sf::Vector2f{16.f, 16.f}) {
    (void)world;
    (void)theme;

    setSprite(ITEMS_OBJECTS_TEXTURE_PATH);
    if (m_sprite) {
        m_ballSprite = std::make_unique<sf::Sprite>(*m_sprite);
        m_ballSprite->setOrigin({4.f, 4.f});
        m_ballSprite->setScale({2.f, 2.f});
    }

    // Initialize initial ball positions
    m_ballPositions.resize(m_ballCount);
    for (int i = 0; i < m_ballCount; ++i) {
        const float r = static_cast<float>(i + 1) * m_ballSpacing;
        m_ballPositions[i] = m_center + sf::Vector2f(std::cos(m_angle) * r,
                                                     std::sin(m_angle) * r);
    }
}

void Firebar::update(float dt) {
    m_angle += m_rotationSpeed * dt;

    // Normalize angle
    constexpr float TWO_PI = 6.28318530718f;
    if (m_angle >= TWO_PI) {
        m_angle -= TWO_PI;
    } else if (m_angle < 0.f) {
        m_angle += TWO_PI;
    }

    m_animTimer += dt;
    if (m_animTimer >= FRAME_DURATION) {
        m_animTimer -= FRAME_DURATION;
        m_animFrame = (m_animFrame + 1) % 4;
    }

    for (int i = 0; i < m_ballCount; ++i) {
        const float r = static_cast<float>(i + 1) * m_ballSpacing;
        m_ballPositions[i] = m_center + sf::Vector2f(std::cos(m_angle) * r,
                                                     std::sin(m_angle) * r);
    }
}

void Firebar::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (!m_ballSprite && m_sprite) {
        auto* self = const_cast<Firebar*>(this);
        self->m_ballSprite = std::make_unique<sf::Sprite>(*m_sprite);
        self->m_ballSprite->setOrigin({4.f, 4.f});
        self->m_ballSprite->setScale({2.f, 2.f});
    }

    if (!m_ballSprite) {
        return;
    }

    const auto& frames = getFireballFrames(LevelTheme::CASTLE);
    m_ballSprite->setTextureRect(frames[m_animFrame]);

    for (const auto& ballPos : m_ballPositions) {
        m_ballSprite->setPosition(ballPos);
        target.draw(*m_ballSprite, states);
    }
}

bool Firebar::checkMarioCollision(const sf::FloatRect& marioBox) const {
    constexpr float BALL_HITBOX_RADIUS = 7.f;
    for (const auto& ballPos : m_ballPositions) {
        const sf::FloatRect ballBox(
            {ballPos.x - BALL_HITBOX_RADIUS, ballPos.y - BALL_HITBOX_RADIUS},
            {BALL_HITBOX_RADIUS * 2.f, BALL_HITBOX_RADIUS * 2.f});

        if (marioBox.findIntersection(ballBox).has_value()) {
            return true;
        }
    }
    return false;
}

/**
 * @file BowserFire.cpp
 * @brief Bowser fire breath implementation - straight flight, sine wave
 */

#include "entities/BowserFire.h"

#include <cmath>

#include <box2d/box2d.h>

#include "core/AnimationSystem.h"
#include "core/SpriteFrames_shared.h"
#include "physics/PhysicsEngine.h"

namespace {

const sf::Vector2f FIRE_SIZE{48.f, 16.f};
constexpr const char* FIRE_TEXTURE_PATH = "assets/textures/enemies/enemies.png";
constexpr const char* BURN_ANIMATION = "burn";

const sf::IntRect& fire1(LevelTheme theme) {
    switch (theme) {
        case LevelTheme::UNDERGROUND:
        case LevelTheme::CASTLE:
            return SpriteFrames::legacy::Enemies::BowserFireball::UG_FIRE1;
        case LevelTheme::UNDERWATER:
            return SpriteFrames::legacy::Enemies::BowserFireball::UW_FIRE1;
        case LevelTheme::OVERWORLD:
        default:
            return SpriteFrames::legacy::Enemies::BowserFireball::FIRE1;
    }
}

const sf::IntRect& fire2(LevelTheme theme) {
    switch (theme) {
        case LevelTheme::UNDERGROUND:
        case LevelTheme::CASTLE:
            return SpriteFrames::legacy::Enemies::BowserFireball::UG_FIRE2;
        case LevelTheme::UNDERWATER:
            return SpriteFrames::legacy::Enemies::BowserFireball::UW_FIRE2;
        case LevelTheme::OVERWORLD:
        default:
            return SpriteFrames::legacy::Enemies::BowserFireball::FIRE2;
    }
}

} // namespace

BowserFire::BowserFire(const sf::Vector2f& position,
                       b2World* world,
                       LevelTheme theme,
                       Direction direction)
    : EnemyProjectile(position, FIRE_SIZE, world,
                      {direction == Direction::LEFT ? -SPEED : SPEED, 0.f},
                      0.f),
      m_direction(direction) {
    m_baseY = position.y;

    setSprite(FIRE_TEXTURE_PATH);

    m_animationSystem = std::make_unique<AnimationSystem>();
    m_animationSystem->addAnimation(
        BURN_ANIMATION,
        AnimationSystem::createManualAnimation({fire1(theme), fire2(theme)},
                                                FRAME_DURATION, true));
    playAnimation(BURN_ANIMATION);
}

void BowserFire::update(float dt) {
    // The wave is a position offset, not a velocity: the flame keeps its
    // horizontal speed while its altitude oscillates around the launch row.
    m_waveTime += dt;

    sf::Vector2f velocity = getVelocity();
    velocity.x = m_direction == Direction::LEFT ? -SPEED : SPEED;
    velocity.y = 0.f;
    setVelocity(velocity);

    EnemyProjectile::update(dt);

    constexpr float TWO_PI = 6.28318530718f;
    const float offsetY =
        WAVE_AMPLITUDE * std::sin(TWO_PI * WAVE_FREQUENCY * m_waveTime);

    if (m_body) {
        const b2Vec2 center = m_body->GetPosition();
        const float centerY =
            PhysicsEngine::pixelsToMeters(m_baseY + offsetY + m_size.y / 2.f);
        m_body->SetTransform(b2Vec2(center.x, centerY), 0.f);
    }
}

void BowserFire::updatePresentation(float dt) {
    (void)dt;

    if (m_sprite) {
        m_sprite->setPosition(m_position);
        m_sprite->setScale(m_direction == Direction::RIGHT
                               ? sf::Vector2f{-2.f, 2.f}
                               : sf::Vector2f{2.f, 2.f});
        if (m_direction == Direction::RIGHT) {
            m_sprite->setOrigin({static_cast<float>(m_sprite->getTextureRect().size.x), 0.f});
            m_sprite->setPosition({m_position.x + m_size.x, m_position.y});
        } else {
            m_sprite->setOrigin({0.f, 0.f});
        }
    }
}

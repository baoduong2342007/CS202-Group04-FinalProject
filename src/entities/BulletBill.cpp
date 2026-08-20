/**
 * @file BulletBill.cpp
 * @brief Bullet Bill implementation - straight sensor flight, flip-out death
 */

#include "entities/BulletBill.h"

#include <cmath>

#include <box2d/box2d.h>

#include "core/AnimationSystem.h"
#include "core/SpriteFrames_shared.h"

namespace {

const sf::Vector2f BULLET_SIZE{32.f, 32.f};
constexpr const char* BULLET_TEXTURE_PATH = "assets/textures/enemies/enemies.png";
constexpr float BULLET_FRAME_DURATION = 0.15f;
constexpr const char* FLY_ANIMATION = "fly";

const sf::IntRect& bulletFrame(LevelTheme theme) {
    switch (theme) {
        case LevelTheme::UNDERGROUND:
            return SpriteFrames::legacy::Enemies::BulletBill::UG_BULLET;
        case LevelTheme::CASTLE:
            return SpriteFrames::legacy::Enemies::BulletBill::CASTLE_BULLET;
        case LevelTheme::UNDERWATER:
            return SpriteFrames::legacy::Enemies::BulletBill::UW_BULLET;
        case LevelTheme::OVERWORLD:
        default:
            return SpriteFrames::legacy::Enemies::BulletBill::BULLET;
    }
}

} // namespace

BulletBill::BulletBill(const sf::Vector2f& position,
                       b2World* world,
                       LevelTheme theme,
                       Direction direction)
    : Enemy(position, BULLET_SIZE, 1),
      m_direction(direction) {
    setSprite(BULLET_TEXTURE_PATH);

    // A pure projectile body: sensor fixtures so terrain and enemies never
    // deflect it, and no gravity so it flies level.
    initPhysics(world, b2_dynamicBody, BULLET_SIZE, true);

    if (m_body) {
        m_body->SetGravityScale(0.f);
    }

    setVelocity({m_direction == Direction::LEFT ? -SPEED : SPEED, 0.f});

    m_animationSystem = std::make_unique<AnimationSystem>();
    m_animationSystem->addAnimation(
        FLY_ANIMATION,
        AnimationSystem::createManualAnimation({bulletFrame(theme)},
                                                BULLET_FRAME_DURATION, false));
    playAnimation(FLY_ANIMATION);
}

void BulletBill::update(float dt) {
    syncPhysics();

    if (m_isFlippedDead) {
        if (m_sprite) {
            m_sprite->setPosition(m_position + sf::Vector2f(m_size.x / 2.f, m_size.y / 2.f));
            m_sprite->setOrigin({8.f, 8.f});
            m_sprite->setScale({2.f, -2.f});
        }
        if (m_position.y > PIT_CLEANUP_Y) {
            markForRemoval();
        }
        return;
    }

    // The bullet never changes course; the level-bounds cleanup removes it
    // once it flies off the far edge.
    sf::Vector2f velocity = getVelocity();
    velocity.x = m_direction == Direction::LEFT ? -SPEED : SPEED;
    velocity.y = 0.f;
    setVelocity(velocity);

    updateAnimation(dt);

    if (m_sprite) {
        m_sprite->setPosition(m_position);
        // The atlas bullet faces left; mirror it when flying right.
        m_sprite->setScale(m_direction == Direction::RIGHT ? sf::Vector2f{-2.f, 2.f}
                                                           : sf::Vector2f{2.f, 2.f});
        if (m_direction == Direction::RIGHT) {
            m_sprite->setOrigin({16.f, 0.f});
            m_sprite->setPosition({m_position.x + m_size.x, m_position.y});
        } else {
            m_sprite->setOrigin({0.f, 0.f});
        }
    }
}

void BulletBill::patrol() {
    // Flight is driven entirely by update().
}

void BulletBill::onStomp() {
    if (m_isFlippedDead) return;
    dieFlipped();
}

void BulletBill::onWallCollision() {
    // Bullets fly straight through the world.
}

void BulletBill::onFireHit() {
    // Unreachable through fireballs (fireproof) but star contact lands here.
    if (m_isFlippedDead) return;
    dieFlipped();
}

void BulletBill::dieFlipped() {
    m_isFlippedDead = true;
    setHealth(0);

    if (m_body) {
        m_body->SetGravityScale(1.f);
        m_body->SetLinearVelocity(b2Vec2(0.f, -8.f));
    }
}

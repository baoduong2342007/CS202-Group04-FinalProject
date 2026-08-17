/**
 * @file Blooper.cpp
 * @brief Blooper implementation - zig-zag pursuit cycle over a gravity-free body
 */

#include "entities/Blooper.h"

#include <cmath>

#include <box2d/box2d.h>

#include "core/AnimationSystem.h"
#include "core/SpriteFrames_ovw.h"
#include "core/SpriteFrames_udg.h"
#include "core/SpriteFrames_castle.h"
#include "core/SpriteFrames_udw.h"
#include "core/SpriteFrames_shared.h"

namespace {

const sf::Vector2f BLOOPER_SIZE{32.f, 32.f};
constexpr const char* BLOOPER_TEXTURE_PATH = "assets/textures/enemies/enemies.png";
constexpr float BLOOPER_FRAME_DURATION = 0.15f;
constexpr const char* DRIFT_ANIMATION = "drift";
constexpr const char* RISE_ANIMATION = "rise";

const sf::IntRect& driftFrame(LevelTheme theme) {
    switch (theme) {
        case LevelTheme::UNDERGROUND:
            return SpriteFrames::legacy::Enemies::Blooper::UG_SWIM_OPEN;
        case LevelTheme::CASTLE:
            return SpriteFrames::legacy::Enemies::Blooper::CASTLE_SWIM_OPEN;
        case LevelTheme::UNDERWATER:
            return SpriteFrames::legacy::Enemies::Blooper::UW_SWIM_OPEN;
        case LevelTheme::OVERWORLD:
        default:
            return SpriteFrames::legacy::Enemies::Blooper::SWIM_OPEN;
    }
}

const sf::IntRect& riseFrame(LevelTheme theme) {
    switch (theme) {
        case LevelTheme::UNDERGROUND:
            return SpriteFrames::legacy::Enemies::Blooper::UG_SWIM_CLOSED;
        case LevelTheme::CASTLE:
            return SpriteFrames::legacy::Enemies::Blooper::CASTLE_SWIM_CLOSED;
        case LevelTheme::UNDERWATER:
            return SpriteFrames::legacy::Enemies::Blooper::UW_SWIM_CLOSED;
        case LevelTheme::OVERWORLD:
        default:
            return SpriteFrames::legacy::Enemies::Blooper::SWIM_CLOSED;
    }
}

} // namespace

Blooper::Blooper(const sf::Vector2f& position,
                 b2World* world,
                 LevelTheme theme)
    : Enemy(position, BLOOPER_SIZE, 1) {
    setSprite(BLOOPER_TEXTURE_PATH);

    initPhysics(world, b2_dynamicBody, BLOOPER_SIZE);

    if (m_body) {
        // The squid floats; gravity only returns after a fireball defeat.
        m_body->SetGravityScale(0.f);
    }

    m_animationSystem = std::make_unique<AnimationSystem>();
    m_animationSystem->addAnimation(
        DRIFT_ANIMATION,
        AnimationSystem::createManualAnimation({driftFrame(theme)},
                                                BLOOPER_FRAME_DURATION, false));
    m_animationSystem->addAnimation(
        RISE_ANIMATION,
        AnimationSystem::createManualAnimation({riseFrame(theme)},
                                                BLOOPER_FRAME_DURATION, false));
    playAnimation(RISE_ANIMATION);
}

void Blooper::update(float dt) {
    if (m_isFlippedDead) {
        syncPhysics();
        if (m_sprite) {
            m_sprite->setOrigin({8.f, 12.f});
            m_sprite->setPosition(m_position + sf::Vector2f(m_size.x / 2.f, m_size.y / 2.f));
            m_sprite->setScale({2.f, -2.f});
        }
        if (m_position.y > PIT_CLEANUP_Y) {
            markForRemoval();
        }
        return;
    }

    syncPhysics();

    if (isDead()) {
        updateAnimation(dt);
        if (m_sprite) {
            m_sprite->setPosition(m_position);
            m_sprite->setScale({2.f, 2.f});
        }
        return;
    }

    m_stateTimer += dt;

    sf::Vector2f velocity = getVelocity();

    if (m_state == State::RISE) {
        velocity.y = -RISE_SPEED;
        velocity.x = 0.f;
        if (m_stateTimer >= RISE_TIME) {
            m_state = State::DRIFT;
            m_stateTimer = 0.f;
            playAnimation(DRIFT_ANIMATION);
        }
    } else {
        velocity.y = DRIFT_FALL_SPEED;
        // Home in on Mario's column while sinking; drift left by default
        // before any player position has been fed in.
        float targetVx = -DRIFT_TRACK_SPEED;
        if (m_marioKnown) {
            const float dx = m_marioPosition.x - m_position.x;
            targetVx = (std::abs(dx) < 4.f)
                           ? 0.f
                           : (dx > 0.f ? DRIFT_TRACK_SPEED : -DRIFT_TRACK_SPEED);
        }
        velocity.x = targetVx;

        if (m_stateTimer >= DRIFT_TIME) {
            m_state = State::RISE;
            m_stateTimer = 0.f;
            playAnimation(RISE_ANIMATION);
        }
    }

    setVelocity(velocity);
    updateAnimation(dt);

    if (m_sprite) {
        m_sprite->setPosition(m_position);
        m_sprite->setScale({2.f, 2.f});
    }
}

void Blooper::patrol() {
    // Movement is driven entirely by the update() state machine.
}

void Blooper::onStomp() {
    // Unstompable: CollisionManager powers Mario down before reaching here.
}

void Blooper::onWallCollision() {
    // The squid floats freely and never walks into walls.
}

void Blooper::onFireHit() {
    if (m_isFlippedDead) return;

    m_isFlippedDead = true;
    setHealth(0);

    b2Body* body = getBody();
    if (body) {
        for (b2Fixture* fixture = body->GetFixtureList(); fixture != nullptr; fixture = fixture->GetNext()) {
            fixture->SetSensor(true);
        }
        body->SetGravityScale(1.f);
        body->SetLinearVelocity(b2Vec2(0.f, -8.f));
    }
}

void Blooper::updateMarioPosition(const sf::Vector2f& marioPos) {
    m_marioPosition = marioPos;
    m_marioKnown = true;
}

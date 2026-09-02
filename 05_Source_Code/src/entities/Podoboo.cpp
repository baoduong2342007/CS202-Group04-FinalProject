/**
 * @file Podoboo.cpp
 * @brief Podoboo implementation - manual parabolic leap out of the lava
 */

#include "entities/Podoboo.h"
#include "entities/FireballExplosion.h"

#include <cmath>

#include <box2d/box2d.h>

#include "core/AnimationSystem.h"
#include "core/SpriteFrames_shared.h"
#include "physics/PhysicsEngine.h"

namespace {

const sf::Vector2f PODOBOO_SIZE{32.f, 32.f};
constexpr const char* PODOBOO_TEXTURE_PATH = "assets/textures/enemies/enemies.png";
constexpr float PODOBOO_FRAME_DURATION = 0.15f;
constexpr const char* FLY_ANIMATION = "fly";

} // namespace

Podoboo::Podoboo(const sf::Vector2f& position,
                 b2World* world,
                 LevelTheme theme)
    : Enemy(position, PODOBOO_SIZE, 1) {
    (void)theme;

    // The spawn cell is the lava surface: the bubble hides one body height
    // below it while waiting and leaps up from there.
    m_lavaY = position.y + PODOBOO_SIZE.y;
    m_position.y = m_lavaY;

    // Stagger neighbouring podoboos so a lava field ripples instead of
    // firing in lockstep.
    const int cellOffset = static_cast<int>(std::floor(position.x / 32.f)) % 4;
    m_timer = -0.4f * static_cast<float>(cellOffset);

    setSprite(PODOBOO_TEXTURE_PATH);

    // A sensor-only kinematic body: nothing can push or stand on it.
    initPhysics(world, b2_kinematicBody, PODOBOO_SIZE, true);

    m_animationSystem = std::make_unique<AnimationSystem>();
    m_animationSystem->addAnimation(
        FLY_ANIMATION,
        AnimationSystem::createManualAnimation(
            {SpriteFrames::legacy::Enemies::Podoboo::BUBBLE},
            PODOBOO_FRAME_DURATION, false));
    playAnimation(FLY_ANIMATION);
}

void Podoboo::update(float dt) {
    if (!isActive()) {
        return;
    }

    m_timer += dt;

    if (m_phase == Phase::SUBMERGED_WAITING) {
        m_position.y = m_lavaY;
        if (m_timer >= WAIT_TIME) {
            m_phase = Phase::FLYING;
            m_timer = 0.f;
            m_velocityY = -LAUNCH_SPEED;
            // Spawn lava splash when leaping out of lava
            m_pending.push_back(std::make_unique<FireballExplosion>(
                sf::Vector2f{m_position.x + 8.f, m_lavaY - 16.f}));
        }
    } else {
        // Manual parabola: integrate upward velocity against gravity until
        // the bubble sinks back to the lava line.
        m_velocityY += GRAVITY * dt;
        m_position.y += m_velocityY * dt;

        if (m_velocityY > 0.f && m_position.y >= m_lavaY) {
            m_position.y = m_lavaY;
            m_phase = Phase::SUBMERGED_WAITING;
            m_timer = 0.f;
            // Spawn lava splash when plunging back into lava
            m_pending.push_back(std::make_unique<FireballExplosion>(
                sf::Vector2f{m_position.x + 8.f, m_lavaY - 16.f}));
        }
    }

    m_velocity = {0.f, m_velocityY};

    if (m_body) {
        m_body->SetTransform(
            PhysicsEngine::pixelsToMeters(m_position + m_size / 2.f), 0.f);
    }

    updateAnimation(dt);

    if (m_sprite) {
        if (m_velocityY > 0.f) {
            // Flipped on descent: flame point aims downward
            m_sprite->setScale({2.f, -2.f});
            m_sprite->setOrigin({0.f, 16.f});
        } else {
            // Upright on ascent: flame point aims upward
            m_sprite->setScale({2.f, 2.f});
            m_sprite->setOrigin({0.f, 0.f});
        }
        m_sprite->setPosition(m_position);
    }
}

std::vector<std::unique_ptr<Entity>> Podoboo::takePendingSpawns() {
    std::vector<std::unique_ptr<Entity>> drained;
    drained.swap(m_pending);
    return drained;
}

void Podoboo::patrol() {
    // The leap cycle is driven entirely by the update() state machine.
}

void Podoboo::onStomp() {
    // Unstompable: CollisionManager powers Mario down before reaching here.
}

void Podoboo::onWallCollision() {
}

void Podoboo::onFireHit() {
    // Completely invincible in SMB1: star, fire, and shell all do nothing.
}

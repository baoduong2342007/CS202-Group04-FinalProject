/**
 * @file SpinyEgg.cpp
 * @brief Spiny egg implementation - thrown arc, one bounce, hatch into Spiny
 */

#include "entities/SpinyEgg.h"
#include "entities/Spiny.h"
#include "entities/FireballExplosion.h"

#include <cmath>

#include <box2d/box2d.h>

#include "core/AnimationSystem.h"
#include "core/SpriteFrames_shared.h"

namespace {

const sf::Vector2f EGG_SIZE{32.f, 32.f};
constexpr const char* EGG_TEXTURE_PATH = "assets/textures/enemies/enemies.png";
constexpr float EGG_FRAME_DURATION = 0.1f;
constexpr const char* SPIN_ANIMATION = "spin";

} // namespace

SpinyEgg::SpinyEgg(const sf::Vector2f& position,
                   b2World* world,
                   LevelTheme theme,
                   Direction throwDirection)
    : Enemy(position, EGG_SIZE, 1),
      m_throwDirection(throwDirection),
      m_theme(theme) {
    setSprite(EGG_TEXTURE_PATH);

    initPhysics(world, b2_dynamicBody, EGG_SIZE);

    setVelocity({throwDirection == Direction::LEFT ? -THROW_VX : THROW_VX,
                 THROW_VY});

    m_animationSystem = std::make_unique<AnimationSystem>();
    m_animationSystem->addAnimation(
        SPIN_ANIMATION,
        AnimationSystem::createManualAnimation(
            {SpriteFrames::legacy::Enemies::RedSpiny::EGG1,
             SpriteFrames::legacy::Enemies::RedSpiny::EGG2},
            EGG_FRAME_DURATION, true));
    playAnimation(SPIN_ANIMATION);
}

void SpinyEgg::update(float dt) {
    if (m_isFlippedDead) {
        syncPhysics();
        if (m_sprite) {
            m_sprite->setPosition(m_position);
            m_sprite->setScale({2.f, -2.f});
        }
        if (m_position.y > PIT_CLEANUP_Y) {
            markForRemoval();
        }
        return;
    }

    if (m_stage == Stage::HATCH) {
        return;
    }

    syncPhysics();

    m_fuse += dt;
    if (m_fuse >= FUSE_TIME) {
        hatch();
        return;
    }

    const sf::Vector2f velocity = getVelocity();

    // A body that was clearly falling and has since (nearly) stopped has
    // just touched ground - the physics solver zeroes the vertical speed.
    if (m_wasFalling && std::abs(velocity.y) < LANDED_THRESHOLD) {
        if (m_stage == Stage::FLYING) {
            m_stage = Stage::BOUNCING;
            setVelocity({velocity.x, BOUNCE_VY});
        } else {
            hatch();
            return;
        }
    }

    m_wasFalling = getVelocity().y > FALLING_THRESHOLD;

    updateAnimation(dt);

    if (m_sprite) {
        m_sprite->setPosition(m_position);
        m_sprite->setScale({2.f, 2.f});
    }
}

void SpinyEgg::patrol() {
    // The arc is driven entirely by physics + update().
}

void SpinyEgg::onStomp() {
    // Unstompable: CollisionManager powers Mario down before reaching here.
}

void SpinyEgg::onWallCollision() {
    // Eggs keep their horizontal heading; walls are rare mid-arc.
}

void SpinyEgg::onFireHit() {
    if (m_isFlippedDead) return;

    m_isFlippedDead = true;
    setHealth(0);

    b2Body* body = getBody();
    if (body) {
        for (b2Fixture* fixture = body->GetFixtureList(); fixture != nullptr; fixture = fixture->GetNext()) {
            fixture->SetSensor(true);
        }
        body->SetLinearVelocity(b2Vec2(0.f, -8.f));
    }
}

void SpinyEgg::hatch() {
    if (m_hatched) {
        return;
    }
    m_hatched = true;
    m_stage = Stage::HATCH;

    // The hatchling walks off facing the direction the egg was travelling,
    // which is the side Lakitu threw it toward.
    m_pending.push_back(std::make_unique<Spiny>(
        m_position, getBody() ? getBody()->GetWorld() : nullptr, m_theme,
        m_throwDirection));

    markForRemoval();
}

std::vector<std::unique_ptr<Entity>> SpinyEgg::takePendingSpawns() {
    std::vector<std::unique_ptr<Entity>> drained;
    drained.swap(m_pending);
    return drained;
}

/**
 * @file Koopa.cpp
 * @author TV4 (Vy)
 * @brief Implementation of Koopa walking and patrol foundation
 * @note Sprint 5 - walking, stationary shell, and sliding shell states
 */

#include "entities/Koopa.h"

#include <memory>

#include <box2d/box2d.h>

#include "core/AnimationSystem.h"
#include "core/SpriteFrames.h"
#include "core/SoundManager.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"

namespace {

constexpr int DEFAULT_KOOPA_HEALTH = 1;
constexpr float DEFAULT_KOOPA_PATROL_SPEED = 50.f;
constexpr float KOOPA_SLIDE_SPEED = 240.f;
constexpr float PIT_CLEANUP_Y = 800.f;

constexpr const char* KOOPA_TEXTURE_PATH = "assets/textures/enemies/koopa.png";

constexpr float TILE_SIZE = 32.f;
constexpr float KOOPA_HEIGHT = 48.f;
constexpr float KOOPA_VERTICAL_SPAWN_OFFSET = KOOPA_HEIGHT - TILE_SIZE;

const sf::Vector2f KOOPA_SIZE{32.f, KOOPA_HEIGHT};

constexpr int KOOPA_FRAME_START_X = 0;
constexpr int KOOPA_FRAME_START_Y = 0;
constexpr int KOOPA_FRAME_WIDTH = 32;
constexpr int KOOPA_FRAME_HEIGHT = 48;
constexpr int KOOPA_WALK_FRAME_COUNT = 2;

constexpr float KOOPA_WALK_FRAME_DURATION = 0.15f;

constexpr const char* KOOPA_WALK_ANIMATION = "walk";

constexpr int KOOPA_SHELL_FRAME_START_X = 64;
constexpr int KOOPA_SHELL_FRAME_COUNT = 1;

constexpr float KOOPA_SHELL_FRAME_DURATION = 0.15f;

constexpr const char* KOOPA_SHELL_IDLE_ANIMATION = "shell_idle";

sf::Vector2f alignKoopaToGround(const sf::Vector2f& position) {
    return {position.x, position.y - KOOPA_VERTICAL_SPAWN_OFFSET};
}

} // namespace

Koopa::Koopa(const sf::Vector2f& position, b2World* world)
    : Enemy(alignKoopaToGround(position),
            KOOPA_SIZE,
            DEFAULT_KOOPA_HEALTH
            ),
      m_state(KoopaState::WALKING),
      m_patrolSpeed(DEFAULT_KOOPA_PATROL_SPEED) {

    setFacingDirection(Direction::LEFT);
    initPhysics(world, b2_dynamicBody, KOOPA_SIZE);

    setSprite(KOOPA_TEXTURE_PATH);

    m_animationSystem = std::make_unique<AnimationSystem>();

    const Animation walkAnimation =
    AnimationSystem::createGridAnimation(KOOPA_FRAME_START_X,
                                         KOOPA_FRAME_START_Y,
                                         KOOPA_FRAME_WIDTH,
                                         KOOPA_FRAME_HEIGHT,
                                         KOOPA_WALK_FRAME_COUNT,
                                         KOOPA_WALK_FRAME_DURATION,
                                         true
                                         );

    const Animation shellIdleAnimation =
    AnimationSystem::createGridAnimation(KOOPA_SHELL_FRAME_START_X,
                                         KOOPA_FRAME_START_Y,
                                         KOOPA_FRAME_WIDTH,
                                         KOOPA_FRAME_HEIGHT,
                                         KOOPA_SHELL_FRAME_COUNT,
                                         KOOPA_SHELL_FRAME_DURATION,
                                         false
                                         );

    m_animationSystem->addAnimation(KOOPA_WALK_ANIMATION, walkAnimation);

    m_animationSystem->addAnimation(KOOPA_SHELL_IDLE_ANIMATION,
                                    shellIdleAnimation);

    playAnimation(KOOPA_WALK_ANIMATION);
}

void Koopa::update(float dt) {
    if (m_isFlippedDead) {
        syncPhysics();
        if (m_sprite) {
            m_sprite->setPosition(m_position + sf::Vector2f(m_size.x / 2.f, m_size.y / 2.f));
            m_sprite->setOrigin({8.f, 16.f});
            m_sprite->setScale({2.f, -2.f}); // Upside down flip!
        }
        if (m_position.y > PIT_CLEANUP_Y) {
            markForRemoval();
        }
        return;
    }

    syncPhysics();

    if (m_position.y > PIT_CLEANUP_Y) {
        markForRemoval();
        return;
    }

    if (isDead()) {
        updateAnimation(dt);
        return;
    }

    if (m_state == KoopaState::WALKING) {
        patrol();
    } else if (m_state == KoopaState::SHELL_SLIDING) {
        sf::Vector2f velocity = getVelocity();

        if (getFacingDirection() == Direction::LEFT) {
            velocity.x = -KOOPA_SLIDE_SPEED;
        } else {
            velocity.x = KOOPA_SLIDE_SPEED;
        }

        setVelocity(velocity);
    }

    updateAnimation(dt);
}

void Koopa::onFireHit() {
    if (m_isFlippedDead) return;

    m_isFlippedDead = true;
    setHealth(0);
    SoundManager::getInstance().playSound("kickkill");

    b2Body* body = getBody();
    if (body) {
        for (b2Fixture* fixture = body->GetFixtureList(); fixture != nullptr; fixture = fixture->GetNext()) {
            fixture->SetSensor(true);
        }
        body->SetLinearVelocity(b2Vec2(0.f, -8.f));
    }
}

void Koopa::onStomp() {
    if (m_state == KoopaState::WALKING) {
        m_state = KoopaState::SHELL_IDLE;

        const sf::Vector2f currentVelocity = getVelocity();
        setVelocity({0.f, currentVelocity.y});

        playAnimation(KOOPA_SHELL_IDLE_ANIMATION);
        updateAnimation(0.f);
        return;
    }

    if (m_state == KoopaState::SHELL_SLIDING) {
        m_state = KoopaState::SHELL_IDLE;

        const sf::Vector2f currentVelocity = getVelocity();
        setVelocity({0.f, currentVelocity.y});

        playAnimation(KOOPA_SHELL_IDLE_ANIMATION);
        updateAnimation(0.f);
    }
}

void Koopa::onWallCollision() {
    if (isDead()) {
        return;
    }

    if (m_state != KoopaState::WALKING &&
        m_state != KoopaState::SHELL_SLIDING) {
        return;
    }

    reverseDirection();
}

void Koopa::patrol() {
    if (m_state != KoopaState::WALKING || isDead()) {
        return;
    }

    sf::Vector2f velocity = getVelocity();

    if (getFacingDirection() == Direction::LEFT) {
        velocity.x = -m_patrolSpeed;
    } else {
        velocity.x = m_patrolSpeed;
    }

    setVelocity(velocity);
}

void Koopa::kick(Direction direction) {
    if (m_state != KoopaState::SHELL_IDLE) {
        return;
    }

    m_state = KoopaState::SHELL_SLIDING;
    setFacingDirection(direction);

    sf::Vector2f velocity = getVelocity();

    if (direction == Direction::LEFT) {
        velocity.x = -KOOPA_SLIDE_SPEED;
    } else {
        velocity.x = KOOPA_SLIDE_SPEED;
    }

    setVelocity(velocity);
    EventBus::getInstance().notify(EventType::SHELL_KICKED);
}

bool Koopa::isInShell() const {
    return m_state != KoopaState::WALKING;
}

bool Koopa::isShellSliding() const {
    return m_state == KoopaState::SHELL_SLIDING;
}

KoopaState Koopa::getState() const {
    return m_state;
}

void Koopa::reverseDirection() {
    if (getFacingDirection() == Direction::LEFT) {
        setFacingDirection(Direction::RIGHT);
    } else {
        setFacingDirection(Direction::LEFT);
    }

    sf::Vector2f velocity = getVelocity();

    if (m_state == KoopaState::SHELL_SLIDING) {
        velocity.x =
            getFacingDirection() == Direction::LEFT
                ? -KOOPA_SLIDE_SPEED
                : KOOPA_SLIDE_SPEED;

        setVelocity(velocity);
        return;
    }

    patrol();
}

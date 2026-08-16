/**
 * @file Bowser.cpp
 * @brief Bowser boss implementation - pacing, hops, fire breath, lava sink
 */

#include "entities/Bowser.h"
#include "entities/BowserFire.h"
#include "entities/Hammer.h"

#include <algorithm>
#include <cmath>
#include <random>

#include <box2d/box2d.h>

#include "core/AnimationSystem.h"
#include "core/ScoreRules.h"
#include "core/SpriteFrames_shared.h"

namespace {

const sf::Vector2f BOWSER_SIZE{64.f, 64.f};
constexpr const char* BOWSER_TEXTURE_PATH = "assets/textures/enemies/enemies.png";
constexpr const char* WALK_ANIMATION = "walk";
constexpr const char* FIRE_ANIMATION = "fire";
constexpr const char* THROW_ANIMATION = "throw";
constexpr float BOWSER_FRAME_DURATION = 0.18f;

const std::vector<sf::IntRect>& walkFrames(LevelTheme theme) {
    if (theme == LevelTheme::UNDERWATER) {
        static const std::vector<sf::IntRect> uwFrames = {
            SpriteFrames::legacy::Enemies::Bowser::UW_WALK_CLOSED,
            SpriteFrames::legacy::Enemies::Bowser::UW_WALK_OPEN};
        return uwFrames;
    }
    // Overworld, Underground, and Castle share the UG palette row.
    static const std::vector<sf::IntRect> ugFrames = {
        SpriteFrames::legacy::Enemies::Bowser::UG_WALK_CLOSED,
        SpriteFrames::legacy::Enemies::Bowser::UG_WALK_OPEN};
    return ugFrames;
}

const std::vector<sf::IntRect>& fireFrames(LevelTheme theme) {
    if (theme == LevelTheme::UNDERWATER) {
        static const std::vector<sf::IntRect> frames = {
            SpriteFrames::legacy::Enemies::Bowser::UW_FIRE_POSE1,
            SpriteFrames::legacy::Enemies::Bowser::UW_FIRE_POSE2};
        return frames;
    }
    static const std::vector<sf::IntRect> frames = {
        SpriteFrames::legacy::Enemies::Bowser::UG_FIRE_POSE1,
        SpriteFrames::legacy::Enemies::Bowser::UG_FIRE_POSE2};
    return frames;
}

const std::vector<sf::IntRect>& throwFrames(LevelTheme theme) {
    if (theme == LevelTheme::UNDERWATER) {
        static const std::vector<sf::IntRect> frames = {
            SpriteFrames::legacy::Enemies::Bowser::UW_THROW_LEFT,
            SpriteFrames::legacy::Enemies::Bowser::UW_THROW_RIGHT};
        return frames;
    }
    static const std::vector<sf::IntRect> frames = {
        SpriteFrames::legacy::Enemies::Bowser::UG_THROW_LEFT,
        SpriteFrames::legacy::Enemies::Bowser::UG_THROW_RIGHT};
    return frames;
}

} // namespace

Bowser::Bowser(const sf::Vector2f& position,
               b2World* world,
               LevelTheme theme,
               bool hammerVariant)
    : Enemy(position, BOWSER_SIZE, FIREBALL_HITS_TO_KILL),
      m_hammerVariant(hammerVariant),
      m_world(world),
      m_theme(theme) {
    setFacingDirection(Direction::LEFT);
    initPhysics(world, b2_dynamicBody, BOWSER_SIZE);
    setSprite(BOWSER_TEXTURE_PATH);

    m_animationSystem = std::make_unique<AnimationSystem>();
    m_animationSystem->addAnimation(
        WALK_ANIMATION,
        AnimationSystem::createManualAnimation(walkFrames(theme),
                                                BOWSER_FRAME_DURATION, true));
    m_animationSystem->addAnimation(
        FIRE_ANIMATION,
        AnimationSystem::createManualAnimation(fireFrames(theme),
                                                BOWSER_FRAME_DURATION, true));
    m_animationSystem->addAnimation(
        THROW_ANIMATION,
        AnimationSystem::createManualAnimation(throwFrames(theme),
                                                BOWSER_FRAME_DURATION, true));
    playAnimation(WALK_ANIMATION);
}

void Bowser::update(float dt) {
    if (m_state == State::DIE) {
        syncPhysics();
        if (m_sprite) {
            m_sprite->setPosition(m_position);
            m_sprite->setScale({2.f, 2.f});
        }
        if (m_position.y > DIE_CLEANUP_Y) {
            markForRemoval();
        }
        return;
    }

    syncPhysics();

    if (isDead()) {
        enterDie();
        return;
    }

    m_stateTimer += dt;

    switch (m_state) {
        case State::PATROL: {
            patrol();

            m_attackTimer -= dt;
            if (m_attackTimer <= 0.f) {
                static std::mt19937 rng(std::random_device{}());
                std::uniform_real_distribution<float> nextAttack(1.6f, 3.2f);
                m_attackTimer = nextAttack(rng);

                // Half of the attacks are hops; the rest are fire breath
                // (or a hammer fling for the hammer variant).
                std::uniform_int_distribution<int> coin(0, 1);
                if (coin(rng) == 0) {
                    sf::Vector2f velocity = getVelocity();
                    velocity.y = -HOP_SPEED;
                    setVelocity(velocity);
                } else {
                    m_state = State::BREATHE;
                    m_stateTimer = 0.f;
                    m_fireReleased = false;
                    playAnimation(m_hammerVariant ? THROW_ANIMATION : FIRE_ANIMATION);
                }
            }
            break;
        }
        case State::BREATHE: {
            sf::Vector2f velocity = getVelocity();
            velocity.x = 0.f;
            setVelocity(velocity);

            if (!m_fireReleased && m_stateTimer >= FIRE_RELEASE_TIME) {
                m_fireReleased = true;
                breatheFire();
            }
            if (m_stateTimer >= BREATHE_DURATION) {
                m_state = State::PATROL;
                m_stateTimer = 0.f;
                playAnimation(WALK_ANIMATION);
            }
            break;
        }
        case State::DIE:
            break;
    }

    updateAnimation(dt);

    if (m_sprite) {
        m_sprite->setPosition(m_position);
        m_sprite->setScale({2.f, 2.f});
    }
}

void Bowser::patrol() {
    // Pace the arena around the spawn column, turning at the edges.
    const float centerX = m_position.x + m_size.x / 2.f;
    const float anchorX = m_marioKnown ? m_marioPosition.x : m_position.x;

    if (centerX > anchorX + ARENA_HALF_WIDTH) {
        setFacingDirection(Direction::LEFT);
    } else if (centerX < anchorX - ARENA_HALF_WIDTH) {
        setFacingDirection(Direction::RIGHT);
    }

    // Face the intruder between attacks.
    if (m_marioKnown) {
        setFacingDirection(m_marioPosition.x < m_position.x ? Direction::LEFT
                                                            : Direction::RIGHT);
    }

    sf::Vector2f velocity = getVelocity();
    velocity.x = getFacingDirection() == Direction::LEFT ? -PATROL_SPEED : PATROL_SPEED;
    setVelocity(velocity);
}

void Bowser::onStomp() {
    // Stomping the spiked shell hurts Mario; CollisionManager never routes
    // a stomp here because canBeStomped() is false.
}

void Bowser::onWallCollision() {
    // The arena bounds reverse is handled in patrol().
}

void Bowser::onFireHit() {
    if (m_state == State::DIE) return;

    takeDamage(1);
    if (getHealth() <= 0) {
        enterDie();
    }
}

void Bowser::onStarHit() {
    if (m_state == State::DIE) return;
    setHealth(0);
    enterDie();
}

void Bowser::collapseIntoLava() {
    if (m_state == State::DIE) return;
    setHealth(0);
    enterDie();
}

void Bowser::updateMarioPosition(const sf::Vector2f& marioPos) {
    m_marioPosition = marioPos;
    m_marioKnown = true;
}

std::vector<std::unique_ptr<Entity>> Bowser::takePendingSpawns() {
    std::vector<std::unique_ptr<Entity>> drained;
    drained.swap(m_pending);
    return drained;
}

void Bowser::enterDie() {
    m_state = State::DIE;
    setHealth(0);

    b2Body* body = getBody();
    if (body) {
        for (b2Fixture* fixture = body->GetFixtureList(); fixture != nullptr; fixture = fixture->GetNext()) {
            fixture->SetSensor(true);
        }
        body->SetGravityScale(0.f);
        body->SetLinearVelocity(b2Vec2(0.f, SINK_SPEED / 32.f));
    }
}

void Bowser::breatheFire() {
    if (!m_world) {
        return;
    }

    const Direction direction = getFacingDirection();
    const float spawnX = direction == Direction::LEFT
                             ? m_position.x - 48.f
                             : m_position.x + m_size.x;
    const float spawnY = m_position.y + m_size.y * 0.35f;

    if (m_hammerVariant) {
        m_pending.push_back(
            std::make_unique<Hammer>(sf::Vector2f{spawnX, spawnY}, m_world, direction));
        return;
    }

    m_pending.push_back(std::make_unique<BowserFire>(
        sf::Vector2f{spawnX, spawnY}, m_world, m_theme, direction));
}

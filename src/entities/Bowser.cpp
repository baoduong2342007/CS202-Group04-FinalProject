/**
 * @file Bowser.cpp
 * @brief Bowser boss implementation - pacing, hops, fire breath, lava sink
 */

#include "entities/Bowser.h"
#include "entities/BowserFire.h"
#include "entities/Hammer.h"
#include "entities/FireballExplosion.h"

#include <algorithm>
#include <cmath>
#include <random>

#include <box2d/box2d.h>

#include "core/AnimationSystem.h"
#include "core/ScoreRules.h"
#include "core/SoundManager.h"
#include "core/SpriteFrames_shared.h"

namespace {

const sf::Vector2f BOWSER_SIZE{64.f, 64.f};
constexpr const char* BOWSER_TEXTURE_PATH = "assets/textures/enemies/enemies.png";
constexpr const char* WALK_ANIMATION = "walk";
constexpr const char* FIRE_WINDUP_ANIMATION = "fire_windup";
constexpr const char* FIRE_EXHALE_ANIMATION = "fire_exhale";
constexpr const char* THROW_ANIMATION = "throw";
constexpr float BOWSER_FRAME_DURATION = 0.18f;

const std::vector<sf::IntRect>& walkFrames(LevelTheme theme) {
    if (theme == LevelTheme::UNDERWATER) {
        static const std::vector<sf::IntRect> uwFrames = {
            SpriteFrames::legacy::Enemies::Bowser::UW_WALK_CLOSED,
            SpriteFrames::legacy::Enemies::Bowser::UW_WALK_OPEN};
        return uwFrames;
    }
    if (theme == LevelTheme::OVERWORLD) {
        static const std::vector<sf::IntRect> owFrames = {
            SpriteFrames::legacy::Enemies::Bowser::WALK_CLOSED,
            SpriteFrames::legacy::Enemies::Bowser::WALK_OPEN};
        return owFrames;
    }
    // Underground and Castle share the UG palette row.
    static const std::vector<sf::IntRect> ugFrames = {
        SpriteFrames::legacy::Enemies::Bowser::UG_WALK_CLOSED,
        SpriteFrames::legacy::Enemies::Bowser::UG_WALK_OPEN};
    return ugFrames;
}

const sf::IntRect& fireWindupFrame(LevelTheme theme) {
    if (theme == LevelTheme::UNDERWATER) {
        return SpriteFrames::legacy::Enemies::Bowser::UW_FIRE_POSE1;
    }
    if (theme == LevelTheme::OVERWORLD) {
        return SpriteFrames::legacy::Enemies::Bowser::FIRE_POSE1;
    }
    return SpriteFrames::legacy::Enemies::Bowser::UG_FIRE_POSE1;
}

const sf::IntRect& fireExhaleFrame(LevelTheme theme) {
    if (theme == LevelTheme::UNDERWATER) {
        return SpriteFrames::legacy::Enemies::Bowser::UW_FIRE_POSE2;
    }
    if (theme == LevelTheme::OVERWORLD) {
        return SpriteFrames::legacy::Enemies::Bowser::FIRE_POSE2;
    }
    return SpriteFrames::legacy::Enemies::Bowser::UG_FIRE_POSE2;
}

const std::vector<sf::IntRect>& throwFrames(LevelTheme theme) {
    if (theme == LevelTheme::UNDERWATER) {
        static const std::vector<sf::IntRect> frames = {
            SpriteFrames::legacy::Enemies::Bowser::UW_THROW_LEFT,
            SpriteFrames::legacy::Enemies::Bowser::UW_THROW_RIGHT};
        return frames;
    }
    if (theme == LevelTheme::OVERWORLD) {
        static const std::vector<sf::IntRect> frames = {
            SpriteFrames::legacy::Enemies::Bowser::THROW_LEFT,
            SpriteFrames::legacy::Enemies::Bowser::THROW_RIGHT};
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
      m_spawnOriginX(position.x),
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
        FIRE_WINDUP_ANIMATION,
        AnimationSystem::createManualAnimation({fireWindupFrame(theme)},
                                                BOWSER_FRAME_DURATION, true));
    m_animationSystem->addAnimation(
        FIRE_EXHALE_ANIMATION,
        AnimationSystem::createManualAnimation({fireExhaleFrame(theme)},
                                                BOWSER_FRAME_DURATION, true));
    m_animationSystem->addAnimation(
        THROW_ANIMATION,
        AnimationSystem::createManualAnimation(throwFrames(theme),
                                                BOWSER_FRAME_DURATION, true));
    playAnimation(WALK_ANIMATION);
}

void Bowser::update(float dt) {
    syncPhysics();

    if (m_state == State::DIE) {
        if (m_sprite) {
            m_sprite->setPosition(m_position);
            m_sprite->setScale({2.f, 2.f});
            m_sprite->setColor(sf::Color::White);
        }
        if (m_position.y > DIE_CLEANUP_Y) {
            markForRemoval();
        }
        return;
    }

    if (isDead()) {
        enterDie();
        return;
    }

    // Damage flash timer feedback
    if (m_damageFlashTimer > 0.f) {
        m_damageFlashTimer -= dt;
        if (m_sprite) {
            m_sprite->setColor(sf::Color(255, 120, 120, 240));
        }
    } else {
        if (m_sprite) {
            m_sprite->setColor(sf::Color::White);
        }
    }

    m_stateTimer += dt;

    switch (m_state) {
        case State::PATROL: {
            patrol();

            m_attackTimer -= dt;
            if (m_attackTimer <= 0.f) {
                static std::mt19937 rng(std::random_device{}());
                std::uniform_real_distribution<float> nextAttack(1.8f, 3.2f);
                m_attackTimer = nextAttack(rng);

                // Authentic boss rhythm: 75% fire breath / hammer toss, 25% short hop
                std::uniform_int_distribution<int> attackChoice(0, 3);
                int choice = attackChoice(rng);
                if (choice == 0) {
                    sf::Vector2f velocity = getVelocity();
                    velocity.y = -HOP_SPEED;
                    setVelocity(velocity);
                } else {
                    m_state = State::BREATHE;
                    m_stateTimer = 0.f;
                    m_fireReleased = false;
                    playAnimation(m_hammerVariant ? THROW_ANIMATION : FIRE_WINDUP_ANIMATION);
                }
            }
            break;
        }
        case State::BREATHE: {
            sf::Vector2f velocity = getVelocity();
            velocity.x = 0.f;
            setVelocity(velocity);

            // Phase 1 (Windup): Bowser holds mouth open (FIRE_WINDUP_ANIMATION)
            // Phase 2 (Release): Bowser unleashes flame at FIRE_RELEASE_TIME and switches to FIRE_EXHALE_ANIMATION
            if (!m_fireReleased && m_stateTimer >= FIRE_RELEASE_TIME) {
                m_fireReleased = true;
                breatheFire();
                if (!m_hammerVariant) {
                    playAnimation(FIRE_EXHALE_ANIMATION);
                }
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

    // While airborne in PATROL, keep feet steady on frame 1 instead of frantically running mid-air
    if (m_state == State::PATROL && std::abs(getVelocity().y) > 20.f) {
        // Paused on walk frame while airborne
    } else {
        updateAnimation(dt);
    }

    if (m_sprite) {
        if (getFacingDirection() == Direction::RIGHT) {
            m_sprite->setScale({-2.f, 2.f});
            m_sprite->setOrigin({static_cast<float>(m_sprite->getTextureRect().size.x), 0.f});
        } else {
            m_sprite->setScale({2.f, 2.f});
            m_sprite->setOrigin({0.f, 0.f});
        }
        m_sprite->setPosition(m_position);
    }
}

void Bowser::patrol() {
    // Pace the arena around the spawn column within ARENA_HALF_WIDTH
    const float centerX = m_position.x + m_size.x / 2.f;

    if (centerX > m_spawnOriginX + ARENA_HALF_WIDTH) {
        m_patrolMoveDir = Direction::LEFT;
    } else if (centerX < m_spawnOriginX - ARENA_HALF_WIDTH) {
        m_patrolMoveDir = Direction::RIGHT;
    }

    // Direction swap timer for lively pacing
    m_patrolTurnTimer -= 0.016f;
    if (m_patrolTurnTimer <= 0.f) {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> turnTime(1.5f, 3.0f);
        m_patrolTurnTimer = turnTime(rng);
        m_patrolMoveDir = (m_patrolMoveDir == Direction::LEFT) ? Direction::RIGHT : Direction::LEFT;
    }

    // Face the intruder (Mario)
    if (m_marioKnown) {
        setFacingDirection(m_marioPosition.x < m_position.x ? Direction::LEFT
                                                            : Direction::RIGHT);
    } else {
        setFacingDirection(m_patrolMoveDir);
    }

    sf::Vector2f velocity = getVelocity();
    velocity.x = (m_patrolMoveDir == Direction::LEFT) ? -PATROL_SPEED : PATROL_SPEED;
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
    m_damageFlashTimer = DAMAGE_FLASH_DURATION;

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

    SoundManager::getInstance().playSound("bowser_fall");

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
    const float spawnX = (direction == Direction::LEFT)
                             ? (m_position.x - 44.f)
                             : (m_position.x + m_size.x - 4.f);

    // Authentic SMB1 multi-height fire trajectory:
    // 1. LOW (40%): Spits downward to skim the floor (Y = m_position.y + 38.f), forcing Small and Big Mario to jump.
    // 2. MID (35%): Chest level (Y = m_position.y + 22.f), hitting Big Mario torso and Small Mario head.
    // 3. HIGH (25%): Head level (Y = m_position.y + 8.f), allowing Small Mario to sprint underneath.
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> heightDist(0, 99);
    const int roll = heightDist(rng);

    float offsetY = 8.f; // Default HIGH (Head-level)
    if (roll < 40) {
        offsetY = 38.f;  // LOW (Ground-skimmer)
    } else if (roll < 75) {
        offsetY = 22.f;  // MID (Chest-level)
    } else {
        offsetY = 8.f;   // HIGH (Head-level)
    }

    const float spawnY = m_position.y + offsetY;

    if (m_hammerVariant) {
        m_pending.push_back(
            std::make_unique<Hammer>(sf::Vector2f{spawnX, spawnY}, m_world, direction));
        return;
    }

    SoundManager::getInstance().playSound("bowser_fire");

    m_pending.push_back(std::make_unique<BowserFire>(
        sf::Vector2f{spawnX, spawnY}, m_world, m_theme, direction));
}


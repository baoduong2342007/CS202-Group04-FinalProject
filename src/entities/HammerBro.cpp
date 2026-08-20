/**
 * @file HammerBro.cpp
 * @brief Hammer Bro implementation - hop patrol + hammer bursts
 */

#include "entities/HammerBro.h"
#include "entities/Hammer.h"

#include <algorithm>
#include <cmath>
#include <random>

#include "level/TileMap.h"

#include <box2d/box2d.h>

#include "core/AnimationSystem.h"
#include "core/SpriteFrames_shared.h"

namespace {

const sf::Vector2f HAMMER_BRO_SIZE{32.f, 48.f};
constexpr const char* HAMMER_BRO_TEXTURE_PATH = "assets/textures/enemies/enemies.png";
constexpr const char* WALK_ANIMATION = "walk";
constexpr const char* THROW_ANIMATION = "throw";
constexpr float WALK_FRAME_DURATION = 0.15f;
constexpr float THROW_FRAME_DURATION = 0.15f;

constexpr float TILE_SIZE = 32.f;
constexpr float EDGE_PROBE_OFFSET = 2.f;

const std::vector<sf::IntRect>& walkFrames(LevelTheme theme) {
    switch (theme) {
        case LevelTheme::UNDERGROUND:
        case LevelTheme::CASTLE:
            return SpriteFrames::legacy::Enemies::HammerBroIdle::ugCastleIdleFrames();
        case LevelTheme::UNDERWATER:
            return SpriteFrames::legacy::Enemies::HammerBroIdle::uwIdleFrames();
        case LevelTheme::OVERWORLD:
        default:
            return SpriteFrames::legacy::Enemies::HammerBroIdle::idleFrames();
    }
}

const std::vector<sf::IntRect>& throwFrames(LevelTheme theme) {
    switch (theme) {
        case LevelTheme::UNDERGROUND:
        case LevelTheme::CASTLE:
            return SpriteFrames::legacy::Enemies::HammerBroThrow::ugCastleThrowFrames();
        case LevelTheme::UNDERWATER:
            return SpriteFrames::legacy::Enemies::HammerBroThrow::uwThrowFrames();
        case LevelTheme::OVERWORLD:
        default:
            return SpriteFrames::legacy::Enemies::HammerBroThrow::throwFrames();
    }
}

} // namespace

HammerBro::HammerBro(const sf::Vector2f& position,
                     b2World* world,
                     LevelTheme theme)
    : Enemy(position, HAMMER_BRO_SIZE, 1),
      m_world(world),
      m_theme(theme) {
    setFacingDirection(Direction::LEFT);
    initPhysics(world, b2_dynamicBody, HAMMER_BRO_SIZE);
    setSprite(HAMMER_BRO_TEXTURE_PATH);

    m_animationSystem = std::make_unique<AnimationSystem>();
    m_animationSystem->addAnimation(
        WALK_ANIMATION,
        AnimationSystem::createManualAnimation(walkFrames(theme),
                                                WALK_FRAME_DURATION, true));
    m_animationSystem->addAnimation(
        THROW_ANIMATION,
        AnimationSystem::createManualAnimation(throwFrames(theme),
                                                THROW_FRAME_DURATION, false));
    playAnimation(WALK_ANIMATION);
}

void HammerBro::update(float dt) {
    syncPhysics();

    if (m_isFlippedDead) {
        if (m_sprite) {
            m_sprite->setPosition(m_position + sf::Vector2f(m_size.x / 2.f, m_size.y / 2.f));
            m_sprite->setOrigin({8.f, 12.f});
            m_sprite->setScale({2.f, -2.f});
        }
        if (m_position.y > PIT_CLEANUP_Y) {
            markForRemoval();
        }
        return;
    }

    if (m_rearmJump) {
        m_jumpTimer -= dt;
        if (m_jumpTimer <= 0.f) {
            m_rearmJump = false;
        }
    }

    if (m_position.y > PIT_CLEANUP_Y) {
        markForRemoval();
        return;
    }

    if (isDead()) {
        updateAnimation(dt);
        syncSpriteToFeet();
        return;
    }

    // Occasional hop, like the original shuffling between brick rows.
    if (!m_rearmJump && m_state == State::PATROL &&
        std::abs(getVelocity().y) < 4.f) {
        m_jumpTimer += dt;
        if (m_jumpTimer >= JUMP_INTERVAL) {
            m_jumpTimer = 0.f;
            m_rearmJump = true;
            sf::Vector2f velocity = getVelocity();
            velocity.y = -JUMP_SPEED;
            setVelocity(velocity);
        }
    }

    if (m_state == State::PATROL) {
        patrol();

        m_attackCooldown -= dt;
        if (m_attackCooldown <= 0.f) {
            static std::mt19937 rng(std::random_device{}());
            std::uniform_int_distribution<int> burstSize(1, 3);
            std::uniform_real_distribution<float> nextAttack(1.8f, 3.5f);

            m_hammersLeftInBurst = burstSize(rng);
            m_hammerSpacingTimer = 0.f;
            m_attackCooldown = nextAttack(rng);

            m_state = State::THROW;
            m_stateTimer = 0.f;
            playAnimation(THROW_ANIMATION);
        }
    } else {
        // THROW: hammers leave one by one while the arm swings.
        m_stateTimer += dt;
        m_hammerSpacingTimer += dt;

        if (m_hammersLeftInBurst > 0 && m_hammerSpacingTimer >= HAMMER_SPACING) {
            m_hammerSpacingTimer = 0.f;
            --m_hammersLeftInBurst;
            throwHammer();
        }

        if (m_stateTimer >= THROW_TIME) {
            m_state = State::PATROL;
            m_stateTimer = 0.f;
            playAnimation(WALK_ANIMATION);
        }
    }

    updateAnimation(dt);
    syncSpriteToFeet();
}

void HammerBro::patrol() {
    if (m_state != State::PATROL || isDead()) {
        return;
    }

    if (isApproachingLedge()) {
        reverseDirection();
    }

    sf::Vector2f velocity = getVelocity();
    velocity.x = getFacingDirection() == Direction::LEFT ? -m_patrolSpeed : m_patrolSpeed;
    setVelocity(velocity);
}

void HammerBro::onStomp() {
    if (m_isFlippedDead) return;
    dieFlipped();
}

void HammerBro::onWallCollision() {
    if (isDead() || m_isFlippedDead) {
        return;
    }
    reverseDirection();
}

void HammerBro::onFireHit() {
    if (m_isFlippedDead) return;
    dieFlipped();
}

void HammerBro::setTileMap(const TileMap* tileMap) {
    m_tileMap = tileMap;
}

void HammerBro::updateMarioPosition(const sf::Vector2f& marioPos) {
    m_marioPosition = marioPos;
    m_marioKnown = true;
}

std::vector<std::unique_ptr<Entity>> HammerBro::takePendingSpawns() {
    std::vector<std::unique_ptr<Entity>> drained;
    drained.swap(m_pending);
    return drained;
}

void HammerBro::reverseDirection() {
    setFacingDirection(getFacingDirection() == Direction::LEFT ? Direction::RIGHT
                                                               : Direction::LEFT);
}

bool HammerBro::isApproachingLedge() const {
    if (!m_tileMap) {
        return false;
    }

    const float footY = m_position.y + m_size.y + EDGE_PROBE_OFFSET;
    const float currentX = m_position.x + m_size.x / 2.f;
    const float frontX = getFacingDirection() == Direction::LEFT
                             ? m_position.x - EDGE_PROBE_OFFSET
                             : m_position.x + m_size.x + EDGE_PROBE_OFFSET;

    const int row = static_cast<int>(std::floor(footY / TILE_SIZE));
    const int currentColumn = static_cast<int>(std::floor(currentX / TILE_SIZE));
    const int frontColumn = static_cast<int>(std::floor(frontX / TILE_SIZE));

    const bool hasCurrentGround = m_tileMap->isEnemySupport(currentColumn, row);
    const bool hasFrontGround = m_tileMap->isEnemySupport(frontColumn, row);

    return hasCurrentGround && !hasFrontGround;
}

void HammerBro::throwHammer() {
    if (!m_world) {
        return;
    }

    Direction direction = getFacingDirection();
    if (m_marioKnown) {
        direction = m_marioPosition.x < m_position.x ? Direction::LEFT : Direction::RIGHT;
        setFacingDirection(direction);
    }

    m_pending.push_back(std::make_unique<Hammer>(
        m_position + sf::Vector2f{0.f, -8.f}, m_world, direction));
}

void HammerBro::dieFlipped() {
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

void HammerBro::syncSpriteToFeet() {
    if (!m_sprite) {
        return;
    }

    constexpr float SPRITE_SCALE = 2.f;

    const sf::IntRect rect = m_sprite->getTextureRect();

    if (getFacingDirection() == Direction::RIGHT) {
        m_sprite->setScale({-SPRITE_SCALE, SPRITE_SCALE});
        m_sprite->setOrigin({static_cast<float>(rect.size.x), 0.f});
    } else {
        m_sprite->setScale({SPRITE_SCALE, SPRITE_SCALE});
        m_sprite->setOrigin({0.f, 0.f});
    }

    const float renderedHeight = static_cast<float>(rect.size.y) * SPRITE_SCALE;
    const float footY = m_position.y + m_size.y;

    m_sprite->setPosition({m_position.x, footY - renderedHeight});
}

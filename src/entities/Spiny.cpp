/**
 * @file Spiny.cpp
 * @brief Spiny implementation - Goomba-style walker that punishes stomps
 */

#include "entities/Spiny.h"

#include <cmath>

#include "level/TileMap.h"

#include <box2d/box2d.h>

#include "core/AnimationSystem.h"
#include "core/SpriteFrames_shared.h"

namespace {

const sf::Vector2f SPINY_SIZE{32.f, 32.f};
constexpr const char* SPINY_TEXTURE_PATH = "assets/textures/enemies/enemies.png";
constexpr const char* WALK_ANIMATION = "walk";
constexpr float WALK_FRAME_DURATION = 0.15f;

constexpr float TILE_SIZE = 32.f;
constexpr float EDGE_PROBE_OFFSET = 2.f;

} // namespace

Spiny::Spiny(const sf::Vector2f& position,
             b2World* world,
             LevelTheme theme,
             Direction initialDirection)
    : Enemy(position, SPINY_SIZE, 1) {
    (void)theme; // The red Spiny row is palette-independent.

    setFacingDirection(initialDirection);
    initPhysics(world, b2_dynamicBody, SPINY_SIZE);
    setSprite(SPINY_TEXTURE_PATH);

    m_animationSystem = std::make_unique<AnimationSystem>();
    m_animationSystem->addAnimation(
        WALK_ANIMATION,
        AnimationSystem::createManualAnimation(
            SpriteFrames::legacy::Enemies::RedSpiny::walkFrames(),
            WALK_FRAME_DURATION, true));
    playAnimation(WALK_ANIMATION);
}

void Spiny::update(float dt) {
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

    if (m_position.y > PIT_CLEANUP_Y) {
        markForRemoval();
        return;
    }

    updateNarrowEscapeStatus();

    if (!isDead()) {
        patrol();
    }

    updateAnimation(dt);
    syncSpriteToFeet();
}

void Spiny::patrol() {
    if (isDead()) {
        return;
    }

    // Canonical SMB1: a Spiny walks off ledges like every ground walker.
    if (turnsAtLedge() && isApproachingLedge()) {
        reverseDirection();
    }

    sf::Vector2f velocity = getVelocity();
    velocity.x = getFacingDirection() == Direction::LEFT ? -m_patrolSpeed : m_patrolSpeed;
    setVelocity(velocity);
}

void Spiny::onStomp() {
    // Unstompable: CollisionManager powers Mario down before reaching here.
}

void Spiny::onWallCollision() {
    if (isDead()) {
        return;
    }
    reverseDirection();
}

void Spiny::onFireHit() {
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

void Spiny::setTileMap(const TileMap* tileMap) {
    m_tileMap = tileMap;
}

void Spiny::reverseDirection() {
    setFacingDirection(getFacingDirection() == Direction::LEFT ? Direction::RIGHT
                                                               : Direction::LEFT);
    notifyTurnaround();
}

bool Spiny::isApproachingLedge() const {
    if (isEscapingNarrowRange()) {
        return false;
    }

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

void Spiny::syncSpriteToFeet() {
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

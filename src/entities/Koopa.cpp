/**
 * @file Koopa.cpp
 * @author TV4 (Vy)
 * @brief Implementation of Koopa walking and patrol foundation
 * @note Sprint 5 - walking, stationary shell, and sliding shell states
 */

#include "entities/Koopa.h"
#include "level/TileMap.h"

#include <memory>
#include <cmath>
#include <vector>

#include <box2d/box2d.h>

#include "core/AnimationSystem.h"
#include "core/SpriteFrames_ovw.h"
#include "core/SpriteFrames_udg.h"
#include "core/SpriteFrames_castle.h"
#include "core/SpriteFrames_udw.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "physics/PhysicsEngine.h"

namespace {

constexpr int DEFAULT_KOOPA_HEALTH = 1;
constexpr float DEFAULT_KOOPA_PATROL_SPEED = 50.f;
constexpr float KOOPA_SLIDE_SPEED = 240.f;
constexpr float PIT_CLEANUP_Y = 800.f;
constexpr float EDGE_PROBE_OFFSET = 2.f;

constexpr const char* KOOPA_TEXTURE_PATH = "assets/textures/enemies/enemies.png";

constexpr float TILE_SIZE = 32.f;
constexpr float KOOPA_HEIGHT = 48.f;
constexpr float KOOPA_VERTICAL_SPAWN_OFFSET = KOOPA_HEIGHT - TILE_SIZE;

const sf::Vector2f KOOPA_SIZE{32.f, KOOPA_HEIGHT};

constexpr float KOOPA_SHELL_WIDTH = 32.f;
constexpr float KOOPA_SHELL_HEIGHT = 28.f;

constexpr float KOOPA_WALK_FRAME_DURATION = 0.15f;

constexpr const char* KOOPA_WALK_ANIMATION = "walk";

constexpr float KOOPA_SHELL_FRAME_DURATION = 0.15f;

constexpr const char* KOOPA_SHELL_IDLE_ANIMATION = "shell_idle";

sf::Vector2f alignKoopaToGround(const sf::Vector2f& position) {
    return {position.x, position.y - KOOPA_VERTICAL_SPAWN_OFFSET};
}

} // namespace

Koopa::Koopa(const sf::Vector2f& position, b2World* world, LevelTheme theme)
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

    const bool useUndergroundPalette = theme == LevelTheme::UNDERGROUND ||
                                       theme == LevelTheme::CASTLE;
    const bool useUnderwaterPalette = theme == LevelTheme::UNDERWATER;

    const auto& walkFrames = [theme]() -> const std::vector<sf::IntRect>& {
        switch (theme) {
            case LevelTheme::UNDERGROUND:
                return SpriteFrames::udg::Enemies::Koopa::walkFrames();
            case LevelTheme::CASTLE:
                return SpriteFrames::castle::Enemies::Koopa::walkFrames();
            case LevelTheme::UNDERWATER:
                return SpriteFrames::udw::Enemies::Koopa::walkFrames();
            case LevelTheme::OVERWORLD:
            default:
                return SpriteFrames::ovw::Enemies::Koopa::walkFrames();
        }
    }();

    const Animation walkAnimation =
        AnimationSystem::createManualAnimation(walkFrames,
                                                KOOPA_WALK_FRAME_DURATION,
                                                true);

    const Animation shellIdleAnimation =
        AnimationSystem::createManualAnimation(
            {useUndergroundPalette
                 ? (theme == LevelTheme::CASTLE
                        ? SpriteFrames::castle::Enemies::Koopa::SHELL
                        : SpriteFrames::udg::Enemies::Koopa::SHELL)
                 : (useUnderwaterPalette ? SpriteFrames::udw::Enemies::Koopa::SHELL
                 : SpriteFrames::ovw::Enemies::Koopa::SHELL)},
            KOOPA_SHELL_FRAME_DURATION,
            false);

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

    if (m_pendingShellFixtureRebuild) {
        rebuildShellFixture();
    }

    syncPhysics();

    if (m_position.y > PIT_CLEANUP_Y) {
        markForRemoval();
        return;
    }

    if (isDead()) {
        updateAnimation(dt);
        syncSpriteToFeet();
        return;
    }

    if (m_state == KoopaState::WALKING) {
        patrol();
    } else if (m_state == KoopaState::SHELL_IDLE) {
        // Box2D can leave a small horizontal impulse on the shell after the
        // stomp contact has been solved. An idle shell must stay put until
        // Mario explicitly kicks/pushes it into the sliding state; preserve
        // the vertical velocity so it can still settle onto the ground.
        const sf::Vector2f velocity = getVelocity();
        if (std::abs(velocity.x) > 0.001f) {
            setVelocity({0.f, velocity.y});
        }
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
    syncSpriteToFeet();
}

void Koopa::onFireHit() {
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

void Koopa::onStomp() {
    if (m_state == KoopaState::WALKING) {
        m_state = KoopaState::SHELL_IDLE;
        m_pendingShellFixtureRebuild = true;

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

    if (isApproachingLedge()) {
        reverseDirection();
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
        velocity.x = getFacingDirection() == Direction::LEFT
                ? -KOOPA_SLIDE_SPEED : KOOPA_SLIDE_SPEED;
    } else if (m_state == KoopaState::WALKING) {
        velocity.x = getFacingDirection() == Direction::LEFT
                ? -m_patrolSpeed : m_patrolSpeed;
    }

    setVelocity(velocity);
}

void Koopa::setTileMap(const TileMap* tileMap) {
    m_tileMap = tileMap;
}

bool Koopa::isApproachingLedge() const {
    if (!m_tileMap) {
        return false;
    }

    const float footY = m_position.y + m_size.y + EDGE_PROBE_OFFSET;

    const float currentX = m_position.x + m_size.x / 2.f;

    const float frontX = getFacingDirection() == Direction::LEFT
            ? m_position.x - EDGE_PROBE_OFFSET
            : m_position.x + m_size.x + EDGE_PROBE_OFFSET;

    const int row = static_cast<int>(std::floor(footY / TILE_SIZE));

    const int currentColumn =static_cast<int>(std::floor(currentX / TILE_SIZE));
    const int frontColumn =static_cast<int>(std::floor(frontX / TILE_SIZE));

    const bool hasCurrentGround = m_tileMap->isEnemySupport(currentColumn, row);
    const bool hasFrontGround = m_tileMap->isEnemySupport(frontColumn, row);

    return hasCurrentGround && !hasFrontGround;
}

void Koopa::rebuildShellFixture() {
    if (!m_body) {
        m_pendingShellFixtureRebuild = false;
        return;
    }

    b2World* world = m_body->GetWorld();

    if (!world || world->IsLocked()) {
        return;
    }

    b2Fixture* oldFixture = m_body->GetFixtureList();

    if (!oldFixture) {
        m_pendingShellFixtureRebuild = false;
        return;
    }

    b2FixtureDef fixtureDef;
    fixtureDef.density = oldFixture->GetDensity();
    fixtureDef.friction = oldFixture->GetFriction();
    fixtureDef.restitution = oldFixture->GetRestitution();
    fixtureDef.isSensor = oldFixture->IsSensor();
    fixtureDef.filter = oldFixture->GetFilterData();

    const float halfWidth = PhysicsEngine::pixelsToMeters(KOOPA_SHELL_WIDTH / 2.f);
    const float halfHeight = PhysicsEngine::pixelsToMeters(KOOPA_SHELL_HEIGHT / 2.f);

    const float footOffset = PhysicsEngine::pixelsToMeters((KOOPA_HEIGHT - KOOPA_SHELL_HEIGHT) / 2.f);

    b2PolygonShape shellShape;

    shellShape.SetAsBox(halfWidth, halfHeight,
                        b2Vec2(0.f, footOffset),
                        0.f
                        );

    fixtureDef.shape = &shellShape;

    m_body->DestroyFixture(oldFixture);
    m_body->CreateFixture(&fixtureDef);
    m_body->ResetMassData();

    m_pendingShellFixtureRebuild = false;
}

void Koopa::syncSpriteToFeet() {
    if (!m_sprite) {
        return;
    }

    constexpr float SPRITE_SCALE = 2.f;

    const sf::IntRect rect = m_sprite->getTextureRect();

    const float renderedWidth = static_cast<float>(rect.size.x) * SPRITE_SCALE;
    const float renderedHeight = static_cast<float>(rect.size.y) * SPRITE_SCALE;

    const float footY = m_position.y + KOOPA_HEIGHT;

    if (getFacingDirection() == Direction::LEFT) {
        m_sprite->setScale({SPRITE_SCALE, SPRITE_SCALE});
        m_sprite->setPosition({m_position.x, footY - renderedHeight});
    } else {
        m_sprite->setScale({-SPRITE_SCALE, SPRITE_SCALE});
        m_sprite->setPosition({m_position.x + renderedWidth, footY - renderedHeight});
    }
}

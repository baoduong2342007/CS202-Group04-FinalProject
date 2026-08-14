/**
 * @file CheepCheep.cpp
 * @author TV4 (Vy) & CS202 Team
 * @brief Cheep Cheep fish enemy implementation
 * @note Adheres strictly to NES Super Mario Bros canon mechanics
 */

#include "entities/CheepCheep.h"

#include <cmath>
#include <box2d/box2d.h>

#include "core/AnimationSystem.h"
#include "core/SpriteFrames_ovw.h"
#include "core/SpriteFrames_udg.h"
#include "core/SpriteFrames_castle.h"
#include "core/SpriteFrames_udw.h"
#include "physics/PhysicsEngine.h"

namespace {

constexpr int DEFAULT_CHEEP_HEALTH = 1;
constexpr const char* CHEEP_TEXTURE_PATH = "assets/textures/enemies/enemies.png";
const sf::Vector2f CHEEP_CHEEP_SIZE{32.f, 32.f};
constexpr float SPRITE_SCALE = 2.0f;
constexpr float ORIGINAL_FRAME_SIZE = 16.0f;

} // namespace

CheepCheep::CheepCheep(const sf::Vector2f& position,
                       b2World* world,
                       LevelTheme theme,
                       CheepCheepBehavior behavior,
                       CheepCheepColor color,
                       const sf::Vector2f& initialVelocity)
    : Enemy(position, CHEEP_CHEEP_SIZE, DEFAULT_CHEEP_HEALTH),
      m_behavior(behavior),
      m_color(color),
      m_theme(theme),
      m_baseY(position.y) {
    
    m_swimSpeed = (color == CheepCheepColor::RED) ? DEFAULT_RED_SWIM_SPEED : DEFAULT_GREEN_SWIM_SPEED;
    if (color == CheepCheepColor::RED && behavior == CheepCheepBehavior::SWIMMING) {
        m_waveAmplitude = DEFAULT_WAVE_AMPLITUDE;
        m_waveFrequency = DEFAULT_WAVE_FREQUENCY;
    }

    setFacingDirection(Direction::LEFT);
    initPhysics(world, b2_dynamicBody, CHEEP_CHEEP_SIZE);
    
    if (m_body) {
        if (m_behavior == CheepCheepBehavior::SWIMMING) {
            m_body->SetGravityScale(0.0f);
        } else {
            // Jumping Cheep Cheep: behaves as a dynamic body subject to gravity,
            // with sensor fixtures so it leaps through bridge structures smoothly.
            m_body->SetGravityScale(1.0f);
            for (b2Fixture* fixture = m_body->GetFixtureList(); fixture != nullptr; fixture = fixture->GetNext()) {
                fixture->SetSensor(true);
            }
            if (initialVelocity.x != 0.f || initialVelocity.y != 0.f) {
                m_body->SetLinearVelocity(PhysicsEngine::pixelsToMeters(initialVelocity));
                if (initialVelocity.x > 0.f) {
                    setFacingDirection(Direction::RIGHT);
                } else if (initialVelocity.x < 0.f) {
                    setFacingDirection(Direction::LEFT);
                }
            }
        }
    }

    setSprite(CHEEP_TEXTURE_PATH);
    initAnimations(theme);
    syncSprite();
}

void CheepCheep::initAnimations(LevelTheme theme) {
    const auto& frames = [this, theme]() -> const std::vector<sf::IntRect>& {
        switch (theme) {
            case LevelTheme::UNDERGROUND:
                return SpriteFrames::udg::Enemies::CheepCheep::greenSwimFrames();
            case LevelTheme::CASTLE:
                return SpriteFrames::castle::Enemies::CheepCheep::greenSwimFrames();
            case LevelTheme::UNDERWATER:
                return SpriteFrames::udw::Enemies::CheepCheep::greenSwimFrames();
            case LevelTheme::OVERWORLD:
            default:
                if (m_color == CheepCheepColor::RED) {
                    return SpriteFrames::ovw::Enemies::CheepCheep::redSwimFrames();
                }
                return SpriteFrames::ovw::Enemies::CheepCheep::greenSwimFrames();
        }
    }();

    m_animationSystem->addAnimation("swim", AnimationSystem::createManualAnimation(frames, 0.15f));
    playAnimation("swim");
}

void CheepCheep::setWaveParameters(float amplitude, float frequency) {
    m_waveAmplitude = amplitude;
    m_waveFrequency = frequency;
}

void CheepCheep::setRoute(const sf::Vector2f& start, const sf::Vector2f& end, float speed) {
    m_routeStart = start;
    m_routeEnd = end;
    m_swimSpeed = speed;
    if (std::abs(start.x - end.x) < 1.0f) {
        m_routeAxis = RouteAxis::VERTICAL;
        m_routeDirection = (start.y < end.y) ? 1 : -1;
    } else {
        m_routeAxis = RouteAxis::HORIZONTAL;
        m_routeDirection = (start.x < end.x) ? 1 : -1;
        setFacingDirection((m_routeDirection > 0) ? Direction::RIGHT : Direction::LEFT);
    }
}

void CheepCheep::setTileMap(const TileMap* tileMap) {
    m_tileMap = tileMap;
}

void CheepCheep::update(float dt) {
    // 1. Box2D Physics Sync Order rule: ALWAYS syncPhysics() at the start of update
    syncPhysics();

    if (m_isFlippedDead) {
        syncSprite();
        if (m_position.y > PIT_CLEANUP_Y) {
            markForRemoval();
        }
        return;
    }

    if (m_position.y > PIT_CLEANUP_Y) {
        markForRemoval();
        return;
    }

    if (!isDead()) {
        if (m_behavior == CheepCheepBehavior::SWIMMING) {
            patrol();
            m_waveTime += dt;
        } else {
            // Jumping mode: track facing direction from horizontal velocity
            if (m_velocity.x > 5.f) {
                setFacingDirection(Direction::RIGHT);
            } else if (m_velocity.x < -5.f) {
                setFacingDirection(Direction::LEFT);
            }
        }
    }

    updateAnimation(dt);
    syncSprite();
}

void CheepCheep::patrol() {
    if (m_isFlippedDead || isDead()) {
        return;
    }

    if (m_routeAxis == RouteAxis::VERTICAL) {
        const float minY = std::min(m_routeStart.y, m_routeEnd.y);
        const float maxY = std::max(m_routeStart.y, m_routeEnd.y);

        if (m_position.y <= minY) {
            m_routeDirection = 1;
        } else if (m_position.y >= maxY) {
            m_routeDirection = -1;
        }

        sf::Vector2f vel(0.f, static_cast<float>(m_routeDirection) * m_swimSpeed);
        setVelocity(vel);
        return;
    }

    if (m_routeAxis == RouteAxis::HORIZONTAL) {
        const float minX = std::min(m_routeStart.x, m_routeEnd.x);
        const float maxX = std::max(m_routeStart.x, m_routeEnd.x);

        if (m_position.x <= minX) {
            m_routeDirection = 1;
            setFacingDirection(Direction::RIGHT);
        } else if (m_position.x >= maxX) {
            m_routeDirection = -1;
            setFacingDirection(Direction::LEFT);
        }

        sf::Vector2f vel(static_cast<float>(m_routeDirection) * m_swimSpeed, 0.f);
        setVelocity(vel);
        return;
    }

    sf::Vector2f vel = getVelocity();
    vel.x = (getFacingDirection() == Direction::LEFT) ? -m_swimSpeed : m_swimSpeed;

    if (m_waveAmplitude > 0.0f && m_waveFrequency > 0.0f) {
        // Sinusoidal vertical velocity: dy/dt = A * omega * cos(omega * t)
        vel.y = m_waveAmplitude * m_waveFrequency * std::cos(m_waveFrequency * m_waveTime);
    } else {
        vel.y = 0.f;
    }

    setVelocity(vel);
}

void CheepCheep::onWallCollision() {
    if (m_isFlippedDead || isDead()) {
        return;
    }

    if (m_behavior == CheepCheepBehavior::SWIMMING) {
        reverseDirection();
    }
}

void CheepCheep::reverseDirection() {
    if (getFacingDirection() == Direction::LEFT) {
        setFacingDirection(Direction::RIGHT);
    } else {
        setFacingDirection(Direction::LEFT);
    }
}

void CheepCheep::onFireHit() {
    if (m_isFlippedDead) {
        return;
    }

    m_isFlippedDead = true;
    setHealth(0);

    b2Body* body = getBody();
    if (body) {
        body->SetGravityScale(1.0f);
        for (b2Fixture* fixture = body->GetFixtureList(); fixture != nullptr; fixture = fixture->GetNext()) {
            fixture->SetSensor(true);
        }
        body->SetLinearVelocity(b2Vec2(0.f, -8.f));
    }
}

void CheepCheep::onStomp() {
    if (m_isFlippedDead || !canBeStomped()) {
        return;
    }

    m_isFlippedDead = true;
    setHealth(0);

    b2Body* body = getBody();
    if (body) {
        body->SetGravityScale(1.0f);
        for (b2Fixture* fixture = body->GetFixtureList(); fixture != nullptr; fixture = fixture->GetNext()) {
            fixture->SetSensor(true);
        }
        body->SetLinearVelocity(b2Vec2(0.f, 4.f));
    }
}

void CheepCheep::syncSprite() {
    if (!m_sprite) {
        return;
    }

    const float scaleX = (getFacingDirection() == Direction::RIGHT) ? -SPRITE_SCALE : SPRITE_SCALE;
    const float scaleY = m_isFlippedDead ? -SPRITE_SCALE : SPRITE_SCALE;

    m_sprite->setScale({scaleX, scaleY});

    const float originX = (getFacingDirection() == Direction::RIGHT) ? ORIGINAL_FRAME_SIZE : 0.f;
    const float originY = m_isFlippedDead ? ORIGINAL_FRAME_SIZE : 0.f;

    m_sprite->setOrigin({originX, originY});
    m_sprite->setPosition(m_position);
}

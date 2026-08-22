/**
 * @file Elevator.cpp
 * @author TV1 (Duong)
 * @brief Implementation of the kinematic moving platform / lift entity
 */

#include "entities/Elevator.h"

#include <cmath>

#include <box2d/box2d.h>

#include "core/SpriteFrames_shared.h"
#include "physics/PhysicsEngine.h"

namespace {
constexpr const char* ELEVATOR_TEXTURE_PATH = "assets/textures/items/items_objects.png";
constexpr float ARRIVAL_EPSILON_PIXELS = 2.0f;
} // namespace

Elevator::Elevator(const sf::Vector2f& startPosition,
                   const sf::Vector2f& endPosition,
                   float speedPixelsPerSecond,
                   Axis axis,
                   float pauseSeconds,
                   LevelTheme theme)
    : Entity(startPosition, {64.f, 16.f}),
      m_start(startPosition),
      m_end(endPosition),
      m_axis(axis),
      m_speedPixels(speedPixelsPerSecond),
      m_pauseDuration(pauseSeconds) {
    setSprite(ELEVATOR_TEXTURE_PATH);
    initTheme(theme);
}

void Elevator::initTheme(LevelTheme theme) {
    switch (theme) {
        case LevelTheme::UNDERGROUND:
            m_platformRect = SpriteFrames::shared::Items::UG_PLATFORM_LONG;
            break;
        case LevelTheme::CASTLE:
            m_platformRect = SpriteFrames::shared::Items::CASTLE_PLATFORM_LONG;
            break;
        case LevelTheme::UNDERWATER:
            m_platformRect = SpriteFrames::shared::Items::UW_PLATFORM_LONG;
            break;
        case LevelTheme::OVERWORLD:
        default:
            m_platformRect = SpriteFrames::shared::Items::PLATFORM_LONG;
            break;
    }

    if (m_sprite.has_value()) {
        m_sprite->setTextureRect(m_platformRect);
    }
}

void Elevator::initPhysics(b2World* world, b2BodyType /*type*/, const sf::Vector2f& size, bool isSensor) {
    Entity::initPhysics(world, b2_kinematicBody, size, isSensor);
}

void Elevator::update(float dt) {
    if (!m_body) {
        return;
    }

    if (m_pauseRemaining > 0.0f) {
        m_pauseRemaining -= dt;
        if (m_pauseRemaining <= 0.0f) {
            m_direction = -m_direction;
        }
        m_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
        syncPhysics();
        return;
    }

    const sf::Vector2f current = PhysicsEngine::metersToPixels(m_body->GetPosition());
    const sf::Vector2f target = (m_direction > 0) ? m_end : m_start;
    const sf::Vector2f delta = (target + m_size / 2.0f) - current;

    const float remaining = (m_axis == Axis::VERTICAL) ? std::abs(delta.y)
                                                       : std::abs(delta.x);

    if (remaining <= ARRIVAL_EPSILON_PIXELS) {
        // Snap to the waypoint exactly, then pause or reverse.
        m_body->SetTransform(
            PhysicsEngine::pixelsToMeters(target + m_size / 2.0f),
            m_body->GetAngle());
        m_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
        if (m_pauseDuration > 0.0f) {
            m_pauseRemaining = m_pauseDuration;
        } else {
            m_direction = -m_direction;
        }
        syncPhysics();
        return;
    }

    const float speedMeters = PhysicsEngine::pixelsToMeters(m_speedPixels);
    float velocityX = 0.0f;
    float velocityY = 0.0f;
    if (m_axis == Axis::VERTICAL) {
        velocityY = (delta.y > 0.0f) ? speedMeters : -speedMeters;
    } else {
        velocityX = (delta.x > 0.0f) ? speedMeters : -speedMeters;
    }
    m_body->SetLinearVelocity(b2Vec2(velocityX, velocityY));
    syncPhysics();
}

void Elevator::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (!m_active || !m_sprite.has_value()) {
        return;
    }

    sf::Sprite platform = *m_sprite;
    platform.setTextureRect(m_platformRect);
    platform.setPosition(m_position);
    platform.setScale({m_size.x / static_cast<float>(m_platformRect.size.x),
                       m_size.y / static_cast<float>(m_platformRect.size.y)});
    target.draw(platform, states);
}

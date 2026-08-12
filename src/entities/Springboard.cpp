/**
 * @file Springboard.cpp
 * @author TV1 (Dương)
 * @brief Implementation of Springboard / Trampoline entity
 */

#include "entities/Springboard.h"
#include "entities/Mario.h"
#include "core/SpriteFrames_shared.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "physics/PhysicsEngine.h"
#include <box2d/box2d.h>

namespace {
constexpr const char* SPRINGBOARD_TEXTURE_PATH = "assets/textures/items/items_objects.png";
}

Springboard::Springboard()
    : Entity({0.f, 0.f}, {32.f, 32.f}) {
    setSprite(SPRINGBOARD_TEXTURE_PATH);
    initTheme(LevelTheme::OVERWORLD);
}

Springboard::Springboard(const sf::Vector2f& position, LevelTheme theme)
    : Entity(position, {32.f, 32.f}) {
    setSprite(SPRINGBOARD_TEXTURE_PATH);
    initTheme(theme);
}

void Springboard::initTheme(LevelTheme theme) {
    switch (theme) {
        case LevelTheme::UNDERWATER:
            m_idleRect = SpriteFrames::shared::Items::UW_SPRING_IDLE;
            m_midRect = SpriteFrames::shared::Items::UW_SPRING_MID;
            m_extendedRect = SpriteFrames::shared::Items::UW_SPRING_EXTENDED;
            break;
        case LevelTheme::UNDERGROUND:
            m_idleRect = SpriteFrames::shared::Items::UG_SPRING_IDLE;
            m_midRect = SpriteFrames::shared::Items::UG_SPRING_MID;
            m_extendedRect = SpriteFrames::shared::Items::UG_SPRING_EXTENDED;
            break;
        case LevelTheme::CASTLE:
            m_idleRect = SpriteFrames::shared::Items::CASTLE_SPRING_IDLE;
            m_midRect = SpriteFrames::shared::Items::CASTLE_SPRING_MID;
            m_extendedRect = SpriteFrames::shared::Items::CASTLE_SPRING_EXTENDED;
            break;
        case LevelTheme::OVERWORLD:
        default:
            m_idleRect = SpriteFrames::shared::Items::SPRING_IDLE;
            m_midRect = SpriteFrames::shared::Items::SPRING_MID;
            m_extendedRect = SpriteFrames::shared::Items::SPRING_EXTENDED;
            break;
    }

    if (m_sprite.has_value()) {
        m_sprite->setTextureRect(m_idleRect);
        m_sprite->setScale({2.0f, 2.0f});
        m_sprite->setOrigin({0.f, static_cast<float>(m_idleRect.size.y)});
    }
}

void Springboard::initPhysics(b2World* world, b2BodyType /*type*/, const sf::Vector2f& size, bool isSensor) {
    Entity::initPhysics(world, b2_staticBody, size, isSensor);
}

void Springboard::triggerSpring(Mario& mario, bool isHoldingJump) {
    if (m_state != State::IDLE) {
        return;
    }

    m_state = State::COMPRESSING;
    m_stateTimer = 0.10f; // 0.1s compression phase

    float launchSpeed = isHoldingJump ? BOUNCE_SUPER_SPEED : BOUNCE_NORMAL_SPEED;

    b2Body* body = mario.getBody();
    if (body) {
        b2Vec2 vel = body->GetLinearVelocity();
        float targetVyMeters = -PhysicsEngine::pixelsToMeters(launchSpeed);
        body->SetLinearVelocity(b2Vec2(vel.x, targetVyMeters));
        mario.clearGroundedState();
    }

    EventBus::getInstance().notify(EventType::PLAYER_JUMPED);
}

void Springboard::update(float dt) {
    if (m_state != State::IDLE) {
        m_stateTimer -= dt;

        if (m_state == State::COMPRESSING) {
            if (m_stateTimer <= 0.f) {
                m_state = State::LAUNCHING;
                m_stateTimer = 0.12f; // 0.12s extended launch phase
            }
        } else if (m_state == State::LAUNCHING) {
            if (m_stateTimer <= 0.f) {
                m_state = State::COOLDOWN;
                m_stateTimer = 0.08f; // 0.08s return to idle
            }
        } else if (m_state == State::COOLDOWN) {
            if (m_stateTimer <= 0.f) {
                m_state = State::IDLE;
            }
        }
    }

    sf::IntRect currentRect;
    switch (m_state) {
        case State::COMPRESSING:
            currentRect = m_midRect;
            break;
        case State::LAUNCHING:
            currentRect = m_extendedRect;
            break;
        case State::COOLDOWN:
            currentRect = m_midRect;
            break;
        case State::IDLE:
        default:
            currentRect = m_idleRect;
            break;
    }

    if (m_sprite.has_value()) {
        m_sprite->setTextureRect(currentRect);
        m_sprite->setScale({2.0f, 2.0f});
        m_sprite->setOrigin({0.f, static_cast<float>(currentRect.size.y)});
    }
}

void Springboard::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (!m_active || !m_sprite.has_value()) {
        return;
    }

    sf::Sprite drawSprite = *m_sprite;
    // Draw relative to the bottom of the 32x32 tile cell so it expands upwards
    drawSprite.setPosition({m_position.x, m_position.y + m_size.y});
    target.draw(drawSprite, states);
}

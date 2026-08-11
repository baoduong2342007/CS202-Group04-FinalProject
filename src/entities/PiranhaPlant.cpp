/**
 * @file PiranhaPlant.cpp
 * @author TV4 & TV1
 * @brief Piranha Plant pipe enemy implementation with sensor physics and emergence state machine
 */

#include "entities/PiranhaPlant.h"
#include <cmath>
#include "core/AnimationSystem.h"
#include "core/SpriteFrames_shared.h"
#include "physics/PhysicsEngine.h"

namespace {
const sf::Vector2f PIRANHA_SIZE(32.f, 48.f);
const sf::Vector2f PIRANHA_PHYSICS_SIZE(28.f, 44.f);
constexpr const char* ENEMIES_TEXTURE_PATH = "assets/textures/enemies/enemies.png";
} // namespace

PiranhaPlant::PiranhaPlant(const sf::Vector2f& position,
                           b2World* world,
                           LevelTheme theme)
    : Enemy(sf::Vector2f(position.x + 16.f, position.y + 16.f), PIRANHA_SIZE, 1),
      m_basePosition(position.x + 16.f, position.y + 16.f) {
    setSprite(ENEMIES_TEXTURE_PATH);

    if (world) {
        initPhysics(world, b2_kinematicBody, PIRANHA_PHYSICS_SIZE, true);
    }

    initAnimations(theme);
}

void PiranhaPlant::initAnimations(LevelTheme theme) {
    sf::IntRect closedRect;
    sf::IntRect openRect;

    switch (theme) {
        case LevelTheme::UNDERGROUND:
        case LevelTheme::CASTLE:
            closedRect = SpriteFrames::legacy::Enemies::PiranhaPlant::UG_CASTLE_CLOSED;
            openRect = SpriteFrames::legacy::Enemies::PiranhaPlant::UG_CASTLE_OPEN;
            break;
        case LevelTheme::OVERWORLD:
        default:
            closedRect = SpriteFrames::legacy::Enemies::PiranhaPlant::CLOSED;
            openRect = SpriteFrames::legacy::Enemies::PiranhaPlant::OPEN;
            break;
    }

    m_animationSystem->addAnimation("bite",
        AnimationSystem::createManualAnimation(
            std::vector<sf::IntRect>{closedRect, openRect}, 0.15f));
    playAnimation("bite");
}

void PiranhaPlant::update(float dt) {
    if (!isActive()) return;

    if (m_isFlippedDead) {
        m_flipTimer += dt;
        m_position.y += 200.f * dt;
        if (m_sprite) {
            m_sprite->setPosition(m_position);
            m_sprite->setScale({2.f, -2.f}); // Flipped upside down
        }
        if (m_flipTimer >= 1.0f) {
            markForRemoval();
        }
        return;
    }

    syncPhysics(); // CRITICAL: Sync first!
    updateAnimation(dt);
    updateState(dt);

    if (m_body) {
        m_body->SetTransform(PhysicsEngine::pixelsToMeters(m_position + m_size / 2.0f), 0.0f);
    }

    if (m_sprite) {
        m_sprite->setScale({2.f, 2.f});
        m_sprite->setPosition(m_position);
    }
}

void PiranhaPlant::updateState(float dt) {
    switch (m_state) {
        case State::EMERGING: {
            m_position.y -= MOVE_SPEED * dt;
            float topY = m_basePosition.y - TRAVEL_DISTANCE;
            if (m_position.y <= topY) {
                m_position.y = topY;
                m_state = State::WAITING_TOP;
                m_stateTimer = 0.f;
            }
            break;
        }
        case State::WAITING_TOP: {
            m_stateTimer += dt;
            if (m_stateTimer >= WAIT_TIME) {
                m_state = State::RETRACTING;
                m_stateTimer = 0.f;
            }
            break;
        }
        case State::RETRACTING: {
            m_position.y += MOVE_SPEED * dt;
            if (m_position.y >= m_basePosition.y) {
                m_position.y = m_basePosition.y;
                m_state = State::WAITING_BOTTOM;
                m_stateTimer = 0.f;
            }
            break;
        }
        case State::WAITING_BOTTOM: {
            m_stateTimer += dt;
            if (m_stateTimer >= WAIT_TIME) {
                if (isMarioNearPipe()) {
                    m_stateTimer = WAIT_TIME - 0.5f;
                } else {
                    m_state = State::EMERGING;
                    m_stateTimer = 0.f;
                }
            }
            break;
        }
    }
}

void PiranhaPlant::updateMarioProximity(const sf::Vector2f& marioPos) {
    float plantCenterX = m_basePosition.x + m_size.x / 2.0f;
    float marioCenterX = marioPos.x + 16.f;
    m_isMarioNear = (std::abs(marioCenterX - plantCenterX) < PROXIMITY_RADIUS);
}

void PiranhaPlant::patrol() {
}

void PiranhaPlant::onStomp() {
}

void PiranhaPlant::onWallCollision() {
}

void PiranhaPlant::onFireHit() {
    if (!tryCommitDefeat()) return;

    m_isFlippedDead = true;
    m_flipTimer = 0.f;

    if (m_body) {
        m_body->SetEnabled(false);
    }
}

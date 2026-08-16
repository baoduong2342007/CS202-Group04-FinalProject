/**
 * @file Mushroom.cpp
 * @author TV5 (Truyền)
 * @brief Super Mushroom — horizontal patrol and body-tier growth
 * @note Week 5 — power-up logic queries Mario's state directly (no EventBus payload)
 */

#include "items/Mushroom.h"

#include "entities/Mario.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "core/AnimationSystem.h"
#include "core/SpriteFrames_ovw.h"
#include "core/SpriteFrames_udg.h"
#include "core/SpriteFrames_castle.h"
#include "core/SpriteFrames_udw.h"
#include "core/ScoreRules.h"

namespace {
constexpr float MUSHROOM_WIDTH  = 32.f;
constexpr float MUSHROOM_HEIGHT = 32.f;
constexpr float DEFAULT_MUSHROOM_SPEED = 60.f;
constexpr const char* MUSHROOM_TEXTURE_PATH =
    "assets/textures/items/items_objects.png";

sf::IntRect getMushroomFrame(MushroomType type, LevelTheme theme) {
    if (type == MushroomType::ONE_UP) {
        switch (theme) {
            case LevelTheme::UNDERGROUND: return SpriteFrames::udg::Items::ONE_UP_MUSHROOM;
            case LevelTheme::CASTLE:      return SpriteFrames::castle::Items::ONE_UP_MUSHROOM;
            case LevelTheme::UNDERWATER:  return SpriteFrames::udw::Items::ONE_UP_MUSHROOM;
            case LevelTheme::OVERWORLD:
            default:                      return SpriteFrames::ovw::Items::ONE_UP_MUSHROOM;
        }
    } else {
        switch (theme) {
            case LevelTheme::UNDERGROUND: return SpriteFrames::udg::Items::SUPER_MUSHROOM;
            case LevelTheme::CASTLE:      return SpriteFrames::castle::Items::SUPER_MUSHROOM;
            case LevelTheme::UNDERWATER:  return SpriteFrames::udw::Items::SUPER_MUSHROOM;
            case LevelTheme::OVERWORLD:
            default:                      return SpriteFrames::ovw::Items::SUPER_MUSHROOM;
        }
    }
}
} // namespace

Mushroom::Mushroom(MushroomType type, LevelTheme theme)
    : Item(sf::Vector2f(0.f, 0.f), sf::Vector2f(MUSHROOM_WIDTH, MUSHROOM_HEIGHT)),
      m_type(type),
      m_theme(theme),
      m_patrolSpeed(DEFAULT_MUSHROOM_SPEED),
      m_patrolDirection(1) {
    initPhysics(nullptr, b2_dynamicBody, sf::Vector2f(MUSHROOM_WIDTH, MUSHROOM_HEIGHT));
    setSprite(MUSHROOM_TEXTURE_PATH);
    sf::IntRect frame = getMushroomFrame(m_type, m_theme);
    m_animationSystem->addAnimation("idle",
        AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{frame}, 1.f));
    playAnimation("idle");
}

Mushroom::Mushroom(const sf::Vector2f& position, b2World* world, MushroomType type, LevelTheme theme)
    : Item(position, sf::Vector2f(MUSHROOM_WIDTH, MUSHROOM_HEIGHT)),
      m_type(type),
      m_theme(theme),
      m_patrolSpeed(DEFAULT_MUSHROOM_SPEED),
      m_patrolDirection(1) {
    initPhysics(world, b2_dynamicBody, sf::Vector2f(MUSHROOM_WIDTH, MUSHROOM_HEIGHT));
    setSprite(MUSHROOM_TEXTURE_PATH);
    sf::IntRect frame = getMushroomFrame(m_type, m_theme);
    m_animationSystem->addAnimation("idle",
        AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{frame}, 1.f));
    playAnimation("idle");
}

void Mushroom::update(float dt) {
    updateCollectibleDelay(dt);

    // Sync visual position with Box2D body first, then apply patrol velocity
    syncPhysics();
    updateAnimation(dt);

    // Auto-reverse if stuck against a wall/block seam while patrolling
    if (m_collectibleDelay <= 0.f && std::abs(m_velocity.x) < 1.0f && m_patrolSpeed > 0.f) {
        m_stuckTimer += dt;
        if (m_stuckTimer >= 0.1f) {
            onWallCollision();
            m_stuckTimer = 0.f;
        }
    } else {
        m_stuckTimer = 0.f;
    }

    patrol();
}

void Mushroom::patrol() {
    sf::Vector2f velocity = getVelocity();
    velocity.x = static_cast<float>(m_patrolDirection) * m_patrolSpeed;
    setVelocity(velocity);
}

void Mushroom::onWallCollision() {
    m_patrolDirection = -m_patrolDirection;
    patrol();
}

void Mushroom::onCollect(Mario& mario) {
    if (m_isCollected) {
        return;
    }

    m_isCollected = true;

    bool eventPublished = false;
    if (m_type == MushroomType::ONE_UP) {
        mario.addLife(1);
        ScoreRules::award(mario, ScoreEvent::POWER_UP_COLLECTED);
        EventBus::getInstance().notify(EventType::ONE_UP_COLLECTED);
        return;
    } else {
        MarioState targetState = mario.getMarioState();
        if (targetState == MarioState::SMALL) {
            targetState = MarioState::SUPER;
        } else if (targetState == MarioState::FIRE_SMALL) {
            targetState = MarioState::FIRE_SUPER;
        }

        if (targetState != mario.getMarioState()) {
            mario.powerUp(targetState);
            eventPublished = true;
        }
        ScoreRules::award(mario, ScoreEvent::POWER_UP_COLLECTED);
    }

    // Mario::powerUp() owns the event for a state-changing pickup. For a
    // Mushroom collected while Mario is already powered up, still emit one
    // pickup event, but never emit two events for the same item.
    if (!eventPublished) {
        EventBus::getInstance().notify(EventType::PLAYER_POWER_UP);
    }
}

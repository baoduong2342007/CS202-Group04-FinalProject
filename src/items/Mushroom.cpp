/**
 * @file Mushroom.cpp
 * @author TV5 (Truyền)
 * @brief Super Mushroom — horizontal patrol, grows Mario from SMALL to SUPER
 * @note Week 5 — power-up logic queries Mario's state directly (no EventBus payload)
 */

#include "items/Mushroom.h"

#include "entities/Mario.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "core/AnimationSystem.h"
#include "core/SpriteFrames.h"
#include "core/ScoreRules.h"

namespace {
constexpr float MUSHROOM_WIDTH  = 32.f;
constexpr float MUSHROOM_HEIGHT = 32.f;
constexpr float DEFAULT_MUSHROOM_SPEED = 60.f;
constexpr const char* MUSHROOM_TEXTURE_PATH =
    "assets/textures/items/items_objects.png";
} // namespace

Mushroom::Mushroom(MushroomType type)
    : Item(sf::Vector2f(0.f, 0.f), sf::Vector2f(MUSHROOM_WIDTH, MUSHROOM_HEIGHT)),
      m_type(type),
      m_patrolSpeed(DEFAULT_MUSHROOM_SPEED),
      m_patrolDirection(1) {
    initPhysics(nullptr, b2_dynamicBody, sf::Vector2f(MUSHROOM_WIDTH, MUSHROOM_HEIGHT));
    setSprite(MUSHROOM_TEXTURE_PATH);
    sf::IntRect frame = (m_type == MushroomType::ONE_UP) ? SpriteFrames::Items::ONE_UP_MUSHROOM : SpriteFrames::Items::SUPER_MUSHROOM;
    m_animationSystem->addAnimation("idle",
        AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{frame}, 1.f));
    playAnimation("idle");
}

Mushroom::Mushroom(const sf::Vector2f& position, b2World* world, MushroomType type)
    : Item(position, sf::Vector2f(MUSHROOM_WIDTH, MUSHROOM_HEIGHT)),
      m_type(type),
      m_patrolSpeed(DEFAULT_MUSHROOM_SPEED),
      m_patrolDirection(1) {
    initPhysics(world, b2_dynamicBody, sf::Vector2f(MUSHROOM_WIDTH, MUSHROOM_HEIGHT));
    setSprite(MUSHROOM_TEXTURE_PATH);
    sf::IntRect frame = (m_type == MushroomType::ONE_UP) ? SpriteFrames::Items::ONE_UP_MUSHROOM : SpriteFrames::Items::SUPER_MUSHROOM;
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
        if (mario.getMarioState() == MarioState::SMALL) {
            mario.powerUp(MarioState::SUPER);
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

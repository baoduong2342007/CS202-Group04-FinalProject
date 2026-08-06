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

namespace {
constexpr float MUSHROOM_WIDTH  = 32.f;
constexpr float MUSHROOM_HEIGHT = 32.f;
constexpr float DEFAULT_MUSHROOM_SPEED = 60.f;
constexpr int   MUSHROOM_SCORE_VALUE  = 1000;

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
    // Sync visual position with Box2D body first, then apply patrol velocity
    syncPhysics();
    updateAnimation(dt);
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

    if (m_type == MushroomType::ONE_UP) {
        mario.addLife(1);
        mario.addScore(1000);
        EventBus::getInstance().notify(EventType::PLAYER_POWER_UP);
    } else {
        if (mario.getMarioState() == MarioState::SMALL) {
            mario.powerUp(MarioState::SUPER);
        }
        mario.addScore(MUSHROOM_SCORE_VALUE);
        EventBus::getInstance().notify(EventType::PLAYER_POWER_UP);
    }
}

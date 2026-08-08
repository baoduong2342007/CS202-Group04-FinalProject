/**
 * @file Star.cpp
 * @author TV5 (Truyền)
 * @brief Starman — bouncing item that grants temporary invincibility
 * @note Week 5 — publishes PLAYER_POWER_UP on collect; invincibility is time-limited.
 *       The item queries Mario's state directly (no EventBus payload required).
 */

#include "items/Star.h"

#include "entities/Mario.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "core/AnimationSystem.h"
#include "core/SpriteFrames.h"

namespace {
constexpr float STAR_WIDTH  = 32.f;
constexpr float STAR_HEIGHT = 32.f;
constexpr float DEFAULT_STAR_SPEED = 80.f;
constexpr float DEFAULT_STAR_BOUNCE_VELOCITY = -300.f;
constexpr float STAR_INVINCIBILITY_DURATION = 10.f;
constexpr int   STAR_SCORE_VALUE  = 1000;

constexpr const char* STAR_TEXTURE_PATH =
    "assets/textures/items/items_objects.png";
} // namespace

Star::Star()
    : Item(sf::Vector2f(0.f, 0.f), sf::Vector2f(STAR_WIDTH, STAR_HEIGHT)),
      m_patrolSpeed(DEFAULT_STAR_SPEED),
      m_bounceVelocity(DEFAULT_STAR_BOUNCE_VELOCITY),
      m_patrolDirection(1) {
    initPhysics(nullptr, b2_dynamicBody, sf::Vector2f(STAR_WIDTH, STAR_HEIGHT));
    setSprite(STAR_TEXTURE_PATH);
    m_animationSystem->addAnimation("idle",
        AnimationSystem::createManualAnimation(SpriteFrames::Items::starFrames(), 0.1f));
    playAnimation("idle");
}

Star::Star(const sf::Vector2f& position, b2World* world)
    : Item(position, sf::Vector2f(STAR_WIDTH, STAR_HEIGHT)),
      m_patrolSpeed(DEFAULT_STAR_SPEED),
      m_bounceVelocity(DEFAULT_STAR_BOUNCE_VELOCITY),
      m_patrolDirection(1) {
    initPhysics(world, b2_dynamicBody, sf::Vector2f(STAR_WIDTH, STAR_HEIGHT));
    setSprite(STAR_TEXTURE_PATH);
    m_animationSystem->addAnimation("idle",
        AnimationSystem::createManualAnimation(SpriteFrames::Items::starFrames(), 0.1f));
    playAnimation("idle");
}

void Star::update(float dt) {
    updateCollectibleDelay(dt);

    // Sync visual position with Box2D body first, then apply patrol velocity
    syncPhysics();
    updateAnimation(dt);
    patrol();
}

void Star::patrol() {
    sf::Vector2f velocity = getVelocity();
    velocity.x = static_cast<float>(m_patrolDirection) * m_patrolSpeed;
    setVelocity(velocity);
}

void Star::onWallCollision() {
    m_patrolDirection = -m_patrolDirection;
    patrol();
}

void Star::onGroundCollision() {
    // Apply upward bounce velocity when hitting the ground
    sf::Vector2f velocity = getVelocity();
    velocity.y = m_bounceVelocity;
    setVelocity(velocity);
}

void Star::onCollect(Mario& mario) {
    if (m_isCollected) {
        return;
    }

    m_isCollected = true;

    // Grant temporary invincibility regardless of current state
    mario.setStarInvincible(STAR_INVINCIBILITY_DURATION);

    mario.addScore(STAR_SCORE_VALUE);

    // Notify observers (SoundManager plays powerup.wav, HUD updates score)
    EventBus::getInstance().notify(EventType::PLAYER_POWER_UP);
}

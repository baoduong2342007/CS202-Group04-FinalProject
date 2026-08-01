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

namespace {
constexpr float MUSHROOM_WIDTH  = 32.f;
constexpr float MUSHROOM_HEIGHT = 32.f;
constexpr float DEFAULT_MUSHROOM_SPEED = 60.f;
constexpr int   MUSHROOM_SCORE_VALUE  = 1000;

constexpr const char* MUSHROOM_TEXTURE_PATH =
    "assets/textures/items/mushroom.png";
} // namespace

Mushroom::Mushroom()
    : Item(sf::Vector2f(0.f, 0.f), sf::Vector2f(MUSHROOM_WIDTH, MUSHROOM_HEIGHT)),
      m_patrolSpeed(DEFAULT_MUSHROOM_SPEED),
      m_patrolDirection(1) {
    initPhysics(b2_dynamicBody, sf::Vector2f(MUSHROOM_WIDTH, MUSHROOM_HEIGHT));
    setSprite(MUSHROOM_TEXTURE_PATH);
}

Mushroom::Mushroom(const sf::Vector2f& position)
    : Item(position, sf::Vector2f(MUSHROOM_WIDTH, MUSHROOM_HEIGHT)),
      m_patrolSpeed(DEFAULT_MUSHROOM_SPEED),
      m_patrolDirection(1) {
    initPhysics(b2_dynamicBody, sf::Vector2f(MUSHROOM_WIDTH, MUSHROOM_HEIGHT));
    setSprite(MUSHROOM_TEXTURE_PATH);
}

void Mushroom::update(float dt) {
    (void)dt;

    // Sync visual position with Box2D body first, then apply patrol velocity
    syncPhysics();
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

    // Query Mario's current state directly — no EventBus payload required.
    // Only SMALL Mario benefits from the mushroom; SUPER/FIRE Mario just gets score.
    if (mario.getMarioState() == MarioState::SMALL) {
        mario.powerUp(MarioState::SUPER);
    }

    mario.addScore(MUSHROOM_SCORE_VALUE);
    markForRemoval();

    // Notify observers (SoundManager plays powerup.wav, HUD updates score)
    EventBus::getInstance().notify(EventType::PLAYER_POWER_UP);
}

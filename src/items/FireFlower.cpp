/**
 * @file FireFlower.cpp
 * @author TV5 (Truyền)
 * @brief Fire Flower — stationary item that turns Mario into FIRE state
 * @note Week 5 — power-up logic queries Mario's state directly (no EventBus payload)
 */

#include "items/FireFlower.h"

#include "entities/Mario.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"

namespace {
constexpr float FIRE_FLOWER_WIDTH  = 32.f;
constexpr float FIRE_FLOWER_HEIGHT = 32.f;

constexpr const char* FIRE_FLOWER_TEXTURE_PATH =
    "assets/textures/items/fireflower.png";
} // namespace

FireFlower::FireFlower()
    : Item(sf::Vector2f(0.f, 0.f), sf::Vector2f(FIRE_FLOWER_WIDTH, FIRE_FLOWER_HEIGHT)) {
    initPhysics(b2_staticBody, sf::Vector2f(FIRE_FLOWER_WIDTH, FIRE_FLOWER_HEIGHT), true);
    setSprite(FIRE_FLOWER_TEXTURE_PATH);
}

FireFlower::FireFlower(const sf::Vector2f& position)
    : Item(position, sf::Vector2f(FIRE_FLOWER_WIDTH, FIRE_FLOWER_HEIGHT)) {
    initPhysics(b2_staticBody, sf::Vector2f(FIRE_FLOWER_WIDTH, FIRE_FLOWER_HEIGHT), true);
    setSprite(FIRE_FLOWER_TEXTURE_PATH);
}

void FireFlower::update(float dt) {
    (void)dt;

    // Stationary item — only sync physics position for rendering
    syncPhysics();
}

void FireFlower::onCollect(Mario& mario) {
    if (m_isCollected) {
        return;
    }

    m_isCollected = true;

    // Query Mario's current state directly — no EventBus payload required.
    // SMALL Mario becomes FIRE directly; SUPER Mario also becomes FIRE.
    // (In classic SMB, a mushroom would be spawned for SMALL Mario instead,
    //  but here we apply the fire state for simplicity per the design doc.)
    if (mario.getMarioState() != MarioState::FIRE) {
        mario.powerUp(MarioState::FIRE);
    }

    mario.addScore(FIRE_FLOWER_SCORE_VALUE);

    // Notify observers (SoundManager plays powerup.wav, HUD updates score)
    EventBus::getInstance().notify(EventType::PLAYER_POWER_UP);
}

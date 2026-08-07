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
#include "core/AnimationSystem.h"
#include "core/SpriteFrames.h"

namespace {
constexpr float FIRE_FLOWER_WIDTH  = 32.f;
constexpr float FIRE_FLOWER_HEIGHT = 32.f;

constexpr const char* FIRE_FLOWER_TEXTURE_PATH =
    "assets/textures/items/items_objects.png";
} // namespace

FireFlower::FireFlower()
    : Item(sf::Vector2f(0.f, 0.f), sf::Vector2f(FIRE_FLOWER_WIDTH, FIRE_FLOWER_HEIGHT)) {
    initPhysics(nullptr, b2_staticBody, sf::Vector2f(FIRE_FLOWER_WIDTH, FIRE_FLOWER_HEIGHT), true);
    setSprite(FIRE_FLOWER_TEXTURE_PATH);
    m_animationSystem->addAnimation("idle",
        AnimationSystem::createManualAnimation(SpriteFrames::Items::fireFlowerFrames(), 0.15f));
    playAnimation("idle");
}

FireFlower::FireFlower(const sf::Vector2f& position, b2World* world)
    : Item(position, sf::Vector2f(FIRE_FLOWER_WIDTH, FIRE_FLOWER_HEIGHT)) {
    initPhysics(world, b2_staticBody, sf::Vector2f(FIRE_FLOWER_WIDTH, FIRE_FLOWER_HEIGHT), true);
    setSprite(FIRE_FLOWER_TEXTURE_PATH);
    m_animationSystem->addAnimation("idle",
        AnimationSystem::createManualAnimation(SpriteFrames::Items::fireFlowerFrames(), 0.15f));
    playAnimation("idle");
}

void FireFlower::update(float dt) {
    updateCollectibleDelay(dt);

    // Stationary item — sync physics position and update animation clip
    syncPhysics();
    updateAnimation(dt);
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
    const bool stateChanged = mario.getMarioState() != MarioState::FIRE;
    if (stateChanged) {
        mario.powerUp(MarioState::FIRE);
    }

    mario.addScore(FIRE_FLOWER_SCORE_VALUE);

    // Mario::powerUp() publishes the event when the state changes. If Mario
    // is already FIRE, publish exactly one event for this pickup here.
    if (!stateChanged) {
        EventBus::getInstance().notify(EventType::PLAYER_POWER_UP);
    }
}

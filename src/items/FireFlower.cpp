/**
 * @file FireFlower.cpp
 * @author TV5 (Truyen)
 * @brief Fire Flower — stationary item that applies FIRE to the current body tier
 * @note Small Mario becomes FIRE_SMALL; Super Mario becomes FIRE_SUPER
 */

#include "items/FireFlower.h"

#include "entities/Mario.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "core/AnimationSystem.h"
#include "core/SpriteFrames_shared.h"
#include "core/ScoreRules.h"

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
        AnimationSystem::createManualAnimation(SpriteFrames::shared::Items::fireFlowerFrames(), 0.15f));
    playAnimation("idle");
}

FireFlower::FireFlower(const sf::Vector2f& position, b2World* world)
    : Item(position, sf::Vector2f(FIRE_FLOWER_WIDTH, FIRE_FLOWER_HEIGHT)) {
    initPhysics(world, b2_staticBody, sf::Vector2f(FIRE_FLOWER_WIDTH, FIRE_FLOWER_HEIGHT), true);
    setSprite(FIRE_FLOWER_TEXTURE_PATH);
    m_animationSystem->addAnimation("idle",
        AnimationSystem::createManualAnimation(SpriteFrames::shared::Items::fireFlowerFrames(), 0.15f));
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

    const MarioState currentState = mario.getMarioState();
    MarioState targetState = currentState;
    if (currentState == MarioState::SMALL) {
        targetState = MarioState::FIRE_SMALL;
    } else if (currentState == MarioState::SUPER) {
        targetState = MarioState::FIRE_SUPER;
    }

    const bool stateChanged = (currentState != targetState);
    if (stateChanged) {
        mario.powerUp(targetState);
    }

    ScoreRules::award(mario, ScoreEvent::POWER_UP_COLLECTED);

    if (!stateChanged) {
        EventBus::getInstance().notify(EventType::PLAYER_POWER_UP);
    }
}

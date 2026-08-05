/**
 * @file Coin.cpp
 * @author TV5 (Truyền)
 * @brief Concrete collectible — awards score and publishes COIN_COLLECTED via EventBus
 * @note Week 4
 */

#include "items/Coin.h"
#include "entities/Mario.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "core/AnimationSystem.h"
#include "core/SpriteFrames.h"

namespace {
constexpr float COIN_WIDTH  = 16.f;
constexpr float COIN_HEIGHT = 16.f;
constexpr const char* COIN_TEXTURE_PATH = "assets/textures/tiles/tileset.png";
} // namespace

Coin::Coin()
    : Item(sf::Vector2f(0.f, 0.f), sf::Vector2f(COIN_WIDTH, COIN_HEIGHT)) {
    initPhysics(nullptr, b2_staticBody, sf::Vector2f(COIN_WIDTH, COIN_HEIGHT), true);
    setSprite(COIN_TEXTURE_PATH);
    m_animationSystem->addAnimation("idle",
        AnimationSystem::createManualAnimation(SpriteFrames::Items::coinFrames(), 0.2f));
    playAnimation("idle");
}

Coin::Coin(const sf::Vector2f& position, b2World* world)
    : Item(position, sf::Vector2f(COIN_WIDTH, COIN_HEIGHT)) {
    initPhysics(world, b2_staticBody, sf::Vector2f(COIN_WIDTH, COIN_HEIGHT), true);
    setSprite(COIN_TEXTURE_PATH);
    m_animationSystem->addAnimation("idle",
        AnimationSystem::createManualAnimation(SpriteFrames::Items::coinFrames(), 0.2f));
    playAnimation("idle");
}

void Coin::update(float dt) {
    // Sync visual position with Box2D body (if physics is initialized)
    syncPhysics();
    updateAnimation(dt);
}

void Coin::onCollect(Mario& mario) {
    if (m_isCollected) {
        return;
    }
    mario.addScore(COIN_SCORE_VALUE);

    m_isCollected = true;

    // Notify all observers (SoundManager plays coin.wav, HUD updates score)
    EventBus::getInstance().notify(EventType::COIN_COLLECTED);
}

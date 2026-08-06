/**
 * @file Coin.cpp
 * @author TV5 (Truyền)
 * @brief Concrete collectible — awards score and publishes COIN_COLLECTED via EventBus
 * @note Week 4
 */

#include "items/Coin.h"
#include "entities/Mario.h"
#include "core/AnimationSystem.h"
#include "core/SpriteFrames.h"

namespace {
constexpr float COIN_WIDTH  = 16.f;
constexpr float COIN_HEIGHT = 32.f;
constexpr const char* COIN_TEXTURE_PATH = "assets/textures/items/items_objects.png";

void scaleCoinSprite(sf::Sprite& sprite, const sf::Vector2f& size) {
    sf::IntRect textureRect = sprite.getTextureRect();
    if (textureRect.size.x > 0 && textureRect.size.y > 0) {
        sprite.setScale({size.x / static_cast<float>(textureRect.size.x),
                         size.y / static_cast<float>(textureRect.size.y)});
    }
}
} // namespace

#include <cmath>

Coin::Coin()
    : Item(sf::Vector2f(0.f, 0.f), sf::Vector2f(COIN_WIDTH, COIN_HEIGHT)),
      m_type(CoinType::COLLECTIBLE),
      m_initialY(0.f) {
    initPhysics(nullptr, b2_staticBody, sf::Vector2f(COIN_WIDTH, COIN_HEIGHT), true);
    setSprite(COIN_TEXTURE_PATH);
    m_animationSystem->addAnimation("idle",
        AnimationSystem::createManualAnimation(SpriteFrames::Items::coinFrames(), 0.2f));
    playAnimation("idle");
}

Coin::Coin(const sf::Vector2f& position, b2World* world, CoinType type)
    : Item(position, sf::Vector2f(COIN_WIDTH, COIN_HEIGHT)),
      m_type(type),
      m_initialY(position.y) {
    if (m_type == CoinType::COLLECTIBLE) {
        initPhysics(world, b2_staticBody, sf::Vector2f(COIN_WIDTH, COIN_HEIGHT), true);
    }
    setSprite(COIN_TEXTURE_PATH);

    float frameDuration = (type == CoinType::QUESTION_POPUP) ? 0.08f : 0.2f;
    m_animationSystem->addAnimation("idle",
        AnimationSystem::createManualAnimation(SpriteFrames::Items::coinFrames(), frameDuration));
    playAnimation("idle");

    if (m_type == CoinType::QUESTION_POPUP) {
        m_isCollected = true;
    }
}

void Coin::update(float dt) {
    if (m_type == CoinType::QUESTION_POPUP) {
        m_popupTimer += dt;
        float progress = m_popupTimer / POPUP_DURATION;
        if (progress >= 1.0f) {
            m_markedForRemoval = true;
            return;
        }
        float offsetY = std::sin(progress * 3.14159265f) * -POPUP_HEIGHT;
        m_position.y = m_initialY + offsetY;
        if (m_sprite) {
            m_sprite->setPosition(m_position);
        }
        updateAnimation(dt);
        if (m_sprite) {
            scaleCoinSprite(*m_sprite, m_size);
        }
    } else {
        syncPhysics();
        updateAnimation(dt);
        if (m_sprite) {
            scaleCoinSprite(*m_sprite, m_size);
        }
    }
}

void Coin::onCollect(Mario& mario) {
    if (m_isCollected) {
        return;
    }
    mario.collectCoin(COIN_SCORE_VALUE);

    m_isCollected = true;
}

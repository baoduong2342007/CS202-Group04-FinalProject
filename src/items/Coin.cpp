/**
 * @file Coin.cpp
 * @author TV5 (Truyền)
 * @brief Concrete collectible — awards score and publishes COIN_COLLECTED via
 * EventBus
 * @note Week 4
 */

#include "items/Coin.h"
#include <algorithm>
#include <cmath>
#include "core/AnimationSystem.h"
#include "core/ScoreRules.h"
#include "core/SpriteFrames_shared.h"
#include "entities/Mario.h"
#include "level/TileFrames.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"

namespace {
constexpr float COIN_WIDTH = 32.f;
constexpr float COIN_HEIGHT = 32.f;
constexpr const char *MAP_COIN_TEXTURE_PATH =
    "assets/textures/tiles/tileset.png";
constexpr const char *QUESTION_COIN_TEXTURE_PATH =
    "assets/textures/items/items_objects.png";

std::vector<sf::IntRect> mapCoinFrames(LevelTheme theme) {
  switch (theme) {
    case LevelTheme::UNDERGROUND:
      return {
          TileFrames::COIN_UNDERGROUND,
          TileFrames::COIN_UNDERGROUND_SIDE,
          TileFrames::COIN_UNDERGROUND_THIN,
      };
    case LevelTheme::CASTLE:
      return {
          TileFrames::COIN_CASTLE,
          TileFrames::COIN_CASTLE_SIDE,
          TileFrames::COIN_CASTLE_THIN,
      };
    case LevelTheme::UNDERWATER:
      return {
          TileFrames::COIN_UNDERWATER,
          TileFrames::COIN_UNDERWATER_SIDE,
          TileFrames::COIN_UNDERWATER_THIN,
      };
    case LevelTheme::OVERWORLD:
    default:
      return {
          TileFrames::COIN_OVERWORLD,
          TileFrames::COIN_OVERWORLD_SIDE,
          TileFrames::COIN_OVERWORLD_THIN,
      };
  }
}

void scaleCoinSprite(sf::Sprite &sprite, const sf::Vector2f &size) {
  sf::IntRect textureRect = sprite.getTextureRect();
  if (textureRect.size.x > 0 && textureRect.size.y > 0) {
    sprite.setScale({size.x / static_cast<float>(textureRect.size.x),
                     size.y / static_cast<float>(textureRect.size.y)});
  }
}

float scaleQuestionPopupSprite(sf::Sprite &sprite, const sf::Vector2f &size) {
  const sf::IntRect textureRect = sprite.getTextureRect();
  if (textureRect.size.x <= 0 || textureRect.size.y <= 0) {
    return 0.f;
  }

  const float scaleX = size.x / static_cast<float>(textureRect.size.x);
  const float scaleY = size.y / static_cast<float>(textureRect.size.y);
  const float uniformScale = std::min(scaleX, scaleY);
  sprite.setScale({uniformScale, uniformScale});
  return static_cast<float>(textureRect.size.x) * uniformScale;
}
} // namespace

Coin::Coin(LevelTheme theme)
    : Item(sf::Vector2f(0.f, 0.f), sf::Vector2f(COIN_WIDTH, COIN_HEIGHT)),
      m_type(CoinType::COLLECTIBLE), m_theme(theme), m_initialY(0.f) {
  initPhysics(nullptr, b2_staticBody, sf::Vector2f(COIN_WIDTH, COIN_HEIGHT),
              true);
  setSprite(MAP_COIN_TEXTURE_PATH);
  m_animationSystem->addAnimation(
      "idle", AnimationSystem::createManualAnimation(mapCoinFrames(m_theme), 0.2f));
  playAnimation("idle");
}

Coin::Coin(const sf::Vector2f &position, b2World *world, CoinType type, LevelTheme theme)
    : Item(position, sf::Vector2f(COIN_WIDTH, COIN_HEIGHT)), m_type(type),
      m_theme(theme),
      m_initialY(position.y) {
  if (m_type == CoinType::COLLECTIBLE) {
    initPhysics(world, b2_staticBody, sf::Vector2f(COIN_WIDTH, COIN_HEIGHT),
                true);
  }
  const bool isQuestionPopup = m_type == CoinType::QUESTION_POPUP;
  setSprite(isQuestionPopup ? QUESTION_COIN_TEXTURE_PATH
                            : MAP_COIN_TEXTURE_PATH);

  float frameDuration = isQuestionPopup ? 0.08f : 0.2f;
  m_animationSystem->addAnimation(
      "idle", AnimationSystem::createManualAnimation(
                  isQuestionPopup ? SpriteFrames::shared::Items::coinFrames()
                                  : mapCoinFrames(m_theme),
                  frameDuration));
  playAnimation("idle");

  if (m_type == CoinType::QUESTION_POPUP) {
    m_isCollected = true;
  }
}

void Coin::update(float dt) {
  updateCollectibleDelay(dt);

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
      const float renderedWidth =
          scaleQuestionPopupSprite(*m_sprite, m_size);
      m_sprite->setPosition(
          {m_position.x + (m_size.x - renderedWidth) / 2.f, m_position.y});
    }
  } else {
    syncPhysics();
    updateAnimation(dt);
    if (m_sprite) {
      scaleCoinSprite(*m_sprite, m_size);
    }
  }
}

void Coin::onCollect(Mario &mario) {
  if (m_isCollected) {
    return;
  }

  awardTo(mario);

  m_isCollected = true;
}

void Coin::awardTo(Mario &mario) {
  mario.collectCoin(ScoreRules::pointsFor(ScoreEvent::COIN_COLLECTED));

  int coins = mario.getCoinCount();
  while (coins >= COINS_PER_LIFE) {
    coins -= COINS_PER_LIFE;
    mario.addLife();
    EventBus::getInstance().notify(EventType::ONE_UP_COLLECTED);
  }
  if (coins != mario.getCoinCount()) {
    mario.setCoinCount(coins);
  }
}

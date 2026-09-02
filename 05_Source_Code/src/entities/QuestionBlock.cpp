/**
 * @file QuestionBlock.cpp
 * @author TV5 (Truyen)
 * @brief Animated Question block — 3-frame flashing & 12px bump animation on hit
 * @note Sprint 4 — uses AnimationSystem, same pattern as Coin
 */

#include "entities/QuestionBlock.h"
#include <cmath>
#include <random>
#include "core/AnimationSystem.h"
#include "core/SpriteFrames_ovw.h"
#include "core/SpriteFrames_udg.h"
#include "core/SpriteFrames_castle.h"
#include "core/SpriteFrames_udw.h"
#include "core/TextureManager.h"
#include "items/Coin.h"
#include "items/Mushroom.h"
#include "items/FireFlower.h"
#include "items/Star.h"
#include "entities/Mario.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"

namespace {
constexpr float BLOCK_SIZE = 32.f;
constexpr const char* QUESTION_BLOCK_TEXTURE = "assets/textures/items/items_blocks.png";
constexpr float ANIM_FRAME_DURATION = 0.2f;
constexpr unsigned int COIN_ROLL_LIMIT =
    QuestionBlock::CONTENT_ROLL_RANGE * QuestionBlock::COIN_DROP_RATE_PERCENT / 100;
constexpr unsigned int MUSHROOM_ROLL_LIMIT =
    COIN_ROLL_LIMIT + (QuestionBlock::CONTENT_ROLL_RANGE - COIN_ROLL_LIMIT) / 2;

std::mt19937& questionBlockRandomEngine() {
    static std::mt19937 engine(std::random_device{}());
    return engine;
}

unsigned int rollQuestionBlockContent() {
    static std::uniform_int_distribution<unsigned int> distribution(
        0, QuestionBlock::CONTENT_ROLL_RANGE - 1);
    return distribution(questionBlockRandomEngine());
}
} // namespace

QuestionBlockContent QuestionBlock::chooseRandomContent(unsigned int roll) {
    const unsigned int normalizedRoll = roll % CONTENT_ROLL_RANGE;
    if (normalizedRoll < COIN_ROLL_LIMIT) {
        return QuestionBlockContent::COIN;
    }
    if (normalizedRoll < MUSHROOM_ROLL_LIMIT) {
        return QuestionBlockContent::SUPER_MUSHROOM;
    }
    return QuestionBlockContent::FIRE_FLOWER;
}

QuestionBlockContent QuestionBlock::chooseRandomContent() {
    return chooseRandomContent(rollQuestionBlockContent());
}

QuestionBlock::QuestionBlock(const sf::Vector2f& position, b2World* world, QuestionBlockContent content, BlockTheme theme)
    : Entity(position, sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE)),
      m_content(content),
      m_theme(theme) {
    m_contentResolved = content != QuestionBlockContent::ADAPTIVE;
    initPhysics(world, b2_staticBody, sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE), false);
    setSprite(QUESTION_BLOCK_TEXTURE);

    const auto& animFrames = [theme]() -> const std::vector<sf::IntRect>& {
        switch (theme) {
            case BlockTheme::UNDERGROUND: return SpriteFrames::udg::Blocks::questionBlockFrames();
            case BlockTheme::CASTLE:      return SpriteFrames::castle::Blocks::questionBlockFrames();
            case BlockTheme::UNDERWATER:  return SpriteFrames::udw::Blocks::questionBlockFrames();
            case BlockTheme::OVERWORLD:
            default:                      return SpriteFrames::ovw::Blocks::questionBlockFrames();
        }
    }();

    const sf::IntRect emptyRect = [theme]() -> sf::IntRect {
        switch (theme) {
            case BlockTheme::UNDERGROUND: return SpriteFrames::udg::Blocks::EMPTY;
            case BlockTheme::CASTLE:      return SpriteFrames::castle::Blocks::EMPTY;
            case BlockTheme::UNDERWATER:  return SpriteFrames::udw::Blocks::EMPTY;
            case BlockTheme::OVERWORLD:
            default:                      return SpriteFrames::ovw::Blocks::EMPTY;
        }
    }();

    m_animationSystem->addAnimation("idle",
        AnimationSystem::createManualAnimation(animFrames, ANIM_FRAME_DURATION));
    m_animationSystem->addAnimation("empty",
        AnimationSystem::createManualAnimation(
            std::vector<sf::IntRect>{emptyRect}, 1.f));
    playAnimation("idle");
}

void QuestionBlock::update(float dt) {
    syncPhysics();

    if (m_isBumping) {
        m_bumpTimer += dt;
        if (m_bumpTimer >= BUMP_DURATION) {
            m_bumpTimer = BUMP_DURATION;
            m_isBumping = false;
            m_bumpOffsetY = 0.f;
        } else {
            float progress = m_bumpTimer / BUMP_DURATION;
            m_bumpOffsetY = std::sin(progress * 3.14159265f) * MAX_BUMP_OFFSET;
        }
    }

    updateAnimation(dt);

    if (m_sprite) {
        sf::Vector2f renderPos = m_position + sf::Vector2f(0.f, m_bumpOffsetY);
        m_sprite->setPosition(renderPos);
    }
}

void QuestionBlock::onHit(Mario& mario, std::vector<std::unique_ptr<Entity>>* entities, TextureManager* textureManager) {
    if (m_isHit) return;

    m_isHit = true;
    m_isBumping = true;
    m_bumpTimer = 0.f;

    playAnimation("empty");
    EventBus::getInstance().notify(EventType::BLOCK_BUMPED);

    // Normal '?' blocks resolve exactly once. Their result is independent of
    // Result distribution: 70% Coin, 15% Mushroom, or 15% FireFlower.
    // Explicit item routes remain deterministic.
    if (!m_contentResolved && m_content == QuestionBlockContent::ADAPTIVE) {
        m_content = chooseRandomContent();
        m_contentResolved = true;
    }

    const QuestionBlockContent resolvedContent = m_content;
    if (resolvedContent == QuestionBlockContent::COIN) {
        Coin::awardTo(mario);
    }

    if (entities && (textureManager || m_textureManager)) {
        TextureManager& texMgr = textureManager ? *textureManager : *m_textureManager;
        sf::Vector2f spawnPos = getPosition() - sf::Vector2f(0.f, BLOCK_SIZE);
        b2World* world = getBody() ? getBody()->GetWorld() : nullptr;

        const LevelTheme levelTheme = [this]() {
            switch (m_theme) {
                case BlockTheme::UNDERGROUND: return LevelTheme::UNDERGROUND;
                case BlockTheme::CASTLE:      return LevelTheme::CASTLE;
                case BlockTheme::UNDERWATER:  return LevelTheme::UNDERWATER;
                case BlockTheme::OVERWORLD:
                default:                      return LevelTheme::OVERWORLD;
            }
        }();

        if (resolvedContent == QuestionBlockContent::COIN) {
            auto popupCoin = std::make_unique<Coin>(spawnPos, world, CoinType::QUESTION_POPUP, levelTheme);
            popupCoin->setTextureManager(texMgr);
            entities->push_back(std::move(popupCoin));
        } else if (resolvedContent == QuestionBlockContent::SUPER_MUSHROOM ||
                   resolvedContent == QuestionBlockContent::ONEUP_MUSHROOM) {
            const MushroomType mushroomType =
                resolvedContent == QuestionBlockContent::ONEUP_MUSHROOM
                    ? MushroomType::ONE_UP
                    : MushroomType::SUPER;
            auto mushroom = std::make_unique<Mushroom>(spawnPos, world, mushroomType, levelTheme);
            mushroom->setTextureManager(texMgr);
            mushroom->setCollectibleDelay(ITEM_EMERGE_DELAY);
            entities->push_back(std::move(mushroom));
        } else if (resolvedContent == QuestionBlockContent::FIRE_FLOWER) {
            auto flower = std::make_unique<FireFlower>(spawnPos, world);
            flower->setTextureManager(texMgr);
            flower->setCollectibleDelay(ITEM_EMERGE_DELAY);
            entities->push_back(std::move(flower));
        } else if (resolvedContent == QuestionBlockContent::STAR) {
            auto star = std::make_unique<Star>(spawnPos, world);
            star->setTextureManager(texMgr);
            star->setCollectibleDelay(ITEM_EMERGE_DELAY);
            entities->push_back(std::move(star));
        }

        if (resolvedContent != QuestionBlockContent::COIN) {
            EventBus::getInstance().notify(EventType::ITEM_EMERGED);
        }
    }
}

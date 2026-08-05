/**
 * @file QuestionBlock.cpp
 * @author TV5 (Truyền)
 * @brief Animated Question block — 3-frame flashing & 12px bump animation on hit
 * @note Sprint 4 — uses AnimationSystem, same pattern as Coin
 */

#include "entities/QuestionBlock.h"
#include <cmath>
#include "core/AnimationSystem.h"
#include "core/SpriteFrames.h"
#include "core/TextureManager.h"
#include "items/Coin.h"
#include "items/Mushroom.h"
#include "entities/Mario.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"

namespace {
constexpr float BLOCK_SIZE = 32.f;
constexpr const char* QUESTION_BLOCK_TEXTURE = "assets/textures/items/items_blocks.png";
constexpr float ANIM_FRAME_DURATION = 0.2f;
} // namespace

QuestionBlock::QuestionBlock(const sf::Vector2f& position, b2World* world, QuestionBlockContent content)
    : Entity(position, sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE)),
      m_content(content) {
    initPhysics(world, b2_staticBody, sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE), false);
    setSprite(QUESTION_BLOCK_TEXTURE);
    m_animationSystem->addAnimation("idle",
        AnimationSystem::createManualAnimation(
            SpriteFrames::Blocks::questionBlockFrames(), ANIM_FRAME_DURATION));
    m_animationSystem->addAnimation("empty",
        AnimationSystem::createManualAnimation(
            std::vector<sf::IntRect>{SpriteFrames::Blocks::EMPTY}, 1.f));
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
    (void)mario;
    if (m_isHit) return;

    m_isHit = true;
    m_isBumping = true;
    m_bumpTimer = 0.f;

    playAnimation("empty");

    if (entities && (textureManager || m_textureManager)) {
        TextureManager& texMgr = textureManager ? *textureManager : *m_textureManager;
        sf::Vector2f spawnPos = getPosition() - sf::Vector2f(0.f, BLOCK_SIZE);
        b2World* world = getBody() ? getBody()->GetWorld() : nullptr;

        if (m_content == QuestionBlockContent::COIN) {
            auto popupCoin = std::make_unique<Coin>(spawnPos, world, CoinType::QUESTION_POPUP);
            popupCoin->setTextureManager(texMgr);
            entities->push_back(std::move(popupCoin));
        } else if (m_content == QuestionBlockContent::SUPER_MUSHROOM) {
            auto mushroom = std::make_unique<Mushroom>(spawnPos, world, MushroomType::SUPER);
            mushroom->setTextureManager(texMgr);
            entities->push_back(std::move(mushroom));
        } else if (m_content == QuestionBlockContent::ONEUP_MUSHROOM) {
            auto mushroom = std::make_unique<Mushroom>(spawnPos, world, MushroomType::ONE_UP);
            mushroom->setTextureManager(texMgr);
            entities->push_back(std::move(mushroom));
        }
    }
}


/**
 * @file Lakitu.cpp
 * @author TV4 (Vy)
 * @brief Lakitu implementation - cloud pursuit, egg drops, and respawn
 */

#include "entities/Lakitu.h"
#include "entities/SpinyEgg.h"

#include <algorithm>
#include <cmath>

#include <box2d/box2d.h>

#include "core/AnimationSystem.h"
#include "core/SpriteFrames_shared.h"

namespace {

const sf::Vector2f LAKITU_SIZE{32.f, 32.f};
constexpr const char* LAKITU_TEXTURE_PATH = "assets/textures/enemies/enemies.png";
constexpr const char* PEEK_ANIMATION = "peek";
constexpr const char* HIDE_ANIMATION = "hide";
constexpr float LAKITU_FRAME_DURATION = 0.15f;

const sf::IntRect& peekFrame(LevelTheme theme) {
    switch (theme) {
        case LevelTheme::UNDERGROUND:
        case LevelTheme::CASTLE:
            return SpriteFrames::legacy::Enemies::Lakitu::UG_CASTLE_PEEK;
        case LevelTheme::UNDERWATER:
            return SpriteFrames::legacy::Enemies::Lakitu::UW_PEEK;
        case LevelTheme::OVERWORLD:
        default:
            return SpriteFrames::legacy::Enemies::Lakitu::PEEK;
    }
}

const sf::IntRect& hideFrame(LevelTheme theme) {
    switch (theme) {
        case LevelTheme::UNDERGROUND:
        case LevelTheme::CASTLE:
            return SpriteFrames::legacy::Enemies::Lakitu::UG_CASTLE_HIDE;
        case LevelTheme::UNDERWATER:
            return SpriteFrames::legacy::Enemies::Lakitu::UW_HIDE;
        case LevelTheme::OVERWORLD:
        default:
            return SpriteFrames::legacy::Enemies::Lakitu::HIDE;
    }
}

} // namespace

Lakitu::Lakitu(const sf::Vector2f& position,
               b2World* world,
               LevelTheme theme)
    : Enemy(position, LAKITU_SIZE, 1),
      m_world(world),
      m_theme(theme) {
    setSprite(LAKITU_TEXTURE_PATH);

    // A floating sensor: Mario can stomp through it, nothing can stand on it.
    initPhysics(world, b2_dynamicBody, LAKITU_SIZE, true);

    if (m_body) {
        m_body->SetGravityScale(0.f);
    }

    m_animationSystem = std::make_unique<AnimationSystem>();
    m_animationSystem->addAnimation(
        PEEK_ANIMATION,
        AnimationSystem::createManualAnimation({peekFrame(theme)},
                                                LAKITU_FRAME_DURATION, false));
    m_animationSystem->addAnimation(
        HIDE_ANIMATION,
        AnimationSystem::createManualAnimation({hideFrame(theme)},
                                                LAKITU_FRAME_DURATION, false));
    playAnimation(PEEK_ANIMATION);
}

void Lakitu::update(float dt) {
    syncPhysics();

    if (m_isFlippedDead) {
        if (m_sprite) {
            m_sprite->setPosition(m_position + sf::Vector2f(m_size.x / 2.f, m_size.y / 2.f));
            m_sprite->setOrigin({8.f, 12.f});
            m_sprite->setScale({2.f, -2.f});
        }

        // The corpse falls out of sight, then Lakitu reappears at flight
        // height behind the player, SMB1-style.
        if (m_position.y >= RESPAWN_Y) {
            m_isFlippedDead = false;
            setHealth(1);
            allowNextStomp();
            // The revived Lakitu is a fresh target: re-arm the terminal
            // defeat latch or fireballs/stars/shells would bounce off it
            // forever after the first knock-down.
            allowNextDefeat();

            const float respawnX = m_marioKnown
                                       ? m_marioPosition.x - 128.f
                                       : m_position.x;
            m_position = {respawnX, FLY_Y};

            if (m_body) {
                m_body->SetGravityScale(0.f);
                m_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
            }
            setVelocity({0.f, 0.f});

            m_state = State::CRUISE;
            m_stateTimer = 0.f;
            playAnimation(PEEK_ANIMATION);
        }
        return;
    }

    // Ease toward Mario's column, capped like the NES cloud glide.
    float targetX = m_position.x;
    if (m_marioKnown) {
        targetX = m_marioPosition.x;
    }
    float vx = (targetX - m_position.x) * TRACK_GAIN;
    vx = std::clamp(vx, -TRACK_SPEED, TRACK_SPEED);

    setVelocity({vx, 0.f});

    m_stateTimer += dt;
    switch (m_state) {
        case State::CRUISE:
            if (m_stateTimer >= CRUISE_TIME) {
                m_state = State::HIDE;
                m_stateTimer = 0.f;
                m_eggQueued = false;
                playAnimation(HIDE_ANIMATION);
            }
            break;
        case State::HIDE:
            if (m_stateTimer >= HIDE_TIME) {
                m_state = State::THROW;
                m_stateTimer = 0.f;
            }
            break;
        case State::THROW:
            if (!m_eggQueued && m_world) {
                m_eggQueued = true;
                const float predictedX = m_marioKnown
                    ? (m_marioPosition.x + std::clamp(m_marioVelX * 0.4f, -60.f, 60.f))
                    : m_position.x;
                const Direction throwDirection =
                    (predictedX < m_position.x)
                        ? Direction::LEFT : Direction::RIGHT;
                m_pending.push_back(std::make_unique<SpinyEgg>(
                    m_position + sf::Vector2f{0.f, m_size.y},
                    m_world, m_theme, throwDirection));
            }
            if (m_stateTimer >= THROW_TIME) {
                m_state = State::CRUISE;
                m_stateTimer = 0.f;
                playAnimation(PEEK_ANIMATION);
            }
            break;
    }

    if (m_marioKnown) {
        setFacingDirection(m_marioPosition.x > m_position.x + 8.f ? Direction::RIGHT : Direction::LEFT);
    } else if (std::abs(vx) > 5.f) {
        setFacingDirection(vx > 0.f ? Direction::RIGHT : Direction::LEFT);
    }

    updateAnimation(dt);

    if (m_sprite) {
        if (getFacingDirection() == Direction::RIGHT) {
            m_sprite->setScale({-2.f, 2.f});
            m_sprite->setOrigin({static_cast<float>(m_sprite->getTextureRect().size.x), 0.f});
        } else {
            m_sprite->setScale({2.f, 2.f});
            m_sprite->setOrigin({0.f, 0.f});
        }
        m_sprite->setPosition(m_position);
    }
}

void Lakitu::patrol() {
    // Cloud tracking is driven entirely by update().
}

void Lakitu::onStomp() {
    if (m_isFlippedDead) return;
    dieFlipped();
}

void Lakitu::onWallCollision() {
}

void Lakitu::onFireHit() {
    if (m_isFlippedDead) return;
    dieFlipped();
}

void Lakitu::dieFlipped() {
    m_isFlippedDead = true;
    setHealth(0);

    if (m_body) {
        m_body->SetGravityScale(1.f);
        m_body->SetLinearVelocity(b2Vec2(0.f, -8.f));
    }
}

void Lakitu::updateMarioPosition(const sf::Vector2f& marioPos) {
    if (m_marioKnown) {
        m_marioVelX = (marioPos.x - m_marioPosition.x) / 0.016f;
    }
    m_prevMarioPosition = m_marioPosition;
    m_marioPosition = marioPos;
    m_marioKnown = true;
}

std::vector<std::unique_ptr<Entity>> Lakitu::takePendingSpawns() {
    std::vector<std::unique_ptr<Entity>> drained;
    drained.swap(m_pending);
    return drained;
}

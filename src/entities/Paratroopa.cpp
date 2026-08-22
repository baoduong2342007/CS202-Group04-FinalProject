/**
 * @file Paratroopa.cpp
 * @brief Paratroopa implementation - winged movement layered on the Koopa core
 */

#include "entities/Paratroopa.h"

#include <cmath>

#include "core/AnimationSystem.h"
#include "core/SpriteFrames_ovw.h"
#include "core/SpriteFrames_udg.h"
#include "core/SpriteFrames_castle.h"
#include "core/SpriteFrames_udw.h"

namespace {

constexpr float PARATROOPA_PATROL_SPEED = 50.f;
const sf::Vector2f PARATROOPA_SIZE{32.f, 48.f};
const sf::Vector2f PARATROOPA_SHELL_SIZE{32.f, 28.f};

constexpr float HOP_LAUNCH_SPEED = 260.f;   ///< Upward launch of a green hop
constexpr float HOP_PATROL_SPEED = 30.f;    ///< Slow drift between hops
constexpr float HOP_REARM_TIME = 0.5f;      ///< Guards against a mid-air double hop

constexpr float FLY_SPEED = 80.f;           ///< Red flyer vertical speed
constexpr float FLY_RANGE = 96.f;           ///< Red flyer corridor half-height

constexpr float FLY_FRAME_DURATION = 0.15f;
constexpr const char* FLY_ANIMATION = "fly";
// Registered by the Koopa core; re-used after the wings are gone.
constexpr const char* WALK_ANIMATION = "walk";

const std::vector<sf::IntRect>& greenFlyFrames(LevelTheme theme) {
    switch (theme) {
        case LevelTheme::UNDERGROUND:
            return SpriteFrames::udg::Enemies::Paratroopa::flyFrames();
        case LevelTheme::CASTLE:
            return SpriteFrames::castle::Enemies::Paratroopa::flyFrames();
        case LevelTheme::UNDERWATER:
            return SpriteFrames::udw::Enemies::Paratroopa::flyFrames();
        case LevelTheme::OVERWORLD:
        default:
            return SpriteFrames::ovw::Enemies::Paratroopa::flyFrames();
    }
}

} // namespace

Paratroopa::Paratroopa(const sf::Vector2f& position,
                       b2World* world,
                       LevelTheme theme,
                       ParatroopaMode mode)
    : Koopa(position, world, theme,
            PARATROOPA_SIZE, PARATROOPA_SHELL_SIZE, PARATROOPA_PATROL_SPEED),
      m_mode(mode) {
    const bool red = mode == ParatroopaMode::FLY_VERTICAL;

    const std::vector<sf::IntRect>& flyFrames = red
        ? SpriteFrames::ovw::Enemies::RedKoopa::flyFrames()
        : greenFlyFrames(theme);

    if (red) {
        // One red palette row serves every environment.
        registerKoopaAnimations(SpriteFrames::ovw::Enemies::RedKoopa::walkFrames(),
                                SpriteFrames::ovw::Enemies::RedKoopa::SHELL,
                                SpriteFrames::ovw::Enemies::RedKoopa::SHELL_WAKING);
    } else {
        const auto& walkFrames = [theme]() -> const std::vector<sf::IntRect>& {
            switch (theme) {
                case LevelTheme::UNDERGROUND:
                    return SpriteFrames::udg::Enemies::Koopa::walkFrames();
                case LevelTheme::CASTLE:
                    return SpriteFrames::castle::Enemies::Koopa::walkFrames();
                case LevelTheme::UNDERWATER:
                    return SpriteFrames::udw::Enemies::Koopa::walkFrames();
                case LevelTheme::OVERWORLD:
                default:
                    return SpriteFrames::ovw::Enemies::Koopa::walkFrames();
            }
        }();

        const sf::IntRect& shellRect = [theme]() -> const sf::IntRect& {
            switch (theme) {
                case LevelTheme::UNDERGROUND:
                    return SpriteFrames::udg::Enemies::Koopa::SHELL;
                case LevelTheme::CASTLE:
                    return SpriteFrames::castle::Enemies::Koopa::SHELL;
                case LevelTheme::UNDERWATER:
                    return SpriteFrames::udw::Enemies::Koopa::SHELL;
                case LevelTheme::OVERWORLD:
                default:
                    return SpriteFrames::ovw::Enemies::Koopa::SHELL;
            }
        }();

        const sf::IntRect& wakingRect = [theme]() -> const sf::IntRect& {
            switch (theme) {
                case LevelTheme::UNDERGROUND:
                    return SpriteFrames::udg::Enemies::Koopa::SHELL_WAKING;
                case LevelTheme::CASTLE:
                    return SpriteFrames::castle::Enemies::Koopa::SHELL_WAKING;
                case LevelTheme::UNDERWATER:
                    return SpriteFrames::udw::Enemies::Koopa::SHELL_WAKING;
                case LevelTheme::OVERWORLD:
                default:
                    return SpriteFrames::ovw::Enemies::Koopa::SHELL_WAKING;
            }
        }();

        registerKoopaAnimations(walkFrames, shellRect, wakingRect);
    }

    m_animationSystem->addAnimation(
        FLY_ANIMATION,
        AnimationSystem::createManualAnimation(flyFrames, FLY_FRAME_DURATION, true));
    playAnimation(FLY_ANIMATION);

    if (red && m_body) {
        // The vertical flyer floats; gravity only returns after the wings are
        // clipped.
        m_body->SetGravityScale(0.f);
        setVelocity({0.f, -FLY_SPEED});
    }

    m_anchorY = m_position.y;
}

void Paratroopa::update(float dt) {
    if (m_hasWings && m_rearmTimer > 0.f) {
        m_rearmTimer -= dt;
        if (m_rearmTimer < 0.f) {
            m_rearmTimer = 0.f;
        }
    }

    Koopa::update(dt);
}

void Paratroopa::patrol() {
    if (!m_hasWings || m_state != KoopaState::WALKING) {
        Koopa::patrol();
        return;
    }

    if (m_mode == ParatroopaMode::HOP) {
        // Canonical SMB1: the hopper drifts off ledges too - its wings only
        // hop, they do not fly. Wingless walkers follow Koopa::patrol().
        if (turnsAtLedge() && isApproachingLedge()) {
            reverseDirection();
        }

        sf::Vector2f velocity = getVelocity();
        velocity.x = getFacingDirection() == Direction::LEFT
                         ? -HOP_PATROL_SPEED : HOP_PATROL_SPEED;

        // A near-zero vertical velocity means the hopper is resting on the
        // ground (the rearm timer keeps the mid-air apex from retriggering).
        if (m_rearmTimer <= 0.f && std::abs(velocity.y) < 4.f) {
            velocity.y = -HOP_LAUNCH_SPEED;
            m_rearmTimer = HOP_REARM_TIME;
        }

        setVelocity(velocity);
        return;
    }

    // FLY_VERTICAL: patrol the corridor around the spawn anchor.
    if (m_position.y <= m_anchorY - FLY_RANGE) {
        m_flyingUp = false;
    } else if (m_position.y >= m_anchorY + FLY_RANGE) {
        m_flyingUp = true;
    }

    setVelocity({0.f, m_flyingUp ? -FLY_SPEED : FLY_SPEED});
}

void Paratroopa::onStomp() {
    if (m_hasWings) {
        clipWings();
        return;
    }

    Koopa::onStomp();
}

void Paratroopa::onWallCollision() {
    // The vertical flyer never travels horizontally, so it has no wall to
    // turn away from while winged.
    if (m_hasWings && m_mode == ParatroopaMode::FLY_VERTICAL) {
        return;
    }

    Koopa::onWallCollision();
}

void Paratroopa::clipWings() {
    m_hasWings = false;

    if (m_body) {
        m_body->SetGravityScale(1.f);
    }

    playAnimation(WALK_ANIMATION);
    updateAnimation(0.f);

    // Re-arm the one-shot stomp latch: the wing clip was an interaction, not
    // a defeat, and the next stomp still has to claim the shell transition.
    allowNextStomp();
}

std::vector<std::unique_ptr<Entity>> Paratroopa::takePendingSpawns() {
    std::vector<std::unique_ptr<Entity>> drained;
    drained.swap(m_pending);
    return drained;
}

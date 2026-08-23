/**
 * @file Bowser.cpp
 * @brief Bowser boss implementation - pacing, hops, fire breath, lava sink
 */

#include "entities/Bowser.h"
#include "entities/BowserFire.h"
#include "entities/Mario.h"
#include "entities/FireballExplosion.h"

#include <algorithm>
#include <cmath>
#include <random>

#include <box2d/box2d.h>

#include "core/AnimationSystem.h"
#include "core/ScoreRules.h"
#include "core/SoundManager.h"
#include "core/SpriteFrames_shared.h"

namespace {

const sf::Vector2f BOWSER_SIZE{64.f, 64.f};
constexpr const char* BOWSER_TEXTURE_PATH = "assets/textures/enemies/enemies.png";
constexpr const char* WALK_ANIMATION = "walk";
constexpr const char* FIRE_WINDUP_ANIMATION = "fire_windup";
constexpr const char* FIRE_EXHALE_ANIMATION = "fire_exhale";
constexpr float BOWSER_FRAME_DURATION = 0.18f;

const std::vector<sf::IntRect>& walkFrames(LevelTheme theme) {
    if (theme == LevelTheme::UNDERWATER) {
        static const std::vector<sf::IntRect> uwFrames = {
            SpriteFrames::legacy::Enemies::Bowser::UW_WALK_CLOSED,
            SpriteFrames::legacy::Enemies::Bowser::UW_WALK_OPEN};
        return uwFrames;
    }
    if (theme == LevelTheme::OVERWORLD) {
        static const std::vector<sf::IntRect> owFrames = {
            SpriteFrames::legacy::Enemies::Bowser::WALK_CLOSED,
            SpriteFrames::legacy::Enemies::Bowser::WALK_OPEN};
        return owFrames;
    }
    // Underground and Castle share the UG palette row.
    static const std::vector<sf::IntRect> ugFrames = {
        SpriteFrames::legacy::Enemies::Bowser::UG_WALK_CLOSED,
        SpriteFrames::legacy::Enemies::Bowser::UG_WALK_OPEN};
    return ugFrames;
}

const sf::IntRect& fireWindupFrame(LevelTheme theme) {
    if (theme == LevelTheme::UNDERWATER) {
        return SpriteFrames::legacy::Enemies::Bowser::UW_FIRE_POSE1;
    }
    if (theme == LevelTheme::OVERWORLD) {
        return SpriteFrames::legacy::Enemies::Bowser::FIRE_POSE1;
    }
    return SpriteFrames::legacy::Enemies::Bowser::UG_FIRE_POSE1;
}

const sf::IntRect& fireExhaleFrame(LevelTheme theme) {
    if (theme == LevelTheme::UNDERWATER) {
        return SpriteFrames::legacy::Enemies::Bowser::UW_FIRE_POSE2;
    }
    if (theme == LevelTheme::OVERWORLD) {
        return SpriteFrames::legacy::Enemies::Bowser::FIRE_POSE2;
    }
    return SpriteFrames::legacy::Enemies::Bowser::UG_FIRE_POSE2;
}

} // namespace

Bowser::Bowser(const sf::Vector2f& position,
               b2World* world,
               LevelTheme theme)
    : Enemy(position, BOWSER_SIZE, FIREBALL_HITS_TO_KILL),
      m_spawnOriginX(position.x),
      m_world(world),
      m_theme(theme) {
    setFacingDirection(Direction::LEFT);
    initPhysics(world, b2_dynamicBody, BOWSER_SIZE);
    setSprite(BOWSER_TEXTURE_PATH);

    m_animationSystem = std::make_unique<AnimationSystem>();
    m_animationSystem->addAnimation(
        WALK_ANIMATION,
        AnimationSystem::createManualAnimation(walkFrames(theme),
                                                BOWSER_FRAME_DURATION, true));
    m_animationSystem->addAnimation(
        FIRE_WINDUP_ANIMATION,
        AnimationSystem::createManualAnimation({fireWindupFrame(theme)},
                                                BOWSER_FRAME_DURATION, true));
    m_animationSystem->addAnimation(
        FIRE_EXHALE_ANIMATION,
        AnimationSystem::createManualAnimation({fireExhaleFrame(theme)},
                                                BOWSER_FRAME_DURATION, true));
    playAnimation(WALK_ANIMATION);
}

void Bowser::update(float dt) {
    syncPhysics();

    if (m_state == State::DIE) {
        if (m_sprite) {
            m_sprite->setPosition(m_position);
            m_sprite->setScale({2.f, 2.f});
            m_sprite->setColor(sf::Color::White);
        }
        if (m_position.y > DIE_CLEANUP_Y) {
            markForRemoval();
        }
        return;
    }

    if (isDead()) {
        enterDie();
        return;
    }

    // Damage flash or Enraged aura feedback
    if (m_damageFlashTimer > 0.f) {
        m_damageFlashTimer -= dt;
        if (m_sprite) {
            m_sprite->setColor(sf::Color(255, 120, 120, 240));
        }
    } else if (isEnraged()) {
        m_enragedPulseTimer += dt;
        if (m_sprite) {
            const uint8_t pulse = static_cast<uint8_t>(200 + 55 * std::sin(m_enragedPulseTimer * 10.f));
            m_sprite->setColor(sf::Color(255, pulse, pulse, 255));
        }
    } else {
        if (m_sprite) {
            m_sprite->setColor(sf::Color::White);
        }
    }

    m_stateTimer += dt;
    const bool enraged = isEnraged();

    switch (m_state) {
        case State::PATROL: {
            patrol();

            // Anti-Air Intercept hop: if Mario is airborne attempting to vault over Bowser (e.g. from Springboard / Elevator)
            if (m_marioKnown && std::abs(m_marioPosition.x - m_position.x) < 220.f &&
                m_marioPosition.y < m_position.y - 15.f && std::abs(getVelocity().y) < 2.f &&
                m_attackTimer < 1.2f) {
                sf::Vector2f velocity = getVelocity();
                velocity.y = -HOP_SPEED * (enraged ? 1.35f : 1.2f);
                setVelocity(velocity);
                m_attackTimer = enraged ? 1.0f : 1.5f;
            }

            m_attackTimer -= dt;
            if (m_attackTimer <= 0.f) {
                static std::mt19937 rng(std::random_device{}());
                std::uniform_real_distribution<float> nextAttack(enraged ? 0.9f : 1.8f,
                                                                 enraged ? 1.6f : 3.0f);
                m_attackTimer = nextAttack(rng);

                // Attack choice:
                // Enraged: 30% jump, 35% fire breath, 35% hammer toss
                // Normal: 25% jump, 75% fire breath
                std::uniform_int_distribution<int> attackDist(0, 99);
                const int roll = attackDist(rng);

                if (roll < (enraged ? 30 : 25)) {
                    sf::Vector2f velocity = getVelocity();
                    velocity.y = -HOP_SPEED * (enraged ? 1.25f : 1.0f);
                    setVelocity(velocity);
                } else {
                    m_state = State::BREATHE;
                    m_stateTimer = 0.f;
                    m_fireReleased = false;
                    playAnimation(FIRE_WINDUP_ANIMATION);
                }
            }
            break;
        }
        case State::BREATHE: {
            sf::Vector2f velocity = getVelocity();
            velocity.x = 0.f;
            setVelocity(velocity);

            // Phase 1 (Windup): Bowser holds mouth open
            // Phase 2 (Release): Bowser unleashes flame at FIRE_RELEASE_TIME
            if (!m_fireReleased && m_stateTimer >= (enraged ? 0.2f : FIRE_RELEASE_TIME)) {
                m_fireReleased = true;
                breatheFire();
                playAnimation(FIRE_EXHALE_ANIMATION);
            }
            const float attackDuration = enraged ? 0.55f : BREATHE_DURATION;
            if (m_stateTimer >= attackDuration) {
                m_state = State::PATROL;
                m_stateTimer = 0.f;
                playAnimation(WALK_ANIMATION);
            }
            break;
        }
        case State::DIE:
            break;
    }

    // Airborne and Landing Detection for Ground Stomp Shockwave
    // Only activates if Bowser actively jumped/hopped into the air (vy < -80.f)
    // and triggers the ground shockwave at the exact instant of impact with the floor
    const float vy = getVelocity().y;
    if (vy < -80.f) {
        m_wasAirborne = true;
    } else if (m_wasAirborne && m_previousVy > 30.f && std::abs(vy) < 2.f) {
        triggerGroundStomp();
        m_wasAirborne = false;
    }
    m_previousVy = vy;

    if (m_stompEffectTimer > 0.f) {
        m_stompEffectTimer -= dt;
    }

    // While airborne in PATROL, keep feet steady on frame 1 instead of frantically running mid-air
    if (m_state == State::PATROL && std::abs(getVelocity().y) > 20.f) {
        // Paused on walk frame while airborne
    } else {
        updateAnimation(dt);
    }

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

void Bowser::triggerGroundStomp() {
    if (!m_world || m_state == State::DIE) {
        return;
    }

    SoundManager::getInstance().playSound(SoundId::BUMP);
    m_stompEffectTimer = 0.50f;
    m_stompEffectPos = sf::Vector2f{m_position.x + m_size.x / 2.f, m_position.y + m_size.y};

    // Seismic shockwave stun: stuns Mario if within radius and touching the ground
    if (m_marioTarget && m_marioTarget->isActive() && !m_marioTarget->isDying() && !m_marioTarget->isDead()) {
        const float dx = std::abs(m_marioTarget->getPosition().x - m_stompEffectPos.x);
        const float dy = std::abs((m_marioTarget->getPosition().y + m_marioTarget->getSize().y) - m_stompEffectPos.y);

        if (dx <= SHOCKWAVE_RADIUS && dy <= 48.f && m_marioTarget->isGrounded()) {
            m_marioTarget->stun(1.2f);
        }
    }
}

void Bowser::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    Enemy::draw(target, states);

    // Draw Propagating Concentric Horizontal Ground Shockwaves & Dust VFX
    if (m_stompEffectTimer > 0.f) {
        const float totalDuration = 0.50f;
        const float progress = 1.f - (m_stompEffectTimer / totalDuration);

        // 1. Center Ground Impact Burst (initial spark at touchdown point)
        if (progress < 0.35f) {
            const float burstProgress = progress / 0.35f;
            const uint8_t burstAlpha = static_cast<uint8_t>(255 * (1.f - burstProgress));
            
            sf::ConvexShape centerSpark(8);
            for (int p = 0; p < 8; ++p) {
                const float ang = p * 3.14159265f / 4.f;
                const float rad = (p % 2 == 0 ? 14.f : 6.f) * (1.f + burstProgress * 0.4f);
                centerSpark.setPoint(p, sf::Vector2f(rad * std::cos(ang), -std::abs(rad * std::sin(ang))));
            }
            centerSpark.setPosition({m_stompEffectPos.x, m_stompEffectPos.y});
            centerSpark.setFillColor(sf::Color(255, 245, 160, burstAlpha));
            target.draw(centerSpark, states);
        }

        // 2. Ground-level glowing energy rail along the floor line
        const float maxSpread = progress * SHOCKWAVE_RADIUS;
        const uint8_t beamAlpha = static_cast<uint8_t>(200 * (1.f - progress));
        for (float side : {-1.f, 1.f}) {
            sf::RectangleShape groundBeam({maxSpread, 3.f});
            groundBeam.setOrigin({side < 0.f ? maxSpread : 0.f, 3.f});
            groundBeam.setPosition({m_stompEffectPos.x, m_stompEffectPos.y});
            groundBeam.setFillColor(sf::Color(255, 215, 60, beamAlpha));
            target.draw(groundBeam, states);

            sf::RectangleShape groundCore({maxSpread * 0.85f, 1.f});
            groundCore.setOrigin({side < 0.f ? maxSpread * 0.85f : 0.f, 2.f});
            groundCore.setPosition({m_stompEffectPos.x, m_stompEffectPos.y});
            groundCore.setFillColor(sf::Color(255, 255, 220, beamAlpha));
            target.draw(groundCore, states);
        }

        // 3. 3 Successive Propagating Wave Fronts (Concentric Ground Ripple Waves)
        for (int ring = 0; ring < 3; ++ring) {
            const float delay = ring * 0.12f;
            if (progress < delay) continue;
            const float ringProgress = (progress - delay) / (1.f - delay);
            const float spread = ringProgress * SHOCKWAVE_RADIUS;
            const uint8_t alpha = static_cast<uint8_t>(255 * (1.f - ringProgress) * (1.f - ring * 0.2f));
            const float heightScale = (1.f - ring * 0.25f);

            for (float side : {-1.f, 1.f}) {
                const float waveX = m_stompEffectPos.x + side * spread;
                const float waveY = m_stompEffectPos.y;

                // Outer Orange/Gold ground wave crest
                sf::ConvexShape energySpike(3);
                energySpike.setPoint(0, sf::Vector2f(side * 2.f, -16.f * (1.f - ringProgress) * heightScale - 2.f));
                energySpike.setPoint(1, sf::Vector2f(side * 16.f, 0.f));
                energySpike.setPoint(2, sf::Vector2f(-side * 8.f, 0.f));
                energySpike.setPosition({waveX, waveY});
                energySpike.setFillColor(sf::Color(255, 235 - ring * 30, 60 + ring * 50, alpha));
                target.draw(energySpike, states);

                // Inner hot-white core spike for leading wave
                if (ring == 0) {
                    sf::ConvexShape whiteCore(3);
                    whiteCore.setPoint(0, sf::Vector2f(0.f, -10.f * (1.f - ringProgress)));
                    whiteCore.setPoint(1, sf::Vector2f(side * 8.f, 0.f));
                    whiteCore.setPoint(2, sf::Vector2f(-side * 4.f, 0.f));
                    whiteCore.setPosition({waveX, waveY});
                    whiteCore.setFillColor(sf::Color(255, 255, 230, alpha));
                    target.draw(whiteCore, states);
                }

                // Trailing low ground dust smoke puffs
                if (ring <= 1) {
                    sf::CircleShape dust(6.f * (1.f - ringProgress * 0.35f));
                    dust.setOrigin({dust.getRadius(), dust.getRadius()});
                    dust.setPosition({waveX - side * 8.f, waveY - 4.f - 5.f * ringProgress});
                    dust.setFillColor(sf::Color(240, 240, 250, static_cast<uint8_t>(alpha * 0.75f)));
                    target.draw(dust, states);

                    sf::CircleShape dustSmall(4.f * (1.f - ringProgress * 0.35f));
                    dustSmall.setOrigin({dustSmall.getRadius(), dustSmall.getRadius()});
                    dustSmall.setPosition({waveX - side * 15.f, waveY - 6.f - 8.f * ringProgress});
                    dustSmall.setFillColor(sf::Color(190, 195, 210, static_cast<uint8_t>(alpha * 0.55f)));
                    target.draw(dustSmall, states);
                }
            }
        }
    }
}

void Bowser::patrol() {
    // Pace the arena around the spawn column within ARENA_HALF_WIDTH
    const float centerX = m_position.x + m_size.x / 2.f;

    if (centerX > m_spawnOriginX + ARENA_HALF_WIDTH) {
        m_patrolMoveDir = Direction::LEFT;
    } else if (centerX < m_spawnOriginX - ARENA_HALF_WIDTH) {
        m_patrolMoveDir = Direction::RIGHT;
    }

    // Direction swap timer for lively pacing
    m_patrolTurnTimer -= 0.016f;
    if (m_patrolTurnTimer <= 0.f) {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> turnTime(1.5f, 3.0f);
        m_patrolTurnTimer = turnTime(rng);
        m_patrolMoveDir = (m_patrolMoveDir == Direction::LEFT) ? Direction::RIGHT : Direction::LEFT;
    }

    // Face the intruder (Mario)
    if (m_marioKnown) {
        setFacingDirection(m_marioPosition.x < m_position.x ? Direction::LEFT
                                                            : Direction::RIGHT);
    } else {
        setFacingDirection(m_patrolMoveDir);
    }

    sf::Vector2f velocity = getVelocity();
    velocity.x = (m_patrolMoveDir == Direction::LEFT) ? -PATROL_SPEED : PATROL_SPEED;
    setVelocity(velocity);
}

void Bowser::onStomp() {
    // Stomping the spiked shell hurts Mario; CollisionManager never routes
    // a stomp here because canBeStomped() is false.
}

void Bowser::onWallCollision() {
    // The arena bounds reverse is handled in patrol().
}

void Bowser::onFireHit() {
    if (m_state == State::DIE) return;

    takeDamage(1);
    m_damageFlashTimer = DAMAGE_FLASH_DURATION;

    if (getHealth() <= 0) {
        enterDie();
    }
}

void Bowser::onStarHit() {
    // Canonical SMB1: star contact never harms Bowser — only five fireballs
    // or the axe can defeat him. This is intentionally a no-op.
}

void Bowser::collapseIntoLava() {
    if (m_state == State::DIE) return;
    setHealth(0);
    enterDie();
}

void Bowser::updateMarioPosition(const sf::Vector2f& marioPos) {
    m_marioPosition = marioPos;
    m_marioKnown = true;
}

std::vector<std::unique_ptr<Entity>> Bowser::takePendingSpawns() {
    std::vector<std::unique_ptr<Entity>> drained;
    drained.swap(m_pending);
    return drained;
}

void Bowser::enterDie() {
    m_state = State::DIE;
    setHealth(0);

    SoundManager::getInstance().playSound(SoundId::BOWSER_FALL);

    b2Body* body = getBody();
    if (body) {
        for (b2Fixture* fixture = body->GetFixtureList(); fixture != nullptr; fixture = fixture->GetNext()) {
            fixture->SetSensor(true);
        }
        body->SetGravityScale(0.f);
        body->SetLinearVelocity(b2Vec2(0.f, SINK_SPEED / 32.f));
    }
}

void Bowser::breatheFire() {
    if (!m_world) {
        return;
    }

    const Direction direction = getFacingDirection();
    const float spawnX = (direction == Direction::LEFT)
                             ? (m_position.x - 44.f)
                             : (m_position.x + m_size.x - 4.f);

    // Adaptive multi-height fire trajectory:
    // When Mario is airborne, bias toward HIGH/MID. When Mario is on ground, bias toward LOW.
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> heightDist(0, 99);
    const int roll = heightDist(rng);

    float offsetY = 8.f; // Default HIGH (Head-level)
    if (m_marioKnown && m_marioPosition.y < m_position.y - 10.f) {
        // Mario is in air: 60% HIGH, 40% MID
        offsetY = (roll < 60) ? 8.f : 22.f;
    } else {
        // Mario is grounded: 50% LOW (skimmer), 35% MID, 15% HIGH
        if (roll < 50) {
            offsetY = 38.f;  // LOW (Ground-skimmer)
        } else if (roll < 85) {
            offsetY = 22.f;  // MID (Chest-level)
        } else {
            offsetY = 8.f;   // HIGH (Head-level)
        }
    }

    const float spawnY = m_position.y + offsetY;

    SoundManager::getInstance().playSound(SoundId::BOWSER_FIRE);

    // Primary fire wave
    m_pending.push_back(std::make_unique<BowserFire>(
        sf::Vector2f{spawnX, spawnY}, m_world, m_theme, direction));

    // When Enraged, Bowser unleashes a complementary Twin Fire Wave at a second height
    if (isEnraged()) {
        const float secondaryOffsetY = (offsetY <= 15.f) ? 38.f : 8.f;
        m_pending.push_back(std::make_unique<BowserFire>(
            sf::Vector2f{spawnX, m_position.y + secondaryOffsetY}, m_world, m_theme, direction));
    }
}

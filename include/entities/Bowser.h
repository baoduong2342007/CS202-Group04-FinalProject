/**
 * @file Bowser.h
 * @brief Castle boss: patrols the bridge, hops, and breathes wavy fire
 * @note Enemy expansion - SMB1-accurate: five fireballs defeat him, star
 *       contact cannot harm him, stomping him hurts Mario, and the axe that
 *       collapses his bridge drops him into the lava regardless of health.
 */

#pragma once

#include "core/LevelCatalog.h"
#include "core/ScoreRules.h"
#include "entities/Enemy.h"

class Bowser : public Enemy {
public:
    enum class State {
        PATROL,  ///< Pacing the arena floor
        BREATHE, ///< Fire-breath pose; a BowserFire leaves mid-windup
        DIE      ///< Sinking into the lava, upright and unstoppable
    };

    Bowser(const sf::Vector2f& position,
           b2World* world,
           LevelTheme theme,
           bool hammerVariant = false);
    ~Bowser() override = default;

    void update(float dt) override;

    void patrol() override;
    void onStomp() override;
    void onWallCollision() override;
    void onFireHit() override;
    void onStarHit() override;

    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::BOWSER; }
    Capabilities getCapabilities() const noexcept override {
        return Enemy::getCapabilities() & ~capability(Capability::STOMPABLE);
    }
    int getFireballHealth() const override { return FIREBALL_HITS_TO_KILL; }
    int getDefeatScore(int cause) const override {
        (void)cause;
        return ScoreRules::BOWSER_DEFEATED;
    }
    bool isDying() const override {
        return m_state == State::DIE || isDead() || !isActive();
    }

    State getState() const { return m_state; }
    int getHealth() const { return Character::getHealth(); }

    /// Feed Mario's position; fire breath and hops aim at the player.
    void updateMarioPosition(const sf::Vector2f& marioPos);

    /// The axe was touched: the bridge is gone, sink no matter the health.
    void collapseIntoLava();

    std::vector<std::unique_ptr<Entity>> takePendingSpawns() override;

private:
    void enterDie();
    void breatheFire();

    State m_state{State::PATROL};
    float m_stateTimer{0.f};
    float m_attackTimer{2.f};
    bool m_fireReleased{false};
    bool m_hammerVariant{false};

    Direction m_patrolMoveDir{Direction::LEFT};
    float m_patrolTurnTimer{2.f};
    float m_damageFlashTimer{0.f};
    float m_spawnOriginX{0.f};

    sf::Vector2f m_marioPosition{0.f, 0.f};
    bool m_marioKnown{false};
    b2World* m_world = nullptr;
    LevelTheme m_theme{LevelTheme::CASTLE};
    std::vector<std::unique_ptr<Entity>> m_pending;

    static constexpr int FIREBALL_HITS_TO_KILL = 5;
    static constexpr float PATROL_SPEED = 50.f;
    static constexpr float ARENA_HALF_WIDTH = 96.f;   ///< Pacing range around spawn
    static constexpr float BREATHE_DURATION = 0.8f;
    static constexpr float FIRE_RELEASE_TIME = 0.3f;
    static constexpr float HOP_SPEED = 300.f;
    static constexpr float DAMAGE_FLASH_DURATION = 0.15f;
    static constexpr float SINK_SPEED = 80.f;
    static constexpr float DIE_CLEANUP_Y = 700.f;
};

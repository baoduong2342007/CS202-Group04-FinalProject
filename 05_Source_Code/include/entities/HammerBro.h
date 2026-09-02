/**
 * @file HammerBro.h
 * @brief Armored Koopa that hops between rows and throws claw hammers
 * @note Enemy expansion - SMB1-accurate: bursts of 1-3 spinning hammers on
 *       arcing trajectories; every defeat method pays the same 1000 points.
 */

#pragma once

#include "core/LevelCatalog.h"
#include "entities/Enemy.h"

class TileMap;

class HammerBro : public Enemy {
public:
    enum class State {
        PATROL, ///< Walking the platform, winding up the next attack
        THROW   ///< Four-frame throw animation; hammers leave mid-animation
    };

    HammerBro(const sf::Vector2f& position,
              b2World* world,
              LevelTheme theme);
    ~HammerBro() override = default;

    void update(float dt) override;

    void patrol() override;
    void onStomp() override;
    void onWallCollision() override;
    void onFireHit() override;

    void setTileMap(const TileMap* tileMap) override;

    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::HAMMER_BRO; }
    int getStompScore() const override { return 1000; }
    int getDefeatScore(int cause) const override {
        (void)cause;
        return 1000;
    }
    bool isDying() const override {
        return m_isFlippedDead || isDead() || !isActive();
    }

    /// Canonical SMB1: the Hammer Bro is effectively platform-bound and
    /// reverses at ledges instead of walking off its platform.
    bool turnsAtLedge() const override { return true; }

    State getState() const { return m_state; }

    /// Feed Mario's position; hammer bursts aim at the player's side.
    void updateMarioPosition(const sf::Vector2f& marioPos);

    std::vector<std::unique_ptr<Entity>> takePendingSpawns() override;

private:
    void reverseDirection();
    bool isApproachingLedge() const;
    void syncSpriteToFeet();
    void throwHammer();
    void dieFlipped();

    State m_state{State::PATROL};
    float m_stateTimer{0.f};
    float m_attackCooldown{2.f};
    int m_hammersLeftInBurst{0};
    float m_hammerSpacingTimer{0.f};

    bool m_isFlippedDead = false;
    float m_patrolSpeed = 40.f;
    float m_jumpTimer{0.f};
    bool m_rearmJump{false};

    sf::Vector2f m_marioPosition{0.f, 0.f};
    bool m_marioKnown{false};
    const TileMap* m_tileMap = nullptr;
    b2World* m_world = nullptr;
    LevelTheme m_theme{LevelTheme::OVERWORLD};
    std::vector<std::unique_ptr<Entity>> m_pending;

    static constexpr float PATROL_TIME = 2.0f;
    static constexpr float THROW_TIME = 0.6f;
    static constexpr float HAMMER_SPACING = 0.25f;
    static constexpr float JUMP_INTERVAL = 3.0f;
    static constexpr float JUMP_SPEED = 380.f;
    static constexpr float PIT_CLEANUP_Y = 800.f;
};

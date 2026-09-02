/**
 * @file Lakitu.h
 * @author TV4 (Vy)
 * @brief Cloud-riding enemy that shadows Mario from above and drops Spiny eggs
 * @note Enemy expansion - SMB1-accurate: hides in its cloud before throwing,
 *       stomp 800 / fireball 200, and it comes back a few seconds after
 *       being knocked down.
 */

#pragma once

#include "core/LevelCatalog.h"
#include "entities/Enemy.h"

class Lakitu : public Enemy {
public:
    enum class State {
        CRUISE, ///< Peeking out of the cloud, tracking Mario
        HIDE,   ///< Retracted into the cloud, winding up a throw
        THROW   ///< Releasing a Spiny egg
    };

    Lakitu(const sf::Vector2f& position,
           b2World* world,
           LevelTheme theme);
    ~Lakitu() override = default;

    void update(float dt) override;

    void patrol() override;
    void onStomp() override;
    void onWallCollision() override;
    void onFireHit() override;

    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::LAKITU; }
    int getStompScore() const override { return 800; }
    bool isDying() const override {
        return m_isFlippedDead || isDead() || !isActive();
    }

    State getState() const { return m_state; }

    void updateMarioPosition(const sf::Vector2f& marioPos);

    std::vector<std::unique_ptr<Entity>> takePendingSpawns() override;

private:
    /// Shared flipped-and-falling death presentation (stomp or fireball).
    void dieFlipped();

    State m_state{State::CRUISE};
    float m_stateTimer{0.f};
    sf::Vector2f m_marioPosition{0.f, 0.f};
    sf::Vector2f m_prevMarioPosition{0.f, 0.f};
    float m_marioVelX{0.f};
    bool m_marioKnown{false};
    bool m_isFlippedDead{false};
    bool m_eggQueued{false};
    b2World* m_world = nullptr;
    LevelTheme m_theme{LevelTheme::OVERWORLD};
    std::vector<std::unique_ptr<Entity>> m_pending;

    static constexpr float CRUISE_TIME = 2.5f;
    static constexpr float HIDE_TIME = 1.0f;
    static constexpr float THROW_TIME = 0.4f;
    static constexpr float TRACK_SPEED = 120.f;
    static constexpr float TRACK_GAIN = 1.5f;
    static constexpr float FLY_Y = 64.f;
    /// Respawns once the falling corpse sinks past this line (still above
    /// Level's out-of-bounds cleanup so the entity survives).
    static constexpr float RESPAWN_Y = 500.f;
};

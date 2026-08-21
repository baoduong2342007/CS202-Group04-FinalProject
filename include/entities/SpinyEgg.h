/**
 * @file SpinyEgg.h
 * @brief Spiny egg thrown by Lakitu: bounces once, then hatches into a Spiny
 * @note Enemy expansion - SMB1-accurate: contact harms Mario, fireballs
 *       destroy it, and a fuse guarantees hatching even in mid-air.
 */

#pragma once

#include "core/LevelCatalog.h"
#include "entities/Enemy.h"

class SpinyEgg : public Enemy {
public:
    SpinyEgg(const sf::Vector2f& position,
             b2World* world,
             LevelTheme theme,
             Direction throwDirection);
    ~SpinyEgg() override = default;

    void update(float dt) override;

    void patrol() override;
    void onStomp() override;
    void onWallCollision() override;
    void onFireHit() override;

    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::SPINY_EGG; }
    Capabilities getCapabilities() const noexcept override {
        return Enemy::getCapabilities() & ~capability(Capability::STOMPABLE);
    }
    bool isDying() const override {
        return m_isFlippedDead || isDead() || !isActive();
    }

    bool hasHatched() const { return m_hatched; }

    std::vector<std::unique_ptr<Entity>> takePendingSpawns() override;

private:
    enum class Stage {
        FLYING,   ///< Initial thrown arc
        BOUNCING, ///< Recoil hop after the first landing
        HATCH     ///< Cracked open: hand the Spiny to Level and leave
    };

    void hatch();

    Stage m_stage{Stage::FLYING};
    Direction m_throwDirection;
    LevelTheme m_theme{LevelTheme::OVERWORLD};
    bool m_wasFalling{false};
    float m_fuse{0.f};
    bool m_isFlippedDead{false};
    bool m_hatched{false};
    std::vector<std::unique_ptr<Entity>> m_pending;

    static constexpr float THROW_VX = 60.f;
    static constexpr float THROW_VY = -200.f;
    static constexpr float BOUNCE_VY = -140.f;
    static constexpr float FALLING_THRESHOLD = 40.f;
    static constexpr float LANDED_THRESHOLD = 10.f;
    static constexpr float FUSE_TIME = 4.f;
    static constexpr float PIT_CLEANUP_Y = 800.f;
};

/**
 * @file BowserFire.h
 * @brief Bowser's fire breath: a horizontal wave of flame
 * @note Enemy expansion - SMB1-accurate: travels straight ahead with a
 *       sinusoidal vertical wave and cannot be blocked or destroyed.
 */

#pragma once

#include "core/LevelCatalog.h"
#include "entities/Character.h"
#include "entities/EnemyProjectile.h"

class BowserFire : public EnemyProjectile {
public:
    BowserFire(const sf::Vector2f& position,
               b2World* world,
               LevelTheme theme,
               Direction direction);
    ~BowserFire() override = default;

    void update(float dt) override;

    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::BOWSER_FIRE; }

private:
    void updatePresentation(float dt) override;

    Direction m_direction;
    float m_waveTime{0.f};
    float m_baseY{0.f};

    static constexpr float SPEED = 180.f;
    static constexpr float WAVE_AMPLITUDE = 6.f;
    static constexpr float WAVE_FREQUENCY = 1.0f;   ///< Hz
    static constexpr float FRAME_DURATION = 0.08f;
};

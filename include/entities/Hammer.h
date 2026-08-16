/**
 * @file Hammer.h
 * @brief Claw hammer thrown by Hammer Bros (and hammer-throwing Bowsers)
 * @note Enemy expansion - SMB1-accurate: gravity arc, spins counter-
 *       clockwise, flies over terrain, harms Mario on contact.
 */

#pragma once

#include "entities/Character.h"
#include "entities/EnemyProjectile.h"

class Hammer : public EnemyProjectile {
public:
    Hammer(const sf::Vector2f& position,
           b2World* world,
           Direction direction);
    ~Hammer() override = default;

    void update(float dt) override;

    bool isHammer() const { return true; }

private:
    void updatePresentation(float dt) override;

    float m_rotation{0.f};

    static constexpr float THROW_VX = 130.f;
    static constexpr float THROW_VY = -420.f;
    static constexpr float SPIN_DEGREES_PER_SECOND = -540.f;
};

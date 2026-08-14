/**
 * @file ScorePopup.h
 * @brief Short-lived world-space score feedback for accepted enemy stomps.
 */

#pragma once

#include "entities/Entity.h"

class ScorePopup : public Entity {
public:
    ScorePopup(const sf::Vector2f& position, int points, bool oneUp = false);
    ~ScorePopup() override = default;

    void update(float dt) override;
    EntityType getType() const override { return EntityType::TERRAIN; }

    int getDisplayedPoints() const { return m_points; }
    bool isOneUp() const { return m_oneUp; }

    /// Resolve an award to the shared items-atlas frame used for rendering.
    static const sf::IntRect& frameFor(int points, bool oneUp = false);

private:
    int m_points;
    bool m_oneUp;
    float m_lifetime;
};

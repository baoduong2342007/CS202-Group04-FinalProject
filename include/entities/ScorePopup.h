/**
 * @file ScorePopup.h
 * @brief Short-lived world-space score feedback for accepted enemy stomps and defeats.
 */

#pragma once

#include "entities/Entity.h"
#include "core/LevelCatalog.h"

class ScorePopup : public Entity {
public:
    ScorePopup(const sf::Vector2f& position, int points, bool oneUp = false, LevelTheme theme = LevelTheme::OVERWORLD);
    ~ScorePopup() override = default;

    void update(float dt) override;
    EntityType getType() const override { return EntityType::TERRAIN; }

    int getDisplayedPoints() const { return m_points; }
    bool isOneUp() const { return m_oneUp; }
    LevelTheme getTheme() const { return m_theme; }

    /// Resolve an award to the shared items-atlas frame used for rendering.
    static const sf::IntRect& frameFor(int points, bool oneUp = false, LevelTheme theme = LevelTheme::OVERWORLD);

private:
    int m_points;
    bool m_oneUp;
    LevelTheme m_theme;
    float m_lifetime;
};

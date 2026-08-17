/**
 * @file Firebar.h
 * @author TV4 (Vy)
 * @brief Rotating firebar hazard in Castle levels
 * @note Week 4 / Sprint 7 - authentic SMB1 castle obstacle
 */

#pragma once

#include "core/LevelCatalog.h"
#include "entities/Enemy.h"
#include <vector>
#include <SFML/Graphics/Sprite.hpp>

class Firebar : public Enemy {
public:
    Firebar(const sf::Vector2f& position,
            b2World* world,
            LevelTheme theme = LevelTheme::CASTLE,
            float initialAngle = 0.f,
            float rotationSpeed = 2.0f,
            int ballCount = 5);
    ~Firebar() override = default;

    void update(float dt) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void patrol() override {}
    void onStomp() override {}
    void onWallCollision() override {}
    void onFireHit() override {}

    bool isFirebar() const override { return true; }
    bool isIndestructible() const override { return true; }
    bool canBeStomped() const override { return false; }
    bool isDying() const override { return false; }

    /// Returns true if the given bounding box touches any of the rotating fireballs.
    bool checkMarioCollision(const sf::FloatRect& marioBox) const;

    float getAngle() const { return m_angle; }
    int getBallCount() const { return m_ballCount; }

private:
    float m_angle{0.f};
    float m_rotationSpeed{2.0f};
    int m_ballCount{5};
    float m_ballSpacing{16.f};
    float m_animTimer{0.f};
    int m_animFrame{0};

    sf::Vector2f m_center;
    std::vector<sf::Vector2f> m_ballPositions;
    std::unique_ptr<sf::Sprite> m_ballSprite;
};

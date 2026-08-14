/**
 * @file CheepCheep.h
 * @author TV4 (Vy) & CS202 Team
 * @brief Cheep Cheep fish enemy with swimming (underwater) and jumping (bridge) behaviors
 * @note Adheres strictly to NES Super Mario Bros canon mechanics
 */

#pragma once

#include "core/LevelCatalog.h"
#include "entities/Enemy.h"

#include <SFML/System/Vector2.hpp>

enum class CheepCheepBehavior {
    SWIMMING,
    JUMPING
};

enum class CheepCheepColor {
    GREEN,
    RED
};

class TileMap;

class CheepCheep : public Enemy {
public:
    CheepCheep(const sf::Vector2f& position,
               b2World* world,
               LevelTheme theme = LevelTheme::OVERWORLD,
               CheepCheepBehavior behavior = CheepCheepBehavior::SWIMMING,
               CheepCheepColor color = CheepCheepColor::GREEN,
               const sf::Vector2f& initialVelocity = {0.f, 0.f});

    ~CheepCheep() override = default;

    void update(float dt) override;

    void onStomp() override;
    void patrol() override;
    void onWallCollision() override;
    void onFireHit() override;

    bool isCheepCheep() const override { return true; }
    bool isDying() const override {
        return m_isFlippedDead || isDead() || !isActive();
    }

    bool canBeStomped() const {
        return m_behavior == CheepCheepBehavior::JUMPING && !m_isFlippedDead && !isDead();
    }

    CheepCheepBehavior getBehavior() const { return m_behavior; }
    CheepCheepColor getColor() const { return m_color; }

    enum class RouteAxis {
        NONE,
        VERTICAL,
        HORIZONTAL
    };

    void setTileMap(const TileMap* tileMap) override;
    void setSwimSpeed(float speed) { m_swimSpeed = speed; }
    void setWaveParameters(float amplitude, float frequency);
    void setRoute(const sf::Vector2f& start, const sf::Vector2f& end, float speed = 40.0f);

    RouteAxis getRouteAxis() const { return m_routeAxis; }
    bool hasRoute() const { return m_routeAxis != RouteAxis::NONE; }

private:
    void initAnimations(LevelTheme theme);
    void reverseDirection();
    void syncSprite();

    CheepCheepBehavior m_behavior;
    CheepCheepColor m_color;
    LevelTheme m_theme;

    const TileMap* m_tileMap = nullptr;

    bool m_isFlippedDead = false;
    float m_swimSpeed = 50.f;
    float m_waveTime = 0.f;
    float m_waveAmplitude = 0.f;
    float m_waveFrequency = 0.f;
    float m_baseY = 0.f;

    RouteAxis m_routeAxis = RouteAxis::NONE;
    sf::Vector2f m_routeStart{0.f, 0.f};
    sf::Vector2f m_routeEnd{0.f, 0.f};
    int m_routeDirection = 1;

    static constexpr float DEFAULT_GREEN_SWIM_SPEED = 50.f;
    static constexpr float DEFAULT_RED_SWIM_SPEED = 75.f;
    static constexpr float DEFAULT_WAVE_AMPLITUDE = 16.f;
    static constexpr float DEFAULT_WAVE_FREQUENCY = 3.0f;
    static constexpr float PIT_CLEANUP_Y = 900.f;
};

/**
 * @file Elevator.h
 * @author TV1 (Dương)
 * @brief Moving platform / lift entity driven by a Box2D kinematic body
 * @note Travels between two level markers with optional pause at each end;
 *       kinematic bodies carry Mario and enemies natively via the solver.
 */

#pragma once

#include "entities/Entity.h"
#include "core/LevelCatalog.h"

class Elevator : public Entity {
public:
    enum class Axis {
        VERTICAL,
        HORIZONTAL
    };

    // 1. Constructor / Destructor
    Elevator(const sf::Vector2f& startPosition,
             const sf::Vector2f& endPosition,
             float speedPixelsPerSecond,
             Axis axis,
             float pauseSeconds = 0.0f,
             LevelTheme theme = LevelTheme::OVERWORLD);
    ~Elevator() override = default;

    // 2. Override methods
    void initPhysics(b2World* world, b2BodyType type, const sf::Vector2f& size, bool isSensor = false) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    // 3. Public methods
    void initTheme(LevelTheme theme);

    // 4. Getters / Setters
    EntityType getType() const override { return EntityType::ELEVATOR; }
    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::ELEVATOR; }
    Axis getAxis() const { return m_axis; }
    float getSpeed() const { return m_speedPixels; }
    bool isPaused() const { return m_pauseRemaining > 0.0f; }
    /// +1 while moving toward the end marker, -1 while moving toward the start.
    int getDirection() const { return m_direction; }

    static constexpr float DEFAULT_SPEED = 60.0f; // px/s
    static constexpr float DEFAULT_PAUSE = 1.0f;  // seconds

private:
    sf::Vector2f m_start;
    sf::Vector2f m_end;
    Axis m_axis;
    float m_speedPixels;
    float m_pauseRemaining = 0.0f;
    float m_pauseDuration = 0.0f;
    int m_direction = 1;

    sf::IntRect m_platformRect;
};

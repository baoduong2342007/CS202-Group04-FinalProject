/**
 * @file PhysicsEngine.h
 * @author TV3 (Bảo)
 * @brief PhysicsEngine singleton managing the Box2D simulation world
 * @note Week 2 Box2D integration
 */

#pragma once

#include <box2d/box2d.h>
#include <SFML/System/Vector2.hpp>

class PhysicsEngine {
public:
    PhysicsEngine() = default;
    ~PhysicsEngine() = default;

    /// Advances the fixed-step simulation with a max substep clamp of 5 per frame.
    static bool update(b2World& world, float dt, float& timeAccumulator);
    
    /// Backward-compatible update overload using instance accumulator
    bool update(b2World& world, float dt) { return update(world, dt, m_timeAccumulator); }
    void resetAccumulator() { m_timeAccumulator = 0.0f; }

    static constexpr int MAX_SUBSTEPS = 5;

    // Pixel to Meter conversions (e.g. 30 pixels = 1 meter)
    static constexpr float PPM = 30.0f;
    
    static inline float pixelsToMeters(float pixels) { return pixels / PPM; }
    static inline float metersToPixels(float meters) { return meters * PPM; }
    static inline b2Vec2 pixelsToMeters(const sf::Vector2f& pixels) { return b2Vec2(pixels.x / PPM, pixels.y / PPM); }
    static inline sf::Vector2f metersToPixels(const b2Vec2& meters) { return sf::Vector2f(meters.x * PPM, meters.y * PPM); }

private:
    float m_timeAccumulator = 0.0f;
    static constexpr float TIME_STEP = 1.0f / 60.0f;
    static constexpr int VELOCITY_ITERATIONS = 8;
    static constexpr int POSITION_ITERATIONS = 3;
};

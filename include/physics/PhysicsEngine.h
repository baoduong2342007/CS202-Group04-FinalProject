/**
 * @file PhysicsEngine.h
 * @author TV3 (Bảo)
 * @brief PhysicsEngine singleton managing the Box2D simulation world
 * @note Week 2 Box2D integration
 */

#pragma once

#include <box2d/box2d.h>
#include <SFML/System/Vector2.hpp>
#include "core/DisplayConfig.h"

class PhysicsEngine {
public:
    /// Advances the fixed-step simulation and reports whether at least one
    /// Box2D step occurred. Callers can then refresh contact-derived state.
    static bool update(b2World& world, float dt);

    // Pixel to Meter conversions (e.g. 30 pixels = 1 meter)
    static constexpr float PPM = 30.0f;
    
    static inline float pixelsToMeters(float pixels) { return pixels / PPM; }
    static inline float metersToPixels(float meters) { return meters * PPM; }
    static inline b2Vec2 pixelsToMeters(const sf::Vector2f& pixels) { return b2Vec2(pixels.x / PPM, pixels.y / PPM); }
    static inline sf::Vector2f metersToPixels(const b2Vec2& meters) { return sf::Vector2f(meters.x * PPM, meters.y * PPM); }

private:
    PhysicsEngine() = default;
    ~PhysicsEngine() = default;

    static float s_timeAccumulator;
    static constexpr float TIME_STEP = 1.0f / static_cast<float>(DisplayConfig::FRAMERATE_LIMIT);
    static constexpr int VELOCITY_ITERATIONS = 8;
    static constexpr int POSITION_ITERATIONS = 3;
};

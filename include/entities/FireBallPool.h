/**
 * @file FireBallPool.h
 * @author TV3 (Bảo)
 * @brief Object Pool for FireBall projectiles to eliminate runtime dynamic allocation overhead
 * @note Level 1B Optimization
 */

#pragma once

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "entities/FireBall.h"

class FireBallPool {
public:
    // ============================================================
    // PATTERN: Object Pool
    // Reason: Pre-allocates FireBall objects to avoid new/delete memory overhead
    // ============================================================

    explicit FireBallPool(size_t capacity = 2);
    ~FireBallPool() = default;

    /// Initialize physics bodies for all pooled fireballs
    void init(b2World* world);

    /// Acquire an inactive FireBall from pool and spawn at position
    FireBall* acquire(const sf::Vector2f& position, Direction direction, b2World* world);

    /// Update active fireballs in pool
    void update(float dt);

    /// Render active fireballs in pool
    void render(sf::RenderWindow& window);

    /// Get active fireball count
    size_t getActiveCount() const;

    /// Clear and destroy physics bodies
    void clear();

private:
    std::vector<std::unique_ptr<FireBall>> m_pool;
};

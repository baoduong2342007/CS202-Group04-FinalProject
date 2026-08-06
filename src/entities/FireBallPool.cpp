/**
 * @file FireBallPool.cpp
 * @author TV3 (Bảo)
 * @brief Implementation of FireBallPool object pool for projectile reuse
 */

#include "entities/FireBallPool.h"

FireBallPool::FireBallPool(size_t capacity)
    : m_capacity(capacity) {
    m_pool.reserve(capacity);
    for (size_t i = 0; i < capacity; ++i) {
        auto fb = std::make_unique<FireBall>();
        fb->setActive(false);
        m_pool.push_back(std::move(fb));
    }
}

void FireBallPool::init(b2World* world) {
    (void)world; // Pre-initialization optional; bodies are initialized on first spawn
}

FireBall* FireBallPool::acquire(const sf::Vector2f& position, Direction direction, b2World* world) {
    for (auto& fb : m_pool) {
        if (!fb->isActive() && !fb->isPendingDestroy()) {
            fb->spawn(position, direction, world);
            return fb.get();
        }
    }
    return nullptr; // Pool exhausted (max active count reached)
}

void FireBallPool::update(float dt) {
    for (auto& fb : m_pool) {
        if (fb->isActive()) {
            fb->update(dt);
        }
    }
}

void FireBallPool::render(sf::RenderWindow& window) {
    for (const auto& fb : m_pool) {
        if (fb->isActive()) {
            window.draw(*fb);
        }
    }
}

size_t FireBallPool::getActiveCount() const {
    size_t count = 0;
    for (const auto& fb : m_pool) {
        if (fb->isActive()) {
            count++;
        }
    }
    return count;
}

void FireBallPool::clear() {
    for (auto& fb : m_pool) {
        fb->deactivate();
        fb->destroyPhysicsBody();
    }
}

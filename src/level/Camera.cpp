/**
 * @file Camera.cpp
 * @author TV2 (Nhật)
 * @brief Implementation of the Camera class for tracking and boundary clamping.
 * @note Uses SFML 3's position/size vectors for FloatRect and std::clamp for bounds.
 */

#include "level/Camera.h"
#include <algorithm>
#include <cstdlib> // For std::rand()

namespace {
    constexpr int RANDOM_PERCENT = 100;
    constexpr float HALF_PERCENT = 50.f;
    constexpr float SHAKE_OFFSET = 1.f;
}

// ============================================================
// 1. Constructor
// ============================================================
Camera::Camera() = default;

// ============================================================
// 3. Public methods
// ============================================================
void Camera::init(const sf::Vector2f& viewSize, const sf::FloatRect& levelBounds) {
    m_view.setSize(viewSize);
    m_levelBounds = levelBounds;
    
    // Set an initial center to avoid a 0,0 snap on the very first frame
    m_view.setCenter({viewSize.x / 2.0f, viewSize.y / 2.0f});
}

void Camera::shake(float duration, float magnitude) {
    m_shakeTimer = duration;
    m_shakeMagnitude = magnitude;
}

void Camera::update(float dt, const sf::Vector2f& targetPosition) {
    sf::Vector2f currentCenter = m_view.getCenter();
    sf::Vector2f newCenter = currentCenter;

    // Strict horizontal tracking (1:1 lock)
    newCenter.x = targetPosition.x;

    // Vertical deadzone tracking
    // Only adjust Y if the target moves beyond the center 50% of the screen vertically.
    const float verticalDeadzone = m_view.getSize().y * 0.25f;

    if (targetPosition.y < currentCenter.y - verticalDeadzone) {
        newCenter.y = targetPosition.y + verticalDeadzone;
    } else if (targetPosition.y > currentCenter.y + verticalDeadzone) {
        newCenter.y = targetPosition.y - verticalDeadzone;
    }

    m_view.setCenter(newCenter);
    clampToBoundaries();

    // Apply screen shake if active
    if (m_shakeTimer > 0.f) {
        m_shakeTimer -= dt;
        
        // Generate random offsets between -m_shakeMagnitude and +m_shakeMagnitude
        float offsetX = ((std::rand() % RANDOM_PERCENT) / HALF_PERCENT - SHAKE_OFFSET) * m_shakeMagnitude;
        float offsetY = ((std::rand() % RANDOM_PERCENT) / HALF_PERCENT - SHAKE_OFFSET) * m_shakeMagnitude;
        
        sf::Vector2f shakenCenter = m_view.getCenter();
        shakenCenter.x += offsetX;
        shakenCenter.y += offsetY;
        
        m_view.setCenter(shakenCenter);
    }
}

// ============================================================
// 4. Getters
// ============================================================
const sf::View& Camera::getView() const {
    return m_view;
}

// ============================================================
// 5. Private methods
// ============================================================
void Camera::clampToBoundaries() {
    sf::Vector2f currentCenter = m_view.getCenter();
    sf::Vector2f halfSize = m_view.getSize() / 2.0f;

    // SFML 3 uses .position and .size instead of .left, .top, .width, .height
    float minX = m_levelBounds.position.x + halfSize.x;
    float maxX = m_levelBounds.position.x + m_levelBounds.size.x - halfSize.x;
    
    float minY = m_levelBounds.position.y + halfSize.y;
    float maxY = m_levelBounds.position.y + m_levelBounds.size.y - halfSize.y;

    // Handle X bounds
    if (minX > maxX) {
        // Failsafe: Level is narrower than screen width
        currentCenter.x = m_levelBounds.position.x + (m_levelBounds.size.x / 2.0f);
    } else {
        // Normal clamping
        currentCenter.x = std::clamp(currentCenter.x, minX, maxX);
    }

    // Handle Y bounds
    if (minY > maxY) {
        // Failsafe: Level is shorter than screen height
        currentCenter.y = m_levelBounds.position.y + (m_levelBounds.size.y / 2.0f);
    } else {
        // Normal clamping
        currentCenter.y = std::clamp(currentCenter.y, minY, maxY);
    }

    m_view.setCenter(currentCenter);
}
/**
 * @file Camera.cpp
 * @author TV2 (Nhật)
 * @brief Implementation of the Camera class for tracking and boundary clamping.
 * @note Uses SFML 3's position/size vectors for FloatRect and std::clamp for bounds.
 */

#include "level/Camera.h"
#include <algorithm>

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

void Camera::update(const sf::Vector2f& targetPosition) {
    m_view.setCenter(targetPosition);
    clampToBoundaries();
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
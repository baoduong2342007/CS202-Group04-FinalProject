/**
 * @file Camera.cpp
 * @author TV2 (Nhật)
 * @brief Implementation of the Camera class for tracking and boundary clamping.
 * @note Uses SFML 3's position/size vectors for FloatRect and std::clamp for
 * bounds.
 */

#include "level/Camera.h"

#include <algorithm>
#include <random>

namespace {
    std::mt19937 &getShakeRng() {
      static std::mt19937 rng(std::random_device{}());
      return rng;
    }

    /**
     * @brief The ratio of the view width used as a horizontal deadzone.
     * @details A value of 0.05 means 5% of the screen width on either side of the center.
     */
    constexpr float HORIZONTAL_DEADZONE_RATIO = 0.05f;

    /**
     * @brief The minimum distance kept between the target and the view's top edge.
     * @details The camera only pans vertically once the target is closer than
     * 32 logical pixels to the top edge, preserving more of the ground view
     * while Mario is jumping.
     */
    constexpr float TOP_FOLLOW_DISTANCE = 32.0f;
} // namespace

// ============================================================
// 1. Constructor
// ============================================================
Camera::Camera() = default;

// ============================================================
// 3. Public methods
// ============================================================
void Camera::init(const sf::Vector2f &viewSize,
                  const sf::FloatRect &levelBounds) {
  m_view.setSize(viewSize);
  m_levelBounds = levelBounds;

  // Release maps rest against their bottom edge. Small worlds are centered by
  // clampCenter() on either axis. Keep this initial position as the vertical
  // reference that the camera returns to after following a high jump.
  m_originalCenter = clampCenter({
      levelBounds.position.x + viewSize.x / 2.0f,
      levelBounds.position.y + levelBounds.size.y - viewSize.y / 2.0f});
  m_stableCenter = m_originalCenter;
  m_view.setCenter(m_stableCenter);
}

void Camera::shake(float duration, float magnitude) {
  m_shakeTimer = duration;
  m_shakeMagnitude = magnitude;
}

void Camera::update(float dt, const sf::Vector2f &targetPosition) {
  sf::Vector2f newCenter = m_stableCenter;

  // Horizontal deadzone tracking (5% of screen width)
  const float horizontalDeadzone = m_view.getSize().x * HORIZONTAL_DEADZONE_RATIO;
  if (targetPosition.x < m_stableCenter.x - horizontalDeadzone) {
    newCenter.x = targetPosition.x + horizontalDeadzone;
  } else if (targetPosition.x > m_stableCenter.x + horizontalDeadzone) {
    newCenter.x = targetPosition.x - horizontalDeadzone;
  }

  if (m_verticalMode == CameraVerticalMode::LOCKED) {
    newCenter.y = m_levelBounds.position.y + m_levelBounds.size.y -
                  m_view.getSize().y / 2.0f;
  } else {
    const float originalTop =
        m_originalCenter.y - m_view.getSize().y / 2.0f;
    const float followThreshold = originalTop + TOP_FOLLOW_DISTANCE;
    if (targetPosition.y < followThreshold) {
      newCenter.y = targetPosition.y + m_view.getSize().y / 2.0f -
                    TOP_FOLLOW_DISTANCE;
    } else {
      newCenter.y = m_originalCenter.y;
    }
  }

  m_stableCenter = clampCenter(newCenter);
  sf::Vector2f renderCenter = m_stableCenter;

  // Apply screen shake if active BEFORE clamping
  if (m_shakeTimer > 0.f) {
    m_shakeTimer -= dt;

    std::uniform_real_distribution<float> dist(-m_shakeMagnitude,
                                               m_shakeMagnitude);
    float offsetX = dist(getShakeRng());
    float offsetY = dist(getShakeRng());

    renderCenter.x += offsetX;
    renderCenter.y += offsetY;
  }

  m_view.setCenter(clampCenter(renderCenter));
}

// ============================================================
// 4. Getters
// ============================================================
const sf::View &Camera::getView() const { return m_view; }

// ============================================================
// 5. Private methods
// ============================================================
void Camera::clampToBoundaries() {
  m_view.setCenter(clampCenter(m_view.getCenter()));
}

sf::Vector2f Camera::clampCenter(const sf::Vector2f& center) const {
  sf::Vector2f currentCenter = center;
  sf::Vector2f halfSize = m_view.getSize() / 2.0f;

  float minX = m_levelBounds.position.x + halfSize.x;
  float maxX = m_levelBounds.position.x + m_levelBounds.size.x - halfSize.x;

  if (minX > maxX) {
    currentCenter.x = m_levelBounds.position.x + (m_levelBounds.size.x / 2.0f);
  } else {
    currentCenter.x = std::clamp(currentCenter.x, minX, maxX);
  }

  const float minY = m_levelBounds.position.y + halfSize.y;
  const float maxY = m_levelBounds.position.y + m_levelBounds.size.y - halfSize.y;
  if (minY > maxY) {
    currentCenter.y = m_levelBounds.position.y + m_levelBounds.size.y / 2.0f;
  } else {
    currentCenter.y = std::clamp(currentCenter.y, minY, maxY);
  }

  return currentCenter;
}

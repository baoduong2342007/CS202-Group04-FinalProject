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
     * @brief The padding below the bottom of the level bounds.
     * @details Zero keeps the camera's resting bottom edge flush with the map.
     */
    constexpr float BOTTOM_TILE_PADDING = 0.0f;

    /**
     * @brief The fixed distance maintained between the target and the top of the screen when tracking upwards.
     */
    constexpr float TOP_BUFFER_PADDING = 40.0f;
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

  // Set an initial center to avoid a 0,0 snap on the very first frame
  m_view.setCenter({viewSize.x / 2.0f, viewSize.y / 2.0f});
}

void Camera::shake(float duration, float magnitude) {
  m_shakeTimer = duration;
  m_shakeMagnitude = magnitude;
}

void Camera::update(float dt, const sf::Vector2f &targetPosition) {
  sf::Vector2f currentCenter = m_view.getCenter();
  sf::Vector2f newCenter = currentCenter;

  // Horizontal deadzone tracking (5% of screen width)
  const float horizontalDeadzone = m_view.getSize().x * HORIZONTAL_DEADZONE_RATIO;
  if (targetPosition.x < currentCenter.x - horizontalDeadzone) {
    newCenter.x = targetPosition.x + horizontalDeadzone;
  } else if (targetPosition.x > currentCenter.x + horizontalDeadzone) {
    newCenter.x = targetPosition.x - horizontalDeadzone;
  }

  // Vertical tracking: Rigid top buffer with an absolute baseline
  // Calculate the default camera Y position so that the bottom of the screen
  // is flush with the bottom of the level.
  float defaultCenterY = m_levelBounds.position.y + m_levelBounds.size.y + BOTTOM_TILE_PADDING - (m_view.getSize().y / 2.0f);

  // The camera strictly maintains a 40-pixel buffer between Mario and the TOP of the screen
  const float topOffset = m_view.getSize().y / 2.0f - TOP_BUFFER_PADDING;
  
  // Calculate where the camera *wants* to be to maintain that 40px top buffer
  newCenter.y = targetPosition.y + topOffset;

  // But lock the camera so it NEVER drops below the default resting position.
  // This means the camera ONLY moves if Mario jumps high enough to push against 
  // that 40-pixel top buffer. If he falls, it tracks him exactly at that buffer 
  // until it gracefully hits the baseline and stops.
  newCenter.y = std::min(newCenter.y, defaultCenterY);

  m_view.setCenter(newCenter);

  // Apply screen shake if active BEFORE clamping
  if (m_shakeTimer > 0.f) {
    m_shakeTimer -= dt;

    std::uniform_real_distribution<float> dist(-m_shakeMagnitude,
                                               m_shakeMagnitude);
    float offsetX = dist(getShakeRng());
    float offsetY = dist(getShakeRng());

    sf::Vector2f shakenCenter = m_view.getCenter();
    shakenCenter.x += offsetX;
    shakenCenter.y += offsetY;

    m_view.setCenter(shakenCenter);
  }

  // Clamp to boundaries AFTER all modifications
  clampToBoundaries();
}

// ============================================================
// 4. Getters
// ============================================================
const sf::View &Camera::getView() const { return m_view; }

// ============================================================
// 5. Private methods
// ============================================================
void Camera::clampToBoundaries() {
  sf::Vector2f currentCenter = m_view.getCenter();
  sf::Vector2f halfSize = m_view.getSize() / 2.0f;

  float minX = m_levelBounds.position.x + halfSize.x;
  float maxX = m_levelBounds.position.x + m_levelBounds.size.x - halfSize.x;

  // Handle X bounds
  if (minX > maxX) {
    currentCenter.x = m_levelBounds.position.x + (m_levelBounds.size.x / 2.0f);
  } else {
    currentCenter.x = std::clamp(currentCenter.x, minX, maxX);
  }

  // Y bounds clamping remains disabled so vertical tracking can follow jumps;
  // the resting baseline above keeps the bottom edge aligned with the map.

  m_view.setCenter(currentCenter);
}

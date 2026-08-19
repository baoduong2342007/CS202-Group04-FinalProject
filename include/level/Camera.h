/**
 * @file Camera.h
 * @author TV2 (Nhật)
 * @brief Manages sf::View to track the player with boundary clamping against level limits.
 * @note Designed for SFML 3. Clamps camera center to prevent rendering out-of-bounds areas.
 */

#pragma once

#include <SFML/Graphics.hpp>
#include "core/LevelCatalog.h"

class Camera {
public:
    // 1. Constructor / Destructor
    Camera();
    ~Camera() = default;

    // Prevent copying to avoid unintended view state duplication
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;

    // 2. Override methods
    // (None currently required for this class)

    // 3. Public methods (alphabetical)
    /**
     * @brief Configures the camera's initial size and the bounds it cannot cross.
     * @param viewSize The width and height of the camera view (usually matching screen resolution).
     * @param levelBounds The total width and height of the current level map.
     */
    void init(const sf::Vector2f& viewSize, const sf::FloatRect& levelBounds);

    /**
     * @brief Applies a shake effect to the camera.
     * @param duration How long the shake should last in seconds.
     * @param magnitude The maximum pixel offset for the shake.
     */
    void shake(float duration, float magnitude);

    void setVerticalMode(CameraVerticalMode mode) { m_verticalMode = mode; }

    /**
     * @brief Updates the camera's center position to follow a target.
     * @details Tracks the X-axis continuously. In DEAD_ZONE mode, the camera
     *          follows targets that rise within 32 logical pixels of the
     *          original view's top edge and returns to that original view
     *          when the target comes back down.
     * @param dt The time elapsed since the last update.
     * @param targetPosition The current X and Y coordinates of the entity to track (e.g., Mario).
     */
    void update(float dt, const sf::Vector2f& targetPosition);

    // 4. Getters / Setters
    /**
     * @brief Retrieves the internal sf::View to be applied to the sf::RenderWindow.
     * @return const sf::View& reference to the updated view for the current frame.
     */
    const sf::View& getView() const;
    CameraVerticalMode getVerticalMode() const { return m_verticalMode; }
    void setHorizontalDeadzoneRatio(float ratio) { m_horizontalDeadzoneRatio = ratio; }
    float getHorizontalDeadzoneRatio() const { return m_horizontalDeadzoneRatio; }

private:
    // 5. Private methods
    /**
     * @brief Adjusts the view's center point so its edges never exceed m_levelBounds.
     */
    void clampToBoundaries();
    sf::Vector2f clampCenter(const sf::Vector2f& center) const;

    // 6. Private members
    sf::FloatRect m_levelBounds;
    sf::View m_view;
    sf::Vector2f m_originalCenter{0.f, 0.f};
    sf::Vector2f m_stableCenter{0.f, 0.f};
    CameraVerticalMode m_verticalMode = CameraVerticalMode::LOCKED;
    float m_horizontalDeadzoneRatio = 0.05f;

    float m_shakeTimer = 0.f;
    float m_shakeMagnitude = 0.f;
};

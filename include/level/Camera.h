#pragma once

/**
 * @file Camera.h
 * @author TV2 (Nhật)
 * @brief Manages sf::View to track the player with boundary clamping against level limits.
 * @note Designed for SFML 3. Clamps camera center to prevent rendering out-of-bounds areas.
 */

#include <SFML/Graphics.hpp>

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
     * @brief Updates the camera's center position to follow a target.
     * @param targetPosition The current X and Y coordinates of the entity to track (e.g., Mario).
     */
    void update(const sf::Vector2f& targetPosition);

    // 4. Getters / Setters
    /**
     * @brief Retrieves the internal sf::View to be applied to the sf::RenderWindow.
     * @return const sf::View& reference to the updated view for the current frame.
     */
    const sf::View& getView() const;

private:
    // 5. Private methods
    /**
     * @brief Adjusts the view's center point so its edges never exceed m_levelBounds.
     */
    void clampToBoundaries();

    // 6. Private members
    sf::FloatRect m_levelBounds;
    sf::View m_view;
};
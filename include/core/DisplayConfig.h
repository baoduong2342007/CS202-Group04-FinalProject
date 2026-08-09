/**
 * @file DisplayConfig.h
 * @author TV2
 * @brief Global display and rendering configurations
 * @note Contains constants for window resolution, logical resolution, and frame rate.
 */

#pragma once

#include <SFML/Graphics/Color.hpp>

namespace DisplayConfig {
    /**
     * @brief The physical width of the application window in pixels.
     */
    constexpr unsigned int WINDOW_WIDTH = 1280;

    /**
     * @brief The physical height of the application window in pixels.
     */
    constexpr unsigned int WINDOW_HEIGHT = 720;

    /**
     * @brief The internal logical rendering width. 
     * @details The game is drawn to a texture of this size, which is then scaled up to fit the window.
     */
    constexpr unsigned int LOGICAL_WIDTH = 640;

    /**
     * @brief The internal logical rendering height.
     */
    constexpr unsigned int LOGICAL_HEIGHT = 360;

    /**
     * @brief The maximum allowed frame rate (FPS) for the application.
     */
    constexpr unsigned int FRAMERATE_LIMIT = 60;

    /**
     * @brief The default background color used to clear the render texture (Sky Blue).
     */
    const sf::Color BACKGROUND_COLOR(69, 197, 250);

} // namespace DisplayConfig

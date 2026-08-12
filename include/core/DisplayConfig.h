/**
 * @file DisplayConfig.h
 * @author TV2
 * @brief Global display and rendering configurations
 * @note Contains constants for window resolution, logical resolution, and frame rate.
 */

#pragma once

#include <algorithm>
#include <optional>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

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

    struct IntegerViewport {
        sf::Vector2i position;
        sf::Vector2u size;
        unsigned int scale = 1;
    };

    /// Pixel-perfect content rectangle centered inside the physical window.
    inline IntegerViewport calculateIntegerViewport(
        const sf::Vector2u& windowSize) {
        const unsigned int scaleX = windowSize.x / LOGICAL_WIDTH;
        const unsigned int scaleY = windowSize.y / LOGICAL_HEIGHT;
        const unsigned int scale = std::max(1u, std::min(scaleX, scaleY));
        const sf::Vector2u contentSize{
            LOGICAL_WIDTH * scale, LOGICAL_HEIGHT * scale};
        return {
            {(static_cast<int>(windowSize.x) -
              static_cast<int>(contentSize.x)) / 2,
             (static_cast<int>(windowSize.y) -
              static_cast<int>(contentSize.y)) / 2},
            contentSize,
            scale};
    }

    /// Maps a physical window pixel into the logical 640x360 canvas.
    /// Letterbox-bar clicks are deliberately rejected.
    inline std::optional<sf::Vector2i> mapPhysicalToLogical(
        const sf::Vector2i& physicalPosition,
        const sf::Vector2u& windowSize) {
        const IntegerViewport viewport = calculateIntegerViewport(windowSize);
        const int right = viewport.position.x +
                          static_cast<int>(viewport.size.x);
        const int bottom = viewport.position.y +
                           static_cast<int>(viewport.size.y);
        if (physicalPosition.x < viewport.position.x ||
            physicalPosition.y < viewport.position.y ||
            physicalPosition.x >= right || physicalPosition.y >= bottom) {
            return std::nullopt;
        }
        return sf::Vector2i{
            (physicalPosition.x - viewport.position.x) /
                static_cast<int>(viewport.scale),
            (physicalPosition.y - viewport.position.y) /
                static_cast<int>(viewport.scale)};
    }

} // namespace DisplayConfig

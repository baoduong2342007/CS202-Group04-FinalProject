/**
 * @file UILayoutHelper.h
 * @author TV2
 * @brief Helper for positioning UI elements based on standard anchor points.
 */

#pragma once

#include <SFML/Graphics.hpp>
#include "core/DisplayConfig.h"

enum class UIAnchor {
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

class UILayoutHelper {
public:
    /// Centers the origin of the text based on its local bounds.
    static void centerOrigin(sf::Text& text);
    
    /// Centers the origin of the sprite based on its local bounds.
    static void centerOrigin(sf::Sprite& sprite);
    
    /// Returns the absolute screen coordinates for the given anchor on the logical canvas.
    static sf::Vector2f getAnchorPosition(UIAnchor anchor);
    
    /// Sets the position of the text using the specified anchor and an optional offset.
    /// Automatically centers the origin of the text first.
    static void setPosition(sf::Text& text, UIAnchor anchor, const sf::Vector2f& offset = {0.f, 0.f});
    
    /// Sets the position of the sprite using the specified anchor and an optional offset.
    /// Automatically centers the origin of the sprite first.
    static void setPosition(sf::Sprite& sprite, UIAnchor anchor, const sf::Vector2f& offset = {0.f, 0.f});
};

/**
 * @file UIMenuWidget.h
 * @author TV2
 * @brief Interactive menu widget for handling keyboard and mouse selection.
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>
#include <optional>
#include <optional>
#include "patterns/InputState.h"
#include "ui/UILayoutHelper.h"

class UIMenuWidget {
public:
    UIMenuWidget(const sf::Font& font);

    /// Adds a menu item to the list.
    void addItem(const std::string& label, std::function<void()> onSelect, unsigned int characterSize = 16);

    /// Positions all menu items vertically starting from the specified anchor point.
    void setPosition(const sf::Vector2f& position, UIAnchor alignAnchor);
    
    /// Sets the vertical pixel spacing between menu items.
    void setSpacing(float spacing);

    /// Processes mouse hover and click events.
    void processEvents(const sf::Event& event);
    
    /// Processes keyboard navigation (Up/Down) and Enter to select.
    void processInput(const InputState& inputState);
    
    /// Updates the menu state (animations/timers).
    void update(float dt);
    
    /// Draws all menu items to the target.
    void draw(sf::RenderTarget& target) const;

    std::optional<sf::FloatRect> getItemBounds(std::size_t index) const;
    int getSelectedIndex() const { return m_selectedIndex; }

private:
    void updateLayout();
    void updateSelectionColors();

    struct MenuItem {
        sf::Text text;
        std::function<void()> onSelect;
    };

    std::vector<MenuItem> m_items;
    int m_selectedIndex = 0;
    float m_spacing = 30.f;
    const sf::Font& m_font;
    
    sf::Vector2f m_position{0.f, 0.f};
    UIAnchor m_alignAnchor{UIAnchor::Center};
};

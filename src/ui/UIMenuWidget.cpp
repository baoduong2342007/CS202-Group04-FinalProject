#include "ui/UIMenuWidget.h"
#include "core/DisplayConfig.h"

UIMenuWidget::UIMenuWidget(const sf::Font& font)
    : m_font(font) {}

void UIMenuWidget::addItem(const std::string& label, std::function<void()> onSelect) {
    MenuItem item{sf::Text(m_font, label, 16), std::move(onSelect)};
    
    m_items.push_back(std::move(item));
    updateLayout();
    updateSelectionColors();
}

void UIMenuWidget::setPosition(const sf::Vector2f& position, UIAnchor alignAnchor) {
    m_position = position;
    m_alignAnchor = alignAnchor;
    updateLayout();
}

void UIMenuWidget::setSpacing(float spacing) {
    m_spacing = spacing;
    updateLayout();
}

void UIMenuWidget::updateLayout() {
    if (m_items.empty()) return;

    float totalHeight = 0.f;
    for (const auto& item : m_items) {
        totalHeight += item.text.getLocalBounds().size.y + m_spacing;
    }
    totalHeight -= m_spacing; // Remove trailing spacing

    float currentY = m_position.y;
    
    // Adjust starting Y based on vertical anchor
    if (m_alignAnchor == UIAnchor::Center || 
        m_alignAnchor == UIAnchor::CenterLeft || 
        m_alignAnchor == UIAnchor::CenterRight) {
        currentY -= totalHeight / 2.f;
    } else if (m_alignAnchor == UIAnchor::BottomCenter || 
               m_alignAnchor == UIAnchor::BottomLeft || 
               m_alignAnchor == UIAnchor::BottomRight) {
        currentY -= totalHeight;
    }

    for (auto& item : m_items) {
        UILayoutHelper::centerOrigin(item.text);
        
        // We assume the X coordinate is the center line of the menu
        item.text.setPosition({m_position.x, currentY + item.text.getLocalBounds().size.y / 2.f});
        currentY += item.text.getLocalBounds().size.y + m_spacing;
    }
}

void UIMenuWidget::updateSelectionColors() {
    for (size_t i = 0; i < m_items.size(); ++i) {
        if (static_cast<int>(i) == m_selectedIndex) {
            m_items[i].text.setFillColor(sf::Color::Yellow);
        } else {
            m_items[i].text.setFillColor(sf::Color::White);
        }
    }
}

void UIMenuWidget::processEvents(const sf::Event& event) {
    // Mouse support can be added later when we map window coordinates to logical coordinates.
    // For now, this is cleanly handled by processInput using keyboard.
    (void)event;
}

void UIMenuWidget::processInput(const InputState& inputState) {
    if (m_items.empty()) return;

    if (inputState.wasPressed(sf::Keyboard::Key::Down) || inputState.wasPressed(sf::Keyboard::Key::S)) {
        m_selectedIndex = (m_selectedIndex + 1) % static_cast<int>(m_items.size());
        updateSelectionColors();
    }
    else if (inputState.wasPressed(sf::Keyboard::Key::Up) || inputState.wasPressed(sf::Keyboard::Key::W)) {
        m_selectedIndex = (m_selectedIndex - 1 + static_cast<int>(m_items.size())) % static_cast<int>(m_items.size());
        updateSelectionColors();
    }
    else if (inputState.wasPressed(sf::Keyboard::Key::Enter)) {
        if (m_items[m_selectedIndex].onSelect) {
            m_items[m_selectedIndex].onSelect();
        }
    }
}

void UIMenuWidget::update(float dt) {
    (void)dt;
}

void UIMenuWidget::draw(sf::RenderTarget& target) const {
    for (const auto& item : m_items) {
        target.draw(item.text);
    }
}

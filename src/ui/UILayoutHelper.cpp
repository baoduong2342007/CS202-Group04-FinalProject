#include "ui/UILayoutHelper.h"

void UILayoutHelper::centerOrigin(sf::Text& text) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin({bounds.position.x + bounds.size.x / 2.f, 
                    bounds.position.y + bounds.size.y / 2.f});
}

void UILayoutHelper::centerOrigin(sf::Sprite& sprite) {
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin({bounds.position.x + bounds.size.x / 2.f, 
                      bounds.position.y + bounds.size.y / 2.f});
}

sf::Vector2f UILayoutHelper::getAnchorPosition(UIAnchor anchor) {
    float width = static_cast<float>(DisplayConfig::LOGICAL_WIDTH);
    float height = static_cast<float>(DisplayConfig::LOGICAL_HEIGHT);
    
    switch (anchor) {
        case UIAnchor::TopLeft:      return {0.f, 0.f};
        case UIAnchor::TopCenter:    return {width / 2.f, 0.f};
        case UIAnchor::TopRight:     return {width, 0.f};
        case UIAnchor::CenterLeft:   return {0.f, height / 2.f};
        case UIAnchor::Center:       return {width / 2.f, height / 2.f};
        case UIAnchor::CenterRight:  return {width, height / 2.f};
        case UIAnchor::BottomLeft:   return {0.f, height};
        case UIAnchor::BottomCenter: return {width / 2.f, height};
        case UIAnchor::BottomRight:  return {width, height};
        default:                     return {width / 2.f, height / 2.f};
    }
}

void UILayoutHelper::setPosition(sf::Text& text, UIAnchor anchor, const sf::Vector2f& offset) {
    centerOrigin(text);
    sf::Vector2f pos = getAnchorPosition(anchor);
    text.setPosition({pos.x + offset.x, pos.y + offset.y});
}

void UILayoutHelper::setPosition(sf::Sprite& sprite, UIAnchor anchor, const sf::Vector2f& offset) {
    centerOrigin(sprite);
    sf::Vector2f pos = getAnchorPosition(anchor);
    sprite.setPosition({pos.x + offset.x, pos.y + offset.y});
}

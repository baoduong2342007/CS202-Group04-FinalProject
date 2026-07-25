/**
 * @file Item.cpp
 * @author TV5 (Truyền)
 * @brief Abstract base class for collectible items
 * @note Week 4
 */

#include "items/Item.h"

Item::Item()
    : Entity(), m_isCollected(false) {}

Item::Item(const sf::Vector2f& position, const sf::Vector2f& size)
    : Entity(position, size), m_isCollected(false) {}

bool Item::checkOverlap(const Entity& other) const {
    if (m_isCollected) {
        return false;
    }
    return getBoundingBox().findIntersection(other.getBoundingBox()).has_value();
}

bool Item::isCollected() const {
    return m_isCollected;
}

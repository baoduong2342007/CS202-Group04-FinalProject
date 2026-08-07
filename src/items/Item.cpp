/**
 * @file Item.cpp
 * @author TV5 (Truyền)
 * @brief Abstract base class for collectible items
 * @note Week 4
 */

#include "items/Item.h"

#include <algorithm>

Item::Item()
    : Entity(), m_isCollected(false) {}

Item::Item(const sf::Vector2f& position, const sf::Vector2f& size)
    : Entity(position, size), m_isCollected(false) {}

bool Item::checkOverlap(const Entity& other) const {
    if (m_isCollected || m_collectibleDelay > 0.f) {
        return false;
    }
    return getBoundingBox().findIntersection(other.getBoundingBox()).has_value();
}

bool Item::isCollected() const {
    return m_isCollected;
}

void Item::setCollectibleDelay(float seconds) {
    m_collectibleDelay = std::max(0.f, seconds);
}

void Item::updateCollectibleDelay(float dt) {
    m_collectibleDelay = std::max(0.f, m_collectibleDelay - std::max(0.f, dt));
}

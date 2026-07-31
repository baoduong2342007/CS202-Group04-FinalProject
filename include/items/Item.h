/**
 * @file Item.h
 * @author TV5 (Truyền)
 * @brief Abstract base class for collectible items (Coin, Mushroom, etc.)
 * @note Week 4 — inherits Entity, adds pure virtual onCollect()
 */

#pragma once

#include "entities/Entity.h"

class Mario;

class Item : public Entity {
public:
    // 1. Constructor / Destructor
    Item();
    Item(const sf::Vector2f& position, const sf::Vector2f& size);
    ~Item() override = default;

    // 2. Override methods
    void update(float dt) override = 0;

    // 3. Public methods
    /// Called when Mario overlaps this item — subclass defines the effect
    virtual void onCollect(Mario& mario) = 0;

    /// Check whether this item's bounding box overlaps Mario's
    bool checkOverlap(const Entity& other) const;

    // 4. Getters / Setters
    bool isCollected() const;

    /// Identifies this entity as an Item for RTTI-free type checking
    bool isItem() const override { return true; }

protected:
    // 6. Protected members
    bool m_isCollected;
};

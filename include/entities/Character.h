/**
 * @file Character.h
 * @author TV3 (Bảo)
 * @brief Character class
 * @note Week 1 skeleton setup
 */

#pragma once

#include "entities/Entity.h"

enum class Direction {
    LEFT,
    RIGHT
};

class Character : public Entity { // inherit Entity class
public:
    // 1. Constructor / Destructor
    Character();
    Character(const sf::Vector2f& position, const sf::Vector2f& size, int health);
    ~Character() override = default;

    // 2. Override methods
    void update(float dt) override = 0;

    // 3. Public methods
    void takeDamage(int amount);

    // 4. Getters / Setters
    Direction getFacingDirection() const;
    int getHealth() const;
    bool isDead() const;
    bool isGrounded() const;

    void setFacingDirection(Direction dir);
    void setHealth(int health);
    void setGrounded(bool grounded);

protected:
    // 5. Protected members
    Direction m_facingDirection;
    int m_health;
    bool m_isGrounded;
};

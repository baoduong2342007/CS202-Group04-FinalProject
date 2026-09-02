/**
 * @file Character.cpp
 * @author TV3
 * @brief Character abstract base class implementation
 */

#include "entities/Character.h"

Character::Character()
    : Entity(),
      m_facingDirection(Direction::RIGHT),
      m_health(100),
      m_isGrounded(false) {}

Character::Character(const sf::Vector2f& position, const sf::Vector2f& size, int health)
    : Entity(position, size),
      m_facingDirection(Direction::RIGHT),
      m_health(health),
      m_isGrounded(false) {}

void Character::takeDamage(int amount) {
    m_health -= amount;
    if (m_health < 0) {
        m_health = 0;
    }
}

Direction Character::getFacingDirection() const {
    return m_facingDirection;
}

int Character::getHealth() const {
    return m_health;
}

bool Character::isDead() const {
    return m_health <= 0;
}

bool Character::isGrounded() const {
    return m_isGrounded;
}

void Character::setFacingDirection(Direction dir) {
    m_facingDirection = dir;
}

void Character::setHealth(int health) {
    m_health = health;
}

void Character::setGrounded(bool grounded) {
    m_isGrounded = grounded;
}

/**
 * @file Enemy.cpp
 * @author TV4 (Vy)
 * @brief Implementation of the abstract Enemy base class
 */

#include "entities/Enemy.h"

Enemy::Enemy(const sf::Vector2f& position, const sf::Vector2f& size, int health)
: Character(position, size, health){}

void Enemy::onSideCollision(Entity* other) {
    (void)other;
}

void Enemy::onFireHit() {
    takeDamage(100);
    markForRemoval();
}

void Enemy::activate() {
    m_activated = true;
}

bool Enemy::isActivated() const {
    return m_activated;
}

bool Enemy::tryCommitDefeat() {
    if (m_defeatCommitted || isDead() || shouldRemove() ||
        isPendingDestroy() || !isActive()) {
        return false;
    }

    m_defeatCommitted = true;
    return true;
}

bool Enemy::tryCommitStomp() {
    if (m_stompCommitted || isDead() || shouldRemove() ||
        isPendingDestroy() || !isActive()) {
        return false;
    }

    m_stompCommitted = true;
    return true;
}

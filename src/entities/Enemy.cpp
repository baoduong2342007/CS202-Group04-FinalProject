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



/**
 * @file Enemy.h
 * @author TV4 (Vy)
 * @brief Abstract Enemy base class for Goomba and Koopa
 * @note Sprint 4 - abstract enemy interface for patrol and collision responses
 */

#pragma once

#include "entities/Character.h"

/*
 * OOP PRINCIPLE: Runtime Polymorphism
 * Reason: Enemy defines the common interface for enemy behavior,
 *         while specific enemies such as Goomba and Koopa implement
 *         their own patrol, stomp, and wall-collision behaviour.
 */

class Enemy : public Character {
public:
    Enemy(const sf::Vector2f& position, const sf::Vector2f& size, int health);
    ~Enemy() override = default;

    virtual void patrol() = 0;
    virtual void onStomp() = 0;
    virtual void onWallCollision() = 0;
};

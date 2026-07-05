/**
 * @file Enemy.h
 * @author TV4 (Vy)
 * @brief Abstract Enemy base class for Goomba and Koopa
 * @note Week 2 skeleton setup
 */

#pragma once

#include "entities/Character.h"

/*
 * PATTERN: Template Method / Polymorphism
 * Reason: Enemy defines the common interface for enemy behavior,
 *         while specific enemies such as Goomba and Koopa implement
 *         their own patrol movement and stomp reaction.
 */

class Enemy : public Character {
public:
    Enemy() = default;
    ~Enemy() override = default;
    
    virtual void patrol() = 0;
    virtual void onStomp() = 0;
};

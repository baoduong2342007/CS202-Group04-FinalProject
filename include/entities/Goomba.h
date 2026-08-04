/**
 * @file Goomba.h
 * @author TV4 (Vy)
 * @brief Goomba enemy with horizontal patrol and stomp behaviour
 * @note Sprint 4 - basic patrol AI and wall-direction reversal
 */

#pragma once

#include "entities/Enemy.h"

#include <SFML/System/Vector2.hpp>

class TileMap;

class Goomba : public Enemy {
public:
    Goomba(const sf::Vector2f& position, b2World* world);
    ~Goomba() override = default;

    void update(float dt) override;

    void onStomp() override;
    void patrol() override;
    void onWallCollision() override;

    bool isStomped() const;
    
    void setTileMap(const TileMap* tileMap);

private:
    void reverseDirection();
    bool isApproachingLedge() const;
    
    const TileMap* m_tileMap = nullptr;

    bool m_isStomped;
    float m_patrolSpeed;
    float m_squishTimer = 0.f;
    
    static constexpr float SQUISH_DURATION = 0.5f;
};

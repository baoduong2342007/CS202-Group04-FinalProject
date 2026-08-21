/**
 * @file Mushroom.h
 * @author TV5 (Truyền)
 * @brief Super Mushroom — moves horizontally, grows Mario from SMALL to SUPER
 * @note Week 5 — spawned by EntityFactory; power-up logic uses Mario's current state
 *       (no EventBus payload needed — the item queries Mario directly)
 */

#pragma once

#include "core/LevelCatalog.h"
#include "items/Item.h"

enum class MushroomType {
    SUPER,  // Red — grows Mario to Big Mario
    ONE_UP  // Green — grants +1 life
};

class Mushroom : public Item {
public:
    // 1. Constructor / Destructor
    Mushroom(MushroomType type = MushroomType::SUPER, LevelTheme theme = LevelTheme::OVERWORLD);
    Mushroom(const sf::Vector2f& position,
             b2World* world,
             MushroomType type = MushroomType::SUPER,
             LevelTheme theme = LevelTheme::OVERWORLD);
    ~Mushroom() override = default;

    // 2. Override methods
    void update(float dt) override;
    void onCollect(Mario& mario) override;
    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::MUSHROOM; }
    LevelTheme getTheme() const { return m_theme; }

    // 3. Public methods
    /// Reverse horizontal direction when hitting a wall
    void onWallCollision();

    /// Set patrol speed (0 for standing still)
    void setPatrolSpeed(float speed) { m_patrolSpeed = speed; }

private:
    // 5. Private methods
    void patrol();

    // 6. Private members
    MushroomType m_type;
    LevelTheme m_theme = LevelTheme::OVERWORLD;
    float m_patrolSpeed;
    /// +1 = moving right, -1 = moving left
    int m_patrolDirection;
    float m_stuckTimer = 0.f;
};

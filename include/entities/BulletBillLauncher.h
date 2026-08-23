/**
 * @file BulletBillLauncher.h
 * @brief Decorative Turtle Cannon that fires Bullet Bills at approaching players
 * @note Enemy expansion - no physics body of its own; it hands spawned
 *       bullets to Level through the pending-spawns outbox.
 */

#pragma once

#include "core/LevelCatalog.h"
#include "entities/Entity.h"

class BulletBillLauncher : public Entity {
public:
    BulletBillLauncher(const sf::Vector2f& position,
                       b2World* world,
                       LevelTheme theme);
    ~BulletBillLauncher() override = default;

    void update(float dt) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    EntityType getType() const override {
        return EntityType::TERRAIN;
    }

    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::BULLET_BILL_LAUNCHER; }

    /// Feed the player position; the cannon only fires while a player is
    /// inside its horizontal range.
    void updateMarioPosition(const sf::Vector2f& marioPos);

    float getAimAngle() const { return m_currentAimAngle; }

    void setCeaseFire(bool cease) { m_ceaseFire = cease; }
    bool isCeaseFire() const { return m_ceaseFire; }

    std::vector<std::unique_ptr<Entity>> takePendingSpawns() override;

private:
    b2World* m_world = nullptr;
    LevelTheme m_theme;
    sf::Vector2f m_marioPosition{0.f, 0.f};
    bool m_marioKnown = false;
    bool m_ceaseFire = false;
    float m_fireTimer{0.f};
    float m_currentAimAngle{180.f};
    std::vector<std::unique_ptr<Entity>> m_pending;

    // The cannon is drawn from three stacked tileset cells (top, body, base).
    // SFML 3 sprites have no default constructor, so they are built lazily.
    mutable std::optional<sf::Sprite> m_parts[3];

    static constexpr float FIRE_INTERVAL = 3.f;
    static constexpr float RANGE = 850.f; ///< ~26.5 tiles of detection reach across the elevator arena
};

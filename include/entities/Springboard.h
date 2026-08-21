/**
 * @file Springboard.h
 * @author TV1 (Dương)
 * @brief Springboard / Trampoline entity for high jump launches
 * @note Authentic NES SMB springboard with normal bounce and Super Spring Jump mechanics
 */

#pragma once

#include "entities/Entity.h"
#include "core/LevelCatalog.h"

class Mario;

class Springboard : public Entity {
public:
    enum class State {
        IDLE,
        COMPRESSING,
        LAUNCHING,
        COOLDOWN
    };

    // 1. Constructor / Destructor
    Springboard();
    Springboard(const sf::Vector2f& position, LevelTheme theme = LevelTheme::OVERWORLD);
    ~Springboard() override = default;

    // 2. Override methods
    void initPhysics(b2World* world, b2BodyType type, const sf::Vector2f& size, bool isSensor = false) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    // 3. Public methods
    void initTheme(LevelTheme theme);
    void triggerSpring(Mario& mario, bool isHoldingJump);

    // 4. Getters / Setters
    EntityType getType() const override { return EntityType::SPRINGBOARD; }
    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::SPRINGBOARD; }
    State getState() const { return m_state; }

    static constexpr float BOUNCE_NORMAL_SPEED = 600.f; // px/s (was 450)
    static constexpr float BOUNCE_SUPER_SPEED = 1100.f;  // px/s (was 850)

private:
    State m_state = State::IDLE;
    float m_stateTimer = 0.f;

    sf::IntRect m_idleRect;
    sf::IntRect m_midRect;
    sf::IntRect m_extendedRect;
};

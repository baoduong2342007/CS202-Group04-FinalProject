/**
 * @file Hammer.cpp
 * @brief Hammer implementation - arcing spin from the new hammer.png asset
 */

#include "entities/Hammer.h"

#include <cmath>

#include "core/AnimationSystem.h"

namespace {

const sf::Vector2f HAMMER_SIZE{16.f, 16.f};
constexpr const char* HAMMER_TEXTURE_PATH = "assets/textures/enemies/hammer.png";

} // namespace

Hammer::Hammer(const sf::Vector2f& position,
               b2World* world,
               Direction direction)
    : EnemyProjectile(position, HAMMER_SIZE, world,
                      {direction == Direction::LEFT ? -THROW_VX : THROW_VX,
                       THROW_VY},
                      1.f) {
    setSprite(HAMMER_TEXTURE_PATH);

    m_animationSystem = std::make_unique<AnimationSystem>();
}

void Hammer::update(float dt) {
    EnemyProjectile::update(dt);
}

void Hammer::updatePresentation(float dt) {
    if (!m_sprite) {
        return;
    }

    // The claw always spins counter-clockwise, whichever way it was thrown.
    m_rotation += SPIN_DEGREES_PER_SECOND * dt;
    m_sprite->setRotation(sf::degrees(m_rotation));
    m_sprite->setOrigin({8.f, 8.f});
    m_sprite->setScale({2.f, 2.f});
    m_sprite->setPosition(m_position + m_size / 2.f);
}

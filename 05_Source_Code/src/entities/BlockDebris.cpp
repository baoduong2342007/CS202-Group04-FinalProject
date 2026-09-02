/**
 * @file BlockDebris.cpp
 * @author TV4 & TV1
 * @brief Particle debris effect spawned when Big Mario shatters a Brick Block
 */

#include "entities/BlockDebris.h"
#include "core/AnimationSystem.h"

namespace {
constexpr float GRAVITY = 1200.f;
constexpr float DEBRIS_SIZE = 16.f;
constexpr const char* DEBRIS_TEXTURE_PATH = "assets/textures/items/items_blocks.png";
}

BlockDebris::BlockDebris(const sf::Vector2f& position, const sf::Vector2f& velocity, const sf::IntRect& frame)
    : Entity(position, sf::Vector2f(DEBRIS_SIZE, DEBRIS_SIZE)),
      m_vel(velocity) {
    setSprite(DEBRIS_TEXTURE_PATH);
    m_animationSystem->addAnimation("idle",
        AnimationSystem::createManualAnimation(
            std::vector<sf::IntRect>{frame}, 1.f));
    playAnimation("idle");
}

void BlockDebris::update(float dt) {
    if (!m_active) return;

    m_lifetime -= dt;
    if (m_lifetime <= 0.f) {
        markForRemoval();
        return;
    }

    // Custom particle gravity & trajectory (no Box2D body needed)
    m_vel.y += GRAVITY * dt;
    m_position += m_vel * dt;
    m_rotation += 360.f * dt;

    updateAnimation(dt);

    if (m_sprite) {
        sf::IntRect texRect = m_sprite->getTextureRect();
        if (texRect.size.x > 0 && texRect.size.y > 0) {
            m_sprite->setOrigin({texRect.size.x / 2.f, texRect.size.y / 2.f});
            m_sprite->setScale({2.f, 2.f});
        }
        m_sprite->setPosition(m_position);
        m_sprite->setRotation(sf::degrees(m_rotation));
    }
}

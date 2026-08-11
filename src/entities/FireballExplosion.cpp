/**
 * @file FireballExplosion.cpp
 * @author TV3 & TV1
 * @brief Visual particle effect entity spawned when a FireBall explodes on impact
 */

#include "entities/FireballExplosion.h"
#include "core/AnimationSystem.h"
#include "core/SpriteFrames_shared.h"

namespace {
constexpr float EXPLOSION_SIZE = 16.f;
constexpr float FRAME_DURATION = 0.05f; // 3 frames * 0.05s = 0.15s total duration
constexpr float TOTAL_LIFETIME = 0.15f;
constexpr const char* FIREBALL_TEXTURE_PATH = "assets/textures/items/items_objects.png";
}

FireballExplosion::FireballExplosion(const sf::Vector2f& position)
    : Entity(position, sf::Vector2f(EXPLOSION_SIZE, EXPLOSION_SIZE)),
      m_lifetime(TOTAL_LIFETIME) {
    setSprite(FIREBALL_TEXTURE_PATH);
    m_animationSystem->addAnimation("explode",
        AnimationSystem::createManualAnimation(
            SpriteFrames::shared::Items::fireballExplosionFrames(), FRAME_DURATION));
    playAnimation("explode");
}

void FireballExplosion::update(float dt) {
    if (!m_active) return;

    m_lifetime -= dt;
    if (m_lifetime <= 0.f) {
        markForRemoval();
        return;
    }

    updateAnimation(dt);

    if (m_sprite) {
        sf::IntRect texRect = m_sprite->getTextureRect();
        if (texRect.size.x > 0 && texRect.size.y > 0) {
            m_sprite->setOrigin({texRect.size.x / 2.f, texRect.size.y / 2.f});
            m_sprite->setScale({2.f, 2.f});
        }
        m_sprite->setPosition(m_position + sf::Vector2f(EXPLOSION_SIZE / 2.f, EXPLOSION_SIZE / 2.f));
    }
}

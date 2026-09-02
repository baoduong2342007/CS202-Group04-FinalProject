/**
 * @file RedKoopa.cpp
 * @brief Red Koopa implementation - red palette over the Koopa shell core
 */

#include "entities/RedKoopa.h"

#include "core/SpriteFrames_ovw.h"

namespace {

constexpr float RED_KOOPA_PATROL_SPEED = 50.f;
const sf::Vector2f RED_KOOPA_SIZE{32.f, 48.f};
const sf::Vector2f RED_KOOPA_SHELL_SIZE{32.f, 28.f};

} // namespace

RedKoopa::RedKoopa(const sf::Vector2f& position,
                   b2World* world,
                   LevelTheme theme)
    : Koopa(position, world, theme,
            RED_KOOPA_SIZE, RED_KOOPA_SHELL_SIZE, RED_KOOPA_PATROL_SPEED) {
    // The red Koopa row of the atlas is one palette for every environment.
    (void)theme;
    registerKoopaAnimations(SpriteFrames::ovw::Enemies::RedKoopa::walkFrames(),
                            SpriteFrames::ovw::Enemies::RedKoopa::SHELL,
                            SpriteFrames::ovw::Enemies::RedKoopa::SHELL_WAKING);
}

/**
 * @file BuzzyBeetle.cpp
 * @brief Buzzy Beetle implementation - theme palettes over the Koopa shell core
 */

#include "entities/BuzzyBeetle.h"

#include "core/SpriteFrames_ovw.h"
#include "core/SpriteFrames_udg.h"
#include "core/SpriteFrames_castle.h"
#include "core/SpriteFrames_udw.h"

namespace {

constexpr float BUZZY_PATROL_SPEED = 50.f;
const sf::Vector2f BUZZY_SIZE{32.f, 32.f};

} // namespace

BuzzyBeetle::BuzzyBeetle(const sf::Vector2f& position,
                         b2World* world,
                         LevelTheme theme)
    : Koopa(position, world, theme, BUZZY_SIZE, BUZZY_SIZE, BUZZY_PATROL_SPEED) {
    const auto& walkFrames = [theme]() -> const std::vector<sf::IntRect>& {
        switch (theme) {
            case LevelTheme::UNDERGROUND:
                return SpriteFrames::udg::Enemies::BuzzyBeetle::walkFrames();
            case LevelTheme::CASTLE:
                return SpriteFrames::castle::Enemies::BuzzyBeetle::walkFrames();
            case LevelTheme::UNDERWATER:
                return SpriteFrames::udw::Enemies::BuzzyBeetle::walkFrames();
            case LevelTheme::OVERWORLD:
            default:
                return SpriteFrames::ovw::Enemies::BuzzyBeetle::walkFrames();
        }
    }();

    const sf::IntRect& shellRect = [theme]() -> const sf::IntRect& {
        switch (theme) {
            case LevelTheme::UNDERGROUND:
                return SpriteFrames::udg::Enemies::BuzzyBeetle::SHELL;
            case LevelTheme::CASTLE:
                return SpriteFrames::castle::Enemies::BuzzyBeetle::SHELL;
            case LevelTheme::UNDERWATER:
                return SpriteFrames::udw::Enemies::BuzzyBeetle::SHELL;
            case LevelTheme::OVERWORLD:
            default:
                return SpriteFrames::ovw::Enemies::BuzzyBeetle::SHELL;
        }
    }();

    // The atlas has no legs-out frame for the Buzzy Beetle and it never shows
    // a warning, so the shell frame doubles as the (unused) waking rect.
    registerKoopaAnimations(walkFrames, shellRect, shellRect);
}

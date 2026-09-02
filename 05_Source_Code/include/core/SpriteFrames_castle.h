/**
 * @file SpriteFrames_castle.h
 * @brief Normalized Castle sprite-frame catalog.
 *
 * The NES Castle Koopa palette is the same verified palette row as Underground.
 * These aliases are intentionally routed through SpriteFrames::udg so the two
 * themes cannot drift to different atlas coordinates.
 */

#pragma once

#include "core/SpriteFrames_udg.h"

namespace SpriteFrames {
namespace castle {

namespace Enemies {
namespace Goomba {
inline const sf::IntRect& WALK1 = legacy::Enemies::Goomba::CASTLE_WALK1;
inline const sf::IntRect& WALK2 = legacy::Enemies::Goomba::CASTLE_WALK2;
inline const sf::IntRect& STOMPED = legacy::Enemies::Goomba::CASTLE_STOMPED;
inline const std::vector<sf::IntRect>& walkFrames() {
    static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
    return frames;
}
} // namespace Goomba

namespace Koopa {
// Castle uses the same gray/blue Koopa palette as Underground.
inline const sf::IntRect& WALK1 = udg::Enemies::Koopa::WALK1;
inline const sf::IntRect& WALK2 = udg::Enemies::Koopa::WALK2;
inline const sf::IntRect& SHELL = udg::Enemies::Koopa::SHELL;
inline const sf::IntRect& SHELL_RETRACTED = udg::Enemies::Koopa::SHELL_RETRACTED;
inline const sf::IntRect& SHELL_WAKING = udg::Enemies::Koopa::SHELL_WAKING;
inline const std::vector<sf::IntRect>& walkFrames() {
    static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
    return frames;
}
} // namespace Koopa

namespace CheepCheep {
inline const sf::IntRect& SWIM_UP = udg::Enemies::CheepCheep::SWIM_UP;
inline const sf::IntRect& SWIM_DOWN = udg::Enemies::CheepCheep::SWIM_DOWN;
inline const sf::IntRect& RED_SWIM_UP = udg::Enemies::CheepCheep::RED_SWIM_UP;
inline const sf::IntRect& RED_SWIM_DOWN = udg::Enemies::CheepCheep::RED_SWIM_DOWN;
inline const std::vector<sf::IntRect>& greenSwimFrames() {
    return udg::Enemies::CheepCheep::greenSwimFrames();
}
inline const std::vector<sf::IntRect>& redSwimFrames() {
    return udg::Enemies::CheepCheep::redSwimFrames();
}
inline const std::vector<sf::IntRect>& swimFrames() {
    return udg::Enemies::CheepCheep::swimFrames();
}
} // namespace CheepCheep

namespace BuzzyBeetle {
inline const sf::IntRect& WALK1 = legacy::Enemies::BuzzyBeetle::CASTLE_WALK1;
inline const sf::IntRect& WALK2 = legacy::Enemies::BuzzyBeetle::CASTLE_WALK2;
inline const sf::IntRect& SHELL = legacy::Enemies::BuzzyBeetle::CASTLE_SHELL;
inline const std::vector<sf::IntRect>& walkFrames() {
    static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
    return frames;
}
} // namespace BuzzyBeetle

namespace RedKoopa {
inline const sf::IntRect& WALK1 = legacy::Enemies::RedKoopa::WALK1;
inline const sf::IntRect& WALK2 = legacy::Enemies::RedKoopa::WALK2;
inline const sf::IntRect& PARATROOPA1 = legacy::Enemies::RedKoopa::PARATROOPA1;
inline const sf::IntRect& PARATROOPA2 = legacy::Enemies::RedKoopa::PARATROOPA2;
inline const sf::IntRect& SHELL = legacy::Enemies::RedKoopa::SHELL;
inline const sf::IntRect& SHELL_WAKING = legacy::Enemies::RedKoopa::SHELL_WAKING;
inline const std::vector<sf::IntRect>& walkFrames() {
    static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
    return frames;
}
inline const std::vector<sf::IntRect>& flyFrames() {
    static const std::vector<sf::IntRect> frames = {PARATROOPA1, PARATROOPA2};
    return frames;
}
} // namespace RedKoopa

// Castle shares the Underground palette row of the green paratroopa frames.
namespace Paratroopa {
inline const sf::IntRect& FLY1 = udg::Enemies::Paratroopa::FLY1;
inline const sf::IntRect& FLY2 = udg::Enemies::Paratroopa::FLY2;
inline const std::vector<sf::IntRect>& flyFrames() {
    return udg::Enemies::Paratroopa::flyFrames();
}
} // namespace Paratroopa

// Compatibility namespace for callers that still organize frames by atlas block.
namespace GreenEnemiesBlock {
inline const sf::IntRect& WALK1 = Koopa::WALK1;
inline const sf::IntRect& WALK2 = Koopa::WALK2;
inline const sf::IntRect& SHELL = Koopa::SHELL;
inline const sf::IntRect& SHELL_RETRACTED = Koopa::SHELL_RETRACTED;
inline const sf::IntRect& SHELL_WAKING = Koopa::SHELL_WAKING;
inline const std::vector<sf::IntRect>& walkFrames() {
    return Koopa::walkFrames();
}
} // namespace GreenEnemiesBlock
} // namespace Enemies

namespace Items {
inline const sf::IntRect& SUPER_MUSHROOM = legacy::Items::SUPER_MUSHROOM_CASTLE;
inline const sf::IntRect& ONE_UP_MUSHROOM = legacy::Items::ONE_UP_MUSHROOM_CASTLE;
inline const sf::IntRect& FLAGPOLE_FLAG = legacy::Items::CASTLE_FLAGPOLE_FLAG;
} // namespace Items

namespace Blocks {
inline const sf::IntRect& QUESTION1 = legacy::Blocks::CASTLE_QUESTION1;
inline const sf::IntRect& QUESTION2 = legacy::Blocks::CASTLE_QUESTION2;
inline const sf::IntRect& QUESTION3 = legacy::Blocks::CASTLE_QUESTION3;
inline const sf::IntRect& EMPTY = legacy::Blocks::CASTLE_EMPTY;
inline const sf::IntRect& HIT = legacy::Blocks::CASTLE_HIT;
inline const std::vector<sf::IntRect>& questionBlockFrames() {
    static const std::vector<sf::IntRect> frames = {QUESTION1, QUESTION2, QUESTION3};
    return frames;
}
inline const sf::IntRect& DEBRIS_TOP_LEFT = legacy::Blocks::CASTLE_DEBRIS_TOP_LEFT;
inline const sf::IntRect& DEBRIS_TOP_RIGHT = legacy::Blocks::CASTLE_DEBRIS_TOP_RIGHT;
inline const sf::IntRect& DEBRIS_BOTTOM_LEFT = legacy::Blocks::CASTLE_DEBRIS_BOTTOM_LEFT;
inline const sf::IntRect& DEBRIS_BOTTOM_RIGHT = legacy::Blocks::CASTLE_DEBRIS_BOTTOM_RIGHT;
inline const std::vector<sf::IntRect>& debris4Corners() {
    static const std::vector<sf::IntRect> frames = {
        DEBRIS_TOP_LEFT, DEBRIS_TOP_RIGHT, DEBRIS_BOTTOM_LEFT, DEBRIS_BOTTOM_RIGHT};
    return frames;
}
} // namespace Blocks

namespace Backgrounds {
inline constexpr std::string_view CASTLE_PATH = legacy::Backgrounds::CASTLE_PATH;
inline const sf::IntRect& THEME_BG = legacy::Backgrounds::THEME_BG;
} // namespace Backgrounds

} // namespace castle
} // namespace SpriteFrames

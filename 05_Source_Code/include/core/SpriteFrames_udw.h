/**
 * @file SpriteFrames_udw.h
 * @brief Normalized Underwater sprite-frame catalog.
 */

#pragma once

#include "core/SpriteFrames_shared.h"

namespace SpriteFrames {
namespace udw {

namespace Enemies {
namespace Goomba {
inline const sf::IntRect& WALK1 = legacy::Enemies::Goomba::UW_WALK1;
inline const sf::IntRect& WALK2 = legacy::Enemies::Goomba::UW_WALK2;
inline const sf::IntRect& STOMPED = legacy::Enemies::Goomba::UW_STOMPED;
inline const std::vector<sf::IntRect>& walkFrames() {
    static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
    return frames;
}
} // namespace Goomba

namespace Koopa {
inline const sf::IntRect& WALK1 = legacy::Enemies::Koopa::UW_WALK1;
inline const sf::IntRect& WALK2 = legacy::Enemies::Koopa::UW_WALK2;
inline const sf::IntRect& SHELL = legacy::Enemies::Koopa::UW_SHELL;
inline const sf::IntRect& SHELL_RETRACTED = legacy::Enemies::Koopa::UW_SHELL_RETRACTED;
inline const sf::IntRect& SHELL_WAKING = legacy::Enemies::Koopa::UW_SHELL_WAKING;
inline const std::vector<sf::IntRect>& walkFrames() {
    static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
    return frames;
}
} // namespace Koopa

namespace CheepCheep {
inline const sf::IntRect& SWIM_UP = legacy::Enemies::CheepCheepGreen::UW_SWIM_UP;
inline const sf::IntRect& SWIM_DOWN = legacy::Enemies::CheepCheepGreen::UW_SWIM_DOWN;
inline const sf::IntRect& RED_SWIM_UP = legacy::Enemies::RedCheepCheep::SWIM_UP;
inline const sf::IntRect& RED_SWIM_DOWN = legacy::Enemies::RedCheepCheep::SWIM_DOWN;
inline const std::vector<sf::IntRect>& greenSwimFrames() {
    static const std::vector<sf::IntRect> frames = {SWIM_UP, SWIM_DOWN};
    return frames;
}
inline const std::vector<sf::IntRect>& redSwimFrames() {
    static const std::vector<sf::IntRect> frames = {RED_SWIM_UP, RED_SWIM_DOWN};
    return frames;
}
inline const std::vector<sf::IntRect>& swimFrames() {
    return greenSwimFrames();
}
} // namespace CheepCheep

namespace BuzzyBeetle {
inline const sf::IntRect& WALK1 = legacy::Enemies::BuzzyBeetle::UW_WALK1;
inline const sf::IntRect& WALK2 = legacy::Enemies::BuzzyBeetle::UW_WALK2;
inline const sf::IntRect& SHELL = legacy::Enemies::BuzzyBeetle::UW_SHELL;
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

namespace Paratroopa {
inline const sf::IntRect& FLY1 = legacy::Enemies::GreenEnemiesBlock::UW_PARATROOPA_FLY1;
inline const sf::IntRect& FLY2 = legacy::Enemies::GreenEnemiesBlock::UW_PARATROOPA_FLY2;
inline const std::vector<sf::IntRect>& flyFrames() {
    static const std::vector<sf::IntRect> frames = {FLY1, FLY2};
    return frames;
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
inline const sf::IntRect& SUPER_MUSHROOM = legacy::Items::SUPER_MUSHROOM_UW;
inline const sf::IntRect& ONE_UP_MUSHROOM = legacy::Items::ONE_UP_MUSHROOM_UW;
inline const sf::IntRect& FLAGPOLE_FLAG = legacy::Items::UW_FLAGPOLE_FLAG;
} // namespace Items

namespace Blocks {
inline const sf::IntRect& QUESTION1 = legacy::Blocks::UW_QUESTION1;
inline const sf::IntRect& QUESTION2 = legacy::Blocks::UW_QUESTION2;
inline const sf::IntRect& QUESTION3 = legacy::Blocks::UW_QUESTION3;
inline const sf::IntRect& EMPTY = legacy::Blocks::UW_EMPTY;
inline const sf::IntRect& HIT = legacy::Blocks::UW_HIT;
inline const std::vector<sf::IntRect>& questionBlockFrames() {
    static const std::vector<sf::IntRect> frames = {QUESTION1, QUESTION2, QUESTION3};
    return frames;
}
inline const sf::IntRect& DEBRIS_TOP_LEFT = legacy::Blocks::UW_DEBRIS_TOP_LEFT;
inline const sf::IntRect& DEBRIS_TOP_RIGHT = legacy::Blocks::UW_DEBRIS_TOP_RIGHT;
inline const sf::IntRect& DEBRIS_BOTTOM_LEFT = legacy::Blocks::UW_DEBRIS_BOTTOM_LEFT;
inline const sf::IntRect& DEBRIS_BOTTOM_RIGHT = legacy::Blocks::UW_DEBRIS_BOTTOM_RIGHT;
inline const std::vector<sf::IntRect>& debris4Corners() {
    static const std::vector<sf::IntRect> frames = {
        DEBRIS_TOP_LEFT, DEBRIS_TOP_RIGHT, DEBRIS_BOTTOM_LEFT, DEBRIS_BOTTOM_RIGHT};
    return frames;
}
} // namespace Blocks

namespace Backgrounds {
inline constexpr std::string_view UNDERWATER_PATH = legacy::Backgrounds::UNDERWATER_PATH;
inline const sf::IntRect& THEME_BG = legacy::Backgrounds::THEME_BG;
} // namespace Backgrounds

} // namespace udw
} // namespace SpriteFrames

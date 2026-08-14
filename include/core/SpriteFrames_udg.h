/**
 * @file SpriteFrames_udg.h
 * @brief Normalized Underground sprite-frame catalog.
 */

#pragma once

#include "core/SpriteFrames_shared.h"

namespace SpriteFrames {
namespace udg {

namespace Enemies {
namespace Goomba {
inline const sf::IntRect& WALK1 = legacy::Enemies::Goomba::UG_WALK1;
inline const sf::IntRect& WALK2 = legacy::Enemies::Goomba::UG_WALK2;
inline const sf::IntRect& STOMPED = legacy::Enemies::Goomba::UG_STOMPED;
inline const std::vector<sf::IntRect>& walkFrames() {
    static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
    return frames;
}
} // namespace Goomba

namespace Koopa {
inline const sf::IntRect& WALK1 = legacy::Enemies::Koopa::UG_WALK1;
inline const sf::IntRect& WALK2 = legacy::Enemies::Koopa::UG_WALK2;
inline const sf::IntRect& SHELL = legacy::Enemies::Koopa::UG_SHELL;
inline const sf::IntRect& SHELL_RETRACTED = legacy::Enemies::Koopa::UG_SHELL_RETRACTED;
inline const sf::IntRect& SHELL_WAKING = legacy::Enemies::Koopa::UG_SHELL_WAKING;
inline const std::vector<sf::IntRect>& walkFrames() {
    static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
    return frames;
}
} // namespace Koopa

namespace CheepCheep {
inline const sf::IntRect& SWIM_UP = legacy::Enemies::CheepCheepGreen::UG_CASTLE_SWIM_UP;
inline const sf::IntRect& SWIM_DOWN = legacy::Enemies::CheepCheepGreen::UG_CASTLE_SWIM_DOWN;
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
inline const sf::IntRect& SUPER_MUSHROOM = legacy::Items::SUPER_MUSHROOM_UG;
inline const sf::IntRect& ONE_UP_MUSHROOM = legacy::Items::ONE_UP_MUSHROOM_UG;
inline const sf::IntRect& FLAGPOLE_FLAG = legacy::Items::UG_FLAGPOLE_FLAG;
} // namespace Items

namespace Blocks {
inline const sf::IntRect& QUESTION1 = legacy::Blocks::UG_QUESTION1;
inline const sf::IntRect& QUESTION2 = legacy::Blocks::UG_QUESTION2;
inline const sf::IntRect& QUESTION3 = legacy::Blocks::UG_QUESTION3;
inline const sf::IntRect& EMPTY = legacy::Blocks::UG_EMPTY;
inline const sf::IntRect& HIT = legacy::Blocks::UG_HIT;
inline const std::vector<sf::IntRect>& questionBlockFrames() {
    static const std::vector<sf::IntRect> frames = {QUESTION1, QUESTION2, QUESTION3};
    return frames;
}
inline const sf::IntRect& DEBRIS_TOP_LEFT = legacy::Blocks::UG_DEBRIS_TOP_LEFT;
inline const sf::IntRect& DEBRIS_TOP_RIGHT = legacy::Blocks::UG_DEBRIS_TOP_RIGHT;
inline const sf::IntRect& DEBRIS_BOTTOM_LEFT = legacy::Blocks::UG_DEBRIS_BOTTOM_LEFT;
inline const sf::IntRect& DEBRIS_BOTTOM_RIGHT = legacy::Blocks::UG_DEBRIS_BOTTOM_RIGHT;
inline const std::vector<sf::IntRect>& debris4Corners() {
    static const std::vector<sf::IntRect> frames = {
        DEBRIS_TOP_LEFT, DEBRIS_TOP_RIGHT, DEBRIS_BOTTOM_LEFT, DEBRIS_BOTTOM_RIGHT};
    return frames;
}
} // namespace Blocks

} // namespace udg
} // namespace SpriteFrames

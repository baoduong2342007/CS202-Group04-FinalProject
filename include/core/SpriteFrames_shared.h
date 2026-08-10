/**
 * @file SpriteFrames_shared.h
 * @brief Shared Mario/Luigi, transition, and common item sprite frames.
 *
 * Theme-specific raw atlas data remains in the legacy compatibility catalog
 * below. The facade re-exports that catalog for older callers; new runtime code
 * should use one of the explicit theme headers.
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <string_view>
#include <vector>

namespace SpriteFrames {

// Legacy catalog kept in one place for source compatibility. Theme headers
// expose normalized names and select the verified palette rows from this
// catalog.
namespace legacy {
namespace Enemies {

//
// GROUND ENEMIES  4 palette columns (OW / UG / CT / UW)
//

/**
 * @brief Goomba enemy sprite frames (Rows Y=16 & Y=24).
 * @note 4 environmental palettes  3 action frames (Walk 1, Walk 2, Stomped).
 */
namespace Goomba {
// Overworld Palette (Brown Goomba)
inline const sf::IntRect WALK1({0, 16},
                               {16, 16}); // STT #1: Walking frame 1 (left foot)
inline const sf::IntRect
    WALK2({18, 16}, {16, 16}); // STT #2: Walking frame 2 (right foot)
inline const sf::IntRect STOMPED({36, 24},
                                 {16, 8}); // STT #9: Squished flat by Mario

// Underground Palette (Blue Goomba)
inline const sf::IntRect
    UG_WALK1({74, 16}, {16, 16}); // STT #3: Underground walking frame 1
inline const sf::IntRect
    UG_WALK2({92, 16}, {16, 16}); // STT #4: Underground walking frame 2
inline const sf::IntRect
    UG_STOMPED({110, 24}, {16, 8}); // STT #10: Underground squished flat

// Castle Palette (Grey Goomba)
inline const sf::IntRect
    CASTLE_WALK1({148, 16}, {16, 16}); // STT #5: Castle walking frame 1
inline const sf::IntRect
    CASTLE_WALK2({166, 16}, {16, 16}); // STT #6: Castle walking frame 2
inline const sf::IntRect
    CASTLE_STOMPED({184, 24}, {16, 8}); // STT #11: Castle squished flat

// Underwater Palette (White/Blue Goomba)
inline const sf::IntRect
    UW_WALK1({222, 16}, {16, 16}); // STT #7: Underwater walking frame 1
inline const sf::IntRect
    UW_WALK2({240, 16}, {16, 16}); // STT #8: Underwater walking frame 2
inline const sf::IntRect
    UW_STOMPED({258, 24}, {16, 8}); // STT #12: Underwater squished flat

inline const std::vector<sf::IntRect> &walkFrames() {
  static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
  return frames;
}
inline const std::vector<sf::IntRect> &ugWalkFrames() {
  static const std::vector<sf::IntRect> frames = {UG_WALK1, UG_WALK2};
  return frames;
}
inline const std::vector<sf::IntRect> &castleWalkFrames() {
  static const std::vector<sf::IntRect> frames = {CASTLE_WALK1, CASTLE_WALK2};
  return frames;
}
inline const std::vector<sf::IntRect> &uwWalkFrames() {
  static const std::vector<sf::IntRect> frames = {UW_WALK1, UW_WALK2};
  return frames;
}
} // namespace Goomba

/**
 * @brief Buzzy Beetle enemy sprite frames (Rows Y=34..50).
 * @note Fireball-resistant beetle enemy with hard shell.
 */
namespace BuzzyBeetle {
// Overworld Palette
inline const sf::IntRect WALK1({0, 34},
                               {16, 16}); // STT #13: Crawling frame 1 (16x15)
inline const sf::IntRect WALK2({18, 34},
                               {16, 16}); // STT #14: Crawling frame 2 (16x16)
inline const sf::IntRect
    SHELL({36, 34}, {16, 16}); // STT #15: Retracted hard beetle shell (16x15)

// Underground Palette
inline const sf::IntRect UG_WALK1({74, 34},
                                  {16, 16}); // STT #16: Underground crawling 1
inline const sf::IntRect UG_WALK2({92, 34},
                                  {16, 16}); // STT #17: Underground crawling 2
inline const sf::IntRect
    UG_SHELL({110, 34}, {16, 16}); // STT #18: Underground beetle shell

// Castle Palette
inline const sf::IntRect CASTLE_WALK1({148, 34},
                                      {16, 16}); // STT #19: Castle crawling 1
inline const sf::IntRect CASTLE_WALK2({166, 34},
                                      {16, 16}); // STT #20: Castle crawling 2
inline const sf::IntRect CASTLE_SHELL({184, 34},
                                      {16, 16}); // STT #21: Castle beetle shell

// Underwater Palette
inline const sf::IntRect UW_WALK1({222, 34},
                                  {16, 16}); // STT #22: Underwater crawling 1
inline const sf::IntRect UW_WALK2({240, 34},
                                  {16, 16}); // STT #23: Underwater crawling 2
inline const sf::IntRect UW_SHELL({258, 34},
                                  {16, 16}); // STT #24: Underwater beetle shell

inline const std::vector<sf::IntRect> &walkFrames() {
  static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
  return frames;
}
inline const std::vector<sf::IntRect> &ugWalkFrames() {
  static const std::vector<sf::IntRect> frames = {UG_WALK1, UG_WALK2};
  return frames;
}
inline const std::vector<sf::IntRect> &castleWalkFrames() {
  static const std::vector<sf::IntRect> frames = {CASTLE_WALK1, CASTLE_WALK2};
  return frames;
}
inline const std::vector<sf::IntRect> &uwWalkFrames() {
  static const std::vector<sf::IntRect> frames = {UW_WALK1, UW_WALK2};
  return frames;
}
} // namespace BuzzyBeetle

namespace BuzzyBeetleClimb {
// Buzzy Beetle climbing walls/vines and ceilings (perfect 16x16 grid for
// rotation & alignment) Overworld Palette (Y=78)
inline const sf::IntRect
    CLIMB_LEFT_WALL({4, 78}, {8, 16}); // STT #37: Wall/Vine climbing
inline const sf::IntRect CLIMB_CEIL_LEFT({18, 82},
                                         {16, 8}); // STT #45: Ceiling crawling
inline const sf::IntRect
    CLIMB_RIGHT_WALL({40, 78},
                     {8, 16}); // STT #38: Wall/Vine climbing (reversed)
inline const sf::IntRect
    CLIMB_CEIL_RIGHT({54, 82}, {16, 8}); // STT #46: Ceiling crawling (reversed)

// Underground Palette (X + 74)
inline const sf::IntRect UG_CLIMB_LEFT_WALL({78, 78}, {8, 16});   // STT #39
inline const sf::IntRect UG_CLIMB_CEIL_LEFT({92, 82}, {16, 8});   // STT #47
inline const sf::IntRect UG_CLIMB_RIGHT_WALL({114, 78}, {8, 16}); // STT #40
inline const sf::IntRect UG_CLIMB_CEIL_RIGHT({128, 82}, {16, 8}); // STT #48

// Castle Palette (X + 148)
inline const sf::IntRect CASTLE_CLIMB_LEFT_WALL({152, 78}, {8, 16});  // STT #41
inline const sf::IntRect CASTLE_CLIMB_CEIL_LEFT({166, 82}, {16, 8});  // STT #49
inline const sf::IntRect CASTLE_CLIMB_RIGHT_WALL({188, 78}, {8, 16}); // STT #42
inline const sf::IntRect CASTLE_CLIMB_CEIL_RIGHT({202, 82}, {16, 8}); // STT #50

// Underwater Palette (X + 222)
inline const sf::IntRect UW_CLIMB_LEFT_WALL({226, 78}, {8, 16});  // STT #43
inline const sf::IntRect UW_CLIMB_CEIL_LEFT({240, 82}, {16, 8});  // STT #51
inline const sf::IntRect UW_CLIMB_RIGHT_WALL({262, 78}, {8, 16}); // STT #44
inline const sf::IntRect UW_CLIMB_CEIL_RIGHT({276, 82}, {16, 8}); // STT #52

inline const std::vector<sf::IntRect> &climbFrames() {
  static const std::vector<sf::IntRect> frames = {
      CLIMB_LEFT_WALL, CLIMB_CEIL_LEFT, CLIMB_RIGHT_WALL, CLIMB_CEIL_RIGHT};
  return frames;
}
inline const std::vector<sf::IntRect> &ugClimbFrames() {
  static const std::vector<sf::IntRect> frames = {
      UG_CLIMB_LEFT_WALL, UG_CLIMB_CEIL_LEFT, UG_CLIMB_RIGHT_WALL,
      UG_CLIMB_CEIL_RIGHT};
  return frames;
}
inline const std::vector<sf::IntRect> &castleClimbFrames() {
  static const std::vector<sf::IntRect> frames = {
      CASTLE_CLIMB_LEFT_WALL, CASTLE_CLIMB_CEIL_LEFT, CASTLE_CLIMB_RIGHT_WALL,
      CASTLE_CLIMB_CEIL_RIGHT};
  return frames;
}
inline const std::vector<sf::IntRect> &uwClimbFrames() {
  static const std::vector<sf::IntRect> frames = {
      UW_CLIMB_LEFT_WALL, UW_CLIMB_CEIL_LEFT, UW_CLIMB_RIGHT_WALL,
      UW_CLIMB_CEIL_RIGHT};
  return frames;
}
} // namespace BuzzyBeetleClimb

/**
 * @brief Blooper squid enemy sprite frames (Row Y=52).
 * @note Swimming squid enemy with sink/swim thrust poses.
 */
namespace Blooper {
// Overworld Palette
inline const sf::IntRect SWIM_OPEN(
    {0, 52}, {16, 24}); // STT #25: Extended body & tentacles (sinking/floating)
inline const sf::IntRect SWIM_CLOSED(
    {18, 52},
    {16, 16}); // STT #26: Contracted body with flared tentacles (upward thrust)

// Underground Palette
inline const sf::IntRect
    UG_SWIM_OPEN({74, 52}, {16, 24}); // STT #28: Underground extended body
inline const sf::IntRect
    UG_SWIM_CLOSED({92, 52}, {16, 16}); // STT #29: Underground contracted body

// Castle Palette
inline const sf::IntRect
    CASTLE_SWIM_OPEN({148, 52}, {16, 24}); // STT #31: Castle extended body
inline const sf::IntRect
    CASTLE_SWIM_CLOSED({166, 52}, {16, 16}); // STT #32: Castle contracted body

// Underwater Palette
inline const sf::IntRect
    UW_SWIM_OPEN({222, 52}, {16, 24}); // STT #34: Underwater extended body
inline const sf::IntRect
    UW_SWIM_CLOSED({240, 52}, {16, 16}); // STT #35: Underwater contracted body

// Backward-compatible aliases
inline const sf::IntRect &UG_CASTLE_SWIM_OPEN = UG_SWIM_OPEN;
inline const sf::IntRect &UG_CASTLE_SWIM_CLOSED = UG_SWIM_CLOSED;

inline const std::vector<sf::IntRect> &swimFrames() {
  static const std::vector<sf::IntRect> frames = {SWIM_OPEN, SWIM_CLOSED};
  return frames;
}
} // namespace Blooper

/**
 * @brief Bullet Bill projectile sprite frames (Row Y=53).
 * @note Cannon bullet enemy moving horizontally left.
 */
namespace BulletBill {
// Overworld Palette
inline const sf::IntRect BULLET({54, 52},
                                {16, 16}); // STT #27: Overworld Bullet Bill

// Underground Palette
inline const sf::IntRect
    UG_BULLET({128, 52}, {16, 16}); // STT #30: Underground Bullet Bill

// Castle Palette
inline const sf::IntRect CASTLE_BULLET({202, 52},
                                       {16, 16}); // STT #33: Castle Bullet Bill

// Underwater Palette
inline const sf::IntRect UW_BULLET({276, 52},
                                   {16, 16}); // STT #36: Underwater Bullet Bill

// Backward-compatible alias. Use UG_BULLET or CASTLE_BULLET when the palette
// must be explicit; the atlas does not have one shared UG/Castle cell here.
inline const sf::IntRect &UG_CASTLE_BULLET = CASTLE_BULLET;
} // namespace BulletBill

/**
 * @brief Podoboo / Lava Bubble sprite frame (Row Y=370).
 * @note Lava bubble jumping straight up from lava pit.
 */
namespace Podoboo {
inline const sf::IntRect
    BUBBLE({90, 370}, {16, 16}); // STT #157: Lava Bubble / Podoboo (14x16)
inline const sf::IntRect BUBBLE_GRID({90, 370},
                                     {16, 16}); // Grid aligned box (16x16)
} // namespace Podoboo

/**
 * @brief Koopa Troopa and shell sprite frames grouped by enemy type.
 *
 * These are the canonical Koopa frame definitions. The old
 * GreenEnemiesBlock names below are aliases for source compatibility with
 * code that follows the physical atlas layout.
 */
namespace Koopa {
// Overworld palette.
inline const sf::IntRect WALK1({0, 112}, {16, 24});
inline const sf::IntRect WALK2({18, 112}, {16, 24});
inline const sf::IntRect SHELL_RETRACTED({72, 120}, {16, 16});
inline const sf::IntRect &SHELL = SHELL_RETRACTED;
inline const sf::IntRect SHELL_WAKING({90, 120}, {16, 16});

// The verified atlas has no separate kicked-shell drawing; kicking changes
// the shell state/velocity while keeping the retracted shell frame.
inline const sf::IntRect &SHELL_KICKED = SHELL_RETRACTED;

// Underground + Castle palette mapping used by the runtime.
inline const sf::IntRect UG_WALK1({146, 112}, {16, 24});
inline const sf::IntRect UG_WALK2({164, 112}, {16, 24});
inline const sf::IntRect UG_SHELL_RETRACTED({218, 120}, {16, 16});
inline const sf::IntRect &UG_SHELL = UG_SHELL_RETRACTED;
inline const sf::IntRect UG_SHELL_WAKING({236, 120}, {16, 16});
inline const sf::IntRect &UG_SHELL_KICKED = UG_SHELL_RETRACTED;

// Castle uses the same palette as Underground in this atlas.
inline const sf::IntRect &CASTLE_WALK1 = UG_WALK1;
inline const sf::IntRect &CASTLE_WALK2 = UG_WALK2;
inline const sf::IntRect &CASTLE_SHELL_RETRACTED = UG_SHELL_RETRACTED;
inline const sf::IntRect &CASTLE_SHELL = UG_SHELL;
inline const sf::IntRect &CASTLE_SHELL_WAKING = UG_SHELL_WAKING;
inline const sf::IntRect &CASTLE_SHELL_KICKED = UG_SHELL_RETRACTED;

// Underwater palette mapping used by the theme catalog.
inline const sf::IntRect UW_WALK1({292, 112}, {16, 24});
inline const sf::IntRect UW_WALK2({310, 112}, {16, 24});
inline const sf::IntRect UW_SHELL_RETRACTED({364, 120}, {16, 16});
inline const sf::IntRect &UW_SHELL = UW_SHELL_RETRACTED;
inline const sf::IntRect UW_SHELL_WAKING({382, 120}, {16, 16});
inline const sf::IntRect &UW_SHELL_KICKED = UW_SHELL_RETRACTED;

inline const std::vector<sf::IntRect> &walkFrames() {
  static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
  return frames;
}
inline const std::vector<sf::IntRect> &ugWalkFrames() {
  static const std::vector<sf::IntRect> frames = {UG_WALK1, UG_WALK2};
  return frames;
}
inline const std::vector<sf::IntRect> &uwWalkFrames() {
  static const std::vector<sf::IntRect> frames = {UW_WALK1, UW_WALK2};
  return frames;
}
} // namespace Koopa

//
// GREEN ENEMIES BLOCK  3 palette groups (OW / UG+Castle / UW) [STT #53..#70]
//

/**
 * @brief Green Enemies Block (Y=112..136) containing Koopa aliases, Koopa
 * Paratroopa, and shell aliases.
 * @note 3 environmental palette groups: Overworld / Underground+Castle /
 * Underwater.
 */
namespace GreenEnemiesBlock {
// Overworld Palette Group
inline const sf::IntRect &KOOPA_WALK1 = Koopa::WALK1;
inline const sf::IntRect &KOOPA_WALK2 = Koopa::WALK2;
inline const sf::IntRect
    PARATROOPA_FLY1({36, 112},
                    {16, 24}); // STT #55: Koopa Paratroopa winged flight 1
inline const sf::IntRect
    PARATROOPA_FLY2({54, 112},
                    {16, 24}); // STT #56: Koopa Paratroopa winged flight 2
inline const sf::IntRect &SHELL_RETRACTED = Koopa::SHELL_RETRACTED;
inline const sf::IntRect &SHELL_WAKING = Koopa::SHELL_WAKING;

// Underground + Castle Palette Group
inline const sf::IntRect &UG_KOOPA_WALK1 = Koopa::UG_WALK1;
inline const sf::IntRect &UG_KOOPA_WALK2 = Koopa::UG_WALK2;
inline const sf::IntRect
    UG_PARATROOPA_FLY1({182, 112},
                       {16, 24}); // STT #59: Underground Paratroopa fly 1
inline const sf::IntRect
    UG_PARATROOPA_FLY2({200, 112},
                       {16, 24}); // STT #60: Underground Paratroopa fly 2
inline const sf::IntRect &UG_SHELL_RETRACTED = Koopa::UG_SHELL_RETRACTED;
inline const sf::IntRect &UG_SHELL_WAKING = Koopa::UG_SHELL_WAKING;

// Underwater Palette Group
inline const sf::IntRect &UW_KOOPA_WALK1 = Koopa::UW_WALK1;
inline const sf::IntRect &UW_KOOPA_WALK2 = Koopa::UW_WALK2;
inline const sf::IntRect
    UW_PARATROOPA_FLY1({328, 112},
                       {16, 24}); // STT #63: Underwater Paratroopa fly 1
inline const sf::IntRect
    UW_PARATROOPA_FLY2({346, 112},
                       {16, 24}); // STT #64: Underwater Paratroopa fly 2
inline const sf::IntRect &UW_SHELL_RETRACTED = Koopa::UW_SHELL_RETRACTED;
inline const sf::IntRect &UW_SHELL_WAKING = Koopa::UW_SHELL_WAKING;
} // namespace GreenEnemiesBlock

/**
 * @brief Green Cheep Cheep swimming/flying fish enemy sprite frames (Row
 * Y=164).
 * @note Green fish variant with fin flapping animation (fins up & fins down
 * poses).
 */
namespace CheepCheepGreen {
// Overworld Palette (Y=164)
inline const sf::IntRect SWIM_UP({0, 164},
                                 {16, 16}); // STT #83: Overworld fins up
inline const sf::IntRect SWIM_DOWN({18, 164},
                                   {16, 16}); // STT #84: Overworld fins down

// Underground & Castle Palette
inline const sf::IntRect
    UG_CASTLE_SWIM_UP({146, 164},
                      {16, 16}); // STT #85: Underground & Castle fins up
inline const sf::IntRect
    UG_CASTLE_SWIM_DOWN({164, 164},
                        {16, 16}); // STT #86: Underground & Castle fins down

// Underwater Palette
inline const sf::IntRect UW_SWIM_UP({292, 164},
                                    {16, 16}); // STT #87: Underwater fins up
inline const sf::IntRect
    UW_SWIM_DOWN({310, 164}, {16, 16}); // STT #88: Underwater fins down

inline const std::vector<sf::IntRect> &swimFrames() {
  static const std::vector<sf::IntRect> frames = {SWIM_UP, SWIM_DOWN};
  return frames;
}
} // namespace CheepCheepGreen

namespace HammerBroThrow {
// Hammer Bro throwing (Y=172, 15x34)  3 palette groups  4 frames
// Overworld
inline const sf::IntRect THROW1({72, 172}, {16, 34});
inline const sf::IntRect THROW2({90, 172}, {16, 34});
inline const sf::IntRect THROW3({108, 172}, {16, 34});
inline const sf::IntRect THROW4({126, 172}, {16, 34});

// Underground + Castle (X + 146)
inline const sf::IntRect UG_CASTLE_THROW1({218, 172}, {16, 34});
inline const sf::IntRect UG_CASTLE_THROW2({236, 172}, {16, 34});
inline const sf::IntRect UG_CASTLE_THROW3({254, 172}, {16, 34});
inline const sf::IntRect UG_CASTLE_THROW4({272, 172}, {16, 34});

// Underwater (X + 292)
inline const sf::IntRect UW_THROW1({364, 172}, {16, 34});
inline const sf::IntRect UW_THROW2({382, 172}, {16, 34});
inline const sf::IntRect UW_THROW3({400, 172}, {16, 34});
inline const sf::IntRect UW_THROW4({418, 172}, {16, 34});

inline const std::vector<sf::IntRect> &throwFrames() {
  static const std::vector<sf::IntRect> frames = {THROW1, THROW2, THROW3,
                                                  THROW4};
  return frames;
}
inline const std::vector<sf::IntRect> &ugCastleThrowFrames() {
  static const std::vector<sf::IntRect> frames = {
      UG_CASTLE_THROW1, UG_CASTLE_THROW2, UG_CASTLE_THROW3, UG_CASTLE_THROW4};
  return frames;
}
inline const std::vector<sf::IntRect> &uwThrowFrames() {
  static const std::vector<sf::IntRect> frames = {UW_THROW1, UW_THROW2,
                                                  UW_THROW3, UW_THROW4};
  return frames;
}
} // namespace HammerBroThrow

namespace HammerBroIdle {
// Hammer Bro standing/idle (Y=182, 16x24)  3 palette groups  4 frames
// Overworld
inline const sf::IntRect IDLE1({0, 182}, {16, 24});
inline const sf::IntRect IDLE2({18, 182}, {16, 24});
inline const sf::IntRect IDLE3({36, 182}, {16, 24});
inline const sf::IntRect IDLE4({54, 182}, {16, 24});

// Underground + Castle (X + 146)
inline const sf::IntRect UG_CASTLE_IDLE1({146, 182}, {16, 24});
inline const sf::IntRect UG_CASTLE_IDLE2({164, 182}, {16, 24});
inline const sf::IntRect UG_CASTLE_IDLE3({182, 182}, {16, 24});
inline const sf::IntRect UG_CASTLE_IDLE4({200, 182}, {16, 24});

// Underwater (X + 292)
inline const sf::IntRect UW_IDLE1({292, 182}, {16, 24});
inline const sf::IntRect UW_IDLE2({310, 182}, {16, 24});
inline const sf::IntRect UW_IDLE3({328, 182}, {16, 24});
inline const sf::IntRect UW_IDLE4({346, 182}, {16, 24});

inline const std::vector<sf::IntRect> &idleFrames() {
  static const std::vector<sf::IntRect> frames = {IDLE1, IDLE2, IDLE3, IDLE4};
  return frames;
}
inline const std::vector<sf::IntRect> &ugCastleIdleFrames() {
  static const std::vector<sf::IntRect> frames = {
      UG_CASTLE_IDLE1, UG_CASTLE_IDLE2, UG_CASTLE_IDLE3, UG_CASTLE_IDLE4};
  return frames;
}
inline const std::vector<sf::IntRect> &uwIdleFrames() {
  static const std::vector<sf::IntRect> frames = {UW_IDLE1, UW_IDLE2, UW_IDLE3,
                                                  UW_IDLE4};
  return frames;
}
} // namespace HammerBroIdle

/**
 * @brief Lakitu cloud-riding turtle enemy sprite frames (Rows Y=138 & Y=146).
 * @note Cloud rider that peeks out to throw Spiny eggs and hides inside the
 * cloud.
 */
namespace Lakitu {
// Overworld Palette (Y=138 / Y=146)
inline const sf::IntRect PEEK(
    {54, 138},
    {16, 24}); // STT #72: Peeking head out of cloud (preparing to throw Spiny)
inline const sf::IntRect
    HIDE({72, 146},
         {16, 16}); // STT #80: Retracted head inside cloud (or empty cloud)

// Underground & Castle Palette
inline const sf::IntRect
    UG_CASTLE_PEEK({200, 138},
                   {16, 24}); // STT #74: Underground & Castle peeking
inline const sf::IntRect
    UG_CASTLE_HIDE({218, 146},
                   {16, 16}); // STT #81: Underground & Castle retracted/cloud

// Underwater Palette
inline const sf::IntRect UW_PEEK({346, 138},
                                 {16, 24}); // STT #76: Underwater peeking
inline const sf::IntRect
    UW_HIDE({364, 146}, {16, 16}); // STT #82: Underwater retracted/cloud

inline const std::vector<sf::IntRect> &peekFrames() {
  static const std::vector<sf::IntRect> frames = {PEEK, UG_CASTLE_PEEK,
                                                  UW_PEEK};
  return frames;
}
} // namespace Lakitu

/**
 * @brief Piranha Plant pipe enemy sprite frames (Row Y=138).
 * @note Emerging pipe plant with biting animation (mouth open & closed poses).
 */
namespace PiranhaPlant {
// Overworld Palette (Y=138, 16x24)
inline const sf::IntRect
    OPEN({18, 138}, {16, 24}); // STT #72: Overworld mouth open (showing teeth)
inline const sf::IntRect CLOSED({0, 138},
                                {16, 24}); // STT #71: Overworld mouth closed

// Underground & Castle Palette (X + 146)
inline const sf::IntRect
    UG_CASTLE_OPEN({164, 138},
                   {16, 24}); // STT #75: Underground & Castle mouth open
inline const sf::IntRect
    UG_CASTLE_CLOSED({146, 138},
                     {16, 24}); // STT #74: Underground & Castle mouth closed

// Underwater Palette (X + 292)
inline const sf::IntRect UW_OPEN({310, 138},
                                 {16, 24}); // STT #78: Underwater mouth open
inline const sf::IntRect
    UW_CLOSED({292, 138}, {16, 24}); // STT #77: Underwater mouth closed

inline const std::vector<sf::IntRect> &openFrames() {
  static const std::vector<sf::IntRect> frames = {OPEN, UG_CASTLE_OPEN,
                                                  UW_OPEN};
  return frames;
}
inline const std::vector<sf::IntRect> &ugCastleFrames() {
  static const std::vector<sf::IntRect> frames = {UG_CASTLE_OPEN,
                                                  UG_CASTLE_CLOSED};
  return frames;
}
} // namespace PiranhaPlant

namespace PiranhaPlantTall {
// Legacy name retained for source compatibility. The atlas cell is actually
// Podoboo/Lava Bubble STT #157, not a tall Piranha Plant.
inline const sf::IntRect &TALL1 = Podoboo::BUBBLE;

inline const std::vector<sf::IntRect> &tallFrames() {
  static const std::vector<sf::IntRect> frames = {TALL1};
  return frames;
}
} // namespace PiranhaPlantTall

//
// BOWSER & HAMMER BRO (Rows 11, 12, 13) [STT #113..#142]
//

namespace Bowser {
// Normal (Overworld) Palette (Y=208 & 242)
inline const sf::IntRect WALK_CLOSED({0, 208}, {32, 32});  // STT #113
inline const sf::IntRect WALK_OPEN({34, 208}, {32, 32});   // STT #114
inline const sf::IntRect FIRE_POSE1({68, 208}, {32, 32});  // STT #115
inline const sf::IntRect FIRE_POSE2({102, 208}, {32, 32}); // STT #116

inline const sf::IntRect THROW_LEFT({34, 242},
                                    {32, 26}); // STT #125: Bowser giơ búa, quay trái
inline const sf::IntRect THROW_RIGHT({68, 242},
                                     {32, 26}); // STT #126: Bowser giơ búa, quay phải

// Underground + Castle Palette (X + 146)
inline const sf::IntRect UG_WALK_CLOSED({146, 208}, {32, 32}); // STT #117
inline const sf::IntRect UG_WALK_OPEN({180, 208}, {32, 32});   // STT #118
inline const sf::IntRect UG_FIRE_POSE1({214, 208}, {32, 32});  // STT #119
inline const sf::IntRect UG_FIRE_POSE2({248, 208}, {32, 32});  // STT #120

inline const sf::IntRect
    UG_THROW_LEFT({180, 242}, {32, 26}); // STT #128: Bowser giơ búa, quay trái
inline const sf::IntRect
    UG_THROW_RIGHT({214, 242}, {32, 26}); // STT #129: Bowser giơ búa, quay phải

// Underwater Palette (X + 292)
inline const sf::IntRect UW_WALK_CLOSED({292, 208}, {32, 32}); // STT #121
inline const sf::IntRect UW_WALK_OPEN({326, 208}, {32, 32});   // STT #122
inline const sf::IntRect UW_FIRE_POSE1({360, 208}, {32, 32});  // STT #123
inline const sf::IntRect UW_FIRE_POSE2({394, 208}, {32, 32});  // STT #124

inline const sf::IntRect UW_THROW_LEFT({326, 242}, {32, 26});  // STT #131
inline const sf::IntRect UW_THROW_RIGHT({360, 242}, {32, 26}); // STT #132

inline const std::vector<sf::IntRect> &walkFrames() {
  static const std::vector<sf::IntRect> frames = {WALK_CLOSED, WALK_OPEN};
  return frames;
}
inline const std::vector<sf::IntRect> &fireFrames() {
  static const std::vector<sf::IntRect> frames = {FIRE_POSE1, FIRE_POSE2};
  return frames;
}
} // namespace Bowser

namespace BowserFireball {
// Normal
inline const sf::IntRect FIRE1({102, 242}, {24, 8}); // STT #127
inline const sf::IntRect FIRE2({102, 252}, {24, 8}); // STT #134
// Underground + Castle
inline const sf::IntRect UG_FIRE1({248, 242}, {24, 8}); // STT #130
inline const sf::IntRect UG_FIRE2({248, 252}, {24, 8}); // STT #135
// Underwater
inline const sf::IntRect UW_FIRE1({394, 242}, {24, 8}); // STT #133
inline const sf::IntRect UW_FIRE2({394, 252}, {24, 8}); // STT #136
} // namespace BowserFireball

namespace HammerBro {
// Special Castle rows (Y=276). STT #137/#138 and #141/#142 are Bowser
// hammer poses; only STT #139/#140 are Hammer Bro throw poses.
inline const sf::IntRect BOWSER_HAMMER_LEFT1({34, 276},
                                             {32, 26}); // STT #137
inline const sf::IntRect BOWSER_HAMMER_LEFT2({68, 276},
                                             {32, 26}); // STT #138
inline const sf::IntRect THROW1({144, 276},
                                {16, 26}); // STT #139: Hammer Bro throw 1
inline const sf::IntRect THROW2({162, 276},
                                {16, 26}); // STT #140: Hammer Bro throw 2
inline const sf::IntRect BOWSER_HAMMER_RIGHT1({180, 276},
                                              {32, 26}); // STT #141
inline const sf::IntRect BOWSER_HAMMER_RIGHT2({214, 276},
                                              {32, 26}); // STT #142

// Legacy aliases kept for callers that used the old, overly broad names.
inline const sf::IntRect &THROW_HIGH_LEFT1 = BOWSER_HAMMER_LEFT1;
inline const sf::IntRect &THROW_HIGH_LEFT2 = BOWSER_HAMMER_LEFT2;
inline const sf::IntRect &THROW_SOMETHING1 = THROW1;
inline const sf::IntRect &THROW_SOMETHING2 = THROW2;
inline const sf::IntRect &THROW_HIGH_RIGHT1 = BOWSER_HAMMER_RIGHT1;
inline const sf::IntRect &THROW_HIGH_RIGHT2 = BOWSER_HAMMER_RIGHT2;
} // namespace HammerBro

//
//
// RED ENEMIES SECTION  Bottom 3 rows (Y=318..370) [STT #143..#157]
//

/**
 * @brief Red Koopa Troopa, Paratroopa & Shell sprite frames (Row 1:
 * Y=318..326).
 */
namespace RedKoopa {
inline const sf::IntRect WALK1({0, 318},
                               {16, 24}); // STT #143: Red Koopa walking frame 1
inline const sf::IntRect WALK2({18, 318},
                               {16, 24}); // STT #144: Red Koopa walking frame 2
inline const sf::IntRect
    PARATROOPA1({36, 318}, {16, 24}); // STT #145: Red Paratroopa flying frame 1
inline const sf::IntRect
    PARATROOPA2({54, 318}, {16, 24}); // STT #146: Red Paratroopa flying frame 2
inline const sf::IntRect
    SHELL({72, 326}, {16, 16}); // STT #147: Red Shell stomped / retracted
inline const sf::IntRect
    SHELL_WAKING({90, 326},
                 {16, 16}); // STT #148: Red Shell waking up / extending legs

inline const std::vector<sf::IntRect> &walkFrames() {
  static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
  return frames;
}
} // namespace RedKoopa

/**
 * @brief Red Piranha Plant, Spiny Egg & Red Spiny sprite frames (Row 2:
 * Y=344..353).
 */
namespace RedRow2 {
inline const sf::IntRect
    PIRANHA_OPEN({18, 344}, {16, 24}); // STT #150: Red Piranha open mouth
inline const sf::IntRect
    PIRANHA_CLOSED({0, 344}, {16, 24}); // STT #149: Red Piranha closed mouth
inline const sf::IntRect
    SPINY_EGG1({36, 352}, {16, 16}); // STT #151: Red Spiny egg rolling frame 1
inline const sf::IntRect
    SPINY_EGG2({54, 352}, {16, 16}); // STT #152: Red Spiny egg rolling frame 2
inline const sf::IntRect
    SPINY_WALK1({72, 352}, {16, 16}); // STT #153: Red Spiny walking frame 1
inline const sf::IntRect
    SPINY_WALK2({90, 352}, {16, 16}); // STT #154: Red Spiny walking frame 2
} // namespace RedRow2

namespace RedSpiny {
inline const sf::IntRect &WALK1 = RedRow2::SPINY_WALK1;
inline const sf::IntRect &WALK2 = RedRow2::SPINY_WALK2;
inline const sf::IntRect &EGG1 = RedRow2::SPINY_EGG1;
inline const sf::IntRect &EGG2 = RedRow2::SPINY_EGG2;

inline const std::vector<sf::IntRect> &walkFrames() {
  static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
  return frames;
}
inline const std::vector<sf::IntRect> &eggFrames() {
  static const std::vector<sf::IntRect> frames = {EGG1, EGG2};
  return frames;
}
} // namespace RedSpiny

/**
 * @brief Red Cheep Cheep & Podoboo sprite frames (Row 3: Y=370).
 */
namespace RedRow3 {
inline const sf::IntRect
    CHEEP_UP({0, 370}, {16, 16}); // STT #155: Red Cheep Cheep fins up
inline const sf::IntRect
    CHEEP_DOWN({18, 370}, {16, 16}); // STT #156: Red Cheep Cheep fins down
inline const sf::IntRect
    PODOBOO({90, 370}, {16, 16}); // STT #157: Podoboo lava bubble rising flame
} // namespace RedRow3

namespace RedCheepCheep {
inline const sf::IntRect &SWIM_UP = RedRow3::CHEEP_UP;
inline const sf::IntRect &SWIM_DOWN = RedRow3::CHEEP_DOWN;

inline const std::vector<sf::IntRect> &swimFrames() {
  static const std::vector<sf::IntRect> frames = {SWIM_UP, SWIM_DOWN};
  return frames;
}
} // namespace RedCheepCheep

} // namespace Enemies

//  Items (from items_objects.png)

namespace Items {

// Mushrooms (OW)
inline const sf::IntRect SUPER_MUSHROOM({0, 8}, {16, 16});
inline const sf::IntRect ONE_UP_MUSHROOM({0, 26}, {16, 16});

// Mushrooms (UG)
inline const sf::IntRect SUPER_MUSHROOM_UG({0, 116}, {16, 16});
inline const sf::IntRect ONE_UP_MUSHROOM_UG({0, 134}, {16, 16});

// Mushrooms (Castle)
inline const sf::IntRect SUPER_MUSHROOM_CASTLE({0, 224}, {16, 16});
inline const sf::IntRect ONE_UP_MUSHROOM_CASTLE({0, 242}, {16, 16});

// Mushrooms (UW)
inline const sf::IntRect SUPER_MUSHROOM_UW({0, 332}, {16, 16});
inline const sf::IntRect ONE_UP_MUSHROOM_UW({0, 350}, {16, 16});

// Fire Flower
inline const sf::IntRect FIRE_FLOWER_1({32, 8}, {16, 16});
inline const sf::IntRect FIRE_FLOWER_2({50, 8}, {16, 16});
inline const sf::IntRect FIRE_FLOWER_3({68, 8}, {16, 16});
inline const sf::IntRect FIRE_FLOWER_4({86, 8}, {16, 16});
inline const std::vector<sf::IntRect> &fireFlowerFrames() {
  static const std::vector<sf::IntRect> frames = {FIRE_FLOWER_1, FIRE_FLOWER_2,
                                                  FIRE_FLOWER_3, FIRE_FLOWER_4};
  return frames;
}

// Super Star
inline const sf::IntRect SUPER_STAR_1({106, 8}, {16, 16});
inline const sf::IntRect SUPER_STAR_2({124, 8}, {16, 16});
inline const sf::IntRect SUPER_STAR_3({142, 8}, {16, 16});
inline const sf::IntRect SUPER_STAR_4({160, 8}, {16, 16});
inline const std::vector<sf::IntRect> &superStarFrames() {
  static const std::vector<sf::IntRect> frames = {SUPER_STAR_1, SUPER_STAR_2,
                                                  SUPER_STAR_3, SUPER_STAR_4};
  return frames;
}
inline const std::vector<sf::IntRect> &starFrames() {
  return superStarFrames();
}

// Spinning Coin (Loose Coin on ground / from block)
inline const sf::IntRect SPINNING_COIN_1({180, 36}, {8, 16});
inline const sf::IntRect SPINNING_COIN_2({190, 36}, {8, 16});
inline const sf::IntRect SPINNING_COIN_3({200, 36}, {8, 16});
inline const sf::IntRect SPINNING_COIN_4({210, 36}, {8, 16});
inline const std::vector<sf::IntRect> &spinningCoinFrames() {
  static const std::vector<sf::IntRect> frames = {
      SPINNING_COIN_1, SPINNING_COIN_2, SPINNING_COIN_3, SPINNING_COIN_4};
  return frames;
}
inline const std::vector<sf::IntRect> &coinFrames() {
  return spinningCoinFrames();
}

// Fireball (Mario)
inline const sf::IntRect FIREBALL_1({180, 54}, {8, 8});
inline const sf::IntRect FIREBALL_2({190, 54}, {8, 8});
inline const sf::IntRect FIREBALL_3({200, 54}, {8, 8});
inline const sf::IntRect FIREBALL_4({210, 54}, {8, 8});
inline const std::vector<sf::IntRect> &fireballFrames() {
  static const std::vector<sf::IntRect> frames = {FIREBALL_1, FIREBALL_2,
                                                  FIREBALL_3, FIREBALL_4};
  return frames;
}

// Fireball Explosion
inline const sf::IntRect FIREBALL_EXPLOSION_1({180, 64}, {16, 16});
inline const sf::IntRect FIREBALL_EXPLOSION_2({198, 64}, {16, 16});
inline const sf::IntRect FIREBALL_EXPLOSION_3({216, 64}, {16, 16});
inline const std::vector<sf::IntRect> &fireballExplosionFrames() {
  static const std::vector<sf::IntRect> frames = {
      FIREBALL_EXPLOSION_1, FIREBALL_EXPLOSION_2, FIREBALL_EXPLOSION_3};
  return frames;
}

// Score Text
inline const sf::IntRect SCORE_100({234, 26}, {16, 8});
inline const sf::IntRect SCORE_200({252, 26}, {16, 8});
inline const sf::IntRect SCORE_400({270, 26}, {16, 8});
inline const sf::IntRect SCORE_800({288, 26}, {16, 8});
inline const sf::IntRect SCORE_1000({234, 36}, {16, 8});
inline const sf::IntRect SCORE_2000({252, 36}, {16, 8});
inline const sf::IntRect SCORE_4000({270, 36}, {16, 8});
inline const sf::IntRect SCORE_5000({288, 36}, {16, 8});
inline const sf::IntRect SCORE_8000({234, 46}, {16, 8});
inline const sf::IntRect SCORE_1UP({252, 46}, {16, 8});

// Trampoline / Springboard
inline const sf::IntRect SPRING_IDLE({32, 44}, {16, 16});
inline const sf::IntRect SPRING_COMPRESSED({32, 64}, {16, 8});
inline const sf::IntRect SPRING_EXTENDED({32, 75}, {16, 31});
inline const sf::IntRect SPRING_MID({50, 83}, {16, 23});

// Firebar Base
inline const sf::IntRect FIREBAR_BASE({80, 64}, {32, 8});

// Moving Platform / Lift
inline const sf::IntRect PLATFORM_SHORT({52, 64}, {24, 8});
inline const sf::IntRect PLATFORM_LONG({116, 64}, {48, 8});

// Flags and Vines
inline const sf::IntRect FLAGPOLE_FLAG({68, 91}, {16, 15});
inline const sf::IntRect CASTLE_FLAG({92, 90}, {16, 16});
inline const sf::IntRect VINE_TOP({110, 90}, {16, 16});
inline const sf::IntRect VINE_STEM_1({130, 90}, {16, 16});
inline const sf::IntRect VINE_STEM_2({148, 90}, {16, 16});

} // namespace Items

//  Legacy item-atlas tile constants
// TileMap's runtime catalog is TileFrames from
// assets/textures/tiles/tileset.png. These older item-atlas constants remain
// for source compatibility only.
namespace Tiles {
constexpr int Y = 8;
constexpr int W = 16;
constexpr int H = 16;

// 4 tiles separated by 2px dark blue background gaps
inline const sf::IntRect GROUND({180, Y}, {W, H});
inline const sf::IntRect BRICK({198, Y}, {W, H});
inline const sf::IntRect USED_BLOCK({216, Y}, {W, H});
inline const sf::IntRect SPECIAL({234, Y}, {W, H});
} // namespace Tiles

//  Question Blocks & Used Blocks (from items_blocks.png, bounding box
// (80,112) -> (159,127))
namespace Blocks {
constexpr int Y = 112;
constexpr int W = 16;
constexpr int H = 16;

// 3-frame Question Block animation: X=80, 96, 112, Y=112 (1616)
inline const sf::IntRect QUESTION1({80, Y}, {W, H});
inline const sf::IntRect QUESTION2({96, Y}, {W, H});
inline const sf::IntRect QUESTION3({112, Y}, {W, H});

// Empty / Hit Used Block after item emerges: X=128, 144, Y=112 (1616)
inline const sf::IntRect EMPTY({128, Y}, {W, H});
inline const sf::IntRect EMPTY2({144, Y}, {W, H});
inline const sf::IntRect BRICK({272, Y}, {W, H});

inline const std::vector<sf::IntRect> &questionBlockFrames() {
  static const std::vector<sf::IntRect> frames = {QUESTION1, QUESTION2,
                                                  QUESTION3};
  return frames;
}

// 8x8 px Sub-rects for 4-corner Debris particle effect (BlockDebris)
inline const sf::IntRect DEBRIS_TOP_LEFT({304, 112}, {8, 8});
inline const sf::IntRect DEBRIS_TOP_RIGHT({312, 112}, {8, 8});
inline const sf::IntRect DEBRIS_BOTTOM_LEFT({304, 120}, {8, 8});
inline const sf::IntRect DEBRIS_BOTTOM_RIGHT({312, 120}, {8, 8});

inline const sf::IntRect UG_DEBRIS_TOP_LEFT({304, 128}, {8, 8});
inline const sf::IntRect UG_DEBRIS_TOP_RIGHT({312, 128}, {8, 8});
inline const sf::IntRect UG_DEBRIS_BOTTOM_LEFT({304, 136}, {8, 8});
inline const sf::IntRect UG_DEBRIS_BOTTOM_RIGHT({312, 136}, {8, 8});

inline const sf::IntRect CASTLE_DEBRIS_TOP_LEFT({304, 144}, {8, 8});
inline const sf::IntRect CASTLE_DEBRIS_TOP_RIGHT({312, 144}, {8, 8});
inline const sf::IntRect CASTLE_DEBRIS_BOTTOM_LEFT({304, 152}, {8, 8});
inline const sf::IntRect CASTLE_DEBRIS_BOTTOM_RIGHT({312, 152}, {8, 8});

inline const sf::IntRect UW_DEBRIS_TOP_LEFT({304, 160}, {8, 8});
inline const sf::IntRect UW_DEBRIS_TOP_RIGHT({312, 160}, {8, 8});
inline const sf::IntRect UW_DEBRIS_BOTTOM_LEFT({304, 168}, {8, 8});
inline const sf::IntRect UW_DEBRIS_BOTTOM_RIGHT({312, 168}, {8, 8});

inline const std::vector<sf::IntRect> &debris4Corners() {
  static const std::vector<sf::IntRect> frames = {
      DEBRIS_TOP_LEFT, DEBRIS_TOP_RIGHT, DEBRIS_BOTTOM_LEFT,
      DEBRIS_BOTTOM_RIGHT};
  return frames;
}

inline const sf::IntRect UG_QUESTION1({80, 128}, {16, 16});
inline const sf::IntRect UG_QUESTION2({96, 128}, {16, 16});
inline const sf::IntRect UG_QUESTION3({112, 128}, {16, 16});
inline const sf::IntRect UG_HIT({128, 128}, {16, 16});
inline const sf::IntRect UG_EMPTY({144, 128}, {16, 16});

inline const sf::IntRect CASTLE_QUESTION1({80, 144}, {16, 16});
inline const sf::IntRect CASTLE_QUESTION2({96, 144}, {16, 16});
inline const sf::IntRect CASTLE_QUESTION3({112, 144}, {16, 16});
inline const sf::IntRect CASTLE_HIT({128, 144}, {16, 16});
inline const sf::IntRect CASTLE_EMPTY({144, 144}, {16, 16});

inline const sf::IntRect UW_QUESTION1({80, 160}, {16, 16});
inline const sf::IntRect UW_QUESTION2({96, 160}, {16, 16});
inline const sf::IntRect UW_QUESTION3({112, 160}, {16, 16});
inline const sf::IntRect UW_HIT({128, 160}, {16, 16});
inline const sf::IntRect UW_EMPTY({144, 160}, {16, 16});

inline const std::vector<sf::IntRect> &ugQuestionBlockFrames() {
  static const std::vector<sf::IntRect> frames = {UG_QUESTION1, UG_QUESTION2,
                                                  UG_QUESTION3};
  return frames;
}
inline const std::vector<sf::IntRect> &castleQuestionBlockFrames() {
  static const std::vector<sf::IntRect> frames = {
      CASTLE_QUESTION1, CASTLE_QUESTION2, CASTLE_QUESTION3};
  return frames;
}
inline const std::vector<sf::IntRect> &uwQuestionBlockFrames() {
  static const std::vector<sf::IntRect> frames = {UW_QUESTION1, UW_QUESTION2,
                                                  UW_QUESTION3};
  return frames;
}

} // namespace Blocks

//  Modern overworld background (full generated frame)
namespace Backgrounds {
inline constexpr std::string_view WORLD_PATH = "assets/ui/bg_world.png";
inline const sf::IntRect WORLD({0, 0}, {1857, 847});
inline const sf::IntRect MOUNTAIN_BIG({0, 160}, {80, 50});
inline const sf::IntRect MOUNTAIN_SMALL({250, 160}, {64, 50});
} // namespace Backgrounds

//  Entities & Items extracted from Tileset
namespace LevelEntities {

// Map Coins (Stationary)
inline const sf::IntRect MAP_COIN_OW_1({524, 78}, {16, 16});
inline const sf::IntRect MAP_COIN_OW_2({541, 78}, {16, 16});
inline const sf::IntRect MAP_COIN_OW_3({586, 78}, {16, 16});
inline const sf::IntRect MAP_COIN_OW_4({603, 78}, {16, 16});

// Items
inline const sf::IntRect STAR({620, 112}, {16, 16});
inline const sf::IntRect TRAMPOLINE({68, 50}, {16, 16});

// Castle specific entities
inline const sf::IntRect BOWSER_AXE({298, 129}, {16, 16});
inline const sf::IntRect BRIDGE_CHAIN({0, 134}, {16, 16});

// Platforms
inline const sf::IntRect ELEVATOR_PULLEY({298, 146}, {16, 16});

inline const std::vector<sf::IntRect> &mapCoinFrames() {
  static const std::vector<sf::IntRect> frames = {MAP_COIN_OW_1, MAP_COIN_OW_2,
                                                  MAP_COIN_OW_3, MAP_COIN_OW_4};
  return frames;
}

} // namespace LevelEntities
} // namespace legacy

namespace shared {

// Mario, Luigi, fire-state, and transition frames are palette-independent.
namespace SmallMario {
// Row Y=8, each frame 1616 (All 14 Mario frames in region (0, 8) -> (261, 23))
constexpr int Y = 8;
constexpr int W = 16;
constexpr int H = 16;

// Standard Movement & Action frames (Frames 0..6)
inline const sf::IntRect IDLE({0, Y}, {W, H}); // Frame 0 (X=0..15)  Standing
inline const sf::IntRect WALK1({20, Y},
                               {W, H}); // Frame 1 (X=20..35)  Walking 1
inline const sf::IntRect WALK2({38, Y},
                               {W, H}); // Frame 2 (X=38..53)  Walking 2
inline const sf::IntRect WALK3({56, Y},
                               {W, H}); // Frame 3 (X=56..71)  Walking 3
inline const sf::IntRect SKID({76, Y},
                              {W, H}); // Frame 4 (X=76..91)  Turning / Skidding
inline const sf::IntRect JUMP({96, Y}, {W, H}); // Frame 5 (X=96..111)  Jumping
inline const sf::IntRect
    DEATH({116, Y}, {W, H}); // Frame 6 (X=116..131)  Death / Front face

// Flagpole / Climbing & Swimming frames (Frames 7..13)
inline const sf::IntRect CLIMB1({136, Y},
                                {W, H}); // Frame 7 (X=136..151)  Pole Climb 1
inline const sf::IntRect CLIMB2({154, Y},
                                {W, H}); // Frame 8 (X=154..169)  Pole Climb 2
inline const sf::IntRect SWIM1({174, Y},
                               {W, H}); // Frame 9 (X=174..189)  Swim stroke 1
inline const sf::IntRect SWIM2({192, Y},
                               {W, H}); // Frame 10 (X=192..207)  Swim stroke 2
inline const sf::IntRect SWIM3({210, Y},
                               {W, H}); // Frame 11 (X=210..225)  Swim stroke 3
inline const sf::IntRect SWIM4({228, Y},
                               {W, H}); // Frame 12 (X=228..243)  Swim stroke 4
inline const sf::IntRect SWIM5({246, Y},
                               {W, H}); // Frame 13 (X=246..261)  Swim stroke 5

// Numeric aliases FRAME0..FRAME13
inline const sf::IntRect &FRAME0 = IDLE;
inline const sf::IntRect &FRAME1 = WALK1;
inline const sf::IntRect &FRAME2 = WALK2;
inline const sf::IntRect &FRAME3 = WALK3;
inline const sf::IntRect &FRAME4 = SKID;
inline const sf::IntRect &FRAME5 = JUMP;
inline const sf::IntRect &FRAME6 = DEATH;
inline const sf::IntRect &FRAME7 = CLIMB1;
inline const sf::IntRect &FRAME8 = CLIMB2;
inline const sf::IntRect &FRAME9 = SWIM1;
inline const sf::IntRect &FRAME10 = SWIM2;
inline const sf::IntRect &FRAME11 = SWIM3;
inline const sf::IntRect &FRAME12 = SWIM4;
inline const sf::IntRect &FRAME13 = SWIM5;

inline const std::vector<sf::IntRect> &walkFrames() {
  static const std::vector<sf::IntRect> frames = {WALK1, WALK2, WALK3};
  return frames;
}

inline const std::vector<sf::IntRect> &climbFrames() {
  static const std::vector<sf::IntRect> frames = {CLIMB1, CLIMB2};
  return frames;
}

inline const std::vector<sf::IntRect> &swimFrames() {
  static const std::vector<sf::IntRect> frames = {SWIM1, SWIM2, SWIM3, SWIM4,
                                                  SWIM5};
  return frames;
}

// Helper returning all 14 Mario frame rects in region (0, 8) -> (261, 23)
inline const std::vector<sf::IntRect> &all14Frames() {
  static const std::vector<sf::IntRect> frames = {
      IDLE,   WALK1,  WALK2, WALK3, SKID,  JUMP,  DEATH,
      CLIMB1, CLIMB2, SWIM1, SWIM2, SWIM3, SWIM4, SWIM5};
  return frames;
}
} // namespace SmallMario

namespace SmallLuigi {
// Row Y=8, each frame 1616 (All 14 Small Luigi frames shifted right by X +
// 288)
constexpr int Y = 8;
constexpr int W = 16;
constexpr int H = 16;

// Movement & Action frames (Frames 0..6)
inline const sf::IntRect IDLE({288, Y}, {W, H}); // Frame 0 (X=0 + 288) Standing
inline const sf::IntRect WALK1({308, Y},
                               {W, H}); // Frame 1 (X=20 + 288)  Walking 1
inline const sf::IntRect WALK2({326, Y},
                               {W, H}); // Frame 2 (X=38 + 288)  Walking 2
inline const sf::IntRect WALK3({344, Y},
                               {W, H}); // Frame 3 (X=56 + 288)  Walking 3
inline const sf::IntRect
    SKID({364, Y}, {W, H}); // Frame 4 (X=76 + 288)  Turning / Skidding
inline const sf::IntRect JUMP({384, Y}, {W, H}); // Frame 5 (X=96 + 288) Jumping
inline const sf::IntRect
    DEATH({404, Y}, {W, H}); // Frame 6 (X=116 + 288)  Death / Front face

// Flagpole / Climbing & Swimming frames (Frames 7..13)
inline const sf::IntRect CLIMB1({424, Y},
                                {W, H}); // Frame 7 (X=136 + 288)  Pole Climb 1
inline const sf::IntRect CLIMB2({442, Y},
                                {W, H}); // Frame 8 (X=154 + 288)  Pole Climb 2
inline const sf::IntRect SWIM1({462, Y},
                               {W, H}); // Frame 9 (X=174 + 288)  Swim stroke 1
inline const sf::IntRect SWIM2({480, Y},
                               {W, H}); // Frame 10 (X=192 + 288)  Swim stroke 2
inline const sf::IntRect SWIM3({498, Y},
                               {W, H}); // Frame 11 (X=210 + 288)  Swim stroke 3
inline const sf::IntRect SWIM4({516, Y},
                               {W, H}); // Frame 12 (X=228 + 288)  Swim stroke 4
inline const sf::IntRect SWIM5({534, Y},
                               {W, H}); // Frame 13 (X=246 + 288)  Swim stroke 5

// Numeric aliases FRAME0..FRAME13
inline const sf::IntRect &FRAME0 = IDLE;
inline const sf::IntRect &FRAME1 = WALK1;
inline const sf::IntRect &FRAME2 = WALK2;
inline const sf::IntRect &FRAME3 = WALK3;
inline const sf::IntRect &FRAME4 = SKID;
inline const sf::IntRect &FRAME5 = JUMP;
inline const sf::IntRect &FRAME6 = DEATH;
inline const sf::IntRect &FRAME7 = CLIMB1;
inline const sf::IntRect &FRAME8 = CLIMB2;
inline const sf::IntRect &FRAME9 = SWIM1;
inline const sf::IntRect &FRAME10 = SWIM2;
inline const sf::IntRect &FRAME11 = SWIM3;
inline const sf::IntRect &FRAME12 = SWIM4;
inline const sf::IntRect &FRAME13 = SWIM5;

inline const std::vector<sf::IntRect> &walkFrames() {
  static const std::vector<sf::IntRect> frames = {WALK1, WALK2, WALK3};
  return frames;
}

inline const std::vector<sf::IntRect> &climbFrames() {
  static const std::vector<sf::IntRect> frames = {CLIMB1, CLIMB2};
  return frames;
}

inline const std::vector<sf::IntRect> &swimFrames() {
  static const std::vector<sf::IntRect> frames = {SWIM1, SWIM2, SWIM3, SWIM4,
                                                  SWIM5};
  return frames;
}

inline const std::vector<sf::IntRect> &all14Frames() {
  static const std::vector<sf::IntRect> frames = {
      IDLE,   WALK1,  WALK2, WALK3, SKID,  JUMP,  DEATH,
      CLIMB1, CLIMB2, SWIM1, SWIM2, SWIM3, SWIM4, SWIM5};
  return frames;
}
} // namespace SmallLuigi

namespace BigMario {
// Row Y=32 / Y=40, each frame 1632 (All 15 Big Mario frames in region (0, 31)
// -> (280, 64))
constexpr int Y = 32;
constexpr int W = 16;
constexpr int H = 32;

// Movement & Action frames (Frames 0..6)
inline const sf::IntRect IDLE({0, Y}, {W, H}); // Frame 0 (X=0..15)  Standing
inline const sf::IntRect WALK1({20, Y},
                               {W, H}); // Frame 1 (X=20..35)  Walking 1
inline const sf::IntRect WALK2({38, Y},
                               {W, H}); // Frame 2 (X=38..53)  Walking 2
inline const sf::IntRect WALK3({56, Y},
                               {W, H}); // Frame 3 (X=56..71)  Walking 3
inline const sf::IntRect SKID({76, Y},
                              {W, H}); // Frame 4 (X=76..91)  Turning / Skidding
inline const sf::IntRect JUMP({96, Y}, {W, H}); // Frame 5 (X=96..111)  Jumping
inline const sf::IntRect
    CROUCH({116, 40}, {W, 24}); // Frame 6 (X=116..131)  Crouching (Y=40, H=24)

// Flagpole / Climbing & Swimming & Action frames (Frames 7..14)  Y=31, H=32
inline const sf::IntRect CLIMB1({136, 31},
                                {W, H}); // Frame 7 (X=136..151)  Pole Climb 1
inline const sf::IntRect CLIMB2({154, 31},
                                {W, H}); // Frame 8 (X=154..169)  Pole Climb 2
inline const sf::IntRect SWIM1({174, 31},
                               {W, H}); // Frame 9 (X=174..189)  Swim stroke 1
inline const sf::IntRect SWIM2({192, 31},
                               {W, H}); // Frame 10 (X=192..207)  Swim stroke 2
inline const sf::IntRect SWIM3({210, 31},
                               {W, H}); // Frame 11 (X=210..225)  Swim stroke 3
inline const sf::IntRect SWIM4({228, 31},
                               {W, H}); // Frame 12 (X=228..243)  Swim stroke 4
inline const sf::IntRect SWIM5({246, 31},
                               {W, H}); // Frame 13 (X=246..261)  Swim stroke 5
inline const sf::IntRect
    ACTION({264, 31},
           {W, H}); // Frame 14 (X=264..279)  Throwing Fireball / Action

// Numeric aliases FRAME0..FRAME14
inline const sf::IntRect &FRAME0 = IDLE;
inline const sf::IntRect &FRAME1 = WALK1;
inline const sf::IntRect &FRAME2 = WALK2;
inline const sf::IntRect &FRAME3 = WALK3;
inline const sf::IntRect &FRAME4 = SKID;
inline const sf::IntRect &FRAME5 = JUMP;
inline const sf::IntRect &FRAME6 = CROUCH;
inline const sf::IntRect &FRAME7 = CLIMB1;
inline const sf::IntRect &FRAME8 = CLIMB2;
inline const sf::IntRect &FRAME9 = SWIM1;
inline const sf::IntRect &FRAME10 = SWIM2;
inline const sf::IntRect &FRAME11 = SWIM3;
inline const sf::IntRect &FRAME12 = SWIM4;
inline const sf::IntRect &FRAME13 = SWIM5;
inline const sf::IntRect &FRAME14 = ACTION;

inline const std::vector<sf::IntRect> &walkFrames() {
  static const std::vector<sf::IntRect> frames = {WALK1, WALK2, WALK3};
  return frames;
}

inline const std::vector<sf::IntRect> &climbFrames() {
  static const std::vector<sf::IntRect> frames = {CLIMB1, CLIMB2};
  return frames;
}

inline const std::vector<sf::IntRect> &swimFrames() {
  static const std::vector<sf::IntRect> frames = {SWIM1, SWIM2, SWIM3, SWIM4,
                                                  SWIM5};
  return frames;
}

inline const std::vector<sf::IntRect> &all15Frames() {
  static const std::vector<sf::IntRect> frames = {
      IDLE,   WALK1, WALK2, WALK3, SKID,  JUMP,  CROUCH, CLIMB1,
      CLIMB2, SWIM1, SWIM2, SWIM3, SWIM4, SWIM5, ACTION};
  return frames;
}
} // namespace BigMario

namespace BigLuigi {
// Row Y=32 / Y=40, each frame 1632 (All 15 Big Luigi frames shifted right by X
// + 288)
constexpr int Y = 32;
constexpr int W = 16;
constexpr int H = 32;

// Movement & Action frames (Frames 0..6)
inline const sf::IntRect IDLE({288, Y}, {W, H});     // Frame 0 (X=0 + 288)
inline const sf::IntRect WALK1({308, Y}, {W, H});    // Frame 1 (X=20 + 288)
inline const sf::IntRect WALK2({326, Y}, {W, H});    // Frame 2 (X=38 + 288)
inline const sf::IntRect WALK3({344, Y}, {W, H});    // Frame 3 (X=56 + 288)
inline const sf::IntRect SKID({364, Y}, {W, H});     // Frame 4 (X=76 + 288)
inline const sf::IntRect JUMP({384, Y}, {W, H});     // Frame 5 (X=96 + 288)
inline const sf::IntRect CROUCH({404, 40}, {W, 24}); // Frame 6 (X=116 + 288)

// Flagpole / Climbing & Swimming & Action frames (Frames 7..14)  Y=31, H=32
inline const sf::IntRect CLIMB1({424, 31}, {W, H}); // Frame 7 (X=136 + 288)
inline const sf::IntRect CLIMB2({442, 31}, {W, H}); // Frame 8 (X=154 + 288)
inline const sf::IntRect SWIM1({462, 31}, {W, H});  // Frame 9 (X=174 + 288)
inline const sf::IntRect SWIM2({480, 31}, {W, H});  // Frame 10 (X=192 + 288)
inline const sf::IntRect SWIM3({498, 31}, {W, H});  // Frame 11 (X=210 + 288)
inline const sf::IntRect SWIM4({516, 31}, {W, H});  // Frame 12 (X=228 + 288)
inline const sf::IntRect SWIM5({534, 31}, {W, H});  // Frame 13 (X=246 + 288)
inline const sf::IntRect ACTION({552, 31}, {W, H}); // Frame 14 (X=264 + 288)

// Numeric aliases FRAME0..FRAME14
inline const sf::IntRect &FRAME0 = IDLE;
inline const sf::IntRect &FRAME1 = WALK1;
inline const sf::IntRect &FRAME2 = WALK2;
inline const sf::IntRect &FRAME3 = WALK3;
inline const sf::IntRect &FRAME4 = SKID;
inline const sf::IntRect &FRAME5 = JUMP;
inline const sf::IntRect &FRAME6 = CROUCH;
inline const sf::IntRect &FRAME7 = CLIMB1;
inline const sf::IntRect &FRAME8 = CLIMB2;
inline const sf::IntRect &FRAME9 = SWIM1;
inline const sf::IntRect &FRAME10 = SWIM2;
inline const sf::IntRect &FRAME11 = SWIM3;
inline const sf::IntRect &FRAME12 = SWIM4;
inline const sf::IntRect &FRAME13 = SWIM5;
inline const sf::IntRect &FRAME14 = ACTION;

inline const std::vector<sf::IntRect> &walkFrames() {
  static const std::vector<sf::IntRect> frames = {WALK1, WALK2, WALK3};
  return frames;
}

inline const std::vector<sf::IntRect> &climbFrames() {
  static const std::vector<sf::IntRect> frames = {CLIMB1, CLIMB2};
  return frames;
}

inline const std::vector<sf::IntRect> &swimFrames() {
  static const std::vector<sf::IntRect> frames = {SWIM1, SWIM2, SWIM3, SWIM4,
                                                  SWIM5};
  return frames;
}

inline const std::vector<sf::IntRect> &all15Frames() {
  static const std::vector<sf::IntRect> frames = {
      IDLE,   WALK1, WALK2, WALK3, SKID,  JUMP,  CROUCH, CLIMB1,
      CLIMB2, SWIM1, SWIM2, SWIM3, SWIM4, SWIM5, ACTION};
  return frames;
}
} // namespace BigLuigi

namespace FireSmallMario {
// Row Y=116 (Y=8 + 108), each frame 1616 (All 14 Small Fire Mario frames)
constexpr int Y = 116;
constexpr int W = 16;
constexpr int H = 16;

// Movement & Action frames (Frames 0..6)
inline const sf::IntRect IDLE({0, Y}, {W, H}); // Frame 0 (X=0..15)  Standing
inline const sf::IntRect WALK1({20, Y},
                               {W, H}); // Frame 1 (X=20..35)  Walking 1
inline const sf::IntRect WALK2({38, Y},
                               {W, H}); // Frame 2 (X=38..53)  Walking 2
inline const sf::IntRect WALK3({56, Y},
                               {W, H}); // Frame 3 (X=56..71)  Walking 3
inline const sf::IntRect SKID({76, Y},
                              {W, H}); // Frame 4 (X=76..91)  Turning / Skidding
inline const sf::IntRect JUMP({96, Y}, {W, H}); // Frame 5 (X=96..111)  Jumping
inline const sf::IntRect
    DEATH({116, Y}, {W, H}); // Frame 6 (X=116..131)  Death / Front face

// Flagpole / Climbing & Swimming frames (Frames 7..13)
inline const sf::IntRect CLIMB1({136, Y},
                                {W, H}); // Frame 7 (X=136..151)  Pole Climb 1
inline const sf::IntRect CLIMB2({154, Y},
                                {W, H}); // Frame 8 (X=154..169)  Pole Climb 2
inline const sf::IntRect SWIM1({174, Y},
                               {W, H}); // Frame 9 (X=174..189)  Swim stroke 1
inline const sf::IntRect SWIM2({192, Y},
                               {W, H}); // Frame 10 (X=192..207)  Swim stroke 2
inline const sf::IntRect SWIM3({210, Y},
                               {W, H}); // Frame 11 (X=210..225)  Swim stroke 3
inline const sf::IntRect SWIM4({228, Y},
                               {W, H}); // Frame 12 (X=228..243)  Swim stroke 4
inline const sf::IntRect SWIM5({246, Y},
                               {W, H}); // Frame 13 (X=246..261)  Swim stroke 5

// Numeric aliases FRAME0..FRAME13
inline const sf::IntRect &FRAME0 = IDLE;
inline const sf::IntRect &FRAME1 = WALK1;
inline const sf::IntRect &FRAME2 = WALK2;
inline const sf::IntRect &FRAME3 = WALK3;
inline const sf::IntRect &FRAME4 = SKID;
inline const sf::IntRect &FRAME5 = JUMP;
inline const sf::IntRect &FRAME6 = DEATH;
inline const sf::IntRect &FRAME7 = CLIMB1;
inline const sf::IntRect &FRAME8 = CLIMB2;
inline const sf::IntRect &FRAME9 = SWIM1;
inline const sf::IntRect &FRAME10 = SWIM2;
inline const sf::IntRect &FRAME11 = SWIM3;
inline const sf::IntRect &FRAME12 = SWIM4;
inline const sf::IntRect &FRAME13 = SWIM5;

inline const std::vector<sf::IntRect> &walkFrames() {
  static const std::vector<sf::IntRect> frames = {WALK1, WALK2, WALK3};
  return frames;
}

inline const std::vector<sf::IntRect> &climbFrames() {
  static const std::vector<sf::IntRect> frames = {CLIMB1, CLIMB2};
  return frames;
}

inline const std::vector<sf::IntRect> &swimFrames() {
  static const std::vector<sf::IntRect> frames = {SWIM1, SWIM2, SWIM3, SWIM4,
                                                  SWIM5};
  return frames;
}

inline const std::vector<sf::IntRect> &all14Frames() {
  static const std::vector<sf::IntRect> frames = {
      IDLE,   WALK1,  WALK2, WALK3, SKID,  JUMP,  DEATH,
      CLIMB1, CLIMB2, SWIM1, SWIM2, SWIM3, SWIM4, SWIM5};
  return frames;
}
} // namespace FireSmallMario

namespace FireBigMario {
// Row Y=140 / Y=148 (Y=32 + 108), each frame 1632 (All 15 Fire Big Mario
// frames)
constexpr int Y = 140;
constexpr int W = 16;
constexpr int H = 32;

// Movement & Action frames (Frames 0..6)
inline const sf::IntRect IDLE({0, Y}, {W, H}); // Frame 0 (X=0..15)  Standing
inline const sf::IntRect WALK1({20, Y},
                               {W, H}); // Frame 1 (X=20..35)  Walking 1
inline const sf::IntRect WALK2({38, Y},
                               {W, H}); // Frame 2 (X=38..53)  Walking 2
inline const sf::IntRect WALK3({56, Y},
                               {W, H}); // Frame 3 (X=56..71)  Walking 3
inline const sf::IntRect SKID({76, Y},
                              {W, H}); // Frame 4 (X=76..91)  Turning / Skidding
inline const sf::IntRect JUMP({96, Y}, {W, H}); // Frame 5 (X=96..111)  Jumping
inline const sf::IntRect
    CROUCH({116, 148},
           {W, 24}); // Frame 6 (X=116..131)  Crouching (Y=148, H=24)

// Flagpole / Climbing & Swimming & Action frames (Frames 7..14)  Y=139 (31 +
// 108), H=32
inline const sf::IntRect CLIMB1({136, 139},
                                {W, H}); // Frame 7 (X=136..151)  Pole Climb 1
inline const sf::IntRect CLIMB2({154, 139},
                                {W, H}); // Frame 8 (X=154..169)  Pole Climb 2
inline const sf::IntRect SWIM1({174, 139},
                               {W, H}); // Frame 9 (X=174..189)  Swim stroke 1
inline const sf::IntRect SWIM2({192, 139},
                               {W, H}); // Frame 10 (X=192..207)  Swim stroke 2
inline const sf::IntRect SWIM3({210, 139},
                               {W, H}); // Frame 11 (X=210..225)  Swim stroke 3
inline const sf::IntRect SWIM4({228, 139},
                               {W, H}); // Frame 12 (X=228..243)  Swim stroke 4
inline const sf::IntRect SWIM5({246, 139},
                               {W, H}); // Frame 13 (X=246..261)  Swim stroke 5
inline const sf::IntRect
    ACTION({264, 139},
           {W, H}); // Frame 14 (X=264..279)  Throwing Fireball / Action

// Numeric aliases FRAME0..FRAME14
inline const sf::IntRect &FRAME0 = IDLE;
inline const sf::IntRect &FRAME1 = WALK1;
inline const sf::IntRect &FRAME2 = WALK2;
inline const sf::IntRect &FRAME3 = WALK3;
inline const sf::IntRect &FRAME4 = SKID;
inline const sf::IntRect &FRAME5 = JUMP;
inline const sf::IntRect &FRAME6 = CROUCH;
inline const sf::IntRect &FRAME7 = CLIMB1;
inline const sf::IntRect &FRAME8 = CLIMB2;
inline const sf::IntRect &FRAME9 = SWIM1;
inline const sf::IntRect &FRAME10 = SWIM2;
inline const sf::IntRect &FRAME11 = SWIM3;
inline const sf::IntRect &FRAME12 = SWIM4;
inline const sf::IntRect &FRAME13 = SWIM5;
inline const sf::IntRect &FRAME14 = ACTION;

inline const std::vector<sf::IntRect> &walkFrames() {
  static const std::vector<sf::IntRect> frames = {WALK1, WALK2, WALK3};
  return frames;
}

inline const std::vector<sf::IntRect> &climbFrames() {
  static const std::vector<sf::IntRect> frames = {CLIMB1, CLIMB2};
  return frames;
}

inline const std::vector<sf::IntRect> &swimFrames() {
  static const std::vector<sf::IntRect> frames = {SWIM1, SWIM2, SWIM3, SWIM4,
                                                  SWIM5};
  return frames;
}

inline const std::vector<sf::IntRect> &all15Frames() {
  static const std::vector<sf::IntRect> frames = {
      IDLE,   WALK1, WALK2, WALK3, SKID,  JUMP,  CROUCH, CLIMB1,
      CLIMB2, SWIM1, SWIM2, SWIM3, SWIM4, SWIM5, ACTION};
  return frames;
}
} // namespace FireBigMario

//  Grow / Shrink Power-up Transition Frames (from MarioLuigi.png, (0, 72) ->
// (125, 103))
namespace GrowShrink {
namespace Mario {
inline const sf::IntRect SMALL({0, 88}, {16, 16}); // Small Mario (16x16)
inline const sf::IntRect MEDIUM({18, 80},
                                {16, 24}); // Medium/Half-grown Mario (16x24)
inline const sf::IntRect BIG({36, 72},
                             {16, 32}); // Fully-grown Big Mario (16x32)
inline const sf::IntRect BIG_ALT1({56, 72}, {16, 32});    // Big Alt 1 (16x32)
inline const sf::IntRect BIG_ALT2({74, 72}, {16, 32});    // Big Alt 2 (16x32)
inline const sf::IntRect SMALL_ALT1({92, 88}, {16, 16});  // Small Alt 1 (16x16)
inline const sf::IntRect SMALL_ALT2({110, 88}, {16, 16}); // Small Alt 2 (16x16)

inline const std::vector<sf::IntRect> &growSequence() {
  static const std::vector<sf::IntRect> sequence = {SMALL, MEDIUM, BIG, MEDIUM,
                                                    BIG,   MEDIUM, BIG};
  return sequence;
}

inline const std::vector<sf::IntRect> &shrinkSequence() {
  static const std::vector<sf::IntRect> sequence = {
      BIG, MEDIUM, SMALL, MEDIUM, SMALL, MEDIUM, SMALL};
  return sequence;
}
} // namespace Mario

namespace FireMario {
inline const sf::IntRect SMALL({0, 196}, {16, 16}); // Small Fire Mario (16x16)
inline const sf::IntRect MEDIUM({18, 188},
                                {16, 24});         // Medium Fire Mario (16x24)
inline const sf::IntRect BIG({36, 180}, {16, 32}); // Big Fire Mario (16x32)
inline const sf::IntRect BIG_ALT1({56, 180}, {16, 32});   // Big Alt 1 (16x32)
inline const sf::IntRect BIG_ALT2({74, 180}, {16, 32});   // Big Alt 2 (16x32)
inline const sf::IntRect SMALL_ALT1({92, 196}, {16, 16}); // Small Alt 1 (16x16)
inline const sf::IntRect SMALL_ALT2({110, 196},
                                    {16, 16}); // Small Alt 2 (16x16)

inline const std::vector<sf::IntRect> &growSequence() {
  static const std::vector<sf::IntRect> sequence = {SMALL, MEDIUM, BIG, MEDIUM,
                                                    BIG,   MEDIUM, BIG};
  return sequence;
}

inline const std::vector<sf::IntRect> &shrinkSequence() {
  static const std::vector<sf::IntRect> sequence = {
      BIG, MEDIUM, SMALL, MEDIUM, SMALL, MEDIUM, SMALL};
  return sequence;
}
} // namespace FireMario

namespace Luigi {
inline const sf::IntRect SMALL({288, 88}, {16, 16});    // Small Luigi (16x16)
inline const sf::IntRect MEDIUM({306, 80}, {16, 24});   // Medium Luigi (16x24)
inline const sf::IntRect BIG({324, 72}, {16, 32});      // Big Luigi (16x32)
inline const sf::IntRect BIG_ALT1({344, 72}, {16, 32}); // Big Alt 1 (16x32)
inline const sf::IntRect BIG_ALT2({362, 72}, {16, 32}); // Big Alt 2 (16x32)
inline const sf::IntRect SMALL_ALT1({380, 88}, {16, 16}); // Small Alt 1 (16x16)
inline const sf::IntRect SMALL_ALT2({398, 88}, {16, 16}); // Small Alt 2 (16x16)

inline const std::vector<sf::IntRect> &growSequence() {
  static const std::vector<sf::IntRect> sequence = {SMALL, MEDIUM, BIG, MEDIUM,
                                                    BIG,   MEDIUM, BIG};
  return sequence;
}

inline const std::vector<sf::IntRect> &shrinkSequence() {
  static const std::vector<sf::IntRect> sequence = {
      BIG, MEDIUM, SMALL, MEDIUM, SMALL, MEDIUM, SMALL};
  return sequence;
}
} // namespace Luigi
} // namespace GrowShrink

//  Fireball Shooting & Projectile Frames (from MarioLuigi.png, (136, 72) ->
// (223, 103))
namespace FireShooting {
namespace Mario {
inline const sf::IntRect BIG_SHOOT({136, 72},
                                   {16, 32}); // Big Fire Shooting pose (16x32)
inline const sf::IntRect
    SMALL_SHOOT1({172, 88}, {16, 16}); // Small Fire Shooting pose 1 (16x16)
inline const sf::IntRect
    SMALL_SHOOT2({190, 88}, {16, 16}); // Small Fire Shooting pose 2 (16x16)
inline const sf::IntRect
    SMALL_SHOOT3({208, 88}, {16, 16}); // Small Fire Shooting pose 3 (16x16)
inline const sf::IntRect FIREBALL_FRAME1({172, 77},
                                         {8, 8}); // Fireball Projectile 1 (8x8)
inline const sf::IntRect FIREBALL_FRAME2({182, 77},
                                         {8, 8}); // Fireball Projectile 2 (8x8)
inline const sf::IntRect FIREBALL_FRAME3({192, 77},
                                         {8, 8}); // Fireball Projectile 3 (8x8)
inline const sf::IntRect FIREBALL_FRAME4({202, 77},
                                         {8, 8}); // Fireball Projectile 4 (8x8)
} // namespace Mario

namespace FireMario {
inline const sf::IntRect BIG_SHOOT({136, 180},
                                   {16, 32}); // Big Fire Shooting pose (16x32)
inline const sf::IntRect
    SMALL_SHOOT1({172, 196}, {16, 16}); // Small Fire Shooting pose 1 (16x16)
inline const sf::IntRect
    SMALL_SHOOT2({190, 196}, {16, 16}); // Small Fire Shooting pose 2 (16x16)
inline const sf::IntRect
    SMALL_SHOOT3({208, 196}, {16, 16}); // Small Fire Shooting pose 3 (16x16)
inline const sf::IntRect FIREBALL_FRAME1({172, 185},
                                         {8, 8}); // Fireball Projectile 1 (8x8)
inline const sf::IntRect FIREBALL_FRAME2({182, 185},
                                         {8, 8}); // Fireball Projectile 2 (8x8)
inline const sf::IntRect FIREBALL_FRAME3({192, 185},
                                         {8, 8}); // Fireball Projectile 3 (8x8)
inline const sf::IntRect FIREBALL_FRAME4({202, 185},
                                         {8, 8}); // Fireball Projectile 4 (8x8)
} // namespace FireMario

namespace Luigi {
inline const sf::IntRect
    BIG_SHOOT({424, 72}, {16, 32}); // Big Luigi Fire Shooting pose (16x32)
inline const sf::IntRect
    SMALL_SHOOT1({460, 88},
                 {16, 16}); // Small Luigi Fire Shooting pose 1 (16x16)
inline const sf::IntRect
    SMALL_SHOOT2({478, 88},
                 {16, 16}); // Small Luigi Fire Shooting pose 2 (16x16)
inline const sf::IntRect
    SMALL_SHOOT3({496, 88},
                 {16, 16}); // Small Luigi Fire Shooting pose 3 (16x16)
inline const sf::IntRect FIREBALL_FRAME1({460, 77},
                                         {8, 8}); // Fireball Projectile 1 (8x8)
inline const sf::IntRect FIREBALL_FRAME2({470, 77},
                                         {8, 8}); // Fireball Projectile 2 (8x8)
inline const sf::IntRect FIREBALL_FRAME3({480, 77},
                                         {8, 8}); // Fireball Projectile 3 (8x8)
inline const sf::IntRect FIREBALL_FRAME4({490, 77},
                                         {8, 8}); // Fireball Projectile 4 (8x8)
} // namespace Luigi
} // namespace FireShooting

// Common item/effect frames are shared by every level theme.
namespace Items {
inline const sf::IntRect &FIRE_FLOWER_1 = legacy::Items::FIRE_FLOWER_1;
inline const sf::IntRect &FIRE_FLOWER_2 = legacy::Items::FIRE_FLOWER_2;
inline const sf::IntRect &FIRE_FLOWER_3 = legacy::Items::FIRE_FLOWER_3;
inline const sf::IntRect &FIRE_FLOWER_4 = legacy::Items::FIRE_FLOWER_4;
inline const std::vector<sf::IntRect> &fireFlowerFrames() {
  return legacy::Items::fireFlowerFrames();
}

inline const sf::IntRect &SUPER_STAR_1 = legacy::Items::SUPER_STAR_1;
inline const sf::IntRect &SUPER_STAR_2 = legacy::Items::SUPER_STAR_2;
inline const sf::IntRect &SUPER_STAR_3 = legacy::Items::SUPER_STAR_3;
inline const sf::IntRect &SUPER_STAR_4 = legacy::Items::SUPER_STAR_4;
inline const std::vector<sf::IntRect> &superStarFrames() {
  return legacy::Items::superStarFrames();
}
inline const std::vector<sf::IntRect> &starFrames() {
  return legacy::Items::starFrames();
}

inline const sf::IntRect &SPINNING_COIN_1 = legacy::Items::SPINNING_COIN_1;
inline const sf::IntRect &SPINNING_COIN_2 = legacy::Items::SPINNING_COIN_2;
inline const sf::IntRect &SPINNING_COIN_3 = legacy::Items::SPINNING_COIN_3;
inline const sf::IntRect &SPINNING_COIN_4 = legacy::Items::SPINNING_COIN_4;
inline const std::vector<sf::IntRect> &spinningCoinFrames() {
  return legacy::Items::spinningCoinFrames();
}
inline const std::vector<sf::IntRect> &coinFrames() {
  return legacy::Items::coinFrames();
}

inline const sf::IntRect &FIREBALL_1 = legacy::Items::FIREBALL_1;
inline const sf::IntRect &FIREBALL_2 = legacy::Items::FIREBALL_2;
inline const sf::IntRect &FIREBALL_3 = legacy::Items::FIREBALL_3;
inline const sf::IntRect &FIREBALL_4 = legacy::Items::FIREBALL_4;
inline const std::vector<sf::IntRect> &fireballFrames() {
  return legacy::Items::fireballFrames();
}

inline const sf::IntRect &FIREBALL_EXPLOSION_1 =
    legacy::Items::FIREBALL_EXPLOSION_1;
inline const sf::IntRect &FIREBALL_EXPLOSION_2 =
    legacy::Items::FIREBALL_EXPLOSION_2;
inline const sf::IntRect &FIREBALL_EXPLOSION_3 =
    legacy::Items::FIREBALL_EXPLOSION_3;
inline const std::vector<sf::IntRect> &fireballExplosionFrames() {
  return legacy::Items::fireballExplosionFrames();
}

inline const sf::IntRect &SCORE_100 = legacy::Items::SCORE_100;
inline const sf::IntRect &SCORE_200 = legacy::Items::SCORE_200;
inline const sf::IntRect &SCORE_400 = legacy::Items::SCORE_400;
inline const sf::IntRect &SCORE_800 = legacy::Items::SCORE_800;
inline const sf::IntRect &SCORE_1000 = legacy::Items::SCORE_1000;
inline const sf::IntRect &SCORE_2000 = legacy::Items::SCORE_2000;
inline const sf::IntRect &SCORE_4000 = legacy::Items::SCORE_4000;
inline const sf::IntRect &SCORE_5000 = legacy::Items::SCORE_5000;
inline const sf::IntRect &SCORE_8000 = legacy::Items::SCORE_8000;
inline const sf::IntRect &SCORE_1UP = legacy::Items::SCORE_1UP;

inline const sf::IntRect &SPRING_IDLE = legacy::Items::SPRING_IDLE;
inline const sf::IntRect &SPRING_COMPRESSED = legacy::Items::SPRING_COMPRESSED;
inline const sf::IntRect &SPRING_EXTENDED = legacy::Items::SPRING_EXTENDED;
inline const sf::IntRect &SPRING_MID = legacy::Items::SPRING_MID;
inline const sf::IntRect &FIREBAR_BASE = legacy::Items::FIREBAR_BASE;
inline const sf::IntRect &PLATFORM_SHORT = legacy::Items::PLATFORM_SHORT;
inline const sf::IntRect &PLATFORM_LONG = legacy::Items::PLATFORM_LONG;
inline const sf::IntRect &FLAGPOLE_FLAG = legacy::Items::FLAGPOLE_FLAG;
inline const sf::IntRect &CASTLE_FLAG = legacy::Items::CASTLE_FLAG;
inline const sf::IntRect &VINE_TOP = legacy::Items::VINE_TOP;
inline const sf::IntRect &VINE_STEM_1 = legacy::Items::VINE_STEM_1;
inline const sf::IntRect &VINE_STEM_2 = legacy::Items::VINE_STEM_2;
} // namespace Items

} // namespace shared
} // namespace SpriteFrames

/**
 * @file SpriteFrames.h
 * @author TV5 (Truyền)
 * @brief Named IntRect constants for cutting frames from shared spritesheets.
 * @note Pixel offsets measured from actual sheet layouts. All sprites are NES-native
 *       resolution (16×16 for small, 16×32 for big). Entity::syncPhysics() auto-scales
 *       to match entity size (e.g., 32×32 in-game).
 *
 *       Sheet reference: MarioLuigi.png (584×469) — by SuperJustinBros
 *       Left half (X<136) = Mario, Right half (X>=136) = Luigi
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

// ============================================================
// Sprite frame rects — cut from shared spritesheets
// Each IntRect = {position, size} in pixels on the source PNG
// ============================================================

namespace SpriteFrames {

// ── Mario (from MarioLuigi.png) ────────────────────────────

namespace SmallMario {
    // Row Y=8, each frame 16×16 (All 14 Mario frames in region (0, 8) -> (261, 23))
    constexpr int Y = 8;
    constexpr int W = 16;
    constexpr int H = 16;

    // Standard Movement & Action frames (Frames 0..6)
    inline const sf::IntRect IDLE   ({0,   Y}, {W, H}); // Frame 0 (X=0..15) — Standing
    inline const sf::IntRect WALK1  ({20,  Y}, {W, H}); // Frame 1 (X=20..35) — Walking 1
    inline const sf::IntRect WALK2  ({38,  Y}, {W, H}); // Frame 2 (X=38..53) — Walking 2
    inline const sf::IntRect WALK3  ({56,  Y}, {W, H}); // Frame 3 (X=56..71) — Walking 3
    inline const sf::IntRect SKID   ({76,  Y}, {W, H}); // Frame 4 (X=76..91) — Turning / Skidding
    inline const sf::IntRect JUMP   ({96,  Y}, {W, H}); // Frame 5 (X=96..111) — Jumping
    inline const sf::IntRect DEATH  ({116, Y}, {W, H}); // Frame 6 (X=116..131) — Death / Front face

    // Flagpole / Climbing & Swimming frames (Frames 7..13)
    inline const sf::IntRect CLIMB1 ({136, Y}, {W, H}); // Frame 7 (X=136..151) — Pole Climb 1
    inline const sf::IntRect CLIMB2 ({154, Y}, {W, H}); // Frame 8 (X=154..169) — Pole Climb 2
    inline const sf::IntRect SWIM1  ({174, Y}, {W, H}); // Frame 9 (X=174..189) — Swim stroke 1
    inline const sf::IntRect SWIM2  ({192, Y}, {W, H}); // Frame 10 (X=192..207) — Swim stroke 2
    inline const sf::IntRect SWIM3  ({210, Y}, {W, H}); // Frame 11 (X=210..225) — Swim stroke 3
    inline const sf::IntRect SWIM4  ({228, Y}, {W, H}); // Frame 12 (X=228..243) — Swim stroke 4
    inline const sf::IntRect SWIM5  ({246, Y}, {W, H}); // Frame 13 (X=246..261) — Swim stroke 5

    // Numeric aliases FRAME0..FRAME13
    inline const sf::IntRect& FRAME0  = IDLE;
    inline const sf::IntRect& FRAME1  = WALK1;
    inline const sf::IntRect& FRAME2  = WALK2;
    inline const sf::IntRect& FRAME3  = WALK3;
    inline const sf::IntRect& FRAME4  = SKID;
    inline const sf::IntRect& FRAME5  = JUMP;
    inline const sf::IntRect& FRAME6  = DEATH;
    inline const sf::IntRect& FRAME7  = CLIMB1;
    inline const sf::IntRect& FRAME8  = CLIMB2;
    inline const sf::IntRect& FRAME9  = SWIM1;
    inline const sf::IntRect& FRAME10 = SWIM2;
    inline const sf::IntRect& FRAME11 = SWIM3;
    inline const sf::IntRect& FRAME12 = SWIM4;
    inline const sf::IntRect& FRAME13 = SWIM5;

    inline const std::vector<sf::IntRect>& walkFrames() {
        static const std::vector<sf::IntRect> frames = {WALK1, WALK2, WALK3};
        return frames;
    }

    inline const std::vector<sf::IntRect>& climbFrames() {
        static const std::vector<sf::IntRect> frames = {CLIMB1, CLIMB2};
        return frames;
    }

    inline const std::vector<sf::IntRect>& swimFrames() {
        static const std::vector<sf::IntRect> frames = {SWIM1, SWIM2, SWIM3, SWIM4, SWIM5};
        return frames;
    }

    // Helper returning all 14 Mario frame rects in region (0, 8) -> (261, 23)
    inline const std::vector<sf::IntRect>& all14Frames() {
        static const std::vector<sf::IntRect> frames = {
            IDLE, WALK1, WALK2, WALK3, SKID, JUMP, DEATH,
            CLIMB1, CLIMB2, SWIM1, SWIM2, SWIM3, SWIM4, SWIM5
        };
        return frames;
    }
} // namespace SmallMario

namespace SmallLuigi {
    // Row Y=8, each frame 16×16 (All 14 Small Luigi frames shifted right by X + 288)
    constexpr int Y = 8;
    constexpr int W = 16;
    constexpr int H = 16;

    // Movement & Action frames (Frames 0..6)
    inline const sf::IntRect IDLE   ({288, Y}, {W, H}); // Frame 0 (X=0 + 288) — Standing
    inline const sf::IntRect WALK1  ({308, Y}, {W, H}); // Frame 1 (X=20 + 288) — Walking 1
    inline const sf::IntRect WALK2  ({326, Y}, {W, H}); // Frame 2 (X=38 + 288) — Walking 2
    inline const sf::IntRect WALK3  ({344, Y}, {W, H}); // Frame 3 (X=56 + 288) — Walking 3
    inline const sf::IntRect SKID   ({364, Y}, {W, H}); // Frame 4 (X=76 + 288) — Turning / Skidding
    inline const sf::IntRect JUMP   ({384, Y}, {W, H}); // Frame 5 (X=96 + 288) — Jumping
    inline const sf::IntRect DEATH  ({404, Y}, {W, H}); // Frame 6 (X=116 + 288) — Death / Front face

    // Flagpole / Climbing & Swimming frames (Frames 7..13)
    inline const sf::IntRect CLIMB1 ({424, Y}, {W, H}); // Frame 7 (X=136 + 288) — Pole Climb 1
    inline const sf::IntRect CLIMB2 ({442, Y}, {W, H}); // Frame 8 (X=154 + 288) — Pole Climb 2
    inline const sf::IntRect SWIM1  ({462, Y}, {W, H}); // Frame 9 (X=174 + 288) — Swim stroke 1
    inline const sf::IntRect SWIM2  ({480, Y}, {W, H}); // Frame 10 (X=192 + 288) — Swim stroke 2
    inline const sf::IntRect SWIM3  ({498, Y}, {W, H}); // Frame 11 (X=210 + 288) — Swim stroke 3
    inline const sf::IntRect SWIM4  ({516, Y}, {W, H}); // Frame 12 (X=228 + 288) — Swim stroke 4
    inline const sf::IntRect SWIM5  ({534, Y}, {W, H}); // Frame 13 (X=246 + 288) — Swim stroke 5

    // Numeric aliases FRAME0..FRAME13
    inline const sf::IntRect& FRAME0  = IDLE;
    inline const sf::IntRect& FRAME1  = WALK1;
    inline const sf::IntRect& FRAME2  = WALK2;
    inline const sf::IntRect& FRAME3  = WALK3;
    inline const sf::IntRect& FRAME4  = SKID;
    inline const sf::IntRect& FRAME5  = JUMP;
    inline const sf::IntRect& FRAME6  = DEATH;
    inline const sf::IntRect& FRAME7  = CLIMB1;
    inline const sf::IntRect& FRAME8  = CLIMB2;
    inline const sf::IntRect& FRAME9  = SWIM1;
    inline const sf::IntRect& FRAME10 = SWIM2;
    inline const sf::IntRect& FRAME11 = SWIM3;
    inline const sf::IntRect& FRAME12 = SWIM4;
    inline const sf::IntRect& FRAME13 = SWIM5;

    inline const std::vector<sf::IntRect>& walkFrames() {
        static const std::vector<sf::IntRect> frames = {WALK1, WALK2, WALK3};
        return frames;
    }

    inline const std::vector<sf::IntRect>& climbFrames() {
        static const std::vector<sf::IntRect> frames = {CLIMB1, CLIMB2};
        return frames;
    }

    inline const std::vector<sf::IntRect>& swimFrames() {
        static const std::vector<sf::IntRect> frames = {SWIM1, SWIM2, SWIM3, SWIM4, SWIM5};
        return frames;
    }

    inline const std::vector<sf::IntRect>& all14Frames() {
        static const std::vector<sf::IntRect> frames = {
            IDLE, WALK1, WALK2, WALK3, SKID, JUMP, DEATH,
            CLIMB1, CLIMB2, SWIM1, SWIM2, SWIM3, SWIM4, SWIM5
        };
        return frames;
    }
} // namespace SmallLuigi

namespace BigMario {
    // Row Y=32 / Y=40, each frame 16×32 (All 15 Big Mario frames in region (0, 31) -> (280, 64))
    constexpr int Y = 32;
    constexpr int W = 16;
    constexpr int H = 32;

    // Movement & Action frames (Frames 0..6)
    inline const sf::IntRect IDLE   ({0,   Y}, {W, H}); // Frame 0 (X=0..15) — Standing
    inline const sf::IntRect WALK1  ({20,  Y}, {W, H}); // Frame 1 (X=20..35) — Walking 1
    inline const sf::IntRect WALK2  ({38,  Y}, {W, H}); // Frame 2 (X=38..53) — Walking 2
    inline const sf::IntRect WALK3  ({56,  Y}, {W, H}); // Frame 3 (X=56..71) — Walking 3
    inline const sf::IntRect SKID   ({76,  Y}, {W, H}); // Frame 4 (X=76..91) — Turning / Skidding
    inline const sf::IntRect JUMP   ({96,  Y}, {W, H}); // Frame 5 (X=96..111) — Jumping
    inline const sf::IntRect CROUCH ({116, 40}, {W, 24}); // Frame 6 (X=116..131) — Crouching (Y=40, H=24)

    // Flagpole / Climbing & Swimming & Action frames (Frames 7..14) — Y=31, H=32
    inline const sf::IntRect CLIMB1 ({136, 31}, {W, H}); // Frame 7 (X=136..151) — Pole Climb 1
    inline const sf::IntRect CLIMB2 ({154, 31}, {W, H}); // Frame 8 (X=154..169) — Pole Climb 2
    inline const sf::IntRect SWIM1  ({174, 31}, {W, H}); // Frame 9 (X=174..189) — Swim stroke 1
    inline const sf::IntRect SWIM2  ({192, 31}, {W, H}); // Frame 10 (X=192..207) — Swim stroke 2
    inline const sf::IntRect SWIM3  ({210, 31}, {W, H}); // Frame 11 (X=210..225) — Swim stroke 3
    inline const sf::IntRect SWIM4  ({228, 31}, {W, H}); // Frame 12 (X=228..243) — Swim stroke 4
    inline const sf::IntRect SWIM5  ({246, 31}, {W, H}); // Frame 13 (X=246..261) — Swim stroke 5
    inline const sf::IntRect ACTION ({264, 31}, {W, H}); // Frame 14 (X=264..279) — Throwing Fireball / Action

    // Numeric aliases FRAME0..FRAME14
    inline const sf::IntRect& FRAME0  = IDLE;
    inline const sf::IntRect& FRAME1  = WALK1;
    inline const sf::IntRect& FRAME2  = WALK2;
    inline const sf::IntRect& FRAME3  = WALK3;
    inline const sf::IntRect& FRAME4  = SKID;
    inline const sf::IntRect& FRAME5  = JUMP;
    inline const sf::IntRect& FRAME6  = CROUCH;
    inline const sf::IntRect& FRAME7  = CLIMB1;
    inline const sf::IntRect& FRAME8  = CLIMB2;
    inline const sf::IntRect& FRAME9  = SWIM1;
    inline const sf::IntRect& FRAME10 = SWIM2;
    inline const sf::IntRect& FRAME11 = SWIM3;
    inline const sf::IntRect& FRAME12 = SWIM4;
    inline const sf::IntRect& FRAME13 = SWIM5;
    inline const sf::IntRect& FRAME14 = ACTION;

    inline const std::vector<sf::IntRect>& walkFrames() {
        static const std::vector<sf::IntRect> frames = {WALK1, WALK2, WALK3};
        return frames;
    }

    inline const std::vector<sf::IntRect>& climbFrames() {
        static const std::vector<sf::IntRect> frames = {CLIMB1, CLIMB2};
        return frames;
    }

    inline const std::vector<sf::IntRect>& swimFrames() {
        static const std::vector<sf::IntRect> frames = {SWIM1, SWIM2, SWIM3, SWIM4, SWIM5};
        return frames;
    }

    inline const std::vector<sf::IntRect>& all15Frames() {
        static const std::vector<sf::IntRect> frames = {
            IDLE, WALK1, WALK2, WALK3, SKID, JUMP, CROUCH,
            CLIMB1, CLIMB2, SWIM1, SWIM2, SWIM3, SWIM4, SWIM5, ACTION
        };
        return frames;
    }
} // namespace BigMario

namespace BigLuigi {
    // Row Y=32 / Y=40, each frame 16×32 (All 15 Big Luigi frames shifted right by X + 288)
    constexpr int Y = 32;
    constexpr int W = 16;
    constexpr int H = 32;

    // Movement & Action frames (Frames 0..6)
    inline const sf::IntRect IDLE   ({288, Y}, {W, H}); // Frame 0 (X=0 + 288)
    inline const sf::IntRect WALK1  ({308, Y}, {W, H}); // Frame 1 (X=20 + 288)
    inline const sf::IntRect WALK2  ({326, Y}, {W, H}); // Frame 2 (X=38 + 288)
    inline const sf::IntRect WALK3  ({344, Y}, {W, H}); // Frame 3 (X=56 + 288)
    inline const sf::IntRect SKID   ({364, Y}, {W, H}); // Frame 4 (X=76 + 288)
    inline const sf::IntRect JUMP   ({384, Y}, {W, H}); // Frame 5 (X=96 + 288)
    inline const sf::IntRect CROUCH ({404, 40}, {W, 24}); // Frame 6 (X=116 + 288)

    // Flagpole / Climbing & Swimming & Action frames (Frames 7..14) — Y=31, H=32
    inline const sf::IntRect CLIMB1 ({424, 31}, {W, H}); // Frame 7 (X=136 + 288)
    inline const sf::IntRect CLIMB2 ({442, 31}, {W, H}); // Frame 8 (X=154 + 288)
    inline const sf::IntRect SWIM1  ({462, 31}, {W, H}); // Frame 9 (X=174 + 288)
    inline const sf::IntRect SWIM2  ({480, 31}, {W, H}); // Frame 10 (X=192 + 288)
    inline const sf::IntRect SWIM3  ({498, 31}, {W, H}); // Frame 11 (X=210 + 288)
    inline const sf::IntRect SWIM4  ({516, 31}, {W, H}); // Frame 12 (X=228 + 288)
    inline const sf::IntRect SWIM5  ({534, 31}, {W, H}); // Frame 13 (X=246 + 288)
    inline const sf::IntRect ACTION ({552, 31}, {W, H}); // Frame 14 (X=264 + 288)

    // Numeric aliases FRAME0..FRAME14
    inline const sf::IntRect& FRAME0  = IDLE;
    inline const sf::IntRect& FRAME1  = WALK1;
    inline const sf::IntRect& FRAME2  = WALK2;
    inline const sf::IntRect& FRAME3  = WALK3;
    inline const sf::IntRect& FRAME4  = SKID;
    inline const sf::IntRect& FRAME5  = JUMP;
    inline const sf::IntRect& FRAME6  = CROUCH;
    inline const sf::IntRect& FRAME7  = CLIMB1;
    inline const sf::IntRect& FRAME8  = CLIMB2;
    inline const sf::IntRect& FRAME9  = SWIM1;
    inline const sf::IntRect& FRAME10 = SWIM2;
    inline const sf::IntRect& FRAME11 = SWIM3;
    inline const sf::IntRect& FRAME12 = SWIM4;
    inline const sf::IntRect& FRAME13 = SWIM5;
    inline const sf::IntRect& FRAME14 = ACTION;

    inline const std::vector<sf::IntRect>& walkFrames() {
        static const std::vector<sf::IntRect> frames = {WALK1, WALK2, WALK3};
        return frames;
    }

    inline const std::vector<sf::IntRect>& climbFrames() {
        static const std::vector<sf::IntRect> frames = {CLIMB1, CLIMB2};
        return frames;
    }

    inline const std::vector<sf::IntRect>& swimFrames() {
        static const std::vector<sf::IntRect> frames = {SWIM1, SWIM2, SWIM3, SWIM4, SWIM5};
        return frames;
    }

    inline const std::vector<sf::IntRect>& all15Frames() {
        static const std::vector<sf::IntRect> frames = {
            IDLE, WALK1, WALK2, WALK3, SKID, JUMP, CROUCH,
            CLIMB1, CLIMB2, SWIM1, SWIM2, SWIM3, SWIM4, SWIM5, ACTION
        };
        return frames;
    }
} // namespace BigLuigi

namespace FireSmallMario {
    // Row Y=116 (Y=8 + 108), each frame 16×16 (All 14 Small Fire Mario frames)
    constexpr int Y = 116;
    constexpr int W = 16;
    constexpr int H = 16;

    // Movement & Action frames (Frames 0..6)
    inline const sf::IntRect IDLE   ({0,   Y}, {W, H}); // Frame 0 (X=0..15) — Standing
    inline const sf::IntRect WALK1  ({20,  Y}, {W, H}); // Frame 1 (X=20..35) — Walking 1
    inline const sf::IntRect WALK2  ({38,  Y}, {W, H}); // Frame 2 (X=38..53) — Walking 2
    inline const sf::IntRect WALK3  ({56,  Y}, {W, H}); // Frame 3 (X=56..71) — Walking 3
    inline const sf::IntRect SKID   ({76,  Y}, {W, H}); // Frame 4 (X=76..91) — Turning / Skidding
    inline const sf::IntRect JUMP   ({96,  Y}, {W, H}); // Frame 5 (X=96..111) — Jumping
    inline const sf::IntRect DEATH  ({116, Y}, {W, H}); // Frame 6 (X=116..131) — Death / Front face

    // Flagpole / Climbing & Swimming frames (Frames 7..13)
    inline const sf::IntRect CLIMB1 ({136, Y}, {W, H}); // Frame 7 (X=136..151) — Pole Climb 1
    inline const sf::IntRect CLIMB2 ({154, Y}, {W, H}); // Frame 8 (X=154..169) — Pole Climb 2
    inline const sf::IntRect SWIM1  ({174, Y}, {W, H}); // Frame 9 (X=174..189) — Swim stroke 1
    inline const sf::IntRect SWIM2  ({192, Y}, {W, H}); // Frame 10 (X=192..207) — Swim stroke 2
    inline const sf::IntRect SWIM3  ({210, Y}, {W, H}); // Frame 11 (X=210..225) — Swim stroke 3
    inline const sf::IntRect SWIM4  ({228, Y}, {W, H}); // Frame 12 (X=228..243) — Swim stroke 4
    inline const sf::IntRect SWIM5  ({246, Y}, {W, H}); // Frame 13 (X=246..261) — Swim stroke 5

    // Numeric aliases FRAME0..FRAME13
    inline const sf::IntRect& FRAME0  = IDLE;
    inline const sf::IntRect& FRAME1  = WALK1;
    inline const sf::IntRect& FRAME2  = WALK2;
    inline const sf::IntRect& FRAME3  = WALK3;
    inline const sf::IntRect& FRAME4  = SKID;
    inline const sf::IntRect& FRAME5  = JUMP;
    inline const sf::IntRect& FRAME6  = DEATH;
    inline const sf::IntRect& FRAME7  = CLIMB1;
    inline const sf::IntRect& FRAME8  = CLIMB2;
    inline const sf::IntRect& FRAME9  = SWIM1;
    inline const sf::IntRect& FRAME10 = SWIM2;
    inline const sf::IntRect& FRAME11 = SWIM3;
    inline const sf::IntRect& FRAME12 = SWIM4;
    inline const sf::IntRect& FRAME13 = SWIM5;

    inline const std::vector<sf::IntRect>& walkFrames() {
        static const std::vector<sf::IntRect> frames = {WALK1, WALK2, WALK3};
        return frames;
    }

    inline const std::vector<sf::IntRect>& climbFrames() {
        static const std::vector<sf::IntRect> frames = {CLIMB1, CLIMB2};
        return frames;
    }

    inline const std::vector<sf::IntRect>& swimFrames() {
        static const std::vector<sf::IntRect> frames = {SWIM1, SWIM2, SWIM3, SWIM4, SWIM5};
        return frames;
    }

    inline const std::vector<sf::IntRect>& all14Frames() {
        static const std::vector<sf::IntRect> frames = {
            IDLE, WALK1, WALK2, WALK3, SKID, JUMP, DEATH,
            CLIMB1, CLIMB2, SWIM1, SWIM2, SWIM3, SWIM4, SWIM5
        };
        return frames;
    }
} // namespace FireSmallMario

namespace FireBigMario {
    // Row Y=140 / Y=148 (Y=32 + 108), each frame 16×32 (All 15 Fire Big Mario frames)
    constexpr int Y = 140;
    constexpr int W = 16;
    constexpr int H = 32;

    // Movement & Action frames (Frames 0..6)
    inline const sf::IntRect IDLE   ({0,   Y}, {W, H}); // Frame 0 (X=0..15) — Standing
    inline const sf::IntRect WALK1  ({20,  Y}, {W, H}); // Frame 1 (X=20..35) — Walking 1
    inline const sf::IntRect WALK2  ({38,  Y}, {W, H}); // Frame 2 (X=38..53) — Walking 2
    inline const sf::IntRect WALK3  ({56,  Y}, {W, H}); // Frame 3 (X=56..71) — Walking 3
    inline const sf::IntRect SKID   ({76,  Y}, {W, H}); // Frame 4 (X=76..91) — Turning / Skidding
    inline const sf::IntRect JUMP   ({96,  Y}, {W, H}); // Frame 5 (X=96..111) — Jumping
    inline const sf::IntRect CROUCH ({116, 148}, {W, 24}); // Frame 6 (X=116..131) — Crouching (Y=148, H=24)

    // Flagpole / Climbing & Swimming & Action frames (Frames 7..14) — Y=139 (31 + 108), H=32
    inline const sf::IntRect CLIMB1 ({136, 139}, {W, H}); // Frame 7 (X=136..151) — Pole Climb 1
    inline const sf::IntRect CLIMB2 ({154, 139}, {W, H}); // Frame 8 (X=154..169) — Pole Climb 2
    inline const sf::IntRect SWIM1  ({174, 139}, {W, H}); // Frame 9 (X=174..189) — Swim stroke 1
    inline const sf::IntRect SWIM2  ({192, 139}, {W, H}); // Frame 10 (X=192..207) — Swim stroke 2
    inline const sf::IntRect SWIM3  ({210, 139}, {W, H}); // Frame 11 (X=210..225) — Swim stroke 3
    inline const sf::IntRect SWIM4  ({228, 139}, {W, H}); // Frame 12 (X=228..243) — Swim stroke 4
    inline const sf::IntRect SWIM5  ({246, 139}, {W, H}); // Frame 13 (X=246..261) — Swim stroke 5
    inline const sf::IntRect ACTION ({264, 139}, {W, H}); // Frame 14 (X=264..279) — Throwing Fireball / Action

    // Numeric aliases FRAME0..FRAME14
    inline const sf::IntRect& FRAME0  = IDLE;
    inline const sf::IntRect& FRAME1  = WALK1;
    inline const sf::IntRect& FRAME2  = WALK2;
    inline const sf::IntRect& FRAME3  = WALK3;
    inline const sf::IntRect& FRAME4  = SKID;
    inline const sf::IntRect& FRAME5  = JUMP;
    inline const sf::IntRect& FRAME6  = CROUCH;
    inline const sf::IntRect& FRAME7  = CLIMB1;
    inline const sf::IntRect& FRAME8  = CLIMB2;
    inline const sf::IntRect& FRAME9  = SWIM1;
    inline const sf::IntRect& FRAME10 = SWIM2;
    inline const sf::IntRect& FRAME11 = SWIM3;
    inline const sf::IntRect& FRAME12 = SWIM4;
    inline const sf::IntRect& FRAME13 = SWIM5;
    inline const sf::IntRect& FRAME14 = ACTION;

    inline const std::vector<sf::IntRect>& walkFrames() {
        static const std::vector<sf::IntRect> frames = {WALK1, WALK2, WALK3};
        return frames;
    }

    inline const std::vector<sf::IntRect>& climbFrames() {
        static const std::vector<sf::IntRect> frames = {CLIMB1, CLIMB2};
        return frames;
    }

    inline const std::vector<sf::IntRect>& swimFrames() {
        static const std::vector<sf::IntRect> frames = {SWIM1, SWIM2, SWIM3, SWIM4, SWIM5};
        return frames;
    }

    inline const std::vector<sf::IntRect>& all15Frames() {
        static const std::vector<sf::IntRect> frames = {
            IDLE, WALK1, WALK2, WALK3, SKID, JUMP, CROUCH,
            CLIMB1, CLIMB2, SWIM1, SWIM2, SWIM3, SWIM4, SWIM5, ACTION
        };
        return frames;
    }
} // namespace FireBigMario

// ── Grow / Shrink Power-up Transition Frames (from MarioLuigi.png, (0, 72) -> (125, 103)) ──
namespace GrowShrink {
    namespace Mario {
        inline const sf::IntRect SMALL      ({0,   88}, {16, 16}); // Small Mario (16x16)
        inline const sf::IntRect MEDIUM     ({18,  80}, {16, 24}); // Medium/Half-grown Mario (16x24)
        inline const sf::IntRect BIG        ({36,  72}, {16, 32}); // Fully-grown Big Mario (16x32)
        inline const sf::IntRect BIG_ALT1   ({56,  72}, {16, 32}); // Big Alt 1 (16x32)
        inline const sf::IntRect BIG_ALT2   ({74,  72}, {16, 32}); // Big Alt 2 (16x32)
        inline const sf::IntRect SMALL_ALT1 ({92,  88}, {16, 16}); // Small Alt 1 (16x16)
        inline const sf::IntRect SMALL_ALT2 ({110, 88}, {16, 16}); // Small Alt 2 (16x16)

        inline const std::vector<sf::IntRect>& growSequence() {
            static const std::vector<sf::IntRect> sequence = {
                SMALL, MEDIUM, BIG, MEDIUM, BIG, MEDIUM, BIG
            };
            return sequence;
        }

        inline const std::vector<sf::IntRect>& shrinkSequence() {
            static const std::vector<sf::IntRect> sequence = {
                BIG, MEDIUM, SMALL, MEDIUM, SMALL, MEDIUM, SMALL
            };
            return sequence;
        }
    } // namespace Mario

    namespace FireMario {
        inline const sf::IntRect SMALL      ({0,   196}, {16, 16}); // Small Fire Mario (16x16)
        inline const sf::IntRect MEDIUM     ({18,  188}, {16, 24}); // Medium Fire Mario (16x24)
        inline const sf::IntRect BIG        ({36,  180}, {16, 32}); // Big Fire Mario (16x32)
        inline const sf::IntRect BIG_ALT1   ({56,  180}, {16, 32}); // Big Alt 1 (16x32)
        inline const sf::IntRect BIG_ALT2   ({74,  180}, {16, 32}); // Big Alt 2 (16x32)
        inline const sf::IntRect SMALL_ALT1 ({92,  196}, {16, 16}); // Small Alt 1 (16x16)
        inline const sf::IntRect SMALL_ALT2 ({110, 196}, {16, 16}); // Small Alt 2 (16x16)

        inline const std::vector<sf::IntRect>& growSequence() {
            static const std::vector<sf::IntRect> sequence = {
                SMALL, MEDIUM, BIG, MEDIUM, BIG, MEDIUM, BIG
            };
            return sequence;
        }

        inline const std::vector<sf::IntRect>& shrinkSequence() {
            static const std::vector<sf::IntRect> sequence = {
                BIG, MEDIUM, SMALL, MEDIUM, SMALL, MEDIUM, SMALL
            };
            return sequence;
        }
    } // namespace FireMario

    namespace Luigi {
        inline const sf::IntRect SMALL      ({288, 88}, {16, 16}); // Small Luigi (16x16)
        inline const sf::IntRect MEDIUM     ({306, 80}, {16, 24}); // Medium Luigi (16x24)
        inline const sf::IntRect BIG        ({324, 72}, {16, 32}); // Big Luigi (16x32)
        inline const sf::IntRect BIG_ALT1   ({344, 72}, {16, 32}); // Big Alt 1 (16x32)
        inline const sf::IntRect BIG_ALT2   ({362, 72}, {16, 32}); // Big Alt 2 (16x32)
        inline const sf::IntRect SMALL_ALT1 ({380, 88}, {16, 16}); // Small Alt 1 (16x16)
        inline const sf::IntRect SMALL_ALT2 ({398, 88}, {16, 16}); // Small Alt 2 (16x16)

        inline const std::vector<sf::IntRect>& growSequence() {
            static const std::vector<sf::IntRect> sequence = {
                SMALL, MEDIUM, BIG, MEDIUM, BIG, MEDIUM, BIG
            };
            return sequence;
        }

        inline const std::vector<sf::IntRect>& shrinkSequence() {
            static const std::vector<sf::IntRect> sequence = {
                BIG, MEDIUM, SMALL, MEDIUM, SMALL, MEDIUM, SMALL
            };
            return sequence;
        }
    } // namespace Luigi
} // namespace GrowShrink

// ── Fireball Shooting & Projectile Frames (from MarioLuigi.png, (136, 72) -> (223, 103)) ──
namespace FireShooting {
    namespace Mario {
        inline const sf::IntRect BIG_SHOOT       ({136, 72}, {16, 32}); // Big Fire Shooting pose (16x32)
        inline const sf::IntRect SMALL_SHOOT1    ({172, 88}, {16, 16}); // Small Fire Shooting pose 1 (16x16)
        inline const sf::IntRect SMALL_SHOOT2    ({190, 88}, {16, 16}); // Small Fire Shooting pose 2 (16x16)
        inline const sf::IntRect SMALL_SHOOT3    ({208, 88}, {16, 16}); // Small Fire Shooting pose 3 (16x16)
        inline const sf::IntRect FIREBALL_FRAME1 ({172, 77}, {8, 8});   // Fireball Projectile 1 (8x8)
        inline const sf::IntRect FIREBALL_FRAME2 ({182, 77}, {8, 8});   // Fireball Projectile 2 (8x8)
        inline const sf::IntRect FIREBALL_FRAME3 ({192, 77}, {8, 8});   // Fireball Projectile 3 (8x8)
        inline const sf::IntRect FIREBALL_FRAME4 ({202, 77}, {8, 8});   // Fireball Projectile 4 (8x8)
    } // namespace Mario

    namespace FireMario {
        inline const sf::IntRect BIG_SHOOT       ({136, 180}, {16, 32}); // Big Fire Shooting pose (16x32)
        inline const sf::IntRect SMALL_SHOOT1    ({172, 196}, {16, 16}); // Small Fire Shooting pose 1 (16x16)
        inline const sf::IntRect SMALL_SHOOT2    ({190, 196}, {16, 16}); // Small Fire Shooting pose 2 (16x16)
        inline const sf::IntRect SMALL_SHOOT3    ({208, 196}, {16, 16}); // Small Fire Shooting pose 3 (16x16)
        inline const sf::IntRect FIREBALL_FRAME1 ({172, 185}, {8, 8});   // Fireball Projectile 1 (8x8)
        inline const sf::IntRect FIREBALL_FRAME2 ({182, 185}, {8, 8});   // Fireball Projectile 2 (8x8)
        inline const sf::IntRect FIREBALL_FRAME3 ({192, 185}, {8, 8});   // Fireball Projectile 3 (8x8)
        inline const sf::IntRect FIREBALL_FRAME4 ({202, 185}, {8, 8});   // Fireball Projectile 4 (8x8)
    } // namespace FireMario

    namespace Luigi {
        inline const sf::IntRect BIG_SHOOT       ({424, 72}, {16, 32}); // Big Luigi Fire Shooting pose (16x32)
        inline const sf::IntRect SMALL_SHOOT1    ({460, 88}, {16, 16}); // Small Luigi Fire Shooting pose 1 (16x16)
        inline const sf::IntRect SMALL_SHOOT2    ({478, 88}, {16, 16}); // Small Luigi Fire Shooting pose 2 (16x16)
        inline const sf::IntRect SMALL_SHOOT3    ({496, 88}, {16, 16}); // Small Luigi Fire Shooting pose 3 (16x16)
        inline const sf::IntRect FIREBALL_FRAME1 ({460, 77}, {8, 8});   // Fireball Projectile 1 (8x8)
        inline const sf::IntRect FIREBALL_FRAME2 ({470, 77}, {8, 8});   // Fireball Projectile 2 (8x8)
        inline const sf::IntRect FIREBALL_FRAME3 ({480, 77}, {8, 8});   // Fireball Projectile 3 (8x8)
        inline const sf::IntRect FIREBALL_FRAME4 ({490, 77}, {8, 8});   // Fireball Projectile 4 (8x8)
    } // namespace Luigi
} // namespace FireShooting

// ── Enemies (from docs/assets/reference/enemies.png) ───────────────────────
// Verified coordinates from BFS connected-component extraction (157 sprites).
// World Themes for "Ground Enemies" (4-col): Overworld / Underground / Castle / Underwater
// World Themes for "Green Enemies" (3-col):  Overworld / Underground+Castle / Underwater
// "Red Enemies" use a single palette for all environments.
namespace Enemies {

    // ────────────────────────────────────────────────────────────────────────
    // GROUND ENEMIES — 4 palette columns (OW / UG / CT / UW)
    // ────────────────────────────────────────────────────────────────────────

    namespace Goomba {
        // Overworld (Brown Goomba, Y=16)
        inline const sf::IntRect WALK1   ({0,  16}, {16, 16}); // Walking frame 1
        inline const sf::IntRect WALK2   ({18, 16}, {16, 16}); // Walking frame 2
        inline const sf::IntRect STOMPED ({36, 24}, {16,  8}); // Squished flat

        // Underground (Blue Goomba, X + 74)
        inline const sf::IntRect UG_WALK1   ({74,  16}, {16, 16});
        inline const sf::IntRect UG_WALK2   ({92,  16}, {16, 16});
        inline const sf::IntRect UG_STOMPED ({110, 24}, {16,  8});

        // Castle (Grey Goomba, X + 148)
        inline const sf::IntRect CASTLE_WALK1   ({148, 16}, {16, 16});
        inline const sf::IntRect CASTLE_WALK2   ({166, 16}, {16, 16});
        inline const sf::IntRect CASTLE_STOMPED ({184, 24}, {16,  8});

        // Underwater (White/Blue Goomba, X + 222)
        inline const sf::IntRect UW_WALK1   ({222, 16}, {16, 16});
        inline const sf::IntRect UW_WALK2   ({240, 16}, {16, 16});
        inline const sf::IntRect UW_STOMPED ({258, 24}, {16,  8});

        inline const std::vector<sf::IntRect>& walkFrames() {
            static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
            return frames;
        }
        inline const std::vector<sf::IntRect>& ugWalkFrames() {
            static const std::vector<sf::IntRect> frames = {UG_WALK1, UG_WALK2};
            return frames;
        }
        inline const std::vector<sf::IntRect>& castleWalkFrames() {
            static const std::vector<sf::IntRect> frames = {CASTLE_WALK1, CASTLE_WALK2};
            return frames;
        }
        inline const std::vector<sf::IntRect>& uwWalkFrames() {
            static const std::vector<sf::IntRect> frames = {UW_WALK1, UW_WALK2};
            return frames;
        }
    } // namespace Goomba

    namespace BuzzyBeetle {
        // Overworld (Y=34..50) — 4 palette columns
        inline const sf::IntRect WALK1 ({0,  35}, {16, 15}); // Walk 1 (16x15, Y=35)
        inline const sf::IntRect WALK2 ({18, 34}, {16, 16}); // Walk 2 (16x16, Y=34)
        inline const sf::IntRect SHELL ({36, 35}, {16, 15}); // Shell (16x15, Y=35)

        // Underground (X + 74)
        inline const sf::IntRect UG_WALK1 ({74,  35}, {16, 15});
        inline const sf::IntRect UG_WALK2 ({92,  34}, {16, 16});
        inline const sf::IntRect UG_SHELL ({110, 35}, {16, 15});

        // Castle (X + 148)
        inline const sf::IntRect CASTLE_WALK1 ({148, 35}, {16, 15});
        inline const sf::IntRect CASTLE_WALK2 ({166, 34}, {16, 16});
        inline const sf::IntRect CASTLE_SHELL ({184, 35}, {16, 15});

        // Underwater (X + 222)
        inline const sf::IntRect UW_WALK1 ({222, 35}, {16, 15});
        inline const sf::IntRect UW_WALK2 ({240, 34}, {16, 16});
        inline const sf::IntRect UW_SHELL ({258, 35}, {16, 15});

        inline const std::vector<sf::IntRect>& walkFrames() {
            static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
            return frames;
        }
        inline const std::vector<sf::IntRect>& ugWalkFrames() {
            static const std::vector<sf::IntRect> frames = {UG_WALK1, UG_WALK2};
            return frames;
        }
        inline const std::vector<sf::IntRect>& castleWalkFrames() {
            static const std::vector<sf::IntRect> frames = {CASTLE_WALK1, CASTLE_WALK2};
            return frames;
        }
        inline const std::vector<sf::IntRect>& uwWalkFrames() {
            static const std::vector<sf::IntRect> frames = {UW_WALK1, UW_WALK2};
            return frames;
        }
    } // namespace BuzzyBeetle

    namespace Blooper {
        // Overworld (Y=52) — 4 palette columns
        inline const sf::IntRect SWIM_OPEN   ({0,  52}, {16, 24}); // Open tentacles (16x24)
        inline const sf::IntRect SWIM_CLOSED ({18, 52}, {16, 16}); // Closed tentacles (16x16)

        // Underground (X + 74)
        inline const sf::IntRect UG_SWIM_OPEN   ({74,  52}, {16, 24});
        inline const sf::IntRect UG_SWIM_CLOSED ({92,  52}, {16, 16});

        // Castle (X + 148)
        inline const sf::IntRect CASTLE_SWIM_OPEN   ({148, 52}, {16, 24});
        inline const sf::IntRect CASTLE_SWIM_CLOSED ({166, 52}, {16, 16});

        // Underwater (X + 222)
        inline const sf::IntRect UW_SWIM_OPEN   ({222, 52}, {16, 24});
        inline const sf::IntRect UW_SWIM_CLOSED ({240, 52}, {16, 16});

        // Backward-compatible aliases (old code used UG_CASTLE_*)
        inline const sf::IntRect& UG_CASTLE_SWIM_OPEN   = UG_SWIM_OPEN;
        inline const sf::IntRect& UG_CASTLE_SWIM_CLOSED = UG_SWIM_CLOSED;

        inline const std::vector<sf::IntRect>& swimFrames() {
            static const std::vector<sf::IntRect> frames = {SWIM_OPEN, SWIM_CLOSED};
            return frames;
        }
    } // namespace Blooper

    namespace BulletBill {
        // Overworld (Y=53, 16x14) — 4 palette columns
        inline const sf::IntRect BULLET ({54,  53}, {16, 14}); // Overworld Bullet Bill

        // Underground (X=128)
        inline const sf::IntRect UG_BULLET     ({128, 53}, {16, 14});

        // Castle (X=202)
        inline const sf::IntRect CASTLE_BULLET ({202, 53}, {16, 14});

        // Underwater (X=276)
        inline const sf::IntRect UW_BULLET     ({276, 53}, {16, 14});

        // Backward-compatible alias
        inline const sf::IntRect& UG_CASTLE_BULLET = CASTLE_BULLET;
    } // namespace BulletBill

    namespace Podoboo {
        // Firebar / Lava Bubble (Y=78..90) — 4 palette columns × 4 rotation frames
        // Overworld
        inline const sf::IntRect FLAME_V1 ({4,   78}, {8,  16}); // Vertical 1
        inline const sf::IntRect FLAME_H1 ({19,  82}, {14,  8}); // Horizontal 1
        inline const sf::IntRect FLAME_V2 ({40,  78}, {8,  16}); // Vertical 2
        inline const sf::IntRect FLAME_H2 ({55,  82}, {14,  8}); // Horizontal 2

        // Underground (X + 74)
        inline const sf::IntRect UG_FLAME_V1 ({78,  78}, {8,  16});
        inline const sf::IntRect UG_FLAME_H1 ({93,  82}, {14,  8});
        inline const sf::IntRect UG_FLAME_V2 ({114, 78}, {8,  16});
        inline const sf::IntRect UG_FLAME_H2 ({129, 82}, {14,  8});

        // Castle (X + 148)
        inline const sf::IntRect CASTLE_FLAME_V1 ({152, 78}, {8,  16});
        inline const sf::IntRect CASTLE_FLAME_H1 ({167, 82}, {14,  8});
        inline const sf::IntRect CASTLE_FLAME_V2 ({188, 78}, {8,  16});
        inline const sf::IntRect CASTLE_FLAME_H2 ({203, 82}, {14,  8});

        // Underwater (X + 222)
        inline const sf::IntRect UW_FLAME_V1 ({226, 78}, {8,  16});
        inline const sf::IntRect UW_FLAME_H1 ({241, 82}, {14,  8});
        inline const sf::IntRect UW_FLAME_V2 ({262, 78}, {8,  16});
        inline const sf::IntRect UW_FLAME_H2 ({277, 82}, {14,  8});

        // Backward-compatible aliases
        inline const sf::IntRect& UG_CASTLE_FLAME_V1 = CASTLE_FLAME_V1;
        inline const sf::IntRect& UG_CASTLE_FLAME_H1 = CASTLE_FLAME_H1;
        inline const sf::IntRect& UG_CASTLE_FLAME_V2 = CASTLE_FLAME_V2;
        inline const sf::IntRect& UG_CASTLE_FLAME_H2 = CASTLE_FLAME_H2;

        inline const std::vector<sf::IntRect>& flameFrames() {
            static const std::vector<sf::IntRect> frames = {FLAME_V1, FLAME_H1, FLAME_V2, FLAME_H2};
            return frames;
        }
    } // namespace Podoboo

    // ────────────────────────────────────────────────────────────────────────
    // GREEN ENEMIES — 3 palette groups (OW / UG+Castle / UW)
    // ────────────────────────────────────────────────────────────────────────

    namespace SpinyTurtle {
        // Overworld (Y=112..136, green+orange palette)
        inline const sf::IntRect WALK1  ({0,  113}, {16, 23}); // Walk 1 (16x23)
        inline const sf::IntRect WALK2  ({18, 112}, {16, 24}); // Walk 2 (16x24)
        inline const sf::IntRect EGG1   ({36, 113}, {16, 23}); // Egg roll 1
        inline const sf::IntRect EGG2   ({54, 112}, {16, 24}); // Egg roll 2
        inline const sf::IntRect SHELL1 ({72, 120}, {16, 14}); // Stomped shell 1
        inline const sf::IntRect SHELL2 ({90, 120}, {16, 15}); // Stomped shell 2

        // Underground + Castle (X + 146, teal+brown palette)
        inline const sf::IntRect UG_CASTLE_WALK1  ({146, 113}, {16, 23});
        inline const sf::IntRect UG_CASTLE_WALK2  ({164, 112}, {16, 24});
        inline const sf::IntRect UG_CASTLE_EGG1   ({182, 113}, {16, 23});
        inline const sf::IntRect UG_CASTLE_EGG2   ({200, 112}, {16, 24});
        inline const sf::IntRect UG_CASTLE_SHELL1 ({218, 120}, {16, 14});
        inline const sf::IntRect UG_CASTLE_SHELL2 ({236, 120}, {16, 15});

        // Underwater (X + 292, grey palette)
        inline const sf::IntRect UW_WALK1  ({292, 113}, {16, 23});
        inline const sf::IntRect UW_WALK2  ({310, 112}, {16, 24});
        inline const sf::IntRect UW_EGG1   ({328, 113}, {16, 23});
        inline const sf::IntRect UW_EGG2   ({346, 112}, {16, 24});
        inline const sf::IntRect UW_SHELL1 ({364, 120}, {16, 14});
        inline const sf::IntRect UW_SHELL2 ({382, 120}, {16, 15});

        inline const std::vector<sf::IntRect>& walkFrames() {
            static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
            return frames;
        }
        inline const std::vector<sf::IntRect>& eggFrames() {
            static const std::vector<sf::IntRect> frames = {EGG1, EGG2};
            return frames;
        }
    } // namespace SpinyTurtle

    namespace Koopa {
        // Overworld (Green Koopa Troopa, Y=138..176)
        inline const sf::IntRect WALK1         ({0,  139}, {16, 23}); // Walk 1 (16x23)
        inline const sf::IntRect WALK2         ({18, 138}, {16, 24}); // Walk 2 (16x24)
        inline const sf::IntRect SHELL_KICKED  ({54, 138}, {16, 24}); // Shell sliding / kicked (16x24)
        inline const sf::IntRect SHELL         ({72, 146}, {16, 16}); // Shell idle (16x16)
        inline const sf::IntRect SHELL_FLIPPED1({0,  164}, {16, 16}); // Upside-down shell 1 (16x16)
        inline const sf::IntRect SHELL_FLIPPED2({18, 164}, {15, 16}); // Upside-down shell 2 (15x16)

        // Underground + Castle (X + 146)
        inline const sf::IntRect UG_WALK1         ({146, 139}, {16, 23});
        inline const sf::IntRect UG_WALK2         ({164, 138}, {16, 24});
        inline const sf::IntRect UG_SHELL_KICKED  ({200, 138}, {16, 24});
        inline const sf::IntRect UG_SHELL         ({218, 146}, {16, 16});
        inline const sf::IntRect UG_SHELL_FLIPPED1({146, 164}, {16, 16});
        inline const sf::IntRect UG_SHELL_FLIPPED2({164, 164}, {15, 16});

        // Castle aliases (same palette as UG in NES)
        inline const sf::IntRect& CASTLE_WALK1         = UG_WALK1;
        inline const sf::IntRect& CASTLE_WALK2         = UG_WALK2;
        inline const sf::IntRect& CASTLE_SHELL_KICKED  = UG_SHELL_KICKED;
        inline const sf::IntRect& CASTLE_SHELL         = UG_SHELL;
        inline const sf::IntRect& CASTLE_SHELL_FLIPPED1= UG_SHELL_FLIPPED1;
        inline const sf::IntRect& CASTLE_SHELL_FLIPPED2= UG_SHELL_FLIPPED2;

        // Underwater (X + 292)
        inline const sf::IntRect UW_WALK1         ({292, 139}, {16, 23});
        inline const sf::IntRect UW_WALK2         ({310, 138}, {16, 24});
        inline const sf::IntRect UW_SHELL_KICKED  ({346, 138}, {16, 24});
        inline const sf::IntRect UW_SHELL         ({364, 146}, {16, 16});
        inline const sf::IntRect UW_SHELL_FLIPPED1({292, 164}, {16, 16});
        inline const sf::IntRect UW_SHELL_FLIPPED2({310, 164}, {15, 16});

        inline const std::vector<sf::IntRect>& walkFrames() {
            static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
            return frames;
        }
        inline const std::vector<sf::IntRect>& ugWalkFrames() {
            static const std::vector<sf::IntRect> frames = {UG_WALK1, UG_WALK2};
            return frames;
        }
        inline const std::vector<sf::IntRect>& uwWalkFrames() {
            static const std::vector<sf::IntRect> frames = {UW_WALK1, UW_WALK2};
            return frames;
        }
    } // namespace Koopa

    namespace KoopaParatroopa {
        // Koopa with wings (Y=172..206, 15x34) — 3 palette groups × 4 flight frames
        // Overworld
        inline const sf::IntRect FLY1 ({73,  172}, {15, 34}); // Wings up
        inline const sf::IntRect FLY2 ({91,  172}, {15, 34}); // Wings down
        inline const sf::IntRect FLY3 ({108, 172}, {15, 34}); // Wings up (alt)
        inline const sf::IntRect FLY4 ({126, 172}, {15, 34}); // Wings down (alt)

        // Underground + Castle (X + 146)
        inline const sf::IntRect UG_CASTLE_FLY1 ({219, 172}, {15, 34});
        inline const sf::IntRect UG_CASTLE_FLY2 ({237, 172}, {15, 34});
        inline const sf::IntRect UG_CASTLE_FLY3 ({254, 172}, {15, 34});
        inline const sf::IntRect UG_CASTLE_FLY4 ({272, 172}, {15, 34});

        // Underwater (X + 292)
        inline const sf::IntRect UW_FLY1 ({365, 172}, {15, 34});
        inline const sf::IntRect UW_FLY2 ({383, 172}, {15, 34});
        inline const sf::IntRect UW_FLY3 ({400, 172}, {15, 34});
        inline const sf::IntRect UW_FLY4 ({418, 172}, {15, 34});

        inline const std::vector<sf::IntRect>& flyFrames() {
            static const std::vector<sf::IntRect> frames = {FLY1, FLY2, FLY3, FLY4};
            return frames;
        }
        inline const std::vector<sf::IntRect>& ugCastleFlyFrames() {
            static const std::vector<sf::IntRect> frames = {UG_CASTLE_FLY1, UG_CASTLE_FLY2, UG_CASTLE_FLY3, UG_CASTLE_FLY4};
            return frames;
        }
        inline const std::vector<sf::IntRect>& uwFlyFrames() {
            static const std::vector<sf::IntRect> frames = {UW_FLY1, UW_FLY2, UW_FLY3, UW_FLY4};
            return frames;
        }
    } // namespace KoopaParatroopa

    namespace CheepCheepFly {
        // Flying/Jumping Cheep Cheep (Y=182..206, 16x24) — 3 palette groups × 4 frames
        // Overworld
        inline const sf::IntRect FLY1 ({0,  182}, {16, 24});
        inline const sf::IntRect FLY2 ({18, 182}, {16, 24});
        inline const sf::IntRect FLY3 ({37, 182}, {15, 24}); // Note: 15px wide
        inline const sf::IntRect FLY4 ({55, 182}, {15, 24}); // Note: 15px wide

        // Underground + Castle (X + 146)
        inline const sf::IntRect UG_CASTLE_FLY1 ({146, 182}, {16, 24});
        inline const sf::IntRect UG_CASTLE_FLY2 ({164, 182}, {16, 24});
        inline const sf::IntRect UG_CASTLE_FLY3 ({183, 182}, {15, 24});
        inline const sf::IntRect UG_CASTLE_FLY4 ({201, 182}, {15, 24});

        // Underwater (X + 292)
        inline const sf::IntRect UW_FLY1 ({292, 182}, {16, 24});
        inline const sf::IntRect UW_FLY2 ({310, 182}, {16, 24});
        inline const sf::IntRect UW_FLY3 ({329, 182}, {15, 24});
        inline const sf::IntRect UW_FLY4 ({347, 182}, {15, 24});

        inline const std::vector<sf::IntRect>& flyFrames() {
            static const std::vector<sf::IntRect> frames = {FLY1, FLY2, FLY3, FLY4};
            return frames;
        }
        inline const std::vector<sf::IntRect>& ugCastleFlyFrames() {
            static const std::vector<sf::IntRect> frames = {UG_CASTLE_FLY1, UG_CASTLE_FLY2, UG_CASTLE_FLY3, UG_CASTLE_FLY4};
            return frames;
        }
        inline const std::vector<sf::IntRect>& uwFlyFrames() {
            static const std::vector<sf::IntRect> frames = {UW_FLY1, UW_FLY2, UW_FLY3, UW_FLY4};
            return frames;
        }
    } // namespace CheepCheepFly

    // ────────────────────────────────────────────────────────────────────────
    // BOSSES — 3 palette groups (OW / UG+Castle / UW)
    // ────────────────────────────────────────────────────────────────────────

    namespace Bowser {
        // Bowser (Y=208..240, 32x32) — 3 palette groups × 4 frames
        // Overworld
        inline const sf::IntRect WALK1 ({0,   208}, {32, 32}); // Walk 1 (mouth closed)
        inline const sf::IntRect WALK2 ({34,  208}, {32, 32}); // Walk 2 (mouth open)
        inline const sf::IntRect FIRE1 ({68,  208}, {32, 32}); // Fire breath 1
        inline const sf::IntRect FIRE2 ({102, 208}, {32, 32}); // Fire breath 2

        // Underground + Castle (X + 146)
        inline const sf::IntRect UG_CASTLE_WALK1 ({146, 208}, {32, 32});
        inline const sf::IntRect UG_CASTLE_WALK2 ({180, 208}, {32, 32});
        inline const sf::IntRect UG_CASTLE_FIRE1 ({214, 208}, {32, 32});
        inline const sf::IntRect UG_CASTLE_FIRE2 ({248, 208}, {32, 32});

        // Underwater (X + 292)
        inline const sf::IntRect UW_WALK1 ({292, 208}, {32, 32});
        inline const sf::IntRect UW_WALK2 ({326, 208}, {32, 32});
        inline const sf::IntRect UW_FIRE1 ({360, 208}, {32, 32});
        inline const sf::IntRect UW_FIRE2 ({394, 208}, {32, 32});

        inline const std::vector<sf::IntRect>& walkFrames() {
            static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
            return frames;
        }
        inline const std::vector<sf::IntRect>& fireFrames() {
            static const std::vector<sf::IntRect> frames = {FIRE1, FIRE2};
            return frames;
        }
    } // namespace Bowser

    namespace BowserFireball {
        // Bowser Flame Breath (24x8) — 3 palette groups × 2 frames
        // Overworld
        inline const sf::IntRect FIREBALL1 ({102, 242}, {24, 8}); // Flame frame 1
        inline const sf::IntRect FIREBALL2 ({102, 252}, {24, 8}); // Flame frame 2
        inline const sf::IntRect BLOCK     ({102, 242}, {24, 18}); // Full block (both frames)

        // Underground + Castle (X + 146)
        inline const sf::IntRect UG_CASTLE_FIREBALL1 ({248, 242}, {24, 8});
        inline const sf::IntRect UG_CASTLE_FIREBALL2 ({248, 252}, {24, 8});
        inline const sf::IntRect UG_CASTLE_BLOCK     ({248, 242}, {24, 18});

        // Underwater (X + 292)
        inline const sf::IntRect UW_FIREBALL1 ({394, 242}, {24, 8});
        inline const sf::IntRect UW_FIREBALL2 ({394, 252}, {24, 8});
        inline const sf::IntRect UW_BLOCK     ({394, 242}, {24, 18});

        inline const std::vector<sf::IntRect>& fireballFrames() {
            static const std::vector<sf::IntRect> frames = {FIREBALL1, FIREBALL2};
            return frames;
        }
    } // namespace BowserFireball

    // ────────────────────────────────────────────────────────────────────────
    // IN CASTLE LEVELS — Hammer Bros & Lakitu
    // ────────────────────────────────────────────────────────────────────────

    namespace HammerBros {
        // Hammer Bros (Y=242, 30x26) — 3 palette groups × 2 throwing frames
        // Overworld
        inline const sf::IntRect THROW1 ({34,  242}, {30, 26}); // Arm up (throwing)
        inline const sf::IntRect THROW2 ({70,  242}, {30, 26}); // Arm down

        // Underground + Castle (X + 146)
        inline const sf::IntRect UG_CASTLE_THROW1 ({180, 242}, {30, 26});
        inline const sf::IntRect UG_CASTLE_THROW2 ({216, 242}, {30, 26});

        // Underwater (X + 292)
        inline const sf::IntRect UW_THROW1 ({326, 242}, {30, 26});
        inline const sf::IntRect UW_THROW2 ({362, 242}, {30, 26});

        inline const std::vector<sf::IntRect>& throwFrames() {
            static const std::vector<sf::IntRect> frames = {THROW1, THROW2};
            return frames;
        }
    } // namespace HammerBros

    namespace Hammer {
        // Flying Hammer projectile (24x8) — 3 palette groups × 2 rotation frames
        // Overworld
        inline const sf::IntRect SPIN1 ({102, 242}, {24, 8}); // Rotation frame 1
        inline const sf::IntRect SPIN2 ({102, 252}, {24, 8}); // Rotation frame 2

        // Underground + Castle (X + 146)
        inline const sf::IntRect UG_CASTLE_SPIN1 ({248, 242}, {24, 8});
        inline const sf::IntRect UG_CASTLE_SPIN2 ({248, 252}, {24, 8});

        // Underwater (X + 292)
        inline const sf::IntRect UW_SPIN1 ({394, 242}, {24, 8});
        inline const sf::IntRect UW_SPIN2 ({394, 252}, {24, 8});

        inline const std::vector<sf::IntRect>& spinFrames() {
            static const std::vector<sf::IntRect> frames = {SPIN1, SPIN2};
            return frames;
        }
    } // namespace Hammer

    namespace Lakitu {
        // Lakitu on cloud (Y=276, 30x26) — 2 palette groups × 2 frames
        // Overworld
        inline const sf::IntRect PEEK1 ({34, 276}, {30, 26}); // Looking down
        inline const sf::IntRect PEEK2 ({70, 276}, {30, 26}); // Throwing Spiny

        // Underground + Castle (X + 146)
        inline const sf::IntRect UG_CASTLE_PEEK1 ({180, 276}, {30, 26});
        inline const sf::IntRect UG_CASTLE_PEEK2 ({216, 276}, {30, 26});

        inline const std::vector<sf::IntRect>& peekFrames() {
            static const std::vector<sf::IntRect> frames = {PEEK1, PEEK2};
            return frames;
        }
    } // namespace Lakitu

    // ────────────────────────────────────────────────────────────────────────
    // PIRANHA PLANT — multiple palette variants
    // ────────────────────────────────────────────────────────────────────────

    namespace PiranhaPlant {
        // Underground + Castle green Piranha (Y=276, 15x26)
        inline const sf::IntRect UG_CASTLE_OPEN   ({145, 276}, {15, 26}); // Mouth open
        inline const sf::IntRect UG_CASTLE_CLOSED ({162, 276}, {15, 26}); // Mouth closed

        inline const std::vector<sf::IntRect>& ugCastleFrames() {
            static const std::vector<sf::IntRect> frames = {UG_CASTLE_OPEN, UG_CASTLE_CLOSED};
            return frames;
        }
    } // namespace PiranhaPlant

    namespace PiranhaPlantTall {
        // Red/Orange Piranha Plant — Overworld (Y=370, 14-16x16)
        inline const sf::IntRect TALL1 ({91, 370}, {14, 16}); // Overworld open mouth

        inline const std::vector<sf::IntRect>& tallFrames() {
            static const std::vector<sf::IntRect> frames = {TALL1};
            return frames;
        }
    } // namespace PiranhaPlantTall

    // ────────────────────────────────────────────────────────────────────────
    // RED ENEMIES — single palette for all environments
    // ────────────────────────────────────────────────────────────────────────

    namespace RedSpiny {
        // Red Spiny (Y=318..340, red+orange palette) — All Environments
        inline const sf::IntRect WALK1  ({0,  319}, {16, 23}); // Walk 1
        inline const sf::IntRect WALK2  ({18, 318}, {16, 24}); // Walk 2
        inline const sf::IntRect EGG1   ({36, 319}, {16, 23}); // Egg roll 1
        inline const sf::IntRect EGG2   ({54, 318}, {16, 24}); // Egg roll 2
        inline const sf::IntRect SHELL1 ({72, 326}, {16, 14}); // Stomped shell 1
        inline const sf::IntRect SHELL2 ({90, 326}, {16, 15}); // Stomped shell 2

        inline const std::vector<sf::IntRect>& walkFrames() {
            static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
            return frames;
        }
        inline const std::vector<sf::IntRect>& eggFrames() {
            static const std::vector<sf::IntRect> frames = {EGG1, EGG2};
            return frames;
        }
    } // namespace RedSpiny

    namespace RedKoopa {
        // Red Koopa Troopa (Y=344..386, red palette) — All Environments
        inline const sf::IntRect WALK1         ({0,  345}, {16, 23}); // Walk 1
        inline const sf::IntRect WALK2         ({18, 344}, {16, 24}); // Walk 2
        inline const sf::IntRect SHELL_KICKED1 ({37, 352}, {14, 16}); // Shell sliding 1
        inline const sf::IntRect SHELL_KICKED2 ({55, 352}, {14, 16}); // Shell sliding 2
        inline const sf::IntRect SHELL1        ({72, 353}, {16, 15}); // Shell idle 1
        inline const sf::IntRect SHELL2        ({90, 352}, {16, 16}); // Shell idle 2
        inline const sf::IntRect FLIPPED1      ({0,  370}, {16, 16}); // Upside-down shell 1
        inline const sf::IntRect FLIPPED2      ({18, 370}, {15, 16}); // Upside-down shell 2

        inline const std::vector<sf::IntRect>& walkFrames() {
            static const std::vector<sf::IntRect> frames = {WALK1, WALK2};
            return frames;
        }
    } // namespace RedKoopa

    namespace RedEnemies {
        // Backward-compatible aliases for old code referencing RedEnemies::
        // These actually point to RedSpiny and RedKoopa sprites
        inline const sf::IntRect& CHEEP_SWIM1    = RedSpiny::WALK2;     // Was misidentified
        inline const sf::IntRect& CHEEP_SWIM2    = RedSpiny::EGG2;      // Was misidentified
        inline const sf::IntRect& PIRANHA_OPEN   = RedSpiny::SHELL1;    // Was misidentified
        inline const sf::IntRect& PIRANHA_CLOSED = RedSpiny::SHELL2;    // Was misidentified
        inline const sf::IntRect& CHEEP_FLY1     = RedKoopa::WALK1;     // Was misidentified
        inline const sf::IntRect& CHEEP_FLY2     = RedKoopa::WALK2;     // Was misidentified
        inline const sf::IntRect& RED_WALK1      = RedKoopa::FLIPPED1;  // Was misidentified
        inline const sf::IntRect& RED_WALK2      = RedKoopa::FLIPPED2;  // Was misidentified
    } // namespace RedEnemies

} // namespace Enemies

// ── Items (from items_objects.png) ─────────────────────────

namespace Items {
    // Overworld Objects row Y=8
    constexpr int Y = 8;
    constexpr int W = 16;
    constexpr int H = 16;

    // Mushroom 1 (Super Mushroom — Red) — top sprite in column X=0..15, Y=8..23 (16×16)
    inline const sf::IntRect MUSHROOM({0, 8}, {16, 16});
    inline const sf::IntRect SUPER_MUSHROOM({0, 8}, {16, 16});

    // Mushroom 2 (1UP Mushroom — Green) — bottom sprite in column X=0..15, Y=26..41 (16×16)
    inline const sf::IntRect ONE_UP_MUSHROOM({0, 26}, {16, 16});

    // Fire Flower — 4-frame animation
    inline const sf::IntRect FIRE_FLOWER1({50,  Y}, {W, H});
    inline const sf::IntRect FIRE_FLOWER2({68,  Y}, {W, H});
    inline const sf::IntRect FIRE_FLOWER3({86,  Y}, {W, H});
    inline const sf::IntRect FIRE_FLOWER4({106, Y}, {W, H});

    inline const std::vector<sf::IntRect>& fireFlowerFrames() {
        static const std::vector<sf::IntRect> frames = {
            FIRE_FLOWER1, FIRE_FLOWER2, FIRE_FLOWER3, FIRE_FLOWER4
        };
        return frames;
    }

    // Starman — 4-frame animation (cycling colors)
    inline const sf::IntRect STAR1({124, Y}, {W, H});
    inline const sf::IntRect STAR2({142, Y}, {W, H});
    inline const sf::IntRect STAR3({160, Y}, {W, H});
    inline const sf::IntRect STAR4({124, Y}, {W, H}); // loops back

    inline const std::vector<sf::IntRect>& starFrames() {
        static const std::vector<sf::IntRect> frames = {STAR1, STAR2, STAR3};
        return frames;
    }

    // Coin — 4-frame animation from items_objects.png (180, 36) to (217, 51)
    // Each frame is 8x16 px, separated by 2px dark blue background columns (x=188..189, 198..199, 208..209)
    inline const sf::IntRect COIN1({180, 36}, {8, 16});
    inline const sf::IntRect COIN2({190, 36}, {8, 16});
    inline const sf::IntRect COIN3({200, 36}, {8, 16});
    inline const sf::IntRect COIN4({210, 36}, {8, 16});

    inline const std::vector<sf::IntRect>& coinFrames() {
        static const std::vector<sf::IntRect> frames = {COIN1, COIN2, COIN3, COIN4};
        return frames;
    }
} // namespace Items

// ── Tiles (from items_objects.png, bounding box (180,8) -> (249,23)) ──
namespace Tiles {
    constexpr int Y = 8;
    constexpr int W = 16;
    constexpr int H = 16;

    // 4 tiles separated by 2px dark blue background gaps
    inline const sf::IntRect GROUND    ({180, Y}, {W, H});
    inline const sf::IntRect BRICK     ({198, Y}, {W, H});
    inline const sf::IntRect USED_BLOCK({216, Y}, {W, H});
    inline const sf::IntRect SPECIAL   ({234, Y}, {W, H});
} // namespace Tiles

// ── Question Blocks & Used Blocks (from items_blocks.png, bounding box (80,112) -> (159,127)) ──
namespace Blocks {
    constexpr int Y = 112;
    constexpr int W = 16;
    constexpr int H = 16;

    // 3-frame Question Block animation: X=80, 96, 112, Y=112 (16×16)
    inline const sf::IntRect QUESTION1({80,  Y}, {W, H});
    inline const sf::IntRect QUESTION2({96,  Y}, {W, H});
    inline const sf::IntRect QUESTION3({112, Y}, {W, H});

    // Empty / Hit Used Block after item emerges: X=128, 144, Y=112 (16×16)
    inline const sf::IntRect EMPTY    ({128, Y}, {W, H});
    inline const sf::IntRect EMPTY2   ({144, Y}, {W, H});
    inline const sf::IntRect BRICK    ({272, Y}, {W, H});

    inline const std::vector<sf::IntRect>& questionBlockFrames() {
        static const std::vector<sf::IntRect> frames = {QUESTION1, QUESTION2, QUESTION3};
        return frames;
    }
} // namespace Blocks

// ── Backgrounds (from bg_mountains.png, bounding box (0,40) -> (767,215)) ──
namespace Backgrounds {
    inline constexpr std::string_view MOUNTAINS_PATH = "assets/textures/ui/bg_mountains.png";
    inline const sf::IntRect OVERWORLD({0, 40}, {768, 176});
    inline const sf::IntRect MOUNTAIN_BIG({0, 160}, {80, 50});
    inline const sf::IntRect MOUNTAIN_SMALL({250, 160}, {64, 50});
} // namespace Backgrounds

} // namespace SpriteFrames

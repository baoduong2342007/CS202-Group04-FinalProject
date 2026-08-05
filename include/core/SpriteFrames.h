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
    // Row Y=8, each frame 16×16
    constexpr int Y = 8;
    constexpr int W = 16;
    constexpr int H = 16;

    inline const sf::IntRect IDLE  ({0,   Y}, {W, H});
    inline const sf::IntRect WALK1 ({20,  Y}, {W, H});
    inline const sf::IntRect WALK2 ({38,  Y}, {W, H});
    inline const sf::IntRect WALK3 ({56,  Y}, {W, H});
    inline const sf::IntRect SKID  ({76,  Y}, {W, H});
    inline const sf::IntRect JUMP  ({96,  Y}, {W, H});
    inline const sf::IntRect DEATH ({116, Y}, {W, H});

    inline const std::vector<sf::IntRect>& walkFrames() {
        static const std::vector<sf::IntRect> frames = {WALK1, WALK2, WALK3};
        return frames;
    }
} // namespace SmallMario

namespace BigMario {
    // Row Y=32, each frame 16×32
    constexpr int Y = 32;
    constexpr int W = 16;
    constexpr int H = 32;

    inline const sf::IntRect IDLE  ({0,   Y}, {W, H});
    inline const sf::IntRect WALK1 ({20,  Y}, {W, H});
    inline const sf::IntRect WALK2 ({38,  Y}, {W, H});
    inline const sf::IntRect WALK3 ({56,  Y}, {W, H});
    inline const sf::IntRect SKID  ({76,  Y}, {W, H});
    inline const sf::IntRect JUMP  ({96,  Y}, {W, H});
    // Crouch starts 8px lower, only 24px tall — but we use 16×32 bounding with top padding
    inline const sf::IntRect CROUCH({116, 40}, {W, 24});

    inline const std::vector<sf::IntRect>& walkFrames() {
        static const std::vector<sf::IntRect> frames = {WALK1, WALK2, WALK3};
        return frames;
    }
} // namespace BigMario

namespace BigLuigi {
    // Row Y=32, right half (X >= 136), each frame 16×32
    constexpr int Y = 32;
    constexpr int W = 16;
    constexpr int H = 32;

    inline const sf::IntRect IDLE  ({136, Y}, {W, H});
    inline const sf::IntRect WALK1 ({156, Y}, {W, H});
    inline const sf::IntRect WALK2 ({174, Y}, {W, H});
    inline const sf::IntRect WALK3 ({194, Y}, {W, H});
    inline const sf::IntRect SKID  ({210, Y}, {W, H});
    inline const sf::IntRect JUMP  ({230, Y}, {W, H});
    inline const sf::IntRect CROUCH({246, 40}, {W, 24});

    inline const std::vector<sf::IntRect>& walkFrames() {
        static const std::vector<sf::IntRect> frames = {WALK1, WALK2, WALK3};
        return frames;
    }
} // namespace BigLuigi

namespace FireSmallMario {
    // Row Y=116, each frame 16×16 (same layout as SmallMario but fire palette)
    constexpr int Y = 116;
    constexpr int W = 16;
    constexpr int H = 16;

    inline const sf::IntRect IDLE  ({0,   Y}, {W, H});
    inline const sf::IntRect WALK1 ({20,  Y}, {W, H});
    inline const sf::IntRect WALK2 ({38,  Y}, {W, H});
    inline const sf::IntRect WALK3 ({56,  Y}, {W, H});
    inline const sf::IntRect SKID  ({76,  Y}, {W, H});
    inline const sf::IntRect JUMP  ({96,  Y}, {W, H});
    inline const sf::IntRect DEATH ({116, Y}, {W, H});

    inline const std::vector<sf::IntRect>& walkFrames() {
        static const std::vector<sf::IntRect> frames = {WALK1, WALK2, WALK3};
        return frames;
    }
} // namespace FireSmallMario

namespace FireBigMario {
    // Row Y=140, each frame 16×32
    constexpr int Y = 140;
    constexpr int W = 16;
    constexpr int H = 32;

    inline const sf::IntRect IDLE  ({0,   Y}, {W, H});
    inline const sf::IntRect WALK1 ({20,  Y}, {W, H});
    inline const sf::IntRect WALK2 ({38,  Y}, {W, H});
    inline const sf::IntRect WALK3 ({56,  Y}, {W, H});
    inline const sf::IntRect SKID  ({76,  Y}, {W, H});
    inline const sf::IntRect JUMP  ({96,  Y}, {W, H});
    inline const sf::IntRect CROUCH({116, 148}, {W, 24});

    inline const std::vector<sf::IntRect>& walkFrames() {
        static const std::vector<sf::IntRect> frames = {WALK1, WALK2, WALK3};
        return frames;
    }
} // namespace FireBigMario

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

    // Coin — 4-frame animation from items_objects.png (180, 8) to (249, 23)
    inline const sf::IntRect COIN1({180, Y}, {W, H});
    inline const sf::IntRect COIN2({198, Y}, {W, H});
    inline const sf::IntRect COIN3({216, Y}, {W, H});
    inline const sf::IntRect COIN4({234, Y}, {W, H});

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

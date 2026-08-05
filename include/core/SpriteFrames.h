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
    // Overworld Objects row Y=8, each 16×16
    constexpr int Y = 8;
    constexpr int W = 16;
    constexpr int H = 16;

    // Mushroom (Super — red) — first sprite
    inline const sf::IntRect MUSHROOM({0, Y}, {W, H});

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

    // Coin — animated 3 frames from tileset.png: (298,95) to (332,95) with 1px gap
    inline const sf::IntRect COIN1({298, 95}, {W, H});
    inline const sf::IntRect COIN2({315, 95}, {W, H});
    inline const sf::IntRect COIN3({332, 95}, {W, H});

    inline const std::vector<sf::IntRect>& coinFrames() {
        static const std::vector<sf::IntRect> frames = {COIN1, COIN2, COIN3};
        return frames;
    }
} // namespace Items

// ── Blocks (from items_blocks.png) ─────────────────────────

namespace Blocks {
    constexpr int W = 16;
    constexpr int H = 16;

    // Question block — 3-frame animation from items_blocks.png: (80,112) to (112,112)
    inline const sf::IntRect QUESTION1({80,  112}, {W, H});
    inline const sf::IntRect QUESTION2({96,  112}, {W, H});
    inline const sf::IntRect QUESTION3({112, 112}, {W, H});

    inline const std::vector<sf::IntRect>& questionBlockFrames() {
        static const std::vector<sf::IntRect> frames = {
            QUESTION1, QUESTION2, QUESTION3
        };
        return frames;
    }
} // namespace Blocks

// ── Tiles (from tileset.png) ──────────────────────────────

namespace Tiles {
    constexpr int W = 16;
    constexpr int H = 16;

    // Ground block — (0,16) to (15,31)
    inline const sf::IntRect GROUND({0, 16}, {W, H});

    // Brick block — (17,16) to (32,31)
    inline const sf::IntRect BRICK({17, 16}, {W, H});

    // Question block (static tile from tileset.png) — (34,16) to (49,31)
    inline const sf::IntRect QUESTION({34, 16}, {W, H});
} // namespace Tiles

} // namespace SpriteFrames

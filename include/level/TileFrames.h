/**
 * @file TileFrames.h
 * @author TV4 (Vy)
 * @brief Tile sprite frame texture rect definitions
 * @note Sprint 6 - consolidated tileset texture coordinates for level rendering
 */

#pragma once

#include <SFML/Graphics/Rect.hpp>

namespace TileFrames {

inline constexpr int SOURCE_TILE_SIZE = 16;

// Basic blocks — Palette 1, Overworld.
// These four cells are the actual terrain quartet at the start of the source
// sheet.  The old QUESTION/USED definitions accidentally pointed into this
// group; the animated question/used cells belong to Palette 3 below.
inline constexpr sf::IntRect GROUND{ {0, 16}, {16, 16} };
inline constexpr sf::IntRect BRICK{ {17, 16}, {16, 16} };
inline constexpr sf::IntRect STONE{ {0, 33}, {16, 16} };
inline constexpr sf::IntRect HARD_BLOCK{ {349, 78}, {16, 16} };

// Theme variations — Palette 1 uses theme-specific terrain slots.  The
// Underground row has two brick patterns followed by the plain cyan solid
// block used by Level 2's `S` symbols.
inline constexpr sf::IntRect GROUND_UNDERGROUND{ {147, 16}, {16, 16} };
inline constexpr sf::IntRect BRICK_UNDERGROUND{ {164, 16}, {16, 16} };
inline constexpr sf::IntRect BRICK_VARIANT_UNDERGROUND{ {181, 16}, {16, 16} };
inline constexpr sf::IntRect STONE_UNDERGROUND{ {147, 33}, {16, 16} };
inline constexpr sf::IntRect HARD_BLOCK_UNDERGROUND{ {445, 78}, {16, 16} };

inline constexpr sf::IntRect GROUND_CASTLE{ {0, 151}, {16, 16} };
inline constexpr sf::IntRect BRICK_CASTLE{ {17, 100}, {16, 16} };
inline constexpr sf::IntRect STONE_CASTLE{ {0, 151}, {16, 16} };
inline constexpr sf::IntRect HARD_BLOCK_CASTLE{ {541, 78}, {16, 16} };
inline constexpr sf::IntRect BRIDGE_CHAIN_CASTLE{ {0, 134}, {16, 16} };

inline constexpr sf::IntRect GROUND_UNDERWATER{ {147, 134}, {16, 16} };
inline constexpr sf::IntRect BRICK_UNDERWATER{ {164, 100}, {16, 16} };
inline constexpr sf::IntRect STONE_UNDERWATER{ {215, 297}, {16, 16} };
inline constexpr sf::IntRect HARD_BLOCK_UNDERWATER{ {637, 78}, {16, 16} };

// Palette 3 — animated question and used blocks.  The game currently uses
// items_blocks.png for the QuestionBlock entity, but these rects are the
// canonical tileset coordinates for map/object tooling and future loaders.
inline constexpr sf::IntRect QUESTION{ {298, 78}, {16, 16} };
inline constexpr sf::IntRect QUESTION_UNDERGROUND{ {394, 78}, {16, 16} };
inline constexpr sf::IntRect QUESTION_CASTLE{ {490, 78}, {16, 16} };
inline constexpr sf::IntRect QUESTION_UNDERWATER{ {586, 78}, {16, 16} };

inline constexpr sf::IntRect USED_BLOCK{ {349, 78}, {16, 16} };
inline constexpr sf::IntRect USED_BLOCK_UNDERGROUND{ {445, 78}, {16, 16} };
inline constexpr sf::IntRect USED_BLOCK_CASTLE{ {541, 78}, {16, 16} };
inline constexpr sf::IntRect USED_BLOCK_UNDERWATER{ {637, 78}, {16, 16} };

// Palette 3 — ordinary map coin.  The three horizontal frames are the
// regular coin animation; they are not the coin that pops out of a ? block.
inline constexpr sf::IntRect COIN_OVERWORLD{ {298, 95}, {16, 16} };
inline constexpr sf::IntRect COIN_OVERWORLD_SIDE{ {315, 95}, {16, 16} };
inline constexpr sf::IntRect COIN_OVERWORLD_THIN{ {332, 95}, {16, 16} };

inline constexpr sf::IntRect COIN_UNDERGROUND{ {394, 95}, {16, 16} };
inline constexpr sf::IntRect COIN_UNDERGROUND_SIDE{ {411, 95}, {16, 16} };
inline constexpr sf::IntRect COIN_UNDERGROUND_THIN{ {428, 95}, {16, 16} };

inline constexpr sf::IntRect COIN_CASTLE{ {490, 95}, {16, 16} };
inline constexpr sf::IntRect COIN_CASTLE_SIDE{ {507, 95}, {16, 16} };
inline constexpr sf::IntRect COIN_CASTLE_THIN{ {524, 95}, {16, 16} };

inline constexpr sf::IntRect COIN_UNDERWATER{ {586, 95}, {16, 16} };
inline constexpr sf::IntRect COIN_UNDERWATER_SIDE{ {603, 95}, {16, 16} };
inline constexpr sf::IntRect COIN_UNDERWATER_THIN{ {620, 95}, {16, 16} };

// Castle component tiles used by future structure builders.
inline constexpr sf::IntRect CASTLE_WALL{ {17, 100}, {16, 16} };
inline constexpr sf::IntRect CASTLE_WINDOW{ {85, 100}, {16, 16} };
inline constexpr sf::IntRect CASTLE_DOOR_TOP{ {119, 100}, {16, 16} };
inline constexpr sf::IntRect CASTLE_DOOR_BOTTOM{ {119, 117}, {16, 16} };

// Palette 0 — 2-wide pipe and finish-pole components.  The lavender/blue
// pixels around these cells are sheet background and must be chroma-keyed.
inline constexpr sf::IntRect PIPE_TOP_LEFT{ {119, 196}, {16, 16} };
inline constexpr sf::IntRect PIPE_TOP_RIGHT{ {136, 196}, {16, 16} };
inline constexpr sf::IntRect PIPE_BODY_LEFT{ {119, 213}, {16, 16} };
inline constexpr sf::IntRect PIPE_BODY_RIGHT{ {136, 213}, {16, 16} };
inline constexpr sf::IntRect HORIZONTAL_PIPE{ {328, 298}, {48, 32} };
inline constexpr sf::IntRect FINISH_FLAG{ {92, 90}, {16, 16} };
inline constexpr sf::IntRect FINISH_POLE{ {136, 247}, {16, 16} };
inline constexpr sf::IntRect FINISH_TOP{ {136, 230}, {16, 16} };
inline constexpr sf::IntRect CASTLE{ {328, 196}, {80, 80} };

inline constexpr sf::IntRect PIPE_TOP_LEFT_UNDERGROUND{ {283, 196}, {16, 16} };
inline constexpr sf::IntRect PIPE_TOP_RIGHT_UNDERGROUND{ {300, 196}, {16, 16} };
inline constexpr sf::IntRect PIPE_BODY_LEFT_UNDERGROUND{ {283, 213}, {16, 16} };
inline constexpr sf::IntRect PIPE_BODY_RIGHT_UNDERGROUND{ {300, 213}, {16, 16} };
inline constexpr sf::IntRect HORIZONTAL_PIPE_UNDERGROUND{ {387, 298}, {48, 32} };
inline constexpr sf::IntRect FINISH_FLAG_UNDERGROUND{ {92, 152}, {16, 16} };
inline constexpr sf::IntRect FINISH_POLE_UNDERGROUND{ {300, 247}, {16, 16} };
inline constexpr sf::IntRect FINISH_TOP_UNDERGROUND{ {300, 230}, {16, 16} };
inline constexpr sf::IntRect CASTLE_UNDERGROUND{ {418, 196}, {80, 80} };

inline constexpr sf::IntRect PIPE_TOP_LEFT_CASTLE{ {119, 280}, {16, 16} };
inline constexpr sf::IntRect PIPE_TOP_RIGHT_CASTLE{ {136, 280}, {16, 16} };
inline constexpr sf::IntRect PIPE_BODY_LEFT_CASTLE{ {119, 297}, {16, 16} };
inline constexpr sf::IntRect PIPE_BODY_RIGHT_CASTLE{ {136, 297}, {16, 16} };
inline constexpr sf::IntRect HORIZONTAL_PIPE_CASTLE{ {446, 298}, {48, 32} };
inline constexpr sf::IntRect FINISH_FLAG_CASTLE{ {92, 260}, {16, 16} };
inline constexpr sf::IntRect FINISH_POLE_CASTLE{ {136, 331}, {16, 16} };
inline constexpr sf::IntRect FINISH_TOP_CASTLE{ {136, 314}, {16, 16} };
inline constexpr sf::IntRect CASTLE_CASTLE{ {508, 196}, {80, 80} };

inline constexpr sf::IntRect PIPE_TOP_LEFT_UNDERWATER{ {283, 280}, {16, 16} };
inline constexpr sf::IntRect PIPE_TOP_RIGHT_UNDERWATER{ {300, 280}, {16, 16} };
inline constexpr sf::IntRect PIPE_BODY_LEFT_UNDERWATER{ {283, 297}, {16, 16} };
inline constexpr sf::IntRect PIPE_BODY_RIGHT_UNDERWATER{ {300, 297}, {16, 16} };
inline constexpr sf::IntRect HORIZONTAL_PIPE_UNDERWATER{ {507, 298}, {48, 32} };
inline constexpr sf::IntRect FINISH_FLAG_UNDERWATER{ {92, 368}, {16, 16} };
inline constexpr sf::IntRect FINISH_POLE_UNDERWATER{ {300, 331}, {16, 16} };
inline constexpr sf::IntRect FINISH_TOP_UNDERWATER{ {300, 314}, {16, 16} };
inline constexpr sf::IntRect CASTLE_UNDERWATER{ {599, 196}, {80, 80} };

// Liquid surface / body
inline constexpr sf::IntRect WATER_SURFACE{ {349, 16}, {16, 16} };
inline constexpr sf::IntRect WATER_BODY{ {349, 33}, {16, 16} };

inline constexpr sf::IntRect WATER_SURFACE_UNDERGROUND{ {445, 16}, {16, 16} };
inline constexpr sf::IntRect WATER_BODY_UNDERGROUND{ {445, 33}, {16, 16} };

inline constexpr sf::IntRect WATER_SURFACE_CASTLE{ {541, 16}, {16, 16} };
inline constexpr sf::IntRect WATER_BODY_CASTLE{ {541, 33}, {16, 16} };

inline constexpr sf::IntRect WATER_SURFACE_UNDERWATER{ {637, 16}, {16, 16} };
inline constexpr sf::IntRect WATER_BODY_UNDERWATER{ {637, 33}, {16, 16} };

} // namespace TileFrames

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

// Basic Blocks (Overworld)
inline constexpr sf::IntRect GROUND{ {0, 16}, {16, 16} };
inline constexpr sf::IntRect STONE{ {34, 16}, {16, 16} };
inline constexpr sf::IntRect BRICK{ {17, 16}, {16, 16} };
inline constexpr sf::IntRect QUESTION{ {85, 16}, {16, 16} };
inline constexpr sf::IntRect USED_BLOCK{ {51, 16}, {16, 16} };

// Theme Variations
// Underground keeps the same shapes as Overworld, but uses the matching
// blue palette group in the tileset (not the adjacent brown brick frame).
inline constexpr sf::IntRect GROUND_UNDERGROUND{ {147, 16}, {16, 16} };
inline constexpr sf::IntRect GROUND_CASTLE{ {34, 33}, {16, 16} };
inline constexpr sf::IntRect GROUND_UNDERWATER{ {51, 33}, {16, 16} };
inline constexpr sf::IntRect BRICK_UNDERGROUND{ {164, 16}, {16, 16} };
inline constexpr sf::IntRect STONE_UNDERGROUND{ {198, 16}, {16, 16} };
inline constexpr sf::IntRect QUESTION_UNDERGROUND{ {394, 78}, {16, 16} };
inline constexpr sf::IntRect USED_BLOCK_UNDERGROUND{ {198, 16}, {16, 16} };

// Map Coins (Stationary)
inline constexpr sf::IntRect COIN_OVERWORLD{ {524, 78}, {16, 16} };
inline constexpr sf::IntRect COIN_UNDERGROUND{ {586, 95}, {16, 16} };

// Castle specific
inline constexpr sf::IntRect CASTLE_WALL{ {34, 33}, {16, 16} };
inline constexpr sf::IntRect CASTLE_WINDOW{ {102, 16}, {16, 16} };
inline constexpr sf::IntRect CASTLE_DOOR_TOP{ {119, 16}, {16, 16} };
inline constexpr sf::IntRect CASTLE_DOOR_BOTTOM{ {266, 16}, {13, 16} };

// Current Pipes & Finish Pole (Preserved)
inline constexpr sf::IntRect PIPE_TOP_LEFT{ {119, 196}, {16, 16} };
inline constexpr sf::IntRect PIPE_TOP_RIGHT{ {136, 196}, {16, 16} };
inline constexpr sf::IntRect PIPE_BODY_LEFT{ {119, 213}, {16, 16} };
inline constexpr sf::IntRect PIPE_BODY_RIGHT{ {136, 213}, {16, 16} };
inline constexpr sf::IntRect FINISH_TOP{ {136, 230}, {16, 16} };
inline constexpr sf::IntRect FINISH_POLE{ {136, 247}, {16, 16} };

// Same pipe and pole geometry in the darker Underground palette group.
inline constexpr sf::IntRect PIPE_TOP_LEFT_UNDERGROUND{ {283, 196}, {16, 16} };
inline constexpr sf::IntRect PIPE_TOP_RIGHT_UNDERGROUND{ {300, 196}, {16, 16} };
inline constexpr sf::IntRect PIPE_BODY_LEFT_UNDERGROUND{ {283, 213}, {16, 16} };
inline constexpr sf::IntRect PIPE_BODY_RIGHT_UNDERGROUND{ {300, 213}, {16, 16} };
inline constexpr sf::IntRect FINISH_TOP_UNDERGROUND{ {300, 230}, {16, 16} };
inline constexpr sf::IntRect FINISH_POLE_UNDERGROUND{ {300, 247}, {16, 16} };

} // namespace TileFrames

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
inline constexpr sf::IntRect STONE{ {0, 50}, {16, 16} }; // Fixed from {0, 33} (Battlement) -> {0, 50} (Stone Stair)
inline constexpr sf::IntRect BRICK{ {68, 16}, {16, 16} }; // Fixed from {17, 16} (Underground ground) -> {68, 16} (Overworld brick)
inline constexpr sf::IntRect QUESTION{ {85, 16}, {16, 16} }; // Fixed from {298, 78} (Underground question) -> {85, 16} (Overworld question)
inline constexpr sf::IntRect USED_BLOCK{ {215, 67}, {16, 16} }; // Fixed from {349, 78} (Animation frame) -> {215, 67} (Used block)

// Theme Variations
inline constexpr sf::IntRect GROUND_UNDERGROUND{ {17, 16}, {16, 16} };
inline constexpr sf::IntRect GROUND_CASTLE{ {34, 16}, {16, 16} };
inline constexpr sf::IntRect GROUND_UNDERWATER{ {51, 16}, {16, 16} };
inline constexpr sf::IntRect BRICK_UNDERGROUND{ {147, 16}, {16, 16} };
inline constexpr sf::IntRect QUESTION_UNDERGROUND{ {298, 78}, {16, 16} };

// Map Coins (Stationary)
inline constexpr sf::IntRect COIN_OVERWORLD{ {524, 78}, {16, 16} };
inline constexpr sf::IntRect COIN_UNDERGROUND{ {586, 95}, {16, 16} };

// Castle specific
inline constexpr sf::IntRect CASTLE_WALL{ {102, 33}, {16, 16} };
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

} // namespace TileFrames

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

inline constexpr sf::IntRect GROUND{ {0, 16}, {16, 16} };

inline constexpr sf::IntRect STONE{ {0, 33}, {16, 16} };

inline constexpr sf::IntRect BRICK{ {17, 16}, {16, 16} };

inline constexpr sf::IntRect USED_BLOCK{ {349, 78}, {16, 16} };

inline constexpr sf::IntRect QUESTION{ {298, 78}, {16, 16} };

inline constexpr sf::IntRect PIPE_TOP_LEFT{ {119, 196}, {16, 16} };

inline constexpr sf::IntRect PIPE_TOP_RIGHT{ {136, 196}, {16, 16} };

inline constexpr sf::IntRect PIPE_BODY_LEFT{ {119, 213}, {16, 16} };

inline constexpr sf::IntRect PIPE_BODY_RIGHT{ {136, 213}, {16, 16} };

inline constexpr sf::IntRect FINISH_FLAG{ {136, 230}, {16, 16} };

inline constexpr sf::IntRect FINISH_POLE{ {136, 247}, {16, 16} };

inline constexpr sf::IntRect FINISH_POLE_TOP{ {136, 230}, {16, 16} };

inline constexpr sf::IntRect CASTLE{ {328, 196}, {80, 80} };

inline constexpr sf::IntRect HORIZONTAL_PIPE { {328, 298}, {48, 32} };

} // namespace TileFrames

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

inline constexpr sf::IntRect FINISH_TOP{ {136, 230}, {16, 16} };

inline constexpr sf::IntRect FINISH_POLE{ {136, 247}, {16, 16} };

} // namespace TileFrames

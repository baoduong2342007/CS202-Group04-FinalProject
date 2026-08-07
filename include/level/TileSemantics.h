/**
 * @file TileSemantics.h
 * @author TV4 (Vy)
 * @brief Shared gameplay semantics for level tile symbols
 */

#pragma once

namespace TileSemantics {

constexpr bool isBreakable(char symbol) noexcept {
    return symbol == 'B';
}

} // namespace

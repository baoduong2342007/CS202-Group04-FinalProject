/**
 * @file TileCollisionSpans.h
 * @author TV5 (Truyen)
 * @brief Builds horizontal collision spans from a tile grid.
 */

#pragma once

#include <string>
#include <vector>

struct TileCollisionSpan {
    int column;
    int row;
    int length;
};

using TileSolidPredicate = bool (*)(char tile);

std::vector<TileCollisionSpan> buildHorizontalTileCollisionSpans(
    const std::vector<std::string>& grid,
    TileSolidPredicate isSolidTile
);

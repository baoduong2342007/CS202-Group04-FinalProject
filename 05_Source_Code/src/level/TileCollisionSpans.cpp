/**
 * @file TileCollisionSpans.cpp
 * @author TV5 (Truyen)
 * @brief Horizontal tile-collision span builder.
 */

#include "level/TileCollisionSpans.h"

std::vector<TileCollisionSpan> buildHorizontalTileCollisionSpans(
    const std::vector<std::string>& grid,
    TileSolidPredicate isSolidTile
) {
    std::vector<TileCollisionSpan> spans;
    if (!isSolidTile) {
        return spans;
    }

    for (std::size_t row = 0; row < grid.size(); ++row) {
        const std::string& tileRow = grid[row];
        int runStart = -1;

        for (std::size_t column = 0; column <= tileRow.size(); ++column) {
            const bool isSolid = column < tileRow.size() && isSolidTile(tileRow[column]);
            if (isSolid && runStart < 0) {
                runStart = static_cast<int>(column);
            } else if (!isSolid && runStart >= 0) {
                spans.push_back({
                    runStart,
                    static_cast<int>(row),
                    static_cast<int>(column) - runStart
                });
                runStart = -1;
            }
        }
    }

    return spans;
}

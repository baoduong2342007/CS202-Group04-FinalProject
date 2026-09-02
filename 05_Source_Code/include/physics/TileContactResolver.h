/**
 * @file TileContactResolver.h
 * @author TV5 (Truyen)
 * @brief Maps a Box2D contact on a ceiling face back to the touched tile.
 */

#pragma once

struct TileContactCoordinate {
    int column;
    int row;
};

/// The contact point lies on the lower/right edge of a tile. Biasing both
/// coordinates inward gives the tile that owns the contacted ceiling face.
TileContactCoordinate resolveCeilingTileContact(float contactX,
                                                float contactY,
                                                float tileSize);

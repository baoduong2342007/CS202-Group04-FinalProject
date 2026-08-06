/**
 * @file TileContactResolver.cpp
 * @author TV5 (Truyen)
 * @brief Direction-aware Box2D contact-to-tile conversion.
 */

#include "physics/TileContactResolver.h"

#include <cmath>

namespace {
constexpr float CONTACT_EDGE_EPSILON_PIXELS = 0.25f;
}

TileContactCoordinate resolveCeilingTileContact(float contactX,
                                                float contactY,
                                                float tileSize) {
    if (tileSize <= 0.f) {
        return {0, 0};
    }

    return {
        static_cast<int>(std::floor((contactX - CONTACT_EDGE_EPSILON_PIXELS) / tileSize)),
        static_cast<int>(std::floor((contactY - CONTACT_EDGE_EPSILON_PIXELS) / tileSize))
    };
}

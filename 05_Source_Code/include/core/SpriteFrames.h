/**
 * @file SpriteFrames.h
 * @brief Compatibility facade for shared and theme-specific sprite frames.
 *
 * New code should include the smallest relevant header and use:
 *   SpriteFrames::shared
 *   SpriteFrames::ovw
 *   SpriteFrames::udg
 *   SpriteFrames::castle
 *   SpriteFrames::udw
 *
 * The legacy aliases below intentionally preserve the old API while the
 * verified atlas coordinates live in SpriteFrames_shared.h.
 */

#pragma once

#include "core/SpriteFrames_shared.h" // IWYU pragma: export
#include "core/SpriteFrames_ovw.h"    // IWYU pragma: export
#include "core/SpriteFrames_udg.h"    // IWYU pragma: export
#include "core/SpriteFrames_castle.h" // IWYU pragma: export
#include "core/SpriteFrames_udw.h"    // IWYU pragma: export

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#pragma clang diagnostic ignored "-Wunused"
#endif

namespace SpriteFrames {

constexpr bool IS_FACADE_ACTIVE = true;

// Shared frame aliases retained for existing callers.
namespace SmallMario = shared::SmallMario;
namespace SmallLuigi = shared::SmallLuigi;
namespace BigMario = shared::BigMario;
namespace BigLuigi = shared::BigLuigi;
namespace FireSmallMario = shared::FireSmallMario;
namespace FireBigMario = shared::FireBigMario;
namespace GrowShrink = shared::GrowShrink;
namespace FireShooting = shared::FireShooting;

// Legacy catalogs and palette-prefixed names remain available from one place.
namespace Enemies = legacy::Enemies;
namespace Items = legacy::Items;
namespace Tiles = legacy::Tiles;
namespace Blocks = legacy::Blocks;
namespace Backgrounds = legacy::Backgrounds;
namespace LevelEntities = legacy::LevelEntities;

} // namespace SpriteFrames

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

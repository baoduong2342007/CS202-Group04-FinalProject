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

#include "core/SpriteFrames_shared.h"
#include "core/SpriteFrames_ovw.h"
#include "core/SpriteFrames_udg.h"
#include "core/SpriteFrames_castle.h"
#include "core/SpriteFrames_udw.h"

namespace SpriteFrames {

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

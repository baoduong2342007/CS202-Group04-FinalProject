# Repository Structure

This document reflects the current working tree. Files are grouped by
responsibility; generated build directories are intentionally omitted.

## Root

```text
CS202-Group04-FinalProject/
|-- CMakeLists.txt
|-- CMakePresets.json
|-- README.md
|-- FILE_STRUCTURE.md
|-- CODING_RULES.md
|-- THIRD_PARTY_NOTICES.md
|-- assets/
|-- cmake/
|-- docs/
|-- include/
|-- levels/
|-- src/
`-- thirdparty/
```

- `CMakeLists.txt` defines `game_lib`, `SuperMario`, and asset synchronization.
- `CMakePresets.json` defines `mingw-debug` and `mingw-release`.

## Production code

```text
include/
|-- core/       # game loop, catalog, display, save, score, sound, animation, textures
|-- entities/   # Mario, enemies, blocks, FireBall, explosion, shared entity types
|-- items/      # Coin, Mushroom, FireFlower, Star
|-- level/      # Level ownership, TileMap, Camera, tile semantics and frames
|-- patterns/   # commands, InputState, EventBus, Factory Method, observer interfaces
|-- physics/    # Box2D engine, listener, collision manager and tile resolver
|-- states/     # Menu, Play, Pause, GameOver, Win, PvP duel states and Mario State pattern classes
`-- ui/         # HUD, layout anchors and shared menu widget

src/
|-- core/
|-- entities/
|-- items/
|-- level/
|-- patterns/
|-- physics/
|-- states/
|-- ui/
`-- main.cpp
```

Important ownership rules:

- `LevelCatalog.h` is the only release-level graph: Levels 1 through 4. Level
  2 and Level 3 start with `initialTheme = Overworld` before a warp/area
  transition; their `dominantTheme` values are Underground and Underwater.
- `Level` exclusively owns and mutates the Box2D world, Mario, TileMap,
  Camera, and `std::unique_ptr<Entity>` collection. Callers receive only the
  read-only `EntityView`, including `find` and `count` queries.
- `Level::requestFireBallShot()` is the only production projectile request API. There is no `FireBallPool` class.
- `CollisionManager::defeatEnemy()` is the central enemy-defeat transaction.
- `Entity` identity combines broad `EntityType`, `EntitySubtype`, and
  capabilities; `ContactListener` and `CollisionManager` dispatch through a
  typed two-participant `CollisionContext`.
- `EventBus` publishes value-only `GameEvent` values; subscribers retain
  move-only RAII `Subscription` tokens. EventType-only `notify` remains a
  compatibility overload.
- `SoundManager` is the only SFX playback authority for gameplay events.
- `include/core/SoundManifest.def` is the SFX authority consumed for typed
  `SoundId` values by `SoundManager` and for CMake runtime packaging.
- `EntityFactory` is instantiable; its canonical `create(SpawnRequest,
  SpawnContext)` delegates to abstract `EntityCreator`, `EnemyCreator`,
  `ItemCreator`, and `WorldObjectCreator`. Static helpers remain compatibility
  shims.
- Co-op enemy and launcher AI select the nearest eligible player.
- `Game` owns physical-to-logical mouse remapping and centered integer viewport presentation.

## Level data

| File | Classification | Purpose |
|---|---|---|
| `levels/level1.txt` | Runtime | Overworld, world 1-1 |
| `levels/level2.txt` | Runtime | Starts Overworld, then transitions to dominant Underground, world 1-2 |
| `levels/level3.txt` | Runtime | Starts Overworld, then transitions to dominant Underwater, world 1-3 |
| `levels/level4.txt` | Runtime | Castle finale, world 1-4, then Win |
| `levels/elevators.txt` | Config | External elevator moving platform route registry |
| `levels/cheep_cheep.txt` | Config | External Cheep Cheep route and spawn registry |
| `levels/pvp_arena.txt` | Runtime | 2 PLAYER VERSUS duel arena: one screen, center fire flower pedestal flanked by symmetric floating step blocks over a clear duel floor; loaded directly by `PvpPlayState` (not part of `LevelCatalog`) |

Release levels use normal random `?` blocks and explicit `f`, `U`, and `O` placements. A normal `?` resolves once to a Coin (70%), Super Mushroom (15%), or FireFlower (15%); explicit `f` blocks always give a FireFlower while preserving Small Fire versus Super Fire body size.

Map symbols:
- `M`: Mario spawn point
- `G`: Goomba spawn point
- `K`: Koopa spawn point
- `p` / `r`: Piranha Plant pipe & stem
- `c`: Cheep Cheep (Swimming/Flying start marker or standalone spawn)
- `x`: Cheep Cheep (Flying/Swimming route end marker paired with `c`)
- `H`: Cheep Cheep (Jumping leap from bottom)
- `^` & `~`: Elevator moving platform start and end markers
- `J`: Springboard / Trampoline
- `?` / `B` / `C`: Question Block, Brick Block, Coin
- `F` / `|`: Finish Flagpole

PvP-only arena markers (valid only with `TileMap::LayoutMode::PVP_ARENA`):
- `M`: Player one spawn point (exactly one)
- `m`: Player two spawn point (exactly one)
- `W`: Fire flower pedestal (exactly one; `PvpPlayState` spawns the contested flower here)

## Assets

```text
assets/
|-- ASSETS_LIST.md
|-- fonts/
|-- sounds/
|   |-- effects/
|   `-- music/
|-- textures/
|   |-- backgrounds/
|   |-- enemies/
|   |-- items/
|   |-- mario/
|   |-- tiles/
|   `-- ui/
`-- ui/bg_world.png
```

The authoritative path, dimension, and `Runtime`/`Future` classification is [assets/ASSETS_LIST.md](assets/ASSETS_LIST.md).

## Documentation

```text
docs/
|-- class_diagram.md
|-- design_patterns.md
|-- oop_principles_and_design_patterns.md
|-- DEMO_VIDEO_SCRIPT.md
`-- specs/CS202-FinalProject_SuperMario.md
```

## Generated and local-only paths

Directories such as `build-*`, `.cache/`, CMake caches, executables, copied runtime assets, save files (`saves/`), and IDE settings are generated/local state and must not be treated as source files.

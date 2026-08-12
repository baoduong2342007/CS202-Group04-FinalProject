# Repository Structure

This document reflects the Sprint 6 remediation working tree. Files are grouped by responsibility; generated build directories are intentionally omitted.

## Root

```text
CS202-Group04-FinalProject/
|-- CMakeLists.txt
|-- CMakePresets.json
|-- README.md
|-- FILE_STRUCTURE.md
|-- CODING_RULES.md
|-- PLAN.md
|-- Evaluate.md ... Evaluate_v4.md
|-- s6_fix_plan.md ... s6_fix_plan_v4.md
|-- assets/
|-- docs/
|-- include/
|-- levels/
|-- src/
|-- tests/
`-- thirdparty/
```

- `CMakeLists.txt` defines `game_lib`, `SuperMario`, asset synchronization, and 17 CTest executables.
- `CMakePresets.json` defines `mingw-debug`, `mingw-release`, and `mingw-tests`.
- Evaluation and fix-plan versions are audit history. Version 4 is the input to the current remediation.

## Production code

```text
include/
|-- core/       # game loop, catalog, display, save, score, sound, animation, textures
|-- entities/   # Mario, enemies, blocks, FireBall, explosion, shared entity types
|-- items/      # Coin, Mushroom, FireFlower, Star
|-- level/      # Level ownership, TileMap, Camera, tile semantics and frames
|-- patterns/   # commands, InputState, EventBus, Simple Factory, observer interfaces
|-- physics/    # Box2D engine, listener, collision manager and tile resolver
|-- states/     # Menu, Play, Pause, GameOver, Win and Mario State pattern classes
|-- ui/         # HUD, layout anchors and shared menu widget
`-- demo/       # optional demo interface; excluded from the production target

src/
|-- core/
|-- entities/
|-- items/
|-- level/
|-- patterns/
|-- physics/
|-- states/
|-- ui/
|-- demo/       # TV3Demo.cpp; excluded by CMake from game_lib/SuperMario
`-- main.cpp
```

Important Sprint 6 ownership rules:

- `LevelCatalog.h` is the only release-level graph: Levels 1, 2, and 3.
- `Level` owns the Box2D world, Mario, TileMap, Camera, and `std::unique_ptr<Entity>` collection.
- `Level::requestFireBallShot()` is the only production projectile request API. There is no `FireBallPool` class.
- `CollisionManager::defeatEnemy()` is the central enemy-defeat transaction.
- `SoundManager` is the only SFX playback authority for gameplay events.
- `Game` owns physical-to-logical mouse remapping and centered integer viewport presentation.

## Level data

| File | Classification | Purpose |
|---|---|---|
| `levels/level0.txt` | Test | Non-release fixture; New Game cannot load it |
| `levels/level1.txt` | Runtime | Overworld, world 1-1 |
| `levels/level2.txt` | Runtime | Underground, world 1-2 |
| `levels/level3.txt` | Runtime | Castle finale, world 1-3, then Win |
| `levels/level4.txt` | Future/reference | Mechanics fixture excluded from the release catalog |

Release levels use normal random `?` blocks and explicit `f`, `U`, and `O` placements. A normal `?` resolves once to a Coin (70%), Super Mushroom (15%), or FireFlower (15%); explicit `f` blocks always give a FireFlower while preserving Small Fire versus Super Fire body size.

## Automated tests

```text
tests/
|-- InputStateTests.cpp
|-- TileCollisionSpanTests.cpp
|-- MarioPhysicsTests.cpp
|-- PlayStateTests.cpp
|-- GameManagerTests.cpp
|-- EventBusTests.cpp
|-- TV5IntegrationTests.cpp
|-- LevelCatalogTests.cpp
|-- SaveManagerTests.cpp
|-- LevelValidatorTests.cpp
|-- SpringboardTests.cpp
|-- Gate0ContractTests.cpp
|-- SaveSessionTests.cpp
|-- SpriteFramesThemeTests.cpp
|-- CollisionMatrixTests.cpp
|-- FireBallRequestTests.cpp
`-- DisplayCameraUITests.cpp
```

`TestSpawnDeath.cpp` is a standalone diagnostic source and is not registered as a CTest suite.

## Assets

```text
assets/
|-- ASSETS_LIST.md
|-- fonts/
|-- sounds/
|   |-- effects/
|   `-- music/
|-- textures/
|   |-- enemies/
|   |-- items/
|   |-- mario/
|   |-- tiles/
|   `-- ui/
`-- ui/bg_world.png
```

The authoritative path, dimension, and `Runtime`/`Reference`/`Future` classification is [assets/ASSETS_LIST.md](assets/ASSETS_LIST.md). CMake packages runtime assets but documentation reference atlases remain under `docs/assets/reference/`.

## Documentation

```text
docs/
|-- class_diagram.md
|-- design_patterns.md
|-- change_in_develop.md
|-- management/
|   |-- s6_plan.md
|   |-- S6_AUDIT_TRACKER.md
|   |-- S6_BUG_REGISTER.md
|   |-- S6_DEPENDENCY_BOARD.md
|   |-- S6_LOCKED_INTERFACES.md
|   |-- TV1_CHANGES_SUMMARY.md
|   |-- TV4_CHANGES_SUMMARY.md
|   |-- TV5_CHANGES_SUMMARY.md
|   `-- TV5_AUDIO_HUD_ITEM_CHECKLIST.md
|-- testing/TV4_PLAYTHROUGH_LOG.md
|-- specs/
`-- assets/reference/
```

`docs/management/s7_plan.md` is planned work. Sprint 6 acceptance must not rely on it to defer an unfinished Sprint 6 requirement.

## Generated and local-only paths

Directories such as `build-*`, CMake caches, executables, copied runtime assets, save files, IDE settings, and temporary test output are generated/local state and must not be treated as source files.

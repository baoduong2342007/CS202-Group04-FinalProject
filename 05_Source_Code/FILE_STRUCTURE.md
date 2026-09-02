# Source Code Structure

This document describes the source code organization of the Super Mario Bros. project.

## Root

```text
05_Source_Code/
|-- CMakeLists.txt          # Build system (SFML 3.0.0, Box2D 2.4.1 via FetchContent)
|-- FILE_STRUCTURE.md       # This file
|-- README.md               # Build & run instructions
|-- assets/                 # Runtime textures, sounds, fonts
|-- include/                # C++ header files
|-- levels/                 # Level data files
|-- saves/                  # Auto-generated save directory (stores saves/save.txt)
|-- screenshots/            # Auto-generated directory for captured PNG screenshots
|-- src/                    # C++ source implementations
`-- thirdparty/             # Local third-party binaries (SFML auto-downloaded)
```

- `CMakeLists.txt` defines `game_lib`, `SuperMario`, and asset synchronization.

## Production Code

```text
include/
|-- core/       # Game loop, catalog, display, save, score, sound, animation, textures, time utilities
|-- entities/   # Mario, enemies, blocks, FireBall, explosion, shared entity types
|-- items/      # Coin, Mushroom, FireFlower, Star
|-- level/      # Level ownership, TileMap, Camera, tile semantics and frames
|-- patterns/   # Commands, InputState, EventBus, Factory Method, observer interfaces
|-- physics/    # Box2D engine, listener, collision manager and tile resolver
|-- states/     # Menu, LevelSelect, CharacterSelect, Play, Pause, GameOver, Win, Records, PvP duel states
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

### Important Ownership Rules

- `LevelCatalog.h` is the only release-level graph: Levels 1 through 4. Level
  2 and Level 3 start with `initialTheme = Overworld` before a warp/area
  transition; their `dominantTheme` values are Underground and Underwater.
- `Level` exclusively owns and mutates the Box2D world, Mario, TileMap,
  Camera, and `std::unique_ptr<Entity>` collection. Callers receive only the
  read-only `EntityView`, including `find` and `count` queries.
- `Level::requestFireBallShot()` is the only production projectile request API.
- `CollisionManager::defeatEnemy()` is the central enemy-defeat transaction.
- `Entity` identity combines broad `EntityType`, `EntitySubtype`, and
  capabilities; `ContactListener` and `CollisionManager` dispatch through a
  typed two-participant `CollisionContext`.
- `EventBus` publishes value-only `GameEvent` values; subscribers retain
  move-only RAII `Subscription` tokens.
- `SoundManager` is the only SFX playback authority for gameplay events.
- `EntityFactory` is instantiable; its canonical `create(SpawnRequest,
  SpawnContext)` delegates to abstract `EntityCreator`, `EnemyCreator`,
  `ItemCreator`, and `WorldObjectCreator`.
- Co-op enemy and launcher AI select the nearest eligible player.
- `Game` owns physical-to-logical mouse remapping and centered integer viewport presentation.

## Level Data

| File | Classification | Purpose |
|---|---|---|
| `levels/level1.txt` | Runtime | Overworld, world 1-1 |
| `levels/level2.txt` | Runtime | Starts Overworld → dominant Underground, world 1-2 |
| `levels/level3.txt` | Runtime | Starts Overworld → dominant Underwater, world 1-3 |
| `levels/level4.txt` | Runtime | Castle finale, world 1-4, boss Bowser → Win |
| `levels/elevators.txt` | Config | External elevator moving platform route registry |
| `levels/cheep_cheep.txt` | Config | External Cheep Cheep route and spawn registry |
| `levels/pvp_arena.txt` | Runtime | 2-Player Versus duel arena |

### Map Symbols

- `M`: Mario spawn point
- `G`: Goomba, `K`: Koopa
- `p` / `r`: Piranha Plant pipe & stem
- `c` / `x`: Cheep Cheep start/end markers, `H`: Cheep Cheep jumping
- `^` / `~`: Elevator start/end markers
- `J`: Springboard / Trampoline
- `?` / `B` / `C`: Question Block, Brick Block, Coin
- `F` / `|`: Finish Flagpole

## Assets

```text
assets/
|-- ASSETS_LIST.md          # Authoritative asset manifest
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

## Generated Paths (Not Included)

Build directories, CMake caches, executables, and IDE settings are not included in this submission.

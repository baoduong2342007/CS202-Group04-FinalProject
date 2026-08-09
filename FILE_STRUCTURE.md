# Directory & File Structure

> Golden Rule: **each class → 1 pair of `.h` + `.cpp`**, placed in the correct directory according to its function.

---

## Complete Directory Tree

```
SuperMario/
│
├── CMakeLists.txt              ← TV1 (Dương) maintain (SFML configuration)
├── CMakePresets.json           ← TV1 (Dương): Debug/Release/Tests presets
├── README.md
├── FILE_STRUCTURE.md
├── CODING_RULES.md
├── PLAN.md                     ← Sprint 6 plan (current sprint)
├── s6_fix_plan_v2.md           ← Sprint 6 fix plan — round 2 (current working plan)
├── Evaluate_v2.md              ← Sprint 6 re-evaluation — round 2 (current)
├── .gitignore
│
├── thirdparty/                 ← Folder containing external libraries
│   └── SFML/                   ← SFML 3.0.0 (MinGW-w64)
│       ├── bin/                ← Contains DLLs to run the game
│       ├── include/            ← Headers of SFML library
│       └── lib/                ← Library files (.lib / .a)
│
├── docs/                       ← TV1 (Dương) maintain
│   ├── class_diagram.md        ← class diagram (Week 6 + Sprint 6 updates)
│   ├── change_in_develop.md    ← changelog of merged changes (ex: change_in_test_game.md)
│   ├── design_patterns.md      ← design patterns documentation
│   ├── blocks_coordinate.md    ← TV5: block-frame coordinates (analysis)
│   ├── enemies_coordinate.md   ← TV5: enemy-frame coordinates (analysis)
│   ├── items_objects_coordinate.md ← TV5: item-frame coordinates (analysis)
│   ├── tiles_coordinate.md     ← TV5: tileset-frame coordinates (analysis)
│   ├── engineering/            ← module engineering notes
│   │   └── TV5_IMPLEMENTATION_NOTES.md
│   ├── management/             ← Sprint planning and task tracking
│   │   ├── ROLES.md
│   │   ├── WEEKLY_PLAN.md
│   │   ├── s6_plan.md          ← Sprint 6 plan (full task list)
│   │   ├── s7_plan.md          ← Sprint 7 plan (future)
│   │   ├── S6_AUDIT_TRACKER.md ← TV1: Sprint 1–6 deliverable tracker
│   │   ├── S6_BUG_REGISTER.md  ← TV1: P0/P1/P2 bug register
│   │   ├── S6_DEPENDENCY_BOARD.md
│   │   ├── S6_LOCKED_INTERFACES.md
│   │   ├── TV1_CHANGES_SUMMARY.md
│   │   ├── TV4_CHANGES_SUMMARY.md
│   │   ├── TV5_AUDIO_HUD_ITEM_CHECKLIST.md
│   │   └── TV5_INDEPENDENT_TASKS.md
│   └── specs/                  ← Project specifications
│       └── CS202-FinalProject_SuperMario.md
│
├── assets/                     ← TV5 (Truyền) maintain
│   ├── ASSETS_LIST.md          ← chi tiết quy ước cắt frame từ sheet
│   ├── textures/
│   │   ├── mario/
│   │   │   └── MarioLuigi.png  ← SPRITESHEET CHUNG: mọi trạng thái Mario & Luigi
│   │   │                          (SMALL/SUPER/FIRE × idle/walk/jump/death)
│   │   │                          Code dùng setTextureRect() để cắt frame.
│   │   ├── enemies/
│   │   │   ├── goomba.png      ← spritesheet: walk + squish
│   │   │   └── koopa.png       ← spritesheet: walk + shell
│   │   ├── tiles/
│   │   │   └── tileset.png     ← tileset 4 tile (ground/brick/?/flag), dùng texture rect
│   │   ├── items/
│   │   │   ├── items_objects.png ← SPRITESHEET: Coin, FireFlower, Star...
│   │   │   └── items_blocks.png ← SPRITESHEET: Mushroom, block items...
│   │   │                          KHÔNG có file riêng coin.png/mushroom.png/...
│   │   │                          Code dùng setTextureRect() để cắt frame.
│   │   └── ui/
│   │       ├── hud.png
│   │       ├── bg_clouds.png
│   │       ├── bg_mountains.png
│   │       └── bg_trees.png
│   ├── sounds/
│   │   ├── effects/
│   │   │   ├── jump.wav
│   │   │   ├── coin.wav
│   │   │   ├── death.wav
│   │   │   ├── powerup.wav
│   │   │   ├── kickkill.wav
│   │   │   ├── fireball.wav
│   │   │   └── ... (additional sound effects)
│   │   └── music/
│   │       ├── overworld.flac   ← main music (FLAC audio format)
│   │       ├── underground.flac
│   │       ├── gameover.flac
│   │       ├── castle.flac
│   │       └── ... (additional audio tracks)
│   └── fonts/
│       └── mario.ttf           ← pixel font (runtime)

├── docs/assets/reference/      ← TV5: source/reference, không package runtime
│   ├── enemies.png
│   ├── enemies_all_components_atlas_full.png
│   ├── blocks_all_components_atlas_full.png
│   ├── items_objects_all_components_atlas_full.png
│   ├── general_tile.png
│   └── tileset(v2).png
│
├── levels/                     ← TV4 (Vy) maintain
│   ├── level0.txt              ← test fixture (NOT in release catalog)
│   ├── level1.txt              ← World 1-1 (overworld tutorial)
│   ├── level2.txt              ← World 1-2 (underground)
│   └── level3.txt              ← World 1-3 (castle/sky, released)
│
├── saves/                      ← auto-generated at runtime by SaveManager (do not commit)
│   └── save.txt                ← created lazily on first high-score/volume write
│
├── tests/                      ← automated tests (CTest)
│   ├── InputStateTests.cpp
│   ├── TileCollisionSpanTests.cpp
│   ├── MarioPhysicsTests.cpp
│   ├── PlayStateTests.cpp      ← TV1: death contract + Win-decision regression
│   ├── GameManagerTests.cpp
│   ├── EventBusTests.cpp
│   ├── TV5IntegrationTests.cpp
│   ├── LevelCatalogTests.cpp
│   ├── SaveManagerTests.cpp
│   ├── LevelValidatorTests.cpp
│   ├── Gate0ContractTests.cpp  ← TV1: Gate 0 release-contract guards
│   ├── SaveSessionTests.cpp    ← TV1: save restart-session integration
│   └── TestSpawnDeath.cpp      ← local diagnostic (manual/UNTESTED in CTest)
│
├── include/                    ← all .h header files
│   ├── core/
│   │   ├── AnimationSystem.h   ← TV2 (Nhật): sprite animation management
│   │   ├── DisplayConfig.h     ← TV2 (Nhật): window/logical resolution config
│   │   ├── Game.h              ← TV2 (Nhật): main loop, window, delta time
│   │   ├── GameManager.h       ← TV1 (Dương): Singleton, state machine host + SaveManager
│   │   ├── GameProgress.h      ← TV1 (Dương): session progress (score/coin/lives/power)
│   │   ├── LevelCatalog.h      ← TV1 (Dương): centralized release-level catalog (1-based)
│   │   ├── SaveManager.h       ← TV4 (Vy): persistent save (high score/unlock/volumes)
│   │   ├── ScoreRules.h        ← TV5 (Truyền): central score catalog (DefeatCause points)
│   │   ├── SoundManager.h      ← TV5 (Truyền): Singleton audio manager
│   │   ├── SpriteFrames.h      ← TV1/TV5: shared sprite-frame tables
│   │   └── TextureManager.h    ← TV1 (Dương): centralized texture cache
│   │
│   ├── states/                 ← TV1 (Dương) + TV2 (Nhật)
│   │   ├── IGameState.h        ← TV1 (Dương): interface (onEnter/Exit/Pause/Resume)
│   │   ├── IMarioState.h       ← TV3 (Bảo): Mario power-state interface
│   │   ├── SmallMarioState.h   ← TV3 (Bảo)
│   │   ├── SuperMarioState.h   ← TV3 (Bảo)
│   │   ├── FireMarioState.h    ← TV3 (Bảo)
│   │   ├── MenuState.h         ← TV2 (Nhật)
│   │   ├── PlayState.h         ← TV1 (Dương): gameplay controller
│   │   ├── PauseState.h        ← TV2 (Nhật)
│   │   ├── GameOverState.h     ← TV2 (Nhật)
│   │   └── WinState.h          ← TV2 (Nhật)
│   │
│   ├── entities/               ← TV3 (Bảo) (Mario) + TV4 (Vy) (Enemy) + TV1 (Dương) (base)
│   │   ├── Entity.h            ← TV1 (Dương): base class for all physical objects
│   │   ├── Character.h         ← TV1 (Dương): abstract (common base for enemy + mario)
│   │   ├── Mario.h             ← TV3 (Bảo)
│   │   ├── Enemy.h             ← TV4 (Vy): abstract base for enemies
│   │   ├── Goomba.h            ← TV4 (Vy)
│   │   ├── Koopa.h             ← TV4 (Vy)
│   │   ├── FireBall.h          ← TV3 (Bảo)
│   │   ├── FireBallPool.h      ← TV3 (Bảo): projectile pooling
│   │   ├── QuestionBlock.h     ← TV1/TV5: adaptive ?-block entity
│   │   └── BlockDebris.h       ← TV1: brick-breaking debris particle
│   │
│   ├── items/                  ← TV5 (Truyền)
│   │   ├── Item.h              ← abstract base
│   │   ├── Coin.h
│   │   ├── Mushroom.h
│   │   ├── FireFlower.h
│   │   └── Star.h
│   │
│   ├── level/                  ← TV4 (Vy)
│   │   ├── Level.h             ← contains TileMap + entity list
│   │   ├── TileMap.h           ← parse + render tiles (+ validator)
│   │   ├── TileCollisionSpans.h← TV4: merged static-tile collision spans
│   │   ├── TileSemantics.h     ← TV4: tile symbol semantics (breakable/coin/...)
│   │   ├── TileFrames.h        ← TV4: per-symbol tile frame lookup
│   │   └── Camera.h            ← TV2 (Nhật): sf::View follow player
│   │
│   ├── patterns/               ← TV1 (Dương) + TV5 (Truyền)
│   │   ├── EntityFactory.h     ← TV1 (Dương): Factory pattern
│   │   ├── EventBus.h          ← TV1 (Dương): global event bus (Singleton)
│   │   ├── EventType.h         ← TV1 (Dương): event type definitions
│   │   ├── ICommand.h          ← TV5 (Truyền): Command interface
│   │   ├── IObserver.h         ← TV1 (Dương): Observer interface
│   │   ├── ISubject.h          ← TV1 (Dương)
│   │   ├── InputHandler.h      ← TV5 (Truyền): map key → command
│   │   ├── InputState.h        ← TV5 (Truyền): Pressed/Held/Released trigger states
│   │   ├── JumpCommand.h       ← concrete Command
│   │   ├── JumpReleaseCommand.h← TV5 (Truyền): jump-release trigger
│   │   ├── MoveLeftCommand.h   ← concrete Command
│   │   ├── MoveRightCommand.h  ← concrete Command
│   │   ├── RunCommand.h        ← TV5 (Truyền): run modifier
│   │   ├── ShootCommand.h      ← TV5 (Truyền): FireBall shoot trigger
│   │   └── PauseCommand.h      ← concrete Command
│   │
│   ├── physics/                ← TV3 (Bảo)
│   │   ├── CollisionManager.h  ← TV3: central gameplay collision handler
│   │   ├── ContactListener.h   ← Box2D contact callback listener
│   │   ├── PhysicsEngine.h
│   │   └── TileContactResolver.h ← TV3: tile-vs-entity contact resolution
│   │
│   └── ui/                     ← TV5 (Truyền)
│       ├── HUD.h
│       ├── UILayoutHelper.h    ← TV2 (Nhật): shared anchor/layout helper
│       └── UIMenuWidget.h      ← TV2 (Nhật): keyboard/mouse menu widget
│
└── src/                        ← all .cpp source files, mirroring include/ structure
    ├── main.cpp                ← entry point (SuperMario executable)
    ├── core/
    │   ├── AnimationSystem.cpp
    │   ├── Game.cpp
    │   ├── GameManager.cpp     ← singleton state stack + shared SaveManager
    │   ├── SaveManager.cpp
    │   ├── ScoreRules.cpp
    │   ├── SoundManager.cpp
    │   └── TextureManager.cpp
    ├── demo/
    │   └── TV3Demo.cpp         ← physical movement testing demo (excluded from
    │                             production target by CMake — S6-TV1-26)
    ├── states/
    │   ├── SmallMarioState.cpp
    │   ├── SuperMarioState.cpp
    │   ├── FireMarioState.cpp
    │   ├── MenuState.cpp
    │   ├── PlayState.cpp
    │   ├── PauseState.cpp
    │   ├── GameOverState.cpp
    │   └── WinState.cpp
    ├── entities/
    │   ├── Entity.cpp
    │   ├── Character.cpp
    │   ├── Mario.cpp
    │   ├── Enemy.cpp
    │   ├── Goomba.cpp
    │   ├── Koopa.cpp
    │   ├── FireBall.cpp
    │   ├── FireBallPool.cpp
    │   ├── QuestionBlock.cpp
    │   └── BlockDebris.cpp
    ├── items/
    │   ├── Item.cpp
    │   ├── Coin.cpp
    │   ├── Mushroom.cpp
    │   ├── FireFlower.cpp
    │   └── Star.cpp
    ├── level/
    │   ├── Level.cpp
    │   ├── TileMap.cpp
    │   ├── TileCollisionSpans.cpp
    │   └── Camera.cpp
    ├── patterns/
    │   ├── EntityFactory.cpp
    │   ├── EventBus.cpp
    │   ├── InputHandler.cpp
    │   ├── InputState.cpp
    │   ├── JumpCommand.cpp
    │   ├── JumpReleaseCommand.cpp
    │   ├── MoveLeftCommand.cpp
    │   ├── MoveRightCommand.cpp
    │   ├── RunCommand.cpp
    │   ├── ShootCommand.cpp
    │   └── PauseCommand.cpp
    ├── physics/
    │   ├── CollisionManager.cpp
    │   ├── ContactListener.cpp
    │   ├── PhysicsEngine.cpp
    │   └── TileContactResolver.cpp
    └── ui/
        ├── HUD.cpp
        ├── UILayoutHelper.cpp
        └── UIMenuWidget.cpp
```

---

## File Rules

### Header files `.h`

```cpp
// Required at the top of each .h file
#pragma once

// Include order:
// 1. Standard library
#include <string>
#include <vector>

// 2. SFML
#include <SFML/Graphics.hpp>

// 3. Project headers (relative path from include/)
#include "entities/Character.h"
```

### Source files `.cpp`

```cpp
// Required header comment at the top of each .cpp file
/**
 * @file Mario.cpp
 * @author TV3
 * @brief Mario character: movement, jump, power-up state machine
 */

// Include its own header first
#include "entities/Mario.h"

// Followed by other includes
#include "physics/CollisionManager.h"
```

### Asset paths

Assets dùng **spritesheet chung** — KHÔNG tách file riêng. Load sheet 1 lần qua `TextureManager`,
cắt frame bằng `setTextureRect()` hoặc `AnimationSystem`.

Always use relative paths from the executable (since CMake copies assets to the `build/` output folder):

```cpp
// CORRECT — simple texture loading (e.g. tileset)
texture.loadFromFile("assets/textures/tiles/tileset.png");

// CORRECT — load spritesheet chung, cắt frame bằng code
auto& tex = TextureManager::getInstance().getTexture("assets/textures/mario/MarioLuigi.png");
sprite.setTexture(tex);
sprite.setTextureRect(sf::IntRect({FRAME_X, FRAME_Y}, {FRAME_W, FRAME_H}));

// INCORRECT — file riêng KHÔNG TỒN TẠI, sẽ hiện khối magenta
texture.loadFromFile("assets/textures/mario/idle.png");   // ❌ file này không có!
texture.loadFromFile("assets/textures/items/coin.png");   // ❌ file này không có!

// INCORRECT — hardcoded absolute path, will not run on others' machines
texture.loadFromFile("C:/project/SuperMario/assets/textures/mario/MarioLuigi.png");
```

### Level files

```
# Comments start with #, ignored when parsed
# Each line = 1 row of tiles
# Each character = 1 tile; characters are written consecutively without spaces
#
# Characters:
# . = empty (air)
# 1 = ground tile (solid)
# S = ground tile variant
# B = brick block
# ? = question block (contains item)
# C = coin (airborne)
# G = Goomba spawn point
# K = Koopa spawn point
# M = Mario spawn point (exactly 1 per level)
# F = finish flag (end of level)
# | = finish flagpole
# [ ] { } = pipe top/body tiles
```

---

## Naming Conventions Quick Guide

| Type | Convention | Example |
|---|---|---|
| Class | `PascalCase` | `GameManager`, `TileMap` |
| Method | `camelCase` | `update()`, `loadFromFile()` |
| Variable | `camelCase` | `playerScore`, `tileSize` |
| Private member | `m_` prefix | `m_position`, `m_health` |
| Constant / enum | `UPPER_SNAKE` | `TILE_SIZE`, `MAX_LIVES` |
| File | class name | `GameManager.h`, `GameManager.cpp` |
| Branch | `feature/...` | `feature/goomba-ai` |

→ Full details: see [`CODING_RULES.md`](CODING_RULES.md)

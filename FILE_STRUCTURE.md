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
├── implementation_plan_sprint5_error.md ← LEGACY Sprint 5 error plan (archived)
├── .gitignore
│
├── thirdparty/                 ← Folder containing external libraries
│   └── SFML/                   ← SFML 3.0.0 (MinGW-w64)
│       ├── bin/                ← Contains DLLs to run the game
│       ├── include/            ← Headers of SFML library
│       └── lib/                ← Library files (.lib / .a)
│
├── docs/                       ← TV1 (Dương) maintain
│   ├── class_diagram.md        ← class diagram (Week 6)
│   ├── PLAN_TV1.md             ← TV1 personal plan
│   ├── management/             ← Sprint planning and task tracking
│   │   ├── ROLES.md
│   │   └── WEEKLY_PLAN.md
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
│   ├── enemies_all_components_atlas.png
│   ├── general_tile.png
│   └── tileset(v2).png
│
├── levels/                     ← TV4 (Vy) maintain
│   ├── level0.txt              ← test fixture (NOT in release catalog)
│   ├── level1.txt
│   ├── level2.txt
│   └── level3.txt              ← (planned for upcoming release)
│
├── saves/                      ← auto-generated, do not commit
│   └── .gitkeep                ← empty file to force Git to track the directory
│
├── tests/                      ← automated tests (CTest)
│   ├── InputStateTests.cpp
│   ├── MarioPhysicsTests.cpp
│   └── TileCollisionSpanTests.cpp
│
├── include/                    ← all .h header files
│   ├── core/
│   │   ├── AnimationSystem.h   ← TV2 (Nhật): sprite animation management
│   │   ├── Game.h              ← TV2 (Nhật): main loop, window, delta time
│   │   ├── GameManager.h       ← TV1 (Dương): Singleton, state machine host
│   │   ├── SoundManager.h      ← TV5 (Truyền): Singleton audio manager
│   │   └── TextureManager.h    ← TV1 (Dương): centralized texture cache
│   │
│   ├── states/                 ← TV1 (Dương) + TV2 (Nhật)
│   │   ├── IGameState.h        ← TV1 (Dương): interface (init/update/render/exit)
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
│   │   └── FireBall.h          ← TV3 (Bảo)
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
│   │   ├── TileMap.h           ← parse + render tiles
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
│   │   ├── JumpCommand.h       ← concrete Command
│   │   ├── MoveLeftCommand.h   ← concrete Command
│   │   ├── MoveRightCommand.h  ← concrete Command
│   │   └── PauseCommand.h      ← concrete Command
│   │
│   ├── physics/                ← TV3 (Bảo)
│   │   ├── CollisionManager.h
│   │   ├── ContactListener.h   ← Box2D contact callback listener
│   │   └── PhysicsEngine.h
│   │
│   └── ui/                     ← TV5 (Truyền)
│       ├── HUD.h
│       └── Button.h            ← (planned for interactive UI menus)
│
└── src/                        ← all .cpp source files, mirroring include/ structure
    ├── core/
    │   ├── AnimationSystem.cpp
    │   ├── Game.cpp
    │   ├── GameManager.cpp
    │   ├── SoundManager.cpp
    │   └── TextureManager.cpp
    ├── demo/
    │   └── TV3Demo.cpp         ← physical movement testing demo
    ├── states/
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
    │   └── FireBall.cpp
    ├── items/
    │   ├── Item.cpp
    │   ├── Coin.cpp
    │   ├── Mushroom.cpp
    │   ├── FireFlower.cpp
    │   └── Star.cpp
    ├── level/
    │   ├── Level.cpp
    │   ├── TileMap.cpp
    │   └── Camera.cpp
    ├── patterns/
    │   ├── EntityFactory.cpp
    │   ├── EventBus.cpp
    │   ├── InputHandler.cpp
    │   ├── JumpCommand.cpp
    │   ├── MoveLeftCommand.cpp
    │   ├── MoveRightCommand.cpp
    │   └── PauseCommand.cpp
    ├── physics/
    │   ├── CollisionManager.cpp
    │   ├── ContactListener.cpp
    │   └── PhysicsEngine.cpp
    ├── ui/
    │   ├── HUD.cpp
    │   └── Button.cpp          ← (planned for interactive UI menus)
    └── main.cpp                ← entry point
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

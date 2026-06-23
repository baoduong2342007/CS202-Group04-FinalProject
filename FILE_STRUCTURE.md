# Directory & File Structure

> Golden Rule: **each class → 1 pair of `.h` + `.cpp`**, placed in the correct directory according to its function.

---

## Complete Directory Tree

```
SuperMario/
│
├── CMakeLists.txt              ← TV1 (Dương) maintain (SFML configuration)
├── README.md
├── ROLES.md
├── WEEKLY_PLAN.md
├── FILE_STRUCTURE.md
├── CODING_RULES.md
├── .gitignore
│
├── thirdparty/                 ← Folder containing external libraries
│   └── SFML/                   ← SFML 3.0.0 (MinGW-w64)
│       ├── bin/                ← Contains DLLs to run the game
│       ├── include/            ← Headers of SFML library
│       └── lib/                ← Library files (.lib / .a)
│
├── docs/                       ← TV1 (Dương) maintain
│   ├── class_diagram.drawio    ← draw.io file for editing
│   ├── class_diagram.png       ← exported PNG for submission
│   └── design_patterns.md      ← description of 5 patterns (TV1 (Dương) writes in Week 5)
│
├── assets/                     ← TV5 (Truyền) maintain
│   ├── textures/
│   │   ├── mario/
│   │   │   ├── idle.png
│   │   │   ├── walk.png        ← sprite sheet (horizontal frames)
│   │   │   ├── jump.png
│   │   │   ├── big_idle.png
│   │   │   ├── big_walk.png
│   │   │   ├── fire_idle.png
│   │   │   └── death.png
│   │   ├── enemies/
│   │   │   ├── goomba.png      ← sprite sheet: walk + squish
│   │   │   └── koopa.png       ← sprite sheet: walk + shell
│   │   ├── tiles/
│   │   │   └── tileset.png     ← single file, using texture rect
│   │   ├── items/
│   │   │   ├── coin.png
│   │   │   ├── mushroom.png
│   │   │   ├── fireflower.png
│   │   │   └── star.png
│   │   └── ui/
│   │       ├── hud_icons.png
│   │       └── menu_bg.png
│   ├── sounds/
│   │   ├── effects/
│   │   │   ├── jump.wav
│   │   │   ├── coin.wav
│   │   │   ├── die.wav
│   │   │   ├── powerup.wav
│   │   │   ├── kick.wav
│   │   │   └── fireball.wav
│   │   └── music/
│   │       ├── overworld.ogg   ← main music (ogg for SFML streaming)
│   │       ├── underground.ogg
│   │       └── gameover.ogg
│   └── fonts/
│       └── mario.ttf           ← or any free pixel font
│
├── levels/                     ← TV4 (Vy) maintain
│   ├── level1.txt
│   ├── level2.txt
│   └── level3.txt
│
├── saves/                      ← auto-generated, do not commit
│   └── .gitkeep                ← empty file to force Git to track the directory
│
├── include/                    ← all .h header files
│   ├── core/
│   │   ├── Game.h              ← TV2 (Nhật): main loop, window, delta time
│   │   ├── GameManager.h       ← TV1 (Dương): Singleton, state machine host
│   │   └── SoundManager.h      ← TV5 (Truyền): Singleton audio manager
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
│   │   ├── IObserver.h         ← TV1 (Dương): Observer interface
│   │   ├── ISubject.h          ← TV1 (Dương)
│   │   ├── EventBus.h          ← TV1 (Dương): global event bus (Singleton)
│   │   ├── ICommand.h          ← TV5 (Truyền): Command interface
│   │   └── InputHandler.h      ← TV5 (Truyền): map key → command
│   │
│   ├── physics/                ← TV3 (Bảo)
│   │   ├── PhysicsEngine.h
│   │   └── CollisionManager.h
│   │
│   └── ui/                     ← TV5 (Truyền)
│       ├── HUD.h
│       └── Button.h
│
└── src/                        ← all .cpp source files, mirroring include/ structure
    ├── core/
    │   ├── Game.cpp
    │   ├── GameManager.cpp
    │   └── SoundManager.cpp
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
    │   └── InputHandler.cpp
    ├── physics/
    │   ├── PhysicsEngine.cpp
    │   └── CollisionManager.cpp
    ├── ui/
    │   ├── HUD.cpp
    │   └── Button.cpp
    └── main.cpp                ← only contains: Game game; game.run(); return 0;
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

Always use relative paths from the executable (since CMake copies assets to the `build/` output folder):

```cpp
// CORRECT
texture.loadFromFile("assets/textures/mario/walk.png");

// INCORRECT — hardcoded absolute path, will not run on others' machines
texture.loadFromFile("C:/project/SuperMario/assets/textures/mario/walk.png");
```

### Level files

```
# Comments start with #, ignored when parsed
# Each line = 1 row of tiles
# Each character separated by a space
#
# Characters:
# 0 = empty (air)
# 1 = ground tile (solid)
# 2 = brick block
# 3 = question block (contains item)
# C = coin (airborne)
# G = Goomba spawn point
# K = Koopa spawn point
# P = Mario spawn point (exactly 1 per level)
# F = finish flag (end of level)
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

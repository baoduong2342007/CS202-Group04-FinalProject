# Cấu trúc thư mục & file

> Rule vàng: **mỗi class → 1 cặp `.h` + `.cpp`**, đặt đúng thư mục theo chức năng.

---

## Cây thư mục đầy đủ

```
SuperMario/
│
├── CMakeLists.txt              ← TV1 maintain (cấu hình SFML)
├── README.md
├── ROLES.md
├── WEEKLY_PLAN.md
├── FILE_STRUCTURE.md
├── CODING_RULES.md
├── .gitignore
│
├── docs/                       ← TV1 maintain
│   ├── class_diagram.drawio    ← file draw.io để edit
│   ├── class_diagram.png       ← export PNG để nộp bài
│   └── design_patterns.md      ← mô tả 5 pattern (TV1 viết tuần 5)
│
├── assets/                     ← TV5 maintain
│   ├── textures/
│   │   ├── mario/
│   │   │   ├── idle.png
│   │   │   ├── walk.png        ← sprite sheet (nhiều frame ngang)
│   │   │   ├── jump.png
│   │   │   ├── big_idle.png
│   │   │   ├── big_walk.png
│   │   │   ├── fire_idle.png
│   │   │   └── death.png
│   │   ├── enemies/
│   │   │   ├── goomba.png      ← sprite sheet: walk + squish
│   │   │   └── koopa.png       ← sprite sheet: walk + shell
│   │   ├── tiles/
│   │   │   └── tileset.png     ← 1 file duy nhất, dùng texture rect
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
│   │       ├── overworld.ogg   ← nhạc chính (ogg vì SFML streaming)
│   │       ├── underground.ogg
│   │       └── gameover.ogg
│   └── fonts/
│       └── mario.ttf           ← hoặc font pixel free bất kỳ
│
├── levels/                     ← TV4 maintain
│   ├── level1.txt
│   ├── level2.txt
│   └── level3.txt
│
├── saves/                      ← auto-generated, không commit
│   └── .gitkeep               ← file rỗng để Git track thư mục
│
├── include/                    ← tất cả file .h
│   ├── core/
│   │   ├── Game.h              ← TV2: main loop, window, delta time
│   │   ├── GameManager.h       ← TV1: Singleton, state machine host
│   │   └── SoundManager.h      ← TV5: Singleton âm thanh
│   │
│   ├── states/                 ← TV1 + TV2
│   │   ├── IGameState.h        ← TV1: interface (init/update/render/exit)
│   │   ├── MenuState.h         ← TV2
│   │   ├── PlayState.h         ← TV1: điều phối gameplay
│   │   ├── PauseState.h        ← TV2
│   │   ├── GameOverState.h     ← TV2
│   │   └── WinState.h          ← TV2
│   │
│   ├── entities/               ← TV3 (Mario) + TV4 (Enemy) + TV1 (base)
│   │   ├── Entity.h            ← TV1: base class mọi object có vật lý
│   │   ├── Character.h         ← TV1: abstract (enemy + mario chung)
│   │   ├── Mario.h             ← TV3
│   │   ├── Enemy.h             ← TV4: abstract base cho enemy
│   │   ├── Goomba.h            ← TV4
│   │   ├── Koopa.h             ← TV4
│   │   └── FireBall.h          ← TV3
│   │
│   ├── items/                  ← TV5
│   │   ├── Item.h              ← abstract base
│   │   ├── Coin.h
│   │   ├── Mushroom.h
│   │   ├── FireFlower.h
│   │   └── Star.h
│   │
│   ├── level/                  ← TV4
│   │   ├── Level.h             ← chứa TileMap + danh sách entity
│   │   ├── TileMap.h           ← parse + render tile
│   │   └── Camera.h            ← TV2: sf::View follow player
│   │
│   ├── patterns/               ← TV1 + TV5
│   │   ├── EntityFactory.h     ← TV1: Factory pattern
│   │   ├── IObserver.h         ← TV1: Observer interface
│   │   ├── ISubject.h          ← TV1
│   │   ├── EventBus.h          ← TV1: global event bus (Singleton)
│   │   ├── ICommand.h          ← TV5: Command interface
│   │   └── InputHandler.h      ← TV5: map key → command
│   │
│   ├── physics/                ← TV3
│   │   ├── PhysicsEngine.h
│   │   └── CollisionManager.h
│   │
│   └── ui/                     ← TV5
│       ├── HUD.h
│       └── Button.h
│
└── src/                        ← tất cả file .cpp, mirror cấu trúc include/
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
    └── main.cpp                ← chỉ có: Game game; game.run(); return 0;
```

---

## Quy tắc về file

### Header file `.h`

```cpp
// Đầu mỗi .h — bắt buộc
#pragma once

// Thứ tự include:
// 1. Standard library
#include <string>
#include <vector>

// 2. SFML
#include <SFML/Graphics.hpp>

// 3. Project headers (relative path từ include/)
#include "entities/Character.h"
```

### Source file `.cpp`

```cpp
// Đầu mỗi .cpp — bắt buộc có comment header
/**
 * @file Mario.cpp
 * @author TV3
 * @brief Mario character: movement, jump, power-up state machine
 */

// Include header của chính nó trước
#include "entities/Mario.h"

// Sau đó là các include khác
#include "physics/CollisionManager.h"
```

### Asset path

Luôn dùng path tương đối từ executable (do CMake copy assets vào `build/`):

```cpp
// ĐÚNG
texture.loadFromFile("assets/textures/mario/walk.png");

// SAI — hard-code absolute path, máy người khác không chạy được
texture.loadFromFile("C:/project/SuperMario/assets/textures/mario/walk.png");
```

### File level

```
# Chú thích bắt đầu bằng #, bị bỏ qua khi parse
# Mỗi dòng = 1 hàng tile
# Mỗi ký tự cách nhau bởi space
#
# Ký tự:
# 0 = empty (air)
# 1 = ground tile (solid)
# 2 = brick block
# 3 = question block (có item bên trong)
# C = coin (trên không)
# G = Goomba spawn point
# K = Koopa spawn point
# P = Mario spawn point (chỉ 1 cái mỗi level)
# F = finish flag (điểm cuối level)
```

---

## Naming convention nhanh

| Loại | Convention | Ví dụ |
|---|---|---|
| Class | `PascalCase` | `GameManager`, `TileMap` |
| Method | `camelCase` | `update()`, `loadFromFile()` |
| Variable | `camelCase` | `playerHealth`, `tileSize` |
| Private member | `m_` prefix | `m_position`, `m_health` |
| Constant / enum | `UPPER_SNAKE` | `TILE_SIZE`, `MAX_LIVES` |
| File | class name | `GameManager.h`, `GameManager.cpp` |
| Branch | `feature/...` | `feature/goomba-ai` |

→ Chi tiết đầy đủ: xem [`CODING_RULES.md`](CODING_RULES.md)

# Weekly Report Content

### General Information
- **Group ID**: 04
- **Group Name**: CS202 - Group 04
- **Project Name**: Super Mario (2D Game in C++17 & SFML 3.0.0)
- **Date range**: 13/07/2026 -> 18/07/2026

---

# Tasks Completed This Week

## There were no tasks completed this week.

> **Reason:** All team members temporarily paused active development during this week to focus on university **midterm examinations**.

---

# AI Usage Declaration

> No AI tools were utilized for code generation or debugging this week, as development was paused for the midterm examination period.

---

# Tasks Planned for Next Week

> **Context:** Due to midterm examinations spanning the past two weeks, the project timeline has shifted from the original 6-week plan. The team has restructured the remaining work into Sprints 4–8 (see updated `WEEKLY_PLAN.md`). With exams now completed, the following tasks correspond to **Sprint 4 — Catch-up & Core Gameplay**.

**25125009 - Trương Quang Bảo Dương (TV1)**
- Implement `EntityFactory.h/.cpp` (Factory pattern) with `createEnemy(EnemyType, sf::Vector2f)` and `createItem(ItemType, sf::Vector2f)` returning polymorphic `Entity*` based on tilemap character codes (`G`, `K`, `C`).
- Connect `EventBus` signals across physics and audio modules for decoupled inter-module communication.
- Conduct code review for all pending pull requests and merge completed branches into `develop`.

**25125033 - Nguyễn Phan Minh Nhật (TV2)**
- Implement `TextureManager.h/.cpp` to load, cache, and retrieve `sf::Texture` objects by string IDs, preventing redundant disk reads.
- Build `AnimationSystem.h/.cpp` to switch sprite sheet frames based on delta time for Mario's animation states (idle, walk, jump).
- Implement `Camera.h/.cpp` (`sf::View`) to track Mario's position with boundary clamping against level limits.

**25125049 - Trần Hồng Vy (TV4)**
- Complete `TileMap` rendering pipeline via `sf::VertexArray` or individual sprites using `TextureManager` and build a full-length `levels/level1.txt` with platform gaps, pits, enemy spawn points, coin placements, and finish flag.
- Start implementing `Goomba.h/.cpp` inheriting from `Enemy`: horizontal patrol AI with velocity reversal on wall collision.

**25125068 - Lê Phi Truyền (TV5)**
- Implement `src/patterns/InputHandler.cpp` and all concrete command `.cpp` files (`JumpCommand`, `MoveLeftCommand`, `MoveRightCommand`, `PauseCommand`) to complete the Command pattern integration with Mario's controller.
- Implement `src/core/SoundManager.cpp` (Singleton): `loadSound()`, `playSound()`, `loadMusic()`, `playMusic()` with `EventBus` subscription for automatic playback of `jump.wav`, `coin.wav` on corresponding game events.
- Implement abstract `Item.h/.cpp` (inheriting from `Entity` with pure virtual `onCollect()`) and concrete `Coin.h/.cpp` with overlap detection, score award, and `COIN_COLLECTED` event publishing via `EventBus`.

**25125079 - Nguyễn Quốc Bảo (TV3)**
- Resolve pending Sprint 2 review items: replace magic numbers in `Mario.cpp` with named constants (`DEFAULT_JUMP_FORCE`, `DEFAULT_MOVE_SPEED`, `DEFAULT_HEALTH`) and ensure `src/main.cpp` remains the 3-line standard.
- Finalize directional collision in `ContactListener.cpp`: top-stomp (`normal.y > 0.7f`) destroys enemy and bounces Mario; lateral/bottom contact damages Mario.
- Refine `PhysicsEngine.cpp`: clamp terminal fall velocity, fix corner-clipping glitches, and prevent AABB tunneling against solid tiles.

---

# Issues

- **Issue 1: Timeline Shift Due to Midterm Examinations**
  Active development was paused during the past two weeks (Sprint 2 partial + Sprint 3 full) due to midterm examinations, causing the project to fall behind the original 6-week plan.
  *How addressed*: The team restructured the remaining work into Sprints 4–8 in an updated `WEEKLY_PLAN.md`. Tasks are modularized using pre-defined contracts (`EventBus`, `ICommand`, `EntityFactory`) to allow high-parallelism development without cross-module blockers.

- **Issue 2: TV3 Branch Pending Review Items**
  `feature/mario-and-physics` was merged into `develop` during Sprint 2, but magic numbers in `Mario.cpp` and `ContactListener.cpp` still need to be replaced with named constants per the team's coding conventions.
  *How addressed*: TV3 will resolve these items as first priority in Sprint 4 before proceeding to new collision work.

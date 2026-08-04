# Revised Sprint Plan — Work Allocation by Member

> **Note (Updated 18/07/2026):** The original 6-week plan assumed uninterrupted development. Due to university midterm examinations (12/07–18/07), the timeline has shifted. This revised plan is based on **actual codebase state** rather than the original weekly schedule. Sprints 1–3 are summarized as completed/paused; Sprints 4–8 contain the remaining work.

> **Weekly Demo Checkpoint:** At the end of each sprint, the team will sit together, run the game, and verify visually — "done but untested" is not accepted.

---

## Sprint 1 (17/06 – 04/07) — Setup & Architecture ✅ COMPLETED

**Demo Checkpoint:** Everyone clones the repo → runs `cmake .. && cmake --build .` → runs executable → sees a **blank black SFML window** without errors.

| | TV1 (Dương - Architect) | TV2 (Nhật - Engine) | TV3 (Bảo - Physics) | TV4 (Vy - Level) | TV5 (Truyền - Sound/UI) |
|---|---|---|---|---|---|
| **Main Task** | Create repo, README, CMakeLists.txt (SFML fetching), allocation | SFML window + game loop skeleton | Design `Entity` and `Character` base classes | Define level `.txt` file format | SoundManager skeleton + collect assets |
| **Details** | Draw draft class diagram on draw.io; select and document 5 design patterns; write CMakeLists.txt; create `develop` branch | Write `Game.h / Game.cpp`: window initialization, game loop with delta time, handle sf::Event; test build on everyone's machines | Write `Entity.h` (position, velocity, sprite); `Character.h` (health, direction, abstract `update()`); no implementation yet | Write level `.txt` spec (save in ROLES.md under TV4 section); create placeholder `levels/level1.txt` | Create `SoundManager.h` (Singleton skeleton); list required assets (Mario sprite sheet, tileset, sounds); find free/placeholder assets |
| **Deliverables** | `README.md`, `ROLES.md`, `WEEKLY_PLAN.md`, class diagram draft | Project compiles, 1280x720 window opens | `Entity.h`, `Character.h` with stub methods | `levels/level1.txt` (placeholder), format spec in doc | `SoundManager.h`, list of acquired assets |
| **Status** | ✅ Done | ✅ Done | ✅ Done | ✅ Done | ✅ Done |

---

## Sprint 2 (06/07 – 11/07) — Core Mechanics (Headers & Partial Implementation) ⚠️ 70% COMPLETED

**Demo Checkpoint:** Run game → see **Mario sprite on screen** → press ←/→ Mario moves → press Space Mario jumps → Mario falls back to the ground without falling through tiles.

| | TV1 (Dương - Architect) | TV2 (Nhật - Engine) | TV3 (Bảo - Physics) | TV4 (Vy - Level) | TV5 (Truyền - Sound/UI) |
|---|---|---|---|---|---|
| **Main Task** | Implement `Observer` / `EventBus` + `EventBus::publish/subscribe` | Refactor `Game.h/.cpp` to follow coding rules | `Mario`: Box2D integration, `PhysicsEngine`, `ContactListener` | `TileMap`: parse `.txt` file, validate grid data | `InputHandler` + Command pattern headers (ICommand, JumpCommand, MoveLeft, MoveRight, Pause) |
| **Details** | Write `IObserver.h`, `ISubject.h`, `EventBus.h` (Singleton); `EventType.h` scoped enum; `EventBus.cpp` with safe copy-on-notify iteration and `EventTypeHash` functor for cross-compiler portability | Replace magic numbers with named constants (`SCREEN_WIDTH`, `FRAMERATE_LIMIT`); use `m_` prefix for private members; change `update(sf::Time)` to `update(float dt)`; wrap debug output with `#ifdef DEBUG` | Integrate Box2D v2.4.1 via CMake FetchContent; build `PhysicsEngine` Singleton with fixed timestep; `ContactListener` for collision detection; `Mario.h/.cpp` skeleton with movement and jump | `TileMap.cpp`: `loadFromFile()` with error handling for missing files, auto-skip comment lines, row width validation; `Enemy.h` abstract base skeleton | Write `ICommand.h` (abstract `execute()`/`undo()` contract); `InputHandler.h` (map `sf::Keyboard::Key` → `ICommand` via `unique_ptr`); concrete command headers for Jump, MoveLeft, MoveRight, Pause |
| **Deliverables** | Functional `EventBus.h/.cpp` with safe iteration ✅ | `Game.h/.cpp` refactored ✅ | Box2D integrated, `PhysicsEngine`+`ContactListener` working ✅ | `TileMap.cpp` parses level files ✅ | Command pattern headers complete ✅ |
| **Still Pending** | — | `TextureManager`, `AnimationSystem`, `Camera` not started | Magic numbers in `Mario.cpp` not replaced; `main.cpp` conflict resolved with workaround | `TileMap` rendering not implemented; `level1.txt` still placeholder-level | All `.cpp` files for commands not written; `SoundManager.cpp` not implemented |

---

## Sprint 3 (12/07 – 18/07) — ⏸️ EXAM BREAK

> All team members paused active development to focus on university **midterm examinations**. No code was pushed during this period.

---

## Sprint 4 (19/07 – 25/07) — Catch-up & Core Gameplay

**Demo Checkpoint:** Run game → see **Mario sprite walking on rendered tiles** → press ←/→ Mario moves with animation → press Space Mario jumps → Mario collides with ground tiles and does not fall through → coins visible on map → jump sound effect plays.

| | TV1 (Dương - Architect) | TV2 (Nhật - Engine) | TV3 (Bảo - Physics) | TV4 (Vy - Level) | TV5 (Truyền - Sound/UI) |
|---|---|---|---|---|---|
| **Main Task** | Implement `EntityFactory` (Factory pattern); code review & integration | `TextureManager`, `AnimationSystem`, `Camera` | Fix Sprint 2 review items; finalize collision logic | Complete `TileMap` rendering; build full Level 1; start `Goomba` | Complete Command `.cpp` files; `SoundManager.cpp`; `Item` + `Coin` |
| **Details** | `EntityFactory.h/.cpp`: `createEnemy(EnemyType, sf::Vector2f)` and `createItem(ItemType, sf::Vector2f)` returning polymorphic `Entity*`; TV4 uses it to spawn entities from level file chars (`G`, `K`, `C`); connect `EventBus` signals across physics/audio modules; review all pending PRs | `TextureManager.h/.cpp`: load, cache `sf::Texture` by string ID, prevent redundant disk reads; `AnimationSystem.h/.cpp`: sprite sheet frame switching based on `float dt` for Mario states (idle, walk, jump); `Camera.h/.cpp`: `sf::View` follows player X-coordinate, clamp to level boundaries | Replace magic numbers in `Mario.cpp` with named constants (`DEFAULT_JUMP_FORCE`, `DEFAULT_MOVE_SPEED`, `DEFAULT_HEALTH`); revert `src/main.cpp` to 3-line standard if still needed; finalize `ContactListener` directional collision (top stomp vs lateral hit); clamp terminal fall velocity | `TileMap` rendering via `sf::VertexArray` or individual sprites using `TextureManager`; build full-length `levels/level1.txt` (>= 3 platform sections, gaps, pits, enemy spawn points `G`/`K`, coin placements `C`, finish flag `F`); start `Goomba.h/.cpp`: patrol AI moving left until wall → reverse direction | `InputHandler.cpp`: implement `bindKey()`, `unbindKey()`, `handleInput()`, `getAction()`; `JumpCommand.cpp`, `MoveLeftCommand.cpp`, `MoveRightCommand.cpp`, `PauseCommand.cpp`: each calls corresponding method on `Character*`; `SoundManager.cpp`: Singleton `getInstance()`, `loadSound(id, path)`, `playSound(id)`, `loadMusic()`, `playMusic()`; subscribe to `EventBus` events (`PLAYER_JUMPED` → `jump.wav`, `COIN_COLLECTED` → `coin.wav`); `Item.h/.cpp`: abstract base inheriting from `Entity`, pure virtual `onCollect(Character*)`; `Coin.h/.cpp`: hide on collect, publish `COIN_COLLECTED` via `EventBus` |
| **Deliverables** | `EntityFactory.h/.cpp` functional; all Sprint 2 branches merged cleanly | Mario sprite visible with walk/jump animation; camera follows player | Physics feels correct; no magic numbers; collision directions work | Level 1 renders correctly; Goomba walks back and forth | `InputHandler` maps keys to actions; `SoundManager` plays sounds on events; `Coin` collectable |
| **Est. Time** | ~6–8h | ~10–12h | ~8–10h | ~10–12h | ~10–12h |

---

## Sprint 5 (26/07 – 01/08) — Level 1 Playable End-to-End

**Demo Checkpoint:** Play from beginning of Level 1 → encounter Goomba → stomp Goomba to kill it or touch it to die → collect coins and mushroom → restart on death → reach the end of the level → transition to next screen.

| | TV1 (Dương - Architect) | TV2 (Nhật - Engine) | TV3 (Bảo - Physics) | TV4 (Vy - Level) | TV5 (Truyền - Sound/UI) |
|---|---|---|---|---|---|
| **Main Task** | `GameState` machine (State pattern); `PlayState` | Death animation, spawn animation, level transition | Collision with enemies: stomp vs side hit; lives system; `FireBall` | `Goomba` fully working; `Koopa` enemy; start Level 2 | `Mushroom`, `FireFlower`, `Star` items; basic sound effects; `HUD` |
| **Details** | `IGameState.h` interface (init/update/render/exit); `PlayState.h/.cpp`: gameplay controller that owns `Level`, `Mario`, processes input and physics each frame; `GameManager.h/.cpp`: Singleton that stores current state and handles `changeState()` transitions; integrate with TV2's screen transitions | `AnimationSystem` adds "death" and "spawn" clips; `Camera` shakes briefly when Mario dies; implement transition fade-to-black (0.5s `sf::RectangleShape` alpha interpolation) when passing level end flag; `MenuState.h/.cpp`: background image + "Press Enter to Start" text + selection sound | `CollisionManager::resolve()`: Mario hits top of enemy → enemy dies, Mario bounces upward; Mario hits side/bottom of enemy → Mario loses a life or downgrades power-up; `LivesManager` tracks remaining lives; `FireBall.h/.cpp`: spawned when Fire Mario presses attack key, moves horizontally, destroys enemies on contact, despawns on wall collision | `Goomba.cpp` complete: patrol, squish death animation, despawn after squish timer; `Koopa.h/.cpp`: patrol mode → stomped → stationary shell → kicked shell slides at high speed, destroys other enemies; Level 2 design (harder: more enemies, higher platforms, fewer coins, introduce Koopa); all enemies spawned via `EntityFactory` | `Mushroom.h/.cpp`: emerges from `?` block, moves horizontally, on collect → publish `POWER_UP` event for TV3 to grow Mario; `FireFlower.h/.cpp`: on collect → Mario enters Fire state; `Star.h/.cpp`: on collect → invincibility timer (flash animation); load 3+ sound effect files (`jump.wav`, `coin.wav`, `die.wav`, `powerup.wav`, `kick.wav`); `HUD.h/.cpp`: render score, lives, coin count using pixel font (`mario.ttf` or Press Start 2P) |
| **Deliverables** | Game state machine works: can transition between states | Death/spawn animations; level transition works | Enemy collision behavior correct; FireBall works | Level 1 playable end-to-end; Koopa works; Level 2 started | Items work correctly; 3+ sound effects active; HUD displays info |
| **Est. Time** | ~8–10h | ~8–10h | ~10–12h | ~10–12h | ~10–12h |

---

## Sprint 6 (02/08 – 08/08) — Full Game Loop (3 Levels, Menus, Power-ups)

**Demo Checkpoint:** From menu → select Start → play 3 consecutive levels with increasing difficulty → Mushroom grows Mario → Game Over / Win screen → return to menu. High score saved to file.

| | TV1 (Dương - Architect) | TV2 (Nhật - Engine) | TV3 (Bảo - Physics) | TV4 (Vy - Level) | TV5 (Truyền - Sound/UI) |
|---|---|---|---|---|---|
| **Main Task** | Complete state machine; `SaveManager` for high scores | GameOver, Win, Pause screens | Power-up system: Small/Big/Fire Mario | Level 2 + Level 3 complete; enemy balancing | Background music; full HUD; volume control |
| **Details** | Complete all state transitions: `MenuState` → `PlayState` → `PauseState` (ESC toggle) → `GameOverState` (0 lives) → `WinState` (beat Level 3); `SaveManager.h/.cpp`: save high score + player name to `saves/highscore.txt`; load and display on menu | `GameOverState.h/.cpp`: display "GAME OVER" + final score + "Press Enter to Restart"; `WinState.h/.cpp`: "Congratulations!" + final score; `PauseState.h/.cpp`: dark semi-transparent overlay + "PAUSED" text + resume/quit options; polish sprite rendering z-ordering | `MarioState` enum class (`SMALL`, `BIG`, `FIRE`); Mushroom collect → grow animation (sprite swap + hitbox resize); hit while Big → shrink back to Small; FireFlower collect → Fire form (palette swap) + ability to shoot `FireBall`; hit while Small → death; smooth transition animations between states | Level 2 design: underground theme, more gaps, introduce pipes, Koopa-heavy; Level 3 design: castle/sky theme, tightest platforms, mixed Goomba+Koopa, time pressure; ensure all 3 levels have `M` spawn, `F` flag, varied coin placement; tweak enemy density for smooth difficulty curve | Background music: load `overworld.ogg` via `sf::Music` streaming, loop during `PlayState`; different track for underground level if available; `HUD` additions: level timer (counts down from 400), current level indicator ("WORLD 1-1"); `SoundManager` volume control (`setSoundVolume`, `setMusicVolume`); bind all remaining SFX to `EventBus` events (`fireball.wav`, `stomp.wav`) |
| **Deliverables** | All 5 game states transition correctly; high scores save/load | Menu + GameOver + Win + Pause screens functional | Power-ups work correctly with visual feedback | Levels 2 & 3 playable; Koopa works in all levels | Background music loops; full HUD with timer; all SFX bound |
| **Est. Time** | ~8–10h | ~8–10h | ~8–10h | ~10–12h | ~8–10h |

---

## Sprint 7 (09/08 – 15/08) — Design Patterns Documentation & Polish

**Demo Checkpoint:** Code review: TV1 reviews the code and shows correct implementation of all 5 patterns. Running game has polished feel, improved enemy AI, and no common glitches.

| | TV1 (Dương - Architect) | TV2 (Nhật - Engine) | TV3 (Bảo - Physics) | TV4 (Vy - Level) | TV5 (Truyền - Sound/UI) |
|---|---|---|---|---|---|
| **Main Task** | Write `docs/design_patterns.md`; full code review | Visual polish: particles, smooth transitions | Physics edge cases; gameplay feel tuning | Improve enemy AI; final level balancing | Full sound integration; save/load high scores |
| **Details** | Review entire codebase; confirm all 5 patterns are correctly implemented: (1) Factory — `EntityFactory`, (2) Singleton — `GameManager`, `SoundManager`, (3) Observer — `EventBus`/`IObserver`, (4) State — `IGameState`/`GameManager`, (5) Command — `ICommand`/`InputHandler`; write detailed `docs/design_patterns.md` explaining each pattern with code references; ensure architecture supports future extension (Multiple Players: just add `Luigi : Character` subclass; Level Editor: `TileMap` already loads `.txt` files) | Add simple particle effects (small squares) when enemy dies; screen shake on Mario hit; smooth sprite direction flipping (no instant pop); ensure consistent 60fps frame rate cap; test rendering on different window sizes; fix any z-ordering bugs (items behind tiles, etc.) | Fix remaining physics bugs: Mario stuck in corners, unintended double jumps, thin wall clipping, falling through moving platforms; tweak `DEFAULT_JUMP_FORCE` and `DEFAULT_MOVE_SPEED` for satisfying game feel; ensure FireBall physics are consistent; test all collision edge cases across 3 levels | Goomba AI improvement: detect Mario within range → increase patrol speed; Koopa shell interactions: shell bounces off walls, destroys Goomba on contact; verify all enemy spawn positions in 3 levels produce fair gameplay; adjust platform gaps to be challenging but not unfair; test enemy patrol paths for all 3 levels | Bind all remaining sound effects to correct `EventBus` events (ensure every action has audio feedback); `SoundManager` supports pausing music when game is paused; menu screen displays saved high score from file; verify audio plays correctly across all 3 levels and state transitions (music changes between levels, stops on pause/gameover) |
| **Deliverables** | `docs/design_patterns.md` complete; architecture review done | Visual quality improved; stable frame rate | Gameplay smooth; no common glitches | Enemy AI + Factory spawning complete; levels balanced | Sound integration complete; high score system works |
| **Est. Time** | ~8–10h | ~6–8h | ~6–8h | ~6–8h | ~6–8h |

> **Extensibility Note:** By this sprint, the architecture naturally supports adding bonus features if time permits:
> - **Multiple Players (Luigi):** Create `Luigi.h/.cpp` inheriting from `Character` with different stats (`LUIGI_JUMP_FORCE = 520.f`, `LUIGI_MOVE_SPEED = 180.f`). Add character selection screen in `MenuState`. Worth 5 bonus points.
> - **Level Editor:** `TileMap` already loads from `.txt` files. A simple editor tool that writes `.txt` is a self-contained addition. Worth 5 bonus points.
> - These can be attempted during Sprint 8 buffer time if the core game is stable.

---

## Sprint 8 (16/08 – 22/08) — Final Testing, Demo & Submission

**Demo Checkpoint:** Play entire game from menu to win screen without crashing. Demo video recorded with voiceover explaining patterns. Documents complete. Clean build on fresh machine succeeds.

| | TV1 (Dương - Architect) | TV2 (Nhật - Engine) | TV3 (Bảo - Physics) | TV4 (Vy - Level) | TV5 (Truyền - Sound/UI) |
|---|---|---|---|---|---|
| **Main Task** | Final class diagram; rubric checklist; compile documentation | Fix render bugs; stabilize frame rate | Gameplay bug fixes; test 3 levels | Level balancing; enemy bug fixes | Record demo video; package project |
| **Details** | Draw official class diagram on draw.io → export `docs/class_diagram.png`; check rubric criteria row-by-row (Functionality 65pts + Design 35pts + Advanced 15pts); compile all project documentation into final format; verify `docs/design_patterns.md` is complete and accurate; final merge `develop` into `main` | Cap frame rate at 60fps; fix screen flickering and z-ordering bugs; test on different monitor sizes and resolutions; ensure window resize is handled gracefully; final render pass cleanup | Play through all 3 levels at least 3 times without crash; log every bug found and fix immediately; test edge cases: rapid key presses, dying at level boundary, collecting item while dying, FireBall at screen edge; ensure zero critical bugs remain | Final tweak of enemy/coin/platform positions for smooth difficulty curve across all 3 levels; test enemy patrol paths don't get stuck on edges; verify Level 1 is accessible for beginners, Level 3 is challenging; confirm all `EntityFactory` spawn codes work | Record demo video using OBS (~5-7 min): gameplay walkthrough of all 3 levels + voiceover explaining how each of the 5 design patterns is used in the code; zip entire project folder; test clean build from scratch on a fresh directory (`cmake .. && cmake --build .`); verify all assets are included and paths are relative |
| **Deliverables** | `docs/class_diagram.png`; rubric checklist verified | Stable release build; no visual bugs | Zero critical bugs across 3 levels | 3 levels balanced and polished | Demo video + packaged project zip |
| **Est. Time** | ~8–10h | ~4–6h | ~6–8h | ~4–6h | ~6–8h |

---

## Sprint Milestones Summary

```
Sprint 1 (17/06–04/07) │ ✅ SFML window opens on all machines
Sprint 2 (06/07–11/07) │ ⚠️ EventBus, TileMap parsing, Box2D integrated (headers done, some .cpp pending)
Sprint 3 (12/07–18/07) │ ⏸️ Midterm exams — no development
Sprint 4 (19/07–25/07) │ ▪ Mario moves & jumps on rendered tiles with sound
Sprint 5 (26/07–01/08) │ ▪ Level 1 playable start-to-finish (with enemies & items)
Sprint 6 (02/08–08/08) │ ▪ 3 levels + menu + game over + power-ups
Sprint 7 (09/08–15/08) │ ▪ 5 design patterns documented + polish + AI
Sprint 8 (16/08–22/08) │ ▪ Project submission (source code + documentation + video)
```

---

## Rubric Coverage

| Rubric Item | Points | Sprint |
|---|---|---|
| Player Inputs, Movement and Collision | 20 | Sprint 4–5 |
| Enemy Behavior | 10 | Sprint 5–6 |
| Power-Ups and Items | 10 | Sprint 5–6 |
| 3 Level Completion | 15 | Sprint 6 |
| Sounds (effect + background) | 10 | Sprint 4–6 |
| Object-Oriented Design | 10 | All sprints |
| 5 Design Patterns | 25 | Sprint 7 (documented) |
| **Subtotal (Core)** | **100** | |
| AI (advanced enemy behavior) | 5 | Sprint 7 |
| Multiple Players (Luigi) | 5 | Sprint 8 (if time) |
| 3D Game | 5 | Not planned |
| **Maximum Possible** | **115** | |

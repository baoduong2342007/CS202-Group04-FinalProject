# 6-Week Plan — Work Allocation by Member

> **Weekly Demo Checkpoint:** At the end of each week, the team will sit together, run the game, and verify visually — "done but untested" is not accepted.

---

## Week 1 — Setup & Architecture

**Demo Checkpoint:** Everyone clones the repo → runs `cmake .. && cmake --build .` → runs executable → sees a **blank black SFML window** without errors.

| | TV1 (Dương - Architect) | TV2 (Nhật - Engine) | TV3 (Bảo - Physics) | TV4 (Vy - Level) | TV5 (Truyền - Sound/UI) |
|---|---|---|---|---|---|
| **Main Task** | Create repo, README, CMakeLists.txt (SFML fetching), allocation | SFML window + game loop skeleton | Design `Entity` and `Character` base classes | Define level `.txt` file format | SoundManager skeleton + collect assets |
| **Details** | Draw draft class diagram on draw.io; select and document 5 design patterns; write CMakeLists.txt; create `develop` branch | Write `Game.h / Game.cpp`: window initialization, game loop with delta time, handle sf::Event; test build on everyone's machines | Write `Entity.h` (position, velocity, sprite); `Character.h` (health, direction, abstract `update()`); no implementation yet | Write level `.txt` spec (save in ROLES.md under TV4 section); create placeholder `levels/level1.txt` | Create `SoundManager.h` (Singleton skeleton); list required assets (Mario sprite sheet, tileset, sounds); find free/placeholder assets |
| **Deliverables** | `README.md`, `ROLES.md`, `WEEKLY_PLAN.md`, class diagram draft | Project compiles, 1280x720 window opens | `Entity.h`, `Character.h` with stub methods | `levels/level1.txt` (placeholder), format spec in doc | `SoundManager.h`, list of acquired assets |
| **Est. Time** | ~5–6h | ~6–8h | ~3–4h | ~2–3h | ~4–5h |

---

## Week 2 — Core Mechanics

**Demo Checkpoint:** Run game → see **Mario sprite on screen** → press ←/→ Mario moves → press Space Mario jumps → Mario falls back to the ground without falling through tiles.

| | TV1 (Dương - Architect) | TV2 (Nhật - Engine) | TV3 (Bảo - Physics) | TV4 (Vy - Level) | TV5 (Truyền - Sound/UI) |
|---|---|---|---|---|---|
| **Main Task** | Implement `Observer` / `EventBus` + `EventBus::publish/subscribe` | `TextureManager`, `AnimationSystem`, `Camera` | `Mario`: movement, jump, gravity; `PhysicsEngine`; collision with ground | `TileMap`: parse `.txt` file → render tiles, load static Level 1 | `InputHandler` + Command pattern (MoveLeft, MoveRight, Jump, Pause) |
| **Details** | Write `IObserver.h`, `ISubject.h`, `EventBus.h` (Singleton); test with dummy event; ensure TV3/4/5 can use it | `TextureManager`: load, cache sf::Texture; `AnimationSystem`: sprite sheet, frame switching over time; `Camera`: sf::View follow player, clamp boundaries | `Mario.cpp`: `handleInput()` → `CommandHandler`; velocity += gravity * dt; AABB collision with ground tiles; resolve overlapping; no enemies yet | `TileMap`: read file, map char → tile type; render using sf::VertexArray or individual sprites; connect with TV2's `Camera` | `ICommand.h`, `InputHandler.h`; map sf::Keyboard::Key → ICommand; expose `getAction(key)` for TV3 |
| **Deliverables** | Functional `EventBus.h/.cpp` with simple tests | Mario sprite visible, walking animation works | Mario moves, jumps, collides with ground | Level 1 renders correctly, camera follows | InputHandler maps keys to actions |
| **Est. Time** | ~5–6h | ~8–10h | ~10–12h | ~8–10h | ~6–7h |

---

## Week 3 — Level 1 Playable

**Demo Checkpoint:** Play from beginning of Level 1 → encounter Goomba → stomp Goomba to kill it or touch it to die → restart → reach the end of the level → transition to next screen.

| | TV1 (Dương - Architect) | TV2 (Nhật - Engine) | TV3 (Bảo - Physics) | TV4 (Vy - Level) | TV5 (Truyền - Sound/UI) |
|---|---|---|---|---|---|
| **Main Task** | Implement `EntityFactory` (Factory pattern); review & merge Week 2 code | Death animation, spawn animation, level transition (screen fade) | Collision with enemies: stomp → enemy dies; side touch → Mario dies | Level 1 complete + `Goomba` AI patrol | `Coin` item, basic sound effects |
| **Details** | `EntityFactory`: `createEnemy(type, pos)`, `createItem(type, pos)` returning `Entity*` polymorphically; TV4 uses it to spawn entities from level file | `AnimationSystem` adds "death", "spawn" clips; `Camera` shakes when Mario dies; transition fade-to-black 0.5s when passing level | `CollisionManager::resolve()`: Mario hits top of enemy → enemy dies; side hit → Mario loses a life; `LivesManager` handles life count | Level 1 fully built (long enough, >= 3 enemy groups, obstacles); `Goomba.cpp`: move left until hit wall → reverse; Goomba dies on stomp | `Coin.h/.cpp` inherits from `Item`; collects on overlap; publishes `COIN_COLLECTED` event; load sound effect files |
| **Deliverables** | `EntityFactory.h/.cpp` functional; clean merge of Week 2 | Death/spawn animations; level transition works | Enemy collision behavior is correct | Level 1 playable end-to-end | Coin works; 3 basic sound effects active |
| **Est. Time** | ~5–6h | ~7–8h | ~8–10h | ~10–12h | ~6–8h |

---

## Week 4 — Full Game Loop (3 levels, Game State, Items)

**Demo Checkpoint:** From menu → select Start → play 3 consecutive levels → Game Over / Win screen → return to menu. Mushroom grows Mario.

| | TV1 (Dương - Architect) | TV2 (Nhật - Engine) | TV3 (Bảo - Physics) | TV4 (Vy - Level) | TV5 (Truyền - Sound/UI) |
|---|---|---|---|---|---|
| **Main Task** | `GameState` machine (State pattern) | Menu screen, GameOver, Win screen | Power-up system: Small/Big/Fire Mario | Level 2 + Level 3; Koopa enemy | FireFlower + Star items; background music; full HUD |
| **Details** | `IGameState` interface (init/update/render/exit); `MenuState`, `PlayState`, `PauseState`, `GameOverState`, `WinState`; `GameManager` stores state stack and transitions | Menu: background + text + select sound; GameOver: display final score; Win: "Congratulations" screen; Pause: dark overlay | `MarioState` enum (Small/Big/Fire); Mushroom collect → grow animation; hit while Big → shrink; FireFlower collect → Fire form + shoot FireBall | Level 2 & 3 designs (harder: more enemies, higher platforms, fewer coins); `Koopa.cpp`: stomped → shell → slide shell when kicked | `Mushroom.h/.cpp` (collect → notify TV3 to grow); `FireFlower.h/.cpp`; `Star.h/.cpp` (invincibility timer); looping background music; HUD: score, lives, level, time |
| **Deliverables** | Game state machine works: menu → play → gameover | Menu + GameOver + Win screens active | Power-ups work correctly | Levels 2 & 3 playable; Koopa works | Items + music + HUD fully functional |
| **Est. Time** | ~8–10h | ~8–10h | ~8–10h | ~10–12h | ~10–12h |

---

## Week 5 — Design Patterns & Polish

**Demo Checkpoint:** Code review: TV1 reviews the code and shows correct implementation of all 5 patterns. Running game has sound effects and saves high scores on exit.

| | TV1 (Dương - Architect) | TV2 (Nhật - Engine) | TV3 (Bảo - Physics) | TV4 (Vy - Level) | TV5 (Truyền - Sound/UI) |
|---|---|---|---|---|---|
| **Main Task** | Code review 5 patterns; write `docs/design_patterns.md`; `SaveManager` | Visual polish: particles, smooth transitions | Physics edge cases; gameplay feel tuning | Improve enemy AI; spawn via Factory | Full sound integration; save/load high scores |
| **Details** | Review entire code, confirm 5 patterns are correct (Factory, Singleton, Observer, State, Command); write doc detailing patterns; write highscore database logic | Add simple particle effects when enemy dies; screen shake on hit; flip sprites smoothly on direction change | Fix bugs: Mario stuck in corners, double jumps, thin wall clipping; tweak jump height and walk speed for better control | AI: Goomba detects Mario in range → increase speed; spawn all enemies via EntityFactory; test all enemy interactions | Bind all sound effects to correct EventBus events; `SoundManager` volume control; menu displays high score from file |
| **Deliverables** | `docs/design_patterns.md` complete; `SaveManager` works | Visual quality improved | Gameplay smooth, no common glitches | Enemy AI + Factory spawning complete | Sound complete; high score saving works |
| **Est. Time** | ~8–10h | ~6–8h | ~6–8h | ~6–8h | ~6–8h |

---

## Week 6 — Polish, Test & Submission

**Demo Checkpoint:** Play entire game from menu to win screen without crashing. Demo video recorded. Documents ready.

| | TV1 (Dương - Architect) | TV2 (Nhật - Engine) | TV3 (Bảo - Physics) | TV4 (Vy - Level) | TV5 (Truyền - Sound/UI) |
|---|---|---|---|---|---|
| **Main Task** | Final class diagram; check rubric; compile documentation | Fix render bugs; stabilize frame rate | Gameplay bug fixes; test 3 levels | Level balancing; enemy bug fixes | Record demo video; package project |
| **Details** | Draw official class diagram (draw.io); check rubric criteria (65+35+15 points); compile all project documentation | Cap frame rate at 60fps; fix screen flickering, z-ordering bugs; test on different monitor sizes | Play through all 3 levels 3 times without crash; log and fix remaining bugs | Tweak enemy/coin/platform positions for smooth difficulty curve; test enemy paths | Record demo video (OBS) with voiceover explaining patterns used; zip project and test clean build |
| **Deliverables** | `docs/class_diagram.png`; rubric checklist | Stable release build | Zero critical bugs | 3 levels balanced | Demo video + packaged project zip |
| **Est. Time** | ~8–10h | ~4–6h | ~6–8h | ~4–6h | ~6–8h |

---

## Weekly Milestones Summary

```
Week 1 │ ▪ SFML window opens on all machines
Week 2 │ ▪ Mario moves & jumps on platforms
Week 3 │ ▪ Level 1 playable start-to-finish (with enemies)
Week 4 │ ▪ 3 levels + menu + game over + items
Week 5 │ ▪ 5 design patterns complete + sounds + saves
Week 6 │ ▪ Project submission (source code + documentation + video)
```

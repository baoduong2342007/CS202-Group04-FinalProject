# List of Features — Super Mario Bros.

**Course**: CS202 — Object-Oriented Programming (APCS)  
**Project**: Super Mario Bros. (C++17 / SFML 3.0.0 / Box2D 2.4.1)  
**Group**: Group 04

---

## Scoring: 0.25 pts / 10 pts for each feature

---

## A. Functionality Features (65 points)

### Player Inputs, Movement and Collision (20 points)

| # | Feature | Status |
|---|---|:---:|
| 1 | Mario/Luigi walk left/right with acceleration and deceleration | ✅ |
| 2 | Sprint mechanic (hold Shift for faster movement) | ✅ |
| 3 | Variable-height jump (tap vs hold jump button) | ✅ |
| 4 | 120ms jump buffering (inputs not dropped near landing) | ✅ |
| 5 | Gravity and terminal fall velocity | ✅ |
| 6 | Collision detection with ground, walls, and ceilings (Box2D) | ✅ |
| 7 | Collision with enemies (stomp to defeat, lateral = damage) | ✅ |
| 8 | Collision with items (auto-collect coins, power-ups) | ✅ |
| 9 | Pipe warp transitions between sub-areas | ✅ |
| 10 | Flagpole end-of-level interaction | ✅ |
| 11 | Zero-friction wall sliding fix | ✅ |
| 12 | Damage grace period (invincibility frames after hit) | ✅ |
| 13 | Mario state transitions: Small → Super → Fire | ✅ |
| 14 | FireBall shooting mechanic (Fire Mario) | ✅ |
| 15 | Lives system with game over | ✅ |
| 16 | Skid friction for direction reversal | ✅ |

### Enemy Behavior (10 points)

| # | Feature | Status |
|---|---|:---:|
| 17 | Goomba — horizontal patrol AI, stomp to defeat | ✅ |
| 18 | Koopa — patrol, shell mechanics (idle → sliding → defeat) | ✅ |
| 19 | Koopa shell can defeat other enemies | ✅ |
| 20 | Red Koopa — ledge-aware patrol (doesn't fall off platforms) | ✅ |
| 21 | Paratroopa — flying/bouncing variant | ✅ |
| 22 | Piranha Plant — pipe emergence AI with proximity sensor | ✅ |
| 23 | Blooper — underwater squid with diagonal swoop AI | ✅ |
| 24 | Cheep Cheep — flying/swimming enemy | ✅ |
| 25 | Spiny — invulnerable to stomps | ✅ |
| 26 | Lakitu — cloud enemy with egg-tossing AI | ✅ |
| 27 | Buzzy Beetle — shell enemy variant | ✅ |
| 28 | HammerBro — multi-tier jumping with hammer throwing | ✅ |
| 29 | Bowser — boss with fire breath, shockwave stun, enrage phase | ✅ |
| 30 | Bullet Bill — directional projectile from launcher cannons | ✅ |
| 31 | Podoboo — lava fireball hazard | ✅ |
| 32 | Firebar — rotating fire hazard | ✅ |
| 33 | Viewport-based enemy activation (spawn when near camera) | ✅ |
| 34 | Smart ledge detection across all ground enemies | ✅ |
| 35 | Aggro pursuit AI (enemies chase Mario when close) | ✅ |

### Power-Ups and Items (10 points)

| # | Feature | Status |
|---|---|:---:|
| 36 | Coin — static collectible with score award | ✅ |
| 37 | Mushroom — moving item, transforms Mario to Super | ✅ |
| 38 | Fire Flower — transforms Mario to Fire state | ✅ |
| 39 | Star — bouncing item, grants temporary invincibility | ✅ |
| 40 | Question Block — hit from below to spawn items | ✅ |
| 41 | Question Block weighted drop (70% Coin, 15% Mushroom, 15% Fire) | ✅ |
| 42 | Brick Block — breakable by Super/Fire Mario | ✅ |
| 43 | Coin popup animation on Question Block hit | ✅ |
| 44 | Springboard (trampoline) — bounce boost platform | ✅ |

### Level Completion (15 points)

| # | Feature | Status |
|---|---|:---:|
| 45 | Level 1 — World 1-1 Overworld (tutorial flow) | ✅ |
| 46 | Level 2 — World 1-2 Underground | ✅ |
| 47 | Level 3 — World 1-3 Underwater | ✅ |
| 48 | Level 4 — World 1-4 Castle (Bowser boss) | ✅ |
| 49 | Increasing difficulty across levels | ✅ |
| 50 | Multi-theme rendering (Overworld/Underground/Underwater/Castle) | ✅ |
| 51 | Sub-area transitions via pipes with theme changes | ✅ |
| 52 | Castle ending with Toad NPC after Bowser defeat | ✅ |
| 53 | Moving elevator platforms | ✅ |
| 54 | Water/Lava environmental rendering | ✅ |

### Sounds (10 points)

| # | Feature | Status |
|---|---|:---:|
| 55 | Jump sound effect | ✅ |
| 56 | Coin collection sound | ✅ |
| 57 | Enemy stomp/defeat sound | ✅ |
| 58 | Power-up collection sound | ✅ |
| 59 | FireBall shooting sound | ✅ |
| 60 | Player death sound | ✅ |
| 61 | Background music per theme (Overworld/Underground/Castle/Underwater) | ✅ |
| 62 | Star invincibility theme override and restoration | ✅ |
| 63 | Game Over and Win music | ✅ |
| 64 | 4-voice concurrent SFX pool (no channel clipping) | ✅ |
| 65 | Low-time warning SFX | ✅ |

---

## B. Design and Implementation Features (35 points)

### Object-Oriented Design (10 points)

| # | Feature | Status |
|---|---|:---:|
| 66 | Inheritance: Entity → Character → Mario/Enemy hierarchy | ✅ |
| 67 | Polymorphism: `std::vector<std::unique_ptr<Entity>>` with virtual `update`/`draw` | ✅ |
| 68 | Encapsulation: private m_ members with getters/setters | ✅ |
| 69 | Abstraction: IGameState, ICommand, IObserver interfaces | ✅ |
| 70 | Item hierarchy: Item → Coin/Mushroom/FireFlower/Star | ✅ |

### Design Patterns (25 points — 5 patterns × 5 points)

| # | Pattern | Implementation | Status |
|---|---|---|:---:|
| 71 | Factory Method | EntityFactory with polymorphic Creator hierarchy | ✅ |
| 72 | Singleton | GameManager, SoundManager | ✅ |
| 73 | Observer | EventBus with RAII Subscription tokens | ✅ |
| 74 | State | IGameState → MenuState/PlayState/PauseState/GameOverState/WinState | ✅ |
| 75 | Command | ICommand → JumpCommand/MoveLeftCommand/MoveRightCommand/etc. + InputHandler | ✅ |

---

## C. Additional / Advanced Features (15 points)

### AI (5 points)

| # | Feature | Status |
|---|---|:---:|
| 76 | Proximity-based aggro chase mode for ground enemies | ✅ |
| 77 | Predictive lead egg tossing (Lakitu) | ✅ |
| 78 | Altitude-adaptive multi-height fire (Bowser) | ✅ |
| 79 | Smart ledge detection and direction reversal | ✅ |
| 80 | Narrow-range oscillation breakout AI | ✅ |

### Multiple Players (5 points)

| # | Feature | Status |
|---|---|:---:|
| 81 | Character selection screen (Mario / Luigi) | ✅ |
| 82 | Luigi with different sprite and abilities | ✅ |
| 83 | 2-Player Co-op campaign mode | ✅ |
| 84 | 2-Player Versus (PvP) stomp duel mode | ✅ |
| 85 | Co-op head-bounce boost jump | ✅ |
| 86 | Dual midpoint camera tracking in Co-op | ✅ |
| 87 | Invisible ceiling guard for Co-op separation | ✅ |

### 3D Game (5 points)

| # | Feature | Status |
|---|---|:---:|
| 88 | N/A — Project is 2D | ❌ |

---

## D. Game State Management

| # | Feature | Status |
|---|---|:---:|
| 89 | Menu → Play → Pause → GameOver → Win state transitions | ✅ |
| 90 | Score tracking and display via HUD | ✅ |
| 91 | Lives counter display | ✅ |
| 92 | Level countdown timer (400s) with timeout death | ✅ |
| 93 | Save/Load game progress (SaveManager) | ✅ |
| 94 | Atomic save-file replacement for data safety | ✅ |
| 95 | High score persistence | ✅ |
| 96 | Level unlock progression | ✅ |
| 97 | Stomp combo scoring (100→200→400→800→1000→2000→4000→5000→8000→1-Up) | ✅ |
| 98 | Animated score popups | ✅ |

---

## E. Graphics and Rendering

| # | Feature | Status |
|---|---|:---:|
| 99 | NES-style sprite atlas system (consolidated tileset) | ✅ |
| 100 | Animation system with frame-based sprite sequences | ✅ |
| 101 | Parallax background rendering | ✅ |
| 102 | Integer-scaling letterbox display (640×360 logical canvas) | ✅ |
| 103 | Camera with horizontal deadzone and vertical edge-margin follow | ✅ |
| 104 | Theme-aware tile/entity rendering across 4 world themes | ✅ |

---

## Summary

| Category | Points Available | Features Implemented |
|---|:---:|:---:|
| Functionality | 65 | ✅ All categories covered |
| Design & Implementation | 35 | ✅ 5/5 design patterns |
| AI | 5 | ✅ Advanced enemy AI |
| Multiple Players | 5 | ✅ Co-op + PvP + Character Select |
| 3D Game | 5 | ❌ Not applicable (2D project) |
| **Total Available** | **115** | **110/115** |

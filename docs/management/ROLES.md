# Team Roles — 5 Members

> Each person "owns" 1 module: designing, coding, and testing that module.  
> When collaboration is needed in another module → discuss first, do not modify someone else's code without permission.

---

## TV1 (Duong) — Architect / Team Lead

**Main Responsibilities:**
- Overall OOP design: class hierarchy, interfaces between modules
- Write & maintain class diagram (draw.io / PlantUML)
- Responsible for implementing **5 design patterns** (ensuring 25 points on the rubric)
- Code review at the end of each week: check if members are on the right track
- Coordinate when 2 modules need to interface with each other

**Owned Modules:**
- `GameManager` (Singleton) — coordinates the game loop
- `EventBus` / `Observer` — global game event system
- `GameState` machine (State pattern) — menu / playing / pause / gameover

**Interfaces TV1 must expose to other members:**
```cpp
// TV2 uses this to change state
GameManager::getInstance().changeState(new PlayState());

// TV3/TV4/TV5 use this to publish events
EventBus::getInstance().notify(EventType::COIN_COLLECTED);

// TV2 uses this to subscribe to events; subscribers implement IObserver
auto token = EventBus::getInstance().subscribe(EventType::PLAYER_DIED, observer);
```

---

## TV2 (Nhat) — Engine & Render

**Main Responsibilities:**
- Setup SFML: window, game loop, delta time
- Rendering system: sprites, texture manager, z-ordering
- `AnimationSystem`: sprite sheet, frame timing
- `Camera`: follows Mario, clamped to level boundaries
- UI screens: Menu, GameOver, Win screen, transitions

**Owned Modules:**
- `Game` — main loop, window, delta time
- `TextureManager` — load and cache textures
- `AnimationSystem` — manage animation clips
- `Camera` — viewport follow player
- `MenuState`, `GameOverState`, `WinState` — UI screens

**Output for TV3 in Week 2:**
```cpp
// TV2 provides these for TV3 to use
void Entity::setSprite(const std::string& texturePath);
void Entity::playAnimation(const std::string& clipName);
```

---

## TV3 (Bao) — Mario & Physics

**Main Responsibilities:**
- Class `Mario`: movement, jump, gravity, power-up states
- `PhysicsEngine`: gravity, velocity, acceleration
- `CollisionManager`: AABB collision detection, resolution
- Power-up system: Small → Big → Fire (including transition effects)
- `FireBall` projectile if FireFlower is active

**Owned Modules:**
- `Entity` (base class for all physical objects)
- `Character` (abstract, inherits from Entity)
- `Mario` (inherits from Character)
- `PhysicsEngine`
- `CollisionManager`
- `FireBall`

**Collision interface TV4 needs:**
```cpp
// Real dispatch flow (no per-pair checkCollision/resolveCollision API exists):
// Box2D contacts enter via CollisionManager::resolve(b2Contact*, TileMap&),
// which builds a typed CollisionContext and hands it to
CollisionManager::dispatch(CollisionContext& context, TileMap& tileMap);
// Enemy defeats are committed by the central transaction
CollisionManager::defeatEnemy(Enemy& victim, DefeatCause cause, Mario* owner, int streakIndex = 0);
```

---

## TV4 (Vy) — Level & Enemy

**Main Responsibilities:**
- Format and load tilemap from a `.txt` file (define format in Week 1)
- Design and build 3 levels with increasing difficulty
- `Goomba`: AI patrol back and forth, dies when stomped
- `Koopa`: AI patrol, stomped into a shell, shell can slide
- `SaveManager`: save/load high scores to/from file

**Owned Modules:**
- `Level` — load, update, render a level
- `TileMap` — parse file, render tiles
- `Enemy` (abstract, inherits from Character)
- `Goomba`, `Koopa` (inherit from Enemy)
- `SaveManager`
- Data files: `levels/level1.txt`, `level2.txt`, `level3.txt`

**Level file format (defined by TV4, followed by the team):**
```txt
Each level is stored as a `.txt` file inside the `levels/` folder.

Rules:
- Each line represents one row of the map.
- Each character represents one tile or object.
- All rows should have the same length.
- `M` should appear only once as Mario's spawn position.
- Enemies such as `G` and `K` will be spawned based on their positions in the file.

Symbols:
```txt
. = empty space
1 = ground block
B = brick block
? = question block
C = coin
G = Goomba enemy
K = Koopa enemy
M = Mario spawn point
F = finish flag / level end
```

---

## TV5 (Truyen) — UI, Sound & Items

**Main Responsibilities:**
- `SoundManager` (Singleton): load and play sound effects + background music
- `HUD`: display score, lives, time, power-up indicator
- All items: `Coin`, `Mushroom`, `FireFlower`, `Star`
- `InputHandler` with Command pattern: map keys → actions
- Collect and organize assets (textures, sounds) into the correct directories

**Owned Modules:**
- `SoundManager` (Singleton)
- `HUD`
- `Item` (abstract base)
- `Coin`, `Mushroom`, `FireFlower`, `Star`
- `ICommand`, `InputHandler` (Command pattern)
- `assets/` directory

**Command pattern TV5 exposes to TV3:**
```cpp
// TV3 uses InputHandler to get actions
auto action = inputHandler.getAction(sf::Keyboard::Space);
if (action) action->execute();
```

---

## Collaboration Rules

1. **Do not commit to other people's modules** without prior notification.
2. **Interface changes → notify the entire team** before merging into `develop`.
3. **Friday End of Week:** TV1 merges `develop` into `main`, everyone pulls and rebuilds to verify nothing is broken.
4. **TV1 has the final decision** in case of design conflicts. Other opinions → raise during meetings, do not modify directly.

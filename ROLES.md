# Team Roles — 5 Members

> Each person "owns" 1 module: designing, coding, and testing that module.  
> When collaboration is needed in another module → discuss first, do not modify someone else's code without permission.

---

## TV1 (Dương) — Architect / Team Lead

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
EventBus::getInstance().publish(Event::COIN_COLLECTED, data);

// TV2 uses this to subscribe to events
EventBus::getInstance().subscribe(Event::PLAYER_DIED, callback);
```

---

## TV2 (Nhật) — Engine & Render

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

## TV3 (Bảo) — Mario & Physics

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
// TV3 exposes these for TV4 to use for enemies
CollisionManager::checkCollision(Entity* a, Entity* b);
CollisionManager::resolveCollision(Entity* a, Entity* b, Direction dir);
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
```
# level1.txt — 0=empty, 1=ground, 2=brick, 3=coin, G=Goomba, K=Koopa, M=Mario spawn
0 0 0 0 0 0 0 0 0
1 1 G 1 1 0 K 0 M
```

---

## TV5 (Truyền) — UI, Sound & Items

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
if (action) action->execute(mario);
```

---

## Collaboration Rules

1. **Do not commit to other people's modules** without prior notification.
2. **Interface changes → notify the entire team** before merging into `develop`.
3. **Friday End of Week:** TV1 merges `develop` into `main`, everyone pulls and rebuilds to verify nothing is broken.
4. **TV1 has the final decision** in case of design conflicts. Other opinions → raise during meetings, do not modify directly.

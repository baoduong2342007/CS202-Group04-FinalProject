# General Rules — Coding & Git

> These are the rules of the team. If anyone violates them → TV1 has the right to request a refactor before merging.

---

## 1. Git Workflow

### Branches

```
main        ← only contains stable code, merged at the end of each week
develop     ← main integration branch, everyone merges features here
feature/... ← daily development branch
fix/...     ← hotfix branch
```

**Process:**
```
1. Start new task:
   git checkout develop
   git pull
   git checkout -b feature/goomba-ai

2. Finish task → commit:
   git add .
   git commit -m "[TV4] Add Goomba patrol AI with direction reversal"

3. Push to remote:
   git push origin feature/goomba-ai

4. Create Pull Request into develop on GitHub
   → Tag 1 reviewer (prioritize TV1 if it is a core class)
   → Once approved → merge → delete branch

5. End of week (Friday):
   TV1 merges develop into main
   Whole team pulls main and rebuilds to confirm
```

### Commit Message Format

```
[TVx] <Verb> <Short Description>

Good examples:
[TV3] Add gravity and vertical velocity to Mario
[TV2] Fix camera clamp when Mario reaches level boundary
[TV1] Implement Observer pattern with EventBus singleton
[TV4] Design Level 2 tilemap with increasing difficulty

Bad examples (do not use):
fix stuff
update code
TV4 done
wip
```

### Commit Rules

- **All commit messages must be written in English**. Example: `[TV3] Add jump mechanics` instead of `[TV3] Them nhay`.
- **Commit frequently**, after each small unit of work — do not bundle 8 hours of work into 1 commit.
- **Do not commit code that does not compile** into `develop`.
- **Do not commit build files** (`/build`, `.exe`, `.o`) — `.gitignore` is already set up.
- **Do not commit temporary files** (`test_main.cpp`, `debug.txt`, personal `TODO.md`).

---

## 2. C++ Coding Conventions

### Language Rule
- **English Only**: All code identifiers (variable names, class names, functions, etc.), source comments, and project documentation must be written in English.

### Naming

```cpp
// Class → PascalCase
class GameManager { };
class TileMap { };

// Method → camelCase
void update(float deltaTime);
bool loadFromFile(const std::string& path);

// Variable → camelCase
int playerScore = 0;
float jumpVelocity = -500.f;

// Private member → m_ prefix
class Mario {
private:
    sf::Vector2f m_position;
    int m_health;
    MarioState m_state;
};

// Constant & enum value → UPPER_SNAKE_CASE
const int TILE_SIZE = 32;
const int MAX_LIVES = 3;

enum class MarioState { SMALL, BIG, FIRE };
// use scoped enum (enum class), do not use plain enum

// File → class name (1-1)
// GameManager → GameManager.h + GameManager.cpp
```

### Class Structure — Layout Order

```cpp
class Mario : public Character {
public:
    // 1. Constructor / Destructor
    Mario();
    ~Mario() override;

    // 2. Override methods
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    // 3. Public methods (alphabetical)
    void applyPowerUp(PowerUpType type);
    void handleInput(const InputHandler& input);
    void loseLife();

    // 4. Getters / Setters
    int getLives() const;
    sf::Vector2f getPosition() const;
    MarioState getState() const;

private:
    // 5. Private methods
    void jump();
    void applyGravity(float dt);
    void updateAnimation();

    // 6. Private members
    int m_lives;
    MarioState m_state;
    sf::Vector2f m_velocity;
};
```

### Pointers & Memory

```cpp
// PREFER smart pointers
std::unique_ptr<Enemy> enemy = std::make_unique<Goomba>(pos);
std::shared_ptr<sf::Texture> tex = std::make_shared<sf::Texture>();

// Use raw pointers ONLY WHEN: the object does not own the memory (observer pointer)
void setTarget(Mario* mario);  // OK — does not own

// DO NOT use new/delete directly in game logic
// BAD:
Enemy* e = new Goomba(pos);  // who deletes it?

// EXCEPTION: EntityFactory returns raw pointer — caller owns it
// → Factory method: caller wraps into unique_ptr immediately
auto enemy = std::unique_ptr<Enemy>(EntityFactory::create(EnemyType::GOOMBA, pos));
```

### Functions

```cpp
// Maximum 40–50 lines per function
// If longer → split into smaller private helper functions

// Functions that do not modify state → const
int getScore() const;
bool isAlive() const;

// Large parameters → pass by const reference
void loadLevel(const std::string& filename);
void render(const sf::RenderWindow& window) const;

// Return bool if it can fail
bool loadTexture(const std::string& path);  // false if file doesn't exist
```

### Do Not Use

```cpp
// Do not use using namespace std; in headers — namespace pollution
// BAD in header:
using namespace std;

// OK in .cpp (and only there):
using namespace std;  // acceptable, but prefer explicit std::

// Do not use magic numbers
// BAD:
if (mario.y > 720) mario.die();

// GOOD:
const float SCREEN_HEIGHT = 720.f;
if (mario.getY() > SCREEN_HEIGHT) mario.die();

// Do not leave dead code (commented-out code) in PRs
// BAD:
// void oldUpdate() { ... }   ← delete it
```

---

## 3. Required Header Comments

Each `.cpp` and `.h` file must have a header comment like this:

```cpp
/**
 * @file Goomba.cpp
 * @author TV4
 * @brief Goomba enemy: patrol AI, death on stomp, Factory-compatible
 * @note Week 3 — uses EntityFactory for spawning
 */
```

---

## 4. Design Patterns Guidelines

Each pattern must be clearly documented in the code with a comment block:

```cpp
// ============================================================
// PATTERN: Factory Method
// Reason: avoids hardcoded new Goomba(), new Koopa() in multiple places;
//         allows adding new enemy types without modifying Level.cpp
// ============================================================
Enemy* EntityFactory::create(EnemyType type, sf::Vector2f pos) {
    switch (type) {
        case EnemyType::GOOMBA: return new Goomba(pos);
        case EnemyType::KOOPA:  return new Koopa(pos);
        default: return nullptr;
    }
}
```

5 patterns to implement and document:

| # | Pattern | Main File | Implementer |
|---|---|---|---|
| 1 | Factory | `EntityFactory.h/.cpp` | TV1 (Dương) |
| 2 | Singleton | `GameManager.h`, `SoundManager.h` | TV1 (Dương), TV5 (Truyền) |
| 3 | Observer | `EventBus.h`, `IObserver.h` | TV1 (Dương) |
| 4 | State | `IGameState.h`, `*State.cpp` | TV1 (Dương), TV2 (Nhật) |
| 5 | Command | `ICommand.h`, `InputHandler.h` | TV5 (Truyền) |

---

## 5. Pull Request Checklist

Before creating a PR, check yourself:

- [ ] Code compiles without warnings
- [ ] No dead code (meaningless commented-out code)
- [ ] No messy debug `std::cout` — use `#ifdef DEBUG` if needed
- [ ] Manual test of the scenario being implemented
- [ ] Branch name and commit message formatted correctly

Reviewer check:
- [ ] Logic aligns with class diagram design
- [ ] Naming rules are followed
- [ ] Build is not broken (checkout to branch and test build)
- [ ] Patterns are used correctly if applicable

---

## 6. Team Meetings Rules

- **Weekly Sync Meeting** (~30 minutes): each member states their tasks for the week
- **Mid-week Sync** (~15 minutes): raise any blockers so other members can support
- **Weekend Demo Checkpoint**: sit together, run the game, check off list — do not skip
- **Communication Channels**: Discord/Zalo for team; ask technical questions in dedicated threads

---

## 7. Pre-submission Checklist

In Week 6, TV1 uses this checklist:

- [ ] Clean build succeeds from scratch on a new machine
- [ ] Play through 3 levels without crash
- [ ] `docs/class_diagram.png` contains all main classes
- [ ] `docs/design_patterns.md` describes all 5 patterns
- [ ] Code is well documented (especially where patterns are used)
- [ ] Demo video recorded, with voice explanation
- [ ] Rubric 65+35+15: each row checked done or reason documented
- [ ] Submit on time and in correct format

# Sprint 6 Locked Interfaces

> Owner: TV1 — S6-TV1-03
> Updated: 2026-08-12
> Status: implemented; future changes require architecture review

## Progress and release catalog

```cpp
struct GameProgress {
    int currentLevel = 1;
    int score = 0;
    int coins = 0;
    int lives = 3;
    MarioState power = MarioState::SMALL;
};

enum class CameraVerticalMode {
    LOCKED,
    DEAD_ZONE
};

struct LevelDefinition {
    int number;
    std::string filePath;
    std::string worldLabel;
    LevelTheme theme;
    MusicId music;
    CameraVerticalMode cameraMode;
};
```

The additional camera mode was approved during Sprint 6 remediation because camera behavior is level metadata, not a global theme guess. `LevelCatalog::getAll()` contains exactly Levels 1, 2, and 3.

## Save data

```cpp
struct SaveData {
    int version = 1;
    int highScore = 0;
    int highestUnlockedLevel = 1;
    float soundVolume = 80.0f;
    float musicVolume = 70.0f;
};
```

SaveManager owns persistence. Loaded/unlocked level values are clamped to `LevelCatalog::count()` without discarding other valid fields.

## State lifecycle

```cpp
virtual void onEnter();
virtual void onExit();
virtual void onPause();
virtual void onResume();
```

`GameManager` defers state operations. Push pauses the prior state; pop exits the overlay and resumes the state below.

## Input and commands

- `InputState` distinguishes Pressed, Held, and Released.
- Run is a held Shift command.
- Shoot is a pressed X command.
- `ICommand` has no undo API because the runtime has no undo stack.

## Enemy defeat

```cpp
enum class DefeatCause {
    STOMP,
    SHELL,
    FIREBALL,
    STAR,
    PIT
};
```

`CollisionManager::defeatEnemy(Enemy&, DefeatCause, Mario*)` is the single score/event commit transaction.

## FireBall request

```cpp
bool Level::requestFireBallShot(Mario& mario);
bool Level::requestFireBallShot();
```

These are the only production request overloads. Level owns projectile construction, active maximum two, locked-world deferral, and entity storage. Mario owns the authoritative shot cooldown. `FIREBALL_SHOT` is published only after construction; SoundManager owns playback.

## Power-up progression

The body tier and FIRE capability are separate parts of Mario's runtime form:

```text
SMALL + Mushroom       -> SUPER
SMALL + FireFlower     -> SMALL FIRE
SUPER + FireFlower     -> SUPER FIRE
```

Normal `?` blocks remain adaptive (Small -> Mushroom, Super/Fire -> FireFlower). Explicit `f` blocks always spawn a FireFlower. `Mario::powerUp()` preserves the current body tier, and the atlas selects `FireSmallMario` or `FireBigMario` accordingly. `Mario::setMarioState()` remains the exact-state API used by restore/debug flows; the overload with `fireIsSuper` preserves the form across level transitions.

## Ownership

- EntityFactory returns `std::unique_ptr<Entity>`.
- Level owns Mario, TileMap, Camera, Box2D world/listener, and the entity collection.
- Raw pointers are non-owning references only.
- A Box2D body is never destroyed inside a contact callback.
- Entity bounds have one source of truth derived from current position/size or the active fixture; no stale parallel cache is public.

## Change process

1. State the reason and affected producer/consumer modules.
2. Add or update regression evidence for the public behavior.
3. Obtain TV1 architecture approval before merge.
4. Update this document and the class diagram in the same change.

# Design Patterns

This document describes the five main design patterns implemented in the
SuperMario project.

## 1. Factory Method Pattern
**Location:** `include/patterns/EntityFactory.h`, `include/patterns/EntityCreator.h`, `src/patterns/EntityFactory.cpp`
**Implementer:** TV1 (Dương)
**Reasoning:** Avoids hardcoded instantiation (`new Goomba()`, `new Koopa()`)
scattered throughout `Level.cpp`. The instantiable `EntityFactory` exposes the
canonical `create(const SpawnRequest&, const SpawnContext&)` seam and delegates
to the abstract `EntityCreator` contract implemented by `EnemyCreator`,
`ItemCreator`, and `WorldObjectCreator`. The static `createEnemy`, `createItem`,
and `createFromTileCode` methods remain compatibility shims for existing
callers and forward to the same mapping authority.

## 2. Singleton Pattern
**Location:** `include/core/GameManager.h`, `include/core/SoundManager.h`, `include/core/SaveManager.h`
**Implementer:** TV1 (Dương), TV5 (Truyền)
**Reasoning:** Ensures only one instance of core systems exists throughout the application lifecycle.
- **GameManager**: Manages the global `IGameState` stack and `SaveManager`. It must be universally accessible for states to push/pop other states.
- **SoundManager**: Coordinates audio playback. A singleton ensures that audio playing asynchronously isn't duplicated or lost.
- **SaveManager**: Manages session-wide progress (high score, highest unlocked level) ensuring consistency across all menus and gameplay levels.

The SFX contract is centralized in `include/core/SoundManifest.def`. It is
consumed to derive typed `SoundId` metadata for `SoundManager`; CMake reads the
same manifest while packaging runtime sound effects.

## 3. Observer Pattern
**Location:** `include/patterns/EventBus.h`, `include/patterns/IObserver.h`
**Implementer:** TV1 (Dương)
**Reasoning:** Decouples gameplay logic from audio and UI code. Publishers
send value-only `GameEvent` data through `EventBus`; `HUD` and `SoundManager`
subscribe without direct gameplay references. Each subscription is represented
by a move-only RAII `Subscription` token: destroying or resetting the token
disconnects that registration. The EventType-only `notify` overload remains a
compatibility publisher.

## 4. State Pattern
**Location:** `include/states/IGameState.h`, `MenuState`, `PlayState`, `GameOverState`, `WinState`, `PauseState` (and Mario power-up states)
**Implementer:** TV1 (Dương), TV2 (Nhật)
**Reasoning:** Encapsulates the behavior of different game modes into independent objects. `GameManager` switches seamlessly between them using a stack-based approach (push/pop).
We also employ the State Pattern within `Mario.cpp` for powerups (`SmallMarioState`, `SuperMarioState`, `SmallFireMarioState`, `SuperFireMarioState`) to delegate state-specific animation logic cleanly.

## 5. Command Pattern
**Location:** `include/patterns/ICommand.h`, `include/patterns/InputHandler.h`
**Implementer:** TV5 (Truyền)
**Reasoning:** Decouples raw user input (e.g., pressing 'X' or 'Up') from the actions performed by `Mario`. This enables the `PlayState` to rebind keys dynamically and allows actions to be encapsulated into discrete command objects like `JumpCommand`, `RunCommand`, and `ShootCommand`.

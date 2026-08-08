# Design Patterns

This document describes the 5 main design patterns implemented in the SuperMario project.

## 1. Factory Pattern
**Location:** `include/patterns/EntityFactory.h`, `src/patterns/EntityFactory.cpp`
**Implementer:** TV1 (Dương)
**Reasoning:** Avoids hardcoded instantiation (`new Goomba()`, `new Koopa()`) scattered throughout `Level.cpp`. Provides a centralized location to map `EntityIdentifier` (from `TileMap`) or `EnemyType` directly to their heap-allocated representations without coupling `Level` tightly to every new enemy or item type.

## 2. Singleton Pattern
**Location:** `include/core/GameManager.h`, `include/core/SoundManager.h`, `include/core/SaveManager.h`
**Implementer:** TV1 (Dương), TV5 (Truyền)
**Reasoning:** Ensures only one instance of core systems exists throughout the application lifecycle.
- **GameManager**: Manages the global `IGameState` stack and `SaveManager`. It must be universally accessible for states to push/pop other states.
- **SoundManager**: Coordinates audio playback. A singleton ensures that audio playing asynchronously isn't duplicated or lost.
- **SaveManager**: Manages session-wide progress (high score, highest unlocked level) ensuring consistency across all menus and gameplay levels.

## 3. Observer Pattern
**Location:** `include/patterns/EventBus.h`, `include/patterns/IObserver.h`
**Implementer:** TV1 (Dương)
**Reasoning:** Decouples gameplay logic from audio and UI code. For example, when Mario collects a coin, `Mario` simply notifies the `EventBus` (`COIN_COLLECTED`). The `HUD` and `SoundManager` are subscribed to this event and update the score/play the sound accordingly without `Mario` needing a direct reference to them.

## 4. State Pattern
**Location:** `include/states/IGameState.h`, `MenuState`, `PlayState`, `GameOverState`, `WinState`, `PauseState` (and Mario power-up states)
**Implementer:** TV1 (Dương), TV2 (Nhật)
**Reasoning:** Encapsulates the behavior of different game modes into independent objects. `GameManager` switches seamlessly between them using a stack-based approach (push/pop).
We also employ the State Pattern within `Mario.cpp` for powerups (`SmallMarioState`, `SuperMarioState`, `FireMarioState`) to delegate state-specific animation logic cleanly.

## 5. Command Pattern
**Location:** `include/patterns/ICommand.h`, `include/patterns/InputHandler.h`
**Implementer:** TV5 (Truyền)
**Reasoning:** Decouples raw user input (e.g., pressing 'X' or 'Up') from the actions performed by `Mario`. This enables the `PlayState` to rebind keys dynamically and allows actions to be encapsulated into discrete command objects like `JumpCommand`, `RunCommand`, and `ShootCommand`.

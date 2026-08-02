# Code Review: TV3 (Bảo) — `feature/mario-and-physics`

**Reviewer:** Antigravity (Senior C++ Reviewer)
**Date:** 2026-07-28
**Scope:** All files modified/added on `origin/feature/mario-and-physics` vs `develop`
**Reference:** [ACTION_ITEMS_SPRINT4_FIXES.md](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/docs/ACTION_ITEMS_SPRINT4_FIXES.md) — Tasks 3.1, 3.2, 3.3

---

## 1. Verification Log

| File | Verified via Tool? | Verification Notes |
|---|---|---|
| [Entity.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/Entity.h) | `git show` + `git diff` | Added `destroyPhysicsBody()`, `isActive()`, `isPendingDestroy()`, `markForDestroy()`, `m_active`, `m_pendingDestroy` |
| [Entity.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Entity.cpp) | `git show` + `git diff` | Destructor now calls `destroyPhysicsBody()`, nullifies userData before destroy; zero friction on dynamic; checked include order |
| [Mario.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/Mario.h) | `git show` + `git diff` | New members: `m_lives`, `m_isRunning`, `m_isSkidding`, `m_wasJumpPressed`; new methods: `loseLife()`, `respawn()`, `canShootFireBall()`, `rebuildFixture()`, `applyMovementPhysics()` |
| [Mario.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp) | `git show` + `git diff` | Full rewrite of physics model; verified PPM conversions; checked EventBus integration |
| [FireBall.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/FireBall.h) | `git show` | New class; extends Entity; includes bounce/deactivate mechanics |
| [FireBall.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/FireBall.cpp) | `git show` | Verified PPM conversions, deferred destruction, bounce logic |
| [CollisionManager.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/physics/CollisionManager.h) | `git show` | New static utility class; forward declares `Mario`, `Entity` |
| [CollisionManager.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/physics/CollisionManager.cpp) | `git show` | Verified stomp logic, FireBall resolution, grounded detection, PPM usage |
| [ContactListener.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/physics/ContactListener.cpp) | `git show` + `git diff` | Refactored to delegate to CollisionManager — clean and thin |
| [TV3Demo.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/demo/TV3Demo.h) | `git show` | Free function header for demo runner |
| [TV3Demo.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/demo/TV3Demo.cpp) | `git show` | Standalone demo with floor/wall/overhead block; verified PPM consistency |
| [main.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/main.cpp) | `git show` | Replaced game.run() with runTV3Demo() |
| [CMakeLists.txt](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/CMakeLists.txt) | `git diff` | Added `DEBUG` compile definition; removed macOS comment |
| `Group04_07 (1).md` | `git show` | Weekly report file — not code |
| [EventType.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/patterns/EventType.h) | `grep_search` | Confirmed `PLAYER_JUMPED`, `PLAYER_DIED`, `PLAYER_POWER_UP`, `PLAYER_POWER_DOWN`, `ENEMY_STOMPED` all exist |
| [Character.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/Character.h) | `git show` | Confirmed `Direction` enum, `m_health` (protected), `isGrounded()/setGrounded()` exist |

---

## 2. Summary

### 🟡 Request Changes

TV3's branch delivers a **substantial and well-engineered** physics rewrite for Mario with authentic NES-style acceleration/friction model, FireBall projectile class, and a completely refactored CollisionManager. The code quality is generally high — proper PPM conversions, safe Box2D body destruction, clean naming conventions, and good use of EventBus for decoupling.

However, there are **several issues that must be addressed before merge**:

| Severity | Count | Summary |
|---|---|---|
| 🔴 Critical | 2 | `main.cpp` hijacked; Task 3.1 (Enemy↔Wall, Mario↔Coin) not implemented |
| 🟠 Major | 3 | `handleInput()` still hardcodes keys (Task 3.2 open); DEBUG always defined; `applyMovementPhysics` >50 lines |
| 🟡 Minor | 4 | Include order issues; `Group04_07 (1).md` committed; `MAX_FALL_SPEED` unused in a subtle way; missing `setSprite()` call |

---

## 3. Code Conventions

### 🔴 Critical: `main.cpp` hijacked — breaks everyone's build

**File:** [main.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/main.cpp)

```cpp
#include "demo/TV3Demo.h"

int main() {
    return runTV3Demo();
}
```

Per project convention, `main.cpp` should only contain `Game game; game.run(); return 0;`. TV3's branch replaces the entire game entry point with a personal demo. **This will break all other members' workflows when merged to develop.**

> [!CAUTION]
> **Fix:** Revert `main.cpp` to its develop-branch state. Keep `TV3Demo` as a separate demo executable or behind a `#ifdef TV3_DEMO` guard. Do **not** merge this change into develop.

---

### 🟡 Include Order Violations

**File:** [Mario.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp) — Top of file

```cpp
#include "entities/Mario.h"    // ✅ Own header first
#include <iostream>            // ✅ Standard lib
#include <cmath>               // ✅ Standard lib
#include "physics/PhysicsEngine.h"  // ✅ Project header
#include "patterns/EventBus.h"      // ✅ Project header
#include "patterns/EventType.h"     // ✅ Project header
```
✅ Include order is correct in Mario.cpp.

**File:** [CollisionManager.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/physics/CollisionManager.cpp) — Top of file

```cpp
#include "physics/CollisionManager.h"  // ✅ Own header first
#include <iostream>                     // ✅ Standard lib
#include <cmath>                        // ✅ Standard lib
#include "entities/Entity.h"            // ⚠️ Project headers (should come after SFML)
```
No SFML includes needed here — acceptable. ✅

**File:** [TV3Demo.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/demo/TV3Demo.cpp) — missing header comment with `@file`, `@author`, `@brief` block, though the `.h` file has it.

> [!NOTE]
> TV3Demo.cpp is missing the required `/** @file ... */` header comment block at the top of the file. The first line shown is a truncated include section.

---

### 🟡 Naming — all OK

- Classes: `Mario`, `FireBall`, `CollisionManager` → PascalCase ✅
- Methods: `handleInput()`, `applyMovementPhysics()`, `rebuildFixture()` → camelCase ✅
- Private members: `m_marioState`, `m_jumpForce`, `m_lives`, `m_isRunning`, `m_isSkidding`, `m_wasJumpPressed` → m_ prefix ✅
- Constants: `WALK_MAX_SPEED`, `MAX_FALL_SPEED`, `FATAL_DAMAGE` → UPPER_SNAKE_CASE ✅
- Enum: `MarioState::SMALL/SUPER/FIRE` — scoped enum ✅

---

### 🟠 Function Length Violation

**File:** [Mario.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp) — `applyMovementPhysics()`

This method is approximately **55–60 lines** of logic, exceeding the project convention of 40–50 lines max.

> [!WARNING]
> **Recommendation:** Split into `applyGroundPhysics()` and `applyAirPhysics()` private helpers. The ground/air physics blocks are naturally separable.

---

### 🟠 `DEBUG` always defined in CMakeLists.txt

**File:** [CMakeLists.txt](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/CMakeLists.txt)

```cmake
target_compile_definitions(main PRIVATE DEBUG)
```

This defines `DEBUG` unconditionally for **all build types**, meaning debug `std::cout` statements in Mario and FireBall will fire even in Release builds. This violates the coding rule: *"No messy debug `std::cout` — use `#ifdef DEBUG` if needed"*.

> [!WARNING]
> **Fix:** Should be conditional: `target_compile_definitions(main PRIVATE $<$<CONFIG:Debug>:DEBUG>)` (generator expression).

---

### 🟡 `Group04_07 (1).md` committed to repo

This is a weekly report file with spaces and parentheses in the filename, placed at the project root. It doesn't belong in the repo.

> [!NOTE]
> **Fix:** Add to `.gitignore` or move to `docs/`. Avoid filenames with spaces and parentheses.

---

## 4. Architecture & Physics Risks

### ✅ Box2D Body Destruction — Properly Handled

**File:** [Entity.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Entity.cpp) — `destroyPhysicsBody()`

```cpp
void Entity::destroyPhysicsBody() {
    if (m_body) {
        b2World* world = m_body->GetWorld();
        if (world) {
            m_body->GetUserData().pointer = 0;  // ✅ Nullify before destroy
            world->DestroyBody(m_body);
        }
        m_body = nullptr;
    }
}
```

TV3 correctly:
1. Nullifies `userData.pointer` **before** calling `DestroyBody` to prevent dangling pointers in contact callbacks
2. Checks `world != nullptr` before destroying
3. Sets `m_body = nullptr` after destruction
4. Called from destructor `~Entity()` and from `initPhysics()` (clearing previous body)

**No memory leak risk** for b2Body objects. ✅

---

### ✅ PPM Conversions — Consistent and Correct

All pixel↔meter conversions across Mario, FireBall, CollisionManager, and Entity use `PhysicsEngine::pixelsToMeters()` / `PhysicsEngine::metersToPixels()` consistently. Verified in:

- [Mario.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp): Jump velocity, movement velocity, terminal velocity clamp, position sync — all use PPM helpers ✅
- [FireBall.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/FireBall.cpp): Speed, bounce velocity — all use PPM helpers ✅
- [CollisionManager.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/physics/CollisionManager.cpp): Stomp bounce velocity — uses PPM helper ✅
- [TV3Demo.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/demo/TV3Demo.cpp): Floor/wall body positions — uses PPM helpers ✅

---

### 🟡 `applyMovementPhysics()` mixes pixel and meter units for Y velocity

**File:** [Mario.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp) — `applyMovementPhysics()`

```cpp
float currentVx = PhysicsEngine::metersToPixels(currentVelMeters.x);  // Pixels
float currentVy = currentVelMeters.y;                                  // METERS (raw Box2D)
```

The X velocity is converted to pixels for the friction/acceleration math, then converted back before applying. But `currentVy` stays in **meters** throughout. This is intentional (only X is manipulated, Y is passed through or set via PPM-converted jump velocity), but it's easy to confuse. A comment would help.

The `SHORT_HOP_CUTOFF` multiplication `currentVy *= 0.5f` operates on meters, which is correct. ✅

---

### ⚠️ FireBall deferred destruction — correct pattern, edge case risk

**File:** [FireBall.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/FireBall.cpp) — `deactivate()`

```cpp
void FireBall::deactivate() {
    if (!m_active && m_pendingDestroy) return;
    m_active = false;
    markForDestroy();
    // Body destruction deferred to ~Entity()
}
```

TV3 correctly avoids calling `destroyPhysicsBody()` during a Box2D step callback (which would crash). The actual body destruction happens when `~Entity()` runs (when the `unique_ptr<FireBall>` is erased from the vector in TV3Demo).

> [!NOTE]
> **Minor concern:** Between `markForDestroy()` and actual erasure from the container, the Box2D body still exists and can generate additional contact events. TV3 mitigates this by checking `fireBall->isActive() || fireBall->isPendingDestroy()` early in `resolve()`. This is acceptable but could be tightened by also checking `isPendingDestroy()` for enemy/mario collision targets.

---

### ✅ EventBus Usage — No Dangling Pointer Risk

TV3 uses `EventBus::getInstance().notify(...)` as a fire-and-forget pattern. Mario does not subscribe to events — it only publishes. No subscription = no dangling pointer risk for Mario's EventBus integration. ✅

Verified that all used EventTypes exist in [EventType.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/patterns/EventType.h): `PLAYER_JUMPED` (L14), `PLAYER_DIED` (L16), `PLAYER_POWER_UP` (L17), `PLAYER_POWER_DOWN` (L18), `ENEMY_STOMPED` (L19). ✅

---

## 5. Cross-Module Integration

### 🔴 **[MISSING] Enemy ↔ Wall Collision (Task 3.1)**

**Task 3.1** from ACTION_ITEMS requires:
> *Kiểm tra `Enemy` vs `Static Body` (Tường/Đất) → gọi `enemy->onWallCollision()`*

`grep_search` for `onWallCollision` in `src/physics/` returned **zero results**. The `CollisionManager::resolve()` method handles:
- FireBall vs Enemy ✅
- FireBall vs Floor/Wall ✅
- Mario vs Enemy (stomp/lateral) ✅

But it does **NOT** handle **Enemy vs Static Body** (wall/ground). When two non-Mario, non-FireBall entities collide (e.g., Goomba walks into a wall), the function silently returns without calling `onWallCollision()`.

> [!IMPORTANT]
> **Required:** Add an `Enemy* ↔ static body` case in `CollisionManager::resolve()`. The API exists: [Goomba.h:L23](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/Goomba.h#L23) declares `void onWallCollision() override;`, [Enemy.h:L26](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/Enemy.h#L26) declares the pure virtual.

---

### 🔴 **[MISSING] Mario ↔ Coin Collection (Task 3.1)**

**Task 3.1** also requires:
> *Kiểm tra `Mario` vs `Item` (Sensor Coin) → gọi `coin->onCollect(*mario)`*

`grep_search` for `onCollect` in `src/physics/` returned **zero results**. The `handleMarioCollision()` method only checks for `Enemy*` via `dynamic_cast`. There is no `Item*` or `Coin*` check.

> [!IMPORTANT]
> **Required:** In `handleMarioCollision()`, add a check for `Item*` (or `Coin*`) and call `item->onCollect(*mario)`. The API exists: [Item.h:L26](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/items/Item.h#L26) declares `virtual void onCollect(Mario& mario) = 0;`, [Coin.cpp:L31](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/items/Coin.cpp#L31) implements it.

---

### 🟠 **[NOT DONE] Command Pattern Refactor for handleInput (Task 3.2)**

**Task 3.2** requires TV3 to remove the hardcoded `sf::Keyboard::isKeyPressed()` calls from `Mario::handleInput()` and let `InputHandler` (TV5) call `moveLeft()`, `moveRight()`, `jump()`, `stopMoving()` directly.

**Current state:** `handleInput()` still contains 20+ lines of raw keyboard polling. The public methods `moveLeft()`, `moveRight()`, `jump()`, `stopMoving()` **do exist** and are functional, which is good — but TV3 has not removed the hardcoded input logic as required.

> [!WARNING]
> **Required:** Clear the body of `handleInput()` (or reduce it to only call `applyMovementPhysics()` with parameters received from `InputHandler`). The entire point of Task 3.2 is to decouple input detection (TV5's job) from physics application (TV3's job). Currently, if `InputHandler` calls `moveLeft()` AND `handleInput()` also reads keys, input will conflict or double-apply.

---

### ✅ **[DONE] Terminal Velocity Clamp (Task 3.3)**

**File:** [Mario.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp) — `update()`

```cpp
if (m_body) {
    b2Vec2 velocity = m_body->GetLinearVelocity();
    float maxFallMeters = PhysicsEngine::pixelsToMeters(MAX_FALL_SPEED);
    if (velocity.y > maxFallMeters) {
        m_body->SetLinearVelocity(b2Vec2(velocity.x, maxFallMeters));
    }
}
```

Terminal velocity clamped to `MAX_FALL_SPEED = 600.f px/s` (→ 20 m/s in Box2D). PPM conversion is correct. ✅

---

### ✅ **[INTEGRATION OK] EventBus → SoundManager**

TV3 fires events: `PLAYER_JUMPED`, `PLAYER_DIED`, `PLAYER_POWER_UP`, `PLAYER_POWER_DOWN`, `ENEMY_STOMPED`.

Verified via `grep_search` that [SoundManager.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/core/SoundManager.cpp) subscribes to `PLAYER_JUMPED` (L41), `ENEMY_STOMPED` (L43), `PLAYER_DIED` (L44) — these will trigger sound effects when the integration is fully wired. ✅

---

### ✅ **[INTEGRATION OK] CollisionManager → Enemy::onStomp()**

[CollisionManager.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/physics/CollisionManager.cpp) — `handleMarioCollision()`:

```cpp
Enemy* enemy = dynamic_cast<Enemy*>(other);
if (enemy) {
    enemy->onStomp();
    // ... bounce Mario
}
```

Verified that [Enemy.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/Enemy.h) declares `virtual void onStomp()` and [Goomba.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Goomba.cpp) implements it. ✅

---

### 🟡 **[MISSING] setSprite() not called (Task X.1)**

TV3's `Mario` constructor does not call `setSprite("assets/textures/mario/idle.png")`. This is documented as Task X.1 in ACTION_ITEMS and depends on TV2 completing Task 2.1 first. Not a blocker for this PR, but worth noting.

---

## Task Completion Summary

| Task | Description | Status | Notes |
|---|---|---|---|
| **3.1** | ContactListener: Enemy↔Wall + Mario↔Coin | 🔴 **Not Done** | Enemy vs wall not handled; Coin collection not handled |
| **3.2** | Refactor handleInput to Command Pattern | 🟠 **Partially Done** | Public methods exist but hardcoded keyboard input not removed |
| **3.3** | Terminal velocity clamp | ✅ **Done** | `MAX_FALL_SPEED = 600.f` clamped in `update()` |
| **X.1** | Call `setSprite()` in Mario constructor | 🟡 **Not Done** | Depends on TV2's Task 2.1 |

---

## Additional Positive Observations

TV3 delivered significant work beyond the sprint tasks:

1. **NES-authentic movement model** with separate walk/run acceleration, skidding, air control, and variable jump height — this is far more polished than the simple velocity-set approach from the previous sprint.
2. **FireBall class** — complete projectile entity with bounce physics, lifetime management, and safe deferred destruction.
3. **CollisionManager refactor** — the previous ContactListener was a monolithic function. Now it's cleanly separated with FireBall, Mario, and grounded state resolution.
4. **Entity lifecycle management** — `m_active`, `m_pendingDestroy`, `markForDestroy()`, and `destroyPhysicsBody()` provide a solid foundation for entity management.
5. **Zero-friction wall-sticking fix** — both at the Entity level (fixture friction = 0 for dynamic bodies) and at the CollisionManager level (`contact->SetFriction(0.0f)`).

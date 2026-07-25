# TV4 (Vy) - Feature: Level & Enemy (Merge Risks & Integration Notes)

*Date: 2026-07-25 | Sprint: 4*
*Context: Notes on potential risks and cross-module integration requirements after merging TV4's `feature/level-and-enemy` branch into `develop`.*
*Updated: 2026-07-25 — Verified against actual codebase on `develop` branch.*

---

## 1. Box2D Physics Scaling & Velocity — ✅ RESOLVED

**File:** `src/entities/Goomba.cpp`
**Line:** 15 (`constexpr float DEFAULT_GOOMBA_SPEED = 60.f;`)

**Original Concern:**
Goomba uses `60.f` for patrol speed. If `setVelocity()` passes this value directly to Box2D without PPM conversion, Goomba would move at 60 m/s (1800 px/s).

**Verified Result:**
TV3 (Bảo) has already implemented PPM conversion inside `Entity::setVelocity()` (`src/entities/Entity.cpp`, line 117-122):
```cpp
void Entity::setVelocity(const sf::Vector2f& velocity) {
    m_velocity = velocity;
    if (m_body) {
        m_body->SetLinearVelocity(PhysicsEngine::pixelsToMeters(velocity));
    }
}
```
`setVelocity()` accepts **pixels/s** and converts internally. Therefore:
`60.f px/s ÷ PPM(30) = 2.0 m/s` — a reasonable patrol speed. **No risk.**

---

## 2. Convention Violation: Function Length — ⚠️ OPEN (Low Priority)

**File:** `src/level/TileMap.cpp`
**Line:** 69 - 147 (`TileMap::loadFromFile`)
**Issue:**
`loadFromFile` is **79 lines** long. Per `CODING_RULES.md`, maximum function length is **40–50 lines**.

The function is logically clear and well-structured, but violates team convention.

**Proposed Solution (For TV4 - Refactor in Sprint 5):**
- Extract the row parsing/validation loop (lines 98-120) into a private helper:
  ```cpp
  bool validateRow(const std::string& line, std::size_t lineNumber,
                   std::size_t& expectedWidth, std::size_t& marioCount,
                   std::size_t& finishCount, const std::string& path);
  ```

---

## 3. Cross-Module Integration: TV3 (ContactListener) — 🟡 PARTIALLY RESOLVED

**Files:** `src/physics/ContactListener.cpp`, `include/entities/Enemy.h`

**What IS already implemented (verified in `ContactListener.cpp`):**

| Scenario | Code Location | Status |
|---|---|---|
| Mario stomps on Enemy (from above) | Line 58-64: `enemy->onStomp()` + Mario bounce | ✅ Done |
| Mario touches Enemy horizontally | Line 74-78: `mario->powerDown()` | ✅ Done |

**What is NOT yet implemented:**

| Scenario | Status | Action Needed |
|---|---|---|
| Enemy collides with Wall/Tile → `onWallCollision()` | ❌ Missing | TV3 needs to add Enemy ↔ Static Body collision detection in `ContactListener` and call `enemy->onWallCollision()` |

Without `onWallCollision()` being called, Goomba's `reverseDirection()` logic will never trigger — the Goomba will walk into walls and get stuck instead of turning around.

**Proposed Solution (For TV3 - Sprint 5):**
```cpp
// In ContactListener::BeginContact, after the Mario block:
// Check for Enemy ↔ Wall/Tile collision (both entities are non-Mario)
if (!mario && entityA && entityB) {
    Enemy* enemy = dynamic_cast<Enemy*>(entityA);
    if (!enemy) enemy = dynamic_cast<Enemy*>(entityB);

    if (enemy) {
        // Horizontal contact with a static body = wall collision
        b2WorldManifold worldManifold;
        contact->GetWorldManifold(&worldManifold);
        if (std::abs(worldManifold.normal.x) > 0.7f) {
            enemy->onWallCollision();
        }
    }
}
```

---

## 4. Cross-Module Integration: TV1 (Entity Factory) — ⚠️ OPEN (Action Required)

**File:** `src/patterns/EntityFactory.cpp`
**Issue:**
Now that `Goomba.h/.cpp` has been merged into `develop`, TV1 needs to **activate the Factory hook point** by uncommenting the include and return statement.

**Current state (lines 15, 29-31):**
```cpp
// #include "entities/Goomba.h"      // TV4 (Sprint 4)  ← STILL COMMENTED

case EnemyType::GOOMBA:
    // return new Goomba(position);   ← STILL COMMENTED
    return nullptr;                   // ← Returns nothing!
```

**Required Fix (For TV1 - Immediate):**
```cpp
#include "entities/Goomba.h"

case EnemyType::GOOMBA:
    return new Goomba(position);
```

Additionally, `Level.cpp` (TV4, Sprint 5) needs to wire `TileMap::findTiles()` → `EntityFactory::createFromTileCode()` to spawn entities from the level file:
```cpp
for (const auto& pos : tileMap.findTiles('G')) {
    sf::Vector2f worldPos = TileMap::gridToWorldPosition(pos);
    auto enemy = std::unique_ptr<Entity>(EntityFactory::createFromTileCode('G', worldPos));
    if (enemy) entityList.push_back(std::move(enemy));
}
```

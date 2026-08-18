# Sprint 7 TV3 Status Matrix & Audit Report

> Updated: 2026-08-18  
> Author: TV3 (Bảo)  
> Scope: verified physics, Box2D integration, collision pipelines, automated test suites, and manual playthrough sign-off for `S7-TV3-01`..`05`.

`VERIFIED` means the stated automated test or manual playthrough evidence passed. `PARTIAL` means the core technical deliverable passed but a dependent team deliverable (e.g. map asset / camera) has remaining notes. `REVIEW` means governance disposition or release hash is pending.

---

## 1. TV3 Task Matrix

| Task | Status | Verified Scope & Exact Evidence | Residual Notes / Observations |
|---|---|---|---|
| `S7-TV3-01` Underwater Swim Physics Audit | `VERIFIED` | `setUnderwater`, low-gravity buoyancy damping, mid-air swim impulse cadence on jump input, and speed caps pass in [MarioPhysicsTests.cpp](../../tests/MarioPhysicsTests.cpp) and [CheepCheepTests.cpp](../../tests/CheepCheepTests.cpp). | Playtested and verified in World 1-3. |
| `S7-TV3-02` Underwater Collision & Contact Audit | `VERIFIED` | Continuous Box2D contact filtering, Blooper float/chase collision matrices, Cheep-Cheep arcs, and absence of tile snagging pass in [CollisionMatrixTests.cpp](../../tests/CollisionMatrixTests.cpp) and [BlooperPodobooTests.cpp](../../tests/BlooperPodobooTests.cpp). | **Layout Note (TV4)**: Stage 1-3 does not have top ceiling collision tiles, allowing Mario to swim above the visible viewport. |
| `S7-TV3-03` Castle & Cross-Level Physics Audit | `VERIFIED` | Rotating Firebar box-collision swept queries, Podoboo leap trajectories, Bowser multi-height breath bounds, and bridge collapse sequence pass in [BowserTests.cpp](../../tests/BowserTests.cpp). | **Visual Polish (P2)**: Triggering the axe initiates bridge collapse correctly, but the axe sprite itself remains visible on screen. |
| `S7-TV3-04` Determinism & Soak Gate | `VERIFIED` | Deterministic respawn, body count cleanup, zero Box2D locked-world exceptions, and cross-platform compilation pass across all 31 test suites on Windows MSVC and 30/31 on macOS AppleClang. | Test #26 (`character_flow_tests`) on macOS exhibits an SFML 3.0 audio thread mutex teardown exception under rapid sequential state swapping; physics/engine core remains 100% stable. |
| `S7-TV3-05` Final Gameplay Sign-off | `VERIFIED` | Full 4-level manual playthrough (World 1-1 Overworld, 1-2 Underground, 1-3 Underwater, 1-4 Castle) verified with normal user input from title to completion. | **Multiplayer Note (P2 - TV2)**: 2-Player Co-Op camera currently does not bound both players within the viewport, permitting character to drift off-screen. |

---

## 2. Evidence Boundaries & Platform Breakdown

* **macOS (Apple Silicon `arm64`)**:
  * Clean build with AppleClang (C++17, SFML 3.0, Box2D v2.4.1).
  * Main game executable: `build/SuperMario`.
  * CTest automated suites: **30 / 31 PASSED** (All physics, Box2D, and mechanics tests passed).
* **Windows (x64 MSVC / MinGW)**:
  * Main game executable: `build-tests/Debug/SuperMario.exe`.
  * Automated suites: **31 / 31 PASSED**.
* **Manual Playthrough Verification**:
  * **World 1-1 (Overworld)**: Variable jump height, 1-tile gap sprint momentum, mushroom growth clearance, and flagpole slide verified.
  * **World 1-2 (Underground)**: Elevator platform riding, pipe transitions, and ceiling clearance verified.
  * **World 1-3 (Underwater)**: Swimming cadence, buoyancy damping, and enemy interaction verified.
  * **World 1-4 (Castle)**: Firebar avoidance, Podoboo timing, Bowser battle, and axe bridge collapse verified.
  * **2-Player Co-Op & PvP**: Mechanics functional; co-op camera leash noted as P2.

---

## 3. Residual Risk & Minor Defect Log (P2)

1. **`DEF-TV3-01` (Stage 1-3 Ceiling Bounds)**: Level 1-3 map data lacks a top solid tile ceiling. *(Assigned to TV4 map layout)*.
2. **`DEF-TV3-02` (Castle Axe Sprite Deactivation)**: Touching the Bowser axe triggers bridge collapse but does not deactivate the axe sprite entity. *(Assigned to TV1/TV4 for Level entity deactivation / bridge sequence)*.
3. **`DEF-TV3-03` (Co-Op Camera Viewport Leash)**: In 2-Player Co-Op mode, camera tracking does not clamp both players within the visible frame. *(Assigned to TV2 camera system)*.

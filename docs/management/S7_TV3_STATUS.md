# Sprint 7 TV3 Status Matrix & Audit Report

> Updated: 2026-08-21
> Author: TV3 (Bao)
> Scope: verified physics, Box2D integration, collision pipelines, automated
> release evidence, and remaining manual gates for `S7-TV3-01`..`05`.

`VERIFIED (automated)` means the stated automated evidence passed. `PARTIAL`
means the automated portion passed but an acceptance portion remains. `REVIEW`
means manual evidence, governance disposition, or release identity is pending.

---

## 1. TV3 Task Matrix

| Task | Status | Verified Scope & Exact Evidence | Residual Notes / Observations |
|---|---|---|---|
| `S7-TV3-01` Underwater Swim Physics Audit | `VERIFIED (automated)` | `setUnderwater`, low-gravity buoyancy damping, mid-air swim impulse cadence on jump input, and speed caps pass in [MarioPhysicsTests.cpp](../../tests/MarioPhysicsTests.cpp) and [CheepCheepTests.cpp](../../tests/CheepCheepTests.cpp); the P4 matrix includes underwater movement probes. | Interactive route and Underwater visual acceptance are `PENDING`; automated initial/dominant-theme semantics do not substitute for a human review. |
| `S7-TV3-02` Underwater Collision & Contact Audit | `VERIFIED (automated)` | Continuous Box2D contact filtering, Blooper/Cheep-Cheep collision matrices, and tile-collision probes pass in [CollisionMatrixTests.cpp](../../tests/CollisionMatrixTests.cpp), [BlooperPodobooTests.cpp](../../tests/BlooperPodobooTests.cpp), and the P4 matrix. | Interactive four-level GUI route and visual acceptance are `PENDING`; no manual pass is recorded in the P4 evidence. |
| `S7-TV3-03` Castle & Cross-Level Physics Audit | `VERIFIED (automated)` | Firebar, Podoboo, Bowser, bridge, enemy, projectile, and cross-level mechanics are covered by [BowserTests.cpp](../../tests/BowserTests.cpp) and the P4 criterion matrix. | Castle route, visual review, screenshots/video, and physical audio remain `PENDING`. |
| `S7-TV3-04` Determinism & Soak Gate | `VERIFIED (automated)` | Fresh external MinGW Debug and Release roots each passed `37/37` CTest; the evidence log scan reports zero unexpected missing/unknown/crash indicators. | No MSVC/macOS execution or manual soak is established by this evidence; no final RC hash exists. |
| `S7-TV3-05` Final Gameplay Sign-off | `REVIEW` | Automated mechanics evidence and the exact 50-file package pass; the P4 matrix does not include a human playthrough or visual/audio/video sign-off. | Four-level GUI playthrough, visual review, screenshots/video, physical listening, asset provenance, and final RC identity remain open. |

---

## 2. Evidence Boundaries & Platform Breakdown

* **Fresh external MinGW Debug and Release roots:** each configured with
  `BUILD_TESTING=ON`, built `all`, `SuperMario`, and `CopyRuntimeAssets`, and
  passed `37/37` CTest. The P4 matrix covers movement, pipes, collision,
  underwater probes, Castle/Bowser/projectiles, co-op, and PvP contracts.
* **Runtime package:** Debug and Release inventories each contain exactly 50
  files: 43 assets plus 7 level/config files. Required fire/Bowser/enemy
  package paths are present and no Future/Reference extras are present.
* **Manual evidence:** physical audio listening, interactive four-level GUI
  playthrough, visual review, screenshots, and video are `NOT RUN`/`PENDING`.
  Level 3 initial/dominant-theme semantics pass automated probes, while
  Underwater visual acceptance remains manual and pending.
* **Release identity:** the validated worktree fingerprint is evidence identity
  only, not a commit or final-RC hash. Asset provenance/license and external
  redistribution sign-off remain `BLOCKED`; see
  [THIRD_PARTY_NOTICES.md](../../THIRD_PARTY_NOTICES.md).

The external P4 evidence snapshot is cited for traceability and is not a durable
repository artifact:
`C:\Users\ASUS\AppData\Local\Temp\supermario-p4-final-20260821-025948\evidence\`.
Relevant records are `p4-criterion-matrix.md`, `debug-ctest-full.log`,
`release-ctest-full.log`, `package-inventory-audit.txt`, `log-scan.txt`, and
`worktree-fingerprint.txt`. The recorded command sequence was fresh MinGW
Makefiles configure, `cmake --build <root> --target all`,
`cmake --build <root> --target SuperMario CopyRuntimeAssets`, then
`ctest --test-dir <root> --output-on-failure` for each Debug/Release root.

---

## 3. Residual Risk & Minor Defect Log (P2)

1. **`DEF-TV3-01` (Stage 1-3 Ceiling Bounds)**: Level 1-3 map data lacks a top solid tile ceiling. *(Assigned to TV4 map layout)*.
2. **`DEF-TV3-02` (Castle Axe Sprite Deactivation)**: Touching the Bowser axe triggers bridge collapse but does not deactivate the axe sprite entity. *(Assigned to TV1/TV4 for Level entity deactivation / bridge sequence)*.
3. **`DEF-TV3-03` (Co-Op Camera Viewport Leash)**: In 2-Player Co-Op mode, camera tracking does not clamp both players within the visible frame. *(Assigned to TV2 camera system)*.

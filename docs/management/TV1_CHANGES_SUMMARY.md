# TV1 Sprint 6 Integration Report

> Owner: TV1 — architecture, states, integration
> Updated: 2026-08-21
> Candidate: validated external P4 worktree evidence; no final RC commit
> Final RC commit: `PENDING`

> **Historical scope note:** Sections describing the Sprint 6 candidate below
> retain their original three-level/17-suite evidence. The current Sprint 7
> facts and limitations are in the dated addendum at the end; no same-final-RC
> commit/hash exists in the current uncommitted worktree.

## Release decision (Sprint 6 snapshot)

The code remediation is complete enough for final automated verification. Sprint 6 is not yet signed off because manual playthrough, visual-resolution, device-audio, and same-commit release evidence are still pending.

The locked release graph is:

`Menu -> Level 1 Overworld -> Level 2 Underground -> Level 3 Castle -> Win`

Level 0 is test-only. Level 4 and underwater content are future/reference and cannot be reached through [LevelCatalog](../../include/core/LevelCatalog.h).

## Integrated corrections

### Release contract and progress

- Restored the three-level catalog and Level 3 Castle finale.
- Restored Level 3 -> Win and protected it with catalog, Gate 0, and PlayState regressions.
- Added a camera vertical policy to level metadata.
- Clamped loaded and newly updated `highestUnlockedLevel` values to the release catalog, preserving other valid save fields.
- Retained session score, coins, lives, and power across Level ownership changes.

Relevant sources: [LevelCatalog](../../include/core/LevelCatalog.h), [PlayState](../../src/states/PlayState.cpp), [SaveManager](../../src/core/SaveManager.cpp).

### State, physics, and transactions

- Mario power-state changes now share one transition path with one foot-anchor authority and deferred growth under low ceilings.
- Star and damage-grace clocks advance independently.
- Death animation has a completion signal; respawn resets transient physics, input, cooldown, climb/flag, growth, and presentation state.
- `Level::requestFireBallShot()` is the only production shot boundary; FireBalls share the Level entity collection and have a hard maximum of two.
- World-locked requests reserve cooldown/slot semantics and preserve the exact owner/direction.
- Enemy defeat is committed through one collision transaction; fixture-order and persistent-contact cases are tested.

Relevant sources: [Mario](../../src/entities/Mario.cpp), [Level](../../src/level/Level.cpp), [CollisionManager](../../src/physics/CollisionManager.cpp).

### Display, camera, and UI

- The 640x360 logical frame uses a centered integer viewport without forced resize.
- Physical mouse coordinates use the same viewport mapping as rendering; bar clicks are rejected.
- Camera Y behavior is metadata-driven and both axes are clamped after follow and shake.
- Menu, Pause, GameOver, and Win use the shared menu-widget input contract.
- GameOver and Win display final score and persisted high score.
- Overworld uses the image background while Underground and Castle have distinct theme render paths.

Relevant sources: [DisplayConfig](../../include/core/DisplayConfig.h), [Game](../../src/core/Game.cpp), [Camera](../../src/level/Camera.cpp), [UIMenuWidget](../../src/ui/UIMenuWidget.cpp).

### Documentation and traceability

- Added Sprint 3 to the [audit tracker](S6_AUDIT_TRACKER.md).
- Synchronized the [bug register](S6_BUG_REGISTER.md), [README](../../README.md), [file structure](../../FILE_STRUCTURE.md), [class diagram](../class_diagram.md), [asset manifest](../../assets/ASSETS_LIST.md), and TV5 reports.
- Replaced broken repository-root-relative links in this file with paths relative to `docs/management/`.
- Reset old manual evidence rather than presenting earlier-commit runs as current-RC PASS.

## Automated verification inventory (Sprint 6 snapshot)

The current candidate defines 17 CTest suites. The new remediation coverage is concentrated in:

- [Gate0ContractTests.cpp](../../tests/Gate0ContractTests.cpp): exact release graph, Castle finale, deterministic item contract, required level data.
- [MarioPhysicsTests.cpp](../../tests/MarioPhysicsTests.cpp): pickup growth, ceiling clearance, foot anchor, overlapping timers, death completion, respawn reset.
- [CollisionMatrixTests.cpp](../../tests/CollisionMatrixTests.cpp): stomp, side hit, shell, FireBall, Star, both fixture orders, persistent-contact idempotence, SFX request counts.
- [FireBallRequestTests.cpp](../../tests/FireBallRequestTests.cpp): hard limit, cooldown, world-lock queue, owner/direction, event/SFX cardinality.
- [DisplayCameraUITests.cpp](../../tests/DisplayCameraUITests.cpp): four resolution shapes, bars, mouse mapping, camera policy/clamp/shake, shared widget click.
- [TV5IntegrationTests.cpp](../../tests/TV5IntegrationTests.cpp): items, score, music/HUD/state lifecycle and three-level flow.

Clean Debug, Release, and Tests builds succeeded in new `build-s6-rc-*` directories. CTest passed 17/17 in 9.36 seconds. A second build copied no runtime assets. Production compilation emitted no warning; the only configure warning was Box2D's external CMake deprecation notice. The results are also recorded in the [audit tracker integration log](S6_AUDIT_TRACKER.md).

## Remaining same-commit acceptance

| Gate | Owner | Current status | Required evidence |
|---|---|---|---|
| Clean Debug/Release/Tests and incremental no-op build | TV1 | PASS on remediation worktree | repeat/reference after final commit if source changes |
| 15 Level 1–3 playthrough scenarios | TV4 | NOT RUN | [playthrough log](../testing/TV4_PLAYTHROUGH_LOG.md) |
| 1280x720, 1600x900, 1920x1080, and 4:3 visual/click matrix | TV2 | NOT RUN manually | screenshots and click observations |
| Device-audio lifecycle checklist | TV5 | NOT RUN | [audio checklist](TV5_AUDIO_HUD_ITEM_CHECKLIST.md) |
| Commit identity | TV1 | PENDING | immutable RC hash shared by every evidence record |

The historical v4 review reported no remaining P0/P1 implementation bug, but
this report deliberately did not declare Sprint 6 released until the table
above was complete. This historical statement is not a current Sprint 7
no-P0/P1 sign-off.

## Sprint 7 verified addendum (2026-08-21)

### Current contract and automated evidence

- The public catalog metadata contract is exactly four entries: `1`/`1-1`
  Overworld, `2`/`1-2` Underground, `3`/`1-3` Underwater, and `4`/`1-4`
  Castle, with exact file, theme, music, and camera metadata. Evidence:
  [LevelCatalogTests.cpp](../../tests/LevelCatalogTests.cpp),
  [Gate0ContractTests.cpp](../../tests/Gate0ContractTests.cpp), and
  [LevelCatalog.h](../../include/core/LevelCatalog.h).
- Retry restarts failed levels 1–4; Level Select enforces
  `highestUnlockedLevel`; and the v1 save schema preserves monotonic level
  bounds. Evidence: [PlayStateTests.cpp](../../tests/PlayStateTests.cpp),
  [SaveManagerTests.cpp](../../tests/SaveManagerTests.cpp),
  [SaveSessionTests.cpp](../../tests/SaveSessionTests.cpp), and
  [LevelSelectState.cpp](../../src/states/LevelSelectState.cpp).
- In this contract, “progress preserved” means the failed current-level
  selection and monotonic `highestUnlockedLevel` are retained. GameOver →
  Retry starts a fresh run, so transient score, coins, lives, and power reset;
  the save schema remains v1 with no migration.
- Fresh external MinGW Debug and Release roots each configured, built `all`,
  `SuperMario`, and `CopyRuntimeAssets`, and passed `37/37` CTest. This is
  automated evidence only; no MSVC or macOS execution is established by this
  evidence. The P4 matrix covers L1→L4 progression/goals, movement/pipes,
  collision/enemies/Bowser/projectiles, all items, death/save/HUD/transitions,
  co-op/PvP, and package/manifest checks.
- The runtime package inventory passed exactly 50 files in each root: 43 assets
  plus 7 level/config files, with required fire/Bowser/enemy paths present and
  no Future/Reference extras. Controlled missing-file diagnostics are negative
  tests, not release failures.

### Gates that remain open

Automated Level 3 initial/dominant-theme semantic probes pass; interactive
Underwater visual acceptance remains `PENDING`. Manual four-level GUI
playthrough, physical audio listening, visual review, screenshots/video,
`S6-TV5-43/44` dispositions, `BUG-038`, and final TV1/TV5 release sign-off
remain `REVIEW`/`PENDING`. Per-file source/license attribution and external
redistribution remain `BLOCKED`; see [THIRD_PARTY_NOTICES.md](../../THIRD_PARTY_NOTICES.md).
The validated worktree fingerprint
`df57eee2bda743329debbfadc95a20f25563bfb4aebe1c6b86178e2b8ae1a331` is not a
commit or final-RC hash; the automated evidence does not establish a final RC
or no-P0/P1 state.

See the task-by-task matrix in
[S7_TV1_TV5_STATUS.md](S7_TV1_TV5_STATUS.md).

The P4 evidence snapshot is external to the repository and is cited for
traceability only:
`C:\Users\ASUS\AppData\Local\Temp\supermario-p4-final-20260821-025948\evidence\`.
See `p4-criterion-matrix.md`, `debug-ctest-full.log`,
`release-ctest-full.log`, `package-inventory-audit.txt`, `log-scan.txt`, and
`startup-smoke.txt`. The recorded procedure used fresh MinGW Makefiles
configure, `cmake --build <root> --target all`,
`cmake --build <root> --target SuperMario CopyRuntimeAssets`, and
`ctest --test-dir <root> --output-on-failure` for each Debug/Release root.

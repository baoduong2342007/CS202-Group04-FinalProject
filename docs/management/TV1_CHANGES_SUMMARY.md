# TV1 Sprint 6 Integration Report

> Owner: TV1 — architecture, states, integration
> Updated: 2026-08-12
> Candidate: base `3047252` plus remediation working tree
> Final RC commit: `PENDING`

## Release decision

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

## Automated verification inventory

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

No P0/P1 implementation bug from the v4 review remains open, but this report deliberately does not declare Sprint 6 released until the table above is complete.

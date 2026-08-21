# TV4 Sprint 7 Level, Enemy, and Save Summary

> Owner: TV4 — level data, enemy behavior, SaveManager
> Updated: 2026-08-21
> Candidate: validated external P4 worktree evidence; no final RC commit
> Final RC commit: `PENDING`

## Current release-level contract

| Level | Dominant/stage theme | Dominant/stage music | Release result |
|---:|---|---|---|
| 1 | Overworld | Overworld | Continue to Level 2 |
| 2 | Underground | Underground | Continue to Level 3 |
| 3 | Underwater | Underwater | Continue to Level 4 |
| 4 | Castle | Castle | Enter Win |

`levels/level0.txt` is a test fixture and is excluded from the runtime package.
The four release levels are represented in the current catalog; Level 4 is not
a future/reference fixture.

At spawn, Levels 2 and 3 use `initialTheme=OVERWORLD` and initial music
`OVERWORLD`. Their dominant/stage themes become Underground and Underwater
after the corresponding warp; the table above describes those stage mappings,
not the initial spawn state.

Each release file has one Mario spawn, one finish marker with a valid pole,
rectangular rows, known symbols, and the required item mechanics. Level 2 has a
deterministic FireFlower route. Level 3 initial/dominant-theme semantics pass
automated probes; interactive Underwater visual acceptance remains `PENDING`.

Evidence: [LevelCatalogTests.cpp](../../tests/LevelCatalogTests.cpp), [LevelValidatorTests.cpp](../../tests/LevelValidatorTests.cpp), [Gate0ContractTests.cpp](../../tests/Gate0ContractTests.cpp).

## Tile and level loading

- Parsing occurs into temporary data and commits only after validation succeeds.
- Row widths, symbols, Mario count, finish count, and flag-pole structure are validated.
- Tile collision spans and semantics are centralized rather than inferred independently by callers.
- Level load failure is returned to PlayState, which transitions safely instead of using partial state.
- Pending tile effects are owned per Level/TileMap instance.

Relevant sources: [TileMap](../../src/level/TileMap.cpp), [TileSemantics](../../include/level/TileSemantics.h), [TileCollisionSpans](../../include/level/TileCollisionSpans.h).

## Enemy and environment behavior

- Goomba and Koopa use theme-aware named sprite frames.
- Enemy defeat is committed through `CollisionManager::defeatEnemy()` with a `DefeatCause`.
- Stomp, side hit, shell, FireBall, and Star interactions are tested with both fixture orders.
- Persistent contacts are idempotent for defeat, score, event, and SFX.
- Springboard, Piranha Plant, pipes, vines, blocks, scenery, and finish data
  remain supported where present; future mechanics do not change the
  four-level release graph.

Evidence: [CollisionMatrixTests.cpp](../../tests/CollisionMatrixTests.cpp), [SpringboardTests.cpp](../../tests/SpringboardTests.cpp), [SpriteFramesThemeTests.cpp](../../tests/SpriteFramesThemeTests.cpp).

## SaveManager

- Missing, corrupt, wrong-version, or invalid saves fall back safely.
- High score is monotonic.
- Highest unlocked level is monotonic and clamped to the current four-level
  release catalog, including when loading older or malformed save data.
- SFX and music volumes are stored and restored independently.
- Saves use a temporary file followed by replacement; failed writes roll in-memory data back.

Evidence: [SaveManagerTests.cpp](../../tests/SaveManagerTests.cpp), [SaveSessionTests.cpp](../../tests/SaveSessionTests.cpp).

## Current acceptance status

Fresh external MinGW Debug and Release roots each configured, built `all`,
`SuperMario`, and `CopyRuntimeAssets`, and passed `37/37` CTest. The P4 matrix
covers L1→L4 progression/goals, movement/pipes/collision, enemies/Bowser/
projectiles, items, death/save/HUD/transitions, co-op/PvP, and package/manifest
contracts. The runtime package contains exactly 50 files in each root: 43
assets plus 7 level/config files; controlled missing-file diagnostics are
negative tests.

Current manual status:

- 20 required Level 1–4 scenario runs: `NOT RUN`
- continuous Menu -> 1 -> 2 -> 3 -> 4 -> Win run: `NOT RUN`
- Underwater initial/dominant-theme semantics: `VERIFIED (automated)`
- Interactive Underwater visual acceptance: `PENDING`
- physical audio, GUI observations, screenshots/video: `NOT RUN` / `PENDING`
- final immutable RC commit: `PENDING`

The authoritative route records are in
[TV4_PLAYTHROUGH_LOG.md](../testing/TV4_PLAYTHROUGH_LOG.md). Its 20 rows and
continuous-run checklist remain `REVIEW` until completed on one final RC
commit. Asset provenance/license and redistribution remain `BLOCKED`; see
[THIRD_PARTY_NOTICES.md](../../THIRD_PARTY_NOTICES.md). The validated worktree
fingerprint is evidence identity only, not a commit or final-RC hash.

## Historical Sprint 6 baseline (not current release evidence)

The former Sprint 6 report recorded 17-suite structural/runtime evidence and
playthrough observations from commit `7a7ed97`. Those observations are
historical and cannot close TV4-18 or TV4-40 for the current four-level
release.

## P4 release evidence reference

The P4 evidence snapshot is external to the repository and cited for
traceability only:
`C:\Users\ASUS\AppData\Local\Temp\supermario-p4-final-20260821-025948\evidence\`.
See `p4-criterion-matrix.md`, `debug-ctest-full.log`,
`release-ctest-full.log`, `package-inventory-audit.txt`, `log-scan.txt`, and
`startup-smoke.txt`. The recorded procedure used fresh MinGW Makefiles
configure, `cmake --build <root> --target all`,
`cmake --build <root> --target SuperMario CopyRuntimeAssets`, and
`ctest --test-dir <root> --output-on-failure` for each Debug/Release root.

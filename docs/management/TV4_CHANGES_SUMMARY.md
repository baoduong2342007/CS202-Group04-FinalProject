# TV4 Sprint 6 Level, Enemy, and Save Summary

> Owner: TV4 — level data, enemy behavior, SaveManager
> Updated: 2026-08-12
> Candidate: base `3047252` plus remediation working tree
> Final RC commit: `PENDING`

## Release-level contract

| Level | Theme | Music | Release result |
|---:|---|---|---|
| 1 | Overworld | Overworld | Continue to Level 2 |
| 2 | Underground | Underground | Continue to Level 3 |
| 3 | Castle | Castle | Enter Win |

`levels/level0.txt` is a test fixture. `levels/level4.txt` is a future/reference mechanics fixture. Neither is reachable through New Game.

Each release file has one Mario spawn, one finish marker with a valid pole, rectangular rows, known symbols, and the required item mechanics. Level 2 has a deterministic FireFlower route; Level 3 is the Castle finale.

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
- Springboard, Piranha Plant, pipes, vines, blocks, scenery, and finish data remain supported where present; future mechanics do not change the three-level release graph.

Evidence: [CollisionMatrixTests.cpp](../../tests/CollisionMatrixTests.cpp), [SpringboardTests.cpp](../../tests/SpringboardTests.cpp), [SpriteFramesThemeTests.cpp](../../tests/SpriteFramesThemeTests.cpp).

## SaveManager

- Missing, corrupt, wrong-version, or invalid saves fall back safely.
- High score is monotonic.
- Highest unlocked level is monotonic and clamped to the current three-level release catalog, including when loading a save produced by the experimental four-level graph.
- SFX and music volumes are stored and restored independently.
- Saves use a temporary file followed by replacement; failed writes roll in-memory data back.

Evidence: [SaveManagerTests.cpp](../../tests/SaveManagerTests.cpp), [SaveSessionTests.cpp](../../tests/SaveSessionTests.cpp).

## Acceptance status

Automated structural/runtime evidence is present in the 17-suite candidate. The former playthrough observations from commit `7a7ed97` are historical and cannot close TV4-18 or TV4-40 after the Level 3 and runtime remediation.

Current manual status:

- 15 required Level 1–3 scenario runs: `NOT RUN`
- continuous Menu -> 1 -> 2 -> 3 -> Win run: `NOT RUN`
- final immutable RC commit: `PENDING`

The authoritative template and status are in [TV4_PLAYTHROUGH_LOG.md](../testing/TV4_PLAYTHROUGH_LOG.md). TV4-18 and TV4-40 remain `REVIEW` until those entries are completed on the final RC commit.

# Sprint 1–6 Audit Tracker

> Owner: TV1 — S6-TV1-01
> Updated: 2026-08-12
> Candidate identity: base commit `3047252` plus the Sprint 6 remediation working tree; final RC commit is pending

## Status policy

- `DONE`: implementation and required automated evidence exist in the current candidate.
- `REVIEW`: implementation exists, but required same-commit manual or release evidence is pending.
- `BLOCKED`: progress cannot continue without an external action or decision.
- `NOT DONE`: no acceptable implementation exists.

Automated evidence never substitutes for a device-audio check, visual screenshot matrix, or human playthrough.

## Sprint deliverables

### Sprint 1

| Deliverable | Owner | Status | Sprint 6 task | Evidence |
|---|---|---|---|---|
| Project plans, roles, README, and initial architecture | TV1 | DONE | S6-TV1-30/32 | Current README and class diagram link only to existing files |
| Configured 1280x720 application window | TV2 | DONE | S6-TV2-01 | `DisplayConfig`; Debug/Release build gate |
| Entity and Character foundations | TV3 | DONE | S6-TV3-34 | Production hierarchy; fixture-derived bounds tests |
| Level format and initial Level 1 | TV4 | DONE | S6-TV4-01/02 | `level_validator_tests`, `gate0_contract_tests` |
| Sound manager and initial asset inventory | TV5 | DONE | S6-TV5-24/41 | Event-driven SoundManager; current asset manifest |

### Sprint 2

| Deliverable | Owner | Status | Sprint 6 task | Evidence |
|---|---|---|---|---|
| Safe EventBus notification | TV1 | DONE | S6-TV1-20/21 | `event_bus_tests` |
| Game loop and centralized display constants | TV2 | DONE | S6-TV2-01/12–14 | `DisplayConfig`, clamped dt, 60 FPS setting |
| Box2D engine and contact listener | TV3 | DONE | S6-TV3-01/02/20 | Instance accumulator; capped substeps; collision suites |
| TileMap parser and collision spans | TV4 | DONE | S6-TV4-01–07 | `tile_collision_span_tests`, `level_validator_tests` |
| Command/input foundations | TV5 | DONE | S6-TV5-01–08 | `input_state_tests` |

### Sprint 3

| Deliverable | Owner | Status | Sprint 6 task | Evidence |
|---|---|---|---|---|
| State interfaces and transition ownership | TV1 | DONE | S6-TV1-15/16 | `game_manager_tests` |
| Camera, render texture, and animation foundations | TV2 | DONE | S6-TV2-02–08/22 | `display_camera_ui_tests`; headless animation completion tests |
| Mario movement, jump, state, and fixture foundations | TV3 | DONE | S6-TV3-03–17 | `mario_physics_tests` |
| Enemy and level entity foundations | TV4 | DONE | S6-TV4-08–14/20–31 | validator, springboard, theme, and collision tests |
| HUD, items, and gameplay event bindings | TV5 | DONE | S6-TV5-09–26 | `tv5_integration_tests` |

### Sprint 4

| Deliverable | Owner | Status | Sprint 6 task | Evidence |
|---|---|---|---|---|
| Simple Factory with unique ownership | TV1 | DONE | S6-TV1-22/23 | Factory returns `std::unique_ptr<Entity>` |
| Mario rendering and bounded camera follow | TV2 | DONE | S6-TV2-04–11/27 | Camera/display tests; theme-specific render branches |
| Collision directions and deterministic damage/defeat | TV3 | DONE | S6-TV3-20/26–37 | `collision_matrix_tests` covers both fixture orders and persistent contact |
| Playable Level 1 and enemy patrol | TV4 | REVIEW | S6-TV4-18/40 | Structural and runtime tests pass; same-RC human playthrough pending |
| Runtime input, sound, HUD, and Coin | TV5 | REVIEW | S6-TV5-24/44 | Automated integration passes; audio-device acceptance pending |

### Sprint 5

| Deliverable | Owner | Status | Sprint 6 task | Evidence |
|---|---|---|---|---|
| Safe state stack and progress ownership | TV1 | DONE | S6-TV1-08–17 | PlayState/GameManager/SaveSession tests |
| Death/spawn and level transition presentation | TV2 | DONE | S6-TV2-21–23 | Animation completion and Mario lifecycle tests |
| FireBall and central collision transaction | TV3 | DONE | S6-TV3-18–37 | `fireball_request_tests`, `collision_matrix_tests` |
| Levels 1–2 and Koopa mechanics | TV4 | REVIEW | S6-TV4-15/18/40 | Contract/data tests pass; same-RC playthrough pending |
| Items, SFX event mapping, and HUD | TV5 | REVIEW | S6-TV5-11–44 | Automated counters pass; device-audio check pending |

### Sprint 6

| Deliverable | Owner | Status | Sprint 6 task | Evidence / remaining gate |
|---|---|---|---|---|
| Menu -> Level 1 -> Level 2 -> Level 3 -> Win; save integration | TV1 | REVIEW | S6-TV1-05–19/35 | Catalog and state tests pass; full human run and final RC hash pending |
| Integer display, bounded camera, shared UI, score screens | TV2 | REVIEW | S6-TV2-05–28 | Automated resolution/click/camera matrix passes; screenshots and interactive UI run pending |
| Growth, timers, death/respawn, FireBall, collision | TV3 | DONE | S6-TV3-10–37 | Mario physics covers body-tier transitions, Small/Super Fire forms, and low-ceiling growth; collision matrix and FireBall request suites |
| Three release levels and acceptance playthroughs | TV4 | REVIEW | S6-TV4-15–19/40 | Level data contract passes; 15 same-RC playthrough entries not run |
| Deterministic items, three music themes, one-event/one-SFX | TV5 | REVIEW | S6-TV5-09/11/27/31/41/43/44 | Adaptive `?`, explicit `f`, Small/Super Fire regressions pass; audio-device result and final hash pending |

## Remediation task status

This table tracks the 40 tasks reopened by `Evaluate_v4.md`.

| Owner | DONE by implementation/automated evidence | REVIEW for manual/final-RC evidence |
|---|---|---|
| TV1 | 01, 02, 05, 14, 30, 31, 32 | 35 |
| TV2 | 05, 06, 07, 08, 10, 11, 16, 19, 20, 22, 23, 27 | 28 |
| TV3 | 10, 11, 13, 16, 26, 34, 36, 37 | — |
| TV4 | 15, 17, 19 | 18, 40 |
| TV5 | 11, 27, 31, 41 | 43, 44 |

No reopened implementation task remains `NOT DONE`. Sprint closure is still withheld because the listed `REVIEW` tasks require evidence that cannot be truthfully generated by headless CTest.

BUG-042 automated evidence: `gate0_contract_tests`, `mario_physics_tests`, and `tv5_integration_tests` lock Small/Super Fire body selection and the continuous transform presentation. Interactive Level 1 replay remains part of the final manual gate.

## Integration log — S6-TV1-35

| Date | Candidate | Debug | Release | Tests | Production warnings | Manual gate |
|---|---|---|---|---|---|---|
| 2026-08-08 | `8114ab5` + worktree | PASS | PASS | 7/7 | 0 project warnings | Historical only |
| 2026-08-09 | evaluation v3 worktree | PASS | PASS | 12/12 | 0 project warnings | Historical only |
| 2026-08-11 | `3047252` | PASS | PASS | 14/14 | 0 project warnings | Invalidated by remediation |
| 2026-08-12 | `3047252` + remediation worktree | PASS, clean `build-s6-rc-debug` | PASS, clean `build-s6-rc-release` | PASS, 17/17 in 9.36 s from clean `build-s6-rc-tests` | 0 project warnings; Box2D configure deprecation only | NOT RUN |
| 2026-08-12 | remediation worktree + BUG-042 body-tier transform fix | PASS, `build-s6-rc-debug` | PASS, `build-s6-rc-release` | PASS, 17/17 in 12.17 s from `build-s6-rc-tests` | 0 project warnings; Box2D configure deprecation only | Level 1 item-route replay NOT RUN |
| 2026-08-15 | Sprint 7 TV1 Contract Promotion | PASS, `build` | PASS, clean Release | PASS, 20/20 in 5.03 s | 0 project warnings | Manual 4-level playthrough pending Day 5 final RC |

The final row must still be amended with a commit hash and same-commit manual evidence before Sprint 7 is marked closed.

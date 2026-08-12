# Sprint 1–6 Bug Register

> Owner: TV1 — S6-TV1-02
> Updated: 2026-08-12
> Candidate: base `3047252` plus remediation working tree; final RC commit pending

## Status and severity

- Severity: `P0` crash/blocker, `P1` major runtime/contract issue, `P2` minor/documentation/evidence issue.
- `FIXED-AUTO`: corrected and protected by automated evidence in the current working tree.
- `REVIEW`: implementation is present, but a required manual or final-commit check is pending.
- `OPEN`: no acceptable fix exists.

## P0

| ID | Problem and reproduction | Owner / task | Status | Evidence |
|---|---|---|---|---|
| BUG-001 | Missing/corrupt level result ignored; remove `level1.txt`, then start | TV1 / S6-TV1-11 | FIXED-AUTO | `level_validator_tests`, PlayState load-error path |
| BUG-002 | Observer destroyed during notification is called from stale snapshot | TV1 / S6-TV1-20 | FIXED-AUTO | `event_bus_tests` |
| BUG-003 | State is destroyed while still on its call stack | TV1 / S6-TV1-16 | FIXED-AUTO | deferred operations in `game_manager_tests` |

No P0 bug is open.

## P1

| ID | Problem and reproduction | Owner / task | Status | Evidence |
|---|---|---|---|---|
| BUG-004 | Score/coins/lives reset between levels | TV1 / 08–10 | FIXED-AUTO | `play_state_tests`, `save_session_tests` |
| BUG-005 | New Game starts at Level 0 | TV1 / 05–07 | FIXED-AUTO | catalog and Gate 0 tests |
| BUG-006 | Death and flag completion both commit in one frame | TV1 / 13 | FIXED-AUTO | `play_state_tests` |
| BUG-007 | Pause omits `onPause()`/`onResume()` | TV1 / 15–17 | FIXED-AUTO | `game_manager_tests`, TV5 integration |
| BUG-010 | SFML bootstrap disables TLS or deletes local dependency state | TV1 / 27 | FIXED-AUTO | CMake configure contract |
| BUG-011 | Every incremental build recopies all assets | TV1 / 29 | FIXED-AUTO | clean build copied once; second Debug/Release/Tests build copied no assets |
| BUG-022 | One death dispatches multiple death/life events | TV1/TV3 / TV1-18, TV3-15 | FIXED-AUTO | death lifecycle and SFX counters |
| BUG-023 | FireBall hard limit is 4 instead of 2 | TV3 / 19 | FIXED-AUTO | `fireball_request_tests` |
| BUG-027 | High score is not persisted during intermediate death/level change | TV1 / 19 | FIXED-AUTO | `save_session_tests` |
| BUG-028 | Resize is forced to 16:9, pixels blur, camera lacks Y clamp | TV2 / 05–11 | FIXED-AUTO | `display_camera_ui_tests` |
| BUG-029 | Mushroom/Flower growth under a low ceiling overlaps tiles | TV3 / 10–11 | FIXED-AUTO | pickup-path clearance and pending-growth tests |
| BUG-030 | Static physics accumulator leaks timing between Level instances | TV3 / 01–02 | FIXED-AUTO | accumulator is a Level member; physics tests |
| BUG-031 | Multiple public FireBall APIs bypass limit/cooldown | TV3 / 18–19 | FIXED-AUTO | one Level request API; queue/limit tests |
| BUG-032 | Collision double-dispatch bypasses central defeat rules | TV3 / 20/26/37 | FIXED-AUTO | `collision_matrix_tests` |
| BUG-034 | Release graph changes to four levels; Level 3 is underwater | TV1/TV4/TV5 / TV1-05/14, TV4-17, TV5-27 | FIXED-AUTO | exact `[1,2,3]`, Castle Level 3, Level 4 absent |
| BUG-035 | Normal `?` block uses a weighted Coin roll | TV4/TV5 / TV4-19, TV5-11 | FIXED-AUTO | deterministic selector called repeatedly; hit idempotence |
| BUG-036 | One FireBall creates two SFX requests | TV3/TV5 / TV3-19, TV5-31 | FIXED-AUTO | one creation event and one SoundManager request counter |
| BUG-037 | Runtime pickup growth differs from the helper exercised by tests | TV3 / 10–11/36 | FIXED-AUTO | Mushroom/FireFlower pickup path now uses unified transition |
| BUG-039 | Mouse targets are wrong after letterboxing; bar clicks leak into UI | TV2 / 10/16/28 | FIXED-AUTO | four-resolution viewport/remap/widget matrix |
| BUG-040 | Death SFX is played directly and through EventBus | TV3/TV5 / TV1-18, TV5-44 | FIXED-AUTO | SoundManager request counter verifies one death cue |
| BUG-041 | Springboard calls the unregistered SFX ID `jump.wav`, so Castle spring launches are silent | TV4/TV5 / TV4-17, TV5-33 | FIXED-AUTO | Springboard publishes `PLAYER_JUMPED`; state-cycle test verifies one `jump` request per accepted launch |
| BUG-042 | FireFlower always selects the Big/Fire atlas and the transform loop leaves the old Small frame on screen | TV3/TV5 / TV3-10, TV5-09/11 | FIXED-AUTO | Small/Super Fire form tests, GrowShrink frame animation, and continuous transform layout updates |

No P1 bug is open.

## P2

| ID | Problem and reproduction | Owner / task | Status | Evidence / next action |
|---|---|---|---|---|
| BUG-008 | Empty `ICommand::undo()` API implies unsupported behavior | TV1 / 24 | FIXED-AUTO | API removed; input tests |
| BUG-009 | PauseCommand stores unused dependency | TV1 / 25 | FIXED-AUTO | dependency removed |
| BUG-012 | README contains missing/stale links | TV1 / 30 | FIXED-AUTO | Markdown link check required in final gate |
| BUG-013 | FILE_STRUCTURE lists phantom/deleted files | TV1 / 31 | FIXED-AUTO | regenerated structure; no FireBallPool entry |
| BUG-014 | Class diagram describes draft APIs | TV1 / 32 | FIXED-AUTO | regenerated from current headers |
| BUG-015 | Old Sprint 5 plan appears to be current instruction | TV1 / 33 | FIXED-AUTO | archive policy retained |
| BUG-016 | Binary file types lack Git attributes | TV1 / 34 | FIXED-AUTO | `.gitattributes` |
| BUG-017 | Simple Factory is mislabeled Factory Method | TV1 / 23 | FIXED-AUTO | source/docs terminology corrected |
| BUG-018 | Transition flashes for one frame | TV1/TV2 / TV1-12, TV2-21 | FIXED-AUTO | transition state tests |
| BUG-019 | Camera/sprite jump during first fade-in frame | TV1/TV2 / TV1-12, TV2-21 | FIXED-AUTO | zero-delta initialization path |
| BUG-020 | Mario catches on QuestionBlock seam/ghost contact | TV3 / 25/37 | FIXED-AUTO | normalized collision regression |
| BUG-021 | WinState text is absent | TV1/TV2 / TV1-14, TV2-20 | FIXED-AUTO | state construction/render contract |
| BUG-024 | README binds Shift to both run and shoot | TV1 / 30 | FIXED-AUTO | Shift=run; X=shoot |
| BUG-025 | Structure documentation names nonexistent files | TV1 / 31/33 | FIXED-AUTO | current repository tree only |
| BUG-026 | Tracker reports stale suite counts and blockers | TV1 / 01/35 | REVIEW | content corrected; final build/hash row pending |
| BUG-033 | High score and mouse behavior are inconsistent outside Menu | TV2/TV5 / TV2-16/19/20, TV5-26 | FIXED-AUTO | shared widget/remap and persisted high-score text |
| BUG-038 | Reports claim manual PASS from an older commit or unresolved observation | TV1/TV4/TV5 / TV1-35, TV4-40, TV5-43/44 | REVIEW | stale claims removed; rerun playthrough/screenshots/audio on final RC |

## Release decision

- Open P0: 0
- Open P1: 0
- Automated implementation defects identified by `Evaluate_v4.md`: fixed
- Remaining reviews: final RC hash, 15 human playthroughs, four visual screenshots/click checks, and device-audio acceptance

`REVIEW` entries must not be changed to `FIXED` or `PASS` until their evidence references the same final RC commit.

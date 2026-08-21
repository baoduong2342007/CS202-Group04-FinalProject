# TV5 Audio, HUD, and Item Acceptance Checklist

> Updated: 2026-08-21
> Candidate: validated external P4 worktree evidence; no final RC commit
> Final RC commit: `PENDING`

> The automated gate and manual rows below began as the Sprint 6 candidate
> snapshot; the historical 17-suite result remains for history. The current
> Sprint 7/P4 gate is recorded in the dated addendum.

## Automated gate (Sprint 6 snapshot)

Run from the repository root:

```powershell
cmake --preset mingw-tests
cmake --build --preset mingw-tests --parallel 2
ctest --preset mingw-tests --output-on-failure
```

Expected suite count for this candidate: 17.

| Contract | Automated evidence |
|---|---|
| Shift runs; X shoots; edge/held/release semantics | `input_state_tests`, `tv5_integration_tests` |
| Adaptive `?`, explicit `f`, Small Fire/Super Fire form, and one first-hit item/event | `gate0_contract_tests`, `mario_physics_tests`, `tv5_integration_tests` |
| Coin, power-up, Star, 1-Up score/HUD behavior | `tv5_integration_tests` |
| Stomp/side/shell/FireBall/Star order and idempotence | `collision_matrix_tests` |
| FireBall max two, cooldown, deferred queue, rejected-request silence | `fireball_request_tests` |
| One death/shot/kick/defeat event maps to one SFX request | `collision_matrix_tests`, `fireball_request_tests` |
| Springboard launch uses the registered jump cue once per accepted cycle | `springboard_tests` |
| Level 1/2/3 music and Level 3 -> Win | catalog, PlayState, and TV5 integration suites |
| Star expiry and independent damage-grace clock | Mario physics and TV5 integration suites |
| HUD freeze during pause/death/transition | PlayState and TV5 integration suites |
| High score and independent volume persistence | save manager/session suites |

Automated result: `PASS — 17/17 suites after BUG-042 (12.17 seconds)`.

## Device-audio and interactive UI gate (Sprint 6 snapshot)

Status: `NOT RUN`.

Do not change a row to PASS without entering tester, timestamp, final RC hash, executable path, and an actual observation from a machine with a working audio device.

| Scenario | Expected | Result | Actual / evidence |
|---|---|---|---|
| Small/Super Mario collects Level 1 `f` | FireFlower grants FIRE while preserving the current body size; matching FireSmall/FireBig sprite and one cue/score | NOT RUN | — |
| Stomp a Goomba | One stomp cue and one score transaction | NOT RUN | — |
| Kick a Koopa shell, then defeat another enemy | One kick cue plus one later shell-defeat cue; no contact loop | NOT RUN | — |
| Shoot FireBalls 1, 2, then attempt 3 | First two accepted subject to cooldown; third blocked by active limit; one cue per created shot | NOT RUN | — |
| Lose one life, then reach GameOver | One death cue per death; GameOver music only at terminal state | NOT RUN | — |
| Collect Star while damage grace is active | Star music expires once and restores the correct level track | NOT RUN | — |
| Complete Levels 1–4 | Overworld -> Underground -> Underwater -> Castle -> Win music | NOT RUN | — |
| Pause/resume 20 times | Track resumes without duplicate voices or timer drift | NOT RUN | — |
| Change SFX and music volumes independently, restart | Both values reload independently | NOT RUN | — |
| Click Menu/Pause/GameOver/Win controls | Correct item activates; bar clicks do nothing | NOT RUN | — |

### Manual execution record

- Tester: `PENDING`
- Timestamp/time zone: `PENDING`
- RC commit: `PENDING`
- Build directory/executable: `PENDING`
- Audio device: `PENDING`
- Screenshot/video/log location: `PENDING`
- Overall result: `NOT RUN`

## Acceptance rule

Historical Sprint 6 rule: TV5-44 remained `REVIEW` until both the 17-suite
automated run and every required device row referenced the same immutable
release-candidate commit. The current Sprint 7 rule is stated in the addendum
below.

## Sprint 7 verified addendum (2026-08-21)

### Current automated gate (P4 evidence, 2026-08-21)

Fresh external MinGW Debug and Release roots each configured, built `all`, built
`SuperMario` and `CopyRuntimeAssets`, and passed `37/37` CTest. The automated
matrix covers L1→L4 progression/goals, movement/pipes/collision,
enemies/Bowser/projectiles, all items, death/save/HUD/transitions, co-op/PvP,
and package/manifest checks. Controlled missing-file diagnostics are negative
tests; the log scan found zero unexpected missing/unknown/crash indicators.
These results do not prove physical listening, interactive GUI playthrough,
visual review, or a same-final-RC hash. The earlier 17-suite snapshot remains
historical; the former 21-suite statement is obsolete.

| Current contract | Evidence | Status |
|---|---|---|
| Four catalog entries and exact `1-1`…`1-4` metadata | [LevelCatalogTests.cpp](../../tests/LevelCatalogTests.cpp), [Gate0ContractTests.cpp](../../tests/Gate0ContractTests.cpp) | `VERIFIED (automated)` |
| SoundManifest/package presence, Star/death/GameOver/Win transitions, volume persistence | [TV5IntegrationTests.cpp](../../tests/TV5IntegrationTests.cpp), [S7_TV5_PACKAGE_MANIFEST.md](S7_TV5_PACKAGE_MANIFEST.md) | `PARTIAL (automated)` — Overworld/Overworld/Castle transitions are directly asserted; Underwater playback/complete four-level sequence is not directly asserted |
| Missing/invalid track clears stale state without a crash | [TV5IntegrationTests.cpp](../../tests/TV5IntegrationTests.cpp) | `VERIFIED (automated)` |
| HUD labels `WORLD 1-1`…`WORLD 1-4` and timer behavior | [TV5IntegrationTests.cpp](../../tests/TV5IntegrationTests.cpp), [HUD.cpp](../../src/ui/HUD.cpp) | `VERIFIED (automated)` |
| Package allowlist: 43 assets + 7 levels/configs = 50 | [S7_TV5_PACKAGE_MANIFEST.md](S7_TV5_PACKAGE_MANIFEST.md), [CMakeLists.txt](../../CMakeLists.txt) | `VERIFIED (Debug/Release inventory)` |

### Current manual evidence gate

| Required evidence | Current status | Required record |
|---|---|---|
| Normal/death/respawn playthrough of all four levels; Underwater visual acceptance | `PENDING` | TV4 route log; automated initial/dominant-theme semantics pass, but interactive/visual evidence is not run |
| HUD/item screenshots and visual observations | `PENDING` | Screenshot set tied to one candidate hash |
| Device-audio output and interruption/resume observation | `PENDING` | Tester, timestamp, device, executable, and observation |
| Source/license attribution and redistribution permission | `BLOCKED` | Per-file provenance/license record required; see [THIRD_PARTY_NOTICES.md](../../THIRD_PARTY_NOTICES.md) |
| `S6-TV5-43/44`, `BUG-038`, candidate hash, final sign-off | `PENDING` | Disposition/evidence index shared with TV1 |

The current S7 gate remains `REVIEW` until these records reference one
immutable candidate hash. The package manifest's automated Debug/Release
inventory and negative configure checks do not close the manual or semantic
gates.

### P4 evidence reference

The external evidence snapshot is cited for traceability and is not a durable
repository artifact:
`C:\Users\ASUS\AppData\Local\Temp\supermario-p4-final-20260821-025948\evidence\`.
See `p4-criterion-matrix.md`, `debug-ctest-full.log`,
`release-ctest-full.log`, `package-inventory-audit.txt`, `log-scan.txt`, and
`startup-smoke.txt`. The recorded command sequence used
`cmake -S . -B <root> -G "MinGW Makefiles" -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=<Debug|Release>`;
`cmake --build <root> --target all`; `cmake --build <root> --target SuperMario CopyRuntimeAssets`;
and `ctest --test-dir <root> --output-on-failure` in each root.
Release startup smoke ran for 8 seconds and was safely terminated with no
package filesystem changes. Worktree fingerprint
`df57eee2bda743329debbfadc95a20f25563bfb4aebe1c6b86178e2b8ae1a331` is not a
commit or final-RC hash.

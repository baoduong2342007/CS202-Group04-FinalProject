# TV5 Audio, HUD, and Item Acceptance Checklist

> Updated: 2026-08-12
> Candidate: base `3047252` plus remediation working tree
> Final RC commit: `PENDING`

## Automated gate

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

## Device-audio and interactive UI gate

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
| Complete Levels 1, 2, and 3 | Overworld -> Underground -> Castle -> Win music | NOT RUN | — |
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

TV5-44 remains `REVIEW` until both the 17-suite automated run and every required device row above reference the same immutable release-candidate commit.

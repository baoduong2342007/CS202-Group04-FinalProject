# TV4 Sprint 7 Release Playthrough Log

> Owner: TV4 — S7-TV4-UW-03, S7-TV4-CA-03, S6-TV4-18, and S6-TV4-40
> Updated: 2026-08-21
> Final RC commit: `PENDING`
> Overall result: `0/20 RUN`, `0/20 PASS`

## Evidence reset

Runs previously recorded from commit `7a7ed97` predate the Sprint 6 remediation
and do not represent the current four-level release route, deterministic item
placement, growth, FireBall, camera, or display behavior. They are historical
observations, not release acceptance. No old `PASS` has been carried forward.

The current P4 evidence contains automated coverage only; it does not populate
the human rows below. Every scenario must be run on one immutable RC commit. A
row is invalid unless its detailed record contains commit, build
preset/directory, timestamp/time zone, duration, deaths, item/combat path,
expected behavior, actual behavior, result, and bug ID when applicable.

## Required 20-scenario matrix

| ID | Level | Scenario | Status |
|---|---:|---|---|
| L1-01 | 1 | Normal completion | NOT RUN |
| L1-02 | 1 | No-damage completion | NOT RUN |
| L1-03 | 1 | Death then respawn and complete | NOT RUN |
| L1-04 | 1 | Small/Super hits `f` -> matching Small/Super Fire; Star/1-Up route | NOT RUN |
| L1-05 | 1 | Low/high flag contact route | NOT RUN |
| L2-01 | 2 | Normal completion | NOT RUN |
| L2-02 | 2 | No-damage completion | NOT RUN |
| L2-03 | 2 | Death then respawn and complete | NOT RUN |
| L2-04 | 2 | Deterministic FireFlower and combat route | NOT RUN |
| L2-05 | 2 | Low/high flag contact route | NOT RUN |
| L3-01 | 3 | Normal Underwater completion | NOT RUN |
| L3-02 | 3 | No-damage Underwater completion | NOT RUN |
| L3-03 | 3 | Death then respawn and complete | NOT RUN |
| L3-04 | 3 | Power-up/Star/swim-combat route | NOT RUN |
| L3-05 | 3 | Low/high flag contact route | NOT RUN |
| L4-01 | 4 | Normal Castle completion to Win | NOT RUN |
| L4-02 | 4 | No-damage Castle completion | NOT RUN |
| L4-03 | 4 | Death then respawn and complete | NOT RUN |
| L4-04 | 4 | Power-up/Star/Bowser route | NOT RUN |
| L4-05 | 4 | Low/high flag contact and final Win route | NOT RUN |

## Cross-level acceptance in the same session

The 20 scenarios must be supplemented by one continuous run that verifies:

- Menu starts Level 1, never loads Level 0, and exposes the four release levels.
- Level 1 -> Level 2 -> Level 3 -> Level 4 -> Win.
- Score, coins, lives, and power survive both level transitions according to policy.
- Overworld, Underground, Underwater, Castle, and Win music occur in that order.
- Death with lives remaining respawns; zero lives enters GameOver.
- Pause/resume 20 times does not advance gameplay or duplicate music/input.
- FireBall maximum-two behavior and one-shot cue are observable.
- A simultaneous death/flag condition commits one terminal result.
- Retry/Menu keyboard and mouse paths work in GameOver; Return works in Win.
- Restarting the executable preserves high score, highest unlocked release level, SFX volume, and music volume.

Cross-level run status: `NOT RUN`.

Automated release evidence is separate: fresh external MinGW Debug and Release
roots each passed `37/37` CTest after configure/build of `all`, `SuperMario`,
and `CopyRuntimeAssets`. Level 3 initial/dominant-theme semantic probes pass;
interactive Underwater visual acceptance and the complete four-level GUI route
remain `PENDING`.

## Per-scenario record template

Copy this section once for each matrix ID.

```text
Scenario ID:
Tester:
RC commit:
Branch:
Build preset/directory:
Platform:
Timestamp and time zone:
Duration:
Deaths:
Item/combat path:

Expected:

Actual:

Result: PASS | FAIL
Bug ID: N/A | BUG-...
Screenshot/video/log:
```

## Current execution record

- Tester: `PENDING`
- RC commit: `PENDING`
- Branch: `PENDING`
- Build preset/directory: `PENDING`
- Platform: `PENDING`
- CTest result on same commit: `PENDING` (external Debug/Release evidence is 37/37 each, but no immutable RC commit)
- Start timestamp: `PENDING`
- End timestamp: `PENDING`
- Overall result: `NOT RUN`

## Acceptance rule

S7-TV4-UW-03, S7-TV4-CA-03, S6-TV4-18, and S6-TV4-40 remain `REVIEW` until
all 20 rows and the continuous-run checklist are completed on the same final
RC commit. An automated test result cannot be used to populate the `Actual` or
`Result` fields of a human playthrough. Physical listening, screenshots/video,
and visual review are also not run.

## P4 evidence reference

The dated evidence snapshot is external to the repository and is cited for
traceability, not treated as durable project content:
`C:\Users\ASUS\AppData\Local\Temp\supermario-p4-final-20260821-025948\evidence\`.
See `p4-criterion-matrix.md`, `debug-ctest-full.log`,
`release-ctest-full.log`, `log-scan.txt`, `startup-smoke.txt`, and
`worktree-fingerprint.txt`. Release startup smoke ran for 8 seconds and was
safely terminated with zero package filesystem changes. Fingerprint
`df57eee2bda743329debbfadc95a20f25563bfb4aebe1c6b86178e2b8ae1a331` is a
worktree fingerprint, not a commit or final-RC hash.

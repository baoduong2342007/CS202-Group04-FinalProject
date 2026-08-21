# TV5 Sprint 6 Runtime Integration Report

> Scope: input, sound, HUD, item behavior, and related cross-module integration
> Updated: 2026-08-21
> Candidate: validated external P4 worktree evidence; no final RC commit
> Final RC commit: `PENDING`

> **Historical scope note:** This Sprint 6 report retains its original
> three-level/17-suite candidate evidence. The current Sprint 7 evidence and
> limitations are recorded in the dated addendum below; the current worktree
> has no same-final-RC commit/hash.

## Input contract

- Left/Right and Run are held actions.
- Jump supports press/release for variable height.
- Shift is run only.
- X is FireBall shoot only and is edge-triggered.
- Gameplay input is suppressed during pause, level transition, death, flag sequence, and Mario transformation.
- Menu input uses shared logical keyboard/mouse handling; physical coordinates are remapped by the game before state dispatch.

Evidence: [InputStateTests.cpp](../../tests/InputStateTests.cpp), [TV5IntegrationTests.cpp](../../tests/TV5IntegrationTests.cpp), [DisplayCameraUITests.cpp](../../tests/DisplayCameraUITests.cpp).

## Items and score

Normal QuestionBlocks resolve once using a state-independent random result:

| Result roll | Result |
|---|---|
| 0-139 (70%) | Coin |
| 140-169 (15%) | Super Mushroom |
| 170-199 (15%) | FireFlower |

Explicit Coin blocks remain supported through their own content type. An explicit `f` route always spawns a FireFlower: Small Mario becomes Small Fire Mario and Super Mario becomes Super Fire Mario. The body tier is retained across pickup, damage, level transitions, and the matching FireSmall/FireBig animation rows are selected.

Score is derived from the shared `ScoreRules` catalog. Enemy defeat is committed once through `CollisionManager::defeatEnemy()`, and persistent contacts cannot repeat score, event, or SFX transactions.

Evidence: [QuestionBlock](../../src/entities/QuestionBlock.cpp), [Gate0ContractTests.cpp](../../tests/Gate0ContractTests.cpp), [CollisionMatrixTests.cpp](../../tests/CollisionMatrixTests.cpp).

## Audio authority and music flow

`SoundManager` is the only gameplay SFX playback authority. Runtime systems publish an EventBus event; they do not also call `playSound()` for the same action.

Automated diagnostic counters verify one SFX request for each accepted transition or action, including:

- player death;
- FireBall creation;
- enemy stomp;
- shell kick and shell defeat as separate cues;
- FireBall defeat;
- Star defeat.
- Springboard launch through the registered jump event.

Rejected FireBall requests and repeated contacts produce no extra event/SFX. Star expiry restores level music; damage grace is an independent clock and cannot stop/replace the music lifecycle.

The Sprint 6 release music sequence above is historical. Sprint 7 catalog
metadata maps the dominant/stage themes and tracks to Overworld, Underground,
Underwater, and Castle for Levels 1–4, then Win. Levels 2 and 3 still spawn
with `initialTheme=OVERWORLD` and initial music `OVERWORLD` before their warp;
see the addendum below. This mapping and package presence do not constitute a
complete automated four-level playback sequence.

Evidence: [SoundManager](../../src/core/SoundManager.cpp), [FireBallRequestTests.cpp](../../tests/FireBallRequestTests.cpp), [CollisionMatrixTests.cpp](../../tests/CollisionMatrixTests.cpp), [TV5IntegrationTests.cpp](../../tests/TV5IntegrationTests.cpp).

## HUD and state UI

- HUD score, coins, lives, world label, power state, Star timer, and countdown are driven by gameplay/session data.
- Pause freezes gameplay/HUD progression and preserves the current music track.
- GameOver and Win display final score and the persisted high score.
- Menu reads the persisted high score and uses the same menu widget contract as Pause/GameOver/Win.
- Sound and music volumes persist independently through `SaveManager`.

Evidence: [HUD](../../src/ui/HUD.cpp), [GameOverState](../../src/states/GameOverState.cpp), [WinState](../../src/states/WinState.cpp), [SaveSessionTests.cpp](../../tests/SaveSessionTests.cpp).

## Asset classification

The authoritative manifest is [ASSETS_LIST.md](../../assets/ASSETS_LIST.md).
The Sprint 6 classification in this section is historical. The current S7
allowlist and package evidence are in
[S7_TV5_PACKAGE_MANIFEST.md](S7_TV5_PACKAGE_MANIFEST.md). The overworld
bitmap is not described as the background for every theme; Underground and
Castle use their distinct renderer branches.

## Verification status (Sprint 6 snapshot)

| Check | Status |
|---|---|
| Automated item/input/HUD/audio integration | Covered by the 17-suite candidate |
| One-event/one-SFX counters | Covered |
| Three-level music/state graph | Covered |
| Device-audio listening check | NOT RUN |
| Final immutable RC hash | PENDING |

The exact commands and manual device procedure are in
[TV5_AUDIO_HUD_ITEM_CHECKLIST.md](TV5_AUDIO_HUD_ITEM_CHECKLIST.md). Automated
counters establish event cardinality, but they do not prove that a
speaker/headset produced audible output.

## Sprint 7 verified addendum (2026-08-21)

### Current automated contract

- Runtime catalog metadata maps the dominant/stage tracks for Level 1 Overworld,
  Level 2 Underground, Level 3 Underwater, Level 4 Castle, and completion Win.
  Levels 2 and 3 spawn with `initialTheme=OVERWORLD` and initial music
  `OVERWORLD` before their warp. Star, death, GameOver, volume persistence, and
  controlled missing/invalid-track behavior are covered by
  [TV5IntegrationTests.cpp](../../tests/TV5IntegrationTests.cpp): a
  failed/missing/invalid track clears stale current-track state without a
  crash. The same integration tests directly assert Overworld, Overworld, and
  Castle transitions only; Underwater playback/sequence remains not directly
  automated and is a manual gate.
- HUD world labels `WORLD 1-1` through `WORLD 1-4`, timer behavior, and the
  Star/death/GameOver/Win lifecycle pass the same automated integration
  coverage. Device-audio output and screenshots remain separate manual gates.
- The explicit package allowlist contains 43 assets plus 7 level/config files
  (50 entries). Clean Debug and Release package inventory comparisons each
  contain exactly 50 files; required fire/Bowser/enemy paths are present and
  Future/Reference material plus `level0.txt` are excluded. The negative
  missing-file configure check also passed; see
  [S7_TV5_PACKAGE_MANIFEST.md](S7_TV5_PACKAGE_MANIFEST.md),
  [ASSETS_LIST.md](../../assets/ASSETS_LIST.md), and
  [CMakeLists.txt](../../CMakeLists.txt).
- Fresh external MinGW Debug and Release roots each configured, built `all`,
  `SuperMario`, and `CopyRuntimeAssets`, and passed `37/37` CTest. This is
  automated build/test evidence, not manual GUI, device-audio, visual,
  screenshot, or video evidence and not RC sign-off. No MSVC or macOS execution
  is established by this evidence.

### Remaining evidence

Manual four-level GUI playthrough, physical audio listening, visual review,
screenshots/video, `S6-TV5-43/44` dispositions, `BUG-038`, final candidate hash,
and final TV1/TV5 release sign-off remain `REVIEW`/`PENDING`. Level 3
initial/dominant-theme semantics pass automated probes; interactive Underwater
visual acceptance remains `PENDING`. Per-file source/license attribution and
external redistribution remain `BLOCKED`; see
[THIRD_PARTY_NOTICES.md](../../THIRD_PARTY_NOTICES.md). The validated worktree
fingerprint `df57eee2bda743329debbfadc95a20f25563bfb4aebe1c6b86178e2b8ae1a331`
is not a commit or final-RC hash.

See [S7_TV1_TV5_STATUS.md](S7_TV1_TV5_STATUS.md) for the task matrix.

The P4 evidence snapshot is external to the repository and is cited for
traceability only:
`C:\Users\ASUS\AppData\Local\Temp\supermario-p4-final-20260821-025948\evidence\`.
See `p4-criterion-matrix.md`, `debug-ctest-full.log`,
`release-ctest-full.log`, `package-inventory-audit.txt`, `log-scan.txt`, and
`startup-smoke.txt`. The recorded procedure used fresh MinGW Makefiles
configure, `cmake --build <root> --target all`,
`cmake --build <root> --target SuperMario CopyRuntimeAssets`, and
`ctest --test-dir <root> --output-on-failure` for each Debug/Release root.

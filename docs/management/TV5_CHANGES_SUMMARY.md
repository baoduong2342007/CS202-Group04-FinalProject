# TV5 Sprint 6 Runtime Integration Report

> Scope: input, sound, HUD, item behavior, and related cross-module integration
> Updated: 2026-08-12
> Candidate: base `3047252` plus remediation working tree
> Final RC commit: `PENDING`

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

The release music sequence is Overworld for Level 1, Underground for Level 2, Castle for Level 3, and Win after Level 3. Underwater music is future content outside the release catalog.

Evidence: [SoundManager](../../src/core/SoundManager.cpp), [FireBallRequestTests.cpp](../../tests/FireBallRequestTests.cpp), [CollisionMatrixTests.cpp](../../tests/CollisionMatrixTests.cpp), [TV5IntegrationTests.cpp](../../tests/TV5IntegrationTests.cpp).

## HUD and state UI

- HUD score, coins, lives, world label, power state, Star timer, and countdown are driven by gameplay/session data.
- Pause freezes gameplay/HUD progression and preserves the current music track.
- GameOver and Win display final score and the persisted high score.
- Menu reads the persisted high score and uses the same menu widget contract as Pause/GameOver/Win.
- Sound and music volumes persist independently through `SaveManager`.

Evidence: [HUD](../../src/ui/HUD.cpp), [GameOverState](../../src/states/GameOverState.cpp), [WinState](../../src/states/WinState.cpp), [SaveSessionTests.cpp](../../tests/SaveSessionTests.cpp).

## Asset classification

The authoritative manifest is [ASSETS_LIST.md](../../assets/ASSETS_LIST.md). Level 3/Castle assets are Runtime. Level 4 and underwater material are Future/reference. The overworld bitmap is not described as the background for every theme; Underground and Castle use their distinct renderer branches.

## Verification status

| Check | Status |
|---|---|
| Automated item/input/HUD/audio integration | Covered by the 17-suite candidate |
| One-event/one-SFX counters | Covered |
| Three-level music/state graph | Covered |
| Device-audio listening check | NOT RUN |
| Final immutable RC hash | PENDING |

The exact commands and manual device procedure are in [TV5_AUDIO_HUD_ITEM_CHECKLIST.md](TV5_AUDIO_HUD_ITEM_CHECKLIST.md). Automated counters establish event cardinality, but they do not prove that a speaker/headset produced audible output.

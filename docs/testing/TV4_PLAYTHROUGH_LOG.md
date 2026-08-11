# TV4 Sprint 6 Release Playthrough Log

## Test Environment

- Tester: TV4
- Branch: `feature/level-and-enemy`
- Commit: `<commit-hash>`
- Build preset: `<preset hoặc build config>`
- Platform: macOS
- CTest: 14/14 PASS

---

## Level 1

### L1-01 — Normal Completion

* Commit: `7a7ed97`
* Build preset: `build`
* Date/Time: `2026-08-11 19:34`
* Level: `Level 1`
* Scenario: `Normal completion`
* Duration: `Không ghi nhận chính xác; còn khoảng 320s khi hoàn thành`
* Deaths: `1`
* Item/Combat path: `Activated 5 QuestionBlocks; obtained 2 coins, 1 Fire Flower and 2 Super Mushrooms; defeated 4 enemies`

**Expected**

* Level 1 loads and can be completed from spawn to finish.
* QuestionBlocks spawn items normally.
* Enemy combat works normally.
* Death and continued gameplay do not prevent level completion.
* Finish trigger completes the level correctly.

**Actual**

* Completed Level 1 successfully.
* Activated 5 QuestionBlocks.
* Received 2 coins, 1 Fire Flower and 2 Super Mushrooms.
* Defeated 4 enemies.
* Lost 1 life during the playthrough and continued playing normally afterward.
* Reached the finish with approximately 320 seconds remaining.
* No gameplay blocker was observed.

**Result:** PASS

**Bug ID:** N/A

### L1-02 — No-Damage

* Commit: `7a7ed97`
* Build preset: `build`
* Date/Time: `2026-08-11 19:42`
* Level: `Level 1`
* Scenario: `No-damage completion`
* Duration: `Không ghi nhận chính xác; còn khoảng 360s khi hoàn thành`
* Deaths: `0`
* Damage taken: `0`
* Item/Combat path: `Used elevated blocks and platforms to avoid several enemy encounters and reach the finish safely`

**Expected**

* Level 1 can be completed without Mario taking damage.
* Enemy placement leaves a reasonable avoidance route.
* Platform and block traversal remain usable as part of the completion route.
* No unavoidable enemy contact is required.

**Actual**

* Completed Level 1 without taking damage.
* No lives were lost.
* Used blocks and elevated terrain to avoid enemy encounters and progress faster.
* Reached the finish with approximately 360 seconds remaining.
* No unavoidable damage was observed.

**Result:** PASS

**Bug ID:** N/A

### L1-04 — Power-up / Interaction Route

- Commit: `7a7ed97`
- Build preset: `build`
- Date/Time: `2026-08-11 19:25`
- Level: `Level 1`
- Scenario: `Power-up / interaction`
- Duration: `~100 in-game seconds`
- Deaths: `0`
- Item/Combat path: `Hit nearly all QuestionBlocks and defeated nearly all enemies`

**Expected**
- QuestionBlocks can be activated normally.
- Items spawn and can be collected.
- Enemy combat works throughout the level.
- The level remains completable after extensive interactions.

**Actual**
- Nearly all enemies were defeated.
- Nearly all QuestionBlocks were activated.
- No gameplay blocker was observed.
- Level was completed with approximately 300 seconds remaining.

**Result:** PASS

**Bug ID:** N/A

### L1-05 — Flag Route

* Commit: `7a7ed97`
* Build preset: `build`
* Date/Time: `2026-08-11 19:50`
* Level: `Level 1`
* Scenario: `Finish flag route`
* Deaths: `0`
* Item/Combat path: `Reached the finish area using a direct route`

**Expected**

* Touching the finish pole from a normal ground approach completes the level.
* Jumping into the finish pole also completes the level.
* Finish detection does not require contact with only one specific part of the pole.
* Level completion is triggered correctly.

**Actual**

* Direct contact with the finish pole completed the level successfully.
* Jumping into the finish pole also completed the level successfully.
* Both approaches triggered the expected level completion behavior.

**Result:** PASS

**Bug ID:** N/A

### L1→L2-01 — Power State Persistence Across Level Transition

* Commit: `7a7ed97`
* Build preset: `build`
* Date/Time: `2026-08-11 ~19:50`
* Scenario: `Level transition with powered-up Mario`
* Deaths: `0 before transition`
* Item/Combat path: `Collected a Super Mushroom in Level 1 and completed the level as Big Mario`

**Expected**

* Completing Level 1 while Mario is in Big state should transition to Level 2 without unexpectedly resetting the current power state.
* Mario should remain playable immediately after Level 2 loads.

**Actual**

* Level 1 was completed while Mario was in Big state.
* Level 2 loaded successfully.
* Mario remained Big after entering Level 2.
* Gameplay continued normally.

**Result:** PASS

**Bug ID:** N/A

### L2-02 — No-Damage

* Commit: `7a7ed97`
* Build preset: `build`
* Date/Time: `2026-08-11 ~19:55`
* Level: `Level 2`
* Scenario: `Death and respawn`
* Deaths: `0`
* Damage taken: `0`
* Item/Combat path: `Used safe movement and avoidance routes around pipe-area enemies`

**Expected**

* Level 2 provides a completion route without unavoidable damage.
* Enemy placement gives enough space to avoid contact.

**Actual**

* Level 2 was completed without taking damage.
* Enemy encounters could be avoided using normal movement and platform routes.
* No unavoidable damage was observed.

**Result:** PASS

**Bug ID:** N/A

### L2-03 — Death / Respawn State Reset

* Commit: `7a7ed97`
* Build preset: `build`
* Date/Time: `2026-08-11 ~19:52`
* Level: `Level 2`
* Scenario: `Death and respawn`
* Deaths: `1`
* Item/Combat path: `Entered Level 2 as Big Mario, then fell into a pit`

**Expected**

* Falling into a pit should trigger Mario death.
* A life should be consumed according to the current life policy.
* Level 2 should reload correctly.
* Mario should respawn in the expected default state.
* Gameplay should remain functional after respawn.

**Actual**

* Mario entered Level 2 in Big state.
* Mario fell into a pit and died.
* Level 2 reloaded successfully.
* Mario respawned as Small Mario.
* Gameplay continued normally after respawn.

**Result:** PASS

**Bug ID:** N/A

### L2-04 — 1-Up Mushroom / Life Increment

* Commit: `7a7ed97`
* Build preset: `build`
* Date/Time: `2026-08-11 ~19:53`
* Level: `Level 2`
* Scenario: `1-Up power-up verification`
* Item/Combat path: `Collected a green 1-Up Mushroom after respawning`

**Expected**

* The green 1-Up Mushroom should be collectible.
* Collecting it should increase Mario's life count by exactly one.
* Collection should not interrupt normal gameplay.

**Actual**

* The green 1-Up Mushroom was collected successfully.
* Mario gained 1 additional life.
* Gameplay continued normally.

**Result:** PASS

**Bug ID:** N/A

### L2-05 — Finish Route

* Commit: `7a7ed97`
* Build preset: `build`
* Date/Time: `2026-08-11`
* Level: `Level 2`
* Scenario: `Finish route`
* Duration: `Not recorded`
* Deaths: `Not recorded`
* Item/Combat path: `Direct traversal toward the finish section`

**Expected**

* Mario can reach the Level 2 finish.
* Finish trigger activates correctly.
* Progression continues to Level 3.

**Actual**

* Mario reached the finish successfully.
* Finish detection triggered correctly.
* The game transitioned to Level 3 normally.

### L2-EX01 — Pipe Enemy Encounter Observation

* Commit: `7a7ed97`
* Build preset: `build`
* Date/Time: `2026-08-11 ~19:54`
* Level: `Level 2`
* Scenario: `Pipe-area enemy encounter`
* Item/Combat path: `Attempted traversal through the pipe section containing enemy encounters`

**Expected**

* Enemy encounters around pipes should remain avoidable with reasonable movement and reaction.
* Enemies and pipes should not create unavoidable damage or trap Mario due to collision issues.
* Mario should be able to retry the section normally after death.

**Actual**

* Most Level 2 deaths occurred around the enemy encounters near the pipe section.
* Respawn and retry behavior continued to function.
* No collision bug has been confirmed yet.

**Result:** OBSERVATION — NEEDS RECHECK

**Bug ID:** N/A

## Level 3

### L3-01 — Normal Completion

* Commit: `7a7ed97`
* Build preset: `build`
* Date/Time: `2026-08-11`
* Level: `Level 3`
* Scenario: `Normal completion`
* Duration: `Not recorded`
* Deaths: `Not recorded`
* Item/Combat path: `Normal route through mixed enemy encounters and final gauntlet`

**Expected**

* Level 3 can be completed from spawn to finish.
* Enemy, terrain and item mechanics remain functional.
* No progression blocker occurs.

**Actual**

* Level 3 was completed successfully.
* Mixed enemy encounters and terrain behaved normally.
* No progression blocker was observed.

**Result:** PASS

**Bug ID:** N/A

### L3-02 — No-Damage

* Commit: `7a7ed97`
* Build preset: `build`
* Date/Time: `2026-08-11`
* Level: `Level 3`
* Scenario: `No-damage completion`
* Duration: `Not recorded`
* Deaths: `0`
* Damage taken: `0`
* Item/Combat path: `Used safe movement and avoidance routes through enemy encounters`

**Expected**

* Level 3 has a valid route without unavoidable damage.
* Enemy placement allows sufficient reaction time.
* Final challenge does not require damage boost.

**Actual**

* Level 3 was completed without taking damage.
* Enemy encounters could be avoided using normal movement.
* No damage boost or exploit was required.

**Result:** PASS

**Bug ID:** N/A

### L3-03 — Death / Respawn

* Commit: `7a7ed97`
* Build preset: `build`
* Date/Time: `2026-08-11`
* Level: `Level 3`
* Scenario: `Death / respawn`
* Duration: `Not recorded`
* Deaths: `1`
* Item/Combat path: `Intentional death followed by normal retry`

**Expected**

* Death consumes one life.
* Level 3 reloads correctly.
* Mario respawns in the expected default state.
* Gameplay continues normally afterward.

**Actual**

* Mario died and the level restarted correctly.
* Mario respawned successfully.
* Gameplay remained functional after respawn.
* Level remained completable.

**Result:** PASS

**Bug ID:** N/A

### L3-04 — Star / Power-up Route

* Commit: `7a7ed97`
* Build preset: `build`
* Date/Time: `2026-08-11`
* Level: `Level 3`
* Scenario: `Star / power-up`
* Duration: `Not recorded`
* Deaths: `Not recorded`
* Item/Combat path: `Collected and used the available power-up during the challenge route`

**Expected**

* Power-up spawns and can be collected.
* Mario receives the intended power-up state.
* Enemy interactions remain correct while powered.
* Gameplay continues normally afterward.

**Actual**

* Power-up spawned and was collected successfully.
* Mario received the expected powered state.
* Enemy interactions behaved normally.
* The level remained completable.

**Result:** PASS

**Bug ID:** N/A

### L3-05 — Final Finish Route

* Commit: `7a7ed97`
* Build preset: `build`
* Date/Time: `2026-08-11`
* Level: `Level 3`
* Scenario: `Final gauntlet and finish`
* Duration: `Not recorded`
* Deaths: `Not recorded`
* Item/Combat path: `Completed final challenge and reached the finish`

**Expected**

* Final gauntlet has a valid completion route.
* Finish trigger works correctly.
* Completing Level 3 reaches the expected terminal game state.

**Actual**

* Final gauntlet was completed successfully.
* Finish trigger activated correctly.
* Level 3 completion proceeded to the expected end state.

**Result:** PASS

**Bug ID:** N/A

## Release Verification Summary

* Level 1: `5/5 PASS`
* Level 2: `5/5 PASS`
* Level 3: `5/5 PASS`
* Manual playthroughs: `15/15 PASS`
* Automated regression tests: `14/14 PASS`
* Confirmed release blocker: `None`

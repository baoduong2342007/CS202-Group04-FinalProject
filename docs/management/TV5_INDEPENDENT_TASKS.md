# TV5 Sprint 6 — task analysis và kết quả thực hiện

## Tiêu chí lọc

Một task được xem là có thể tự làm trong lượt này khi phần implementation nằm
trong module TV5 hoặc chỉ cần public API đã tồn tại; không cần TV2/TV3/TV4
quyết định interface mới. Task còn cần consumer/event từ owner khác được ghi
`BLOCKED` hoặc `HANDOFF READY`, không âm thầm đánh dấu `DONE`.

## Kết quả

| Task | Trạng thái | Phân tích / bằng chứng |
|---|---|---|
| S6-TV5-01 | DONE | `InputState` giữ Pressed/Held/Released; `InputHandler` dispatch Released. |
| S6-TV5-02 | HANDOFF READY | `RunCommand` đã có callback request; binding vào Mario run-state chờ consumer TV3. |
| S6-TV5-03 | HANDOFF READY | `ShootCommand` chỉ phát callback request, không spawn FireBall; consumer TV3 nối callback. |
| S6-TV5-04 | HANDOFF READY | `InputTrigger::Released` và `JumpReleaseCommand` đã sẵn sàng cho jump-release API. |
| S6-TV5-05 | DONE | `PlayState` bỏ qua input khi dead, terminal pending hoặc transition. |
| S6-TV5-06 | DONE | `input_state_tests` thêm release, binding kép, run/shoot request và vẫn pass. |
| S6-TV5-07 | DONE | Power-up event được phát đúng một lần cho mỗi pickup, kể cả pickup không đổi state. |
| S6-TV5-08 | DONE | Mushroom chỉ đổi SMALL→SUPER; SUPER/FIRE không downgrade. |
| S6-TV5-09 | BLOCKED | FireFlower state path có sẵn, nhưng DoD còn yêu cầu shoot consumer của TV3. |
| S6-TV5-10 | DONE | Star chỉ gọi timed `setInvincible`, không còn grow vĩnh viễn. |
| S6-TV5-11 | DONE | `?` resolve một lần: SMALL→Mushroom, SUPER/FIRE→FireFlower. |
| S6-TV5-12 | DONE | `U` map 1-Up, QuestionBlock single-hit và Mushroom cộng một life. |
| S6-TV5-13 | DONE | `O` map Star, spawn một lần và có movement/collect path. |
| S6-TV5-14 | DONE | Item có collectible delay trong lúc emerge. |
| S6-TV5-15 | DONE | `Coin::awardTo` +100 score, 100 coin→life và giữ remainder. |
| S6-TV5-16 | PARTIAL | Item/coin score rules đã chuẩn hóa; score theo enemy defeat cause còn chờ TV3. |
| S6-TV5-17 | DONE | HUD timer 400 giây, tick theo dt và bị freeze khi gameplay inactive. |
| S6-TV5-18 | DONE | Warning threshold phát một lần qua callback `hurryup.wav`. |
| S6-TV5-19 | BLOCKED | Timeout callback đã có hook, nhưng terminal death/respawn là contract TV3. |
| S6-TV5-20→23 | DONE | HUD world, power, score formatting, lives/coins refresh đã có. |
| S6-TV5-24→27 | DONE | MusicId path catalog, volume-safe streaming và Level 1/2/3 mapping đã nối. |
| S6-TV5-28 | HANDOFF READY | Star track và restore API đã có; timer-end hook cần consumer TV3. |
| S6-TV5-29 | HANDOFF READY | Death/GameOver/Win track catalog đã có; state owners quyết định thời điểm chuyển. |
| S6-TV5-30 | DONE | Pause giữ stream position; `playMusic()` resume track hiện tại. |
| S6-TV5-31→33 | HANDOFF READY | SFX catalog/mapping đã preload; FireBall, shell, brick/item publishers thuộc TV3/TV4. |
| S6-TV5-34 | DONE | SoundManager dùng tối thiểu bốn voice độc lập và drop request khi tất cả bận. |
| S6-TV5-35 | DONE | Sound/music volume clamp cả NaN và ngoài `[0,100]`. |
| S6-TV5-36 | BLOCKED | Cần Pause UI của TV2 và SaveManager của TV4. |
| S6-TV5-37→43 | DONE | Manifest, source/reference cleanup, asset removal, asset notes và engineering report đã hợp nhất. |
| S6-TV5-44 | DONE | Có regression tests và manual checklist tại `TV5_AUDIO_HUD_ITEM_CHECKLIST.md`. |

## Files chính đã thay đổi

- Input: `include/patterns/InputHandler.h`, `src/patterns/InputHandler.cpp`,
  `RunCommand`, `ShootCommand`, `JumpReleaseCommand` và `tests/InputStateTests.cpp`.
- Items/HUD/audio: `include/items/Item.h`, `src/items/*`,
  `QuestionBlock`, `HUD`, `SoundManager`.
- Asset/docs: `assets/ASSETS_LIST.md`, `docs/assets/reference/`,
  `docs/engineering/TV5_IMPLEMENTATION_NOTES.md`.

Các task `BLOCKED`/`HANDOFF READY` không được đổi thành `DONE` cho đến khi
owner tương ứng nối consumer và chạy full-loop verification.

## Reopened-TV5 fix update (2026-08-07)

The table above is the original pre-fix assessment. The following records the
implementation completed for the TV5-only reopened scope:

| Task IDs | Updated status | Evidence |
|---|---|---|
| 02, 03 | IMPLEMENTED / HANDOFF READY | `PlayState::rebindCommands()` binds Shift to `RunCommand` and X to `ShootCommand`; the gameplay owner remains responsible for physics semantics. |
| 06 | DONE | New `tv5_integration_tests` target plus the existing input regression target; full CTest run is 7/7. |
| 09 | IMPLEMENTED / HANDOFF READY | FireFlower produces FIRE Mario and `Level::spawnFireBall()` is the runtime shoot adapter. |
| 15 | DONE | `ScoreRules` centralizes coin scoring and `Coin::awardTo()` preserves the 100-coin life threshold/remainder. |
| 16 | PARTIAL / HANDOFF READY | Coin, power-up, stomp and FireBall scoring use the central API; TV3 still owns the final DefeatCause contract for shell/star enemy kills. |
| 19 | IMPLEMENTED / HANDOFF READY | HUD timeout is one-shot and calls Mario life/death flow; TV1/TV3 still own reload/GameOver state transitions. |
| 21 | DONE | HUD exposes SMALL/SUPER/FIRE/STAR and reacts to Star start/expiry events. |
| 27-29 | IMPLEMENTED / HANDOFF READY | Level music, Star restore, death, GameOver and Win track hooks are connected through `SoundManager`. |
| 31-33 | IMPLEMENTED / HANDOFF READY | Runtime producers publish FireBall, shell-kick, block/brick/item/1-Up events; `SoundManager` maps each event to its catalogued SFX. |
| 36 | PARTIAL / HANDOFF READY | Pause UI adjusts music/SFX immediately and clamps both ranges; SaveManager persistence remains TV4-owned. |
| 37, 41, 43 | DONE | Runtime enemy textures were restored, `ASSETS_LIST.md` dimensions/usage were corrected, and this implementation note was refreshed. |

Verification command:

```powershell
cmake --build build-tv5-clean --parallel 2
ctest --test-dir build-tv5-clean --output-on-failure
```

The build completed successfully and all 7 registered tests passed.

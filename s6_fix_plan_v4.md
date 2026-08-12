# Kế hoạch sửa Sprint 6 — vòng 4

## 1. Mục tiêu và phạm vi

Kế hoạch này xử lý **40 task chưa đạt** trong [`Evaluate_v4.md`](Evaluate_v4.md). Mục tiêu là đưa cả 184 task lên `Tốt` hoặc `Rất tốt` trên một commit release candidate duy nhất, giữ owner gốc trong [`s6_plan.md`](docs/management/s6_plan.md) và không tiếp tục mở rộng feature ngoài Sprint 6.

Ưu tiên vòng 4 là sửa đúng contract, không chỉ làm CTest xanh. Level 4, underwater, Piranha Plant, Springboard hoặc mechanic bổ sung có thể được giữ làm future content, nhưng **không được nằm trong Sprint 6 release catalog hoặc thay đổi Level 1–3/Win contract**.

Nguyên tắc thực hiện:

- Khôi phục release contract ba level trước khi sửa test/report/manual evidence.
- Một hành vi runtime chỉ có một production entry point và một event/SFX authority.
- Test phải gọi cùng entry point mà gameplay thật gọi; không dùng helper khác để đại diện.
- Không sửa assertion để hợp thức hóa implementation trái `s6_plan.md`.
- Manual evidence phải ghi commit release candidate, preset, expected, actual và PASS/FAIL thật.
- Tracker/report chỉ được đánh dấu `DONE` sau khi evidence cùng commit đã tồn tại.

### Danh sách task phải mở lại

| Owner | Task ID mở lại | Số lượng |
|---|---|---:|
| TV1 | `01, 02, 05, 14, 30, 31, 32, 35` | 8 |
| TV2 | `05, 06, 07, 08, 10, 11, 16, 19, 20, 22, 23, 27, 28` | 13 |
| TV3 | `10, 11, 13, 16, 26, 34, 36, 37` | 8 |
| TV4 | `15, 17, 18, 19, 40` | 5 |
| TV5 | `11, 27, 31, 41, 43, 44` | 6 |
| **Tổng** |  | **40** |

## 2. Gate 0 — Khôi phục Sprint 6 release contract

Owner chính: **TV1**. Phối hợp: TV4, TV5. Gate này phải merge trước mọi playthrough, report và screenshot mới.

### 2.1 Catalog đúng ba level

Task: TV1-05, TV1-14, TV4-17, TV5-27.

- [ ] `LevelCatalog::getAll()` chỉ chứa Level 1, 2 và 3.
- [ ] Metadata khóa lại: Level 1 = Overworld/Overworld music; Level 2 = Underground/Underground music; Level 3 = Castle hoặc castle/sky/Castle music.
- [ ] Hoàn thành Level 1 → Level 2; Level 2 → Level 3; Level 3 → Win.
- [ ] `highestUnlockedLevel` không vượt release level 3 trong Sprint 6 flow.
- [ ] Chuyển castle data hiện nằm ở `level4.txt` về release Level 3 hoặc khôi phục Level 3 castle data tương đương.
- [ ] Đưa underwater/Level 4 ra khỏi release catalog. Nếu giữ file/code, gắn nhãn `Future` và bảo đảm New Game/full loop không thể đi vào đó.
- [ ] Bỏ mọi comment/assertion “exactly four release levels” khỏi test Sprint 6.

Regression bắt buộc:

- Catalog có đúng `[1, 2, 3]`, không có `0` hoặc `4`.
- `isPastFinalLevel(4) == true`.
- Production PlayState nhận completion Level 3, snapshot final score và queue đúng một WinState.
- Level 3 load được, có đúng một Mario/finish/pole và dùng Castle music.

### 2.2 Khôi phục item distribution deterministic

Task: TV4-15, TV4-19, TV5-11.

- [ ] Normal adaptive `?` block phải resolve đúng contract: SMALL → Mushroom; SUPER/FIRE → FireFlower.
- [ ] Content được quyết định một lần ở first hit; persistent/multiple contact không reroll hoặc spawn hai item.
- [ ] Nếu muốn coin-random block, dùng symbol/content type riêng; không thay semantics của normal `?` trong Sprint 6.
- [ ] Level 2 phải có route lấy FireFlower deterministic, không phụ thuộc roll ngẫu nhiên.
- [ ] Level 1–3 phải có Mushroom, FireFlower, Star và 1-Up theo placement có chủ đích hoặc explicit block code.

Regression bắt buộc:

- 100 lần gọi normal adaptive selector vẫn cho cùng kết quả theo MarioState, không có Coin.
- Hit block hai lần chỉ spawn một item và phát một event.
- Automated level-data test xác nhận mechanic bắt buộc có mặt ở level dự kiến.

Điều kiện qua Gate 0:

- Full release graph là `Menu → 1 → 2 → 3 → Win`.
- Level 3 là Castle và phát Castle music.
- Normal `?` block không còn weighted coin behavior.
- Test gate fail nếu ai thêm Level 4 vào Sprint 6 catalog hoặc dời Win boundary.

## 3. Gate 1 — Sửa runtime physics, growth, FireBall và collision

### 3.1 Một power-state transition path

Owner chính: **TV3**. Phối hợp: TV2, TV5. Task: TV3-10, TV3-11, TV3-13, TV3-16, TV2-22, TV2-23.

- [ ] Tạo một operation duy nhất cho SMALL/SUPER/FIRE transition; `powerUp()` và restore state phải delegate vào operation này.
- [ ] Chỉ foot-anchor trong `rebuildFixture()` hoặc một helper duy nhất; xóa transform 16 px trùng trong `powerUp()`.
- [ ] Query clearance trước khi đổi state, animation và fixture. Nếu thiếu chỗ, giữ state/fixture cũ và lưu pending target.
- [ ] Retry pending growth khi Mario ra khỏi trần; event/score/SFX pickup vẫn chỉ phát một lần.
- [ ] Test phải nhặt `Mushroom::onCollect()`/`FireFlower::onCollect()` dưới trần, không được chỉ gọi `setMarioState()`.
- [ ] Tick Star timer và damage-grace timer độc lập trong cùng frame; không `return` làm freeze timer còn lại.
- [ ] Hoàn thiện `respawn()` reset vertical intent, climb/flag state, gravity scale, pending power/fixture/transform, cooldown, contact/grounded state và animation.
- [ ] PlayState đợi death-animation completion signal; fallback timeout chỉ dùng cho lỗi asset và phải có test.
- [ ] Grow/shrink/spawn/death presentation giữ chân đồng bộ với fixture policy.

Regression bắt buộc:

- Pickup grow trên sàn giữ foot Y trong sai số nhỏ; shrink cũng vậy.
- Pickup dưới trần không đổi fixture; sau khi ra chỗ trống mới grow đúng một lần.
- Star và damage grace cùng active vẫn hết theo hai duration độc lập.
- Death khóa move/jump/shoot, phát một event, trừ một life; respawn hai lần cho cùng clean state.

### 3.2 Một FireBall request và một SFX authority

Owner chính: **TV3** cho projectile; **TV5** cho audio. Task: TV5-31 và regression của TV3-18/19.

- [ ] Chỉ giữ `Level::requestFireBallShot()` làm public Level entry point; xóa `spawnFireBall()`/`shootFireBall()` legacy wrappers và constant limit 4 thừa.
- [ ] Chọn một storage path: entity list hoặc FireBallPool; không duy trì pool public nhưng gameplay lại dùng vector khác.
- [ ] World-locked request phải reserve active slot và consume/reserve cooldown ngay khi request được chấp nhận.
- [ ] Pending request giữ đúng owner/direction; không tự đổi owner sang `m_mario` nếu caller khác.
- [ ] Chỉ notify `FIREBALL_SHOT` sau khi projectile được tạo.
- [ ] SoundManager là SFX authority duy nhất; bỏ `playSound("fireball")` trực tiếp khỏi Level.

Regression bắt buộc:

- Projectile 1 và 2 được tạo, projectile 3 bị từ chối; inactive projectile giải phóng slot.
- Request trong world-lock không vượt limit, không bỏ cooldown và không sai owner.
- Một projectile được tạo → một event → đúng một SFX voice; request bị từ chối không phát event/SFX.

### 3.3 Hoàn tất collision/bounds coverage

Owner chính: **TV3**. Phối hợp: TV4, TV5. Task: TV3-26, TV3-34, TV3-36, TV3-37.

- [ ] Thêm Mario–Enemy side-hit test riêng, bảo đảm damage đúng một bậc và không cộng stomp score.
- [ ] Chạy fixture order A/B cho stomp, side, shell, FireBall và Star.
- [ ] Giữ persistent/multiple contact idempotent cho score/event/SFX.
- [ ] Xóa `m_boundingBox` cache nếu getter dùng position/size, hoặc dùng cache làm source duy nhất; không giữ cả hai representation.
- [ ] Bổ sung test suite cho actual pickup growth, simultaneous timers, death completion và full respawn reset.

Điều kiện qua Gate 1:

- Growth dưới trần và foot anchor đúng qua đường pickup thật.
- Star/grace không freeze lẫn nhau.
- FireBall có một request API và một SFX/event path.
- Collision matrix đủ stomp, side, shell, FireBall, Star, A/B order và persistent contact.

## 4. Gate 2 — Camera, display, UI và presentation

Owner chính: **TV2**. Task: `05, 06, 07, 08, 10, 11, 16, 19, 20, 22, 23, 27, 28`.

### 4.1 Camera policy và clamp

- [ ] Thêm camera mode từ level metadata: Overworld/Castle thường khóa Y; map cao dùng Y dead-zone theo policy rõ.
- [ ] Clamp cả X và Y sau follow và sau shake.
- [ ] Khi world nhỏ hơn viewport, center trục tương ứng thay vì tạo min/max đảo.
- [ ] Test normal jump ở map khóa Y, tall-map follow, bốn cạnh world và shake tại bốn cạnh.

### 4.2 Integer letterbox và mouse mapping

- [ ] Bỏ resize handler cưỡng ép window về 16:9.
- [ ] Tính integer scale từ logical 640×360, center content rectangle và tô bar đen phần dư.
- [ ] Tắt smoothing cho render texture/sprite liên quan.
- [ ] Dùng cùng content rectangle cho render và physical mouse → logical coordinate conversion.
- [ ] Click trong letterbox bar bị từ chối; click target trong content đúng sau mọi resize.

### 4.3 UI, animation và background

- [ ] Menu/Pause/GameOver/Win cùng dùng `UILayoutHelper`/`UIMenuWidget` và shared mouse coordinate contract.
- [ ] GameOver render final score + persisted high score; Retry/Menu hoạt động keyboard/mouse.
- [ ] Win render final score + persisted high score; Return hoạt động keyboard/mouse.
- [ ] Nối death/spawn/grow/shrink completion signal vào gameplay lifecycle thay vì timer trùng duration.
- [ ] Overworld, Underground và Castle có background visual riêng hoặc documented theme renderer riêng; không dùng overworld background cho Castle.

Bằng chứng bắt buộc:

- Unit test viewport, integer scale, bar rectangle và mouse remap.
- Screenshot + click log tại 1280×720, 1600×900, 1920×1080 và ít nhất một 4:3 resolution.
- Keyboard/mouse đi được Menu, Pause, GameOver và Win; high score khớp SaveManager.

Điều kiện qua Gate 2:

- Không stretch/blur/crop HUD ở resolution matrix.
- Camera không lộ ngoài X/Y bounds sau shake.
- UI đủ final/high score và click mapping.
- Presentation signal khớp physics lifecycle.

## 5. Gate 3 — Tài liệu và traceability

### TV1 — Dương

Task: `01, 02, 30, 31, 32, 35`.

- [ ] Bổ sung Sprint 3 vào audit tracker; mỗi deliverable có owner, trạng thái hiện tại, đúng task ID và evidence cùng release commit.
- [ ] Sửa toàn bộ stale bug state; thêm bug release catalog, growth runtime, duplicate FireBall/death SFX, adaptive block, camera/UI và evidence mismatch.
- [ ] README cập nhật đủ 14+ suite thực tế, bỏ dòng Pause trùng, ghi đúng controls và release flow ba level.
- [ ] FILE_STRUCTURE tạo từ file tree hiện tại; thêm/bỏ file theo quyết định Gate 0 và ghi rõ Runtime/Reference/Future.
- [ ] Class diagram sinh lại từ header hiện tại: state ownership, Level/FireBall API, physics accumulator, defeat path, new/removed entity và exact enum/event values.
- [ ] Sửa 41 relative link trong `TV1_CHANGES_SUMMARY.md`; xóa link tới file không tồn tại hoặc tạo archive hợp lệ theo policy.
- [ ] Integration log ghi từng ngày/merge quan trọng và tạo release-candidate row cuối gồm hash, ba build, CTest, warning, manual gate và known issues thật.

### TV5 — Truyền

Task: `41, 43, 44`.

- [ ] ASSETS_LIST khớp release catalog sau Gate 0: Castle = Level 3; underwater/Level 4 là Future nếu còn giữ.
- [ ] Mô tả background theo caller/render branch thật; dimensions/path/usage phải được test từ file hiện tại.
- [ ] TV5 report trỏ đúng release-candidate commit, đúng số suite và không dùng “HEAD + working tree” làm evidence cuối.
- [ ] Audio/HUD/item checklist cập nhật theo flow ba level; tách automated evidence và manual audio-device result.
- [ ] Thêm automated counter/test cho one-event/one-SFX ở FireBall, death, shell kick/kill và repeated contact.

Điều kiện qua Gate 3:

- Link checker trên toàn bộ Markdown tracked trả 0 broken internal link.
- Tracker, register, README, structure, diagram, asset manifest và reports cùng mô tả một commit.
- Không còn claim 4-level release, 13/13 test hoặc 15/15 playthrough nếu evidence không đúng.

## 6. Gate 4 — Manual release acceptance

Owner điều phối: **TV4** cho playthrough; TV2 cho resolution; TV5 cho audio; TV1 chốt release report.

### TV4 — Level verification

Task: TV4-18, TV4-40 và nghiệm thu lại TV4-15/17/19.

- [ ] Chạy đúng năm scenario cho mỗi Level 1–3: normal, no-damage, death/respawn, power-up và flag route.
- [ ] Mỗi run ghi commit, preset, timestamp, duration, deaths, item/combat path, expected, actual, PASS/FAIL và bug ID.
- [ ] Không dùng run từ commit cũ; không sửa FAIL/OBSERVATION thành PASS mà không rerun.
- [ ] Mọi entry phải có dòng `Result`; summary được tính tự động hoặc đối chiếu đúng số entry PASS.

### Ma trận release chung

- [ ] Menu → Level 1 → Level 2 → Level 3 → Win; không load Level 0/4.
- [ ] Score, coins, lives và power giữ đúng qua hai transition.
- [ ] Level 2 có FireFlower deterministic; Level 3 có Star/castle gauntlet và Castle music.
- [ ] Death còn life → respawn; hết life → GameOver; Retry/Menu hoạt động keyboard/mouse.
- [ ] Pause/resume 20 lần; timer/input/music không drift hoặc nhân bản.
- [ ] FireBall 1/2/3 limit, one-shot SFX; stomp/side/shell/FireBall/Star đúng score/event/SFX.
- [ ] Chạm flag thấp/cao; death + flag cùng frame chỉ commit một terminal result.
- [ ] Restart executable: high score, highest unlocked level và hai volume còn đúng.
- [ ] Chạy resolution matrix của TV2 và audio-device checklist của TV5.
- [ ] Debug/Release/Tests configure + build từ clean directory; CTest 100%; production warning = 0; incremental build không copy asset thừa.

Điều kiện qua Gate 4:

- 15/15 playthrough entry hợp lệ và cùng release-candidate commit.
- Full loop ba level, pause, resolution, audio, save và UI matrix PASS.
- Không còn P0/P1; P2 còn lại phải có owner và quyết định rõ, không được ẩn dưới nhãn polish.

## 7. Thứ tự tích hợp đề xuất

1. TV1/TV4/TV5 khôi phục catalog, Level 3 Castle, Win boundary và adaptive block contract.
2. TV3 hợp nhất growth transition; TV2 nối completion signal; TV5 loại duplicate FireBall SFX.
3. TV3 hoàn tất timer/respawn/collision/bounds tests.
4. TV2 merge camera Y policy, X/Y clamp, integer letterbox, mouse remap, high-score UI và background.
5. TV1/TV5 cập nhật docs/report theo code đã ổn định và chạy link checker.
6. TV2 chạy resolution matrix; TV4 chạy 15 playthrough; TV5 chạy audio checklist.
7. TV1 tạo release-candidate report và cả nhóm chỉ sửa regression phát hiện ở Gate 4.

Không chạy lại playthrough hoặc chụp screenshot trước Gate 0–2; evidence đó sẽ không đại diện cho release candidate cuối.

## 8. Definition of Done cuối Sprint

Sprint chỉ được đóng khi tất cả điều kiện sau đúng trên **một commit release candidate**:

- [ ] 184/184 task đạt `Tốt` hoặc `Rất tốt`.
- [ ] Release catalog có đúng ba level; Level 3 Castle → Win.
- [ ] Normal `?` block deterministic theo MarioState; Level 2 có FireFlower route chắc chắn.
- [ ] Growth pickup dưới trần, foot anchor, Star/grace và respawn đều deterministic.
- [ ] Một FireBall request API, tối đa hai active và một event/SFX cho mỗi shot.
- [ ] Collision suite đủ stomp, side, shell, FireBall, Star, fixture order và persistent contact.
- [ ] Camera Y policy, X/Y clamp, integer letterbox và mouse remap đúng toàn matrix.
- [ ] GameOver/Win hiển thị final score + high score và hỗ trợ keyboard/mouse.
- [ ] Save/high score/unlock/volume tồn tại sau restart.
- [ ] Clean Debug/Release/Tests build pass; CTest 100%; production warning = 0.
- [ ] 15 playthrough, full-loop, pause cycle, resolution và audio evidence đều PASS trên cùng commit.
- [ ] Tracker, bug register, README, file structure, class diagram, asset docs và reports khớp code; internal link checker = 0.

Nếu bất kỳ mục nào thiếu evidence, task tương ứng giữ `REVIEW`, `BLOCKED` hoặc `REOPENED`; không đổi assertion, summary hoặc release scope để biến FAIL thành PASS.

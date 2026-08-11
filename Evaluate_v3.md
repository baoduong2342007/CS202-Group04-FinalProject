# Đánh giá Sprint 6 — vòng 3

## 1. Kết luận

Đánh giá lại toàn bộ 184 task trong [`s6_plan.md`](docs/management/s6_plan.md) tại commit `aad6a4a` — cùng commit mà `origin/develop` đang trỏ tới — cho kết quả **139/184 task (75,5%)** đạt mức `Tốt` hoặc `Rất tốt`; còn **45 task phải mở lại**.

So với [`Evaluate_v2.md`](Evaluate_v2.md), số task đạt tăng từ **103 lên 139** (`+36`). Đây là đánh giá lại trực tiếp từ code, asset, tài liệu, build và test ở commit hiện tại; không kế thừa trạng thái `DONE` một cách máy móc.

Kết quả xác minh kỹ thuật:

- Configure và build sạch preset `mingw-debug`: thành công.
- Configure và build sạch preset `mingw-release`: thành công.
- Configure và build sạch preset `mingw-tests`: thành công.
- CTest: **12/12 pass**: `input_state_tests`, `tile_collision_span_tests`, `mario_physics_tests`, `play_state_tests`, `game_manager_tests`, `event_bus_tests`, `tv5_integration_tests`, `level_catalog_tests`, `save_manager_tests`, `level_validator_tests`, `gate0_contract_tests`, `save_session_tests`.
- Không có compiler warning từ production code; chỉ có CMake deprecation warning từ dependency Box2D.
- Build Release lần hai không chạy lại bước đồng bộ asset, nên incremental asset-copy đạt yêu cầu.
- `git diff --check` pass.
- Ba release level đều qua validator và Level 3 hiện load được.

Các kết quả xanh trên chưa đủ để đóng toàn bộ Sprint: suite hiện tại còn một số test chỉ gọi helper trực tiếp hoặc mô phỏng điều kiện thay vì đi qua đường runtime thật. Repository cũng chưa có bằng chứng đầy đủ cho full loop `Menu → Level 1 → Level 2 → Level 3 → Win`, ma trận bốn độ phân giải, 15 playthrough của TV4 và checklist audio/HUD/item tái lập của TV5.

### Quy ước phân loại

- **Rất tốt:** Đạt toàn bộ DoD, implementation rõ ràng và có bằng chứng build/test mạnh.
- **Tốt:** Đạt DoD; bằng chứng đủ để chấp nhận, dù mức test hoặc polish chưa mạnh bằng nhóm `Rất tốt`.
- **Chưa tốt:** Đã có implementation đáng kể nhưng còn lỗi, thiếu tích hợp, thiếu coverage hoặc thiếu bằng chứng nghiệm thu.
- **Hoàn toàn sai:** Đường chạy chính đi ngược hợp đồng/DoD, kể cả khi có helper hoặc test bề mặt đang pass.
- **Chưa làm:** Không tìm thấy implementation hay bằng chứng có ý nghĩa cho yêu cầu của task.

Task được tính cho owner gốc trong `s6_plan.md`. Một task chỉ được tính “đạt DoD” khi ở mức `Tốt` hoặc `Rất tốt`.

## 2. Tổng hợp

| Thành viên | Rất tốt | Tốt | Chưa tốt | Hoàn toàn sai | Chưa làm | Đạt DoD |
|---|---:|---:|---:|---:|---:|---:|
| TV1 — Dương | 15 | 14 | 6 | 0 | 0 | 29/35 — 82,9% |
| TV2 — Nhật | 5 | 8 | 9 | 3 | 3 | 13/28 — 46,4% |
| TV3 — Bảo | 8 | 12 | 9 | 4 | 4 | 20/37 — 54,1% |
| TV4 — Vy | 15 | 23 | 2 | 0 | 0 | 38/40 — 95,0% |
| TV5 — Truyền | 18 | 21 | 5 | 0 | 0 | 39/44 — 88,6% |
| **Tổng** | **61** | **78** | **31** | **7** | **7** | **139/184 — 75,5%** |

### Thay đổi so với vòng 2

| Thành viên | Vòng 2 | Vòng 3 | Chênh lệch task đạt |
|---|---:|---:|---:|
| TV1 | 23/35 | 29/35 | +6 |
| TV2 | 11/28 | 13/28 | +2 |
| TV3 | 15/37 | 20/37 | +5 |
| TV4 | 26/40 | 38/40 | +12 |
| TV5 | 28/44 | 39/44 | +11 |
| **Tổng** | **103/184** | **139/184** | **+36** |

TV4 tiến bộ nhiều nhất nhờ hoàn thiện ba level, validator, enemy lifecycle và tích hợp SaveManager. TV5 đã sửa đúng input contract, item states, Star/HUD/music, volume persistence và asset packaging. TV1 đã nối save vào session/state, sửa death event contract, chuẩn hóa build target và tăng coverage. Phần còn yếu tập trung ở camera/display/UI của TV2 và combat/physics cleanup của TV3.

## 3. Các blocker còn lại

1. **FireBall vẫn có hai đường runtime và sai giới hạn.** Phím `X` đi qua `Level::spawnFireBall()`, nhưng đường này không kiểm tra số projectile active. `Level::shootFireBall()` cũ vẫn tồn tại và dùng giới hạn 4 thay vì 2. Vì vậy test cooldown/owner/event không chứng minh DoD “tối đa hai FireBall”.
2. **Collision vẫn double-dispatch.** `CollisionManager::resolve()` gọi `onCollisionBegin()` của cả hai entity rồi tiếp tục chạy handler trung tâm. Stomp có thể bị callback xử lý trước khiến handler score bỏ qua; shell/FireBall/Star chưa đi qua một shared `DefeatCause`/score/event/SFX path. Star chưa có runtime kill path.
3. **Camera và scaling chưa đạt hợp đồng 640×360.** `Game.cpp` vẫn ép cửa sổ về 16:9 và scale liên tục; chưa có integer letterbox hay remap chuột từ physical window sang logical view. `Camera.cpp` luôn theo Y, chủ động bỏ Y clamp và chỉ clamp X sau shake.
4. **Growth/death/physics còn lỗi nền.** Physics accumulator vẫn là `static`; grow dưới trần thấp vẫn dựng fixture cao thay vì defer; grow/shrink chưa giữ chân nhất quán. Death đã trừ một life/phát một event, nhưng chưa có completion signal và chưa khóa trọn input/physics; respawn chưa reset đầy đủ state/timer/input.
5. **UI/resource cleanup chưa xong.** GameOver/Win thiếu high score và mouse target; shared layout chưa phủ mọi state; background chưa đổi theo `LevelTheme`; `TextureManager::deleteTexture()` và các API animation không có caller vẫn public.
6. **Manual acceptance chưa đủ.** Có các đoạn ghi nhận PASS cho gap Level 1, spawn Level 2 và fairness Level 3, nhưng chưa có log năm playthrough cho từng level, full-loop release, pause 20 lần hay resolution matrix.
7. **Tài liệu vẫn mô tả sai code.** `FILE_STRUCTURE.md` còn file không tồn tại; class diagram còn nhiều API/relationship cũ; `ASSETS_LIST.md` ghi sai usage của một số atlas/background; report TV5 còn test count/commit cũ. Tracker và bug register chưa phản ánh đầy đủ các lỗi đang mở.

## 4. Phân loại chi tiết theo ID

### TV1 — Dương

- **Rất tốt:** `06, 15, 16, 18, 19, 20, 22, 23, 24, 25, 26, 27, 28, 29, 34`
- **Tốt:** `03, 04, 05, 07, 08, 09, 10, 11, 12, 13, 17, 21, 30, 33`
- **Chưa tốt:** `01, 02, 14, 31, 32, 35`
- **Hoàn toàn sai:** không có
- **Chưa làm:** không có

Điểm mạnh: release flow bắt đầu Level 1, GameProgress/transition/state stack, EventBus lifetime, ownership, CMake presets/target `SuperMario`, incremental asset copy và SaveManager integration đều đã được sửa có hệ thống. Death contract hiện chỉ phát `PLAYER_DIED` và trừ một life; high score/unlock/volume đã có caller runtime.

Các task cần mở lại:

| ID | Mức | Lý do |
|---|---|---|
| 01 | Chưa tốt | Audit tracker chưa liệt kê từng deliverable trong `WEEKLY_PLAN.md`; nhiều hàng vẫn gộp và trạng thái Level/item/enemy/HUD/audio không theo code hiện tại. |
| 02 | Chưa tốt | Bug register bỏ sót camera Y/letterbox, growth clearance, collision double-dispatch, FireBall limit và lỗi documentation; một số mục “fixed” thiếu bằng chứng đường runtime. |
| 14 | Chưa tốt | Logic Level 3 → Win có, nhưng `PlayStateTests` tự mô phỏng catalog và ghi rõ full loop vẫn là manual; chưa chứng minh Win transition thật chỉ push một lần với final score thật. |
| 31 | Chưa tốt | `FILE_STRUCTURE.md` còn liệt kê tài liệu đã xóa/không tồn tại và còn mô tả path/usage cũ. |
| 32 | Chưa tốt | Class diagram còn API không tồn tại của SoundManager, Entity/Character/Koopa/Level/TileMap/TextureManager/Animation/Collision/EventBus và relationship ownership cũ. |
| 35 | Chưa tốt | Build/test hiện xanh, nhưng không có chuỗi bằng chứng integration review cuối mỗi ngày; summary/link nội bộ hiện tại vẫn stale hoặc hỏng. |

### TV2 — Nhật

- **Rất tốt:** `03, 04, 12, 13, 14`
- **Tốt:** `01, 02, 09, 15, 17, 18, 21, 24`
- **Chưa tốt:** `06, 07, 08, 16, 19, 20, 22, 23, 27`
- **Hoàn toàn sai:** `05, 10, 11`
- **Chưa làm:** `25, 26, 28`

Điểm mạnh: DisplayConfig/logical view, HUD default view, X dead-zone, close ordering, delta clamp, frame cap, render ordering, Pause overlay và fade đều hoạt động đúng. Menu đã đọc high score và hỗ trợ keyboard/click để Start; absolute system-font fallback đã được bỏ.

Các task cần mở lại:

| ID | Mức | Lý do |
|---|---|---|
| 05 | Hoàn toàn sai | Camera luôn cập nhật Y theo Mario, nên overworld vẫn nảy khi nhảy. |
| 06 | Chưa tốt | Có Y-follow cơ bản nhưng Y clamp bị vô hiệu hóa, nên map cao chưa bảo đảm theo/clamp đúng. |
| 07 | Chưa tốt | X được clamp; Y không được clamp và case world nhỏ hơn viewport chưa hoàn chỉnh. |
| 08 | Chưa tốt | Sau shake chỉ X được clamp; shake vẫn có thể lộ ngoài world theo Y. |
| 10 | Hoàn toàn sai | Resize bị ép về 16:9 và texture được scale liên tục; chưa có viewport letterbox. |
| 11 | Hoàn toàn sai | Không tính integer scale; cửa sổ không phải bội số có thể làm pixel art blur/méo. |
| 16 | Chưa tốt | Layout helper tồn tại nhưng Menu/Pause/GameOver/Win chưa cùng dùng một anchor/mouse mapping contract. |
| 19 | Chưa tốt | GameOver hiển thị final score và có Retry/Menu bằng keyboard, nhưng thiếu high score và mouse interaction. |
| 20 | Chưa tốt | Win hiển thị final score/Return, nhưng thiếu high score và mouse interaction. |
| 22 | Chưa tốt | Death clip có hình ảnh/thời gian chờ nhưng chưa có completion signal cho TV3; lifecycle vẫn dựa timer ngoài animation. |
| 23 | Chưa tốt | Có chuyển state/sprite nhưng fixture grow/shrink chưa giữ world-space foot và chưa xử lý low ceiling. |
| 25 | Chưa làm | `TextureManager::deleteTexture()` vẫn public và còn implementation. |
| 26 | Chưa làm | `AnimationSystem::stop/pause/resume` vẫn public, không có caller và chưa được loại/private hóa. |
| 27 | Chưa tốt | Level luôn dùng `assets/ui/bg_world.png`; chưa chọn visual riêng cho Overworld/Underground/Castle. |
| 28 | Chưa làm | Không có resolution matrix/screenshot/log cho 1280×720, 1600×900, 1920×1080 và 4:3. |

### TV3 — Bảo

- **Rất tốt:** `04, 05, 06, 07, 08, 09, 29, 30`
- **Tốt:** `02, 03, 12, 14, 17, 18, 20, 21, 22, 23, 24, 28`
- **Chưa tốt:** `10, 13, 15, 16, 25, 26, 27, 35, 36`
- **Hoàn toàn sai:** `01, 11, 19, 31`
- **Chưa làm:** `32, 33, 34, 37`

Điểm mạnh: walk/run/variable jump/double-jump prevention, bounds, max substeps, SMALL/SUPER/FIRE, damage chain, FireBall cooldown/owner/bounce/render/cleanup và deferred destruction đều đã tiến bộ rõ. Gate tests bảo vệ input và death event tốt hơn vòng trước.

Các task cần mở lại:

| ID | Mức | Lý do |
|---|---|---|
| 01 | Hoàn toàn sai | `PhysicsEngine::s_timeAccumulator` vẫn là biến `static`, nên các world không có accumulator độc lập. |
| 10 | Chưa tốt | Grow có dịch body trong một trường hợp nhưng shrink không re-anchor; foot position không được bảo toàn nhất quán. |
| 11 | Hoàn toàn sai | Khi thiếu clearance, code không dịch Mario nhưng vẫn dựng fixture cao; không có pending/deferred growth, nên vẫn có thể xuyên/kẹt trần. |
| 13 | Chưa tốt | Damage grace và Star đã có field riêng, nhưng grace timer bị freeze khi Star active và Star chưa giết enemy ở collision runtime. |
| 15 | Chưa tốt | Life/event đã one-shot và sprite còn active, nhưng dying chưa khóa toàn bộ input/physics và chưa chờ animation completion signal. |
| 16 | Chưa tốt | Respawn path có, nhưng chưa reset đầy đủ contact/input/cooldown/timer/animation theo một policy deterministic. |
| 19 | Hoàn toàn sai | Đường bắn chính không giới hạn active count; đường cũ còn giới hạn 4. DoD yêu cầu tối đa 2. |
| 25 | Chưa tốt | Có helper phân loại contact nhưng kết quả tổng thể vẫn phụ thuộc callback entity chạy trước handler trung tâm. |
| 26 | Chưa tốt | Mario–Enemy handler tồn tại, nhưng callback stomp có thể xử lý trước làm shared score path bị bỏ qua. |
| 27 | Chưa tốt | Projectile handler tồn tại nhưng runtime chưa dùng `DefeatCause::FIREBALL` và còn phát event stomp thay thế. |
| 31 | Hoàn toàn sai | `ScoreRules::awardDefeat()` chỉ được test trực tiếp; đường runtime stomp/shell/FireBall/Star chưa thống nhất và chưa bảo đảm một kill/một score event. |
| 32 | Chưa làm | `Mario::handleInput()` deprecated vẫn tồn tại. |
| 33 | Chưa làm | Các member Mario cũ như `m_moveSpeed`, `m_wasJumpPressed` vẫn chưa được audit/xóa hoàn toàn. |
| 34 | Chưa làm | Bounding box/fixture-derived bounds vẫn chưa được gom thành một source of truth có test. |
| 35 | Chưa tốt | Một phần scenario đã có test, nhưng demo/case hữu ích chưa được chuyển hết thành regression tests. |
| 36 | Chưa tốt | Mario tests cover grounding/jump/collision span, chưa đủ walk/run/growth/damage/death/respawn matrix của DoD. |
| 37 | Chưa làm | Chưa có collision regression suite thật cho stomp, side, shell, FireBall và Star qua CollisionManager runtime. |

### TV4 — Vy

- **Rất tốt:** `01, 02, 03, 04, 05, 06, 07, 12, 20, 21, 22, 32, 34, 38, 39`
- **Tốt:** `08, 09, 10, 11, 13, 14, 15, 16, 17, 18, 19, 23, 24, 25, 26, 27, 28, 30, 31, 33, 35, 36, 37`
- **Chưa tốt:** `29, 40`
- **Hoàn toàn sai:** không có
- **Chưa làm:** không có

Đây là phần hoàn thành tốt nhất vòng này. Level 0 đã thành test fixture và nằm ngoài catalog; validator có coverage width/symbol/M/F/pole; Level 1–3 đều hợp lệ và có đủ mechanic chính. Enemy activation/cleanup, Goomba/Koopa/shell state machine và SaveManager missing/corrupt/version/high-score/atomic behavior đều có code và test. High score, unlock và volume đã được TV1/TV5 nối vào runtime.

Các task cần mở lại:

| ID | Mức | Lý do |
|---|---|---|
| 29 | Chưa tốt | Shell có thể giết enemy, nhưng chưa đi qua shared `DefeatCause::SHELL`; thiếu bảo đảm 200 điểm/event/SFX đúng một lần và lần kick đầu còn phát thêm event stomp. |
| 40 | Chưa tốt | Có ghi nhận PASS cho một số kiểm tra gap/spawn/fairness, nhưng không có 15 run log — năm playthrough có kết quả cho mỗi level — như DoD yêu cầu. |

### TV5 — Truyền

- **Rất tốt:** `01, 02, 04, 06, 07, 09, 10, 11, 15, 19, 21, 22, 24, 28, 34, 35, 38, 39`
- **Tốt:** `03, 05, 08, 12, 13, 14, 17, 18, 20, 23, 25, 26, 27, 29, 30, 31, 33, 36, 37, 40, 42`
- **Chưa tốt:** `16, 32, 41, 43, 44`
- **Hoàn toàn sai:** không có
- **Chưa làm:** không có

Điểm mạnh: Pressed/Held/Released, Shift-only Run, X-only Shoot và suppression tests đều đúng; FireFlower/adaptive block/Star/100-coin/HUD timer-power đã được sửa. MusicId, per-theme track, Star override/restore, death/GameOver/Win/Pause lifecycle, voice pool, volume clamp/persistence và reference-asset packaging đều có implementation tốt.

Các task cần mở lại:

| ID | Mức | Lý do |
|---|---|---|
| 16 | Chưa tốt | Bảng điểm/helper đúng, nhưng runtime defeat producers chưa cùng gọi một score API/event; shell/FireBall/Star có thể thiếu hoặc sai điểm. |
| 32 | Chưa tốt | `SHELL_KICKED` có latch theo state, nhưng lần kick đầu còn đồng thời phát `ENEMY_STOMPED`, dẫn đến mapping/SFX thừa. |
| 41 | Chưa tốt | Dimensions/path phần lớn đúng, nhưng usage còn sai: Mushroom dùng `items_objects`, `bg_clouds` không có caller runtime và crop note gán nhầm atlas cho TileMap. |
| 43 | Chưa tốt | Report còn header commit/working-tree cũ, ghi 10/10 test trong khi hiện là 12/12 và có một số claim rộng hơn coverage/runtime thực tế. |
| 44 | Chưa tốt | Integration suite kiểm tra nhiều helper/event tốt, nhưng chưa đi qua đầy đủ state/collision/audio runtime; manual checklist Level 1–3, Star, death, GameOver, Win, Pause/resume và restart vẫn thiếu. |

## 5. Đối chiếu test với đường runtime

Các điểm sau được dùng để tránh chấm quá cao chỉ vì CTest xanh:

- `SaveSessionTests::testMidSessionDeathSavesHighScore` gọi SaveManager trực tiếp; việc SaveManager thật sự được tích hợp được xác nhận thêm bằng code review ở GameManager/Game/Menu/Play/GameOver/Win/Pause.
- `PlayStateTests` kiểm tra catalog/terminal policy nhưng không chạy một Level 3 thực đến flag rồi quan sát WinState; vì vậy TV1-14 chưa đóng.
- Gate FireBall test xác nhận state/cooldown/owner/event nhưng không kiểm tra hai đường API và giới hạn hai projectile; vì vậy TV3-19 vẫn `Hoàn toàn sai`.
- ScoreRules tests xác nhận con số, không chứng minh CollisionManager gọi helper cho mọi `DefeatCause`; vì vậy TV3-31 và TV5-16 vẫn mở.
- Validator tests đủ mạnh để chấp nhận TV4-01–20 liên quan validator/data, nhưng không thay thế playthrough log của TV4-40.

## 6. Kết luận cuối

Branch hiện tại đã vượt xa vòng 2: build/release gate sạch, 12/12 test pass, ba level hợp lệ, save/input/items/HUD/audio và phần lớn state flow đã được tích hợp. Tuy nhiên Sprint 6 vẫn **chưa đạt Definition of Done** vì còn 45 task mở, trong đó các lỗi P0/P1 đáng chú ý là FireBall limit/duplicate API, collision/score double path, growth dưới trần thấp, camera/letterbox và thiếu full-loop/manual acceptance.

Ưu tiên sửa nên theo thứ tự: hợp nhất combat contract → sửa physics/growth/death → camera/letterbox/UI → tài liệu và manual release gate. Chi tiết phân công nằm trong [`s6_fix_plan_v3.md`](s6_fix_plan_v3.md).

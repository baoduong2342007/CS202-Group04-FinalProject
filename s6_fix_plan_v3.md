# Kế hoạch sửa Sprint 6 — vòng 3

## 1. Mục tiêu và phạm vi

Kế hoạch này xử lý **45 task chưa đạt** trong [`Evaluate_v3.md`](Evaluate_v3.md). Mục tiêu là đưa cả 184 task lên `Tốt` hoặc `Rất tốt`, giữ owner gốc trong [`s6_plan.md`](docs/management/s6_plan.md) và không mở thêm feature ngoài Sprint 6.

Build và 12/12 test hiện đã xanh, nên trọng tâm vòng này không phải tăng test count đơn thuần. Mỗi fix phải chứng minh đúng đường runtime, đặc biệt với collision, state transition, rendering coordinate và persistence.

Nguyên tắc thực hiện:

- Sửa hợp đồng dùng chung trước, rồi mới sửa consumer và tài liệu.
- Không giữ hai API cho cùng một hành vi runtime.
- Test phải đi qua production entry point; helper-only test chỉ là unit coverage, không thay thế integration DoD.
- Manual DoD phải có log tái lập gồm commit, cấu hình, bước chạy, expected, actual và kết quả.
- Không đánh dấu tracker/report `DONE` trước khi bằng chứng tương ứng đã tồn tại.
- Các task đã đạt chỉ chạy regression; không rewrite ngoài phạm vi.

### Danh sách task phải mở lại

| Owner | Task ID mở lại | Số lượng |
|---|---|---:|
| TV1 | `01, 02, 14, 31, 32, 35` | 6 |
| TV2 | `05, 06, 07, 08, 10, 11, 16, 19, 20, 22, 23, 25, 26, 27, 28` | 15 |
| TV3 | `01, 10, 11, 13, 15, 16, 19, 25, 26, 27, 31, 32, 33, 34, 35, 36, 37` | 17 |
| TV4 | `29, 40` | 2 |
| TV5 | `16, 32, 41, 43, 44` | 5 |
| **Tổng** |  | **45** |

## 2. Gate 0 — Hợp nhất combat và physics contract

Gate này phải hoàn tất trước UI polish và manual playthrough, vì các lỗi còn lại ảnh hưởng score, death và khả năng nghiệm thu level.

### 2.1 Một FireBall API duy nhất

Owner chính: **TV3**. Phối hợp: TV1, TV5.

- [ ] Chọn một entry point duy nhất, đề xuất `Level::requestFireBallShot(Mario&)`; xóa hoặc private hóa đường `spawnFireBall()`/`shootFireBall()` còn lại.
- [ ] Kiểm tra FIRE state, cooldown 250 ms, active count và world-lock queue tại cùng một nơi.
- [ ] Giới hạn **tối đa 2 projectile active**; object inactive/pending-destroy không được đếm, nhưng pending spawn phải được reserve để hai request cùng frame không vượt limit.
- [ ] Gắn owner/direction/spawn offset trước khi queue; chỉ phát `FIREBALL_SHOT` sau khi projectile thật sự được tạo.
- [ ] `X` là caller gameplay duy nhất; `Shift` chỉ là Run.

Regression bắt buộc:

- FIRE bắn được; SMALL/SUPER không bắn.
- Viên 1 và 2 được tạo, viên 3 bị từ chối; sau khi một viên inactive, request mới được chấp nhận.
- Request khi Box2D locked vẫn giữ owner/cooldown/limit/event contract.
- Giữ X không spawn mỗi frame; một projectile chỉ phát một shot SFX.

### 2.2 Một defeat/score/event/SFX path duy nhất

Owner chính: **TV3**. Phối hợp: TV4, TV5.

- [ ] Quy định CollisionManager là nơi duy nhất quyết định gameplay collision. Entity callback chỉ cập nhật presentation/local state hoặc trả kết quả `handled`; không được xử lý lần hai sau handler trung tâm.
- [ ] Tạo shared operation, ví dụ `defeatEnemy(victim, DefeatCause, owner)`, có victim latch/idempotency.
- [ ] Mọi đường `STOMP`, `SHELL`, `FIREBALL`, `STAR`, `PIT` đi qua shared operation; cause quyết định score/event/SFX đúng một lần.
- [ ] Chuẩn hóa điểm: stomp 100; shell/FireBall/Star 200. Pit theo policy đã khóa và không phát score nếu không có owner.
- [ ] Bỏ việc dùng `ENEMY_STOMPED` thay cho FireBall/shell; thêm event rõ hoặc payload `DefeatCause`.
- [ ] Tách shell kick khỏi shell kill: lần kick phát `SHELL_KICKED` một lần, không đồng thời phát stomp; persistent contact không lặp SFX.
- [ ] Thêm STAR collision kill và bảo đảm damage grace không có quyền giết enemy.

Regression bắt buộc:

- Chạy cả fixture order A/B cho stomp, side hit, FireBall và shell.
- Một victim tiếp xúc nhiều fixture/callback trong một frame chỉ chết, cộng điểm và phát SFX một lần.
- Star active giết enemy; damage grace không giết; Star hết hạn trả về damage policy bình thường.
- Test dùng `CollisionManager::resolve()`/production contact entry, không gọi riêng `ScoreRules::awardDefeat()` để thay thế integration test.

### 2.3 Physics, growth và death lifecycle

Owner chính: **TV3**. Phối hợp: TV1, TV2.

- [ ] Chuyển accumulator thành member của từng PhysicsEngine/world; reset khi tạo hoặc reload Level. Thêm test hai world xen kẽ để phát hiện shared state.
- [ ] Dùng foot anchor chung khi rebuild SMALL/SUPER/FIRE fixture. Grow và shrink phải giữ nguyên world-space foot trong sai số nhỏ.
- [ ] Trước grow, query clearance. Nếu không đủ chỗ, giữ power-up pending và fixture cũ; retry khi đủ clearance, không dựng fixture cao xuyên tile.
- [ ] Tách Star duration và damage-grace clock hoàn toàn; cả hai tick đúng một lần/frame và không freeze lẫn nhau.
- [ ] Death sequence: khóa movement/jump/shoot/physics control, giữ render active, phát death clip, trừ một life và phát một `PLAYER_DIED`.
- [ ] AnimationSystem cung cấp completion signal/callback; PlayState chỉ reload/GameOver sau signal hoặc fallback timeout được test.
- [ ] Respawn reset body transform, velocity, contact counters, grounded state, input edges, shooting cooldown, timers, animation và power theo policy đã khóa.

Regression bắt buộc:

- Grow/shrink trên sàn giữ chân; grow dưới trần không overlap và hoàn tất sau khi ra chỗ trống.
- Death trong persistent contact chỉ trừ một life; dying không di chuyển/bắn; sprite chạy hết clip trước transition.
- Respawn hai lần cho cùng kết quả; không giữ Held/Pressed/cooldown/contact từ đời trước.

Điều kiện qua Gate 0:

- Chỉ còn một FireBall production API và giới hạn 2 được integration-test.
- Mỗi defeat có đúng một cause, score event và SFX.
- Không còn static physics accumulator.
- Low-ceiling growth, death và respawn đều có deterministic regression test.

## 3. Gate 1 — Camera, display, UI và resource cleanup

### TV2 — Nhật

Task mở lại: `05, 06, 07, 08, 10, 11, 16, 19, 20, 22, 23, 25, 26, 27, 28`.

#### Camera theo metadata

- [ ] Thêm camera mode hoặc policy từ `LevelTheme`/LevelDefinition: Overworld khóa Y; map cao cho phép Y-follow trong dead-zone.
- [ ] Clamp cả X/Y sau follow và sau shake. Xử lý world nhỏ hơn viewport bằng cách center thay vì tạo min/max đảo.
- [ ] Test Overworld jump không đổi camera Y; tall map theo Y; đầu/cuối map không lộ ngoài bounds; shake ở bốn cạnh vẫn clamp.

#### Integer letterbox và input remap

- [ ] Bỏ logic ép resize cửa sổ về 16:9.
- [ ] Tính `scale = max(1, floor(min(windowW / 640, windowH / 360)))`; render logical texture ở integer scale, center trong viewport và tô bar đen phần dư.
- [ ] Tắt smoothing cho pixel-art render target/texture liên quan.
- [ ] Tạo một helper chuyển physical mouse coordinate → logical 640×360; click trong letterbox bar phải bị từ chối.
- [ ] Dùng cùng viewport rectangle cho render và mouse mapping để không lệch sau resize/fullscreen.

#### UI states và animation

- [ ] Cho Menu/Pause/GameOver/Win dùng chung `UILayoutHelper` và button hit rectangles.
- [ ] GameOver/Win hiển thị cả final score và high score từ SaveManager; Retry/Menu/Return hoạt động bằng keyboard và mouse.
- [ ] Death/spawn/grow/shrink clip có completion signal mà TV3 gọi/đợi; presentation giữ foot anchor theo physics policy.
- [ ] Chọn background theo Overworld/Underground/Castle, có fallback/log rõ nếu asset thiếu.
- [ ] Xóa `TextureManager::deleteTexture()` khỏi public API và implementation nếu không cần.
- [ ] Xóa/private hóa `AnimationSystem::stop/pause/resume` khi không có caller; nếu giữ, thêm caller/test đúng nghĩa.

Bằng chứng bắt buộc:

- Unit test viewport calculation, letterbox bars và mouse remap trong/ngoài content rectangle.
- Screenshot + click log tại 1280×720, 1600×900, 1920×1080 và 4:3; resize liên tục không stretch/crop/sai target.
- Keyboard/mouse đi được Menu → GameOver Retry/Menu và Win → Menu.

Điều kiện qua Gate 1:

- Overworld không nảy Y; map cao và shake không lộ ngoài bounds.
- Mọi kích thước cửa sổ dùng đúng logical 640×360, integer scale và click mapping.
- GameOver/Win đủ final/high score và keyboard/mouse.
- Không còn public resource/animation API không có contract/caller.

## 4. Gate 2 — Tài liệu, traceability và test coverage

### TV1 — Dương

Task mở lại: `01, 02, 14, 31, 32, 35`.

- [ ] **TV1-14:** viết PlayState integration test dùng Level/finish event thật hoặc một test seam tại đúng production boundary; xác nhận Level 3 completion snapshot final score và chỉ queue Win một lần. Sau đó chạy full loop thật ít nhất một lần.
- [ ] **TV1-01:** tách audit tracker thành từng deliverable của `WEEKLY_PLAN.md`; mỗi dòng có owner, task, status hiện tại, commit/test/manual evidence và blocker nếu còn mở.
- [ ] **TV1-02:** thêm toàn bộ bug đang mở từ Evaluate v3: camera/letterbox, growth clearance, static accumulator, duplicate FireBall API/limit, collision score, UI high score/mouse, docs và manual evidence.
- [ ] **TV1-31:** audit `FILE_STRUCTURE.md` bằng danh sách file thật; xóa mục phantom, đánh dấu archive/planned rõ, sửa owner/path/asset usage và bỏ ví dụ absolute path.
- [ ] **TV1-32:** cập nhật class diagram từ header hiện tại: ownership, lifecycle, EventBus, SaveManager, SoundManager, Level/TileMap, Entity/Character/Koopa, Texture/Animation và collision contracts.
- [ ] **TV1-35:** tạo integration report cho commit release candidate, ghi clean configure/build của ba preset, CTest, manual gates, conflict resolution và danh sách known issues bằng trạng thái thật.
- [ ] Chạy link checker cho toàn bộ Markdown; sửa link tương đối trong `TV1_CHANGES_SUMMARY.md` và mọi link tới file đã xóa.

Bằng chứng bắt buộc:

- Full loop `Menu → Level 1 → Level 2 → Level 3 → Win` với commit/hash và final progress.
- State tests cho flag/death race, Level 3 → Win one-shot và SaveManager caller thật.
- Markdown link checker không báo link nội bộ hỏng; class diagram không chứa API không tồn tại.

### TV3 — Bảo: cleanup và coverage

Task cleanup/test mở lại: `32, 33, 34, 35, 36, 37` cùng các runtime task ở Gate 0.

- [ ] Xóa `Mario::handleInput()` deprecated và dead members sau khi xác nhận không có caller.
- [ ] Chọn fixture/body-derived bounds làm source of truth; bỏ cache trùng hoặc cập nhật cache ở đúng một nơi.
- [ ] Chuyển scenario còn giá trị từ demo sang deterministic test rồi loại demo khỏi test/production scope.
- [ ] Mở rộng Mario suite: walk/run acceleration, short/full jump, no-double-jump, grow/shrink anchor, low ceiling, damage chain, grace/Star, death/respawn.
- [ ] Tạo collision regression suite qua production dispatcher: stomp, side, shell kick/kill, FireBall, Star, fixture order, persistent/multiple contact.

### TV5 — Truyền: scoring, asset docs và integration

Task mở lại: `16, 32, 41, 43, 44`.

- [ ] **TV5-16:** dùng shared defeat operation của Gate 0 làm source of truth cho score; giữ `ScoreRules` thuần để map cause → points, không để producer tự cộng điểm.
- [ ] **TV5-32:** map shell kick và shell kill thành hai event/SFX riêng; bỏ `ENEMY_STOMPED` thừa, thêm persistent-contact test.
- [ ] **TV5-41:** sửa `ASSETS_LIST.md` theo caller thật: Mushroom/item dùng đúng atlas, background nào thực sự runtime, TileMap dùng tileset nào; kiểm lại dimensions/path/package class.
- [ ] **TV5-43:** cập nhật report theo release-candidate commit và số suite hiện hành; tách rõ unit/helper coverage, runtime integration và manual evidence; bỏ claim không có bằng chứng.
- [ ] **TV5-44:** thêm tests đi qua EventBus/state/audio runtime cho Level 1–3 track, Star override/restore, death, GameOver, Win, Pause/resume và persisted volume. Phần không tự động hóa phải có checklist tái lập.

Điều kiện qua Gate 2:

- Tracker/register/report khớp cùng release-candidate commit và không claim vượt bằng chứng.
- Source tree/class diagram/asset docs khớp file và API thật; link checker xanh.
- Mario/collision/audio integration coverage bảo vệ các bug đã sửa ở Gate 0/1.

## 5. Gate 3 — Manual release acceptance

### TV4 — Vy

Task mở lại: `29, 40`.

- [ ] **TV4-29:** phối hợp TV3/TV5 đưa shell kill qua `DefeatCause::SHELL`; xác nhận victim chết, +200, event và SFX đúng một lần.
- [ ] **TV4-40:** chạy đủ năm playthrough trên mỗi Level 1, 2 và 3: normal, no-damage, death/respawn, power-up và flag route.
- [ ] Mỗi run ghi: commit, build preset, ngày/giờ, level/scenario, thời lượng, số death, item/combat path đã đi, expected, actual, PASS/FAIL và bug ID.
- [ ] Nếu run fail, sửa và chạy lại scenario bị ảnh hưởng; không sửa log cũ thành PASS.

### Ma trận release chung

- [ ] Full loop Menu → 1 → 2 → 3 → Win; score/coins/lives/power giữ đúng qua transition.
- [ ] Death còn life → reload; hết life → GameOver; Retry và Menu đều hoạt động.
- [ ] Pause/resume 20 lần trong một session; timer/input/music không drift hoặc mất state.
- [ ] Collect Mushroom, FireFlower, Star, 1-Up; stomp, shell kick/kill, FireBall kill và Star kill.
- [ ] Chạm flag thấp/cao; death và flag cùng frame chỉ commit một terminal result.
- [ ] Restart executable: high score, highest unlocked level và hai volume còn đúng.
- [ ] Chạy resolution matrix của TV2, kể cả click target trong content và letterbox bar.
- [ ] Debug/Release/Tests configure + build từ clean directory; CTest 100%; production compiler warning = 0.

Điều kiện qua Gate 3:

- Có 15/15 playthrough log PASS ở release-candidate commit.
- Full loop và manual matrix không còn P0/P1.
- Mọi task trong bảng mở lại có link tới automated test hoặc manual evidence tương ứng.

## 6. Thứ tự tích hợp đề xuất

1. TV3 chuyển accumulator, hợp nhất FireBall API và collision dispatcher.
2. TV4/TV5 nối shell/score/SFX vào shared defeat path; TV3 hoàn thiện Star/growth/death/respawn.
3. TV2 merge camera policy, integer letterbox/mouse remap, UI/animation/background cleanup.
4. TV3/TV5 hoàn thiện regression/integration suites.
5. TV1 chạy full-loop integration, cập nhật tracker/register/structure/diagram và tạo release-candidate report.
6. TV2 chạy resolution matrix; TV4 chạy 15 playthrough; TV5 chạy audio/HUD/item checklist.
7. Cả nhóm chỉ sửa regression phát hiện ở release gates; không thêm Luigi, level editor, Bowser, multiplayer hoặc mechanic mới.

Không nên chạy 15 playthrough trước khi Gate 0 và Gate 1 merge, vì kết quả sẽ không đại diện cho release candidate cuối.

## 7. Definition of Done cuối Sprint

Sprint chỉ được đóng khi tất cả điều kiện sau cùng đúng trên **một commit release candidate**:

- [ ] 184/184 task đạt `Tốt` hoặc `Rất tốt`.
- [ ] Clean Debug/Release/Tests build pass; CTest 100%; production warning = 0.
- [ ] Một FireBall API, tối đa hai active và one-shot event/SFX.
- [ ] Một collision/defeat/score path cho stomp/shell/FireBall/Star.
- [ ] Growth dưới trần, death animation và respawn deterministic.
- [ ] Camera Y policy, clamp, integer letterbox và mouse remap đúng ở toàn bộ matrix.
- [ ] Menu/GameOver/Win/Pause đủ keyboard/mouse và final/high score.
- [ ] Save/high score/unlock/volume tồn tại sau restart.
- [ ] 15 playthrough, full-loop, pause cycle, resolution và audio/HUD/item evidence đều PASS.
- [ ] Tracker, bug register, README, file structure, class diagram, asset docs và reports khớp code; không còn link nội bộ hỏng.

Nếu bất kỳ mục nào chưa có bằng chứng, task tương ứng giữ `REVIEW`, `BLOCKED` hoặc `REOPENED`; không chuyển `DONE` chỉ dựa trên build/test tổng quát.

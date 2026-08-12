# Kế hoạch sửa Sprint 6 — vòng 2

## 1. Mục tiêu và phạm vi

Kế hoạch này xử lý **81 task chưa đạt** trong [Evaluate_v2.md](D:/Mario/CS202-Group04-FinalProject/Evaluate_v2.md:1). Mục tiêu đóng Sprint 6 là đưa toàn bộ 184 task lên `Tốt` hoặc `Rất tốt`, giữ đúng owner trong [s6_plan.md](D:/Mario/CS202-Group04-FinalProject/docs/management/s6_plan.md:1), không thêm feature ngoài scope.

Nguyên tắc thực hiện:

- Sửa hợp đồng chung trước khi tiếp tục viết feature phụ thuộc.
- Mỗi PR ghi task ID, hành vi trước/sau, automated test và bằng chứng manual nếu DoD yêu cầu.
- Không nới assertion để chấp nhận hành vi sai; test phải bảo vệ release contract.
- TV1 chịu trách nhiệm integration/merge, nhưng owner module vẫn sửa và review phần của mình.
- Các task đã `Tốt/Rất tốt` chỉ chạy regression, không rewrite không cần thiết.
- Chỉ cập nhật tracker/report sau khi bằng chứng build/test/manual đã tồn tại.

### Danh sách task phải mở lại

| Owner | Task ID mở lại | Số lượng |
|---|---|---:|
| TV1 | `01, 02, 03, 14, 18, 19, 28, 30, 31, 32, 33, 35` | 12 |
| TV2 | `05, 06, 07, 08, 10, 11, 16, 17, 19, 20, 22, 23, 24, 25, 26, 27, 28` | 17 |
| TV3 | `01, 09, 10, 11, 13, 14, 15, 16, 17, 18, 19, 25, 26, 27, 28, 31, 32, 33, 34, 35, 36, 37` | 22 |
| TV4 | `01, 13, 14, 15, 16, 17, 18, 19, 20, 29, 33, 35, 36, 40` | 14 |
| TV5 | `02, 06, 09, 10, 11, 15, 16, 19, 21, 28, 36, 37, 39, 41, 43, 44` | 16 |
| **Tổng** |  | **81** |

## 2. Gate 0 — Khôi phục release contract

Gate này phải merge trước các fix gameplay khác để tránh tiếp tục xây trên interface sai.

- [ ] **TV1 + TV3 + TV5 — Mario state:** xóa `FIRE_SMALL` khỏi release code; giữ đúng `SMALL`, `SUPER`, `FIRE`. FireFlower luôn đưa Mario vào `FIRE`; cập nhật HUD, tests, class diagram và locked-interface doc.
- [ ] **TV1 — Character scope:** trả nhân vật mặc định về Mario. Nếu muốn giữ thử nghiệm Luigi, tách khỏi release flow hoặc để sau Sprint 6; không thay default khi chưa có product requirement/UI lựa chọn.
- [ ] **TV3 — FireBall contract:** chỉ một đường bắn chính, phím `X`, cooldown 250 ms và tối đa hai projectile active. `Shift` chỉ dùng cho Run.
- [ ] **TV3 + TV4 + TV5 — Defeat contract:** thêm `DefeatCause::{STOMP, SHELL, FIREBALL, STAR}` và một shared defeat/score event path.
- [ ] **TV1 + TV3 — Death contract:** một lần chết trừ đúng một life và phát đúng một `PLAYER_DIED`; PlayState quyết định Reload hay GameOver từ số life còn lại, không phát thêm terminal event cạnh tranh.
- [ ] **TV1 — Test contract:** đổi assertion FireFlower về đúng một kết quả `MarioState::FIRE`; thêm test fail nếu `FIRE_SMALL` hoặc limit 4 quay lại.

Điều kiện qua Gate 0:

- Không còn `FIRE_SMALL` trong production code, docs hoặc tests.
- Shift không tạo FireBall; X không ảnh hưởng Run.
- Một defeat chỉ cộng score/phát event một lần.
- Một death chỉ trừ một life và phát một death event.

## 3. Phân công sửa theo thành viên

### TV1 — Dương: Integration, state, build và tài liệu

Task mở lại: `01, 02, 03, 14, 18, 19, 28, 30, 31, 32, 33, 35`.

- [ ] **S6-TV1-03/18:** chốt lại event/state contract sau Gate 0. Bỏ cặp `PLAYER_DIED` + `PLAYER_LOST_LIFE` cho cùng một death; giữ race guard để death và flag không cùng commit.
- [ ] **S6-TV1-14:** bổ sung state regression cho `Level 3 completion → Win`, xác nhận final score snapshot đúng và Win chỉ push một lần. Chỉ đóng sau khi Level 3 thật sự load/complete được.
- [ ] **S6-TV1-19:** inject một `SaveManager` dùng chung ở composition root/session; Menu gọi load, Win/GameOver cập nhật high score, level completion cập nhật unlock. Không tạo SaveManager thứ hai trong state.
- [ ] **S6-TV1-28:** đổi production target `main` thành tên rõ ràng, đề xuất `SuperMario`; cập nhật ba preset, DLL copy, asset copy và lệnh run tương ứng. Chạy lại từ clean directories.
- [ ] **S6-TV1-01/02/35:** cập nhật tracker và bug register bằng commit/test/log thật. Xóa trạng thái SaveManager `BLOCKED`, sửa 7/7 thành suite hiện hành, ghi rõ owner và reproduction cho mọi P0/P1/P2 còn mở.
- [ ] **S6-TV1-30/31/32:** đồng bộ README, `FILE_STRUCTURE.md` và class diagram với file/API hiện tại; bỏ file không tồn tại, số liệu asset sai, control Shift-Shoot và API draft.
- [ ] **S6-TV1-33:** tìm và archive Sprint 5 error plan với banner `LEGACY`; nếu tài liệu nguồn không còn trong repository, xóa mọi link/claim rằng nó tồn tại và ghi rõ lý do trong tracker.

Bằng chứng bắt buộc:

- Automated state tests cho `death → reload`, `death → GameOver`, `flag/death same frame`, `Level 3 → Win` và SaveManager integration.
- Link checker nội bộ không còn link tới file không tồn tại.
- Debug/Release/Tests configure và build từ clean directory với executable mới.

### TV2 — Nhật: Camera, scaling, UI và animation

Task mở lại: `05, 06, 07, 08, 10, 11, 16, 17, 19, 20, 22, 23, 24, 25, 26, 27, 28`.

- [ ] **S6-TV2-05–08:** thêm camera mode theo level metadata. Overworld khóa Y; map cao theo target trong dead-zone; luôn clamp X/Y sau shake. Xử lý riêng level nhỏ hơn viewport.
- [ ] **S6-TV2-10/11:** bỏ việc ép kích thước cửa sổ. Tính `integerScale = floor(min(windowW/640, windowH/360))`, căn RenderTexture giữa cửa sổ và dùng vùng đen letterbox. Tắt smoothing.
- [ ] **S6-TV2-16/17/19/20:** dùng `UILayoutHelper` cho Menu/Pause/GameOver/Win; bổ sung mapping chuột từ physical window qua letterbox vào logical 640×360. Menu/GameOver/Win đọc và hiển thị high score từ SaveManager.
- [ ] **S6-TV2-22/23:** death/spawn/grow/shrink clip phải có completion signal. Trong death animation Mario vẫn visible nhưng input/physics bị khóa; grow/shrink giữ nguyên world-space foot và không pop sprite.
- [ ] **S6-TV2-24:** chỉ dùng `assets/fonts/mario.ttf`; khi load fail log rõ và degrade có kiểm soát, không truy cập `C:/Windows/Fonts/...`.
- [ ] **S6-TV2-25/26:** xóa `TextureManager::deleteTexture()`; xóa hoặc private hóa `AnimationSystem::stop/pause/resume` nếu không có caller.
- [ ] **S6-TV2-27:** chọn background theo `LevelTheme` cho Overworld, Underground, Castle.
- [ ] **S6-TV2-28:** ghi resolution matrix cho 1280×720, 1600×900, 1920×1080, 4:3 và resize liên tục; xác nhận HUD không crop và click target đúng.

Bằng chứng bắt buộc:

- Unit test cho tính viewport/letterbox và coordinate remap.
- Camera tests cho overworld Y-lock, tall-map Y tracking, start/end clamp và shake clamp.
- Screenshot/log của bốn độ phân giải; keyboard và mouse đều đi qua Menu/GameOver, Win quay về Menu.

### TV3 — Bảo: Physics, Mario, FireBall và collision

Task mở lại: `01, 09, 10, 11, 13, 14, 15, 16, 17, 18, 19, 25, 26, 27, 28, 31, 32, 33, 34, 35, 36, 37`.

- [ ] **S6-TV3-01:** chuyển accumulator từ `static` sang member của từng PhysicsEngine/world; reset khi tạo/load Level. Giữ maximum substeps và test hai world độc lập.
- [ ] **S6-TV3-09/13/14:** áp dụng ba Mario state chuẩn; tách hoàn toàn damage-grace timer khỏi Star timer; gọi timer update đúng một lần/frame. Star expiry phải phát event khôi phục HUD/music, kể cả khi overlap damage grace.
- [ ] **S6-TV3-10/11:** khi grow/shrink, giữ nguyên điểm chân. Nếu trần thấp, lưu pending upgrade và chỉ rebuild fixture khi đủ clearance; không tạo fixture cao xuyên tile.
- [ ] **S6-TV3-15/16:** death sequence gồm khóa input/physics, giữ sprite active để phát clip, trừ life một lần, phát event một lần rồi để PlayState reload/GameOver. Respawn reset body, velocity, contacts, input, animation và power đúng policy.
- [ ] **S6-TV3-17–19:** hợp nhất `Level::spawnFireBall()` và `Level::shootFireBall()` thành một API; mọi đường normal/queued đều đi qua Mario cooldown, gắn owner, phát `FIREBALL_SHOT` khi projectile thật sự được tạo và giới hạn hai active.
- [ ] **S6-TV3-25–28/31:** chọn một nơi duy nhất xử lý gameplay collision. Callback entity không được xử lý lại sau handler trung tâm. Chuẩn hóa normal độc lập fixture order; stomp/side/FireBall/shell/Star dùng `DefeatCause` và một score event.
- [ ] **S6-TV3-32–35:** xóa `Mario::handleInput()` deprecated, dead members (`m_moveSpeed`, `m_wasJumpPressed` nếu còn), bỏ bounding-box cache trùng và chuyển scenario hữu ích khỏi demo sang tests.
- [ ] **S6-TV3-36/37:** mở rộng suite để bao phủ walk/run, growth anchor, low ceiling, damage chain, death/respawn, contact order, shell, FireBall owner/limit và Star kill.

Bằng chứng bắt buộc:

- Test xác nhận Star kéo đủ 10 giây theo simulated dt và expiry event chỉ phát một lần.
- Test hai FireBall active, viên thứ ba bị từ chối, projectile inactive không bị đếm; queued shot vẫn có owner/cooldown/event.
- Test mỗi enemy chỉ nhận một defeat và một score khi callback kéo dài hoặc nhiều fixture contact.
- Test death sprite completion trước reload, không trừ hai lives.

### TV4 — Vy: Level data, validator, shell và SaveManager handoff

Task mở lại: `01, 13, 14, 15, 16, 17, 18, 19, 20, 29, 33, 35, 36, 40`.

- [ ] **S6-TV4-01/20:** biến Level 0 thành fixture được test thật; thêm validator tests cho width, symbol, thiếu/trùng M/F và pole không hợp lệ.
- [ ] **S6-TV4-13–19:** chuẩn hóa cả ba file release chỉ chứa grammar hợp lệ và các hàng cùng chiều rộng. Level 1 có tutorial route; Level 2 có Underground/Pipe/Koopa/shell/FireFlower; Level 3 có mixed enemies/Star/final gauntlet và route tới cờ.
- [ ] **S6-TV4-14/16/18:** đo jump envelope/spawn visibility/fairness bằng thông số và playthrough log, không chỉ đọc map bằng mắt.
- [ ] **S6-TV4-29:** shell kill gọi shared defeat path của TV3 với `DefeatCause::SHELL`; một victim chỉ chết/cộng 200 điểm/phát SFX một lần.
- [ ] **S6-TV4-33/35/36:** cung cấp API/test handoff để TV1/TV5 tích hợp high score, highest unlocked level và audio settings vào runtime; thêm integration test restart session.
- [ ] **S6-TV4-40:** thực hiện năm playthrough cho mỗi level: normal, no-damage, death/respawn, power-up và flag route. Ghi thời gian, death, bug ID và kết quả pass/fail từng run.

Bằng chứng bắt buộc:

- Validator test xanh và tool/fixture chứng minh Level 1–3 đều load được.
- Level 1 → 2 → 3 → Win chạy liên tục ít nhất một lần trước khi vào full playthrough matrix.
- Save test gồm restart và caller thật từ Menu/completion/Pause, không chỉ gọi trực tiếp SaveManager.

### TV5 — Truyền: Input, item, HUD, audio và assets

Task mở lại: `02, 06, 09, 10, 11, 15, 16, 19, 21, 28, 36, 37, 39, 41, 43, 44`.

- [ ] **S6-TV5-02/06:** Shift chỉ bind Run Held, X bind Shoot Pressed; bổ sung tests cho press/release đồng thời, repeat và suppression trong Pause/death/transition.
- [ ] **S6-TV5-09/11:** FireFlower luôn tạo `FIRE`; factory phải tạo `?` với `ADAPTIVE`, resolve đúng một lần: SMALL → Mushroom, SUPER/FIRE → FireFlower.
- [ ] **S6-TV5-10/21/28:** Star chỉ cấp timed invincibility; HUD hiện STAR trong hiệu lực rồi về state thật; Star music override và khôi phục đúng track level qua một expiry event.
- [ ] **S6-TV5-15/16:** 100 coin giữ remainder, tăng một life và phát đúng `ONE_UP_COLLECTED`; mọi score dùng shared API: coin 100, power-up 1000, stomp 100, shell/FireBall/Star 200.
- [ ] **S6-TV5-19:** timeout gọi cùng death request chuẩn đúng một lần và không cạnh tranh với flag/death đang pending.
- [ ] **S6-TV5-36:** Pause chỉnh Music/SFX riêng, phản hồi ngay và gọi SaveManager để volume còn đúng sau restart.
- [ ] **S6-TV5-37/39/41:** kiểm kê mọi asset; chuyển các atlas phân tích/source ra khỏi runtime package hoặc loại khỏi CMake copy list. Sửa manifest theo file thật, tối thiểu tileset `680×776`, enemies `436×530`, Goomba `96×32`, Koopa `128×48`, items blocks `448×256`, items objects `592×572`.
- [ ] **S6-TV5-43:** sửa engineering report và `change_in_test_game.md`: bỏ claim Level 3 valid, Star đủ 10 giây, FireBall constructor không gọi null, 4 FireBall đúng spec và mọi test count cũ.
- [ ] **S6-TV5-44:** mở rộng automated integration tests; điền checklist audio/HUD/item bằng kết quả tái lập, commit hash và bug ID nếu fail.

Bằng chứng bắt buộc:

- Test adaptive block, FireFlower, Star timeout/expiry, 100-coin threshold và score từng DefeatCause.
- Audio transition test cho Level 1–3, Star, death, GameOver, Win và Pause/resume.
- Asset manifest checker đối chiếu path, dimensions và package classification với file thật.

## 4. Thứ tự tích hợp

### Gate 1 — Full game loop tối thiểu

1. Merge Gate 0 contracts.
2. TV4 sửa Level 1–3 và validator tests.
3. TV1 + TV3 sửa death/respawn/GameOver và Win transition.
4. TV3 + TV5 sửa Run/Shoot/FireBall/Star/score.

Gate đạt khi:

- Cả ba level load và complete được.
- Chết khi còn life reload đúng; hết life sang GameOver; Retry/Menu hoạt động.
- Level 3 chỉ chuyển Win một lần.
- Shift/X, FireFlower, Star và tối đa hai FireBall đúng contract.

### Gate 2 — Display, UI, save và audio

1. TV2 merge integer scaling/letterbox/camera trước UI coordinate mapping.
2. TV1 + TV4 nối SaveManager vào state/session.
3. TV2 hoàn thiện Menu/GameOver/Win; TV5 nối volume và music lifecycle.
4. TV2/TV5 hoàn thiện background, HUD, font và asset package.

Gate đạt khi:

- 1280×720, 1600×900, 1920×1080 và 4:3 không stretch/crop/sai click.
- High score, unlocked level và hai volume tồn tại sau restart executable.
- Star/death/GameOver/Win/Pause không làm mất hoặc overlap track.

### Gate 3 — Regression, manual QA và documentation

1. TV3/TV4/TV5 hoàn thiện các suite còn thiếu.
2. TV4 chạy 15 playthrough có log; TV2 chạy resolution matrix; TV5 điền checklist.
3. TV1 chạy clean build/test, audit link/file/API và cập nhật tracker cuối cùng.
4. Chỉ sửa regression phát hiện ở gate; không thêm Luigi, state mới hay feature ngoài Sprint 6.

## 5. Definition of Done cuối Sprint

### Automated gate

- [ ] Configure/build sạch `mingw-debug`, `mingw-release`, `mingw-tests`.
- [ ] 100% CTest pass; không có test nới assertion để chấp nhận interface sai.
- [ ] Không có compiler warning mới trong production target.
- [ ] Có coverage cho state/death/Win, camera/viewport, physics/Mario/collision, level validator, SaveManager integration, input/item/HUD/audio và asset manifest.
- [ ] Search xác nhận không còn `FIRE_SMALL`, default Luigi, `MAX_ACTIVE_FIREBALLS = 4`, absolute font path hoặc public dead APIs đã nêu.

### Manual gate

- [ ] Chơi liên tục `Menu → Level 1 → Level 2 → Level 3 → Win`.
- [ ] Chạy death → respawn, death → GameOver, Retry và Menu.
- [ ] Năm playthrough mỗi level có log.
- [ ] Pause/resume ít nhất 20 lần ở nhiều state; timer/music/input không lệch.
- [ ] Kiểm tra bốn nhóm độ phân giải và resize liên tục.
- [ ] Kiểm tra Mushroom, FireFlower, Star, 1-Up, stomp, shell, hai FireBall, timeout và flag cao/thấp.
- [ ] Restart game để xác nhận high score, unlock level và volume.

### Documentation gate

- [ ] Tracker, bug register, README, file structure, class diagram, asset manifest và engineering notes khớp code/commit cuối.
- [ ] Không có link nội bộ hỏng, file tưởng tượng, dimensions sai hoặc test count cũ.
- [ ] Mỗi task trong `s6_plan.md` có bằng chứng tương ứng; không còn task ở `Chưa tốt`, `Hoàn toàn sai` hoặc `Chưa làm`.

Sprint 6 chỉ được đóng sau khi cả ba gate trên đều đạt. Build xanh và 9/9 test hiện tại là baseline tốt, nhưng chưa thay thế full-loop, integration và manual acceptance evidence.

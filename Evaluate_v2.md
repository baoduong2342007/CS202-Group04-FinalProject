# Đánh giá Sprint 6 — vòng 2

## 1. Kết luận

Sprint 6 **chưa đạt Definition of Done**. Đánh giá lại toàn bộ 184 task trong [s6_plan.md](D:/Mario/CS202-Group04-FinalProject/docs/management/s6_plan.md:1) trên commit `881335d` cho kết quả **103/184 task (56,0%)** đạt mức `Tốt` hoặc `Rất tốt`; còn **81 task** phải mở lại.

Kết quả xác minh kỹ thuật:

- Build preset `mingw-debug`: thành công.
- Build preset `mingw-release`: thành công.
- Build preset `mingw-tests`: thành công.
- CTest: **9/9 pass** (`input_state`, `tile_collision_span`, `mario_physics`, `play_state`, `game_manager`, `event_bus`, `tv5_integration`, `level_catalog`, `save_manager`).
- Không thấy compiler warning từ production code; chỉ còn CMake deprecation warning của Box2D.
- Các task yêu cầu playthrough/manual QA không được công nhận hoàn thành khi checklist hoặc log kết quả vẫn trống.

So với [Evaluate.md](D:/Mario/CS202-Group04-FinalProject/Evaluate.md:1), số task đạt tăng từ **85 lên 103** (`+18`). Đây là đánh giá lại từ trạng thái code hiện tại, không phải cộng điểm máy móc từ lần trước. Task vẫn được tính cho owner gốc trong `s6_plan.md`; 19 thay đổi ngoài lề của TV1 được đánh giá riêng và không cộng vào mẫu số 184 task.

### Quy ước phân loại

- **Rất tốt:** Đạt toàn bộ DoD, implementation rõ ràng và có bằng chứng test/build mạnh.
- **Tốt:** Đạt DoD; có thể còn phần polish nhỏ nhưng không làm sai hành vi yêu cầu.
- **Chưa tốt:** Đã có phần implementation đáng kể nhưng còn lỗi, thiếu tích hợp, thiếu test hoặc thiếu bằng chứng nghiệm thu.
- **Hoàn toàn sai:** Implementation đi ngược hợp đồng/DoD hoặc tạo hành vi sai ở đường chạy chính.
- **Chưa làm:** Không tìm thấy implementation hay bằng chứng có ý nghĩa cho task.

## 2. Các blocker chính

1. **Level 3 không qua validator và không thể đi tới Win.** File bắt đầu bằng các dòng mô tả chứa `#`, có chiều dài hàng không đồng nhất và ký hiệu ngoài grammar, ngay từ [level3.txt](D:/Mario/CS202-Group04-FinalProject/levels/level3.txt:1). Vì vậy các claim “Level 3 qua validator 100%” trong changelog không đúng.
2. **Death contract vẫn sai.** Một lần chết còn phát cả `PLAYER_DIED` và `PLAYER_LOST_LIFE` tại [Mario.cpp](D:/Mario/CS202-Group04-FinalProject/src/entities/Mario.cpp:642); Mario đồng thời bị đặt inactive nên [Entity.cpp](D:/Mario/CS202-Group04-FinalProject/src/entities/Entity.cpp:56) không vẽ death sprite. Điều này trái DoD “một death chỉ phát một event” và làm death clip không thể hiện đúng.
3. **Render/camera chưa đạt hợp đồng 640×360.** [Game.cpp](D:/Mario/CS202-Group04-FinalProject/src/core/Game.cpp:88) ép lại kích thước cửa sổ và dùng continuous scaling thay vì integer scale + letterbox; [Camera.cpp](D:/Mario/CS202-Group04-FinalProject/src/level/Camera.cpp:72) luôn theo trục Y và chủ động bỏ Y clamp.
4. **SaveManager chỉ là module độc lập.** `SaveManager` và test của nó tốt, nhưng không có caller trong Menu/Win/GameOver/Pause; high score, unlock level và volume vì thế chưa tồn tại qua một session game thật.
5. **Mario/FireBall/Star còn regression nghiêm trọng.** `updateInvincibility(dt)` bị gọi hai lần trong một frame tại [Mario.cpp](D:/Mario/CS202-Group04-FinalProject/src/entities/Mario.cpp:260) và [Mario.cpp](D:/Mario/CS202-Group04-FinalProject/src/entities/Mario.cpp:288); nhánh Star hết hạn không phát `PLAYER_INVINCIBILITY_EXPIRED`. Giới hạn FireBall đang là 4 thay vì 2 tại [Level.cpp](D:/Mario/CS202-Group04-FinalProject/src/level/Level.cpp:412), còn đường bắn chính không gắn owner tại [Level.cpp](D:/Mario/CS202-Group04-FinalProject/src/level/Level.cpp:436).
6. **Collision xử lý trùng đường.** [CollisionManager.cpp](D:/Mario/CS202-Group04-FinalProject/src/physics/CollisionManager.cpp:191) gọi callback đa hình của cả hai entity rồi tiếp tục chạy handler trung tâm; chưa có `DefeatCause`, nên stomp/shell/FireBall/Star chưa bảo đảm một defeat và một score event.
7. **Interface và asset documentation không khớp release contract.** `FIRE_SMALL` được thêm dù task khóa chỉ cho `SMALL/SUPER/FIRE`; `?` vẫn được factory tạo cố định thành Mushroom tại [EntityFactory.cpp](D:/Mario/CS202-Group04-FinalProject/src/patterns/EntityFactory.cpp:62). [ASSETS_LIST.md](D:/Mario/CS202-Group04-FinalProject/assets/ASSETS_LIST.md:26) ghi tileset `128×32`, trong khi file thật là `680×776`; manual checklist của TV5 vẫn chưa được điền.
8. **Build target và tài liệu quản lý còn claim sai.** Production executable vẫn tên `main` tại [CMakeLists.txt](D:/Mario/CS202-Group04-FinalProject/CMakeLists.txt:136); audit tracker còn ghi SaveManager blocked và 7/7 test, còn `FILE_STRUCTURE.md` liệt kê nhiều file không tồn tại.

## 3. Tổng hợp

| Thành viên | Rất tốt | Tốt | Chưa tốt | Hoàn toàn sai | Chưa làm | Đạt DoD |
|---|---:|---:|---:|---:|---:|---:|
| TV1 — Dương | 12 | 11 | 8 | 2 | 2 | 23/35 — 65,7% |
| TV2 — Nhật | 5 | 6 | 8 | 5 | 4 | 11/28 — 39,3% |
| TV3 — Bảo | 7 | 8 | 12 | 6 | 4 | 15/37 — 40,5% |
| TV4 — Vy | 11 | 15 | 10 | 1 | 3 | 26/40 — 65,0% |
| TV5 — Truyền | 8 | 20 | 11 | 5 | 0 | 28/44 — 63,6% |
| **Tổng** | **43** | **60** | **49** | **19** | **13** | **103/184 — 56,0%** |

### Thay đổi so với lần đánh giá trước

| Thành viên | Lần trước | Lần này | Chênh lệch task đạt |
|---|---:|---:|---:|
| TV1 | 25/35 | 23/35 | -2 |
| TV2 | 6/28 | 11/28 | +5 |
| TV3 | 14/37 | 15/37 | +1 |
| TV4 | 14/40 | 26/40 | +12 |
| TV5 | 26/44 | 28/44 | +2 |
| **Tổng** | **85/184** | **103/184** | **+18** |

TV4 tiến bộ nhiều nhất nhờ hoàn thiện enemy activation/cleanup, Koopa và toàn bộ nền SaveManager. TV2 đã sửa logical resolution, game loop và một phần UI. TV1 bị giảm điểm do tài liệu quản lý trở nên stale sau merge và các thay đổi ngoài scope làm lệch interface đã khóa.

## 4. Phân loại chi tiết theo ID

### TV1 — Dương

- **Rất tốt:** `06, 15, 16, 20, 22, 23, 24, 25, 26, 27, 29, 34`
- **Tốt:** `04, 05, 07, 08, 09, 10, 11, 12, 13, 17, 21`
- **Chưa tốt:** `01, 02, 03, 14, 28, 30, 32, 35`
- **Hoàn toàn sai:** `18, 31`
- **Chưa làm:** `19, 33`

Điểm mạnh là state stack/deferred operations, EventBus lifetime, ownership, LevelCatalog/GameProgress và build bootstrap. Các điểm phải mở lại là death event, SaveManager integration, Win end-to-end, tên executable và bộ tài liệu traceability. `S6_AUDIT_TRACKER.md` vẫn mô tả trạng thái trước merge; `FILE_STRUCTURE.md` liệt kê file không tồn tại; Sprint 5 error plan chưa được archive.

### TV2 — Nhật

- **Rất tốt:** `03, 04, 12, 13, 14`
- **Tốt:** `01, 02, 09, 15, 18, 21`
- **Chưa tốt:** `06, 07, 08, 16, 17, 19, 20, 23`
- **Hoàn toàn sai:** `05, 10, 11, 22, 24`
- **Chưa làm:** `25, 26, 27, 28`

DisplayConfig 1280×720/640×360, HUD default view, X dead-zone, close ordering, delta clamp, frame cap và fade đã có. Tuy nhiên overworld camera vẫn nảy theo Y, không có Y clamp, scaling không letterbox/integer, UI helper chưa dùng cho Menu, các state thiếu high score/mouse, death clip không có completion signal, font fallback tuyệt đối vẫn còn, và chưa có theme background hay resolution matrix.

### TV3 — Bảo

- **Rất tốt:** `04, 05, 06, 07, 08, 29, 30`
- **Tốt:** `02, 03, 12, 20, 21, 22, 23, 24`
- **Chưa tốt:** `10, 13, 14, 16, 17, 18, 25, 26, 27, 28, 35, 36`
- **Hoàn toàn sai:** `01, 09, 11, 15, 19, 31`
- **Chưa làm:** `32, 33, 34, 37`

Movement/jump, max-substeps, bounds, deferred destruction và đường render/bounce FireBall đã tiến bộ. Blocker còn lại là accumulator vẫn `static`, state thừa `FIRE_SMALL`, low-ceiling growth không defer, death animation/respawn không đúng, limit FireBall bằng 4, không có `DefeatCause`, collision bị double-dispatch và test suite còn thiếu growth/damage/death/collision.

### TV4 — Vy

- **Rất tốt:** `04, 05, 06, 07, 12, 21, 22, 32, 34, 38, 39`
- **Tốt:** `02, 03, 08, 09, 10, 11, 23, 24, 25, 26, 27, 28, 30, 31, 37`
- **Chưa tốt:** `01, 13, 14, 15, 16, 19, 29, 33, 35, 36`
- **Hoàn toàn sai:** `17`
- **Chưa làm:** `18, 20, 40`

Validator, tile semantics, flag trigger, enemy activation/cleanup, Koopa state machine và SaveManager module đều là phần cải thiện rõ nhất. Level 3 hiện không load được; Level 2 thiếu FireFlower theo DoD; shell kill chưa dùng shared defeat/score path; SaveManager chưa được nối vào Menu/completion/audio; không có validator tests hoặc năm playthrough cho mỗi level.

### TV5 — Truyền

- **Rất tốt:** `01, 04, 07, 22, 24, 34, 35, 38`
- **Tốt:** `03, 05, 08, 12, 13, 14, 17, 18, 20, 23, 25, 26, 27, 29, 30, 31, 32, 33, 40, 42`
- **Chưa tốt:** `02, 06, 10, 15, 16, 19, 21, 28, 36, 37, 44`
- **Hoàn toàn sai:** `09, 11, 39, 41, 43`
- **Chưa làm:** không có

Input state, item single-collect guard, timer/HUD formatting, MusicId/mapping, voice pool và volume clamp đã có chất lượng tốt. Các lỗi chính là Shift vừa Run vừa Shoot, FireFlower đi vào `FIRE_SMALL`, factory vô hiệu hóa adaptive `?`, Star hết hạn không phát event phục hồi HUD/music, 100 coin thiếu 1-Up event, score shell/FireBall/Star không qua một shared path, volume chưa persist, source atlas vẫn bị package và asset/report documentation còn nhiều claim sai.

## 5. Review 19 thay đổi ngoài lề của TV1

Các mục dưới đây lấy theo thứ tự Entry trong [change_in_test_game.md](D:/Mario/CS202-Group04-FinalProject/docs/change_in_test_game.md:68). Chúng **không thay đổi điểm 184 task chính thức**.

| Entry | Mức | Kết luận ngắn |
|---:|---|---|
| 1 | Chưa tốt | Star timer đã chạy nhưng bị tick hai lần mỗi frame, còn khoảng 5 giây thay vì 10; nhánh Star hết hạn không phát expiry event. |
| 2 | Chưa tốt | BlockDebris được nối texture/update; multi-theme QuestionBlock tồn tại nhưng TileMap/factory không truyền theme nên đường runtime vẫn dùng Overworld mặc định. |
| 3 | Hoàn toàn sai | Đổi nhân vật mặc định sang Luigi đi ngược nguyên tắc Sprint 6 “không thêm Luigi” và không có UI chọn nhân vật. |
| 4 | Chưa tốt | Có tài liệu phân tích tọa độ, nhưng changelog link tới `items_objects_atlas.png` không tồn tại; file thật mang tên `items_objects_all_components_atlas.png`. |
| 5 | Rất tốt | Các helper `SpriteFrames::Items` đã đồng bộ call site và cả ba build preset đều thành công. |
| 6 | Chưa tốt | Reset animation/character có thực hiện, nhưng Mario bị inactive ngay khi chết nên death sprite vẫn không được vẽ. |
| 7 | Chưa tốt | FireBall hoạt động trong main game, nhưng gán cả Shift/J/F/X/Ctrl gây xung đột control; còn hai API bắn song song. |
| 8 | Chưa tốt | Fire hit và action frame đã có, nhưng dùng `ENEMY_STOMPED`, thiếu owner/score và chưa theo `DefeatCause::FIREBALL`. |
| 9 | Chưa tốt | Queue tránh tạo body khi Box2D locked, nhưng đường queue bỏ qua cooldown, owner và event contract. |
| 10 | Hoàn toàn sai | Hiệu ứng enemy flip có ích, nhưng thêm `FIRE_SMALL` phá interface SMALL/SUPER/FIRE đã khóa và làm FireFlower sai DoD. |
| 11 | Hoàn toàn sai | Chỉ mở rộng tiếp một state không hợp lệ; không phải fix đúng contract Sprint 6. |
| 12 | Chưa tốt | Log null được làm im, nhưng constructor mặc định của FireBall vẫn gọi `initPhysics(nullptr)` tại [FireBall.cpp](D:/Mario/CS202-Group04-FinalProject/src/entities/FireBall.cpp:23), trái chính claim trong changelog. |
| 13 | Hoàn toàn sai | Cài đúng giới hạn 4 theo changelog nhưng trái task chính thức yêu cầu tối đa 2 FireBall active. |
| 14 | Tốt | Spawn offset, ignore Mario và wall grace 50 ms đã được triển khai nhất quán; cần thêm regression test. |
| 15 | Chưa tốt | Merge và test pass, nhưng assertion được nới để chấp nhận `FIRE_SMALL` thay vì bắt regression theo interface đã khóa. |
| 16 | Chưa tốt | Merge/resolve conflict và SaveManager tests thành công, nhưng claim Level 3 qua validator 100% là sai. |
| 17 | Chưa tốt | Merge sạch và nhiều phần game loop/UI có ích, nhưng “khóa 16:9” hiện là ép resize + continuous scale, chưa phải letterbox/integer scaling. |
| 18 | Rất tốt | Dọn include thừa, build Debug/Release/Tests không phát sinh warning production. |
| 19 | Tốt | Xử lý đủ nhánh switch và hết warning; đây chỉ là cleanup cục bộ, không hợp thức hóa việc giữ `FIRE_SMALL`. |

Tổng riêng 19 entry: **2 Rất tốt, 2 Tốt, 11 Chưa tốt, 4 Hoàn toàn sai, 0 Chưa làm**. Bốn entry `3, 10, 11, 13` cần rollback hoặc chỉnh theo release contract trước khi tiếp tục sửa các module phụ thuộc.

## 6. Kết luận cuối

Branch hiện tại đạt gate biên dịch và unit test cơ bản, đồng thời có tiến bộ lớn ở SaveManager, enemy lifecycle, input/HUD/audio và state infrastructure. Tuy vậy, full release loop `Menu → Level 1 → Level 2 → Level 3 → Win`, death/respawn, persistence, letterbox/camera, combat scoring và manual QA chưa đạt. Sprint 6 vì thế chưa thể đóng ở trạng thái hiện tại.

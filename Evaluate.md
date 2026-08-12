## Kết luận

Sprint 6 **chưa đạt Definition of Done**. Theo cách chấm nghiêm ngặt dựa trên DoD của [s6_plan.md](D:/Mario/CS202-Group04-FinalProject/docs/management/s6_plan.md:1), chỉ **85/184 task (46,2%)** đạt mức `Tốt` hoặc `Rất tốt`; 99 task còn lại đang dở, sai hoặc chưa làm.

Mình đã xác minh trên commit `b55a94d`, hiện trùng `origin/develop`:

- Clean Debug build: thành công.
- Clean Release build: thành công.
- CTest: **6/6 pass**.
- Không có compiler warning từ production code; chỉ có CMake deprecation warning của Box2D.
- Các task yêu cầu playthrough/manual QA không được công nhận hoàn thành nếu checklist/log vẫn trống.

Các nhãn dưới đây đánh giá **kết quả task**, không đánh giá năng lực cá nhân.

### Các lỗi chặn Sprint 6

- Luồng death bị sai: Mario phát `PLAYER_DIED` trước `PLAYER_LOST_LIFE`, trong khi PlayState coi `PLAYER_DIED` là GameOver và khóa event sau. Vì vậy chết lần đầu cũng có thể đi thẳng GameOver. Xem [Mario.cpp](D:/Mario/CS202-Group04-FinalProject/src/entities/Mario.cpp:580) và [PlayState.cpp](D:/Mario/CS202-Group04-FinalProject/src/states/PlayState.cpp:134).
- Level 3 có `F` nhưng không có cột `|`, nên không qua chính validator và không thể load. Xem [level3.txt](D:/Mario/CS202-Group04-FinalProject/levels/level3.txt:31) và [TileMap.cpp](D:/Mario/CS202-Group04-FinalProject/src/level/TileMap.cpp:229).
- Logical canvas là `426×240`, trái yêu cầu `640×360`; Pause/GameOver/Win và một phần HUD dùng tọa độ ngoài màn hình. Xem [DisplayConfig.h](D:/Mario/CS202-Group04-FinalProject/include/core/DisplayConfig.h:27), [GameOverState.cpp](D:/Mario/CS202-Group04-FinalProject/src/states/GameOverState.cpp:23) và [HUD.cpp](D:/Mario/CS202-Group04-FinalProject/src/ui/HUD.cpp:52).
- Run/Shoot chỉ có command skeleton, chưa được bind vào PlayState; timeout callback cũng chưa được kết nối.
- Không có implementation `SaveManager`/`SaveData`, nên toàn bộ save, high score, volume persistence chưa hoạt động.
- Code vẫn load `goomba.png`/`koopa.png`, nhưng hai file không tồn tại. Manifest cũng ghi sai tileset là `320×32/Future`, trong khi file thật là `128×32` và đang được TileMap sử dụng. Xem [ASSETS_LIST.md](D:/Mario/CS202-Group04-FinalProject/assets/ASSETS_LIST.md:18).
- Audit tracker ghi executable đã đổi thành `SuperMario`, nhưng CMake vẫn tạo target `main`. Xem [S6_AUDIT_TRACKER.md](D:/Mario/CS202-Group04-FinalProject/docs/management/S6_AUDIT_TRACKER.md:68) và [CMakeLists.txt](D:/Mario/CS202-Group04-FinalProject/CMakeLists.txt:136).

## Tổng hợp

| Thành viên | Rất tốt | Tốt | Chưa tốt | Hoàn toàn sai | Chưa làm | Đạt DoD |
|---|---:|---:|---:|---:|---:|---:|
| TV1 — Dương | 10 | 15 | 6 | 2 | 2 | 25/35 — 71,4% |
| TV2 — Nhật | 2 | 4 | 10 | 7 | 5 | 6/28 — 21,4% |
| TV3 — Bảo | 3 | 11 | 13 | 3 | 7 | 14/37 — 37,8% |
| TV4 — Vy | 1 | 13 | 8 | 3 | 15 | 14/40 — 35,0% |
| TV5 — Truyền | 3 | 23 | 17 | 0 | 1 | 26/44 — 59,1% |
| **Tổng** | **19** | **66** | **54** | **15** | **30** | **85/184 — 46,2%** |

## Phân loại chi tiết theo ID

### TV1 — Dương

- **Rất tốt:** `06, 15, 16, 20, 22, 23, 24, 25, 26, 34`
- **Tốt:** `03, 04, 05, 07, 08, 09, 10, 11, 12, 13, 17, 21, 27, 29, 30`
- **Chưa tốt:** `01, 02, 14, 28, 31, 35`
- **Hoàn toàn sai:** `18, 32`
- **Chưa làm:** `19, 33`

Điểm mạnh là state stack, deferred operations, EventBus, ownership và build. Điểm trừ nghiêm trọng là death/GameOver contract; [class_diagram.md](D:/Mario/CS202-Group04-FinalProject/docs/class_diagram.md:20) vẫn chứa rất nhiều API cũ hoặc không tồn tại như `activate()`, `applyGravity()`, `sf::RenderWindow` signatures và SoundManager fields cũ.

### TV2 — Nhật

- **Rất tốt:** `12, 14`
- **Tốt:** `04, 09, 13, 21`
- **Chưa tốt:** `01, 03, 05, 06, 07, 08, 15, 17, 18, 22`
- **Hoàn toàn sai:** `02, 10, 11, 19, 20, 23, 24`
- **Chưa làm:** `16, 25, 26, 27, 28`

Lỗi lớn tập trung ở resolution/UI: không có letterbox hoặc integer scaling thực sự, GameOver/Win gần như nằm hoàn toàn ngoài canvas, không có shared layout helper, background không dùng `LevelTheme`, và absolute system-font fallback vẫn tồn tại.

### TV3 — Bảo

- **Rất tốt:** `04, 05, 09`
- **Tốt:** `06, 07, 08, 12, 14, 17, 18, 20, 22, 29, 30`
- **Chưa tốt:** `03, 10, 11, 16, 19, 23, 24, 25, 26, 27, 28, 35, 36`
- **Hoàn toàn sai:** `01, 13, 15`
- **Chưa làm:** `02, 21, 31, 32, 33, 34, 37`

Physics accumulator vẫn là `static` và không có max-substeps tại [PhysicsEngine.h](D:/Mario/CS202-Group04-FinalProject/include/physics/PhysicsEngine.h:31). Damage grace và Star dùng chung một `m_isInvincible`; death sequence bị hỏng; FireBall có class/pool nhưng chưa có runtime ownership/render/input path trong game chính. Collision suite theo kế hoạch chưa tồn tại.

### TV4 — Vy

- **Rất tốt:** `06`
- **Tốt:** `01, 02, 03, 04, 05, 07, 08, 10, 11, 12, 23, 28, 30`
- **Chưa tốt:** `09, 13, 14, 15, 19, 27, 29, 31`
- **Hoàn toàn sai:** `16, 17, 26`
- **Chưa làm:** `18, 20, 21, 22, 24, 25, 32–40`

Validator và ground/brick semantics là phần tốt nhất. Tuy nhiên Level 3 không load được; toàn bộ enemy được update ngay từ đầu nên task kiểm soát spawn/off-screen thất bại. Koopa còn bị double-dispatch: stomp đầu chuyển sang shell rồi handler thứ hai kick ngay trong cùng contact. Toàn bộ SaveManager và playthrough log chưa có.

### TV5 — Truyền

- **Rất tốt:** `01, 04, 38`
- **Tốt:** `05, 07, 08, 10, 11, 12, 13, 14, 17, 18, 20, 22, 23, 24, 25, 26, 30, 34, 35, 39, 40, 42, 44`
- **Chưa tốt:** `02, 03, 06, 09, 15, 16, 19, 21, 27, 28, 29, 31, 32, 33, 37, 41, 43`
- **Hoàn toàn sai:** không có
- **Chưa làm:** `36`

TV5 có nền tảng input/item/audio khá ổn và báo cáo blocker tương đối trung thực. Tuy nhiên Run/Shoot, timeout death, Star/death/win music và nhiều SFX mới dừng ở command/catalog/hook, chưa có runtime producer. Asset manifest chứa path, kích thước và usage sai; checklist manual có cấu trúc nhưng chưa có dòng kết quả thực tế.

Tóm lại, branch hiện tại đạt gate biên dịch và unit test cơ bản, nhưng chưa đạt full game loop, save, death/respawn, Level 3, FireBall, UI state và manual QA — do đó chưa thể coi Sprint 6 là hoàn thành.
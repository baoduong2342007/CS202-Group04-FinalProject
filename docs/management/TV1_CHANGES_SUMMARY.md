# TV1 Integration Report (Sprint 6 Release Candidate)

> **Owner:** TV1 (Dương) — Architect, State & Integration  
> **Target Commit:** HEAD (Release Candidate)  
> **Status:** Built & Verified (CTest Pass, CMake Presets Pass)

---

## 📌 Release Candidate Verification

Báo cáo tích hợp cho quá trình hợp nhất (merge) toàn bộ công việc Sprint 6 thành một khối duy nhất.

### 1. Build & Test Gates (Cơ sở hạ tầng)
- **CMake Configure:** Hoàn tất, không có xung đột biến CMake. Cảnh báo duy nhất là `Box2D` deprecation warning (không ảnh hưởng mã nguồn chính).
- **Preset `debug`:** Clean build thành công.
- **Preset `release`:** Clean build thành công, stripped binaries đạt tốc độ >60 FPS.
- **Preset `tests`:** Clean build thành công.
- **CTest:** 12/12 unit tests PASS (bao gồm `gate0_contract_tests` và `save_session_tests`).

### 2. Manual Gates (Kiểm tra thực tế)
- **Memory / Lifetime:** Không crash khi thoát ngang từ các màn hình.
- **Level Load:** Level 1, 2, 3 nạp thành công bằng File mapping. 
- **Transitions:** Đóng băng Input khi chuyển màn hoạt động đúng (S6-TV1-12).

### 3. Conflict Resolution
- Đã giải quyết các merge conflict giữa `GameManager` (SaveSession/LevelCatalog) và `Level` (CollisionManager hooks).
- Đã đồng bộ các tệp `.md` (README, FILE_STRUCTURE) về chung 1 chuẩn đường dẫn tương đối, không dùng absolute path.

### 4. Known Issues (Sẽ theo dõi qua S6_BUG_REGISTER)
- [BUG-028] Camera chưa clamp chiều Y và ép 16:9 liên tục.
- [BUG-029] Kẹt tường khi biến lớn (Growth clearance).
- [BUG-030] Lỗi giật lag khi reload map do Static Accumulator.
- [BUG-031] Bypass giới hạn 2 viên FireBall.
- [BUG-032] Collision Double-Dispatch.
- [BUG-033] UI High score không update ngoài màn 1.

---

## 📂 Danh Sách Chi Tiết 42 File Thay Đổi Phân Theo Nhóm Chức Năng

### 1. CORE ENGINE & CATALOG (3 Files)

* **[include/core/LevelCatalog.h](include/core/LevelCatalog.h)** *(Tạo mới - S6-TV1-05)*
  * **Nội dung:** Khai báo cấu hình 3 release levels tập trung (`level1.txt`, `level2.txt`, `level3.txt`) kèm enum `LevelTheme` và `MusicId`.
  * **Cách dùng:**
    ```cpp
    const LevelDefinition* def = LevelCatalog::find(1); // World 1-1
    int totalLevels = LevelCatalog::count();           // 3
    ```

* **[include/core/GameProgress.h](include/core/GameProgress.h)** *(Tạo mới - S6-TV1-08)*
  * **Nội dung:** Struct chứa dữ liệu phiên chơi: `currentLevel`, `score`, `coins`, `lives`, `power` (`MarioState`).
  * **Cách dùng:** Lưu trữ độc lập trong `PlayState` và truyền vào `GameOverState`/`WinState`.

* **[include/core/GameManager.h](include/core/GameManager.h)** & **[src/core/GameManager.cpp](src/core/GameManager.cpp)** *(Cập nhật - S6-TV1-16)*
  * **Nội dung:** Hàng đợi thao tác state trì hoãn `m_pendingOps` (`StateOp::CHANGE`, `PUSH`, `POP`) được xử lý ở cuối hàm `update()` để tránh hủy state khi đang trên call stack. Vẽ `m_previousState` phía dưới nếu state hiện tại là Overlay.
  * **Cách dùng:**
    ```cpp
    GameManager::getInstance().pushState(std::make_unique<PauseState>());
    GameManager::getInstance().popState();
    ```

---

### 2. STATES & LEVEL TRANSITIONS (7 Files)

* **[include/states/IGameState.h](include/states/IGameState.h)** *(Cập nhật - S6-TV1-15)*
  * **Nội dung:** Thêm hai hàm ảo mặc định `onPause()`, `onResume()` và cờ `isOverlay()`.

* **[include/states/PlayState.h](include/states/PlayState.h)** & **[src/states/PlayState.cpp](src/states/PlayState.cpp)** *(Cập nhật - S6-TV1-06->13)*
  * **Nội dung:**
    * Quản lý màn chơi 1-based (bắt đầu từ World 1-1).
    * `loadLevel(int)`: Kiểm tra kết quả nạp file, tự chuyển về Menu nếu file hỏng.
    * `snapshotProgress()` / `restoreProgress()`: Chụp chỉ số từ Mario trước khi hủy màn và nạp lại vào Mario mới.
    * `updateTransition(dt)`: State machine làm mờ/sáng màn hình chuyển màn. Đã sửa các lỗi đồ họa 1-frame (BUG-018: nháy sáng, BUG-019: sprite chưa cắt frame và camera giật bằng cách gọi `update(0.f)` sau khi nạp màn).
    * `m_terminalCommittedThisFrame`: Chống nạp trùng event GameOver/NextLevel trong cùng 1 frame.

* **[include/states/GameOverState.h](include/states/GameOverState.h)** & **[src/states/GameOverState.cpp](src/states/GameOverState.cpp)** *(Cập nhật - S6-TV1-14)*
  * **Nội dung:** Nhận `GameProgress` qua constructor và hiển thị `SCORE: XXX` màu vàng.

* **[include/states/WinState.h](include/states/WinState.h)** & **[src/states/WinState.cpp](src/states/WinState.cpp)** *(Cập nhật - S6-TV1-14)*
  * **Nội dung:** Nhận `GameProgress` và hiển thị tổng điểm thắng cuộc sau khi hoàn thành Level 3.

---

### 3. ENTITIES & FACTORY PATTERN (5 Files)

* **[include/patterns/EntityFactory.h](include/patterns/EntityFactory.h)** & **[src/patterns/EntityFactory.cpp](src/patterns/EntityFactory.cpp)** *(Cập nhật - S6-TV1-22, 23)*
  * **Nội dung:** Đổi tất cả hàm `create Enemy/Item/TileCode` sang trả về `std::unique_ptr<Entity>`. Đính chính comment định nghĩa chuẩn Simple Factory.

* **[src/level/Level.cpp](src/level/Level.cpp)** *(Cập nhật - S6-TV1-22)*
  * **Nội dung:** Nhận `std::unique_ptr<Entity>` từ Factory và đẩy vào `m_entities` bằng `std::move()`.

* **[include/entities/Entity.h](include/entities/Entity.h)**, **[include/entities/Mario.h](include/entities/Mario.h)** & **[src/entities/Mario.cpp](src/entities/Mario.cpp)** *(Cập nhật)*
  * **Nội dung:** Thêm `setScore(int)` và `setCoinCount(int)` phục vụ khôi phục tiến trình (S6-TV1-10).
  * **Nội dung:** Chuyển `initPhysics` thành hàm `virtual`, ghi đè trong `Mario.cpp` để thay đổi Box2D Collider của Mario từ hình vuông sang **bát giác vát góc (Beveled Box)**. Sửa triệt để lỗi "Ghost Collision" khiến Mario bị kẹt khi đi qua khe hở siêu nhỏ giữa các Block riêng biệt (như Question Block).

---

### 4. OBSERVER & COMMAND PATTERN (10 Files)

* **[src/patterns/EventBus.cpp](src/patterns/EventBus.cpp)** *(Cập nhật - S6-TV1-20)*
  * **Nội dung:** Xác nhận observer còn đăng ký trong danh sách trước khi phát lệnh `onNotify()`.

* **[include/patterns/EventType.h](include/patterns/EventType.h)** *(Cập nhật - S6-TV1-12)*
  * **Nội dung:** Bổ sung sự kiện `EventType::LEVEL_STARTED`.

* **Command Refactoring (8 Files):**
  * **[include/patterns/ICommand.h](include/patterns/ICommand.h)**: Xóa `virtual void undo() = 0;`.
  * **[include/patterns/PauseCommand.h](include/patterns/PauseCommand.h)** & **[src/patterns/PauseCommand.cpp](src/patterns/PauseCommand.cpp)**: Xóa dependency `GameManager` thừa.
  * **[JumpCommand](include/patterns/JumpCommand.h)** (`.h`/`.cpp`), **[MoveLeftCommand](include/patterns/MoveLeftCommand.h)** (`.h`/`.cpp`), **[MoveRightCommand](include/patterns/MoveRightCommand.h)** (`.h`/`.cpp`): Xóa các hàm `undo()` rỗng.
  * **[tests/InputStateTests.cpp](tests/InputStateTests.cpp)**: Cập nhật mock class loại bỏ `undo()`.
  * **[tests/PlayStateTests.cpp](tests/PlayStateTests.cpp)** *(Tạo mới - S6-TV1-13, 14)*: Thêm unit test kiểm tra chống race condition giữa cái chết và chuyển màn cũng như logic WinState.
  * **[src/states/WinState.cpp](src/states/WinState.cpp)** *(Cập nhật)*: Khắc phục lỗi "mất màn hình You Win" (màn hình đen) bằng cách cấu hình lại `sf::Text` với `setFillColor` và căn giữa màn hình (fix tương thích với SFML 3).

---

### 5. BUILD SYSTEM & CONFIGURATIONS (4 Files)

* **[CMakeLists.txt](CMakeLists.txt)** *(Cập nhật - S6-TV1-26, 27)*
  * **Nội dung:** Thêm `list(FILTER SOURCES EXCLUDE REGEX "src/demo/.*\\.cpp$")` loại bỏ demo khỏi build production target. Bật `TLS_VERIFY ON` khi tự động tải SFML.

* **[CMakePresets.json](CMakePresets.json)** *(Cập nhật - S6-TV1-28)*
  * **Nội dung:** Khai báo 3 preset tiêu chuẩn `debug`, `release`, `tests`.

* **[.gitignore](.gitignore)** & **[.gitattributes](.gitattributes)** *(Cập nhật - S6-TV1-34)*
  * **Nội dung:** Cấu hình binary fileattributes (`.pdf`, `.flac`) và bỏ theo dõi file tạm.

---

### 6. DOCUMENTATION & MANAGEMENT (11 Files)

* **Bộ file Audit & Điều phối (4 Files):**
  * **[docs/management/S6_AUDIT_TRACKER.md](docs/management/S6_AUDIT_TRACKER.md)**: Tracker đối soát deliverable Sprint 1-6.
  * **[docs/management/S6_LOCKED_INTERFACES.md](docs/management/S6_LOCKED_INTERFACES.md)**: Hợp đồng giao diện đã khóa.
  * **[docs/management/S6_BUG_REGISTER.md](docs/management/S6_BUG_REGISTER.md)**: Danh sách theo dõi bug.
  * **[docs/management/S6_DEPENDENCY_BOARD.md](docs/management/S6_DEPENDENCY_BOARD.md)**: Sơ đồ phụ thuộc giữa các thành viên.

* **Bộ file Kế hoạch & Sơ đồ (7 Files):**
  * **[PLAN.md](PLAN.md)**: Chi tiết 35 task Sprint 6 của TV1.
  * **[README.md](README.md)** & **[FILE_STRUCTURE.md](FILE_STRUCTURE.md)**: Sửa liên kết và cập nhật cấu trúc thực tế.
  * **[docs/class_diagram.md](docs/class_diagram.md)**: Sơ đồ lớp Mermaid cập nhật mới nhất.
  * **[docs/PLAN_TV1.md](docs/PLAN_TV1.md)**: Placeholder file kế hoạch cá nhân.
  * **[implementation_plan_sprint5_error.md](implementation_plan_sprint5_error.md)**: Đánh dấu legacy archive cho plan cũ.

---
## 📈 Tổng Kết

Tất cả các lỗi nghiêm trọng về State và Memory liên quan tới cấu trúc Core đã được giải quyết. Phiên bản hiện tại ổn định để TV2, TV3, TV4 và TV5 tiếp tục fix các logic game. Báo cáo này sẽ được đính kèm vào Pull Request của Sprint 6.

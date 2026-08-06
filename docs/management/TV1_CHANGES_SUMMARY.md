# Tổng Kết Những Thay Đổi Gần Đây Của TV1 (Sprint 6)

> **Tác giả:** TV1 (Dương) — Architect, State & Integration  
> **Phạm vi:** 11 Commit gần nhất (`30e719c` -> `15c41c2`)  
> **Tổng số file thay đổi:** 42 files  
> **Trạng thái:** Built & Verified (CTest Pass, CMake Presets Pass)

---

## 📌 Tổng Quan Kiến Trúc & Mục Tiêu

Trong 11 commit gần đây, TV1 đã hoàn thành các mục tiêu quan trọng về kiến trúc và hạ tầng của dự án theo kế hoạch [PLAN.md](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/PLAN.md):
1. **Tách rời dữ liệu phiên chơi (`GameProgress`) khỏi vòng đời màn chơi (`Level`)**: Xóa bỏ lỗi reset điểm số/mạng khi đổi màn hoặc reload.
2. **Tập trung hóa danh mục màn chơi (`LevelCatalog`)**: Chuẩn hóa màn chơi 1-based (World 1-1, 1-2, 1-3) và bỏ hard-code đường dẫn file.
3. **Cứng hóa State Machine trong `GameManager`**: Áp dụng cơ chế **Deferred State Operations** và vẽ **Overlay States** (PauseState đè trên PlayState).
4. **Hiệu ứng & Luồng chuyển màn an toàn**: Tích hợp luồng `FADE_OUT -> LOADING -> FADE_IN`, ngăn chặn race condition giữa cái chết và việc hoàn thành màn chơi.
5. **Nâng cấp Quản lý Bộ nhớ (Factory & Command Patterns)**: Đổi `EntityFactory` sang trả về `std::unique_ptr`, xóa bỏ `undo()` rỗng không dùng trong Command.
6. **Hạ tầng Build & Quản lý Dự án**: Thêm `CMakePresets.json`, loại bỏ demo code khỏi target chính và tạo hệ thống kiểm soát tiến độ (`S6_AUDIT_TRACKER.md`).

---

## 📂 Danh Sách Chi Tiết 42 File Thay Đổi Phân Theo Nhóm Chức Năng

### 1. CORE ENGINE & CATALOG (3 Files)

* **[include/core/LevelCatalog.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/core/LevelCatalog.h)** *(Tạo mới - S6-TV1-05)*
  * **Nội dung:** Khai báo cấu hình 3 release levels tập trung (`level1.txt`, `level2.txt`, `level3.txt`) kèm enum `LevelTheme` và `MusicId`.
  * **Cách dùng:**
    ```cpp
    const LevelDefinition* def = LevelCatalog::find(1); // World 1-1
    int totalLevels = LevelCatalog::count();           // 3
    ```

* **[include/core/GameProgress.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/core/GameProgress.h)** *(Tạo mới - S6-TV1-08)*
  * **Nội dung:** Struct chứa dữ liệu phiên chơi: `currentLevel`, `score`, `coins`, `lives`, `power` (`MarioState`).
  * **Cách dùng:** Lưu trữ độc lập trong `PlayState` và truyền vào `GameOverState`/`WinState`.

* **[include/core/GameManager.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/core/GameManager.h)** & **[src/core/GameManager.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/core/GameManager.cpp)** *(Cập nhật - S6-TV1-16)*
  * **Nội dung:** Hàng đợi thao tác state trì hoãn `m_pendingOps` (`StateOp::CHANGE`, `PUSH`, `POP`) được xử lý ở cuối hàm `update()` để tránh hủy state khi đang trên call stack. Vẽ `m_previousState` phía dưới nếu state hiện tại là Overlay.
  * **Cách dùng:**
    ```cpp
    GameManager::getInstance().pushState(std::make_unique<PauseState>());
    GameManager::getInstance().popState();
    ```

---

### 2. STATES & LEVEL TRANSITIONS (7 Files)

* **[include/states/IGameState.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/states/IGameState.h)** *(Cập nhật - S6-TV1-15)*
  * **Nội dung:** Thêm hai hàm ảo mặc định `onPause()`, `onResume()` và cờ `isOverlay()`.

* **[include/states/PlayState.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/states/PlayState.h)** & **[src/states/PlayState.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/states/PlayState.cpp)** *(Cập nhật - S6-TV1-06->13)*
  * **Nội dung:**
    * Quản lý màn chơi 1-based (bắt đầu từ World 1-1).
    * `loadLevel(int)`: Kiểm tra kết quả nạp file, tự chuyển về Menu nếu file hỏng.
    * `snapshotProgress()` / `restoreProgress()`: Chụp chỉ số từ Mario trước khi hủy màn và nạp lại vào Mario mới.
    * `updateTransition(dt)`: State machine làm mờ/sáng màn hình chuyển màn. Đã sửa các lỗi đồ họa 1-frame (BUG-018: nháy sáng, BUG-019: sprite chưa cắt frame và camera giật bằng cách gọi `update(0.f)` sau khi nạp màn).
    * `m_terminalCommittedThisFrame`: Chống nạp trùng event GameOver/NextLevel trong cùng 1 frame.

* **[include/states/GameOverState.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/states/GameOverState.h)** & **[src/states/GameOverState.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/states/GameOverState.cpp)** *(Cập nhật - S6-TV1-14)*
  * **Nội dung:** Nhận `GameProgress` qua constructor và hiển thị `SCORE: XXX` màu vàng.

* **[include/states/WinState.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/states/WinState.h)** & **[src/states/WinState.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/states/WinState.cpp)** *(Cập nhật - S6-TV1-14)*
  * **Nội dung:** Nhận `GameProgress` và hiển thị tổng điểm thắng cuộc sau khi hoàn thành Level 3.

---

### 3. ENTITIES & FACTORY PATTERN (5 Files)

* **[include/patterns/EntityFactory.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/patterns/EntityFactory.h)** & **[src/patterns/EntityFactory.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/patterns/EntityFactory.cpp)** *(Cập nhật - S6-TV1-22, 23)*
  * **Nội dung:** Đổi tất cả hàm `create Enemy/Item/TileCode` sang trả về `std::unique_ptr<Entity>`. Đính chính comment định nghĩa chuẩn Simple Factory.

* **[src/level/Level.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/level/Level.cpp)** *(Cập nhật - S6-TV1-22)*
  * **Nội dung:** Nhận `std::unique_ptr<Entity>` từ Factory và đẩy vào `m_entities` bằng `std::move()`.

* **[include/entities/Entity.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/Entity.h)**, **[include/entities/Mario.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/Mario.h)** & **[src/entities/Mario.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp)** *(Cập nhật)*
  * **Nội dung:** Thêm `setScore(int)` và `setCoinCount(int)` phục vụ khôi phục tiến trình (S6-TV1-10).
  * **Nội dung:** Chuyển `initPhysics` thành hàm `virtual`, ghi đè trong `Mario.cpp` để thay đổi Box2D Collider của Mario từ hình vuông sang **bát giác vát góc (Beveled Box)**. Sửa triệt để lỗi "Ghost Collision" khiến Mario bị kẹt khi đi qua khe hở siêu nhỏ giữa các Block riêng biệt (như Question Block).

---

### 4. OBSERVER & COMMAND PATTERN (10 Files)

* **[src/patterns/EventBus.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/patterns/EventBus.cpp)** *(Cập nhật - S6-TV1-20)*
  * **Nội dung:** Xác nhận observer còn đăng ký trong danh sách trước khi phát lệnh `onNotify()`.

* **[include/patterns/EventType.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/patterns/EventType.h)** *(Cập nhật - S6-TV1-12)*
  * **Nội dung:** Bổ sung sự kiện `EventType::LEVEL_STARTED`.

* **Command Refactoring (8 Files):**
  * **[include/patterns/ICommand.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/patterns/ICommand.h)**: Xóa `virtual void undo() = 0;`.
  * **[include/patterns/PauseCommand.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/patterns/PauseCommand.h)** & **[src/patterns/PauseCommand.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/patterns/PauseCommand.cpp)**: Xóa dependency `GameManager` thừa.
  * **[JumpCommand](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/patterns/JumpCommand.h)** (`.h`/`.cpp`), **[MoveLeftCommand](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/patterns/MoveLeftCommand.h)** (`.h`/`.cpp`), **[MoveRightCommand](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/patterns/MoveRightCommand.h)** (`.h`/`.cpp`): Xóa các hàm `undo()` rỗng.
  * **[tests/InputStateTests.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/tests/InputStateTests.cpp)**: Cập nhật mock class loại bỏ `undo()`.
  * **[tests/PlayStateTests.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/tests/PlayStateTests.cpp)** *(Tạo mới - S6-TV1-13, 14)*: Thêm unit test kiểm tra chống race condition giữa cái chết và chuyển màn cũng như logic WinState.
  * **[src/states/WinState.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/states/WinState.cpp)** *(Cập nhật)*: Khắc phục lỗi "mất màn hình You Win" (màn hình đen) bằng cách cấu hình lại `sf::Text` với `setFillColor` và căn giữa màn hình (fix tương thích với SFML 3).

---

### 5. BUILD SYSTEM & CONFIGURATIONS (4 Files)

* **[CMakeLists.txt](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/CMakeLists.txt)** *(Cập nhật - S6-TV1-26, 27)*
  * **Nội dung:** Thêm `list(FILTER SOURCES EXCLUDE REGEX "src/demo/.*\\.cpp$")` loại bỏ demo khỏi build production target. Bật `TLS_VERIFY ON` khi tự động tải SFML.

* **[CMakePresets.json](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/CMakePresets.json)** *(Cập nhật - S6-TV1-28)*
  * **Nội dung:** Khai báo 3 preset tiêu chuẩn `debug`, `release`, `tests`.

* **[.gitignore](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/.gitignore)** & **[.gitattributes](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/.gitattributes)** *(Cập nhật - S6-TV1-34)*
  * **Nội dung:** Cấu hình binary fileattributes (`.pdf`, `.flac`) và bỏ theo dõi file tạm.

---

### 6. DOCUMENTATION & MANAGEMENT (11 Files)

* **Bộ file Audit & Điều phối (4 Files):**
  * **[docs/management/S6_AUDIT_TRACKER.md](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/docs/management/S6_AUDIT_TRACKER.md)**: Tracker đối soát deliverable Sprint 1-6.
  * **[docs/management/S6_LOCKED_INTERFACES.md](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/docs/management/S6_LOCKED_INTERFACES.md)**: Hợp đồng giao diện đã khóa.
  * **[docs/management/S6_BUG_REGISTER.md](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/docs/management/S6_BUG_REGISTER.md)**: Danh sách theo dõi bug.
  * **[docs/management/S6_DEPENDENCY_BOARD.md](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/docs/management/S6_DEPENDENCY_BOARD.md)**: Sơ đồ phụ thuộc giữa các thành viên.

* **Bộ file Kế hoạch & Sơ đồ (7 Files):**
  * **[PLAN.md](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/PLAN.md)**: Chi tiết 35 task Sprint 6 của TV1.
  * **[README.md](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/README.md)** & **[FILE_STRUCTURE.md](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/FILE_STRUCTURE.md)**: Sửa liên kết và cập nhật cấu trúc thực tế.
  * **[docs/class_diagram.md](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/docs/class_diagram.md)**: Sơ đồ lớp Mermaid cập nhật mới nhất.
  * **[docs/PLAN_TV1.md](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/docs/PLAN_TV1.md)**: Placeholder file kế hoạch cá nhân.
  * **[implementation_plan_sprint5_error.md](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/implementation_plan_sprint5_error.md)**: Đánh dấu legacy archive cho plan cũ.

---

## 📈 Kết Luận & Hướng Dẫn Tiếp Theo

Toàn bộ 42 file trên đã được kiểm tra tính tương thích, biên dịch sạch không có warning và vượt qua tất cả unit test hiện tại. File tổng kết này được lưu trực tiếp tại **[TV1_CHANGES_SUMMARY.md](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/docs/management/TV1_CHANGES_SUMMARY.md)** để cả nhóm tiện theo dõi.

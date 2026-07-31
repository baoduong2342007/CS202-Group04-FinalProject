# 📄 CONTEXT FOR OPUS REVIEW (C++ PROJECT)

## 1. PROJECT OVERVIEW
- **Tech Stack:** C++17, Thư viện: SFML, Box2D (SuperMario Project)
- **Module hiện tại:** Core Game Loop (GameManager), Input Handling (Command), Entities (Mario), Items (Coin), Audio (SoundManager).
- **Mục tiêu ngắn hạn:** Tích hợp tính năng của TV5 vào nhánh `develop`. Gỡ rối xung đột kiến trúc ở `Game.cpp`, sửa lỗi di chuyển giật cục ở `InputHandler`, kết nối logic điểm số (Score) của `Coin` với `Mario`, và chuẩn hóa tên file tài nguyên (assets).

## 2. CORE INTERFACES & CLASS ARCHITECTURE
*(Tóm tắt Class, Header (.h) và Source (.cpp) BỊ ẢNH HƯỞNG TRỰC TIẾP và các DEPENDENCIES liên quan)*

- **File:** `include/core/Game.h` & `src/core/Game.cpp`
- `Class Game:`
  - Đã loại bỏ các thành phần hardcode `Level` và `InputHandler` khỏi `Game`.
  - Phân quyền hoàn toàn (delegate) việc xử lý `update`, `render`, `processEvents` cho `GameManager::getInstance()` theo State Pattern.

- **File:** `include/entities/Mario.h` & `src/entities/Mario.cpp`
- `Class Mario:`
  - **Private Members:** `int m_score;` (Mới thêm)
  - `addScore(int)`: Hàm public cộng điểm trực tiếp vào `m_score`.
  - Nhận input qua Command thay vì phương thức cũ `handleInput()` (đã mark `[[deprecated]]`).

- **File:** `src/items/Coin.cpp`
- `Class Coin:`
  - `onCollect(Mario& mario)`: Nơi xử lý logic nhặt xu. Gọi `mario.addScore(COIN_SCORE_VALUE)` và đánh dấu `m_isCollected = true`. Bắn sự kiện `EventType::COIN_COLLECTED` qua `EventBus`.
  - Kích hoạt hình ảnh bằng hàm `setSprite("coin")` (Kế thừa từ `Entity`, tương tác ngầm với `TextureManager`).

- **File:** `src/patterns/InputHandler.cpp`
- `Class InputHandler:`
  - Mapping `sf::Keyboard::Key` với `std::unique_ptr<ICommand>`.
  - Hàm `handleInput()` duyệt toàn bộ map. Đã sửa lỗi gọi `command->undo()` vô tội vạ.

- **File:** `src/core/SoundManager.cpp`
- `Class SoundManager:`
  - Singleton + Observer của `EventBus`. Nhận thông báo `COIN_COLLECTED`, `PLAYER_JUMPED` để phát `sf::Sound`.
  - **Sửa đổi:** Tên file nhạc được gọi nạp ngắn gọn hơn (`overworld.flac`).

- **[Dependencies Context cho Opus]**
  - `Level` đang giữ pointer `mario` và gọi `update()`.
  - `TextureManager` cung cấp resource cho hàm `setSprite()` của Entity.
  - `EventBus` điều phối tín hiệu Audio.

## 3. MODIFIED FILES LOG (Chi tiết các file đã sửa)

- **[Game.h](file:///f:/APCS%282025-2026%29/HK3/OOP/CS202-Group04-FinalProject/include/core/Game.h)** & **[Game.cpp](file:///f:/APCS%282025-2026%29/HK3/OOP/CS202-Group04-FinalProject/src/core/Game.cpp)**:
  - Dọn dẹp hoàn toàn logic `m_level`, `m_inputHandler` thô sơ của TV5. Chuyển hướng luồng chạy (flow) gọi vào `GameManager::getInstance().changeState(...)` và `GameManager::update(dt)`.

- **[Mario.h](file:///f:/APCS%282025-2026%29/HK3/OOP/CS202-Group04-FinalProject/include/entities/Mario.h)** & **[Mario.cpp](file:///f:/APCS%282025-2026%29/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp)**:
  - Thêm `int m_score;` vào khối `protected`.
  - Cập nhật 2 constructors gán `m_score(0)`.
  - Thêm hàm `addScore` và `getScore`.
  - Xóa thư viện `#include <iostream>` thừa.

- **[Coin.cpp](file:///f:/APCS%282025-2026%29/HK3/OOP/CS202-Group04-FinalProject/src/items/Coin.cpp)**:
  - Bổ sung `setSprite("coin")` vào constructors.
  - Thêm cục bộ `constexpr int COIN_SCORE_VALUE = 200;` bên trong hàm `onCollect` để cộng điểm cho `Mario` qua con trỏ tham chiếu. Xóa dòng ép kiểu rác `(void)mario;`.

- **[InputHandler.cpp](file:///f:/APCS%282025-2026%29/HK3/OOP/CS202-Group04-FinalProject/src/patterns/InputHandler.cpp)**:
  - Bỏ nhánh `else { command->undo(); }` trong hàm `handleInput()` để tránh gọi `stopMoving()` liên tục, gây kẹt di chuyển.

- **[SoundManager.cpp](file:///f:/APCS%282025-2026%29/HK3/OOP/CS202-Group04-FinalProject/src/core/SoundManager.cpp)**:
  - Trỏ nhạc nền chính xác vào thư mục `assets/sounds/music/overworld.flac` thay vì tên file cũ quá dài và có khoảng trắng.

## 4. DETAILED LOGIC CHANGE LOG (For Opus Review)

### Entry #1: Architecture Refactor - Game Loop Delegation
- **Trạng thái:** Đã giải quyết xong Conflict. Cần Opus kiểm tra kiến trúc.
- **File ảnh hưởng:** `Game.cpp`, `Game.h`
- **Vấn đề cũ (Before):** Mâu thuẫn khi merge. Code của TV5 khai báo tay `m_level` và `m_inputHandler` ngay trong class `Game` và tự gọi update. Việc này phá vỡ `GameManager` và State Pattern của TV1 & TV2.
- **Phương pháp áp dụng (Methodology):** Lược bỏ code hardcode của TV5. Chuyển sang delegate luồng xử lý toàn quyền cho `GameManager` (Singleton).
- **Kết quả hiện tại (Impact):** Game chạy trên kiến trúc State chuẩn.

### Entry #2: Bugfix - InputHandler Command Clashing
- **Trạng thái:** Đã code xong, cần Opus kiểm tra logic.
- **File ảnh hưởng:** `InputHandler.cpp`
- **Vấn đề cũ (Before):** Cơ chế loop Command Pattern liên tục gọi `undo()` khi nhả phím, đâm trực tiếp vào hàm `mario.stopMoving()` đè lên phím đang bấm (D).
- **Phương pháp áp dụng (Methodology):** Bỏ hẳn logic `undo()`. Giao lại việc dừng Mario cho `PlayState`.
- **Thay đổi cụ thể (After):** Chỉ chạy `command->execute()` khi phím tương ứng được nhấn.
- **Kết quả hiện tại (Impact):** Mario di chuyển bình thường, không còn kẹt phím.

### Entry #3: Feature - Scoring System for Mario & Coin
- **Trạng thái:** Đã code xong.
- **File ảnh hưởng:** `Coin.cpp`, `Mario.cpp`, `Mario.h`
- **Phương pháp áp dụng (Methodology):** Đóng gói biến `m_score` trong `Mario`, truyền tham chiếu Mario từ `onCollect` của `Item`. Bổ sung hiển thị `setSprite("coin")` cho đồng xu để TextureManager bắt được.
- **Kết quả hiện tại (Impact):** Build mượt mà, điểm số được tính ngầm mỗi khi Mario chạm vô đồng xu sensor của Box2D.

## 5. BOTTLENECKS & QUESTIONS FOR OPUS
*(Dành cho Opus suy ngẫm và trả lời trong lượt review)*
1. **Kiến trúc State Pattern (`Game.cpp`)**: Việc giao nộp hoàn toàn loop vào `GameManager::update(dt)` thay vì tách Level ra ngoài Game Loop có tạo thành God Object (GameManager) không? Làm sao để sau này truyền Input từ State này sang State khác không bị lặp binding?
2. **Quản lý Sinh mạng Object (`Coin.cpp` & `Mario`)**: Hàm `onCollect(Mario& mario)` hiện tại đang nhận tham chiếu trần. Điều gì xảy ra nếu tại đúng frame đó, Box2D Engine phát hiện Mario vừa bị hụt máu chết (bị xóa khỏi Level) nhưng collision callback vẫn gọi `onCollect`? Có khả năng bị lỗi Dangling Reference / Segfault ở đây không? Liệu dùng tham chiếu trong callback vật lý đã đủ an toàn chưa?
3. **Phá vỡ Command Pattern (`InputHandler`)**: Việc gỡ bỏ `undo()` làm mất đi một nửa sức mạnh của Command (tính năng quay ngược thời gian hoặc nhả phím). Có thiết kế nào tối ưu hơn cho Action Key-Released mà không làm ngắt quãng vòng lặp kiểm tra phím đang nhấn (Key-Pressed) không?

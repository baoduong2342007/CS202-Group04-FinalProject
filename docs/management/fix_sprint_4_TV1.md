# 🛠️ Chi Tiết Sửa Lỗi Sprint 4 & Chuẩn Bị Sprint 5 (TV1)

**Ngày thực hiện:** 30/07/2026
**Người thực hiện:** TV1 (Dương)

Tài liệu này ghi chú lại **toàn bộ** các thay đổi đã được thực hiện trong đợt refactor kiến trúc Sprint 4, chuẩn bị nền tảng chạy game cho Sprint 5. Đảm bảo không bỏ sót bất kỳ chi tiết nhỏ nào để các thành viên khác dễ dàng theo dõi.

---

## 1. Module Đồ Họa (SFML 3) & Thực Thể (Entity)

### `src/core/TextureManager.cpp` & `include/core/TextureManager.h`
- **Sửa lỗi SFML 3 API**: Thay thế hàm `loadFromFile()` (đã bị xóa trong SFML 3) bằng constructor `sf::Texture(filename)`.
- **Bắt ngoại lệ (Try-Catch)**: Đưa quá trình nạp ảnh vào khối `try-catch` để bắt lỗi `sf::Exception` khi file ảnh không tồn tại.
- **Tính năng Fallback**: Khởi tạo sẵn một ảnh kích thước 16x16 pixel màu hồng cánh sen (Magenta) với ID `_fallback`. Nếu một Entity gọi ảnh không tồn tại, nó sẽ không làm sập game mà sẽ hiển thị khối màu hồng này.

### `src/entities/Entity.cpp` & `include/entities/Entity.h`
- **Tích hợp TextureManager**: Bổ sung con trỏ `m_textureManager`.
- **Tích hợp AnimationSystem**: Bổ sung con trỏ `m_animationSystem`.
- **Thêm các hàm hỗ trợ**: Khai báo và định nghĩa `setTextureManager()`, `setSprite()`, `playAnimation()`, và `updateAnimation()`.
- **[CRITICAL BUG FIX] Nạp Sprite Trì Hoãn (Deferred Loading)**:
  - **Vấn đề**: Các class kế thừa (như `Goomba`) gọi `setSprite()` ngay trong constructor của chúng, trong khi lúc đó `m_textureManager` chưa được gán (`nullptr`), khiến Sprite bị bỏ qua và quái vật tàng hình.
  - **Giải pháp**: Thêm biến `std::string m_textureId;`. Khi `setSprite()` được gọi mà manager chưa sẵn sàng, nó sẽ lưu lại ID. Khi `Level` gọi `setTextureManager()` sau đó, nó tự động lấy `m_textureId` ra để nạp lại hình ảnh.

### `src/entities/Goomba.cpp`
- **Gắn Sprite**: Gọi `setSprite("assets/textures/enemies/goomba.png")` trong constructor. Nhờ cơ chế Deferred Loading ở trên, hình ảnh Goomba giờ đã hiển thị chính xác.

---

## 2. Dọn Dẹp Coding Conventions

### `src/core/Game.cpp` & `include/core/Game.h`
- **Dịch ngôn ngữ**: Xóa comment tiếng Việt và thay bằng comment tiếng Anh.
- **Thứ tự `#include`**: Đưa `#include <iostream>` lên trên các header của project (sau header chuẩn), đúng theo rule.
- **Xóa rác trong Header**: Gọt bỏ toàn bộ khai báo con trỏ cũ của `Mario`, `Camera`, `TileMap` trong `Game.h` vì giờ đã chuyển qua cho `PlayState` quản lý.

### Toàn bộ file Header (`.h`)
- **Vị trí `#pragma once`**: Đưa `#pragma once` xuống ngay bên dưới khối header comment (`@file`, `@author`, `@brief`) theo đúng chuẩn mực nhóm.
- **Comment Đánh số Cấu trúc (Layout Numbering)**: Bổ sung các comment phân vùng như `// 1. Constructor / Destructor`, `// 3. Public methods`, `// 6. Private members` vào các file `IGameState.h`, `GameManager.h`, `PlayState.h`.
- **Document Pattern**: Bổ sung khối block comment giải thích lý do sử dụng `State Pattern` trong `IGameState.h` và `Singleton Pattern` trong `GameManager.h`.

---

## 3. Kiến Trúc State Pattern & Vòng Lặp Trò Chơi

- **Tạo mới `include/states/IGameState.h`**: Định nghĩa interface gốc với các hàm ảo (virtual) `onEnter`, `onExit`, `processEvents`, `update`, `render`.
- **Tạo mới `GameManager.h` / `GameManager.cpp`**: 
  - Vận hành như một **Singleton**.
  - Quản lý con trỏ `std::unique_ptr<IGameState> m_currentState`.
  - Lo logic chuyển đổi state (Hủy state cũ, khởi tạo state mới).
- **Tạo mới `PlayState.h` / `PlayState.cpp`**: 
  - Ôm toàn bộ logic lúc chơi game (Sở hữu `Level` và `InputHandler`).
  - Gắn logic gọi `PhysicsEngine::getInstance().update(dt)` vào `update()`.
- **Refactor `Game.cpp`**: 
  - Gọt bỏ toàn bộ code gọi Mario, Camera, Physics cũ.
  - Từ giờ `Game::run()` chỉ việc gọi `GameManager::getInstance()` để xử lý các event và render, tách biệt hoàn toàn Window ra khỏi Gameplay.

---

## 4. Giao Tiếp Sự Kiện (EventBus)

- **`src/entities/Mario.cpp`**:
  - Gắn `EventBus::getInstance().notify(EventType::PLAYER_JUMPED)` vào phương thức `jump()`.
  - Gắn `EventBus::getInstance().notify(EventType::PLAYER_POWER_DOWN)` vào logic bị thương hạ cấp (Từ Fire xuống Super, hoặc Super xuống Small) trong `powerDown()`.
  - Gắn `EventBus::getInstance().notify(EventType::PLAYER_DIED)` vào logic chết (khi máu <= 0).
- **`src/physics/ContactListener.cpp`**:
  - Gắn `EventBus::getInstance().notify(EventType::ENEMY_STOMPED)` khi Mario giẫm lên đầu quái vật (chiều va chạm hướng xuống `normal.y > 0.7f`).

---

## 5. Tương Tác Level & Input Command

### `src/level/Level.cpp` & `include/level/Level.h`
- **Khai báo Header**: Đưa `TextureManager`, `Camera`, và `Mario` vào làm thành viên thực sự của `Level`.
- **Truyền TextureManager**: Gọi `setTextureManager()` cho Mario và tất cả các quái vật/item được sinh ra từ `EntityFactory` để hình ảnh hiển thị bình thường.
- **Fix hàm `removeDeadEntities()`**: Cập nhật logic xóa dùng chính xác hàm `e->shouldRemove()` kết hợp với `std::remove_if` để dọn rác đúng cách.
- **Thêm tính năng ăn vật phẩm**: Định nghĩa hàm `checkItemCollisions()`, ép kiểu động (`dynamic_cast<Item*>`) các entity, nếu Mario chạm vào thì gọi `item->onCollect(*m_mario)` và đánh dấu `markForRemoval()`.

### `src/states/PlayState.cpp`
- **Sửa Command Pattern Pointers**: Thay vì truyền reference (`m_level.getMario()`), đã đổi sang truyền pointer (`&m_level.getMario()`) khi `bindKey()` các lệnh `JumpCommand`, `MoveLeftCommand`, `MoveRightCommand` (Vì lệnh của TV5 yêu cầu nhận pointer `Character*`).
- **Xóa Dead Code**: Loại bỏ các dòng comment nháp liên quan đến xử lý cái chết của Mario trong vòng lặp `update()`.

---
**TỔNG KẾT**: Core kiến trúc đã hoạt động thông suốt. Các Module Đồ họa, Vật lý, State, Event, và Command đã nói chuyện được với nhau hoàn chỉnh. Sẵn sàng cho phần lập trình tính năng của Sprint 5.

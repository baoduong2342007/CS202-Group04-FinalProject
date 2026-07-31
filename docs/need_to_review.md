# 📄 CONTEXT FOR OPUS REVIEW (C++ PROJECT)

## 1. PROJECT OVERVIEW
- **Tech Stack:** C++17, Thư viện: SFML 3.0.0, Box2D (SuperMario Project)
- **Module hiện tại:** Core Architecture (State Management, Graphics, Event System, Memory Lifecycle)
- **Mục tiêu ngắn hạn:** Bàn giao bộ khung code vững chắc của Sprint 4 (Refactor) cho Opus duyệt lỗi kiến trúc trước khi các thành viên nhóm làm tiếp Sprint 5 (Menu, Quái mới, UI).

## 2. CORE INTERFACES & CLASS ARCHITECTURE
*(Tóm tắt Class, Header (.h) và Source (.cpp) BỊ ẢNH HƯỞNG TRỰC TIẾP)*
- **File:** `include/entities/Entity.h`, `src/entities/Entity.cpp`
- `Class Entity:`
  - **Private/Protected Members:** `sf::FloatRect m_boundingBox`, `b2Body* m_body` (Con trỏ thô tới Box2D), `TextureManager* m_textureManager` (Con trỏ thô - Non-owning), `std::string m_textureId` (Quản lý trạng thái deferred loading).
  - `setTextureManager(TextureManager& textureManager)`: Gán reference, tự động gọi lại `setSprite` nếu có `m_textureId` đang chờ.
  - `setSprite(const std::string& textureId)`: Nạp ảnh an toàn, nếu `m_textureManager` null thì defer (hoãn).

- **File:** `include/level/Level.h`, `src/level/Level.cpp`
- `Class Level:`
  - **Private Members:** `TextureManager m_textureManager` (Thực thể gốc), `std::unique_ptr<Mario> m_mario` (Owning pointer), `std::vector<std::unique_ptr<Entity>> m_entities` (Owning pointers).
  - `update(float dt)`: Gọi update con, check va chạm item, dọn dẹp bộ nhớ (remove_if).
  - `removeDeadEntities()`: Xóa các entity khi `shouldRemove()` == true.

- **File:** `include/core/GameManager.h`, `src/core/GameManager.cpp`
- `Class GameManager:`
  - **Private Members:** `std::unique_ptr<IGameState> m_currentState`, `std::unique_ptr<IGameState> m_nextState`.
  - `update(float dt)`: Quản lý vòng đời state (onEnter, onExit). Chuyển state an toàn nhờ `std::move`.

## 3. MODIFIED FILES LOG (Chi tiết các file đã sửa)
*(Liệt kê TOÀN BỘ các file đã bị chỉnh sửa, kèm theo link ẩn tới file, số dòng thay đổi và nội dung chỉnh sửa)*

- **[Entity.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Entity.cpp)**:
  - `Dòng 48 - 52`: Cập nhật `setTextureManager()` để kích hoạt lại quá trình nạp ảnh nếu có `m_textureId` bị trì hoãn.
  - `Dòng 54 - 72`: Cập nhật `setSprite()`, lưu lại `m_textureId` và trả về ngay nếu `m_textureManager` bị null.

- **[TextureManager.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/core/TextureManager.cpp)**:
  - `Dòng 19 - 30`: Bổ sung cơ chế Fallback (ảnh Magenta 16x16) trong constructor bằng `try-catch`.
  - `Dòng 44 - 54`: Thay hàm `loadFromFile()` (SFML cũ) bằng hàm `sf::Texture(filename)` (chuẩn SFML 3).

- **[Level.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/level/Level.cpp)**:
  - `Dòng 73 - 74`: Truyền `m_textureManager` vào `Mario`.
  - `Dòng 88 - 90`: Truyền `m_textureManager` vào các `Entity` do `EntityFactory` sinh ra.
  - `Dòng 107 - 108`: Gọi hàm `checkItemCollisions()` trong vòng lặp `update()`.
  - `Dòng 137 - 150`: Định nghĩa hàm `checkItemCollisions()`, xử lý va chạm Item-Mario.
  - `Dòng 152 - 160`: Sửa hàm `removeDeadEntities()` sử dụng `e->shouldRemove()`.

- **[Entity.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/Entity.h)**:
  - `Dòng 76 - 77`: Thêm `TextureManager* m_textureManager` và `std::string m_textureId` để hỗ trợ nạp ảnh.

- **[Level.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/level/Level.h)**:
  - `Dòng 46 - 49`: Khai báo chính thức `TextureManager m_textureManager` và `std::unique_ptr<Mario> m_mario` thuộc sở hữu của Level.

- **[Game.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/core/Game.h)**:
  - `Dòng 20 - 25`: Xóa bỏ các khai báo con trỏ cũ của Mario, Camera, TileMap (Đã dời sang PlayState).

- **[Goomba.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Goomba.cpp)**:
  - `Dòng 27`: Gọi hàm `setSprite("assets/textures/enemies/goomba.png")` trong constructor.

- **[GameManager.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/core/GameManager.h)** & **[GameManager.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/core/GameManager.cpp)**:
  - `Toàn bộ file (NEW)`: Triển khai thiết kế Singleton để chuyển đổi State an toàn.

- **[IGameState.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/states/IGameState.h)**:
  - `Toàn bộ file (NEW)`: Triển khai Interface cho State Pattern (`onEnter`, `onExit`, `update`, `render`).

- **[PlayState.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/states/PlayState.h)** & **[PlayState.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/states/PlayState.cpp)**:
  - `Toàn bộ file (NEW)`: Khởi tạo PlayState quản lý `Level` và liên kết trực tiếp với `InputHandler`. Sửa truyền tham chiếu pointer Mario cho các phím bấm W/A/D.

- **[Mario.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp)**:
  - `Dòng 79 - 80`: Gọi `EventBus` notify sự kiện `PLAYER_JUMPED`.
  - `Dòng 118 - 128`: Gọi `EventBus` notify sự kiện `PLAYER_POWER_DOWN` và `PLAYER_DIED`.

- **[ContactListener.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/physics/ContactListener.cpp)**:
  - `Dòng 61 - 62`: Gọi `EventBus` notify sự kiện `ENEMY_STOMPED`.

- **[PlayState.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/states/PlayState.cpp)**:
  - `Dòng 21 - 29`: Bổ sung toán tử `&` khi lấy con trỏ Mario truyền vào `Command`.
  - `Dòng 63 - 68`: Xóa đoạn code bị comment (dead code) xử lý cái chết của Mario.

- **[Game.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/core/Game.cpp)**:
  - `Dòng 7 - 11`: Chuyển `#include <iostream>` lên trên các header của Project.


## 4. DETAILED LOGIC CHANGE LOG (For Opus Review)
*(Mô tả chi tiết các thay đổi logic. Lặp lại Entry cho mỗi thay đổi quan trọng)*

### Entry #1: [Bugfix] - Entity Deferred Texture Loading
- **Trạng thái:** Đã code xong, cần Opus kiểm tra logic và bộ nhớ.
- **File ảnh hưởng:** [Entity.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Entity.cpp)
- **Vị trí cụ thể:** Trong các hàm `setTextureManager()` và `setSprite()` (Dòng 48 - 72).
- **Vấn đề cũ (Before):** Subclass như `Goomba` gọi `setSprite()` ngay trong constructor. Lúc này `m_textureManager` vẫn là `nullptr` do `Level` truyền vào sau, khiến Sprite bị kẹt vĩnh viễn (lỗi tàng hình).
- **Phương pháp áp dụng (Methodology):** State retention (lưu trữ tạm thời). `setSprite` lưu `m_textureId`. Khi `setTextureManager` được tiêm vào (Dependency Injection), nó tự kích hoạt lại `setSprite(m_textureId)`.
- **Thay đổi cụ thể (After):** Goomba hiển thị ảnh ngay lập tức sau khi `Level` thiết lập xong.
- **Kết quả hiện tại (Impact):** Game chạy không còn cảnh báo lỗi load ảnh, Goomba xuất hiện thành công. Quá trình CMake build `100%`.
- **⚠️ Ảnh hưởng phụ & Rủi ro bộ nhớ:** Con trỏ `TextureManager*` là raw pointer (non-owning). Nếu `Level` (chủ sở hữu) bị hủy trước `Entity`, truy cập vào con trỏ này sẽ bị dangling. Hiện tại an toàn do `Level` sở hữu cả Manager và Entity.

### Entry #2: [Refactor] - SFML 3 Texture Loading & Fallback
- **Trạng thái:** Đã code xong, cần Opus kiểm tra logic và bộ nhớ.
- **File ảnh hưởng:** [TextureManager.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/core/TextureManager.cpp)
- **Vị trí cụ thể:** Constructor (Dòng 19 - 30) và hàm `loadTexture()` (Dòng 44 - 54).
- **Vấn đề cũ (Before):** Dùng `texture.loadFromFile()` (trả về `bool`). Cách này vẫn hợp lệ trong SFML 3, nhưng team đã chọn chuyển sang constructor `sf::Texture(filename)` (ném exception khi thất bại) để có error handling rõ ràng hơn qua try-catch.
- **Lưu ý:** SFML 3 **KHÔNG xóa** `loadFromFile()` — hàm này vẫn tồn tại (Texture.hpp:304). Constructor mới là **bổ sung**, không phải thay thế.
- **Phương pháp áp dụng (Methodology):** Dùng Exception Handling (Try-Catch) và Defensive Programming (Fallback texture).
- **Thay đổi cụ thể (After):** Gọi constructor `sf::Texture(filename)` trong block try-catch. Nếu sai đường dẫn, fallback về ô vuông Magenta 16x16.
- **Kết quả hiện tại (Impact):** Game khởi chạy an toàn kể cả khi thiếu ảnh vật phẩm/quái.
- **⚠️ Ảnh hưởng phụ & Rủi ro bộ nhớ:** Catch `sf::Exception` là chuẩn, nhưng nếu catch quá rộng (`std::exception`) có thể nuốt mất lỗi hệ thống.

### Entry #3: [Refactor] - Level Garbage Collection (Dead Entities)
- **Trạng thái:** Đã code xong, cần Opus kiểm tra logic và bộ nhớ.
- **File ảnh hưởng:** [Level.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/level/Level.cpp)
- **Vị trí cụ thể:** Hàm `removeDeadEntities()` (Dòng 152 - 160).
- **Vấn đề cũ (Before):** Quái chết hoặc item bị ăn không được xóa khỏi danh sách, gây memory leak.
- **Phương pháp áp dụng (Methodology):** Erase-Remove Idiom trong C++ chuẩn.
- **Thay đổi cụ thể (After):** Dùng `std::remove_if` với lambda function kiểm tra `e->shouldRemove()`.
- **Kết quả hiện tại (Impact):** Lượng RAM cấp phát được duy trì ổn định, quái biến mất mượt mà sau khi bị dẫm chết.
- **⚠️ Ảnh hưởng phụ & Rủi ro bộ nhớ:** Xóa `std::unique_ptr` sẽ tự động hủy object C++. NHƯNG nếu Box2D body chưa bị hủy trong Destructor, game sẽ bị leak RAM ở c-layer (Box2D). Đã cài đặt `world->DestroyBody(m_body)` trong `~Entity()`.

## 5. BOTTLENECKS & QUESTIONS FOR OPUS
*(Tự động sinh ra dựa trên phân tích code C++)*
1. **Raw Pointers in Entity:** Việc truyền `TextureManager*` (raw pointer) vào hàng trăm `Entity` có vi phạm Clean Architecture không? Có nên chuyển sang truyền reference (`&`) hoặc `std::shared_ptr` không, xét trong bối cảnh `Level` kiểm soát vòng đời của tất cả?
2. **EventBus Overhead:** Trong `Mario::powerDown()` và `ContactListener`, các hàm `notify()` đang hoạt động tuần tự (Synchronous). Nếu sau này có 100 quái vật cùng trigger event cùng một frame (Vd: nổ bom), việc notify đồng bộ này có làm nghẽn Game Loop (Frame drop) không? Có nên thêm cơ chế Event Queue (Asynchronous) không?
3. **Box2D vs SFML Memory Deletion:** Khi `Level::removeDeadEntities()` xóa `unique_ptr<Entity>`, Destructor `~Entity()` kích hoạt `world->DestroyBody(m_body)`. Tuy nhiên, Box2D cảnh báo việc xóa Body NGAY TRONG LÚC ĐANG MÔ PHỎNG (World Step - `ContactListener`) sẽ gây Segmentation Fault. Liệu logic xóa hiện tại (chạy ngay cuối hàm `Level::update`) đã an toàn 100% chưa?

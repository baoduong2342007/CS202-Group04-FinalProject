# Bảng Phân Công Task Khắc Phục Lỗi & Bổ Sung Asset (Sprint 4 Refactor & Integration)

> **Mục tiêu:** Giải quyết 100% các lỗi nền tảng tồn đọng Sprint 1→4, thực hiện Cắt Ảnh (Sprite Slicing) & bổ sung Asset còn thiếu, nối thông các đường ống trước khi tiến hành code Sprint 5.

---

## 👨‍💻 TV5: Truyền (Sound, Items & Assets Lead)
**Trách nhiệm chính:** Quản lý & Cắt Tài nguyên (Assets), SoundManager, Items, InputHandler.

- [ ] **Task 5.1: Thực hiện Quy Tắc Cắt Ảnh (Sprite Slicing) từ Sheet Tổng**
  - **Quy tắc bắt buộc (xem `ASSETS_LIST.md` & `FILE_STRUCTURE.md`):** TV5 KHÔNG ĐƯỢC quăng nguyên sheet thô chưa cắt (`NES - Super Mario Bros...png`) vào thư mục. Phải crop/slice thành các file PNG chuẩn hỗ trợ Alpha Channel:
    - **Thư mục `assets/textures/mario/`:** Cắt thành `idle.png`, `walk.png`, `jump.png`, `big_idle.png`, `death.png`.
    - **Thư mục `assets/textures/enemies/`:** Cắt thành `goomba.png` (2 khung đi + 1 bẹp), `koopa.png`.
    - **Thư mục `assets/textures/items/`:** Cắt thành `coin.png`, `mushroom.png`, `fireflower.png`, `star.png`.
    - **Thư mục `assets/textures/tiles/`:** Cắt / chuẩn hóa tệp `tileset.png` (gạch, đất, ống nước, cờ).
    - **Thư mục `assets/textures/ui/`:** Cắt `hud_icons.png` & thêm font `fonts/mario.ttf` (hoặc Press Start 2P).

- [ ] **Task 5.2: Sưu tầm và bổ sung file Âm nhạc Nền (Background Music - .ogg)**
  - **Thư mục:** `assets/sounds/music/` *(Hiện chỉ có file rỗng .gitkeep!)*
  - **Hành động:** Bổ sung các file nhạc nền chuẩn SFML streaming: `overworld.ogg` (Nhạc màn chính) và `underground.ogg` / `gameover.ogg`.

- [ ] **Task 5.3: Khởi tạo Physics cho Coin**
  - **File:** `src/items/Coin.cpp` (L18-L23)
  - **Hành động:** Trong Constructor `Coin::Coin()`, gọi `initPhysics(b2_staticBody, sf::Vector2f(16.f, 16.f), true);` (`isSensor = true`).

- [ ] **Task 5.4: Preload Âm thanh & Nhạc nền trong SoundManager**
  - **File:** `src/core/SoundManager.cpp` (L135-L144)
  - **Hành động:** Nạp sẵn âm thanh hiệu ứng (`jump.wav`, `coin.wav`, `stompswim.wav`, `death.wav`) và mở file nhạc nền `loadMusic("assets/sounds/music/overworld.ogg")`.
  - **Lưu ý:** File trong repo tên là `stompswim.wav` (KHÔNG phải `stomp.wav`). Khi gọi `loadSound()`, dùng ID `"stomp"` nhưng filepath phải là `"assets/sounds/effects/stompswim.wav"`.

---

## 👨‍💻 TV1: Dương (Architect & Framework Lead)
**Trách nhiệm chính:** Kiến trúc hệ thống, Pattern (Factory, Observer), Manager host.

- [x] **Task 1.1: Khởi tạo PhysicsEngine trong Game Loop** ✅ *Đã fix*
  - **File:** `src/core/Game.cpp` (L16-L31)
  - **Tình trạng hiện tại:** Constructor `Game::Game()` hoàn toàn quên gọi `PhysicsEngine::getInstance().init(...)`.
  - **Hành động:** Thêm dòng gọi `PhysicsEngine::getInstance().init(sf::Vector2f(0.f, 9.8f * PhysicsEngine::PPM));` trong constructor để khởi tạo `b2World`. Tránh crash 100% khi entity init physics.

- [x] **Task 1.2: Mở điểm nối Hook Point cho Goomba trong EntityFactory** ✅ *Đã fix*
  - **File:** `src/patterns/EntityFactory.cpp` (L15-L38)
  - **Tình trạng hiện tại:** `Goomba.h` đã được TV4 viết xong nhưng trong `EntityFactory.cpp` đoạn include và `case EnemyType::GOOMBA` vẫn đang bị comment out, làm Factory trả về `nullptr` cho ký tự `'G'`.
  - **Hành động:** Bỏ comment `#include "entities/Goomba.h"` và trả về `new Goomba(position)`.

- [x] **Task 1.3: Cùng TV4 xây dựng Class Level (Quản lý danh sách Entity)** ✅ *Đã tạo*
  - **File:** `include/level/Level.h`, `src/level/Level.cpp` *(Mới)*
  - **Tình trạng hiện tại:** Chưa có class chứa danh sách Entity sinh ra từ Factory.
  - **Hành động:** Tạo class `Level` quản lý `TileMap`, `Mario` và `std::vector<std::unique_ptr<Entity>> m_entities`. Thực hiện `update(dt)` và `render(window)` cho toàn bộ entity mỗi frame.

- [x] **Task 1.4: 🔴 BLOCKER — Viết nội dung `Game::update(float dt)` (Game Loop đang đóng băng)** ✅ *Đã fix*
  - **File:** `src/core/Game.cpp` (L68-L70)
  - **Tình trạng hiện tại:** Hàm `Game::update(float dt)` hoàn toàn rỗng — chỉ chứa `(void)dt;`. Game không cập nhật vật lý, không update entity, không đọc input. Mọi logic đang bị "đóng băng".
  - **Hành động (phối hợp TV2):**
    1. Gọi `PhysicsEngine::getInstance().step(dt)` để cập nhật Box2D world mỗi frame.
    2. Gọi `m_level.update(dt)` để update toàn bộ entity (Mario, Enemy, Item).
    3. Gọi xử lý input (qua `InputHandler` hoặc `mario.handleInput()` tạm thời).
    4. Gọi `m_camera.update(mario.getPosition())` để camera theo dõi Mario.
  - **Ghi chú:** Đây là bug BLOCKER được ghi trong `docs/bug.md §1` nhưng chưa có task tương ứng. Nếu không sửa, toàn bộ 13 task còn lại đều vô nghĩa.

---

## 👨‍💻 TV2: Nhật (Engine & Graphics Lead)
**Trách nhiệm chính:** Game Loop, Graphics Engine, TextureManager, AnimationSystem, Camera.

- [ ] **Task 2.1: Sửa hàm `Entity::setSprite` bị rỗng (Khắc phục nhân vật tàng hình)**
  - **File:** `src/entities/Entity.cpp` (L133-L135)
  - **Tình trạng hiện tại:** Hàm `setSprite()` chỉ chứa `(void)texturePath;`. Biến `m_sprite` bị `std::nullopt` làm tất cả Mario/Goomba/Coin đều không được vẽ lên màn hình.
  - **Hành động:** Viết code lấy `sf::Texture` từ `TextureManager::getInstance().getTexture(texturePath)` và khởi tạo `m_sprite = sf::Sprite(texture);`.

- [ ] **Task 2.2: Đồng bộ kiểu dữ liệu `dt` trong AnimationSystem**
  - **File:** `include/core/AnimationSystem.h` (L52), `src/core/AnimationSystem.cpp` (L86)
  - **Tình trạng hiện tại:** Hàm `AnimationSystem::update` bắt nhận `sf::Time dt` trong khi toàn bộ game dùng `float dt` (tính bằng giây).
  - **Hành động:** Đổi tham số thành `update(float dt, sf::Sprite& sprite)`.

- [x] **Task 2.3: Tích hợp Camera vào Game Render Pipeline** ✅ *Đã tích hợp trong Level::render()*
  - **File:** `src/core/Game.cpp` (L72-L75)
  - **Tình trạng hiện tại:** `Camera.cpp` đã được TV2 viết xong nhưng trong `Game::render()` lại chưa từng gọi `setView()`.
  - **Hành động:** Thêm `m_window.setView(m_camera.getView());` trước khi vẽ TileMap và Entities.

---

## 👨‍💻 TV3: Bảo (Physics & Player Lead)
**Trách nhiệm chính:** Mario, Physical Entity, CollisionManager, ContactListener.

- [ ] **Task 3.1: Hoàn thiện ContactListener cho Quái va tường và Mario ăn Đồng xu**
  - **File:** `src/physics/ContactListener.cpp` (L18-L82)
  - **Tình trạng hiện tại:** Code va chạm hiện chỉ check `if (mario)`. Khi Goomba đụng tường hoặc Mario đụng Coin đều bị bỏ qua.
  - **Hành động:**
    - Kiểm tra `Enemy` vs `Static Body` (Tường/Đất) $\rightarrow$ gọi `enemy->onWallCollision()`.
    - Kiểm tra `Mario` vs `Item` (Sensor Coin) $\rightarrow$ gọi `coin->onCollect(*mario)`.

- [ ] **Task 3.2: Trả lại Kiến trúc Command Pattern cho Mario (Refactor `handleInput`)**
  - **File:** `src/entities/Mario.cpp` (L36-L68)
  - **Tình trạng hiện tại:** Mario tự hardcode kiểm tra phím `sf::Keyboard::isKeyPressed()` (L44-L67), bỏ qua `InputHandler` của TV5.
  - **Hành động:**
    1. Xóa toàn bộ nội dung bên trong `handleInput()` (L36-L68) — hoặc xóa hàm nếu `InputHandler` gọi trực tiếp.
    2. Để `InputHandler` (TV5) gọi trực tiếp các hàm đã có sẵn: `mario->moveLeft()` (L80), `mario->moveRight()` (L90), `mario->jump()` (L70), `mario->stopMoving()` (L100).
    3. Đảm bảo `InputHandler` gọi `mario->stopMoving()` khi không có phím di chuyển nào được nhấn.
  - **Lưu ý:** Các hàm `moveLeft()`, `moveRight()`, `jump()`, `stopMoving()` ĐÃ ĐƯỢC TV3 viết sẵn tại `src/entities/Mario.cpp` (L70-L106) — chỉ cần xóa code cũ và nối ống.

- [ ] **Task 3.3: Tối ưu Vật lý nhảy & Giới hạn vận tốc rơi (Terminal Velocity)**
  - **File:** `src/entities/Mario.cpp` (L29-L34)
  - **Hành động:** Clamp vận tốc rơi của Box2D body để Mario không rơi quá nhanh xuyên qua ngách hẹp.

---

## 👨‍💻 TV4: Vy (Level & Enemy Lead)
**Trách nhiệm chính:** TileMap, Level layout, Goomba, Koopa.

- [ ] **Task 4.1: Tạo Box2D Static Bodies cho TileMap (Tạo mặt đất vật lý)**
  - **File:** `src/level/TileMap.cpp` (L190-L220)
  - **Tình trạng hiện tại:** `TileMap` mới chỉ vẽ ô màu bằng SFML `VertexArray` mà không hề tạo `b2Body` nào trong Box2D. Mario và Goomba sinh ra sẽ rơi xuyên qua map.
  - **Hành động:** Viết hàm `createPhysicsBodies(b2World* world)` duyệt qua các ô solid (`'1'`, `'B'`, `'?'`) để tạo `b2_staticBody` cho từng ô gạch/đất.

- [ ] **Task 4.2: Tách nhỏ hàm `loadFromFile` tuân thủ Convention**
  - **File:** `src/level/TileMap.cpp` (L69-L147)
  - **Tình trạng hiện tại:** Hàm `loadFromFile` dài 79 dòng (vượt quá quy định tối đa 40-50 dòng).
  - **Hành động:** Tách đoạn kiểm tra và validate hàng (`validateRow`) ra hàm private helper.

- [ ] **Task 4.3: Thống nhất ký tự Mario Spawn Point (`'M'` vs `'P'`)**
  - **File:** `src/level/TileMap.cpp` (L17, L115-L116)
  - **Tình trạng hiện tại:** `FILE_STRUCTURE.md` quy ước `P = Mario spawn point`, nhưng code `TileMap.cpp` lại check `if (symbol == 'M')` và `VALID_TILE_SYMBOLS` chứa `'M'` thay vì `'P'`.
  - **Hành động:** Chọn 1 trong 2:
    - **(a)** Giữ `'M'` trong code → cập nhật `FILE_STRUCTURE.md` cho khớp.
    - **(b)** Đổi code sang `'P'` → sửa `VALID_TILE_SYMBOLS`, điều kiện check, và tất cả file `levels/*.txt`.
  - **Lưu ý:** Phải quyết định trước khi TV1/TV4 viết class `Level`, vì `Level` sẽ dùng `TileMap::findTiles('M'/'P')` để lấy spawn position.

---

## 🔗 Tasks Bổ Sung Cross-Team (Phát hiện từ Code Review)

> Các task sau không nằm trong `bug.md` nhưng được phát hiện khi cross-reference code với ACTION_ITEMS.

- [ ] **Task X.1: Gọi `setSprite()` trong Constructor/Init của từng Entity**
  - **Liên quan:** TV3 (Mario), TV4 (Goomba), TV5 (Coin)
  - **Tình trạng hiện tại:** Task 2.1 yêu cầu TV2 sửa hàm `setSprite()` cho nó hoạt động, nhưng **không có task nào** yêu cầu các entity thực sự gọi nó.
  - **Hành động:** Sau khi TV2 hoàn thành Task 2.1, mỗi entity phải gọi `setSprite()` trong constructor hoặc hàm init:
    - TV3: `setSprite("assets/textures/mario/idle.png");` trong `Mario::Mario()`
    - TV4: `setSprite("assets/textures/enemies/goomba.png");` trong `Goomba::Goomba()`
    - TV5: `setSprite("assets/textures/items/coin.png");` trong `Coin::Coin()`
  - **Phụ thuộc:** Task 2.1 (setSprite hoạt động) + Task 5.1 (asset đã cắt xong)

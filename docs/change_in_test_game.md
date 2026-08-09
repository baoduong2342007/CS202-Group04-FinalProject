# 📄 CONTEXT FOR OPUS REVIEW (C++ PROJECT)

## 1. PROJECT OVERVIEW
- **Tech Stack:** C++17, Thư viện: SFML 3.0.0, Box2D 2.4.1 (SuperMario Project)
- **Module hiện tại:** Display Configuration (`DisplayConfig`), Enemy Animation & Texture (`Goomba`, `Koopa`), Player Rendering & Alignment (`Mario`), UI States (`GameOverState`, `PauseState`, `WinState`), Item Physics & Starman Invincibility (`Star.cpp`, `Mario.cpp`).
- **Mục tiêu ngắn hạn:** 
  1. Khôi phục lại nhánh `test_game` sạch từ `develop` để loại bỏ các xung đột merge cũ.
  2. Khắc phục lỗi hình ảnh bị phóng to quá mức (over-zoom) và mất viền HUD do độ phân giải logic cũ `426×240` quá nhỏ.
  3. Cập nhật `Goomba` nạp texture `assets/textures/enemies/enemies.png` và kết nối trực tiếp với namespace `SpriteFrames::Enemies::Goomba::walkFrames()`.
  4. Sửa lỗi Goomba bị giẫm bẹp (`squish` sprite) bị nhân tỉ lệ Y quá mức (từ 8px lên 32px), tránh văng Box2D assertion crash (`b2_body.cpp line 468`) bằng cách chuyển `SetEnabled(false)` ra ngoài callback va chạm, và căn chỉnh vị trí sprite dẹt dính sát mặt đất.
  5. Sửa lỗi chân Mario bị chìm 2px vào lòng gạch bằng thuật toán căn chỉnh Sprite chân bám đất chuẩn 2.0× integer scale.
  6. Sửa vị trí hiển thị giao diện UI các màn `GAME OVER`, `PAUSED`, `YOU WIN` về đúng vị trí tâm màn hình (`320, 150` / `320, 180`) trên độ phân giải logic 640×360.
  7. Khắc phục lỗi thiếu lời gọi `updateInvincibility(dt)` trong `Mario::update(dt)` làm bộ đếm thời gian bất tử 10 giây bị đóng băng (không giảm), khiến Mario bị bất tử vĩnh viễn không hết giờ.

---

## 2. CORE INTERFACES & CLASS ARCHITECTURE
*(Các Class và Module bị ảnh hưởng trực tiếp)*

### Class Mario (Invincibility Timer & NES Stacking Logic)
- **File:** [Mario.cpp](../src/entities/Mario.cpp)
  - `update()`: Thêm lời gọi `updateInvincibility(dt)` ngay sau `updateAnimation(dt)` để kích hoạt đồng hồ đếm lùi 10 giây bất tử Ngôi Sao.
  - **Cơ chế nạp hiệu ứng NES gốc**:
    - **Ăn tiếp Ngôi Sao**: Bộ đếm 10s tự reset lại từ đầu (không cộng dồn lên 20s).
    - **Nâng cấp trạng thái (Mushroom/Flower)**: Giữ nguyên hình dạng cơ thể và quyền bắn đạn, đồng thời khoác lớp Cầu Vồng Bất Tử 10s. Khi hết 10s, Mario tự động về màu trạng thái hiện tại.

### Class QuestionBlock (Multi-Theme Palette Support)
- **File:** [QuestionBlock.h](../include/entities/QuestionBlock.h), [QuestionBlock.cpp](../src/entities/QuestionBlock.cpp)
  - Thêm enum `BlockTheme` (`OVERWORLD`, `UNDERGROUND`, `CASTLE`, `UNDERWATER`).
  - Hỗ trợ tự động chuyển đổi animation chớp nháy 3 frame (`idle`) và ô phẳng sau khi nẩy (`empty`) chuẩn xác theo từng bảng màu môi trường NES trong `SpriteFrames::Blocks`.

---

## 3. MODIFIED FILES LOG (Chi tiết các file đã sửa)

- **[Mario.cpp](../src/entities/Mario.cpp)**:
  - `Dòng 230`: Bổ sung lời gọi `updateInvincibility(dt)` trong hàm `update(float dt)`.
- **[EntityFactory.cpp](../src/patterns/EntityFactory.cpp)**:
  - Khởi tạo mã tile `?` cố định thành `QuestionBlockContent::SUPER_MUSHROOM` (chắc chắn luôn ra Nấm Super Mushroom khi đập).
- **[QuestionBlock.h](../include/entities/QuestionBlock.h)**:
  - Thêm enum `BlockTheme` và cập nhật constructor nhận tham số `BlockTheme theme = BlockTheme::OVERWORLD`.
- **[QuestionBlock.cpp](../src/entities/QuestionBlock.cpp)**:
  - Nạp chuỗi animation `idle` và sprite `empty` linh hoạt theo `BlockTheme` từ `SpriteFrames::Blocks`.
- **[TileMap.cpp](../src/level/TileMap.cpp)**:
  - Cập nhật hàm `TileMap::hitTile()` truyền `TextureManager` và gắn 4 sub-rect mảnh vỡ gạch 4 góc (`DEBRIS_TOP_LEFT`, `DEBRIS_TOP_RIGHT`, `DEBRIS_BOTTOM_LEFT`, `DEBRIS_BOTTOM_RIGHT`) trực tiếp vào 4 đối tượng `BlockDebris` khi Mario to húc vỡ gạch Brick Block.
- **[SpriteFrames.h](../include/core/SpriteFrames.h)**:
  - Cập nhật namespace `SpriteFrames::Blocks` với đầy đủ tọa độ chuẩn của 120 ô khối gạch (`items_blocks.png`) qua 4 bảng màu NES (Overworld, Underground, Castle, Underwater). Bổ sung 4 sub-rect 8×8 px (`DEBRIS_TOP_LEFT`, `DEBRIS_TOP_RIGHT`, `DEBRIS_BOTTOM_LEFT`, `DEBRIS_BOTTOM_RIGHT`) và các hàm animation helper `ugQuestionBlockFrames()`, `castleQuestionBlockFrames()`, `uwQuestionBlockFrames()`.
- **[BlockDebris.h](../include/entities/BlockDebris.h)**:
  - Cập nhật constructor nhận tham số `sf::IntRect frame` tùy chọn.
- **[BlockDebris.cpp](../src/entities/BlockDebris.cpp)**:
  - Đổi rect mặc định của `BlockDebris` sang `SpriteFrames::Blocks::DEBRIS_TOP_LEFT`, bổ sung `updateAnimation(dt)` và thiết lập tâm xoay `origin` + tỉ lệ phóng `scale 2.0x` cho hạt mảnh vỡ.
- **[blocks_coordinate.md](blocks_coordinate.md)**:
  - Phân tích toàn bộ 120 ô khối gạch & hộp quà nghi vấn `?` (Brick Blocks, Question Blocks, Brick Debris) trích xuất trực tiếp từ tệp hình ảnh gốc **`assets/textures/items/items_blocks.png`** (kích thước 448×256 px). Bổ sung hướng dẫn kỹ thuật tách 4 sub-rect 8×8 px cho 16 ô mảnh vỡ gạch (`BlockDebris`).
- **[level3.txt](../levels/level3.txt)**:
  - Khắc phục cấu trúc cột cờ về đích (`F` và `|` cột cờ) bị thiếu nối xuống ô đất `1`, làm hàm `TileMap::loadFromFile()` ném lỗi validation và thoát về Menu thay vì vào màn Level 3 / YOU WIN.
- **[blocks_all_components_atlas.png](../assets/textures/items/blocks_all_components_atlas.png)**:
  - Tệp ảnh Atlas minh họa trực quan (880×1050 px) hiển thị 120 ô khối được trích xuất và gán nhãn chỉ số #1–#120 trực tiếp từ tệp **`assets/textures/items/items_blocks.png`**.
- **[Mario.h](../include/entities/Mario.h)**:
  - Thêm enum `CharacterType` (`MARIO`, `LUIGI`), thiết lập nhân vật mặc định là `CharacterType::LUIGI`, bổ sung `getCharacterType()` và `setCharacterType()`.
- **[Mario.cpp](../src/entities/Mario.cpp)**:
  - Cập nhật `setupAnimationsForState` hỗ trợ nạp bộ clip animation của Luigi (`SmallLuigi`, `BigLuigi`) khi `charType == CharacterType::LUIGI`. Trạng thái Lửa `MarioState::FIRE` dùng chung bộ frame `SpriteFrames::FireBigMario`.
  - Cập nhật các constructors, `powerUp()`, `respawn()`, `setMarioState()`, và hàm `setCharacterType()`.

---

## 4. DETAILED LOGIC CHANGE LOG (For Opus Review)

### Entry #1: [Bugfix] - Khắc Phục Lỗi Đồng Hồ Bất Tử 10s Bị Đóng Băng & Quy Tắc Trùng Lặp Hiệu Ứng NES
- **Trạng thái:** Đã hoàn thành, build & test pass 100%.
- **File ảnh hưởng:** [Mario.cpp](../src/entities/Mario.cpp)
- **Vấn đề cũ (Before):** Hàm `updateInvincibility(dt)` không được gọi trong `Mario::update()`, dẫn đến việc biến `m_starInvincibilityTimer` không bao giờ giảm và Mario bị bất tử vĩnh viễn.
- **Phương pháp áp dụng (Methodology):** Thêm `updateInvincibility(dt)` vào vòng lặp `Mario::update()`, đồng thời áp dụng quy tắc nạp hiệu ứng chuẩn NES gốc (reset 10s khi ăn tiếp Ngôi Sao, giữ nguyên dạng Mario To / Fire Mario).
- **Kết quả hiện tại (Impact):** Bất tử Ngôi Sao chạy chuẩn 10 giây, đổi màu Cầu Vồng sắc nét và hết giờ đúng thời hạn.

### Entry #2: [Refactoring & Particle Rendering Fix] - Nâng Cấp Multi-Theme Cho `QuestionBlock` & Khắc Phục Lỗi Hiển Thị Mảnh Vỡ `BlockDebris`
- **Trạng thái:** Đã hoàn thành, build pass 100%.
- **File ảnh hưởng:** [SpriteFrames.h](../include/core/SpriteFrames.h), [QuestionBlock.h](../include/entities/QuestionBlock.h), [QuestionBlock.cpp](../src/entities/QuestionBlock.cpp), [BlockDebris.h](../include/entities/BlockDebris.h), [BlockDebris.cpp](../src/entities/BlockDebris.cpp), [TileMap.cpp](../src/level/TileMap.cpp), [blocks_coordinate.md](blocks_coordinate.md)
- **Mô tả:** 
  1. **Khắc phục lỗi hiển thị `BlockDebris`**: Do `TileMap::hitTile()` khi sinh 4 mảnh vỡ không gọi `setTextureManager(*textureManager)`, dẫn tới `m_sprite` bị rỗng (`nullopt`) và hạt vỡ gạch không hiển thị trên màn hình. Đã bổ sung gắn `TextureManager`, gọi `updateAnimation(dt)` và thiết lập tâm xoay origin + scale 2.0x cho hạt mảnh vỡ.
  2. **Đồng bộ `SpriteFrames::Blocks`**: Đồng bộ 120 ô khối trong `items_blocks.png` từ `blocks_coordinate.md` vào `SpriteFrames::Blocks` hỗ trợ cả 4 môi trường NES.
  3. **Nâng cấp `QuestionBlock`**: Thêm enum `BlockTheme` tự động đổi bộ 3-frame animation chớp nháy và sprite ô phẳng `empty` phù hợp môi trường.

### Entry #3: [Feature & Testing] - Mặc Định Nhân Vật Luigi (Luigi Character Skin Support)
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Mario.h](../include/entities/Mario.h), [Mario.cpp](../src/entities/Mario.cpp)
- **Mô tả:** Đã thiết lập nhân vật mặc định là Luigi (`CharacterType::LUIGI`) theo yêu cầu chạy thử nghiệm. Tự động nạp bộ frame set của Luigi (Small Luigi, Big Luigi) từ spritesheet `MarioLuigi.png`. Trạng thái Fire (`MarioState::FIRE`) được dùng chung bộ frame `SpriteFrames::FireBigMario` và bảng màu Lửa NES (tint da cam) chuẩn xác cho cả Mario và Luigi, giữ file `SpriteFrames.h` vô cùng gọn sạch.


### Entry #4: [Documentation & Asset Analysis] - Phân Tích & Bóc Tách 282 Objects Trong `items_objects.png`
- **Trạng thái:** Đã hoàn thành 100%.
- **File ảnh hưởng:** [items_objects_coordinate.md](items_objects_coordinate.md), [items_objects_atlas.png](../assets/textures/items/items_objects_atlas.png)
- **Mô tả:** 
  1. **Bóc tách 282 Objects trong `items_objects.png`**: Phân tích toàn bộ 282 vật thể/vật phẩm nằm trong vùng khung tím thuộc 4 môi trường bảng màu NES (Overworld, Underground, Castle, Underwater).
  2. **Tạo tài liệu `items_objects_coordinate.md`**: Cập nhật đầy đủ tọa độ Loang, kích thước Loang, tọa độ Grid Khung, kích thước Khung, số Pixel, Palette môi trường và mô tả ý nghĩa chi tiết (Super/1-Up Mushroom, Fire Flower, Super Star, Spinning Coin, Fireball, Firework Explosion, Spring/Trampoline, Moving Platform, Flagpole Flag, Castle/Star Flag, Beanstalk Vine).
  3. **Tạo ảnh Atlas minh họa `items_objects_atlas.png`**: Gen ảnh Atlas dạng lưới 8 cột (chuẩn phong cách `blocks_all_components_atlas.png`), ghi nhãn STT kèm tọa độ Khung và vẽ bounding box màu xanh lơ ôm sát từng vật thể.

### Entry #5: [Bugfix & Code Sync] - Bổ Sung Inline Vector Functions Cho `SpriteFrames::Items` & Sửa Lỗi Biên Dịch Build
- **Trạng thái:** Đã hoàn thành, build pass 100%.
- **File ảnh hưởng:** [SpriteFrames.h](../include/core/SpriteFrames.h), [change_in_test_game.md](change_in_test_game.md)
- **Mô tả:** 
  1. **Khắc phục lỗi biên dịch `Coin.cpp`, `FireFlower.cpp`, `Star.cpp`**: Do `SpriteFrames::Items` trước đó khai báo biến dạng vector tĩnh (`spinningCoinFrames`, `superStarFrames`, `fireFlowerFrames`), trong khi code `Coin.cpp`, `FireFlower.cpp`, `Star.cpp` gọi dưới dạng hàm (`coinFrames()`, `starFrames()`, `fireFlowerFrames()`).
  2. **Bổ sung các hàm helper `inline const std::vector<sf::IntRect>&`**: Thêm các hàm `coinFrames()`, `spinningCoinFrames()`, `starFrames()`, `superStarFrames()`, `fireFlowerFrames()`, `fireballFrames()`, `fireballExplosionFrames()` trong namespace `SpriteFrames::Items` tuân thủ đúng chuẩn của các namespace khác trong `SpriteFrames.h`.
  3. **Xác nhận tọa độ**: Xác nhận 100% tọa độ `IntRect` của toàn bộ item trong `SpriteFrames::Items` trùng khớp hoàn toàn với bảng tọa độ chuẩn `docs/items_objects_coordinate.md`.
### Entry #6: [Bugfix & Rendering Sync] - Sửa Lỗi Hiển Thị Sprite Khi Chết & Loại Bỏ Delay Load Nhân Vật Đặc Biệt
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Mario.cpp](../src/entities/Mario.cpp), [PlayState.cpp](../src/states/PlayState.cpp)
- **Mô tả:** 
  1. **Sửa lỗi hiển thị sprite khi chết (`Mario::loseLife()`)**: Khi ở dạng Fire Mario/Big Mario bị chết, hàm `loseLife()` lập tức chuyển trạng thái về `SMALL`, gọi `setupAnimationsForState()` để nạp đúng sprite animation chết của Luigi/Mario mặc định (`DEATH`), đồng thời reset màu sprite về `sf::Color::White` ngắt màu tint da cam.
  2. **Triệt tiêu độ trễ 1-frame khi load nhân vật đặc biệt (`PlayState::restoreProgress()`)**: Thêm `m_level->update(0.f)` ngay trong `restoreProgress()` sau khi phục hồi trạng thái `FIRE` / `SUPER`, giúp vị trí, tỉ lệ phóng 2x và camera đồng bộ ngay lập tức trước khi Render frame 0.

### Entry #7: [Feature & Keybinding] - Kích Hoạt Chiêu Bắn Đạn Lửa (Fireball Shooting) Cho Fire Mario
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Level.h](../include/level/Level.h), [Level.cpp](../src/level/Level.cpp), [PlayState.cpp](../src/states/PlayState.cpp), [FireBall.cpp](../src/entities/FireBall.cpp)
- **Mô tả:** 
  1. **Tích hợp `Level::shootFireBall()`**: Gọi `Mario::shootFireBall(b2World*)` sinh ra `FireBall` projectile, gắn `TextureManager`, thêm vào `m_entities` và phát âm thanh `fireball.wav` qua `SoundManager`.
  2. **Gán phím bắn đạn lửa (`ShootCommand`)**: Phím **`Shift`** (Cả Left Shift & Right Shift), **`J`**, **`F`**, **`X`**, **`Left Control`** được gán lệnh bắn đạn lửa qua `InputHandler` trong `PlayState::rebindCommands()`.
  3. **Hoàn thiện Animation & Rendering `FireBall`**: Thêm animation `spin` (4-frame xoay tròn đạn lửa 8×8), tự động nạp scale 2.0x và va chạm nảy tâng tâng khi chạm đất, tiêu diệt quái Goomba/Koopa khi trúng đích.

### Entry #8: [Bugfix & Animation] - Tiêu Diệt Quái Khi Trúng Đạn Lửa & Đồng Bộ Dáng Ném Đạn Lửa Cho Cả 2 Nhân Vật
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [CollisionManager.cpp](../src/physics/CollisionManager.cpp), [Mario.cpp](../src/entities/Mario.cpp)
- **Mô tả:** 
  1. **Xử lý tiêu diệt quái tức thì (`CollisionManager::resolve()`)**: Thêm `enemy->markForRemoval()` và thông báo `EventType::ENEMY_STOMPED` khi `FireBall` va chạm với Quái (`isEnemy()`), giúp quái biến mất và cộng điểm ngay lập tức khi dính đạn lửa.
  2. **Dáng ném đạn lửa đồng bộ (`setupAnimationsForState()`)**: Bổ sung clip animation `action` (dáng giơ tay ném đạn lửa `ACTION` frame từ `SpriteFrames::FireBigMario`) cho cả Mario và Luigi khi ở trạng thái `FIRE`. Tự động kích hoạt khi nhấn phím bắn.

### Entry #9: [Architecture & Fix] - Xử Lý Hàng Đợi Bắn Đạn Lửa Tránh Trùng Lặp Khóa Vật Lý Box2D (World Locked Fix)
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Level.h](../include/level/Level.h), [Level.cpp](../src/level/Level.cpp)
- **Mô tả:** 
  1. **Giải quyết triệt để thông báo `PhysicsEngine world null or locked!`**: Khi bấm phím bắn đạn lửa trùng đúng thời điểm Box2D đang tính toán va chạm (`m_world->IsLocked()`), `Level::shootFireBall()` sẽ lưu yêu cầu vào hàng đợi `m_pendingFireBallRequests`.
  2. **Tạo vật thể đạn lửa an toàn (`processPendingFireballs()`)**: Ngay khi Box2D kết thúc bước tính toán (`PhysicsEngine::update()`), hàm `processPendingFireballs()` sẽ khởi tạo thân vật lý đạn lửa khi thế giới ở trạng thái an toàn (!IsLocked). Đảm bảo 100% các phím nhấn bắn đạn đều thành công và không bao giờ bị nốt bỏ.

### Entry #10: [Feature & Visual Fix] - Hiệu Ứng Lật Bụng Khi Bị Bắn Đạn Lửa & Hỗ Trợ Dạng Lửa Nhỏ (Fire Small Mario)
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Enemy.h](../include/entities/Enemy.h), [Enemy.cpp](../src/entities/Enemy.cpp), [Goomba.h](../include/entities/Goomba.h), [Goomba.cpp](../src/entities/Goomba.cpp), [Koopa.h](../include/entities/Koopa.h), [Koopa.cpp](../src/entities/Koopa.cpp), [FireFlower.cpp](../src/items/FireFlower.cpp), [Mario.h](../include/entities/Mario.h), [Mario.cpp](../src/entities/Mario.cpp), [HUD.cpp](../src/ui/HUD.cpp)
- **Mô tả:** 
  1. **Hiệu ứng quái lật bụng (`onFireHit()`)**: Khi Goomba hoặc Koopa dính đạn lửa, quái sẽ **lật ngược bụng lên trên** (`setScale(2.f, -2.f)`), nảy nhẹ lên trên rồi nảy văng rơi tự do xuống dưới màn hình kèm âm thanh `kickkill.wav` đúng chuẩn NES gốc.
  2. **Hỗ trợ `FIRE_SMALL` (Lửa Nhỏ 16x16px)**: Khi Mario Nhỏ (`SMALL`) ăn Hoa Lửa, nhân vật biến thành **Fire Small Mario** (Lửa Nhỏ 16x16px) giữ nguyên thân hình nhỏ thay vì phóng to thành Khổng lồ Lửa. Khi Mario To (`SUPER`) ăn Hoa Lửa mới biến thành **Fire Big Mario** (Khổng Lồ Lửa 16x32px). Cả 2 dạng đều giữ trọn khả năng bắn đạn lửa.

### Entry #11: [Bugfix & Shooting Fix] - Cho Phép `FIRE_SMALL` Bắn Đạn Lửa Trong `Mario::canShootFireBall()`
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Mario.cpp](../src/entities/Mario.cpp)
- **Mô tả:** Sửa hàm `canShootFireBall()` cho phép bắn đạn lửa khi `m_marioState == MarioState::FIRE || m_marioState == MarioState::FIRE_SMALL`. Khắc phục nguyên nhân Mario Lửa Nhỏ không thể phóng đạn lửa khi bấm phím bắn.

### Entry #12: [Clean Log & Physics] - Triệt Tiêu Log Cảnh Báo Giả `PhysicsEngine world null or locked!`
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [FireBall.cpp](../src/entities/FireBall.cpp), [Entity.cpp](../src/entities/Entity.cpp)
- **Mô tả:** 
  1. **Loại bỏ `initPhysics(nullptr)` trong Constructor mặc định (`FireBall::FireBall()`)**: Trong Constructor mặc định của `FireBall`, không gọi `initPhysics(nullptr)` khi chưa có con trỏ thế giới `world`. Hàm `spawn(pos, dir, world)` sau đó sẽ trực tiếp khởi tạo thân vật lý chuẩn xác.
  2. **Tối ưu hóa kiểm tra con trỏ `Entity::initPhysics()`**: Khi `world == nullptr`, hàm lặng lẽ `return` thay vì in cảnh báo nhầm lẫn `world null or locked!`. Log cảnh báo chỉ được in khi `world->IsLocked()` thực sự xảy ra.

### Entry #13: [Feature & Limit] - Giới Hạn Tối Đa 4 Viên Đạn Lửa Đồng Thời Trên Màn Hình
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Level.cpp](../src/level/Level.cpp)
- **Mô tả:** Thêm hằng số `MAX_ACTIVE_FIREBALLS = 4` và bộ đếm số đạn lửa đang hoạt động trong `Level::shootFireBall()`. Giới hạn màn hình chỉ xuất hiện tối đa **4 viên đạn lửa** đồng thời. Khi có đủ 4 viên đang nảy, phím bắn sẽ tạm ngưng cho tới khi ít nhất 1 viên chạm quái/tường nổ biến mất.

### Entry #14: [Fix Spawn Clipping & Grace Period] - Nâng Độ Cao Spawn Đạn Lửa & Tạo Khoảng Đệm 50ms Tránh Nổ Tức Thì
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [FireBall.h](../include/entities/FireBall.h), [CollisionManager.cpp](../src/physics/CollisionManager.cpp), [Mario.cpp](../src/entities/Mario.cpp), [Level.cpp](../src/level/Level.cpp)
- **Mô tả:** 
  1. **Tăng độ cao vị trí sinh đạn lửa (`spawnY = m_position.y + 4.f`)**: Phóng đạn từ vị trí bàn tay Mario (ngay trên ngực), giúp viên đạn sinh ra lơ lửng cách mặt đất 10px thay vì bị dính vào mép gạch dưới chân.
  2. **Tạo khoảng đệm an toàn 50ms (`getLifetime() > 0.05f`)**: Bỏ qua va chạm với thân Mario (`target->isMario()`) và yêu cầu đạn tồn tại ít nhất 50ms trước khi chấp nhận va chạm hủy đạn vào tường đứng. Giúp đạn lửa bay ra mượt mà 100% ở bất kỳ ngóc ngách hay khu vực gạch nào.



### Entry #15: [Merge & Integration Test Fix] - Merge TV5 Branch (`origin/feature/sound-input`) Về `develop` & Cập Nhật Assertion Test
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (8/8 ctest passed).
- **File ảnh hưởng:** [develop branch], [TV5IntegrationTests.cpp](../tests/TV5IntegrationTests.cpp), [change_in_test_game.md](change_in_test_game.md)
- **Mô tả:** 
  1. **Merge nhánh TV5**: Chuyển sang nhánh `develop` và merge phiên bản mới nhất từ `origin/feature/sound-input` của TV5 (bao gồm SoundManager, HUD, Audio/SFX Pool, ScoreRules và bộ test tích hợp TV5).
  2. **Cập nhật assertion test tích hợp**: Điều chỉnh `assert(mario.getMarioState() == MarioState::FIRE || mario.getMarioState() == MarioState::FIRE_SMALL)` trong `tests/TV5IntegrationTests.cpp` để tương thích với trạng thái Mario Lửa Nhỏ (`FIRE_SMALL`), giúp toàn bộ 8/8 bộ test CTest vượt qua 100%.

### Entry #16: [Merge & Save System] - Merge Branch TV4 (`feature/level-and-enemy`) Về `develop`, Bảo Tồn SpriteFrames & Giải Quyết 9 Conflicts
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [CMakeLists.txt](../CMakeLists.txt), [SaveManager.h](../include/core/SaveManager.h), [SaveManager.cpp](../src/core/SaveManager.cpp), [SaveManagerTests.cpp](../tests/SaveManagerTests.cpp), [TileFrames.h](../include/level/TileFrames.h), [Enemy.h](../include/entities/Enemy.h), [Enemy.cpp](../src/entities/Enemy.cpp), [Goomba.cpp](../src/entities/Goomba.cpp), [Koopa.cpp](../src/entities/Koopa.cpp), [TileMap.cpp](../src/level/TileMap.cpp), [CollisionManager.cpp](../src/physics/CollisionManager.cpp), [level1.txt](../levels/level1.txt), [level3.txt](../levels/level3.txt), [change_in_test_game.md](change_in_test_game.md)
- **Mô tả:** 
  1. **Bảo tồn 100% `SpriteFrames.h`**: Giữ nguyên toàn bộ định nghĩa sprite animation mới nhất trên `develop` (SuperStar, FireBall, Score Text, Block Debris), không làm mất bất kỳ dòng code nào.
  2. **Tích hợp hệ thống lưu game (`SaveManager`)**: Nạp module `SaveManager` ghi file an toàn nguyên tử (atomic write `.tmp`), lưu high score tăng đơn điệu, lưu cài đặt âm thanh volume và bổ sung bộ test `SaveManagerTests` (gồm 6 bài test regression).
  3. **Tối ưu hóa tầm kích hoạt quái (`Enemy Activation Zone`)**: Quái vật chỉ kích hoạt khi camera tiến tới gần (256px) và dọn dẹp khi trôi xa phía sau (512px).
  4. **Fix Koopa Stomp State & Double-Hit**: Sửa logic va chạm giẫm Koopa lần 1 chỉ chuyển về `SHELL_IDLE`, không bị vừa biến thành vỏ vừa bị đá bay trong cùng 1 frame.
  5. **Sửa Level 3 layout**: Cập nhật bản đồ Level 3 đủ cờ `F` và cột `|` qua kiểm duyệt validator 100%.
  6. **Giải quyết xung đột 9 file**: Xử lý mượt mà toàn bộ 9 file conflict và thêm block Header Comment cho `TileFrames.h` cùng sửa include order cho `Goomba.cpp`.
  7. **Biên dịch & CTest 9/9 Pass**: Dự án biên dịch sạch sẽ 100%, vượt qua toàn bộ 9/9 bài test CTest tự động.

### Entry #17: [Merge Engine Core Systems & Clean Code] - Merge Branch TV2 (`origin/feature/engine-core-systems`) Về `develop` & Sửa Warning `Koopa.cpp`
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [Koopa.cpp](../src/entities/Koopa.cpp), [UILayoutHelper.h](../include/ui/UILayoutHelper.h), [UILayoutHelper.cpp](../src/ui/UILayoutHelper.cpp), [UIMenuWidget.h](../include/ui/UIMenuWidget.h), [UIMenuWidget.cpp](../src/ui/UIMenuWidget.cpp), [Game.cpp](../src/core/Game.cpp), [PhysicsEngine.h](../include/physics/PhysicsEngine.h), [PhysicsEngine.cpp](../src/physics/PhysicsEngine.cpp), [TileMap.h](../include/level/TileMap.h), [TileMap.cpp](../src/level/TileMap.cpp), [Level.cpp](../src/level/Level.cpp), [Mario.cpp](../src/entities/Mario.cpp), [PauseState.h](../include/states/PauseState.h), [PauseState.cpp](../src/states/PauseState.cpp), [GameOverState.h](../include/states/GameOverState.h), [GameOverState.cpp](../src/states/GameOverState.cpp), [WinState.h](../include/states/WinState.h), [WinState.cpp](../src/states/WinState.cpp)
- **Mô tả:** 
  1. **Sửa warning IDE `Koopa.cpp`**: Loại bỏ `#include "core/SpriteFrames.h"` không sử dụng trực tiếp trong `src/entities/Koopa.cpp`.
  2. **Tích hợp bộ helper giao diện UI (`UILayoutHelper` & `UIMenuWidget`)**: Thêm module định vị UI theo điểm neo (`UIAnchor`) và widget menu tương tác hỗ trợ phím mũi tên (`Up`/`Down`/`W`/`S`) + `Enter`.
  3. **Tối ưu hóa game loop (`Game.cpp`)**: Bổ sung `MAX_DELTA_TIME` clamping (0.1s), xử lý an toàn khi đóng cửa sổ (`m_window.close(); return;`), và bắt buộc khóa tỉ lệ màn hình 16:9 khi resize window.
  4. **Tách lớp Render Foreground (`TileMap` & `Level`)**: Bổ sung `renderForeground()` vẽ khối gạch, ống nước, cờ đè lên entity và Mario theo đúng chuẩn NES.
  5. **Giới hạn Substep Physics (`PhysicsEngine`)**: Thêm giới hạn `MAX_SUBSTEPS = 8` để ngăn ngừa lặp vô hạn catch-up lag khi giật khung hình.
  6. **Giải quyết xung đột 5 file**: Xử lý mượt mà conflict ở `PauseState.h/.cpp`, `GameOverState.cpp`, `WinState.cpp`, `TileMap.cpp`. Kết hợp menu `UIMenuWidget` với chức năng chỉnh âm lượng Music/SFX bằng phím Trái/Phải.
  7. **Biên dịch & CTest 9/9 Pass**: Biên dịch Debug/Release thành công 100%, tất cả 9/9 bài test CTest đều vượt qua.

### Entry #18: [Clean Code & Warnings Fix] - Loại Bỏ `#include "core/DisplayConfig.h"` Thừa Trong Các State UI
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [GameOverState.cpp](../src/states/GameOverState.cpp), [PauseState.cpp](../src/states/PauseState.cpp), [WinState.cpp](../src/states/WinState.cpp)
- **Mô tả:** Loại bỏ các câu lệnh `#include "core/DisplayConfig.h"` thừa không được sử dụng trực tiếp trong `GameOverState.cpp`, `PauseState.cpp`, và `WinState.cpp`, dọn sạch cảnh báo IDE và tối ưu hóa biên dịch.

### Entry #19: [Clean Code & Compiler Warning Fix] - Handle `MarioState::FIRE_SMALL` in `HUD::getPowerLabel()`
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [HUD.cpp](../src/ui/HUD.cpp)
- **Mô tả:** Bổ sung `case MarioState::FIRE_SMALL` vào câu lệnh `switch` trong phương thức `HUD::getPowerLabel()`, dọn dứt điểm cảnh báo `-Wswitch` của trình biên dịch GCC/Clang.


### Entry #20: [Gate 0 Contract Compliance] - Purge FIRE_SMALL, Fix PLAYER_LOST_LIFE & Integrate SaveManager
- **Trạng thái:** Đã hoàn thành, build & test pass 100%.
- **File ảnh hưởng:** `Mario.h`, `Mario.cpp`, `HUD.cpp`, `PlayState.cpp`, `FireFlower.cpp`, `Mushroom.cpp`, `GameManager.h`, `GameManager.cpp`, `MenuState.cpp`, `GameOverState.cpp`, `WinState.cpp`, `CMakeLists.txt`
- **Mô tả:** 
  1. **Purge FIRE_SMALL**: Xóa bỏ hoàn toàn trạng thái `FIRE_SMALL` (vi phạm Gate 0). Khi nhặt FireFlower ở trạng thái `SMALL`, Mario chuyển trực tiếp sang `FIRE` (tức Fire Big Mario).
  2. **Remove PLAYER_LOST_LIFE**: Xóa `PLAYER_LOST_LIFE` event để tránh race condition, thay vào đó `HUD` dùng callback timeout trỏ thẳng tới `Mario::loseLife()`, và `PlayState` xử lý mạng trong loop.
  3. **SaveManager Integration**: Đưa `SaveManager` vào trong `GameManager` singleton. Mọi truy cập vào save data đều thông qua `GameManager::getInstance().getSaveManager()`. `MenuState` hiển thị High Score, `WinState`/`GameOverState` cập nhật High Score.
  4. **Rename target**: Đổi tên target từ `main` sang `SuperMario` trong `CMakeLists.txt`.

### Entry #21: [Fix HUD & Character State Desync] - Đồng Bộ Trạng Thái Nhân Vật Với HUD Góc Trên Bên Trái
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [HUD.cpp](../src/ui/HUD.cpp), [PlayState.cpp](../src/states/PlayState.cpp), [Mario.h](../include/entities/Mario.h), [TV5IntegrationTests.cpp](../tests/TV5IntegrationTests.cpp), [change_in_test_game.md](change_in_test_game.md)
- **Mô tả:** 
  1. **Khắc phục lỗi tồn đọng cờ Star Power (`m_starPowerActive`)**: Đăng ký `HUD` lắng nghe sự kiện `PLAYER_POWER_DOWN`. Tự động reset `m_starPowerActive = false` khi nhận sự kiện `PLAYER_DIED`, `PLAYER_POWER_DOWN`, hoặc `LEVEL_STARTED`. Giúp loại bỏ hoàn toàn lỗi góc trái hiển thị `POWER STAR` sau khi nhân vật qua đời / mất mạng / giảm trạng thái.
  2. **Đồng bộ HUD khi restore progress (`PlayState::restoreProgress`)**: Thêm lời gọi `m_hud->update()` ngay sau khi gán lại điểm, xu, mạng và `MarioState` trong `restoreProgress()`, đảm bảo HUD hiển thị đúng ngay tại frame 0.
  3. **Đồng bộ default initializer `m_characterType`**: Đưa giá trị mặc định của `m_characterType` trong `Mario.h` về `CharacterType::MARIO` khớp chuẩn xác với các constructor trong `Mario.cpp`.
  4. **Bổ sung regression unit tests (`TV5IntegrationTests.cpp`)**: Thêm assertion kiểm thử tự động xác nhận `PLAYER_DIED` và `PLAYER_POWER_DOWN` reset nhãn `POWER` về đúng `SMALL`. Toàn bộ 9/9 bộ test CTest đều vượt qua.

### Entry #22: [Fix Dead Enemy Collision] - Bỏ Qua Va Chạm & Gây Sát Thương Khi Quái Đã Chết/Đang Rơi
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [Enemy.h](../include/entities/Enemy.h), [Goomba.h](../include/entities/Goomba.h), [Koopa.h](../include/entities/Koopa.h), [CollisionManager.cpp](../src/physics/CollisionManager.cpp), [change_in_test_game.md](change_in_test_game.md)
- **Mô tả:** 
  1. **Thêm giao diện `Enemy::isDying()`**: Khai báo `virtual bool isDying() const` ở `Enemy.h` và override ở `Goomba.h` (`m_isStomped || m_isFlippedDead || isDead() || !isActive()`) cũng như `Koopa.h` (`m_isFlippedDead || isDead() || !isActive()`).
  2. **Tắt va chạm vật lý Box2D (`CollisionManager::preSolve`)**: Gọi `contact->SetEnabled(false)` khi một trong hai entity va chạm là quái đang dying/dead, giúp Mario và các vật thể khác đi qua quái đang lơ lửng/rơi tự do mà không bị đẩy hay nảy.
  3. **Bỏ qua gây sát thương cho Mario (`CollisionManager::handleMarioCollision`)**: Thêm kiểm tra `if (enemy->isDying()) return;` ngay đầu luồng xử lý va chạm Mario-quái. Mario không còn bị mất mạng hay bị trừ máu khi chạm trúng quái đã bị đạp bẹp hoặc quái đang ngửa bụng lơ lửng rơi xuống vực.

### Entry #23: [Fix Goomba Squished Sprite Position] - Sửa Lỗi Lệch Vị Trí Frame Bẹp Của Goomba
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [Goomba.cpp](../src/entities/Goomba.cpp), [change_in_test_game.md](change_in_test_game.md)
- **Mô tả:** 
  1. **Sửa tính toán chân Sprite Goomba (`syncSpriteToFeet`)**: Đổi tính toán vị trí đáy sprite từ `m_position.y + GOOMBA_SIZE.y` (32px cố định) thành `m_position.y + m_size.y` (16px khi bị đạp).
  2. **Kết quả**: Loại bỏ hoàn toàn hiện tượng sprite Goomba bị dậm bẹp bị chìm 16px sâu bên dưới mặt đất/gạch. Sprite squished phẳng lì nằm chuẩn xác ngay trên mặt sàn.

### Entry #24: [Fix Enemy Sprite Orientation] - Lật Sprite Hướng Mặt Quái Theo Hướng Di Chuyển
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [Koopa.cpp](../src/entities/Koopa.cpp), [Goomba.cpp](../src/entities/Goomba.cpp), [change_in_test_game.md](change_in_test_game.md)
- **Mô tả:** 
  1. **Lật tỉ lệ scale & origin theo hướng di chuyển (`syncSpriteToFeet`)**: Khi `getFacingDirection() == Direction::RIGHT`, thiết lập `m_sprite->setScale({-SPRITE_SCALE, SPRITE_SCALE})` và `setOrigin({rect.size.x, 0.f})`. Khi hướng `LEFT`, giữ `setScale({SPRITE_SCALE, SPRITE_SCALE})` và `setOrigin({0.f, 0.f})`.
  2. **Kết quả**: Loại bỏ hoàn toàn lỗi Koopa / Goomba bị đi giật lùi (moonwalk) khi di chuyển sang phải. Quái luôn quay mặt về đúng hướng di chuyển 100%.

### Entry #25: [Fix Star Invincibility Expiration Event] - Phát Sự Kiện PLAYER_INVINCIBILITY_EXPIRED Khi Hết Hạn Ngôi Sao
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [Mario.cpp](../src/entities/Mario.cpp), [HUD.cpp](../src/ui/HUD.cpp), [TV5IntegrationTests.cpp](../tests/TV5IntegrationTests.cpp), [change_in_test_game.md](change_in_test_game.md)
- **Mô tả:** 
  1. **Bổ sung phát sự kiện hết hạn tàng hình Sao (`Mario::updateInvincibility`)**: Thêm `EventBus::getInstance().notify(EventType::PLAYER_INVINCIBILITY_EXPIRED)` khi bộ đếm 10 giây Starman (`m_starInvincibilityTimer`) đếm về 0.
  2. **Ràng buộc kiểm tra hai lớp ở HUD (`HUD::getPowerLabel`)**: Đổi điều kiện nhãn `STAR` thành `m_starPowerActive && m_mario.isStarInvincible()`.
  3. **Kết quả**: Khi hiệu ứng Sao 10 giây kết thúc, HUD tự động chuyển nhãn `POWER STAR` về lại trạng thái chuẩn (`POWER SMALL` / `POWER SUPER` / `POWER FIRE`) ngay tức thì, loại bỏ dứt điểm hiện tượng treo nhãn `POWER STAR` vĩnh viễn.

### Entry #26: [Tune Mario Skid Duration] - Tăng Thời Gian Hiệu Ứng Phanh Lại Khi Chạy Nhanh
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [Mario.cpp](../src/entities/Mario.cpp), [change_in_test_game.md](change_in_test_game.md)
- **Mô tả:** 
  1. **Điều chỉnh ma sát phanh (`SKID_FRICTION`)**: Đổi hằng số `SKID_FRICTION` từ `2200.f` về `1200.f` trong `Mario.cpp`.
  2. **Kết quả**: Khi Mario đang chạy nhanh (tốc độ tối đa 340 px/s) và bấm phím ngược hướng để phanh lại, thời gian trượt phanh và hiển thị animation `skid` kéo dài từ ~0.15s lên ~0.28s, mang lại cảm giác phanh trượt mượt mà và đúng chuẩn game Mario gốc.

### Entry #27: [Merge TV4 Branch & Finalize Level/Enemy System] - Integrated feature/level-and-enemy into develop
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (10/10 ctest passed).
- **File ảnh hưởng:** `CMakeLists.txt`, `docs/management/TV4_CHANGES_SUMMARY.md`, [TileMap.h](../include/level/TileMap.h), [CollisionManager.h](../include/physics/CollisionManager.h), [level0.txt](../levels/level0.txt), [level1.txt](../levels/level1.txt), [level2.txt](../levels/level2.txt), [level3.txt](../levels/level3.txt), [Goomba.cpp](../src/entities/Goomba.cpp), [Koopa.cpp](../src/entities/Koopa.cpp), [Level.cpp](../src/level/Level.cpp), [TileMap.cpp](../src/level/TileMap.cpp), [CollisionManager.cpp](../src/physics/CollisionManager.cpp), [ContactListener.cpp](../src/physics/ContactListener.cpp), [LevelValidatorTests.cpp](../tests/LevelValidatorTests.cpp)
- **Mô tả:** 
  1. **Level File Validation**: Bổ sung cơ chế validate nghiêm ngặt file level trong `TileMap::loadFromFile()` (đúng 1 Mario spawn `M`, 1 finish point `F`, cột cờ `|` liên tục kết thúc ở ô solid, và cùng chiều rộng hàng). Thêm bộ test `LevelValidatorTests.cpp` với 8 bài unit test tự động (100% pass).
  2. **Release Level Normalization**: Chuẩn hóa cấu trúc và layout 3 màn chơi chính Level 1 (183×12), Level 2 (250×12), Level 3 (281×12) và fixture Level 0. Bổ sung các cụm pipe traversal và phần thưởng Fire Flower ở giữa Level 2.
  3. **Cải tiến Tile & Physics Collision Pipeline**: Tinh chỉnh AI quái Goomba/Koopa với ledge detection và bỏ qua va chạm mép gạch (`isWalkableSupportSeam`). Dependency injection truyền `TileMap&` qua `ContactListener` và `CollisionManager`. Chuẩn hóa `QuestionBlock` làm Single Source of Truth cho tile hits. Mở rộng finish trigger bao phủ toàn bộ chiều cao cột cờ.
  4. **Merge & Giải Quyết Xung Đột**: Hợp nhất thành công nhánh `feature/level-and-enemy` vào `develop`. Giải quyết xung đột merge tại `Koopa.cpp` và `CollisionManager.cpp`, giữ nguyên cả logic bỏ qua va chạm quái dying/dead lẫn logic seam check. Xóa `#include "entities/FireBall.h"` bị lặp trong `Level.cpp`. Toàn bộ 10/10 ctest đều vượt qua.








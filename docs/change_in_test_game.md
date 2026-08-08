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
- **File:** [Mario.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp)
  - `update()`: Thêm lời gọi `updateInvincibility(dt)` ngay sau `updateAnimation(dt)` để kích hoạt đồng hồ đếm lùi 10 giây bất tử Ngôi Sao.
  - **Cơ chế nạp hiệu ứng NES gốc**:
    - **Ăn tiếp Ngôi Sao**: Bộ đếm 10s tự reset lại từ đầu (không cộng dồn lên 20s).
    - **Nâng cấp trạng thái (Mushroom/Flower)**: Giữ nguyên hình dạng cơ thể và quyền bắn đạn, đồng thời khoác lớp Cầu Vồng Bất Tử 10s. Khi hết 10s, Mario tự động về màu trạng thái hiện tại.

### Class QuestionBlock (Multi-Theme Palette Support)
- **File:** [QuestionBlock.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/QuestionBlock.h), [QuestionBlock.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/QuestionBlock.cpp)
  - Thêm enum `BlockTheme` (`OVERWORLD`, `UNDERGROUND`, `CASTLE`, `UNDERWATER`).
  - Hỗ trợ tự động chuyển đổi animation chớp nháy 3 frame (`idle`) và ô phẳng sau khi nẩy (`empty`) chuẩn xác theo từng bảng màu môi trường NES trong `SpriteFrames::Blocks`.

---

## 3. MODIFIED FILES LOG (Chi tiết các file đã sửa)

- **[Mario.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp)**:
  - `Dòng 230`: Bổ sung lời gọi `updateInvincibility(dt)` trong hàm `update(float dt)`.
- **[EntityFactory.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/patterns/EntityFactory.cpp)**:
  - Khởi tạo mã tile `?` cố định thành `QuestionBlockContent::SUPER_MUSHROOM` (chắc chắn luôn ra Nấm Super Mushroom khi đập).
- **[QuestionBlock.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/QuestionBlock.h)**:
  - Thêm enum `BlockTheme` và cập nhật constructor nhận tham số `BlockTheme theme = BlockTheme::OVERWORLD`.
- **[QuestionBlock.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/QuestionBlock.cpp)**:
  - Nạp chuỗi animation `idle` và sprite `empty` linh hoạt theo `BlockTheme` từ `SpriteFrames::Blocks`.
- **[TileMap.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/level/TileMap.cpp)**:
  - Cập nhật hàm `TileMap::hitTile()` truyền `TextureManager` và gắn 4 sub-rect mảnh vỡ gạch 4 góc (`DEBRIS_TOP_LEFT`, `DEBRIS_TOP_RIGHT`, `DEBRIS_BOTTOM_LEFT`, `DEBRIS_BOTTOM_RIGHT`) trực tiếp vào 4 đối tượng `BlockDebris` khi Mario to húc vỡ gạch Brick Block.
- **[SpriteFrames.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/core/SpriteFrames.h)**:
  - Cập nhật namespace `SpriteFrames::Blocks` với đầy đủ tọa độ chuẩn của 120 ô khối gạch (`items_blocks.png`) qua 4 bảng màu NES (Overworld, Underground, Castle, Underwater). Bổ sung 4 sub-rect 8×8 px (`DEBRIS_TOP_LEFT`, `DEBRIS_TOP_RIGHT`, `DEBRIS_BOTTOM_LEFT`, `DEBRIS_BOTTOM_RIGHT`) và các hàm animation helper `ugQuestionBlockFrames()`, `castleQuestionBlockFrames()`, `uwQuestionBlockFrames()`.
- **[BlockDebris.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/BlockDebris.h)**:
  - Cập nhật constructor nhận tham số `sf::IntRect frame` tùy chọn.
- **[BlockDebris.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/BlockDebris.cpp)**:
  - Đổi rect mặc định của `BlockDebris` sang `SpriteFrames::Blocks::DEBRIS_TOP_LEFT`, bổ sung `updateAnimation(dt)` và thiết lập tâm xoay `origin` + tỉ lệ phóng `scale 2.0x` cho hạt mảnh vỡ.
- **[blocks_coordinate.md](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/docs/blocks_coordinate.md)**:
  - Phân tích toàn bộ 120 ô khối gạch & hộp quà nghi vấn `?` (Brick Blocks, Question Blocks, Brick Debris) trích xuất trực tiếp từ tệp hình ảnh gốc **`assets/textures/items/items_blocks.png`** (kích thước 448×256 px). Bổ sung hướng dẫn kỹ thuật tách 4 sub-rect 8×8 px cho 16 ô mảnh vỡ gạch (`BlockDebris`).
- **[level3.txt](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/levels/level3.txt)**:
  - Khắc phục cấu trúc cột cờ về đích (`F` và `|` cột cờ) bị thiếu nối xuống ô đất `1`, làm hàm `TileMap::loadFromFile()` ném lỗi validation và thoát về Menu thay vì vào màn Level 3 / YOU WIN.
- **[blocks_all_components_atlas.png](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/assets/textures/items/blocks_all_components_atlas.png)**:
  - Tệp ảnh Atlas minh họa trực quan (880×1050 px) hiển thị 120 ô khối được trích xuất và gán nhãn chỉ số #1–#120 trực tiếp từ tệp **`assets/textures/items/items_blocks.png`**.
- **[Mario.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/Mario.h)**:
  - Thêm enum `CharacterType` (`MARIO`, `LUIGI`), thiết lập nhân vật mặc định là `CharacterType::LUIGI`, bổ sung `getCharacterType()` và `setCharacterType()`.
- **[Mario.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp)**:
  - Cập nhật `setupAnimationsForState` hỗ trợ nạp bộ clip animation của Luigi (`SmallLuigi`, `BigLuigi`) khi `charType == CharacterType::LUIGI`. Trạng thái Lửa `MarioState::FIRE` dùng chung bộ frame `SpriteFrames::FireBigMario`.
  - Cập nhật các constructors, `powerUp()`, `respawn()`, `setMarioState()`, và hàm `setCharacterType()`.

---

## 4. DETAILED LOGIC CHANGE LOG (For Opus Review)

### Entry #1: [Bugfix] - Khắc Phục Lỗi Đồng Hồ Bất Tử 10s Bị Đóng Băng & Quy Tắc Trùng Lặp Hiệu Ứng NES
- **Trạng thái:** Đã hoàn thành, build & test pass 100%.
- **File ảnh hưởng:** [Mario.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp)
- **Vấn đề cũ (Before):** Hàm `updateInvincibility(dt)` không được gọi trong `Mario::update()`, dẫn đến việc biến `m_starInvincibilityTimer` không bao giờ giảm và Mario bị bất tử vĩnh viễn.
- **Phương pháp áp dụng (Methodology):** Thêm `updateInvincibility(dt)` vào vòng lặp `Mario::update()`, đồng thời áp dụng quy tắc nạp hiệu ứng chuẩn NES gốc (reset 10s khi ăn tiếp Ngôi Sao, giữ nguyên dạng Mario To / Fire Mario).
- **Kết quả hiện tại (Impact):** Bất tử Ngôi Sao chạy chuẩn 10 giây, đổi màu Cầu Vồng sắc nét và hết giờ đúng thời hạn.

### Entry #2: [Refactoring & Particle Rendering Fix] - Nâng Cấp Multi-Theme Cho `QuestionBlock` & Khắc Phục Lỗi Hiển Thị Mảnh Vỡ `BlockDebris`
- **Trạng thái:** Đã hoàn thành, build pass 100%.
- **File ảnh hưởng:** [SpriteFrames.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/core/SpriteFrames.h), [QuestionBlock.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/QuestionBlock.h), [QuestionBlock.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/QuestionBlock.cpp), [BlockDebris.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/BlockDebris.h), [BlockDebris.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/BlockDebris.cpp), [TileMap.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/level/TileMap.cpp), [blocks_coordinate.md](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/docs/blocks_coordinate.md)
- **Mô tả:** 
  1. **Khắc phục lỗi hiển thị `BlockDebris`**: Do `TileMap::hitTile()` khi sinh 4 mảnh vỡ không gọi `setTextureManager(*textureManager)`, dẫn tới `m_sprite` bị rỗng (`nullopt`) và hạt vỡ gạch không hiển thị trên màn hình. Đã bổ sung gắn `TextureManager`, gọi `updateAnimation(dt)` và thiết lập tâm xoay origin + scale 2.0x cho hạt mảnh vỡ.
  2. **Đồng bộ `SpriteFrames::Blocks`**: Đồng bộ 120 ô khối trong `items_blocks.png` từ `blocks_coordinate.md` vào `SpriteFrames::Blocks` hỗ trợ cả 4 môi trường NES.
  3. **Nâng cấp `QuestionBlock`**: Thêm enum `BlockTheme` tự động đổi bộ 3-frame animation chớp nháy và sprite ô phẳng `empty` phù hợp môi trường.

### Entry #3: [Feature & Testing] - Mặc Định Nhân Vật Luigi (Luigi Character Skin Support)
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Mario.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/Mario.h), [Mario.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp)
- **Mô tả:** Đã thiết lập nhân vật mặc định là Luigi (`CharacterType::LUIGI`) theo yêu cầu chạy thử nghiệm. Tự động nạp bộ frame set của Luigi (Small Luigi, Big Luigi) từ spritesheet `MarioLuigi.png`. Trạng thái Fire (`MarioState::FIRE`) được dùng chung bộ frame `SpriteFrames::FireBigMario` và bảng màu Lửa NES (tint da cam) chuẩn xác cho cả Mario và Luigi, giữ file `SpriteFrames.h` vô cùng gọn sạch.


### Entry #4: [Documentation & Asset Analysis] - Phân Tích & Bóc Tách 282 Objects Trong `items_objects.png`
- **Trạng thái:** Đã hoàn thành 100%.
- **File ảnh hưởng:** [items_objects_coordinate.md](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/docs/items_objects_coordinate.md), [items_objects_atlas.png](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/assets/textures/items/items_objects_atlas.png)
- **Mô tả:** 
  1. **Bóc tách 282 Objects trong `items_objects.png`**: Phân tích toàn bộ 282 vật thể/vật phẩm nằm trong vùng khung tím thuộc 4 môi trường bảng màu NES (Overworld, Underground, Castle, Underwater).
  2. **Tạo tài liệu `items_objects_coordinate.md`**: Cập nhật đầy đủ tọa độ Loang, kích thước Loang, tọa độ Grid Khung, kích thước Khung, số Pixel, Palette môi trường và mô tả ý nghĩa chi tiết (Super/1-Up Mushroom, Fire Flower, Super Star, Spinning Coin, Fireball, Firework Explosion, Spring/Trampoline, Moving Platform, Flagpole Flag, Castle/Star Flag, Beanstalk Vine).
  3. **Tạo ảnh Atlas minh họa `items_objects_atlas.png`**: Gen ảnh Atlas dạng lưới 8 cột (chuẩn phong cách `blocks_all_components_atlas.png`), ghi nhãn STT kèm tọa độ Khung và vẽ bounding box màu xanh lơ ôm sát từng vật thể.

### Entry #5: [Bugfix & Code Sync] - Bổ Sung Inline Vector Functions Cho `SpriteFrames::Items` & Sửa Lỗi Biên Dịch Build
- **Trạng thái:** Đã hoàn thành, build pass 100%.
- **File ảnh hưởng:** [SpriteFrames.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/core/SpriteFrames.h), [change_in_test_game.md](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/docs/change_in_test_game.md)
- **Mô tả:** 
  1. **Khắc phục lỗi biên dịch `Coin.cpp`, `FireFlower.cpp`, `Star.cpp`**: Do `SpriteFrames::Items` trước đó khai báo biến dạng vector tĩnh (`spinningCoinFrames`, `superStarFrames`, `fireFlowerFrames`), trong khi code `Coin.cpp`, `FireFlower.cpp`, `Star.cpp` gọi dưới dạng hàm (`coinFrames()`, `starFrames()`, `fireFlowerFrames()`).
  2. **Bổ sung các hàm helper `inline const std::vector<sf::IntRect>&`**: Thêm các hàm `coinFrames()`, `spinningCoinFrames()`, `starFrames()`, `superStarFrames()`, `fireFlowerFrames()`, `fireballFrames()`, `fireballExplosionFrames()` trong namespace `SpriteFrames::Items` tuân thủ đúng chuẩn của các namespace khác trong `SpriteFrames.h`.
  3. **Xác nhận tọa độ**: Xác nhận 100% tọa độ `IntRect` của toàn bộ item trong `SpriteFrames::Items` trùng khớp hoàn toàn với bảng tọa độ chuẩn `docs/items_objects_coordinate.md`.
### Entry #6: [Bugfix & Rendering Sync] - Sửa Lỗi Hiển Thị Sprite Khi Chết & Loại Bỏ Delay Load Nhân Vật Đặc Biệt
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Mario.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp), [PlayState.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/states/PlayState.cpp)
- **Mô tả:** 
  1. **Sửa lỗi hiển thị sprite khi chết (`Mario::loseLife()`)**: Khi ở dạng Fire Mario/Big Mario bị chết, hàm `loseLife()` lập tức chuyển trạng thái về `SMALL`, gọi `setupAnimationsForState()` để nạp đúng sprite animation chết của Luigi/Mario mặc định (`DEATH`), đồng thời reset màu sprite về `sf::Color::White` ngắt màu tint da cam.
  2. **Triệt tiêu độ trễ 1-frame khi load nhân vật đặc biệt (`PlayState::restoreProgress()`)**: Thêm `m_level->update(0.f)` ngay trong `restoreProgress()` sau khi phục hồi trạng thái `FIRE` / `SUPER`, giúp vị trí, tỉ lệ phóng 2x và camera đồng bộ ngay lập tức trước khi Render frame 0.

### Entry #7: [Feature & Keybinding] - Kích Hoạt Chiêu Bắn Đạn Lửa (Fireball Shooting) Cho Fire Mario
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Level.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/level/Level.h), [Level.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/level/Level.cpp), [PlayState.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/states/PlayState.cpp), [FireBall.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/FireBall.cpp)
- **Mô tả:** 
  1. **Tích hợp `Level::shootFireBall()`**: Gọi `Mario::shootFireBall(b2World*)` sinh ra `FireBall` projectile, gắn `TextureManager`, thêm vào `m_entities` và phát âm thanh `fireball.wav` qua `SoundManager`.
  2. **Gán phím bắn đạn lửa (`ShootCommand`)**: Phím **`Shift`** (Cả Left Shift & Right Shift), **`J`**, **`F`**, **`X`**, **`Left Control`** được gán lệnh bắn đạn lửa qua `InputHandler` trong `PlayState::rebindCommands()`.
  3. **Hoàn thiện Animation & Rendering `FireBall`**: Thêm animation `spin` (4-frame xoay tròn đạn lửa 8×8), tự động nạp scale 2.0x và va chạm nảy tâng tâng khi chạm đất, tiêu diệt quái Goomba/Koopa khi trúng đích.

### Entry #8: [Bugfix & Animation] - Tiêu Diệt Quái Khi Trúng Đạn Lửa & Đồng Bộ Dáng Ném Đạn Lửa Cho Cả 2 Nhân Vật
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [CollisionManager.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/physics/CollisionManager.cpp), [Mario.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp)
- **Mô tả:** 
  1. **Xử lý tiêu diệt quái tức thì (`CollisionManager::resolve()`)**: Thêm `enemy->markForRemoval()` và thông báo `EventType::ENEMY_STOMPED` khi `FireBall` va chạm với Quái (`isEnemy()`), giúp quái biến mất và cộng điểm ngay lập tức khi dính đạn lửa.
  2. **Dáng ném đạn lửa đồng bộ (`setupAnimationsForState()`)**: Bổ sung clip animation `action` (dáng giơ tay ném đạn lửa `ACTION` frame từ `SpriteFrames::FireBigMario`) cho cả Mario và Luigi khi ở trạng thái `FIRE`. Tự động kích hoạt khi nhấn phím bắn.

### Entry #9: [Architecture & Fix] - Xử Lý Hàng Đợi Bắn Đạn Lửa Tránh Trùng Lặp Khóa Vật Lý Box2D (World Locked Fix)
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Level.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/level/Level.h), [Level.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/level/Level.cpp)
- **Mô tả:** 
  1. **Giải quyết triệt để thông báo `PhysicsEngine world null or locked!`**: Khi bấm phím bắn đạn lửa trùng đúng thời điểm Box2D đang tính toán va chạm (`m_world->IsLocked()`), `Level::shootFireBall()` sẽ lưu yêu cầu vào hàng đợi `m_pendingFireBallRequests`.
  2. **Tạo vật thể đạn lửa an toàn (`processPendingFireballs()`)**: Ngay khi Box2D kết thúc bước tính toán (`PhysicsEngine::update()`), hàm `processPendingFireballs()` sẽ khởi tạo thân vật lý đạn lửa khi thế giới ở trạng thái an toàn (!IsLocked). Đảm bảo 100% các phím nhấn bắn đạn đều thành công và không bao giờ bị nốt bỏ.

### Entry #10: [Feature & Visual Fix] - Hiệu Ứng Lật Bụng Khi Bị Bắn Đạn Lửa & Hỗ Trợ Dạng Lửa Nhỏ (Fire Small Mario)
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Enemy.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/Enemy.h), [Enemy.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Enemy.cpp), [Goomba.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/Goomba.h), [Goomba.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Goomba.cpp), [Koopa.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/Koopa.h), [Koopa.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Koopa.cpp), [FireFlower.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/items/FireFlower.cpp), [Mario.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/Mario.h), [Mario.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp), [HUD.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/ui/HUD.cpp)
- **Mô tả:** 
  1. **Hiệu ứng quái lật bụng (`onFireHit()`)**: Khi Goomba hoặc Koopa dính đạn lửa, quái sẽ **lật ngược bụng lên trên** (`setScale(2.f, -2.f)`), nảy nhẹ lên trên rồi nảy văng rơi tự do xuống dưới màn hình kèm âm thanh `kickkill.wav` đúng chuẩn NES gốc.
  2. **Hỗ trợ `FIRE_SMALL` (Lửa Nhỏ 16x16px)**: Khi Mario Nhỏ (`SMALL`) ăn Hoa Lửa, nhân vật biến thành **Fire Small Mario** (Lửa Nhỏ 16x16px) giữ nguyên thân hình nhỏ thay vì phóng to thành Khổng lồ Lửa. Khi Mario To (`SUPER`) ăn Hoa Lửa mới biến thành **Fire Big Mario** (Khổng Lồ Lửa 16x32px). Cả 2 dạng đều giữ trọn khả năng bắn đạn lửa.

### Entry #11: [Bugfix & Shooting Fix] - Cho Phép `FIRE_SMALL` Bắn Đạn Lửa Trong `Mario::canShootFireBall()`
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Mario.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp)
- **Mô tả:** Sửa hàm `canShootFireBall()` cho phép bắn đạn lửa khi `m_marioState == MarioState::FIRE || m_marioState == MarioState::FIRE_SMALL`. Khắc phục nguyên nhân Mario Lửa Nhỏ không thể phóng đạn lửa khi bấm phím bắn.

### Entry #12: [Clean Log & Physics] - Triệt Tiêu Log Cảnh Báo Giả `PhysicsEngine world null or locked!`
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [FireBall.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/FireBall.cpp), [Entity.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Entity.cpp)
- **Mô tả:** 
  1. **Loại bỏ `initPhysics(nullptr)` trong Constructor mặc định (`FireBall::FireBall()`)**: Trong Constructor mặc định của `FireBall`, không gọi `initPhysics(nullptr)` khi chưa có con trỏ thế giới `world`. Hàm `spawn(pos, dir, world)` sau đó sẽ trực tiếp khởi tạo thân vật lý chuẩn xác.
  2. **Tối ưu hóa kiểm tra con trỏ `Entity::initPhysics()`**: Khi `world == nullptr`, hàm lặng lẽ `return` thay vì in cảnh báo nhầm lẫn `world null or locked!`. Log cảnh báo chỉ được in khi `world->IsLocked()` thực sự xảy ra.

### Entry #13: [Feature & Limit] - Giới Hạn Tối Đa 4 Viên Đạn Lửa Đồng Thời Trên Màn Hình
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Level.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/level/Level.cpp)
- **Mô tả:** Thêm hằng số `MAX_ACTIVE_FIREBALLS = 4` và bộ đếm số đạn lửa đang hoạt động trong `Level::shootFireBall()`. Giới hạn màn hình chỉ xuất hiện tối đa **4 viên đạn lửa** đồng thời. Khi có đủ 4 viên đang nảy, phím bắn sẽ tạm ngưng cho tới khi ít nhất 1 viên chạm quái/tường nổ biến mất.

### Entry #14: [Fix Spawn Clipping & Grace Period] - Nâng Độ Cao Spawn Đạn Lửa & Tạo Khoảng Đệm 50ms Tránh Nổ Tức Thì
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [FireBall.h](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/include/entities/FireBall.h), [CollisionManager.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/physics/CollisionManager.cpp), [Mario.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/entities/Mario.cpp), [Level.cpp](file:///f:/APCS(2025-2026)/HK3/OOP/CS202-Group04-FinalProject/src/level/Level.cpp)
- **Mô tả:** 
  1. **Tăng độ cao vị trí sinh đạn lửa (`spawnY = m_position.y + 4.f`)**: Phóng đạn từ vị trí bàn tay Mario (ngay trên ngực), giúp viên đạn sinh ra lơ lửng cách mặt đất 10px thay vì bị dính vào mép gạch dưới chân.
  2. **Tạo khoảng đệm an toàn 50ms (`getLifetime() > 0.05f`)**: Bỏ qua va chạm với thân Mario (`target->isMario()`) và yêu cầu đạn tồn tại ít nhất 50ms trước khi chấp nhận va chạm hủy đạn vào tường đứng. Giúp đạn lửa bay ra mượt mà 100% ở bất kỳ ngóc ngách hay khu vực gạch nào.











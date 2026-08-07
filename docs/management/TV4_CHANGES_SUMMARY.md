

# **Tổng Kết Những Thay Đổi Của TV4 — Sprint 6**

> **Tác giả:** TV4 (Vy) — Level, Enemy & SaveManager  
> **Phạm vi hiện tại:** S6-TV4-02 đến S6-TV4-09, S6-TV4-11 đến S6-TV4-14
> **Trạng thái:** Build thành công, toàn bộ CTest hiện tại đã pass  
> **Cập nhật gần nhất:** Sau khi tích hợp nhánh TV3 Mario & Physics

---

## **1. Tổng Quan**

Trong giai đoạn hiện tại của Sprint 6, TV4 đã tập trung vào việc:

1. Kiểm tra tính hợp lệ của file level trước khi đưa dữ liệu vào game.
2. Chuẩn hóa ý nghĩa của ground tile và breakable brick.
3. Loại bỏ trạng thái pending tile hit dùng chung giữa nhiều level.
4. Truyền `TileMap` thông qua collision pipeline bằng dependency injection.
5. Sửa cấu hình test sau khi tích hợp các thay đổi từ nhánh TV3.

---

## 2. Level File Validation

### File liên quan

- [`include/level/TileMap.h`](../../include/level/TileMap.h)
- [`src/level/TileMap.cpp`](../../src/level/TileMap.cpp)

### Các thay đổi

#### S6-TV4-02 — Validate đúng một Mario spawn

Mỗi level hợp lệ phải chứa đúng một ký hiệu:

```text
M
```

Nếu file không có Mario spawn hoặc có nhiều hơn một spawn, `TileMap::loadFromFile()` trả về `false` và in thông báo lỗi.

#### S6-TV4-03 — Validate đúng một finish point

Mỗi level phải chứa đúng một ký hiệu:

```text
F
```

Level thiếu hoặc có nhiều finish point sẽ không được load.

#### S6-TV4-04 — Validate cấu trúc flag pole

Finish point phải:

- Có ít nhất một pole tile `|` ngay bên dưới.
- Pole phải liên tục theo chiều dọc.
- Pole phải kết thúc trên một solid tile.
- Không được có pole tile `|` rời khỏi cột finish chính.

#### S6-TV4-05 — Validate row width và tile symbol

Các hàng của level phải có cùng chiều rộng.

Mỗi ký hiệu trong level phải thuộc tập ký hiệu được hỗ trợ. Khi phát hiện lỗi, hệ thống hiển thị:

- Số hàng.
- Số cột.
- Ký hiệu không hợp lệ.
- Đường dẫn file level.

### Phương pháp sử dụng

Các bước validation được thực hiện tự động khi gọi:

```cpp
TileMap tileMap;

if (!tileMap.loadFromFile("assets/levels/level1.txt")) {
    // Level không hợp lệ hoặc không thể load.
}
```

Các helper chính:

```cpp
validateRow(...)
validateLevelMarkers(...)
validateFlagPole(...)
```

Dữ liệu được đọc vào biến tạm `loadedGrid`. `m_grid` chỉ được cập nhật sau khi toàn bộ validation và việc load tileset thành công. Cách làm này tránh để `TileMap` rơi vào trạng thái chỉ load được một phần.

---

## 3. Ground Và Breakable Brick Semantics

### File liên quan

- [`include/level/TileSemantics.h`](../../include/level/TileSemantics.h)
- [`src/level/TileMap.cpp`](../../src/level/TileMap.cpp)

### S6-TV4-06 — Phân biệt ground và breakable brick

Ý nghĩa tile hiện tại:

| Ký hiệu | Ý nghĩa | Có thể phá |
|---|---|---|
| `1` | Ground tile | Không |
| `B` | Breakable brick | Có |
| `E` | Used/empty block | Không |

Logic kiểm tra breakable tile được tập trung tại:

```cpp
TileSemantics::isBreakable(symbol)
```

Hiện tại hàm chỉ trả về `true` đối với:

```cpp
symbol == 'B'
```

### Cách hoạt động

Mario nhỏ đập vào `B`:

```text
Brick chỉ bump.
```

Mario lớn đập vào `B`:

```text
Brick bị phá, physics terrain được xây dựng lại và BlockDebris được tạo.
```

Mario đập vào ground tile `1`:

```text
Tile không bị phá.
```

### Cách dùng

Không kiểm tra trực tiếp nhiều ký hiệu rải rác trong code:

```cpp
if (symbol == 'B') {
    // ...
}
```

Thay vào đó dùng:

```cpp
if (TileSemantics::isBreakable(symbol)) {
    // Xử lý breakable brick.
}
```

---

## 4. Pending Tile Hit Theo Từng TileMap

### File liên quan

- [`include/level/TileMap.h`](../../include/level/TileMap.h)
- [`src/level/TileMap.cpp`](../../src/level/TileMap.cpp)
- [`include/physics/ContactListener.h`](../../include/physics/ContactListener.h)
- [`src/physics/ContactListener.cpp`](../../src/physics/ContactListener.cpp)
- [`include/physics/CollisionManager.h`](../../include/physics/CollisionManager.h)
- [`src/physics/CollisionManager.cpp`](../../src/physics/CollisionManager.cpp)
- [`src/level/Level.cpp`](../../src/level/Level.cpp)

### S6-TV4-07 — Loại bỏ static pending-hit state

Trước đây pending tile hits được lưu trong biến static/global:

```cpp
std::vector<PendingTileHit> s_pendingTileHits;
```

Điều này khiến dữ liệu hit có thể tồn tại giữa hai `Level` hoặc hai `TileMap` khác nhau.

Pending hit hiện được lưu trực tiếp trong từng đối tượng `TileMap`:

```cpp
std::vector<PendingTileHit> m_pendingTileHits;
```

`queueTileHit()` cũng được chuyển từ static method thành instance method:

```cpp
tileMap.queueTileHit(column, row, overlap);
```

### Phương pháp được sử dụng

#### 1. Dependency Injection

`ContactListener` nhận `TileMap&` qua constructor:

```cpp
explicit ContactListener(TileMap& tileMap);
```

`Level` khởi tạo listener bằng:

```cpp
m_contactListener =
    std::make_unique<ContactListener>(m_tileMap);
```

#### 2. Dependency Routing

`TileMap&` được truyền theo luồng:

```text
Level
  -> ContactListener
  -> CollisionManager
  -> queueTileHit()
```

`ContactListener` gọi:

```cpp
CollisionManager::resolve(contact, m_tileMap);
```

`CollisionManager` sau đó gọi:

```cpp
tileMap.queueTileHit(column, row, overlap);
```

#### 3. Deferred Hit Processing

Collision callback chỉ đưa hit vào hàng đợi. Sau physics step, `TileMap::processPendingHits()` mới xử lý hit.

Khi nhiều tile cùng được phát hiện trong một frame, hệ thống chọn hit có overlap lớn nhất. Điều này giúp tránh hai block cùng phản ứng khi Mario đập sát ranh giới giữa hai tile.

### Cách sử dụng

Thông thường code gameplay không cần gọi trực tiếp `queueTileHit()`.

Chỉ cần tạo `ContactListener` bằng `TileMap` tương ứng:

```cpp
auto listener =
    std::make_unique<ContactListener>(tileMap);
```

Sau physics update, gọi:

```cpp
tileMap.processPendingHits(
    entities,
    textureManager,
    marioIsBig,
    mario
);
```

Khi level được thay thế, pending hits cũ cũng biến mất cùng với `TileMap` cũ.

---

## 5. Integration Và Test Fixes

### File liên quan

- [`CMakeLists.txt`](../../CMakeLists.txt)
- [`src/level/Level.cpp`](../../src/level/Level.cpp)

### Thay đổi sau khi tích hợp TV3

Sau khi merge nhánh Mario & Physics của TV3:

- Giữ lại dynamic pit threshold dựa trên chiều cao thực tế của level.
- Truyền `TileMap` cho enemy và collision pipeline.
- Thêm các source cần thiết cho target `mario_physics_tests`.
- Thêm `src/items/Item.cpp` để cung cấp implementation của:

```cpp
Item::isCollected() const
```

Việc này sửa lỗi linker:

```text
Undefined symbols for architecture x86_64:
Item::isCollected() const
```

### Kiểm tra

Các lệnh đã sử dụng:

```bash
git diff --check
cmake --build build -j1
ctest --test-dir build --output-on-failure
```

Manual testing cần xác nhận:

- Mario đập QuestionBlock vẫn hoạt động.
- Chỉ một block phản ứng khi đập sát mép.
- Ground `1` không bị phá.
- Brick `B` bị phá bởi Big Mario.
- Chuyển hoặc reload level không giữ pending hit từ level cũ.
- FireBall và enemy collisions của TV3 vẫn hoạt động.

---

## 6. QuestionBlock Là Source of Truth Cho U/O

### File liên quan

- [`include/level/TileMap.h`](../../include/level/TileMap.h)
- [`src/level/TileMap.cpp`](../../src/level/TileMap.cpp)
- [`src/entities/QuestionBlock.cpp`](../../src/entities/QuestionBlock.cpp)
- [`src/patterns/EntityFactory.cpp`](../../src/patterns/EntityFactory.cpp)

### S6-TV4-08 — Xóa đường xử lý U/O bị trùng trong TileMap

Trước đây các tile `U` và `O` có hai đường xử lý:

1. `EntityFactory` tạo `QuestionBlock` với loại item tương ứng.
2. `TileMap::hitTile()` tiếp tục tự tạo Mushroom.

Điều này tạo ra hai nguồn chịu trách nhiệm cho cùng một gameplay behavior và có nguy cơ spawn item hai lần.

Đường xử lý `U/O` đã được xóa khỏi `TileMap::hitTile()`.

Hiện tại `QuestionBlock::onHit()` là source of truth duy nhất cho:

| Tile | QuestionBlock content |
| ---- | --------------------- |
| `?`  | Coin                  |
| `U`  | Super Mushroom        |
| `O`  | 1-Up Mushroom         |

### Phương pháp được sử dụng

Thay đổi áp dụng nguyên tắc Single Responsibility và Single Source of Truth:

- `TileMap` chỉ chịu trách nhiệm cho tile terrain như ground và breakable brick.
- `QuestionBlock` chịu trách nhiệm cho trạng thái hit, bump animation và item spawning.
- `EntityFactory` chịu trách nhiệm ánh xạ tile code sang loại `QuestionBlock`.

### Cách sử dụng

Tạo QuestionBlock thông qua `EntityFactory`:

```cpp
auto block = EntityFactory::createFromTileCode(
    'U',
    position,
    world
);
```

Khi Mario đập block, collision pipeline gọi:

```cpp
block->onHit(
    mario,
    &entities,
    &textureManager
);
```

Không gọi trực tiếp logic spawn Mushroom từ `TileMap`.

---

## 7. Full-height Finish Trigger

### File liên quan

- [`src/level/Level.cpp`](../../src/level/Level.cpp)

### S6-TV4-11 — Mở rộng vùng kích hoạt trên toàn bộ cột cờ

Finish trigger trước đây chỉ có kích thước 32×32 tại tile `F`. Vì vậy, Mario phải chạm đúng phần đầu cột cờ mới có thể hoàn thành level.

Trigger hiện được tạo thành một hình chữ nhật bao phủ từ tile `F`đến tile pole `|` thấp nhất nằm trong cùng một cột.

### Phương pháp được sử dụng

`TileMap::findTiles()` được dùng để tìm:

- Finish point `F`.
- Toàn bộ pole tile `|`.

Từ hàng chứa finish point và hàng pole thấp nhất, hệ thống tính chiều cao của vùng kích hoạt bằng:

```cpp
(bottomRow - finishPosition.y + 1) * TILE_SIZE
```

Level validation đã bảo đảm mỗi level chỉ có đúng một finish point, các pole tile liên tục và nằm trong cùng một cột. Vì vậy, vùng trigger luôn tương ứng với đúng cột cờ của level.

### Cách hoạt động

Luồng tile của cột cờ có dạng:

```tex
F
|
|
|
```

Vùng trigger bao phủ toàn bộ các tile trên, nhưng không bao gồm solid tile nằm bên dưới chân cột cờ.

Mario có thể kích hoạt hoàn thành level khi chạm:

- Phần trên gần tile `F`.
- Phần giữa thân cờ.
- Phần dưới thân cờ.

### Cách sử dụng

Gameplay code không cần gọi trực tiếp vùng finish trigger.

`Level::update()` tiếp tục gọi:

```cpp
checkFinishFlag();
```

mỗi frame. Khi bounding box của Mario giao với bất kỳ phần nào trong vùng cột cờ, hệ thống đặt:

```cpp
m_levelCompleted = true;
```

và gửi event:

```cpp
EventBus::getInstance().notify(
    EventType::LEVEL_COMPLETED
);
```

Biến `m_levelCompleted` bảo đảm event hoàn thành level chỉ được gửi một lần.

### Kiểm tra

Các trường hợp manual test:

- Chạm phần trên của cột cờ → hoàn thành level.
- Chạm giữa thân cờ → hoàn thành level.
- Chạm phần dưới thân cờ → hoàn thành level.
- Đứng cạnh nhưng không giao với cột cờ → chưa hoàn thành.
- Sau khi hoàn thành, event không bị gửi lặp lại.

### S6-TV4-12 — Ngăn finish event bị kích hoạt lặp

`Level::update()` kiểm tra finish trigger ở mỗi frame. Vì Mario có thể ở trong vùng cột cờ trong nhiều frame liên tiếp, nếu không có guard thì `LEVEL_COMPLETED` có thể được phát nhiều lần.

`Level` sử dụng `m_levelCompleted` như một completion latch.

Trước khi kiểm tra collision:

```cpp
if (!m_mario || m_levelCompleted) {
    return;
}
```

Khi Mario chạm finish trigger lần đầu:

```
m_levelCompleted = true;

EventBus::getInstance().notify(
    EventType::LEVEL_COMPLETED
);
```

Sau đó mọi lần gọi `checkFinishFlag()` tiếp theo đều return ngay.

`PlayState` cũng có `m_terminalCommittedThisFrame` để tránh nhiều terminal event được commit trong cùng một frame.

Khi chuyển sang level mới, `PlayState` tạo một `Level` instance mới, nên completion state được reset về `false`.

---

## 8. Four-Tile TileMap Spritesheet

### File liên quan

- [`src/level/TileMap.cpp`](../../src/level/TileMap.cpp)
- [`levels/level1.txt`](../../levels/level1.txt)
- [`levels/level2.txt`](../../levels/level2.txt)
- [`assets/textures/tiles/tileset.png`](../../assets/textures/tiles/tileset.png)

### S6-TV4-09 — Chuyển TileMap sang tileset bốn frame

`TileMap` trước đây sử dụng `items_blocks.png` và các frame trong `SpriteFrames::Blocks` để render terrain. Điều này khiến ground, brick và finish pole phụ thuộc vào spritesheet dành cho item/block, đồng thời một số tile cũ như pipe vẫn sử dụng placeholder.

Theo cấu trúc asset mới của nhóm, `TileMap` hiện sử dụng:

```text
assets/textures/tiles/tileset.png
```

Tileset gồm đúng bốn frame nằm ngang, mỗi frame có kích thước 32×32:

| Index | Nội dung       |
| ----- | -------------- |
| 0     | Ground         |
| 1     | Brick          |
| 2     | Question block |
| 3     | Finish pole    |

### Phương pháp được sử dụng

Các frame trong tileset được truy cập thông qua named indices thay vì hardcode trực tiếp texture coordinates.

```
GROUND_TILE_INDEX
BRICK_TILE_INDEX
QUESTION_TILE_INDEX
FINISH_POLE_TILE_INDEX
```

Helper `makeTilesetRect()` chuyển tile index thành `sf::IntRect` tương ứng trong spritesheet.

`TileMap` không còn phụ thuộc vào `SpriteFrames::Blocks` để render terrain.

Question block (`?`, `U`, `O`) tiếp tục được quản lý và render bởi`QuestionBlock` entity, tránh việc cùng một block bị render hai lần.

### Loại bỏ pipe tile cũ

Các ký hiệu pipe:

```
[ ] { }
```

đã được loại khỏi level format vì tileset mới không còn chứa các frame pipe và thiết kế level hiện tại không sử dụng pipe.

Các pipe cũ trong `level1.txt` và `level2.txt` được thay bằng empty tile `.` để giữ nguyên kích thước từng hàng của level.

Các symbol tương ứng cũng được loại khỏi:

- level validation;
- solid tile semantics;
- TileMap rendering;
- tileset frame mapping.

### Kết quả

`TileMap` hiện sử dụng một tileset terrain thống nhất gồm bốn frame, không còn dùng brick hoặc empty block làm placeholder cho terrain khác.

Level files cũng được đồng bộ với tập tile symbol hiện tại.

---

## 9. Level 1 Tutorial Flow

### File liên quan

- [`levels/level1.txt`](../../levels/level1.txt)

### S6-TV4-13 — Hoàn thiện tutorial flow của Level 1

Level 1 được điều chỉnh để giới thiệu các cơ chế gameplay theo thứ tự tăng dần, thay vì đưa enemy hoặc power-up đến người chơi quá sớm.

Flow tutorial hiện tại hướng tới:

```text
Movement
   ↓
Coin
   ↓
Jump / Question Block
   ↓
Super Mushroom
   ↓
Goomba
   ↓
Finish Flag
```

### Thay đổi level

Một coin được đặt ở khu vực đầu level để tạo mục tiêu di chuyển đơn giản ngay sau khi Mario spawn.

Một QuestionBlock được đổi thành ký hiệu `U`:

```
U = QuestionBlock chứa Super Mushroom
```

`EntityFactory` đã hỗ trợ `U` dưới dạng `QuestionBlockContent::SUPER_MUSHROOM`, vì vậy thay đổi này chỉ sử dụng level symbol hiện có và không thay đổi asset path hoặc sprite coordinate.

Enemy đầu tiên tiếp tục nằm sau khu vực tutorial block/power-up để người chơi có cơ hội làm quen với movement và jump trước khi gặp nguy hiểm.

### Nguyên tắc thiết kế

Task này chỉ thay đổi bố trí gameplay trong level.

Không thay đổi:

- texture path;
- sprite sheet;
- texture rectangle;
- sprite coordinate;
- enemy implementation;
- physics constants.

Các thay đổi liên quan trực tiếp đến asset đang được tạm hoãn trong lúc
cấu trúc spritesheet chung của project vẫn đang được cập nhật.

### Kiểm tra

- Level có đúng một Mario spawn `M`.
- Level có đúng một finish point `F`.
- Coin xuất hiện trước enemy đầu tiên.
- Có QuestionBlock thường để giới thiệu block interaction.
- Có `U` để giới thiệu Super Mushroom.
- Mario có thể nhận Mushroom trước khi gặp Goomba đầu tiên.
- Người chơi có thể hoàn thành Level 1 mà không cần sử dụng damage boost.
- Level validation vẫn pass.

---

## 10. Level 1 Gap Validation

### File liên quan

- [`levels/level1.txt`](../../levels/level1.txt)

### S6-TV4-14 — Kiểm tra gap của Level 1 theo jump envelope

Level 1 được kiểm tra để bảo đảm các khoảng trống bắt buộc nằm trong khả năng jump hiện tại của Mario.

Việc đánh giá được thực hiện dựa trên bề mặt mà Mario thực sự có thể đứng và di chuyển, thay vì chỉ dựa trên các ô trống ở các hàng ground thấp nhất.

Một khu vực gần cuối level có khoảng trống dài ở phần ground bên dưới, nhưng các solid block phía trên tạo thành hai cụm bậc thang hướng vào nhau. Vì vậy khoảng cách nhảy thực tế giữa hai ledge ngắn hơn đáng kể so với chiều rộng của khoảng trống ở đáy.

### Kết quả

Không cần thay đổi geometry của Level 1.

Các gap bắt buộc hiện tại có thể vượt qua bằng movement và jump thông thường, không yêu cầu damage boost hoặc một thao tác frame-perfect.

### Kiểm tra

- Các pit đầu level có thể vượt qua bằng normal jump.
- Khu vực staircase gần cuối level có thể vượt qua từ ledge này sang ledge kia.
- Không có gap bắt buộc vượt ngoài jump envelope hiện tại.
- Không cần thay đổi physics hoặc movement constants.
- Level 1 có thể hoàn thành bằng control scheme hiện tại.

---

## 11. Enemy Activation

### File liên quan

- [`include/entities/Enemy.h`](../../include/entities/Enemy.h)
- [`src/entities/Enemy.cpp`](../../src/entities/Enemy.cpp)
- [`src/level/Level.cpp`](../../src/level/Level.cpp)

### S6-TV4-21 — Chỉ activate enemy khi camera nhìn thấy

Trước thay đổi này, `Level::update()` gọi `update()` cho toàn bộ entity trên map ở mỗi frame.

Do đó Goomba và Koopa có thể bắt đầu patrol ngay từ lúc level được load, kể cả khi chúng nằm rất xa camera. Enemy trên platform ngắn có thể di chuyển hoặc rơi khỏi khu vực spawn trước khi Mario tới.

Enemy hiện có một activation state:

```cpp
bool m_activated = false;
```

Enemy chưa activated chỉ bắt đầu chạy AI khi bounding box của nó giao với vùng nhìn ngang hiện tại của camera.

Sau lần activation đầu tiên, trạng thái được giữ lại để tránh reset AI khi enemy tạm thời rời camera.

### Phân chia trách nhiệm

Task này chỉ xử lý first-time activation.

Việc quyết định enemy đã activated có tiếp tục update sau khi rời màn hình hay không được xử lý riêng trong S6-TV4-22.

Physics body không bị disable trước activation. Box2D vẫn có thể settle enemy xuống terrain, nhưng patrol AI chưa chạy nên enemy không tự đi khỏi spawn trước khi được nhìn thấy.

### Kiểm tra

- Enemy ngoài camera chưa chạy patrol AI.
- Enemy bắt đầu update khi đi vào camera.
- Enemy chỉ chuyển từ inactive sang activated một lần.
- Coin, item và QuestionBlock không bị activation gate.
- Level 2 enemy không tự di chuyển trước khi Mario tới.
- Build và toàn bộ CTest hiện tại vẫn pass.

---

## 11. Quy Ước Cập Nhật File Này

Sau mỗi task Sprint 6 hoàn tất, TV4 cần bổ sung:

1. Task ID và mục tiêu.
2. Những file đã thay đổi.
3. Class, method hoặc interface mới.
4. Phương pháp hoặc design technique được sử dụng.
5. Ví dụ cách gọi hoặc cách sử dụng.
6. Build, unit test và manual test đã thực hiện.

Không ghi task là hoàn tất nếu chưa build và test thành công.

# **Tổng Kết Những Thay Đổi Của TV4 — Sprint 6**

> **Tác giả:** TV4 (Vy) — Level, Enemy & SaveManager  
> **Phạm vi hiện tại:** S6-TV4-02 đến S6-TV4-07  
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

Finish trigger trước đây chỉ có kích thước 32×32 tại tile `F`.
Vì vậy, Mario phải chạm đúng phần đầu cột cờ mới có thể hoàn thành level.

Trigger hiện được tạo thành một hình chữ nhật bao phủ từ tile `F`
đến tile pole `|` thấp nhất nằm trong cùng một cột.

### Phương pháp được sử dụng

`TileMap::findTiles()` được dùng để tìm:

- Finish point `F`.
- Toàn bộ pole tile `|`.

Từ hàng chứa finish point và hàng pole thấp nhất, hệ thống tính chiều cao
của vùng kích hoạt bằng:

```cpp
(bottomRow - finishPosition.y + 1) * TILE_SIZE
```

Level validation đã bảo đảm mỗi level chỉ có đúng một finish point,
các pole tile liên tục và nằm trong cùng một cột. Vì vậy, vùng trigger
luôn tương ứng với đúng cột cờ của level.

### Cách hoạt động

Luồng tile của cột cờ có dạng:

```tex
F
|
|
|
```

Vùng trigger bao phủ toàn bộ các tile trên, nhưng không bao gồm solid tile
nằm bên dưới chân cột cờ.

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

mỗi frame. Khi bounding box của Mario giao với bất kỳ phần nào trong vùng
cột cờ, hệ thống đặt:

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

---

## 7. Quy Ước Cập Nhật File Này

Sau mỗi task Sprint 6 hoàn tất, TV4 cần bổ sung:

1. Task ID và mục tiêu.
2. Những file đã thay đổi.
3. Class, method hoặc interface mới.
4. Phương pháp hoặc design technique được sử dụng.
5. Ví dụ cách gọi hoặc cách sử dụng.
6. Build, unit test và manual test đã thực hiện.

Không ghi task là hoàn tất nếu chưa build và test thành công.

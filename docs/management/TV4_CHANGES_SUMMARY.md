

# **Tổng Kết Những Thay Đổi Của TV4 — Sprint 6**

> **Tác giả:** TV4 (Vy) — Level, Enemy & SaveManager  
> **Phạm vi hiện tại:** S6-TV4-02 đến S6-TV4-09, S6-TV4-11 đến S6-TV4-15
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

## 9. Level 1 Tutorial Flow Verification

### S6-TV4-13 — Hoàn thiện Level 1 tutorial flow

Level 1 được kiểm tra theo tutorial sequence của Sprint 6:

```text
Movement → Coin → Jump / QuestionBlock → Mushroom → Goomba → Flag
```

Level hiện có một coin gần khu vực spawn, QuestionBlock và Mushroom block trước enemy đầu tiên, sau đó mới giới thiệu Goomba và các obstacle tiếp theo.

Không cần thay đổi thêm layout vì thứ tự tutorial hiện tại đã đáp ứng mục tiêu của task.

### Kiểm tra

- Mario có khoảng trống để làm quen movement ngay sau spawn.
- Coin xuất hiện trước enemy đầu tiên.
- QuestionBlock xuất hiện trước combat.
- `U` cung cấp Super Mushroom trước Goomba đầu tiên.
- Level có thể hoàn thành từ spawn tới flag.
- Không cần damage boost để hoàn thành.
- Build và toàn bộ CTest hiện tại pass.

---

## 10. Level 1 Gap Validation

### File liên quan

- [`levels/level1.txt`](../../levels/level1.txt)

- ### S6-TV4-14 — Kiểm tra Level 1 gaps

  Level 1 được kiểm tra dựa trên các bề mặt Mario thực sự có thể đứng, không chỉ dựa trên khoảng trống ở hàng ground thấp nhất.

  Các gap bắt buộc trên completion route được thử trực tiếp với movement và jump hiện tại của Mario.

  Kết quả:

  - Không có jump bắt buộc vượt ngoài jump envelope hiện tại.
  - Không có frame-perfect jump.
  - Không cần damage boost để vượt gap.
  - Các staircase/platform phía trên được tính là một phần của traversal route.
  - Nếu một jump yêu cầu run, run phải được expose qua input và được giới thiệu
    trước obstacle đó.

  Không cần thay đổi geometry nếu toàn bộ manual checks trên pass.

---

## 11. Enemy Activation

### File liên quan

- [`include/entities/Enemy.h`](../../include/entities/Enemy.h)
- [`src/entities/Enemy.cpp`](../../src/entities/Enemy.cpp)
- [`src/level/Level.cpp`](../../src/level/Level.cpp)

### S6-TV4-21 — Activate enemy trong vùng 64 px phía trước viewport

Enemy activation được điều chỉnh để tránh việc toàn bộ enemy trên map chạy patrol ngay từ lúc level vừa load.

Enemy chưa được activate sẽ không chạy gameplay update cho đến khi bounding box của nó đi vào viewport hoặc nằm trong vùng 64 px phía trước cạnh phải của viewport.

Khi điều kiện được thỏa mãn:

```cpp
enemy->activate();
```

sẽ chuyển enemy sang trạng thái activated.

Activation là trạng thái một chiều:

```
inactive → activated
```

Sau khi đã activate, enemy tiếp tục được update bình thường và không bị reset hoặc freeze lại khi rời viewport.

Việc cleanup enemy đã đi quá xa phía sau camera được tách sang S6-TV4-22.

### Phân chia trách nhiệm

S6-TV4-21 chỉ xử lý thời điểm enemy bắt đầu hoạt động.

Không xử lý trong task này:

- cleanup enemy phía sau camera;
- pit cleanup;
- ledge detection;
- Koopa shell state;
- sprite hoặc animation asset.

### Kiểm tra

- Enemy xa phía trước camera chưa chạy patrol.
- Enemy bắt đầu hoạt động khi nằm trong vùng 64 px phía trước viewport.
- Enemy chỉ activate một lần.
- Enemy đã activate tiếp tục hoạt động khi rời viewport.
- Item và QuestionBlock không bị activation gate.
- Level 2 enemy không tự di chuyển từ đầu level trước khi Mario tới.
- Build và toàn bộ CTest hiện tại pass.

---

## 12. Level 2 Underground / Vertical Section

### S6-TV4-15 — Bổ sung pipe và vertical traversal cho Level 2

Level 2 được bổ sung các pipe tile để tạo thêm vertical variation và đáp ứng layout requirement của Sprint 6.

Ba cụm pipe được tạo bằng các symbol:

```text
[] = pipe top
{} = pipe body
```

Các pipe:

- có top/body ghép đúng cột;
- có solid terrain bên dưới;
- tham gia TileMap collision như terrain;
- tạo thêm các bề mặt đứng/nhảy mà không thay đổi completion route chính.

### Kiểm tra

- Pipe render đúng từ consolidated tileset.
- Mario đứng và nhảy trên pipe được.
- Mario không xuyên qua pipe.
- Enemy không xuyên qua pipe.
- Các khoảng giữa pipe vẫn vượt qua được bình thường.
- Level 2 vẫn load và hoàn thành được.
- Build và toàn bộ CTest hiện tại pass.

---

## 13. Level 2 Spawn Validation

### S6-TV4-16 — Kiểm tra spawn fairness của Level 2

Các spawn Koopa trong Level 2 được kiểm tra để bảo đảm enemy có terrain
hợp lệ dưới vị trí spawn và không tạo unavoidable damage khi Mario tiếp cận.

Layout hiện tại không cần thay đổi thêm nếu các Koopa vẫn đứng đúng platform
khi bắt đầu xuất hiện trong viewport.

Tuy nhiên, yêu cầu “Koopa không rơi hoặc attack trước khi thấy” còn phụ thuộc
vào enemy activation policy của S6-TV4-21.

### Kiểm tra

- Koopa không spawn giữa không trung.
- Koopa không spawn bên trong pipe hoặc solid terrain.
- Spawn không nằm ngay tại blind landing bắt buộc.
- Không có unavoidable damage ngay khi Mario tiến vào khu vực.
- Enemy activation trước khi Mario tiếp cận hiện được kiểm soát bởi S6-TV4-21 với vùng activation 64 px phía trước viewport.

---

## 14. Off-screen Enemy Cleanup

### S6-TV4-22 — Cleanup enemy quá xa phía sau viewport

Enemy off-screen policy được điều chỉnh để tránh hai vấn đề:

1. Enemy chưa được nhìn thấy chạy AI trên toàn bộ map.
2. Enemy đã đi rất xa phía sau camera vẫn tồn tại vô thời hạn.

S6-TV4-21 chịu trách nhiệm first-time activation trong vùng 64 px phía trước viewport.

Sau khi được activate, enemy tiếp tục gameplay update bình thường và không bị freeze hoặc reset khi tạm rời viewport.

Enemy chỉ được đánh dấu removal khi toàn bộ bounding box đã nằm xa hơn một chiều rộng viewport phía sau cạnh trái camera.

```text
inactive
   ↓
64 px activation range
   ↓
activated
   ↓
continue updating
   ↓
more than one viewport behind camera
   ↓
markForRemoval()
```

### Phân chia trách nhiệm

Task này chỉ cleanup enemy phía sau camera.

Không xử lý:

- item/projectile cleanup;
- entity dưới level bounds;
- pit cleanup;
- Koopa ledge hoặc shell behavior.

Các trường hợp generic entity cleanup được xử lý riêng trong S6-TV4-31.

### Kiểm tra

- Enemy chưa activate không chạy AI.
- Enemy activate đúng theo S6-TV4-21.
- Enemy đã activate không bị reset khi rời viewport.
- Enemy chưa quá một viewport phía sau vẫn tồn tại.
- Enemy quá một viewport phía sau được cleanup.
- Item và QuestionBlock không bị ảnh hưởng.
- Build và toàn bộ CTest pass.

---

## 15. Quy Ước Cập Nhật File Này

Sau mỗi task Sprint 6 hoàn tất, TV4 cần bổ sung:

1. Task ID và mục tiêu.
2. Những file đã thay đổi.
3. Class, method hoặc interface mới.
4. Phương pháp hoặc design technique được sử dụng.
5. Ví dụ cách gọi hoặc cách sử dụng.
6. Build, unit test và manual test đã thực hiện.

Không ghi task là hoàn tất nếu chưa build và test thành công.



# **Tổng Kết Những Thay Đổi Của TV4 — Sprint 6**

> **Tác giả:** TV4 (Vy) — Level, Enemy & SaveManager  
> **Phạm vi hiện tại:** S6-TV4-01 đến S6-TV4-27, S6-TV4-29, S6-TV4-31 đến S6-TV4-39
> **Trạng thái:** Build thành công, toàn bộ CTest hiện tại đã pass  
> **Cập nhật gần nhất:** Sau khi chuẩn hóa release levels và sửa enemy patrol/collision regressions

---

## 1. Tổng Quan

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
cmake --build build --parallel 1
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

| Tile  | QuestionBlock content |
| ----  | --------------------- |
| `?`   | Super Mushroom        |
| `U/u` | 1-Up Mushroom         |
| `f/h` | Fire Flower           |
| `O/o` | Starman / Star        |

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

## 8. Consolidated TileMap Tileset

### File liên quan

- [`src/level/TileMap.cpp`](../../src/level/TileMap.cpp)
- [`levels/level1.txt`](../../levels/level1.txt)
- [`levels/level2.txt`](../../levels/level2.txt)
- [`assets/textures/tiles/tileset.png`](../../assets/textures/tiles/tileset.png)

### S6-TV4-09 — Chuyển TileMap sang consolidated tileset

`TileMap` sử dụng:

```text
assets/textures/tiles/tileset.png
```

làm source terrain chung.

Source frame có kích thước 16×16 và được render ở world tile size 32×32.

Các terrain symbol có frame riêng gồm:

```text
1       Ground
B       Brick
E       Used block
[ ]     Pipe top
{ }     Pipe body
F       Finish top
|       Finish pole
```

QuestionBlock vẫn được quản lý bởi `QuestionBlock` entity để tránh duplicate rendering hoặc item spawning.
Các pipe symbol `[ ] { }` tiếp tục thuộc grammar hợp lệ và được sử dụng trong Level 2.

### Phương pháp được sử dụng

Helper `makeTilesetRect()` chuyển tile index thành `sf::IntRect` tương ứng trong spritesheet.

`TileMap` không còn phụ thuộc vào `SpriteFrames::Blocks` để render terrain.

Question block (`?`, `U`, `O`) tiếp tục được quản lý và render bởi `QuestionBlock` entity, tránh việc cùng một block bị render hai lần.

### Kết quả

`TileMap` sử dụng consolidated terrain atlas làm source chung cho terrain rendering.

Source rectangles được tách khỏi world tile size: atlas sử dụng frame 16×16 trong khi gameplay world vẫn dùng tile 32×32.

QuestionBlock tiếp tục được render và quản lý bởi entity riêng, còn pipe, ground, brick, used block và finish pole được render từ `TileMap`.

Level grammar và rendering hiện sử dụng cùng một symbol contract.

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
- `?` cung cấp Super Mushroom trong phần tutorial.
- Level cũng có Fire Flower, 1-Up và Star ở các khu vực sau để giới thiệu item mechanics có chủ đích.
- Level có thể hoàn thành từ spawn tới flag.
- Không cần damage boost để hoàn thành.
- Build và toàn bộ CTest hiện tại pass.

---

## 10. Level 1 Gap Validation

### File liên quan

- [`levels/level1.txt`](../../levels/level1.txt)

### S6-TV4-14 — Kiểm tra Level 1 gaps

Level 1 được kiểm tra theo các gap bắt buộc trên completion route và movement/jump hiện tại của Mario.

Hai pit gap bắt buộc đều rộng:

```text
3 tiles = 96 px
```

Khoảng cách này nằm trong walking-jump envelope hiện tại và không yêu cầu Run hoặc damage boost.

### Manual verification

Hai gap được test trực tiếp trong gameplay với:

```text
- Không giữ Shift / không Run.
- Walking + Space để jump.
- Không cần run-up dài.
- Không dùng damage boost.
```

Kết quả:

```text
Gap 1: PASS
Gap 2: PASS
```

Không phát hiện frame-perfect jump, blind landing bắt buộc hoặc obstacle vượt ngoài jump envelope.

### Kết luận

Level 1 completion route đáp ứng fairness requirement của S6-TV4-14.

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

Level 2 cũng được bổ sung một Fire Flower block `f` trong khu pipe giữa level.

Vị trí này được chọn sau phần mở đầu và trước các encounter tiếp theo để
Fire Flower đóng vai trò mid-level reward thay vì xuất hiện quá sớm hoặc
quá sát finish.

Level 2 hiện có:

```text
pipe traversal
Goomba
Koopa
shell gameplay
Fire Flower
finish route
```

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

Các spawn Koopa trong Level 2 được kiểm tra trực tiếp trong gameplay sau khi hoàn tất các enemy patrol/collision regression fixes.

Static validatiion xác nhận các Koopa đều có enemy-support surface hợp lệ tại vị trí spawn và không nằm bên trong pipe hoặc solid terrain.

Enemy activation tiếp tục sử dụng vùng:

```text
viewport + 64 px
```

Enemy chưa vào activation range không chạy patrol. Sau khi activate, Koopa giữ state và patrol bình thường trên platform.

### Runtime verification

Các encounter trong Level 2 được chơi trực tiếp từ đầu đến cuối.

Kết quả:

```text
- Cặp Koopa đầu level: PASS
- Các Koopa còn lại: PASS
- Activation timing: PASS
- Platform / ledge behavior: PASS
- Enemy-enemy pass-through: PASS
- No blind / unavoidable attack: PASS
```

Trong các encounter có nhiều enemy, Walking Koopa có thể lướt qua enemy khác mà không push, kẹt hoặc reverse direction.

Koopa cũng đi qua các walkable support seam như:

```text
B -> ? -> B
```

mà không quay đầu sai, trong khi wall và ledge thật vẫn làm Koopa reverse.

Không phát hiện Koopa rơi khỏi platform ngay sau activation hoặc gây unavoidable damage trước khi người chơi có cơ hội quan sát và phản ứng.

### Kết luận

Level 2 đáp ứng spawn fairness requirement của S6-TV4-16.

---

## 14. Release Level Data Normalization

### File liên quan

- [`levels/level1.txt`](../../levels/level1.txt)
- [`levels/level2.txt`](../../levels/level2.txt)
- [`levels/level3.txt`](../../levels/level3.txt)

### S6-TV4-13/15/17/19 — Chuẩn hóa ba release level

Ba release level được rà soát lại để sử dụng cùng level grammar và có
row width nhất quán.

Kích thước hiện tại:

```text
Level 1: 183 × 12
Level 2: 250 × 12
Level 3: 281 × 12
```

Mỗi level có đúng:

```text
1 Mario spawn M
1 finish F
continuous finish pole |
```

Các level sử dụng chung item symbol contract:

```text
?     Super Mushroom
U/u   1-Up Mushroom
f/h   Fire Flower
O/o   Starman / Star
```

### Level 1

Level 1 giữ vai trò tutorial và giới thiệu movement, coin, QuestionBlock, Mushroom, Goomba và finish route trước khi tăng dần mật độ enemy và item.


### Level 2

Level 2 tập trung vào underground-style traversal với pipe, Goomba, Koopa và shell gameplay.

Một Fire Flower block được đặt trong khu pipe giữa level để đáp ứng power-up progression của level và cho người chơi đủ thời gian sử dụng Fire Mario trước finish.

### Level 3

Level 3 được sửa lại thành map hợp lệ 281×12 thay vì các row có chiều dài không nhất quán.

Level giữ layout challenge hiện tại và có:

```text
mixed Goomba / Koopa encounters
Star block
platform / stone gauntlet
final finish-pole section
```

Star được đặt trước phần challenge cuối để item xuất hiện có mục đích thay vì chỉ tồn tại để thỏa symbol requirement.

---

## 15. Level 3 Fairness Verification

### S6-TV4-18 — Kiểm tra fairness của Level 3

Level 3 được chơi trực tiếp từ spawn đến finish sau khi hoàn tất các
enemy patrol, activation và collision regression fixes.

Level được kiểm tra với mục tiêu bảo đảm final challenge khó hơn các level
trước nhưng vẫn có completion route hợp lệ và không yêu cầu exploit hoặc
damage boost.

### Runtime verification

Kết quả playthrough:

```text
Level completion: PASS
Required jumps: PASS
Enemy visibility: PASS
Mixed Goomba / Koopa encounters: PASS
Star placement: PASS
Final gauntlet: PASS
No damage boost required: PASS
```

Các jump bắt buộc có đủ khoảng tiếp cận và landing space, không phát hiện frame-perfect jump bắt buộc.

Enemy được activate theo viewport policy và xuất hiện đủ sớm để người chơi có thời gian quan sát và phản ứng.

Các encounter kết hợp Goomba và Koopa không tạo unavoidable damage. Enemy thông thường có thể lướt qua nhau thay vì tạo blocking hoặc unexpected direction reversal.

Star xuất hiện trước phần challenge cuối và có thể được sử dụng như một power-up có chủ đích trong completion route.

Final gauntlet có ít nhất một route hợp lệ tới finish pole mà không cần:

```text
damage boost
collision exploit
frame-perfect input
```

### Kết luận
Level 3 đáp ứng fairness requirement của S6-TV4-18.

---

## 15. Off-screen Enemy Update Policy

### File liên quan

- [`src/level/Level.cpp`](../../src/level/Level.cpp)
- [`include/entities/Enemy.h`](../../include/entities/Enemy.h)

### S6-TV4-22 — Giữ trạng thái enemy sau khi đã activate

Enemy chưa được nhìn thấy vẫn bị giữ inactive cho đến khi đi vào vùng
activation của viewport.

Sau khi đã activate, enemy không bị reset hoặc xóa chỉ vì camera đã đi
qua nó.

Flow hiện tại:

```text
enemy chưa tới viewport
    ↓
inactive
    ↓
viewport + activation margin
    ↓
activated
    ↓
giữ state trong suốt lifetime
```

Camera position không còn là điều kiện xóa một enemy còn sống.

Entity chỉ được cleanup bởi lifecycle thật sự, ví dụ:

```text
dead / marked for removal
out of level bounds + cleanup margin
```

Điều này giữ trạng thái enemy nhất quán nếu gameplay hoặc camera quay lại khu vực cũ, đồng thời enemy phía trước chưa activate vẫn không chạy AI trên toàn map.

### Kiểm tra

- Enemy xa phía trước chưa update.
- Enemy activate khi Mario tiếp cận.
- Enemy đã activate không bị reset.
- Camera đi xa không tự xóa enemy còn sống.
- Enemy ra khỏi level bounds vẫn được generic cleanup.

---

## 16. Koopa Ledge Detection

### File liên quan

- [`include/entities/Koopa.h`](../../include/entities/Koopa.h)
- [`src/entities/Koopa.cpp`](../../src/entities/Koopa.cpp)
- [`include/entities/Enemy.h`](../../include/entities/Enemy.h)
- [`src/level/Level.cpp`](../../src/level/Level.cpp)
- [`src/level/TileMap.cpp`](../../src/level/TileMap.cpp)

### S6-TV4-24 — Ngăn Walking Koopa rơi khỏi platform

Koopa walking hiện sử dụng `TileMap` để kiểm tra terrain ngay phía trước hướng patrol.

`Level` truyền `TileMap` cho enemy thông qua virtual `Enemy::setTileMap()`. `Koopa` override method này và giữ reference tới TileMap để thực hiện ledge detection.

Trong `isApproachingLedge()`, Koopa kiểm tra:

- enemy-support surface bên dưới vị trí hiện tại;
- enemy-support surface ngay dưới trước hướng di chuyển.

Enemy-support semantics bao gồm terrain solid và bề mặt QuestionBlock, nhưng không làm QuestionBlock trở thành terrain collider trùng.

Nếu Koopa vẫn đang đứng trên solid terrain nhưng phía trước không còn solid tile, Koopa đảo hướng trước khi rời platform.

Ledge detection chỉ áp dụng cho trạng thái `WALKING`.

`SHELL_IDLE` không di chuyển và `SHELL_SLIDING` không sử dụng ledge detection, vì vậy shell vẫn có thể lao xuống pit theo gameplay bình thường.

`reverseDirection()` trực tiếp cập nhật facing direction và horizontal velocity thay vì gọi lại `patrol()`, tránh recursive ledge checks.

### Runtime regression fixes

Runtime verification phát hiện Koopa có thể quay đầu sai tại seam giữa
terrain tile và QuestionBlock, đặc biệt ở các layout dạng:

```text
BBB?BBB
```

Ledge detection hiện phân biệt:

```text
terrain physics solidity
enemy walkable support
```

QuestionBlock và các power-up block được xem là support surface cho enemy mà không bị biến thành terrain collider trùng.

Collision handling cũng bỏ side-contact giả tại walkable support seam,nhưng vẫn giữ wall collision đối với obstacle thật.

Koopa walking sprite sử dụng asset mặc định quay trái. Khi facing direction đổi sang phải, sprite được mirror theo trục X để visual direction luôn khớp với movement direction.

### Kiểm tra bổ sung

- Koopa đi qua `B → ? → B` mà không reverse sai.
- Koopa vẫn reverse ở wall thật.
- Koopa không bị đứng dính vào wall.
- Facing LEFT hiển thị asset gốc.
- Facing RIGHT hiển thị sprite mirror.

### Enemy Collision Regression Fixes

Các enemy thông thường không còn dùng nhau như wall vật lý.

Trong enemy-enemy contact, physical response được disable để Goomba và Walking Koopa có thể lướt qua nhau mà không push, kẹt hoặc reverse direction.

Gameplay contact của Koopa shell vẫn được xử lý trước physical response:

```text
normal enemy <-> normal enemy
    -> pass through

sliding shell <-> enemy
    -> shell attack logic
    -> victim defeat
    -> shell tiếp tục di chuyển
```

Wall collision được xử lý idempotent để nhiều Box2D contacts trên cùng một wall không làm enemy reverse nhiều lần rồi đứng kẹt.

Goomba squish rendering cũng được bottom-align lại theo kích thước hiện tại của entity, tránh sprite bị chìm vài pixel xuống terrain sau khi stomp.

### Kiểm tra

- Goomba và Koopa thường lướt qua nhau.
- Enemy không đứng yên sau khi chạm wall.
- Sliding shell vẫn attack enemy.
- Goomba squish giữ đúng vị trí chân.
- Project build và chạy gameplay thành công.
---

## 17. Koopa Shell Fixture

## File liên quan
- [`include/entities/Koopa.h`](../../include/entities/Koopa.h)
- [`src/entities/Koopa.cpp`](../../src/entities/Koopa.cpp)
- [`assets/textures/enemies/enemies.png`](../../assets/textures/enemies/enemies.png)

### S6-TV4-25 - Resize Koopa shell fixture

Koopa đã được chuyển sang sử dụng sprite atlas chung `assets/texture/enemies/enemies.png` thay cho texture Koopa riêng

Walking animation sử dụng manual frame coordinates từ `enemies_coordinate.md`, do các frame trong atlas không nằm trên một grid liên tục.

Koopa walking sử dụng runtime size khoảng 32×48, tương ứng với source sprite khoảng 16×24 được scale 2 lần.

Khi Koopa chuyển từ `WALKING` sang `SHELL_IDLE`, physics fixture được thu nhỏ từ 32×48 xuống 32×28 để phù hợp với shell source sprite 16×14.

Fixture rebuild được defer khỏi collision callback bằng `m_pendingShellFixtureRebuild`, sau đó thực hiện trong `Koopa::update()` khi Box2D world không còn locked.

Shell fixture được offset xuống 10 px:

```text
(48 - 28) / 2 = 10 px
```

nhằm giữ đáy fixture tại cùng vị trí chân với Walking Koopa.

Sprite rendering sử dụng scale cố định 2× và bottom alignment thông qua `syncSpriteToFeet()`, vì vậy các walking frame có chiều cao khác nhau và shell frame thấp hơn vẫn giữ chung vị trí chân.

### Kiểm tra

- Koopa walking hiển thị đúng frame từ `enemies.png`.
- Walking sprite không bị stretch sai tỉ lệ.
- Stomp đầu chuyển Koopa sang `SHELL_IDLE`.
- Shell hiển thị đúng kích thước khoảng 32×28.
- Shell fixture được resize thành 32×28.
- Chân Koopa không nhảy lên hoặc chìm xuống khi chuyển sang shell.
- Fixture rebuild không thực hiện khi Box2D world đang locked.
- Shell idle đứng yên.
- Shell sliding tiếp tục dùng shell fixture đã resize.
- Build và toàn bộ CTest hiện tại pass.

---

## 18. Koopa State Transitions

### File liên quan

- [`include/entities/Koopa.h`](../../include/entities/Koopa.h)
- [`src/entities/Koopa.cpp`](../../src/entities/Koopa.cpp)
- [`src/physics/CollisionManager.cpp`](../../src/physics/CollisionManager.cpp)

### S6-TV4-26 — Walking → Shell Idle

Koopa stomp handling được chuẩn hóa để lần stomp đầu tiên chỉ thực hiện một state transition:

```text
WALKING -> SHELL_IDLE
```

`CollisionManager` snapshot `KoopaState` trước khi xử lý contact.

Nếu state ban đầu là `WALKING`, hệ thống chỉ gọi `Koopa::onStomp()`. State mới `SHELL_IDLE` không được kiểm tra lại để kick shell trong cùng contact.

Các trường hợp được tách rõ:

```text
WALKING + stomp -> SHELL_IDLE
SHELL_IDLE + stomp -> SHELL_IDLE
SHELL_SLIDING + stomp -> SHELL_IDLE
SHELL_IDLE + lateral contact -> SHELL_SLIDING
```

Nhờ đó một collision không thể vừa biến Walking Koopa thành shell vừa làm shell chạy ngay.

### S6-TV4-27 - Shell Idle -> Shell Sliding

Koopa shell được kick khi Mario thực hiện lateral contact với Koopa đang ở trạng thái:

```text
SHELL_IDLE
```

Hướng kick được xác định từ vị trí tương đối giữa Mario và Koopa:

```text
Mario ở bên trái shell -> shell chạy sang phải
Mario ở bên phải shell -> shell chạy sang trái
```

`Koopa::kick()` chỉ chấp nhận transition:

```text
SHELL_IDLE -> SHELL_SLIDING
```
và đặt horizontal velocity theo `KOOPA_SLIDE_SPEED`.

Shell đang ở trạng thái `SHELL_SLIDING` không bị kick lại khi Mario va chạm ngang. Trường hợp này được xử lý như enemy damage đối với Mario.

Nếu Mario stomp một shell đang sliding:

```text
SHELL_SLIDING -> SHELL_IDLE
```
Shell có thể được kick lại bằng một lateral contact sau đó.

### Phân chia trách nhiệm
S6-TV4-26 xử lý transition khi Mario stomp Koopa.
S6-TV4-27 xử lý transition khi Mario chạm ngang shell idle.
Shared collision dispatch architecture không được thay đổi trong hai task này.

### Kiểm tra
- Stomp Walking Koopa một lần chuyển sang `SHELL_IDLE`.
- Shell không chạy ngay sau stomp đầu.
- Horizontal velocity của shell idle bằng 0.
- Stomp lại shell idle không làm shell chạy.
- Mario chạm ngang shell idle từ bên trái → shell chạy sang phải.
- Mario chạm ngang shell idle từ bên phải → shell chạy sang trái.
- Shell sliding không bị kick lại.
- Lateral contact với shell sliding gây damage cho Mario.
- Stomp shell sliding làm shell dừng lại.
- Shell có thể được kick lại sau khi trở về `SHELL_IDLE`.
- Build và toàn bộ CTest hiện tại pass.

---

## 19. Koopa Shell Enemy Defeat

### File liên quan

- [`src/physics/CollisionManager.cpp`](../../src/physics/CollisionManager.cpp)
- [`include/entities/Koopa.h`](../../include/entities/Koopa.h)
- [`src/entities/Koopa.cpp`](../../src/entities/Koopa.cpp)
- [`include/entities/Enemy.h`](../../include/entities/Enemy.h)

### S6-TV4-29 - Shell defeat duplicate guard

Sliding Koopa shell có thể defeat enemy khác khi xảy ra Enemy-Enemy collision.

Collision handler chỉ cho phép shell attack khi Koopa đang ở trạng thái `SHELL_SLIDING`, shell idle không gây damage.

Trước khi xử lý defeat, handler kiểm tra victim đã chết, đã được đánh dấu remove, pending destroy hoặc inactive hay chưa.

Nếu victim đã được xử lý bởi một contact trước đó, collision được xem là đã handled nhưng không gọi takeDamage() hoặc defeat logic lần nữa.

Điều này tránh duplicate defeat khi Box2D phát nhiều contact callback trước khi entity được erase khỏi Level.

### Phân chia trách nhiệm

Task này chỉ bảo đảm shell defeat được xử lý một lần trên mỗi victim.

Score và DefeatCause được để cho collision/score pipeline chung của TV3 và TV5, tránh tạo nguồn score thứ hai trong TV4.


### Trạng thái

Duplicate shell defeat đã được guard ở TV4 collision logic.

Task chưa được claim DONE theo fix plan vòng 2 vì shell defeat cần được kết nối vào shared defeat pipeline với

```text
DefeatCayse::SHELL
score +200
shared defeat event / SFX path
```

Phần integration này phụ thuộc contract chung của TV3/TV5.

### Kiểm tra
- Shell idle không defeat enemy.
- Shell sliding defeat enemy khi va chạm.
- Một victim không bị defeat nhiều lần.
- Shell không reverse như enemy thường sau khi đã xử lý shell collision.
- Shell có thể tiếp tục defeat enemy khác.
- Build và toàn bộ CTest pass.

---

## 20. Generic Entity Bounds Cleanup

### File liên quan

- [`src/level/Level.cpp`](../../src/level/Level.cpp)
- [`include/entities/Entity.h`](../../include/entities/Entity.h)
- [`src/entities/Entity.cpp`](../../src/entities/Entity.cpp)

### S6-TV4-31 — Cleanup entity ngoài level với margin 64 px

`Level` hiện áp dụng một cleanup policy chung cho các entity nằm ngoài level bounds.

Level bounds được tính từ kích thước `TileMap`:

```text
levelWidth  = mapWidth  × TILE_SIZE
levelHeight = mapHeight × TILE_SIZE
```

Một entity chỉ được đánh dấu removal khi toàn bộ bounding box của nó đã đi ra ngoài level quá margin 64 px ở một trong bốn phía.

```text
level bounds + 64 px cleanup margin
    ↓
entity hoàn toàn vượt qua
    ↓
markForRemoval()
```

Cleanup sử dụng lifecycle mechanism có sẵn của `Entity`. Không erase entity trực tiếp trong update loop.

`Level::removeDeadEntities()` chịu trách nhiệm xóa entity sau khi entity được đánh dấu removal.

### Phân chia trách nhiệm

S6-TV4-31 xử lý generic out-of-level cleanup cho entity như enemy, item và projectile.

Các lifecycle rule riêng như FireBall lifetime và bounce limit vẫn được giữ nguyên.

### Kiểm tra

- Entity trong level không bị cleanup.
- Entity chỉ ra ngoài một phần vẫn được giữ lại.
- Entity hoàn toàn vượt level bounds hơn 64 px được cleanup.
- Mushroom và Star rơi khỏi level được cleanup.
- Projectile bay khỏi level được cleanup.
- Enemy rơi khỏi level được cleanup.
- QuestionBlock và item còn nằm trong level không bị ảnh hưởng.
- Build và toàn bộ CTest pass.

---

## 21. SaveManager Version 1 Foundation

### File liên quan

- [`include/core/SaveManager.h`](../../include/core/SaveManager.h)
- [`src/core/SaveManager.cpp`](../../src/core/SaveManager.cpp)

### S6-TV4-32 - Tạo SaveManager version 1

`SaveManager` được tạo làm nơi quản lý dữ liệu persistent của game.

`SaveData` sử dụng version 1 và cung cấp các giá trị mặc định an toàn:

```text
version = 1
highScore = 0
highestUnlockedLevel = 1
soundVolume = 80
musicVolume = 70
```

Save path được truyền vào thông qua constructor và mặc định là `saves/save.txt`

Việc cho phép inject save path giúp các test của `SaveManager` sau này có thể sử dụng file tạm riêng mà không ghi đè lên save file thật của game

### Phân chia trách nhiệm

Task này chỉ tạo cấu trúc `SaveData`, `SaveManager` version 1 và default data.

Chưa xử lý:

- đọc dữ liệu từ save file;
- ghi dữ liệu xuống save file;
- cập nhật high score;
- lưu sound/music volume;
- atomic file replacement;
- fallback khi save file bị corrupt hoặc sai version;

Các chức năng trên được triển khai trong S6-TV4-39.

### Kiểm tra

- Save data version mặc định bằng 1.
- High score mặc định bằng 0.
- Highest unclocked level mặc định bằng 1.
- Sound volume mặc định bằng 80.
- Music volume mặc định bằng 70.
- Save path có thể được inject để phục vụ test.
- Project build thành công.
- Toàn bộ CTest hiện tại pass.

---

## 22. Load High Score

### File liên quan

- [`include/core/SaveManager.h`](../../include/core/SaveManager.h)
- [`src/core/SaveManager.cpp`](../../src/core/SaveManager.cpp)

### S6-TV4-33 - Load high score từ save file

`SaveManager` được bổ sung `load()` để đọc persistent data từ save path đã được cấu hình.

Save file version 1 sử dụng định dạng key-value:

``` test
version 1
highScore 12500
highestUnlockedLevel 1
soundVolume 80
musicVolume 70
```

Dữ liệu không được đọc trực tiếp vào `m_data`.

Thay và đó, `load()` đọc vào một `SaveData` tạm thời và chỉ cập nhật state của `SaveManager` sau khi quá trình đọc hoàn tất.

Cách này tránh để `SaveManager` giữ dữ liệu chỉ được load một phần khi file không hợp lệ.

Nếu save file không tồn tại, `SaveManager` quay về default data và không làm game crash.

Version của file cũng được kiểm tra với `SAVE_DATA_VERSION`.

### Phân chia trách nhiệm

Task này tập trung vào việc đọc persistent save data, đặc biệt là `highScore`.

Chưa xử lý:
- ghi save file;
- high score monotonic update;
- level unlock update;
- audio setting update;
- atomic file replacement;
- đầy đủ corrupted-save recovery tests.

Các phần trên được xử lý trong S6-TV4-34 đến S6-TV4-39.

### Kiểm tra

- Save file hợp lệ được mở thành công.
- High score được đọc đúng từ file.
- High score vẫn đọc đúng sau khi khởi tạo lại `SaveManager`.
- Missing save file sử dụng default data.
- Save version hợp lệ được chấp nhận.
- State chỉ được cập nhật sau khi quá trình đọc hoàn tất.
- Project build thành công.
- Toàn bộ CTest hiện tại pass.

---

## 23. Monotonic High Score

### File liên quan

- [`include/core/SaveManager.h`](../../include/core/SaveManager.h)
- [`src/core/SaveManager.cpp`](../../src/core/SaveManager.cpp)

### S6-TV4-34 - Chỉ cập nhật high score khi score mới cao hơn

`SaveManager` được bổ sung `save()` để ghi `SaveData` hiện tại xuống save file version 1.

High score được cập nhật thông qua:

```cpp
updateHighScore(score);
```

Score mới chỉ thay thế high score hiện tại khi:
```text
newScore > highScore
```

Nếu score mới nhỏ hơn hoặc bằng high score hiện tại, dữ liệu được giữ nguyên và save file không bị overwrite bởi giá trị thấp hơn.

Khi score mới cao hơn, `SaveManager` cập nhật high score và ghi dữ liệu xuống file.

Nếu quá trình ghi file thất bại, high score trong memory được rollback về giá trị trước đó.

### Phân chia trách nhiệm

Task này đảm bảo high score tăng đơn điệu và có thể persist xuống file.

Việc ghi file hiện vẫn sử dụng direct write.

Atomic temporary-file replacement và bảo vệ save file cũ khi quá trình ghi thất bại được xử lý riêng trong S6-TV4-37.

### Kiểm tra

- Score cao hơn cập nhật high score.
- Score thất hơn không bị overwrite high score.
- Score bằng high score hiện tại không overwrite dữ liệu.
- High score mới được đọc lại đúng sau restart.
- Save failure không giữ high score mới trong memory.
- Project build thành công.
- Toàn bộ CTest hiện tại pass.

---

## 24. Highest Unlocked level Persistence

### File liên quan

- [`include/core/SaveManager.h`](../../include/core/SaveManager.h)
- [`src/core/SaveManager.cpp`](../../src/core/SaveManager.cpp)

### S6-TV4-35 - lưu highest unlocked level

`SaveManager` được bổ sung API:

```cpp
updateHighestUnlockedLevel(level);
```

để lưu level cao nhất mà người chơi đã mở khóa.

Giá trị `highestUnlockedLevel` chỉ được cập nhật khi level mới lớn hơn giá trị hiện tại.

```text
newLevel > highestUnlockedlevel -> update và save
newLevel <= highestUnlockedlevel -> giữ nguyên
```

Cách xử lý monotonic này bảo đảm tiến độ đã mở khóa không bị giảm khi người chơi chơi lại một level cũ.

Nếu quá trình ghi save thất bại, giá trị trong memory được rollback về `highestUnlockedlevel` trước đó.

Sau khi load lại ứng dụng, level đã mở khóa được đọc lại từ save file thông qua `SaveManager::load()`.

### Phân chia trách nhiệm

`SaveManager` chỉ chịu trách nhiệm persist level cao nhất đã mở khóa.

Việc xác định level kế tiếp sau khi hoàn thành một màn và chuyển Level 3 sang Win thuộc state/progression flow và được tích hợp cùng TV1.

### Kiểm tra

- Default highest unlocked level bằng 1.
- Unlock level 2 cập nhật giá trị thành 2.
- Chơi lại Level 1 không làm giá trị giảm/
- Unlock Level 3 cập nhật giá trị thành 3.
- Giá trị vẫn đúng sau restart và `load()`.
- Save failure rollback giá trị trong memory.
- Project build thành công.
- Toàn bộ CTest hiện tại pass.

---

## 25. Audio Settings Persistence

### File liên quan

- [`include/core/SaveManager.h`](../../include/core/SaveManager.h)
- [`src/core/SaveManager.cpp`](../../src/core/SaveManager.cpp)
- [`include/core/SoundManager.h`](../../include/core/SoundManager.h)
- [`src/core/SoundManager.cpp`](../../src/core/SoundManager.cpp)

### S6-TV4-36 - Lưu SFX và music volume

`SaveManager` được bổ sung:

```cpp
updateAudioSettings(soundVolume, musicVolume);
```

để persist hai thiết lập âm thanh trong `SaveData`.

Các giá trị volume được giới hạn trong khoảng:

```text
0 <= volume <= 100
```

trước khi ghi xuống save file.

Nếu giá trị không hữu hạn hoặc nằm ngoài khoảng hợp lệ, dữ liệu được đưa về phạm vi an toàn trước khi lưu.

Khi quá trình ghi file thất bại, cả `soundVolume` và `musicVolume` trong memory được rollback về giá trị trước đó.

Sau khi restart, `SaveManager::load()` khôi phục hai giá trị từ save file.

`SaveManager` không trực tiếp điều khiển audio runtime. Integration layer sử dụng dữ liệu đã load để gọi:

```cpp
SoundManager::setSoundVolume(...)
SoundManager::setMusicVolume(...)
```

Cách tách này giữ `SaveManager` chịu trách nhiệm persistence và `SoundManager` chịu trách nhiệm audio playback.

### Kiểm tra

- Sound volume được ghi đúng xuống save file.
- Music volume được ghi đúng xuống save file.
- Hai giá trị được đọc lại đúng sau restart.
- Volume nhỏ hơn 0 được clamp về 0.
- Volume lớn hơn 100 được clamp về 100.
- Save failure rollback cả hai giá trị trong memory.
- Existing high score và unlocked-level data không bị mất khi lưu audio.
- Project build thành công.
- Toàn bộ CTest hiện tại pass.

---

## 26. Atomic Save File Replacement

### File liên quan

- [`include/core/SaveManager.h`](../../include/core/SaveManager.h)
- [`src/core/SaveManager.cpp`](../../src/core/SaveManager.cpp)

### S6-TV4-37 - Ghi save bằng temporary file và safe replace

`SaveManager::save()` không còn ghi trực tiếp vào production save file bằng `std::ios::trunc`.

Thay vào đó, dữ liệu được ghi trước vào:

```text
save.txt.tmp
```

Luồng save mới:

```text
SaveData
    ↓
write temporary file
    ↓
flush và kiểm tra write state
    ↓
close temporary file
    ↓
replace production save
```

Production `save.txt` chỉ được thay thế sau khi temporary file đã được ghi hoàn chỉnh.

Nếu quá trình ghi temporary file hoặc replace thất bại:

- `save()` trả về `false`;
- save file cũ không bị ghi trực tiếp;
- temporary file được cleanup;
- các update method rollback state trong memory về giá trị trước đó.

Temporary file được đặt cùng directory với production save để việc replace diễn ra trên cùng filesystem.

`writeSaveFile()` cũng tạo parent directory khi cần thiết, giúp injected test path hoạt động mà không phụ thuộc production `saves/` directory.

### Phân chia trách nhiệm

Task này tập trung vào safe file writing và replacement.

Validation đầy dủ cho missing, corrupt và wrong-version save được xử lý trong S6-TV4-38 và automated test matrix được bổ sung ở S6-TV4-39.

### Kiểm tra

- Save data được ghi vào `.tmp` trước.
- Production save chỉ được replace sau khi temporary write thành công.
- Save thành công không để lại `.tmp`.
- Save thất bại trả về `false`.
- Existing save không bị truncate trực tiếp khi temporary write fail.
- High score vẫn giữ monotonic behaviour.
- Highest unlocked level vẫn giữ monotonic behaviour.
- Audio settings vẫn persist đúng.
- Project build thành công.
- Toàn bộ CTest hiện tại pass.

---

## 27. Corrupted Save Fallback

### File liên quan

- [`src/core/SaveManager.cpp`](../../src/core/SaveManager.cpp)
- [`include/core/SaveManager.h`](../../include/core/SaveManager.h)

### S6-TV4-38 — Fallback an toàn khi save file không hợp lệ

`SaveManager::load()` được tăng cường validation để save file thiếu,
hỏng hoặc sai version không làm game crash hoặc tạo partial state.

Save version 1 yêu cầu đầy đủ các field:

```text
version
highScore
highestUnlockedLevel
soundVolume
musicVolume
```

Parser kiểm tra đúng key, đúng kiểu dữ liệu và không chấp nhận token dư sau cấu trúc version 1.

Sau khi parse, dữ liệu tiếp tục được kiểm tra:

```text
version == SAVE_DATA_VERSION
highScore >= 0
highestUnlockedLevel >= 1
0 <= soundVolume <= 100
0 <= musicVolume <= 100
```

Volume cũng phải là giá trị hữu hạn.

Nếu bất kỳ bước validation nào thất bại, SaveManager gọi `resetToDefaults()` và `load()` trả về `false`.

Các failure path có log riêng để phân biệt:

```text
missing save
corrupted format
unsupported version
invalid values
```

### Phân chia trách nhiệm

Task này xử lý fallback an toàn trong production `SaveManager`.

Automated regression tests cho missing, valid, corrupt, version mismatch và high-score monotonicity được bổ sung trong S6-TV4-39.

### Kiểm tra

- Missing file không làm game crash và dùng default data.
- File thiếu field bị reject.
- Key sai hoặc value sai kiểu bị reject.
- Wrong version bị reject.
- High score âm bị reject.
- Highest unlocked level nhỏ hơn 1 bị reject.
- Volume ngoài khoảng 0–100 bị reject.
- Valid version 1 save vẫn load đúng.
- Failure không để lại partial loaded state.
- Project build thành công.
- Toàn bộ CTest hiện tại pass.

---

## 28. SaveManager Regression Tests

### File liên quan

- [`tests/SaveManagerTests.cpp`](../../tests/SaveManagerTests.cpp)
- [`include/core/SaveManager.h`](../../include/core/SaveManager.h)
- [`src/core/SaveManager.cpp`](../../src/core/SaveManager.cpp)
- [`CMakeLists.txt`](../../CMakeLists.txt)

### S6-TV4-39 — Automated tests cho SaveManager

Một test target riêng `save_manager_tests` được bổ sung để kiểm tra các failure path và persistence rule quan trọng của `SaveManager`.

Các test sử dụng save path được inject vào temporary directory của hệ điều hành thay vì production path:

```text
saves/save.txt
```

Do đó automated tests không ghi đè dữ liệu save thật của người chơi.

Test matrix bao gồm:

```text
missing save
valid save
corrupted save
version mismatch
high-score monotonicity
temporary-file cleanup
```

Missing, corrupt và wrong-version save phải trả về `false` từ `load()`và phục hồi toàn bộ default `SaveData`.

Valid save phải khôi phục đúng:

```text
highScore
highestUnlockedLevel
soundVolume
musicVolume
```

High-score test xác nhận score mới cao hơn được persist, sau đó score thấp hơn không thể ghi đè high score hiện tại.

Một regression test bổ sung xác nhận save thành công không để lại file:

```text
save.txt.tmp
```

sau safe replacement.

### Phương pháp kiểm thử

Test suite sử dụng `assert` giống các regression test hiện có của project.

Mỗi test reset temporary directory trước khi chạy để không phụ thuộc state của test trước đó.

Cuối test suite, toàn bộ temporary data được cleanup.

### Kiểm tra

- Missing save fallback pass.
- Valid save load pass.
- Corrupted save fallback pass.
- Wrong-version fallback pass.
- Lower score không overwrite high score.
- Higher score vẫn persist đúng sau reload.
- Successful atomic save không để lại temporary file.
- `save_manager_tests` được đăng ký với CTest.
- Project build thành công.
- Toàn bộ CTest pass.

---

## 29. Level Fixture and Validator Regression Tests

### File liên quan

- [`levels/level0.txt`](../../levels/level0.txt)
- [`tests/LevelValidatorTests.cpp`](../../tests/LevelValidatorTests.cpp)
- [`src/level/TileMap.cpp`](../../src/level/TileMap.cpp)
- [`CMakeLists.txt`](../../CMakeLists.txt)

### S6-TV4-01 - Level 0 trở thành test fixture

`level0.txt` được giữ ngoài release catalog và dùng làm fixture cho automated level-validation tests.

Một row có chiều rộng không nhất quán trong fixture được sửa để toàn bộ 12 map rows đều có chiều rộng 100 tiles.

Automated test xác nhận Level 0:

- load thành công;
- có kích thước 100x12;
- có đúng một Mario spawn;
- có đúng một finish point;

Level 0 không được sử dụng trong New Game hoặc release progression.

### S6-TV4-20 - Level validator regression tests

Một test target mới `level_validator_tests` được bổ sung để bảo vệ các validation rule của `TileMap::loadFromFile()`.

Test matrix bao gồm:

```text
valid Level 0 fixture
inconsistent row width
invalid symbol
missing mario spawn
duplicate Mario spawn
missing finish
duplicate finish
invalid flag pole
```

Các invalid fixture được tạo trong temporary directory nên không làm thay đổi release level data.

CTest chạy validator test với repository root làm working directory để `TileMap` có thể resolve tileset runtime bằng relative path.

### Kiểm tra

- Level 0 fixture load thành công.
- Invalid row width bị reject.
- Invalid tile symbol bị reject.
- Missing/duplicate Mario spawn bị reject.
- Missing/duplicate finish bị reject.
- Invalid flag pole bị reject.
- level_validator_tests được đăng ký với CTest.
- Project build thành công.
- Toàn bộ CTest pass.

---

## 30. Quy Ước Cập Nhật File Này

Sau mỗi task Sprint 6 hoàn tất, TV4 cần bổ sung:

1. Task ID và mục tiêu.
2. Những file đã thay đổi.
3. Class, method hoặc interface mới.
4. Phương pháp hoặc design technique được sử dụng.
5. Ví dụ cách gọi hoặc cách sử dụng.
6. Build, unit test và manual test đã thực hiện.

Không ghi task là hoàn tất nếu chưa build và test thành công.

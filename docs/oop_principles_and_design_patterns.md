# OOP và Design Pattern trong SuperMario

## Phạm vi, đối tượng đọc và cách đọc

Tài liệu này là lối vào nhanh cho developer mới hoặc junior muốn hiểu **vì
sao** code được chia như hiện tại và một thao tác trong game đi qua những lớp
nào. Nội dung mô tả implementation đang có trong repository; chỗ nào là lợi
ích thiết kế hoặc suy luận thì được ghi rõ, không coi đó là cam kết rằng mọi
class đều hoàn hảo theo sách giáo khoa.

Đọc theo thứ tự sau nếu chưa quen codebase:

1. **Mental model** để biết các object chính và ai sở hữu chúng.
2. **OOP** để đọc interface, class con và `unique_ptr` mà không nhầm quan hệ.
3. **Năm pattern** theo nhu cầu: phạm vi toàn cục (Singleton), input
   (Command), thông báo (Observer), màn hình/trạng thái nhân vật (State), sinh
   entity (Factory Method).
4. **Workflow** để nối các mảnh thành một frame game hoàn chỉnh.
5. **Recipe mở rộng**, rồi kiểm tra **pitfall/glossary** trước khi sửa code.

Các sơ đồ đầy đủ nằm ở [class diagram](class_diagram.md). Mô tả pattern dạng
tóm tắt nằm ở [design patterns](design_patterns.md); tài liệu hiện tại giải
thích sâu hơn bằng flow thật và các giới hạn đã kiểm tra từ source.

### Mental model trong một phút

- `Game` chạy vòng lặp cửa sổ. Mỗi frame, nó đưa event/input/update/render cho
  `GameManager`.
- `GameManager` là một Singleton (một instance truy cập toàn cục) và giữ stack
  các `std::unique_ptr<IGameState>`. State trên cùng nhận input/update; state
  overlay có thể được render trên state bên dưới.
- Các service dùng chung khác cũng là Singleton theo source: `EventBus` phát
  event, `SoundManager` quản lý audio, và `TextureManager` cache texture.
  `SaveManager` là ngoại lệ: nó là object được `GameManager` sở hữu theo value.
- `PlayState` là state gameplay. Nó **sở hữu** `Level` và `HUD`, còn `Level`
  sở hữu Box2D world, Mario và danh sách `std::unique_ptr<Entity>`.
- `Entity` là abstraction (hợp đồng chung) cho object trong world. Chuỗi kế
  thừa chính là `Entity -> Character -> Mario/Enemy` và
  `Entity -> Item -> Coin/Mushroom/FireFlower/Star`.
- `InputHandler` chuyển phím thành object `ICommand`. `EventBus` phát
  `GameEvent` cho `PlayState`, `HUD`, `SoundManager` mà publisher không cần gọi
  trực tiếp từng subscriber.
- `EntityFactory` nhận request kiểu `EnemyType`, `ItemType` hoặc tile code rồi
  trả về `std::unique_ptr<Entity>`; `Level` là owner cuối cùng của entity.

> Có thể coi `PlayState` là đạo diễn của màn chơi, `Level` là sân khấu có
> ownership của actor, `Command` là phiếu yêu cầu hành động, `EventBus` là loa
> phát thông báo, còn `State` là bộ luật đang có hiệu lực. Đây là phép so sánh
> giúp đọc code, không phải tên class trong source.

## 1. Từ vựng C++ tối thiểu

- **Class** là bản thiết kế; **object/instance** là một giá trị cụ thể của
  class. Ví dụ `PlayState` là class, `std::make_unique<PlayState>(...)` tạo
  một object.
- **Method** là hàm thành viên của object. `Level::update(float)` là method.
- **Interface/abstract class** là class chỉ nêu hợp đồng cần thực hiện. Trong
  C++, method có `= 0` là **pure virtual**; class còn pure virtual không thể
  tạo object trực tiếp.
- **`virtual`/`override`** cho phép gọi đúng implementation của class con qua
  con trỏ/tham chiếu class cha. Đây là runtime polymorphism (đa hình lúc
  chạy).
- **`std::unique_ptr<T>`** là smart pointer sở hữu duy nhất một `T`. Khi
  pointer bị hủy hoặc bị xóa khỏi container, destructor của `T` được gọi. Dùng
  `std::move` để chuyển ownership; không copy được.
- **Composition (kết hợp)** là object chứa object khác để tạo hành vi lớn hơn;
  **ownership** trả lời “ai chịu trách nhiệm hủy?”. **Aggregation** thường
  chỉ là giữ tham chiếu/con trỏ không sở hữu. Source dùng cả ba dạng nên không
  nên gọi mọi mũi tên là “inheritance”.
- **RAII** (*Resource Acquisition Is Initialization*) nghĩa là lifetime của
  resource gắn với lifetime của object. Ở đây `unique_ptr` quản lý object và
  `Subscription` quản lý đăng ký EventBus.

## 2. Bốn trụ cột OOP trong code thật

### 2.1 Đóng gói (encapsulation)

Đóng gói là giấu dữ liệu và invariant bên trong class, chỉ mở operation cần
thiết. `Entity` giữ `m_body`, vị trí, animation và cờ lifecycle ở vùng
`protected`; `Mario` giữ power/lives/timer và chỉ cho caller gọi các method như
`powerUp`, `powerDown`, `tryStartFireBallShot`. `Level` giữ
`m_entities`, `m_world`, `m_mario` private và cung cấp `getEntities()` dưới
dạng read-only `EntityView`.

Ví dụ runtime: code input không tự sửa velocity Box2D của Mario. Command gọi
`Mario::moveRight()` hoặc đặt intent; `Mario::update/preparePhysics` mới áp
invariant vật lý. Tương tự, `PlayState` không tự `delete` entity mà đưa
`unique_ptr` vào `Level`.

**Giới hạn cần nhớ:** một số getter trả raw pointer không sở hữu (ví dụ
`Level::getMario()`), và `Entity` vẫn có các helper `isEnemy()/isItem()` để
tương thích. Caller phải tuân thủ lifetime của owner; encapsulation không tự
biến raw pointer thành smart pointer.

### 2.2 Trừu tượng (abstraction)

Abstraction giữ lại điều client cần biết và bỏ chi tiết triển khai. Các hợp
đồng tiêu biểu:

- `Entity::update(float)` và `getType()` mô tả object world mà không yêu cầu
  `Level` biết từng thuật toán của Goomba hay Coin.
- `IGameState` mô tả lifecycle + `processInput/update/render`; `GameManager`
  không cần biết vẽ Menu khác Play thế nào.
- `ICommand::execute`, `IObserver::onNotify`, `EntityCreator::create` là các
  seam có thể thay implementation.

Abstraction không có nghĩa là “mọi thứ đều phải interface”. `Level` không kế
thừa `ILevel`; nó là concrete orchestrator sở hữu nhiều subsystem. Tách
interface chỉ đáng làm khi có nhiều implementation hoặc cần boundary rõ ràng.

### 2.3 Kế thừa (inheritance)

Kế thừa biểu diễn quan hệ “là một” (*is-a*) và tái sử dụng contract/chức năng
chung:

```text
Entity
├── Character
│   ├── Mario
│   └── Enemy
│       └── Goomba, Koopa, ...
└── Item
    └── Coin, Mushroom, FireFlower, Star
```

`Character` bổ sung health, facing, grounded; `Enemy` bổ sung patrol/stomp
contract; `Item` bổ sung `onCollect(Mario&)`. Class concrete cung cấp hành vi
riêng. Đây là inheritance, không phải ownership: `Mario : Character` không
có nghĩa Mario sở hữu một Character riêng.

### 2.4 Đa hình (polymorphism)

Runtime polymorphism là cùng một interface nhưng lời gọi được quyết định theo
object thật lúc chạy. `Level` lưu `std::vector<std::unique_ptr<Entity>>`; vòng
lặp gọi `entity->update(dt)` và C++ dispatch tới Goomba, Coin, FireBall, v.v.
Tương tự, `GameManager` lưu `unique_ptr<IGameState>`, `InputHandler` lưu
`unique_ptr<ICommand>`, còn `EventBus` gọi `IObserver::onNotify`.

Đa hình chỉ an toàn khi base destructor là virtual; các base chính đều có
destructor virtual (`Entity`, `IGameState`, `ICommand`, `IObserver`). Khi cần
truy cập API riêng (ví dụ `Enemy::setTileMap`), code hiện tại dùng identity
`EntityType/EntitySubtype` và cast có điều kiện; đó là dấu hiệu cần đọc
invariant, không phải lý do để cast tùy tiện.

## 3. Composition, ownership và RAII

Bảng dưới đây trả lời “object nào chứa object nào?” và phân biệt rõ với cây kế
thừa:

| Owner | Thành phần/lifetime | Ý nghĩa runtime |
| --- | --- | --- |
| `GameManager` | `vector<unique_ptr<IGameState>> m_stateStack` | Manager hủy state khi `CHANGE`, `POP` hoặc kết thúc stack. |
| `GameManager` | `SaveManager m_saveManager` (object theo value) | SaveManager sống cùng manager nhưng không phải Singleton. |
| `PlayState` | `unique_ptr<Level> m_level`, `unique_ptr<HUD> m_hud` | Reload level tạo ownership mới; HUD được tạo sau Mario. |
| `Level` | `unique_ptr<b2World>`, `unique_ptr<Mario>`, `vector<unique_ptr<Entity>>` | Level là owner duy nhất của world/entity list và nhận child spawn qua outbox. |
| `Mario` | `unique_ptr<IMarioState> m_statePattern` | State power-up được thay thế khi chuyển form. |
| `Entity` | `unique_ptr<AnimationSystem>` | Animation state dọn dẹp theo entity. |
| `PlayState`/`HUD`/`SoundManager` | `vector<Subscription>` | Hủy token sẽ unsubscribe EventBus theo RAII. |

Các thành phần như `TextureManager&` trong `Level` hoặc `Mario*` trong command
là non-owning reference/pointer. Owner phải sống lâu hơn consumer. Vì reload
`Level` làm địa chỉ Mario cũ không còn hợp lệ, `PlayState::rebindCommands()`
được gọi sau load/reload để tạo command trỏ tới Mario mới.

## 4. SOLID: đọc như tiêu chí kiểm tra, không phải lời khen tuyệt đối

SOLID là năm heuristic (nguyên tắc gợi ý) để giảm coupling, không phải năm
pattern mà project bắt buộc đạt 100%. Evidence hiện tại cho thấy mức hỗ trợ
như sau:

| Nguyên tắc | Evidence trong repository | Kết luận có giới hạn |
| --- | --- | --- |
| **S — Single Responsibility**: một class nên có một lý do đổi | `InputHandler` lo binding/dispatch; `EventBus` lo subscription/notify; `SaveManager` lo đọc/ghi save. | Có phân tách tốt ở các boundary này, nhưng `PlayState` và `Level` vẫn là orchestrator lớn (input, transition, gameplay/render), nên không tuyên bố SRP hoàn hảo. |
| **O — Open/Closed**: mở rộng qua seam, hạn chế sửa client | `ICommand` cho command mới; `EntityCreator` cho nhóm creator mới; `EntityFactory` trả base `Entity`. | Một loại enemy/item mới vẫn phải thêm enum và `switch` trong creator; `SpawnRequest` là closed `variant`. Mở rộng được nhưng chưa hoàn toàn đóng với sửa mapping. |
| **L — Liskov Substitution**: class con dùng được ở chỗ base | `Level` chạy mọi `unique_ptr<Entity>` qua `update`; state chạy qua `IGameState`; enemy/item giữ các contract virtual. | Điều này đúng cho contract chung, nhưng code vẫn dùng subtype/capability và cast khi cần policy cụ thể. Không nên giả định mọi method base có ý nghĩa như nhau ở mọi subclass. |
| **I — Interface Segregation**: interface nhỏ, client không phụ thuộc method thừa | `ICommand::execute`, `IObserver::onNotify`, `IMarioState` tách theo vai trò; `IGameState` chỉ có lifecycle/frame API. | Đây là boundary hẹp hữu ích. Ngược lại `Entity` gánh physics, render, identity, collision và spawn hook; interface này rộng vì nhu cầu hiện tại. |
| **D — Dependency Inversion**: phụ thuộc abstraction thay vì concrete | `GameManager` giữ `unique_ptr<IGameState>`, EventBus gọi `IObserver`, factory gọi `EntityCreator`. | `PlayState` vẫn include concrete command/state; `Level` include concrete entity và `GameManager` truy cập Singleton. DIP được dùng chọn lọc, chưa phải dependency injection toàn hệ thống. |

Khi review một thay đổi, hãy hỏi “seam nào cần giữ ổn định?” thay vì thêm
interface chỉ để có thêm chữ SOLID. Nếu cần thay physics hay EventBus thật sự,
đó mới là lúc cân nhắc injection; không tự ý đổi public contract trong một task
entity nhỏ.

## 5. Pattern Singleton — một điểm truy cập cho service dùng chung

### Ý nghĩa và participants thật

**Singleton** bảo đảm một class có một instance được tạo/lấy qua điểm truy cập
chung, thường là `getInstance()`. Trong C++, implementation hiện tại dùng
function-local `static` (Meyer's Singleton), constructor private và xóa copy
để caller không tạo bản thứ hai.

| Vai trò | Evidence và cách dùng trong project |
| --- | --- |
| Điều phối game | `GameManager::getInstance()`; constructor private, copy bị xóa; quản lý state stack và deferred operation. `Game`/state gọi `changeState`, `pushState`, `update`, `render` qua instance này. |
| Subject toàn cục | `EventBus::getInstance()`; constructor/private destructor và copy bị xóa; giữ subscription state rồi notify observer. |
| Audio service | `SoundManager::getInstance()`; copy và move đều bị xóa; `Game` khởi tạo service, các state/entity gọi play/pause music hoặc SFX. Nó đồng thời là `IObserver` của EventBus. |
| Resource cache | `TextureManager::getInstance()`; constructor private, copy bị xóa; `Level` lấy reference ở `Level::Level()` rồi truyền non-owning pointer/reference cho entity/renderer. |

Workflow đơn giản:

1. Caller gọi `X::getInstance()`. Lần đầu, function-local `static X instance`
   được dựng; lần sau trả cùng object.
2. `Game::Game()` lấy `SoundManager` và `GameManager`; `Level::Level()` lấy
   `TextureManager`; gameplay phát event qua `EventBus`. Không class client nào
   tự `new` thêm manager cạnh các instance này.
3. Singleton giữ service/cache hoặc điểm điều phối sống đủ lâu; object gameplay
   vẫn có ownership riêng. Ví dụ `Level` sở hữu `unique_ptr<Entity>`, nhưng
   không sở hữu `TextureManager`.

### Lợi ích, trade-off và giới hạn

- Một audio mixer, event registry, texture cache và state coordinator tránh
  duplicate resource hoặc hai nơi cùng điều khiển flow. Đây là lợi ích thiết kế
  suy ra từ cách các caller dùng service, không phải bảo đảm mọi lỗi global đều
  biến mất.
- Đổi lại, `getInstance()` là global access: dependency bị ẩn, test khó thay
  fake instance, test dễ phụ thuộc thứ tự/lifetime static, và callback/event có
  thể tạo coupling ngầm. Khi một class chỉ cần texture/audio, ưu tiên nhận
  reference non-owning rõ ràng như `Level` truyền texture manager cho entity.
- Singleton không đồng nghĩa với “mọi object dùng chung”. `EntityFactory` có
  constructor public; `defaultFactory()` trong compatibility shim chỉ là một
  helper function-local, không biến class thành Singleton.
- **SaveManager không phải Singleton:** `SaveManager` có constructor public và
  `GameManager` giữ `SaveManager m_saveManager` theo value. Muốn đọc save hãy
  dùng `GameManager::getInstance().getSaveManager()`, không thêm
  `SaveManager::getInstance()`.

**Evidence:** `include/core/GameManager.h:19-50`,
`src/core/GameManager.cpp:27-28`,
`include/patterns/EventBus.h:36-58`,
`src/patterns/EventBus.cpp:196-208`,
`include/core/SoundManager.h:116-130`,
`src/core/SoundManager.cpp:40-48`,
`include/core/TextureManager.h:23-33`, `include/core/TextureManager.h:64-84`,
`src/core/TextureManager.cpp:15-21`,
`include/core/SaveManager.h:22-26`.

## 6. Pattern Command — biến input thành yêu cầu có thể thay thế

### Vấn đề và participants

Nếu `Game` hoặc `PlayState` viết một `if (phím X) mario->...` cho từng phím,
đổi key hoặc dùng hai người chơi sẽ làm input dính chặt gameplay. Command đóng
gói một yêu cầu thành object.

| Vai trò | Class/symbol thật |
| --- | --- |
| Command contract | `ICommand::execute()` trong `include/patterns/ICommand.h:16-20` |
| Invoker/registry | `InputHandler`, map key tới `vector<Binding>` trong `include/patterns/InputHandler.h:40-80` |
| Concrete command | `JumpCommand`, `MoveLeftCommand`, `MoveRightCommand`, `PauseCommand`; `RunCommand`/`ShootCommand` bọc callback |
| Receiver | `Mario` cho move/jump; callback của `PlayState` gọi `Level::requestFireBallShot`; Pause gửi event |
| Wiring/client | `PlayState::rebindCommands()` trong `src/states/PlayState.cpp:62-132` |

### Workflow một frame

1. `Game::processEvents()` cập nhật `InputState` (Pressed/Held/Released) rồi
   `GameManager::processInput()` chuyển snapshot đó tới state trên cùng.
2. `PlayState::processInput()` reset intent để input cũ không “chảy” qua pause
   hoặc transition, sau đó gọi `m_inputHandler.handleInput(inputState)`
   (`src/states/PlayState.cpp:339-380`).
3. `InputHandler` kiểm tra trigger. Với phím được bind, nó gọi
   `binding.command->execute()` (`src/patterns/InputHandler.cpp:34-95`).
   Binding `Horizontal`/`Vertical` còn chọn phím có press-order mới nhất để
   tránh hai hướng chạy cùng lúc.
4. `JumpCommand`/move command gọi method Mario. `ShootCommand` gọi request
   của Level; Level kiểm tra FIRE state, cooldown, giới hạn hai FireBall và có
   thể queue khi Box2D đang locked.
5. Sau input, `Level::update()` áp intent vào physics/entity. Command không tự
   sở hữu hay update physics.

Escape là ví dụ dễ thấy: `PauseCommand::execute()` phát `GAME_PAUSED` qua
`EventBus`; `PlayState::onNotify()` queue `GameManager::pushState(PauseState)`.
Command chỉ mô tả yêu cầu; State/Observer quyết định hệ thống phản ứng ra sao.

### Lợi ích, trade-off và extension point

- Lợi ích thiết kế: đổi key bằng `bindKey`, dùng cùng action cho co-op, test
  command độc lập hơn và không rải logic input trong `Game`.
- `InputHandler` **sở hữu** command bằng `unique_ptr`; `getAction()` chỉ trả
  non-owning pointer. Command giữ raw `Mario*`/callback nên phải bind lại sau
  mỗi `Level` reload.
- Đây không phải undo/redo system: `ICommand` hiện chỉ có `execute()`, không
  có `undo()`, history hay queue request. `gameplayEnabled=false` bỏ input
  hiện tại, không buffer nó.
- Khi thêm action, tạo class implement `ICommand` (hoặc callback command hiện
  có), bind với trigger/group thích hợp trong `rebindCommands()`, rồi kiểm tra
  guard khi Mario đang chết, pause hoặc transition.

**Evidence:** `include/patterns/ICommand.h:16-20`,
`include/patterns/InputHandler.h:49-80`,
`src/patterns/InputHandler.cpp:10-95`,
`src/patterns/JumpCommand.cpp:15-23`,
`src/patterns/PauseCommand.cpp:18-23`.

## 7. Pattern Observer — một sự kiện, nhiều phản ứng

### Vấn đề và participants

Khi Mario nhặt Coin, gameplay không nên biết chi tiết HUD refresh hay sound
file nào được phát. Observer cho phép subject thông báo và nhiều observer tự
đăng ký.

| Vai trò | Class/symbol thật |
| --- | --- |
| Subject contract | `ISubject::subscribe/unsubscribe/notify` |
| Subject/bus | Singleton `EventBus`, `EventBus::getInstance()` |
| Observer contract | `IObserver::onNotify(const GameEvent&)` |
| Concrete observers | `PlayState`, `HUD`, `SoundManager` |
| Data | `EventType` + value-only `GameEvent`/`EventContext` (`std::variant`) |
| Lifetime token | move-only `Subscription`; giữ token còn sống nghĩa là còn đăng ký |

### Workflow

1. Khi vào Play, `PlayState::onEnter()` subscribe bốn event cần xử lý; HUD và
   SoundManager subscribe nhóm event riêng. Mỗi lời gọi trả một `Subscription`
   được cất trong vector.
2. Gameplay/collision gọi `EventBus::notify(...)`. Ví dụ
   `CollisionManager::defeatEnemy()` phát `ENEMY_STOMPED` hoặc event defeat;
   `Mario::powerUp()` phát `PLAYER_POWER_UP`, `Mario::powerDown()` phát
   `PLAYER_POWER_DOWN` khi chỉ bị tụt form, còn `Mario::loseLife()` phát
   `PLAYER_DIED`; Level phát `LEVEL_COMPLETED`.
3. `EventBus::notify(const GameEvent&)` dispatch **đồng bộ** tới observer đang
   đăng ký (`src/patterns/EventBus.cpp:242-267`). Nó copy snapshot listener để
   callback có thể unsubscribe mà không làm hỏng vòng lặp, rồi kiểm tra lease
   còn active trước mỗi callback.
4. `PlayState::onNotify()` đổi progress/queue state; `HUD::onNotify()` refresh
   chữ hoặc timer; `SoundManager::onNotify()` chọn SFX. Publisher không cần
   biết ba class này.
5. `onExit()`/destructor clear token. Destructor `Subscription` gọi `reset`,
   lease cuối cùng disconnect khỏi EventBus (`include/patterns/Subscription.h:23-48`
   và `src/patterns/EventBus.cpp:161-194`).

### Lợi ích, trade-off và giới hạn

- Lợi ích: thêm HUD/audio/analytics observer mà không sửa publisher; payload
  là value-only nên bus không giữ pointer/reference tới domain object.
- EventBus là Singleton nên tiện cho event toàn game nhưng tạo global coupling;
  event name/type vẫn là contract chung.
- Dispatch đồng bộ, không phải message queue: observer chạy ngay trong call
  `notify`, nên callback dài hoặc re-entrant có thể ảnh hưởng frame. Không
  publish con trỏ vào `GameEvent`; `EventContext` cố ý chỉ nhận các value
  context đã định nghĩa.
- Raw `IObserver*` trong record không sở hữu observer. **Bắt buộc** giữ
  `Subscription` và hủy token trước/khi observer bị hủy; token là lifecycle
  safety, không biến observer thành owned object.
- `notify(EventType)` là overload tương thích và tạo `GameEvent` rỗng; với dữ
  liệu mới nên dùng context có kiểu rõ ràng.

**Evidence:** `include/patterns/EventBus.h:36-57`,
`include/patterns/IObserver.h:14-26`,
`include/patterns/GameEvent.h:38-61`,
`src/patterns/EventBus.cpp:210-271`,
`src/states/PlayState.cpp:224-258`,
`src/ui/HUD.cpp:112-146`,
`src/core/SoundManager.cpp:45-109`,
`src/physics/CollisionManager.cpp:522-595`.

## 8. Pattern State — thay đổi bộ luật theo trạng thái hiện tại

Project có **hai lớp State** liên quan nhưng không được trộn tên:

1. **Game state:** `IGameState` với `MenuState`, `PlayState`, `PauseState`,
   `GameOverState`, `WinState`, ... đại diện màn hình/flow lớn.
2. **Mario power-up state:** `IMarioState` với `SmallMarioState`,
   `SuperMarioState`, `SmallFireMarioState`, `SuperFireMarioState`, được
   `Mario` sở hữu qua `unique_ptr`.

### 8.1 Game state stack và deferred transition

`IGameState` định nghĩa `onEnter`, `onExit`, `onPause`, `onResume`,
`processEvents`, `processInput`, `update`, `render`, `isOverlay`. `GameManager`
không thay object ngay khi đang ở giữa callback; `changeState`, `pushState`,
`popState` chỉ append `PendingOp`. Sau `top()->update(dt)`,
`processPendingOps()` swap queue và apply ở cuối update
(`src/core/GameManager.cpp:32-113`).

Flow Pause cụ thể:

1. `PlayState` đang ở top và bind Escape cho `PauseCommand`.
2. Command phát `GAME_PAUSED`; `PlayState::onNotify` gọi
   `GameManager::pushState(std::make_unique<PauseState>())`.
3. Cuối update, manager gọi `PlayState::onPause`, push Pause rồi `PauseState::onEnter`.
4. Vì `PauseState::isOverlay()==true`, render vẽ state bên dưới trước; input và
   update chỉ vào Pause. Pop gọi `PauseState::onExit`, rồi Play `onResume`.

`CHANGE` gọi `onExit` mọi state và thay cả stack; `PUSH` giữ state dưới làm
overlay; `POP` bỏ top. Việc deferred có thể tạo trễ tới safe point cuối frame,
nhưng tránh hủy object đang nằm trên call stack và tránh sửa vector khi đang
iterate.

### 8.2 Mario form state

Khi Mushroom/FireFlower gọi `Mario::powerUp`, `Mario::applyStateTransition`
chọn object state tương ứng, còn `Mario` tự dựng animation và rebuild fixture.
Trong runtime production hiện tại, `Mario::canBreakBricks()` mới là method
delegates qua `m_statePattern->canBreakBricks()` (hoặc Star power). Ngược lại,
`Mario::canShootFireBall()` kiểm tra trực tiếp enum `MarioState` và cooldown;
nó **không** gọi `m_statePattern->canShootFireBall()`.

`IMarioState` vẫn khai báo `getStateType()`, `getHitboxSize()`,
`canShootFireBall()`, `canBreakBricks()` cùng `onEnter/onExit/update`, và các
class concrete implement chúng. Tuy nhiên search call site hiện tại chỉ cho
thấy production đọc `canBreakBricks()` qua state; chưa có lời gọi production
cho `getHitboxSize()`, `getStateType()`, `canShootFireBall()` hoặc callback
lifecycle/update của `IMarioState`. Vì vậy hãy phân biệt **seam đã triển khai**
với **hành vi đang được dùng**. Nếu Box2D đang locked hoặc không đủ trần,
growth được lưu vào `m_pendingGrowthState`, rồi xử lý ở update an toàn.

**Caveat quan trọng:** đây là State seam có thật, nhưng không phải toàn bộ logic
đã được đẩy sang bốn class state. `Mario` vẫn giữ enum, chuyển state, timer,
animation setup, fixture và nhiều movement/damage policy; `update()` của các
state hiện tại còn tối giản. Vì vậy hãy mô tả đây là “Mario có State seam cho
form, trong đó production hiện dùng capability phá gạch” chứ không hứa rằng
mỗi form là một gameplay engine độc lập.

**Evidence:** `include/states/IGameState.h:19-35`,
`include/core/GameManager.h:26-74`,
`src/core/GameManager.cpp:44-113`,
`include/states/IMarioState.h:17-33`,
`include/entities/Mario.h:54-63`, `include/entities/Mario.h:187-205`,
`src/entities/Mario.cpp:937-989`, `src/entities/Mario.cpp:1050-1134`,
`src/entities/Mario.cpp:1461-1467`,
`include/states/SmallMarioState.h:10-24`.

## 9. Pattern Factory Method — tạo object qua creator seam

### Vấn đề và participants

Tile map có mã `G`, `K`, `C`, `?`, ...; nếu `Level` tự rải `new Goomba`,
`new Koopa`, `new Coin` thì mapping và constructor context bị phân tán. Factory
Method tập trung việc chọn creator/constructor nhưng vẫn trả abstraction
`unique_ptr<Entity>`.

| Vai trò | Class/symbol thật |
| --- | --- |
| Product abstraction | `Entity` |
| Creator contract | abstract `EntityCreator::create(const SpawnRequest&, const SpawnContext&)` |
| Concrete creators | `EnemyCreator`, `ItemCreator`, `WorldObjectCreator` |
| Orchestrator/canonical seam | `EntityFactory::create(...)` |
| Request/context | `SpawnRequest` (`std::variant<EnemyType, ItemType, char>`), `SpawnContext` (world/theme) |
| Client/owner | `Level::spawnEntitiesFromTileMap()`; Level nhận ownership |

### Workflow load level

1. `Level::spawnEntitiesFromTileMap()` tạo một `EntityFactory`, duyệt
   `SPAWN_CODES`, đổi tile thành world position/theme và tạo
   `SpawnRequest::tile(code, worldPos)` (`src/level/Level.cpp:625-677`).
2. `EntityFactory::create` dùng `std::visit` trên payload. Enemy chọn
   `EnemyCreator`, item chọn `ItemCreator`, char/tile chọn
   `WorldObjectCreator` (`src/patterns/EntityFactory.cpp:18-34`).
3. Creator map enum/tile code sang concrete constructor, truyền Box2D world và
   theme. `WorldObjectCreator` có thể delegate enemy/item creator; request sai
   loại hoặc code không hỗ trợ trả `nullptr`.
4. Khi có entity, Level set `TextureManager`; enemy được gắn `TileMap`; rồi
   `m_entities.push_back(std::move(entity))`. Từ đó Level là single owner,
   update/render/remove bằng base interface và destructor dọn object.

Mario là ngoại lệ có chủ ý: `Level` tạo Mario trực tiếp ở dòng 634 vì Mario có
lifecycle/player wiring riêng; factory hiện tập trung enemy, item và
world-object spawn từ map.

### Canonical seam, compatibility và giới hạn

- Canonical production API là **non-static** `EntityFactory::create` và
  `EntityCreator` abstract method. Static `createEnemy`, `createItem`,
  `createFromTileCode` chỉ là compatibility shims; chúng forward tới một
  `defaultFactory()` để không nhân đôi mapping.
- `EntityFactory` có constructor public và có thể tạo nhiều object. Static
  function-local `defaultFactory` không biến `EntityFactory` thành Singleton.
- Mapping vẫn là `switch` tập trung. Thêm enemy/item thường phải thêm enum và
  case tương ứng; đó là trade-off rõ ràng của request `variant`, không phải
  “mở rộng không sửa code”.
- Lợi ích thực tế: client không cần include/biết constructor của toàn bộ
  concrete entity, ownership trả về rõ ràng, creator category dễ test/thay.

**Evidence:** `include/patterns/EntityFactory.h:20-50`,
`include/patterns/EntityCreator.h:14-20`,
`include/patterns/SpawnRequest.h:44-99`,
`src/patterns/EntityFactory.cpp:12-59`,
`src/patterns/EnemyCreator.cpp:24-76`,
`src/patterns/ItemCreator.cpp:16-36`,
`src/patterns/WorldObjectCreator.cpp:31-116`,
`src/level/Level.cpp:625-677`.

## 10. Các workflow nối OOP và pattern

### 10.1 Một frame input → gameplay → render

1. `Game::run()` tính `dt`, gọi `processEvents`, `update`, `render`.
2. Event window cập nhật `InputState`; `GameManager::getInstance()` forward tới
   `IGameState` trên cùng (Singleton access + abstraction + runtime
   polymorphism).
3. `PlayState` reset intent và `InputHandler` dispatch Command. Command gọi
   API Mario/Level, không chọc vào private physics (encapsulation).
4. `GameManager::update` gọi `PlayState::update`; PlayState gọi `Level::update`.
   Level step Box2D, update Mario và danh sách Entity qua `Entity::update`
   (composition + polymorphism).
5. Level/State/HUD render theo owner; `GameManager` xử lý overlay nếu top state
   là Pause. State transition pending chỉ apply ở safe point cuối update.

### 10.2 Load tile map → entity sống trong Level

`PlayState::onEnter()` load `Level`; Level load TileMap/world, tạo Mario trực
tiếp, sau đó Factory tạo enemies/items/world objects. `unique_ptr` đi từ
creator → Level vector; mỗi frame entity virtual update. Spawner có thể trả
pending `unique_ptr<Entity>` qua `takePendingSpawns`; Level gom trước rồi append
để không mutate vector đang iterate. Khi entity bị mark/remove, `remove_if`
loại nó và RAII hủy physics/entity.

### 10.3 Collision → sự kiện → UI/audio/state

`ContactListener::BeginContact` giao contact cho
`CollisionManager::resolve`; manager tạo `CollisionContext`, gọi callback
entity và dispatch policy. Khi stomp/defeat, collision manager phát event.
Item collision gọi `Item::onCollect`; Mario/Level phát power, coin, fireball,
death/completion event. Cụ thể, `Mario::loseLife()` phát `PLAYER_DIED`, không
phải `PLAYER_POWER_DOWN`; `powerDown()` mới phát `PLAYER_POWER_DOWN` khi tụt
form mà chưa mất life. EventBus gọi đồng bộ:

- `HUD::onNotify` cập nhật score/lives/power/timer;
- `SoundManager::onNotify` phát SFX;
- `PlayState::onNotify` shake camera, ghi pending death/reload/game-over hoặc
  bắt đầu fade completion. Nó không trực tiếp đổi sang `WinState` hay
  `GameOverState` ngay trong callback.

Một publisher có thể vẫn gọi direct method (ví dụ Level gọi `item->onCollect`),
nhưng phần global notification là Observer. Không gắn nhãn toàn bộ call graph
là EventBus.

### 10.4 Power-up → Mario State → event

Item gọi `Mario::powerUp`; Mario kiểm tra upgrade hợp lệ, queue nếu world locked,
thay `IMarioState`, dựng hitbox/animation rồi phát `PLAYER_POWER_UP`. HUD và
SoundManager phản ứng qua Observer. Production dùng `Mario::canBreakBricks()`
để đọc capability phá gạch từ `m_statePattern`, nhưng
`Mario::canShootFireBall()` dùng enum `MarioState` và cooldown trực tiếp, không
đọc callback fire capability của state. Khi bị damage, `powerDown` lùi
`FIRE_SUPER -> SUPER -> SMALL` và phát `PLAYER_POWER_DOWN`; nếu đang SMALL,
`loseLife()` giảm life rồi phát `PLAYER_DIED`.

### 10.5 Chuyển màn hình an toàn

Có hai terminal flow cần tách rõ:

1. `Mario::loseLife()` phát `PLAYER_DIED` ở
   `src/entities/Mario.cpp:1143-1200`. `PlayState::onNotify()` chỉ shake camera,
   lưu score và đặt `m_isReloadPending` hoặc `m_isGameOverPending`
   (`src/states/PlayState.cpp:279-309`). Sau khi animation chết hoàn tất hoặc
   fallback timer hết, `PlayState::update()` mới đặt `m_needsReload`/
   `m_needsGameOver` và gọi `navigateToLevel()` hoặc queue
   `changeState(GameOverState)` (`src/states/PlayState.cpp:569-606`).
2. `LEVEL_COMPLETED` tới `PlayState::onNotify()` để snapshot progress và bắt
   đầu fade (`src/states/PlayState.cpp:314-329`). Trong
   `updateTransition()`, nếu qua level cuối thì mới queue `WinState`; nếu chưa,
   load level kế tiếp (`src/states/PlayState.cpp:640-660`).

`GameManager` chỉ apply các request ở safe point. Chuỗi này minh họa State +
Observer + composition ownership + deferred mutation; đừng gọi `changeState`
trực tiếp từ nơi đang lặp `m_stateStack`.

## 11. Recipe mở rộng cho developer

### Thêm một Command

1. Xác định receiver và lifetime: Mario hiện tại, Level hay callback an toàn.
2. Tạo class kế thừa `ICommand`, implement `execute()`; giữ dependency
   non-owning hoặc callback nhỏ, không tự quản lý receiver.
3. Bind bằng `unique_ptr` trong `PlayState::rebindCommands()` với
   `Pressed/Held/Released` và `InputGroup` đúng.
4. Nếu Level reload tạo Mario mới, gọi lại `rebindCommands()`; không giữ pointer
   cũ. Kiểm tra action bị chặn khi transition/death/pause.
5. Test rebind/unbind, trigger Held và Released; nhớ `InputHandler` không buffer
   input bị suppress.

### Thêm một Entity/enemy/item

1. Chọn đúng quan hệ: kế thừa `Enemy`/`Item` nếu là loại entity tương ứng; dùng
   composition cho component/lifetime thay vì tạo nhánh kế thừa chỉ để chứa
   object.
2. Implement pure virtual `update`, type/subtype/capability và contract riêng
   (`Enemy::patrol/onStomp`, `Item::onCollect`, ...).
3. Chọn request: thêm enum `EnemyType`/`ItemType`, hoặc tile code trong
   `SpawnRequest`; thêm mapping ở creator phù hợp. Với world object có thể
   delegate `WorldObjectCreator`.
4. Bảo đảm constructor nhận `SpawnContext.world/theme` khi cần; unsupported
   request trả `nullptr` thay vì trả object nửa khởi tạo.
5. Khi Level nhận result: set texture, gắn TileMap cho enemy, `std::move` vào
   `m_entities`; không tạo thêm parallel owner. Kiểm tra update, removal,
   Box2D destroy và render.

### Thêm một event

1. Thêm `EventType` và, nếu cần dữ liệu, một struct value trong `EventContext`;
   không đưa raw pointer/reference/domain object vào variant.
2. Subscriber kế thừa `IObserver`, subscribe ở lifecycle rõ ràng và giữ
   `Subscription` member; clear token ở `onExit`/destructor.
3. Publisher gọi `notify(GameEvent{...})` đúng thời điểm; nhớ dispatch đồng bộ
   và callback có thể re-enter/unsubscribe.
4. Cập nhật từng observer có lý do nhận event. Nếu chỉ có direct caller, không
   thêm EventBus cho có hình thức.
5. Kiểm tra token move-only, duplicate subscription, observer chết trước token,
   và payload copy được.

### Thêm một Game State hoặc Mario State

1. Với màn hình/game mode: kế thừa `IGameState`, implement lifecycle + frame
   methods, chọn `isOverlay()`. Đặt resource owned bằng member smart pointer
   hoặc value.
2. Chuyển qua `GameManager::changeState/pushState/popState` bằng
   `unique_ptr`; không tự sửa stack và không destroy state trong callback.
3. Với power-up: implement `IMarioState`, thêm nhánh trong
   `Mario::applyStateTransition`, rồi kiểm tra **call site production**. Hiện
   chỉ `Mario::canBreakBricks()` đọc capability từ state; fire eligibility,
   hitbox và animation vẫn do `Mario`/enum điều phối.
4. `IMarioState` không phải `IGameState`: đừng tự thêm `onPause/onResume`,
   EventBus subscription hoặc command binding vào Mario state. Các callback
   `IMarioState::onEnter/onExit/update` hiện là contract nhưng chưa có call
   site production; nếu muốn dùng chúng, phải thêm call site có chủ đích và
   kiểm tra lifecycle riêng.
5. Kiểm tra đúng lifecycle của loại state đã thêm: game state cần
   `onEnter/onExit/onPause/onResume`, overlay/input/render và failure path;
   Mario state cần transition, fixture/animation/ceiling/world-lock và không
   giữ pointer tới Level/Mario cũ sau reload.

## 12. Pitfall và những thứ **không** nên gọi nhầm là pattern

| Nhầm lẫn | Sự thật trong code |
| --- | --- |
| “`SaveManager` là Singleton vì save dùng toàn game.” | Sai. `SaveManager` có constructor public ở `include/core/SaveManager.h:22-26`; `GameManager` giữ `SaveManager m_saveManager` theo value ở `include/core/GameManager.h:72-74`. Dùng `GameManager::getInstance().getSaveManager()` để lấy object owner, không thêm `SaveManager::getInstance()`. |
| “Mọi hàm static factory là Simple Factory.” | Sai/thiếu. Canonical seam là `EntityFactory::create` non-static → `EntityCreator` → `EnemyCreator/ItemCreator/WorldObjectCreator`. Static helpers chỉ forward compatibility (`include/patterns/EntityFactory.h:25-45`, `src/patterns/EntityFactory.cpp:37-59`). Hãy dùng tên Factory Method cho implementation hiện tại. |
| “`defaultFactory()` làm EntityFactory thành Singleton.” | Không. `EntityFactory` vẫn public-constructible và có member creators; function-local instance chỉ tránh duplicate mapping cho shim. |
| “Kế thừa và composition giống nhau.” | `Mario : Character` hoặc `Coin : Item` là inheritance. `Level` chứa `unique_ptr<Mario>`/`unique_ptr<Entity>`, `Mario` chứa `unique_ptr<IMarioState>` là composition + ownership. |
| “EventBus thay mọi direct call.” | Không. Collision/Level vẫn gọi direct domain methods; EventBus dành cho global notification tới HUD/Sound/PlayState. |
| “Observer là async queue.” | Không. `notify` gọi `onNotify` ngay trong cùng call stack; snapshot chỉ bảo vệ iteration/lifecycle. |
| “State đã tách toàn bộ logic Mario.” | Chưa. `Mario` vẫn giữ enum, transition, physics, fixture và animation orchestration; concrete `IMarioState` có nhiều callback/capability nhưng production hiện chỉ đọc `canBreakBricks()` qua state. |
| “`EntityView` cho phép sửa entity.” | Không. Đây là read-only, non-owning view; Level mới mutate ownership. |
| “Cứ giữ raw Mario pointer trong command là ổn mãi.” | Chỉ ổn trong lifetime của Level hiện tại. Reload phải rebind; token/unique_ptr không tự sửa raw pointer. |

## 13. Checklist trước khi review một thay đổi

- Tôi đã xác định đây là **is-a** (inheritance) hay **has-a/owns-a**
  (composition) chưa?
- Interface mới có pure virtual contract nhỏ, rõ và có class sử dụng thật chưa?
- Object nào sở hữu resource? Có `unique_ptr`, value hoặc RAII token đúng
  lifetime chưa? Có raw pointer nào vượt owner không?
- Thay đổi có đi qua seam hiện có (`ICommand`, `IObserver`, `IGameState`,
  `EntityCreator`) thay vì thêm global `if/switch` ở client không?
- Nếu là Command, đã rebind sau Level reload và chặn state không hoạt động chưa?
- Nếu là event, payload có value-only không, token có được giữ và clear đúng lúc
  không, callback synchronous có an toàn không?
- Nếu là **game state**, operation có deferred qua GameManager không; overlay
  lifecycle và failure path đã rõ chưa? Với Mario State, kiểm tra transition và
  physics/world-lock theo recipe riêng bên trên.
- Nếu dùng Singleton, source có chứng minh `getInstance()`, constructor/copy
  policy và một lý do global access hợp lệ không? Đã cân nhắc global coupling,
  testability và tránh biến `SaveManager` thành Singleton chưa?
- Nếu là entity, factory mapping, context world/theme, texture, removal và
  owner `Level` đã được nối đủ chưa?
- Tôi đang mô tả **evidence** hay chỉ nói một design benefit/inference? Nếu là
  inference, đã ghi rõ giới hạn chưa?

## 14. Bảng source map nhanh

| Chủ đề | Điểm đọc trước |
| --- | --- |
| Entity hierarchy, virtual contract, capability | `include/entities/Entity.h:31-42`; `include/entities/Entity.h:116-135`; `include/entities/Character.h:17-31`; `include/entities/Enemy.h:21-42`; `include/items/Item.h:14-42` |
| Ownership Level/entity | `include/level/Level.h:192-201`; `src/level/Level.cpp:625-677`; `src/level/Level.cpp:986-1065`; `src/level/Level.cpp:1643-1651` |
| Game loop/State manager | `src/core/Game.cpp:105-140`; `include/core/GameManager.h:26-74`; `src/core/GameManager.cpp:32-113` |
| Singleton services | `include/core/GameManager.h:19-50`; `include/patterns/EventBus.h:36-58`; `include/core/SoundManager.h:116-130`; `include/core/TextureManager.h:23-33`; `include/core/TextureManager.h:64-84`; `src/core/TextureManager.cpp:15-21` |
| Command input | `include/patterns/ICommand.h:16-20`; `include/patterns/InputHandler.h:40-80`; `src/patterns/InputHandler.cpp:10-95`; `src/states/PlayState.cpp:339-380` |
| Observer/event lifetime | `include/patterns/EventBus.h:36-57`; `include/patterns/Subscription.h:23-48`; `src/patterns/EventBus.cpp:210-271` |
| State game/Mario | `include/states/IGameState.h:19-35`; `include/states/IMarioState.h:17-33`; `src/entities/Mario.cpp:937-989` |
| Factory Method | `include/patterns/EntityFactory.h:20-50`; `include/patterns/EntityCreator.h:14-20`; `src/patterns/EntityFactory.cpp:18-59`; `src/level/Level.cpp:625-677` |
| Collision → event | `src/physics/ContactListener.cpp:14-25`; `src/physics/CollisionManager.cpp:724-763`; `src/physics/CollisionManager.cpp:522-595` |
| Save ownership correction | `include/core/SaveManager.h:22-40`; `include/core/GameManager.h:42-42`; `include/core/GameManager.h:72-74` |

## Glossary ngắn

- **Abstraction / trừu tượng:** hợp đồng giữ lại điều client cần dùng.
- **Encapsulation / đóng gói:** giấu state/invariant, truy cập qua method.
- **Inheritance / kế thừa:** quan hệ “là một”, class con nhận contract class cha.
- **Polymorphism / đa hình:** gọi cùng base API nhưng chạy implementation thật.
- **Composition:** object chứa object khác; thường đi cùng ownership.
- **Singleton:** class có một instance truy cập toàn cục; dùng có cân nhắc vì
  dependency bị ẩn và test khó thay service.
- **Factory Method:** creator abstraction chọn và tạo product concrete.
- **Command:** request được đóng gói thành object có `execute()`.
- **Observer:** subject phát notification cho nhiều observer đã đăng ký.
- **State:** object hiện tại thay đổi cách xử lý/lifecycle của context.
- **RAII:** lifetime object tự quản lý resource/token.
- **Non-owning:** con trỏ/tham chiếu chỉ quan sát; không được tự hủy object.
- **Deferred operation:** ghi yêu cầu trước, apply tại safe point đã định.

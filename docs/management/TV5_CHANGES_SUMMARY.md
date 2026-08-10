# Tổng Kết Những Thay Đổi Của TV5 — Sprint 6

> **Phạm vi:** Snapshot của các thay đổi TV5 và phần integration
> SpriteFrames/LevelTheme/TileMap đang có trong working tree hiện tại. Tài liệu
> này không dùng để kết luận toàn bộ Definition of Done của Sprint 6 hoặc phân
> định ownership giữa TV1–TV5.
>
> **Mốc kiểm tra:** `HEAD 59b3d6e27c1d20dd589741aba287a13b807b5037 + working tree`,
> ngày `2026-08-10`.
>
> **Verification đã xác nhận:** build Debug `build-codex` thành công; CTest
> `13/13` pass; `git diff --check` pass. Đây là snapshot chưa commit release.

---

## 1. Input và Command

### Shift chỉ chạy, X chỉ bắn

Gameplay input được khóa lại theo đúng trigger:

- `LShift`/`RShift` tạo `RunCommand` với trigger `Held`.
- `X` tạo `ShootCommand` với trigger `Pressed`.
- Input bị suppression khi gameplay đang pause, death hoặc transition; không
  được buffer để chạy sau khi state hoạt động lại.

`InputHandler::handleInput()` nhận cờ `gameplayEnabled`, giúp state owner kiểm
soát rõ thời điểm input được xử lý. Integration tests bao phủ simultaneous press,
held repeat, release, X auto-repeat và suppression.

### File liên quan

- [`include/patterns/InputHandler.h`](../../include/patterns/InputHandler.h)
- [`src/patterns/InputHandler.cpp`](../../src/patterns/InputHandler.cpp)
- [`src/states/PlayState.cpp`](../../src/states/PlayState.cpp)
- [`tests/TV5IntegrationTests.cpp`](../../tests/TV5IntegrationTests.cpp)

---

## 2. Items, power-up và score

### Adaptive QuestionBlock và FireFlower

- Factory tạo tile `?` với `QuestionBlockContent::ADAPTIVE`.
- Khi hit lần đầu, `SMALL` resolve thành Mushroom; `SUPER`/`FIRE` resolve
  thành FireFlower.
- Content đã resolve không đổi khi block bị hit lại.
- FireFlower chuyển Mario sang state `FIRE`; pickup lặp không cộng thêm score.

### Star và coin

- Star có timed invincibility 10 giây và phát đúng một event
  `PLAYER_INVINCIBILITY_EXPIRED` khi hết hạn.
- Coin giữ phần dư khi đạt ngưỡng 100, cộng một life và phát đúng một
  `ONE_UP_COLLECTED` cho mỗi life được tạo.
- Damage grace của Mario độc lập với thời gian Star.

### Shared score catalog

`ScoreRules` dùng `DefeatCause` làm catalog pure cause → points:

| Cause/event | Điểm |
|---|---:|
| Coin | 100 |
| Power-up | 1000 |
| Stomp | 100 |
| Shell / FireBall / Star | 200 |
| Pit | 0 |

Runtime defeat đi qua một operation duy nhất là
`CollisionManager::defeatEnemy()`. Operation này commit victim một lần, gọi
callback gameplay, cộng score cho owner và phát event theo cause. Latch ở
`Enemy` ngăn score/event lặp khi contact còn tồn tại; owner của shell được giữ
từ Mario đã kick shell.

`STOMP` là transaction riêng vì lần stomp đầu chuyển Koopa sang shell. Shell
kick phát `SHELL_KICKED`; shell/fireball/star defeat phát lần lượt
`ENEMY_DEFEATED_BY_SHELL`, `ENEMY_DEFEATED_BY_FIREBALL` và
`ENEMY_DEFEATED_BY_STAR`.

### File liên quan

- [`src/patterns/EntityFactory.cpp`](../../src/patterns/EntityFactory.cpp)
- [`src/items/Coin.cpp`](../../src/items/Coin.cpp)
- [`include/core/ScoreRules.h`](../../include/core/ScoreRules.h)
- [`src/core/ScoreRules.cpp`](../../src/core/ScoreRules.cpp)
- [`include/physics/CollisionManager.h`](../../include/physics/CollisionManager.h)
- [`src/physics/CollisionManager.cpp`](../../src/physics/CollisionManager.cpp)
- [`include/entities/Enemy.h`](../../include/entities/Enemy.h)
- [`src/entities/Mario.cpp`](../../src/entities/Mario.cpp)
- [`tests/TV5IntegrationTests.cpp`](../../tests/TV5IntegrationTests.cpp)

---

## 3. HUD và timeout

- HUD chỉ hiển thị trạng thái `STAR` khi Star còn hiệu lực; sau expiry trở về
  state thật `SMALL`/`SUPER`/`FIRE`.
- Timer mặc định là 400 giây; warning threshold chỉ được gọi một lần.
- Timeout bị vô hiệu hóa sau timeout, death hoặc level completion; callback
  death không cạnh tranh với transition/flag đang pending.
- Runtime không còn fallback font tuyệt đối `C:/Windows/Fonts/...`; dùng
  `assets/fonts/mario.ttf` và degrade có kiểm soát nếu font thiếu.

### File liên quan

- [`include/ui/HUD.h`](../../include/ui/HUD.h)
- [`src/ui/HUD.cpp`](../../src/ui/HUD.cpp)
- [`src/states/PlayState.cpp`](../../src/states/PlayState.cpp)
- [`tests/TV5IntegrationTests.cpp`](../../tests/TV5IntegrationTests.cpp)

---

## 4. Audio và persistence

- Star music override khôi phục đúng track level khi nhận event expiry.
- Damage-grace expiry không tự cắt death/GameOver music khi không có Star
  override.
- Sound/SFX volume mặc định đồng bộ với `SaveData` (`80`/`70`) và clamp trong
  `[0,100]`.
- Pause điều chỉnh Music và SFX độc lập, phản hồi ngay và lưu qua
  `GameManager`/`SaveManager` dùng chung.
- Composition root nạp lại hai volume từ SaveManager khi khởi động.
- `SoundManager` tách hai logical cue `shell_kick`/`shell_kill`; defeat của
  fireball/star cũng có cue riêng. Sample `kickkill.wav` được tái sử dụng cho
  các cue này cho tới khi có recording chuyên biệt.
- Resume đi qua `GAME_RESUMED`; death/GameOver/Win và track Level 1–3 được
  kiểm tra qua state lifecycle thật.

### File liên quan

- [`include/core/SoundManager.h`](../../include/core/SoundManager.h)
- [`src/core/SoundManager.cpp`](../../src/core/SoundManager.cpp)
- [`src/core/Game.cpp`](../../src/core/Game.cpp)
- [`src/states/PauseState.cpp`](../../src/states/PauseState.cpp)
- [`tests/TV5IntegrationTests.cpp`](../../tests/TV5IntegrationTests.cpp)

---

## 5. Asset package và manifest

- Atlas phân tích/source được phân loại khỏi runtime package; asset reference
  nằm ngoài đường copy runtime.
- `assets/ASSETS_LIST.md` ghi rõ kích thước và nhãn `Runtime`, `Reference` hoặc
  `Future` theo PNG thật.
- `enemies.png` là atlas runtime cho Goomba/Koopa; `goomba.png` và `koopa.png`
  là standalone sheet `Future`, chưa có loader runtime riêng.
- `items_objects.png` là atlas runtime của Mushroom, Coin, FireFlower và Star;
  `items_blocks.png` là atlas runtime của QuestionBlock.
- `assets/ui/bg_world.png` là background runtime hiện tại; các background
  legacy chưa có caller runtime giữ ở trạng thái `Future`.
- Kích thước asset được khóa trong integration tests:
  - tileset `680×776`
  - enemies `436×530`
  - Goomba `96×32`
  - Koopa `128×48`
  - items blocks `448×256`
  - items objects `592×572`

### File liên quan

- [`assets/ASSETS_LIST.md`](../../assets/ASSETS_LIST.md)
- [`FILE_STRUCTURE.md`](../../FILE_STRUCTURE.md)
- [`docs/blocks_coordinate.md`](../blocks_coordinate.md)
- [`docs/items_objects_coordinate.md`](../items_objects_coordinate.md)
- [`docs/enemies_coordinate.md`](../enemies_coordinate.md)
- `docs/assets/reference/` — atlas reference ngoài runtime package

---

## 6. Tách SpriteFrames theo shared và theme

### Kiến trúc mới

`include/core/SpriteFrames.h` không còn là catalog monolithic chứa toàn bộ
tọa độ. Nó trở thành compatibility facade và nạp các catalog nhỏ hơn:

- `SpriteFrames_shared.h` chứa catalog `shared` cho Mario/Luigi, animation
  chung và item dùng chung; đồng thời giữ `legacy` raw atlas catalog cho các
  caller cũ.
- `SpriteFrames_ovw.h`, `SpriteFrames_udg.h`, `SpriteFrames_castle.h` và
  `SpriteFrames_udw.h` cung cấp API normalized theo palette/theme.
- Code mới dùng `SpriteFrames::shared`, `SpriteFrames::ovw`,
  `SpriteFrames::udg`, `SpriteFrames::castle` hoặc `SpriteFrames::udw`.
- Alias trong `SpriteFrames.h` vẫn giữ các tên legacy như
  `SpriteFrames::Items`, `SpriteFrames::Blocks` và `SpriteFrames::Enemies`.

### Goomba và Koopa

- Goomba có namespace `Enemies::Goomba` trong từng theme.
- Koopa được tách thành namespace `Enemies::Koopa` trong từng theme, giống
  cách tổ chức của Goomba.
- Tọa độ canonical của Koopa nằm tại `legacy::Enemies::Koopa`; không còn một
  bộ tọa độ Koopa thứ hai độc lập.
- `legacy::Enemies::GreenEnemiesBlock` chỉ giữ alias tương thích cho Koopa và
  các frame Paratroopa theo physical atlas block.
- Runtime Koopa dùng các frame hiện tại:
  - Overworld: Walk `(0,112)`, `(18,112)`, Shell `(72,120)`.
  - Underground: Walk `(146,112)`, `(164,112)`, Shell `(218,120)`.
  - Castle: alias palette Underground.
  - Underwater catalog có namespace riêng để kiểm tra geometry; release
    `LevelTheme` hiện tại gồm Overworld, Underground và Castle.
- Atlas không có hình riêng cho shell đang bị kick; `SHELL_KICKED` alias
  `SHELL_RETRACTED`, còn trạng thái kick thay đổi state/velocity.

### Runtime migration

- `Goomba` và `Koopa` constructor nhận `LevelTheme`.
- `EntityFactory` nhận theme và truyền theme vào enemy.
- `Goomba.cpp`/`Koopa.cpp` chọn animation từ namespace theme tương ứng.
- Mario, FireBall, Coin, FireFlower và Star include `SpriteFrames_shared.h`.
- QuestionBlock dùng catalog `Blocks` của theme tương ứng.
- Mushroom, BlockDebris và background Overworld dùng catalog theme nhỏ nhất
  cần thiết thay vì facade monolithic.

### File liên quan

- [`include/core/SpriteFrames.h`](../../include/core/SpriteFrames.h)
- [`include/core/SpriteFrames_shared.h`](../../include/core/SpriteFrames_shared.h)
- [`include/core/SpriteFrames_ovw.h`](../../include/core/SpriteFrames_ovw.h)
- [`include/core/SpriteFrames_udg.h`](../../include/core/SpriteFrames_udg.h)
- [`include/core/SpriteFrames_castle.h`](../../include/core/SpriteFrames_castle.h)
- [`include/core/SpriteFrames_udw.h`](../../include/core/SpriteFrames_udw.h)
- [`include/entities/Goomba.h`](../../include/entities/Goomba.h)
- [`include/entities/Koopa.h`](../../include/entities/Koopa.h)
- [`src/entities/Goomba.cpp`](../../src/entities/Goomba.cpp)
- [`src/entities/Koopa.cpp`](../../src/entities/Koopa.cpp)
- [`src/entities/Mario.cpp`](../../src/entities/Mario.cpp)
- [`src/entities/FireBall.cpp`](../../src/entities/FireBall.cpp)
- [`src/items/Coin.cpp`](../../src/items/Coin.cpp)
- [`src/items/FireFlower.cpp`](../../src/items/FireFlower.cpp)
- [`src/items/Mushroom.cpp`](../../src/items/Mushroom.cpp)
- [`src/items/Star.cpp`](../../src/items/Star.cpp)
- [`src/entities/QuestionBlock.cpp`](../../src/entities/QuestionBlock.cpp)

---

## 7. Theme-aware Level và TileMap

### Theme propagation

- `Level` lưu `m_theme` và đồng bộ theme xuống `TileMap`.
- Khi spawn entity, `Level` truyền theme hiện tại vào
  `EntityFactory::createFromTileCode()`.
- `EntityFactory` map `LevelTheme` sang `BlockTheme` khi tạo QuestionBlock.
- `TileMap` khởi tạo `m_theme` mặc định là `OVERWORLD` để không còn trạng thái
  chưa khởi tạo.

### Tileset và background

- `TileFrames.h` cập nhật các frame ground, brick, question, used block, pipe
  và flag/pole theo tọa độ tileset hiện tại.
- Bổ sung các frame Underground cho used block, pipe và finish pole.
- `TileMap::getTilesetRect()` chọn frame Underground khi level dùng theme
  Underground; các theme còn lại giữ frame phù hợp của catalog hiện tại.
- Debris của block chọn theo palette Overworld, Underground hoặc Castle thay vì
  luôn dùng frame Overworld.
- Sửa phép tính vertex để dùng tọa độ world đã tính (`x`, `y`) nhất quán với
  tile offset và camera.
- Level Underground render nền xanh đậm kéo dài theo level bounds; các theme
  khác tiếp tục dùng background `ovw::Backgrounds::WORLD` lặp/mirror.

### File liên quan

- [`include/level/Level.h`](../../include/level/Level.h)
- [`src/level/Level.cpp`](../../src/level/Level.cpp)
- [`include/level/TileFrames.h`](../../include/level/TileFrames.h)
- [`include/level/TileMap.h`](../../include/level/TileMap.h)
- [`src/level/TileMap.cpp`](../../src/level/TileMap.cpp)
- [`include/patterns/EntityFactory.h`](../../include/patterns/EntityFactory.h)
- [`src/patterns/EntityFactory.cpp`](../../src/patterns/EntityFactory.cpp)
- [`include/entities/BlockDebris.h`](../../include/entities/BlockDebris.h)

---

## 8. Regression tests và verification

### Test coverage

`tests/TV5IntegrationTests.cpp` tiếp tục bao phủ:

- Input trigger, repeat, release và suppression.
- FireFlower, adaptive block, Star expiry/HUD và one-up threshold.
- Score catalog theo `DefeatCause`.
- HUD timer pause/timeout/death/flag guard.
- Runtime defeat score/event path với Box2D contact thật, shell kick/kill không
  lặp và event-to-SFX mapping.
- Level 1–3 music transition, death/GameOver/Win, pause/resume, Star restore và
  volume persistence qua state/audio lifecycle.
- Asset dimensions, manifest content và runtime/reference classification.

`tests/SpriteFramesThemeTests.cpp` là test mới cho kiến trúc catalog:

- Compile được độc lập với `SpriteFrames_shared.h` và bốn theme headers.
- Kiểm tra namespace Goomba/Koopa của Overworld, Underground, Castle và
  Underwater.
- Kiểm tra facade legacy vẫn truy cập được.
- Kiểm tra tọa độ Koopa Underground/Castle, frame geometry và alias
  `GreenEnemiesBlock`.

Test được đăng ký thành target `sprite_frames_theme_tests` trong
`CMakeLists.txt`, nâng tổng số CTest hiện tại lên `13`.

### Verification snapshot

Các lệnh đã chạy thành công trong working tree này:

```powershell
cmake --build build-codex --config Debug
ctest --test-dir build-codex --output-on-failure -C Debug
git diff --check
```

Kết quả:

```text
SuperMario và các test target Debug build thành công.
100% tests passed, 0 tests failed out of 13
git diff --check: pass
```

Các thư mục build cũ `build-release` và `build-tests` không được dùng để cập
nhật kết luận release trong snapshot này vì CMake bị `Permission denied` khi
reconfigure và ghi file cache vào hai thư mục đó. Do đó tài liệu chỉ khẳng định
verification Debug/CTest ở trên.

Phần kiểm tra thiết bị audio không thể chứng minh bằng headless CTest được ghi
trong [TV5 audio/HUD/item checklist](TV5_AUDIO_HUD_ITEM_CHECKLIST.md); checklist
đó tách automated evidence khỏi các bước manual-only.

---

## 9. Trạng thái snapshot

Working tree hiện có cả thay đổi tracked và các file mới chưa commit, gồm
catalog SpriteFrames theo theme, test `sprite_frames_theme_tests`, tài liệu
đánh giá và kế hoạch Sprint 6. Summary này mô tả trạng thái code đã kiểm tra ở
`HEAD + working tree`; không phải release commit và không phải tuyên bố Sprint 6
đã hoàn thành toàn bộ DoD.

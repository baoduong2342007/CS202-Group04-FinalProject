# Tổng Kết Những Thay Đổi Của TV5 — Sprint 6

> **Phạm vi:** Chỉ ghi các thay đổi TV5 được thực hiện trong lượt fix này.
> Không dùng file này để kết luận task của TV1, TV2, TV3 hoặc TV4.
> **Trạng thái:** Debug/Release/Tests build sạch; CTest `10/10`.
> **Mốc kiểm tra:** `HEAD 59df1a6 + working tree`, ngày `2026-08-09`.

---

## 1. Input và Command

### Shift chỉ chạy, X chỉ bắn

Các binding gameplay trong `PlayState` được khóa lại:

- `LShift`/`RShift` → `RunCommand`, trigger `Held`.
- `X` → `ShootCommand`, trigger `Pressed`.
- Input bị loại bỏ khi gameplay đang pause, death hoặc transition; không được
  buffer để chạy sau khi state hoạt động lại.

`InputHandler::handleInput()` nhận cờ `gameplayEnabled` để state owner có thể
suppression rõ ràng. Bộ test kiểm tra simultaneous press, held repeat, release,
X auto-repeat và suppression.

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
- Content đã resolve không bị đổi khi hit lại.
- FireFlower contract được regression-test theo state release `FIRE`; pickup
  lặp không cộng thêm score.

### Star và 100 coin

- Star được kiểm tra theo timed invincibility 10 giây, phát đúng một event
  `PLAYER_INVINCIBILITY_EXPIRED` khi hết hạn.
- Coin đạt ngưỡng 100 giữ remainder, cộng một life và phát đúng một
  `ONE_UP_COLLECTED` cho mỗi life được tạo.
- Mario giữ damage-grace timer độc lập khi Star hết hạn.

### Shared score catalog

`ScoreRules` được mở rộng với
`DefeatCause::{STOMP, SHELL, FIREBALL, STAR, PIT}`:

| Cause/event | Điểm |
|---|---:|
| Coin | 100 |
| Power-up | 1000 |
| Stomp | 100 |
| Shell / FireBall / Star | 200 |
| Pit | 0 |

Thêm `ScoreRules::awardDefeat()` và regression assertions cho toàn bộ catalog.

### File liên quan

- [`src/patterns/EntityFactory.cpp`](../../src/patterns/EntityFactory.cpp)
- [`src/items/Coin.cpp`](../../src/items/Coin.cpp)
- [`include/core/ScoreRules.h`](../../include/core/ScoreRules.h)
- [`src/core/ScoreRules.cpp`](../../src/core/ScoreRules.cpp)
- [`src/entities/Mario.cpp`](../../src/entities/Mario.cpp)
- [`tests/TV5IntegrationTests.cpp`](../../tests/TV5IntegrationTests.cpp)

---

## 3. HUD và timeout

- HUD chỉ hiển thị `STAR` trong thời gian Star còn hiệu lực; sau expiry trở về
  state thật `SMALL`/`SUPER`/`FIRE`.
- Timer mặc định là 400 giây, warning qua threshold chỉ gọi một lần.
- Timeout được vô hiệu hóa sau timeout, death hoặc level completion; callback
  death không cạnh tranh với transition/flag đang pending.
- Xóa fallback font tuyệt đối `C:/Windows/Fonts/...`; runtime chỉ dùng
  `assets/fonts/mario.ttf` và degrade có kiểm soát nếu font thiếu.

### File liên quan

- [`include/ui/HUD.h`](../../include/ui/HUD.h)
- [`src/ui/HUD.cpp`](../../src/ui/HUD.cpp)
- [`src/states/PlayState.cpp`](../../src/states/PlayState.cpp)
- [`tests/TV5IntegrationTests.cpp`](../../tests/TV5IntegrationTests.cpp)

---

## 4. Audio và persistence

- Star music override khôi phục đúng track level khi nhận event expiry.
- Damage-grace expiry không tự ý cắt death/GameOver music khi không có Star
  override.
- Sound/SFX volume mặc định đồng bộ với `SaveData` (`80`/`70`) và luôn clamp
  trong `[0,100]`.
- Pause điều chỉnh Music và SFX độc lập, phản hồi ngay và lưu qua một
  `GameManager`/`SaveManager` dùng chung.
- Composition root nạp lại hai volume từ SaveManager khi khởi động.

### File liên quan

- [`include/core/SoundManager.h`](../../include/core/SoundManager.h)
- [`src/core/SoundManager.cpp`](../../src/core/SoundManager.cpp)
- [`src/core/Game.cpp`](../../src/core/Game.cpp)
- [`src/states/PauseState.cpp`](../../src/states/PauseState.cpp)
- [`tests/TV5IntegrationTests.cpp`](../../tests/TV5IntegrationTests.cpp)

---

## 5. Asset package và manifest

- Các atlas phân tích/source được chuyển khỏi `assets/` sang
  `docs/assets/reference/`, nên không còn bị CMake đóng gói vào runtime.
- `assets/ASSETS_LIST.md` được đối chiếu theo PNG thật và ghi rõ Runtime,
  Reference hoặc Future.
- Kích thước runtime được khóa trong test:
  - tileset `680×776`
  - enemies `436×530`
  - Goomba `96×32`
  - Koopa `128×48`
  - items blocks `448×256`
  - items objects `592×572`
- Cập nhật link atlas trong tài liệu crop và cấu trúc file.

### File liên quan

- [`assets/ASSETS_LIST.md`](../../assets/ASSETS_LIST.md)
- [`FILE_STRUCTURE.md`](../../FILE_STRUCTURE.md)
- [`docs/blocks_coordinate.md`](../blocks_coordinate.md)
- [`docs/items_objects_coordinate.md`](../items_objects_coordinate.md)
- `assets/textures/enemies/enemies_all_components_atlas.png` (đã chuyển ra
  khỏi runtime)
- `assets/textures/items/blocks_all_components_atlas.png` (đã chuyển ra khỏi
  runtime)
- `assets/textures/items/items_objects_all_components_atlas.png` (đã chuyển ra
  khỏi runtime)

---

## 6. Regression tests và verification

`tests/TV5IntegrationTests.cpp` được mở rộng để bao phủ:

- Input trigger, repeat, release và suppression.
- FireFlower, adaptive block, Star expiry/HUD và one-up threshold.
- Score catalog theo `DefeatCause`.
- HUD timer pause/timeout/death/flag guard.
- Level music catalog, SFX catalog, Star restore và volume persistence.
- Asset dimensions, manifest content và runtime/reference classification.

Verification đã chạy:

```powershell
cmake --fresh --preset mingw-debug
cmake --build --preset mingw-debug --parallel 2

cmake --fresh --preset mingw-release
cmake --build --preset mingw-release --parallel 2

cmake --fresh --preset mingw-tests
cmake --build --preset mingw-tests --parallel 2
ctest --preset mingw-tests --output-on-failure
```

Kết quả: `100% tests passed, 0 tests failed out of 10`. `git diff --check`
cũng pass. File này là nguồn tổng kết duy nhất của TV5 cho lượt fix này.

# Danh sách Assets — BẢN CẬP NHẬT (Spritesheet Chung)

> **📌 QUYẾT ĐỊNH NHÓM (CẬP NHẬT):**
> Toàn bộ đồ họa giữ dưới dạng **spritesheet chung**, **KHÔNG tách** thành các file PNG riêng lẻ
> (không tạo `idle.png`, `walk.png`, `coin.png`, `mushroom.png`...).
> Code dùng `sf::Sprite::setTextureRect()` và `AnimationSystem::createGridAnimation()`
> để cắt frame từ sheet chung. Mọi thành viên khi code phải load sheet qua
> `TextureManager` (1 lần) và cắt frame bằng **named constants** — không hardcode magic number.
>
> *File này thay thế `ASSETS_LIST.md` cũ. Giữ bản cũ để đối chiếu.*

---

## 1. Đồ họa & Textures (`assets/textures/`)

Tất cả tệp hình ảnh dùng định dạng `.png` (hỗ trợ kênh Alpha), phong cách Pixel Art NES/SNES.
**Trạng thái `✅` = file ĐÃ CÓ trong repo. `❌` = CẦN BỔ SUNG.**

### 1.1. Nhân vật Mario (`assets/textures/mario/`)
| Sheet | Mô tả & Quy ước cắt | Trạng thái |
| :--- | :--- | :---: |
| `assets/textures/mario/MarioLuigi.png` | **Sheet chung duy nhất cho Mario & Luigi.** Chứa mọi trạng thái: SMALL/SUPER/FIRE × idle/walk/jump/death, mọi hướng (trái/phải). Cắt frame theo grid NES (frame gốc 16×16, scale lên 32×32 trong game). Code dùng `setTextureRect()` để cắt frame — **KHÔNG tách file riêng**. | ✅ Có sẵn |

> ⚠️ **Nhắc code:** `Mario.cpp` hiện gọi `setSprite("assets/textures/mario/idle.png")` — path KHÔNG tồn tại → Mario hiện là khối magenta. Cần sửa thành load sheet trên + `setTextureRect` frame đúng (phối hợp TV2 - AnimationSystem).

### 1.2. Kẻ địch (`assets/textures/enemies/`)
| Sheet | Mô tả & Quy ước cắt | Trạng thái |
| :--- | :--- | :---: |
| `assets/textures/enemies/goomba.png` | Goomba — 3 frame ngang 32×32: walk ×2 (0–64px) + squish (64–96px). | ✅ Có sẵn (tạm thời) |
| `assets/textures/enemies/enemies.png` | **Sheet chung nhiều enemy** — chứa Goomba, Koopa, v.v. Dùng làm nguồn cắt cho **Koopa** khi TV4 implement (Sprint 5–6). | ✅ Có sẵn |

### 1.3. Ô bản đồ & Gạch đá (`assets/textures/tiles/`)
| Sheet | Mô tả & Quy ước cắt | Trạng thái |
| :--- | :--- | :---: |
| `assets/textures/tiles/tileset.png` | **Tileset chính.** Layout chuẩn: 4 tile ngang × 32px/tile → **kích thước 128×32**. | ✅ Có sẵn |
| | Vị trí tile (KHỚP `TileMap.cpp`): `[0]=ground` (đất), `[1]=brick` (gạch), `[2]=?` (khối hỏi chấm), `[3]=F` (cờ đích/flag). | |

### 1.4. Vật phẩm (`assets/textures/items/`)
| Sheet | Mô tả & Quy ước cắt | Trạng thái |
| :--- | :--- | :---: |
| `assets/textures/items/items_objects.png` | **Sheet vật phẩm/đối tượng** — chứa **Coin, FireFlower, Star**... Cắt frame theo grid. | ✅ Có sẵn |
| `assets/textures/items/items_blocks.png` | **Sheet item + block** — chứa **Mushroom (Nấm đỏ)**, block items... Cắt frame theo grid. | ✅ Có sẵn |

> ⚠️ **Nhắc code:** `Coin/Mushroom/FireFlower/Star` hiện gọi `setSprite(...coin.png / mushroom.png / fireflower.png / star.png)` — các path KHÔNG tồn tại → item hiện là khối magenta. Cần sửa load sheet chung bên trên + `setTextureRect`.

### 1.5. Giao diện người dùng (`assets/textures/ui/`)
| File | Mô tả | Trạng thái |
| :--- | :--- | :---: |
| `assets/textures/ui/hud.png` | Texture HUD (icon, khung hiển thị). | ✅ Có sẵn |
| `assets/textures/ui/bg_clouds.png` | Nền mây — dùng cho menu / background. | ✅ Có sẵn |
| `assets/textures/ui/bg_mountains.png` | Nền núi — dùng cho menu / background. | ✅ Có sẵn |
| `assets/textures/ui/bg_trees.png` | Nền cây — dùng cho menu / background. | ✅ Có sẵn |

---

## 2. Hiệu ứng âm thanh (`assets/sounds/effects/`)

Tất cả **ĐÃ CÓ SẴN** trong repo (định dạng `.wav`). Cột "Kích hoạt" ghi rõ ràng để bind EventBus.

| File | Tình huống kích hoạt | Trạng thái |
| :--- | :--- | :---: |
| `jump.wav` / `jumpsmall.wav` | Mario nhảy (lớn / nhỏ). | ✅ Có sẵn |
| `coin.wav` | Ăn đồng xu. | ✅ Có sẵn |
| `death.wav` | Mario mất mạng. | ✅ Có sẵn |
| `powerup.wav` | Ăn Nấm / Hoa lửa / Sao. | ✅ Có sẵn |
| `pipepowerdown.wav` | Bị hạ cấp (power down). | ✅ Có sẵn |
| `stompswim.wav` | Giẫm/kill enemy. | ✅ Có sẵn |
| `kickkill.wav` | Đá mai rùa Koopa tiêu diệt enemy. | ✅ Có sẵn |
| `fireball.wav` | Mario bắn cầu lửa. | ✅ Có sẵn |
| `flagpole.wav` | Chạm cờ đích (kết thúc level). | ✅ Có sẵn |
| `gameover.wav` / `gameoverunused.wav` | Game Over. | ✅ Có sẵn |
| `brick.wav` | Phá gạch. | ✅ Có sẵn |
| `bump.wav` | Va vào block từ dưới. | ✅ Có sẵn |
| `pause.wav` | Tạm dừng game. | ✅ Có sẵn |
| `1up.wav` | Được mạng (1-UP). | ✅ Có sẵn |
| `item.wav` | Item xuất hiện từ block. | ✅ Có sẵn |
| `fire.wav`, `vine.wav`, `hurryup.wav`, `billfirework.wav`, `bowserfall.wav`, `beep.wav` | SFX phụ (tùy chọn). | ✅ Có sẵn |

> Đã bind trong `SoundManager.cpp`: jump, coin, stomp, death, powerup, powerdown, pause. **Còn cần bind thêm (Sprint 5–6):** `kick.wav` (Koopa), `fireball.wav` (FireBall), `flagpole.wav` (hoàn thành level).

---

## 3. Nhạc nền màn chơi (`assets/sounds/music/`)

Tất cả **ĐÃ CÓ SẴN** — định dạng **`.flac`** (SFML `sf::Music` streaming, loop được).
> ❗ Lưu ý: `ASSETS_LIST.md` cũ ghi `.ogg` — **bản cập nhật này dùng `.flac`** vì là file thực tế trong repo.

| File | Kích hoạt | Trạng thái |
| :--- | :--- | :---: |
| `overworld.flac` | Nhạc nền level ngoài trời (World 1-1). | ✅ Có sẵn |
| `underground.flac` | Nhạc nền level dưới hang (Level 2). | ✅ Có sẵn |
| `castle.flac` | Nhạc nền level lâu đài (Level 3). | ✅ Có sẵn |
| `level_complete.flac` | Hoàn thành level (flag). | ✅ Có sẵn |
| `gameover.flac` | Game Over. | ✅ Có sẵn |
| `death.flac` | Mario chết. | ✅ Có sẵn |
| `invincible.flac` | Trạng thái Bất tử (Starman). | ✅ Có sẵn |
| `pipe.flac` | Vào ống (pipe). | ✅ Có sẵn |
| `underwater.flac` | Nhạc nền dưới nước (tùy chọn). | ✅ Có sẵn |
| `castle_complete.flac` | Hoàn thành lâu đài. | ✅ Có sẵn |

> ⚠️ **Nhắc code:** `SoundManager::playMusic()` CHƯA được gọi ở đâu → game hiện không có nhạc nền. Cần gọi trong `PlayState` (+ `stopMusic()`/`pauseMusic()` khi đổi state).

---

## 4. Phông chữ (`assets/fonts/`)

| File | Mô tả | Trạng thái |
| :--- | :--- | :---: |
| `assets/fonts/mario.ttf` | Font pixel retro (Press Start 2P) cho HUD, Menu, GameOver, Win, Pause. | ❌ **CẦN BỔ SUNG** |

> ⚠️ **Ảnh hưởng:** HUD có font fallback C:/Windows/Fonts nên vẫn hiện được. Nhưng `MenuState / GameOverState / WinState / PauseState` **không có fallback** → nếu thiếu font, chữ màn menu/won/lose có thể không hiển thị. **Ưu tiên bổ sung file này.**

---

## 5. Quy ước code khi dùng spritesheet chung (cho mọi thành viên)

1. **Load 1 lần** qua `TextureManager::loadTexture(path, path)` — key = đường dẫn sheet.
2. **Cắt frame** bằng `sf::Sprite::setTextureRect(sf::IntRect(...))` hoặc `AnimationSystem::createGridAnimation(...)`.
3. **Named constants cho frame rect** — đặt trong `namespace {}` của file `.cpp` (theo `CODING_RULES.md`), KHÔNG hardcode magic number trong logic.
4. **Không tạo file PNG riêng** cho từng state/sprite/item — chỉ dùng sheet chung.
5. **Path luôn tương đối** (không dùng `C:\...` hoặc `../`) để build tái lập được (`cmake .. && cmake --build .`).

---

## 6. Nguồn tải tham khảo (chỉ dùng khi CẦN BỔ SUNG — hiện đã có gần hết)

* **Đồ họa & Sprites:** [The Spriters Resource](https://www.spriters-resource.com/nes/supermariobros/) · [Itch.io Pixel Art](https://itch.io/game-assets/free/tag-pixel-art) · [OpenGameArt](https://opengameart.org/)
* **Âm thanh:** [The Sounds Resource](https://www.sounds-resource.com/nes/supermariobros/) · [Freesound](https://freesound.org/)
* **Phông chữ:** [Google Fonts - Press Start 2P](https://fonts.google.com/specimen/Press+Start+2P) · [Dafont Pixel](https://www.dafont.com/theme.php?cat=501)

---

## 7. Bảng đối chiếu nhanh: bản cũ (tách file) vs bản mới (sheet chung)

| Loại | Bản cũ (`ASSETS_LIST.md`) | Bản mới (file này) |
| :--- | :--- | :--- |
| Mario | 7 file riêng (`idle.png`, `walk.png`...) | 1 sheet chung `MarioLuigi.png` ✅ |
| Goomba | `goomba.png` | `goomba.png` ✅ (dùng luôn) |
| Koopa | `koopa.png` (❌ cần tìm) | `enemies.png` ✅ sheet chung (cắt frame Koopa) |
| Items | 4 file riêng (`coin.png`, `mushroom.png`...) | 2 sheet chung `items_objects.png` + `items_blocks.png` ✅ |
| Tiles | `tileset.png` | `tileset.png` ✅ (ghi rõ layout 4 tile) |
| UI | `hud_icons.png`, `menu_bg.png` (❌) | `hud.png`, `bg_clouds.png`, `bg_mountains.png`, `bg_trees.png` ✅ |
| SFX | 6 file "cần tìm" | ~23 file ✅ (kèm bảng bind EventBus) |
| Music | 3 file `.ogg` "cần tìm" | 10 file `.flac` ✅ |
| Font | `mario.ttf` (❌) | `mario.ttf` ❌ **vẫn cần bổ sung** |
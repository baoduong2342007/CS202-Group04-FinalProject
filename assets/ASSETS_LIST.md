# Danh sách Assets cần tìm kiếm & chuẩn bị

Dưới đây là danh sách chi tiết các tài nguyên (assets) cần thu thập cho dự án Super Mario Bros. Danh sách này được phân chia theo cấu trúc thư mục quy định trong [FILE_STRUCTURE.md](../FILE_STRUCTURE.md) nhằm hỗ trợ TV5 (Truyền) dễ dàng quản lý và tích hợp vào mã nguồn.

---

## 1. Đồ họa & Textures (`assets/textures/`)

Tất cả các tệp hình ảnh nên sử dụng định dạng `.png` để hỗ trợ kênh trong suốt (Alpha channel). Nên ưu tiên phong cách Pixel Art phù hợp với phiên bản NES/SNES.

### 1.1. Nhân vật Mario (`assets/textures/mario/`)
| Đường dẫn tệp | Mô tả chi tiết | Trạng thái |
| :--- | :--- | :---: |
| `assets/textures/mario/idle.png` | Ảnh Mario nhỏ đứng yên nhìn về phía trước/phải. | `[ ]` Cần tìm |
| `assets/textures/mario/walk.png` | Sprite sheet chứa các khung hình di chuyển của Mario nhỏ (dạng hoạt ảnh cắt ngang). | `[ ]` Cần tìm |
| `assets/textures/mario/jump.png` | Ảnh Mario nhỏ ở trạng thái nhảy trên không. | `[ ]` Cần tìm |
| `assets/textures/mario/big_idle.png` | Ảnh Mario lớn (sau khi ăn Nấm) ở trạng thái đứng yên. | `[ ]` Cần tìm |
| `assets/textures/mario/big_walk.png` | Sprite sheet chứa các khung hình di chuyển của Mario lớn. | `[ ]` Cần tìm |
| `assets/textures/mario/fire_idle.png` | Ảnh Mario lửa (sau khi ăn Hoa lửa) ở trạng thái đứng yên. | `[ ]` Cần tìm |
| `assets/textures/mario/death.png` | Khung hình Mario nhỏ khi bị mất mạng (ném lên trời rồi rơi xuống). | `[ ]` Cần tìm |

### 1.2. Kẻ địch (`assets/textures/enemies/`)
| Đường dẫn tệp | Mô tả chi tiết | Trạng thái |
| :--- | :--- | :---: |
| `assets/textures/enemies/goomba.png` | Sprite sheet của Goomba (Nấm độc), bao gồm 2 khung hình di chuyển và 1 khung hình bị giẫm bẹp (squished). | `[ ]` Cần tìm |
| `assets/textures/enemies/koopa.png` | Sprite sheet của Koopa Troopa (Rùa), bao gồm các khung hình di chuyển và khung hình khi rúc vào mai rùa (shell). | `[ ]` Cần tìm |

### 1.3. Ô bản đồ & Gạch đá (`assets/textures/tiles/`)
| Đường dẫn tệp | Mô tả chi tiết | Trạng thái |
| :--- | :--- | :---: |
| `assets/textures/tiles/tileset.png` | Tệp hình ảnh chứa toàn bộ các khối gạch nền đất, gạch phá hủy được, khối hỏi chấm (?), ống nước xanh, và cột cờ về đích. Lớp `TileMap` sẽ dùng TextureRect để cắt. | `[ ]` Cần tìm |

### 1.4. Vật phẩm (`assets/textures/items/`)
| Đường dẫn tệp | Mô tả chi tiết | Trạng thái |
| :--- | :--- | :---: |
| `assets/textures/items/coin.png` | Sprite sheet hoặc hình ảnh tĩnh của đồng xu (Coin). | `[ ]` Cần tìm |
| `assets/textures/items/mushroom.png` | Nấm tăng trưởng màu đỏ (Super Mushroom) giúp Mario hóa lớn. | `[ ]` Cần tìm |
| `assets/textures/items/fireflower.png` | Hoa lửa (Fire Flower) giúp Mario bắn cầu lửa. | `[ ]` Cần tìm |
| `assets/textures/items/star.png` | Ngôi sao may mắn (Starman) giúp Mario bất tử tạm thời. | `[ ]` Cần tìm |

### 1.5. Giao diện người dùng (`assets/textures/ui/`)
| Đường dẫn tệp | Mô tả chi tiết | Trạng thái |
| :--- | :--- | :---: |
| `assets/textures/ui/hud_icons.png` | Các biểu tượng nhỏ dùng trên thanh hiển thị thông số (HUD) như hình mặt Mario cho số mạng, icon đồng xu nhỏ. | `[ ]` Cần tìm |
| `assets/textures/ui/menu_bg.png` | Ảnh nền cho Menu chính (có thể là hình bầu trời xanh với mây trắng phong cách Mario). | `[ ]` Cần tìm |

---

## 2. Hiệu ứng âm thanh (`assets/sounds/effects/`)

Các hiệu ứng âm thanh ngắn nên sử dụng định dạng không nén `.wav` để SFML phát nhanh và không bị trễ tiếng.

| Đường dẫn tệp | Mô tả tình huống kích hoạt | Trạng thái |
| :--- | :--- | :---: |
| `assets/sounds/effects/jump.wav` | Phát khi Mario thực hiện hành động nhảy. | `[ ]` Cần tìm |
| `assets/sounds/effects/coin.wav` | Phát khi Mario chạm vào ăn đồng xu. | `[ ]` Cần tìm |
| `assets/sounds/effects/die.wav` | Phát khi Mario mất mạng. | `[ ]` Cần tìm |
| `assets/sounds/effects/powerup.wav` | Phát khi Mario ăn được Nấm hoặc Hoa lửa. | `[ ]` Cần tìm |
| `assets/sounds/effects/kick.wav` | Phát khi Mario đá mai rùa Koopa. | `[ ]` Cần tìm |
| `assets/sounds/effects/fireball.wav` | Phát khi Mario bắn một cầu lửa ra từ tay. | `[ ]` Cần tìm |

---

## 3. Nhạc nền màn chơi (`assets/sounds/music/`)

Nhạc nền có thời lượng dài nên sử dụng định dạng `.ogg` để tối ưu dung lượng và hỗ trợ cơ chế phát trực tuyến (streaming) của `sf::Music` mà không tốn bộ nhớ RAM.

| Đường dẫn tệp | Mô tả tình huống kích hoạt | Trạng thái |
| :--- | :--- | :---: |
| `assets/sounds/music/overworld.ogg` | Nhạc nền vui nhộn cho màn chơi ngoài trời (World 1-1 style). | `[ ]` Cần tìm |
| `assets/sounds/music/underground.ogg` | Nhạc nền trầm và bí ẩn cho màn chơi dưới hang tối. | `[ ]` Cần tìm |
| `assets/sounds/music/gameover.ogg` | Đoạn nhạc ngắn sầu thảm khi người chơi hết lượt chơi. | `[ ]` Cần tìm |

---

## 4. Phông chữ (`assets/fonts/`)

| Đường dẫn tệp | Mô tả chi tiết | Trạng thái |
| :--- | :--- | :---: |
| `assets/fonts/mario.ttf` | Phông chữ dạng pixel retro để vẽ điểm số, số mạng và màn chữ tiêu đề. | `[ ]` Cần tìm |

---

## 5. Nguồn tải tham khảo (Tự do / Phi thương mại)

Bạn có thể tìm kiếm và tải xuống miễn phí các tài nguyên trên tại các địa chỉ sau:

* **Đồ họa & Sprites:**
  * [The Spriters Resource](https://www.spriters-resource.com/nes/supermariobros/) (Chọn hệ máy NES để có hình ảnh chuẩn gốc 1985).
  * [Itch.io (Asset Pack)](https://itch.io/game-assets/free/tag-pixel-art) (Tìm các gói tài nguyên miễn phí phong cách 8-bit/16-bit).
  * [OpenGameArt](https://opengameart.org/) (Tìm kiếm với từ khóa "Mario platformer tileset").

* **Âm thanh & Hiệu ứng:**
  * [The Sounds Resource](https://www.sounds-resource.com/nes/supermariobros/) (Tải toàn bộ hiệu ứng gốc của game).
  * [Freesound](https://freesound.org/) (Tìm kiếm hiệu ứng nhảy, nổ, đá hoặc ăn đồng xu dạng 8-bit).

* **Phông chữ:**
  * [Google Fonts - Press Start 2P](https://fonts.google.com/specimen/Press+Start+2P) (Phông chữ retro cực kỳ phổ biến và hoàn toàn miễn phí).
  * [Dafont (Retro/Pixel section)](https://www.dafont.com/theme.php?cat=501) (Tìm các phông chữ dạng pixel).

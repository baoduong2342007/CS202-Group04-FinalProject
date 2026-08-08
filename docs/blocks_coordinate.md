# Bảng Tọa Độ & Ý Nghĩa Chi Tiết Khối Gạch & Hộp Quà (`blocks_coordinate.md`)

Danh sách toàn bộ **120** thành phần liên thông khối gạch (`Block Spritesheet`) tách trực tiếp từ tệp nguồn gốc **`assets/textures/items/items_blocks.png`** (kích thước `448 × 256 px`), phân tích theo thuật toán loang (connected component) và khớp ô Grid chuẩn `16 × 16 px`.

> 📌 **NGUỒN DỮ LIỆU CHÍNH THỨC:**
> Tài liệu này (`blocks_coordinate.md`) cùng tệp ảnh minh họa Atlas **`assets/textures/items/blocks_all_components_atlas.png`** được phân tích và tạo dựng dựa trên tệp hình ảnh gốc **`assets/textures/items/items_blocks.png`** (chứa toàn bộ khối gạch Brick Block, khối hộp nghi vấn Question Block `?`, mảnh vỡ gạch Brick Debris và 4 bảng màu môi trường NES: Overworld, Underground, Castle, Underwater).

> **Giải thích hai cột tọa độ & kích thước:**
> - **Tọa độ Loang (X, Y) & Kích thước Loang (W × H)**: Bounding box thực tế ôm sát các điểm ảnh (pixel) của khối gạch do thuật toán loang tìm thấy.
> - **Tọa độ Ô Grid (X, Y) & Kích thước Grid (W × H)**: Khung ô chuẩn trên Spritesheet dùng cho game engine (`sf::IntRect(x, y, 16, 16)`).

> **Quy ước bảng màu NES (Palette):**
> - **OW (Overworld)**: Bảng màu ngoài trời / mặt đất (Màn 1-1, 1-3, 3-1...) — khối gạch cam/nâu, hộp ? viền đen chữ cam.
> - **UG (Underground)**: Bảng màu lòng đất / hang động (Màn 1-2, 4-2...) — khối gạch xanh Teal, hộp ? màu xanh lam.
> - **Castle (CT)**: Bảng màu lâu đài (Màn 1-4, 2-4...) — khối gạch đá xám, hộp ? màu xám/nâu sáng.
> - **UW (Underwater)**: Bảng màu dưới nước (Màn 2-2, 7-2...) — khối gạch xanh lá cây sáng, hộp ? viền xanh dương.

---

## I. Phân Tích Tổng Quan Bức Ảnh Spritesheet (`items_blocks.png`)

Bức ảnh spritesheet `items_blocks.png` có kích thước **448 × 256 pixel**, được chia thành 2 vùng chính theo chiều ngang và bố trí theo cấu trúc trực quan gồm cả **vùng nhãn thông tin hướng dẫn** và **vùng vật thể game (blocks)**:

### 1. Vùng Thông Tin & Hướng Dẫn (Guide & Text Regions)
- **Tiêu đề chính (Main Headers) (y ≈ 0–32)**: 
  - `Coin Blocks` (X: 80–240): Chữ ghi tiêu đề khối hộp quà / khối chứa xu (`?` Question Blocks).
  - `Brick Blocks` (X: 272–432): Chữ ghi tiêu đề các khối gạch xây.
  - Phân vùng môi trường bên dưới: `Overworld / Underwater` và `Underground / Castle`.
- **Nhãn cột trạng thái (Vertical Column Labels) (y ≈ 48–104)**:
  - Chữ hiển thị xoay dọc giải thích chức năng từng cột: `Initial Tile` (khung ban đầu), `Hit (empty)` (khi húc rỗng), `Brick debris` (mảnh gạch vỡ), `Hit` (khi húc nẩy quà), `After Hit` (ô trống sau khi húc).
- **Nhãn hàng bảng màu (Palette Labels) (X ≈ 0–64 & X ≈ 192–256)**:
  - Ghi rõ tên 4 bảng màu NES tương ứng với 4 dòng sprite: `OW Palette`, `UG Palette`, `Castle Palette`, `UW Palette`.

### 2. Vùng Khối Game (Game Block Regions)
- **Vùng Khối Hộp Hỏi Chấm (`Coin Blocks / Question Blocks`) (y = 112–175, X = 80–255)**:
  - Gồm 40 ô sprite 16×16 px chứa 3 khung hình hoạt họa chớp nháy dấu `?`, 1 khung hình nẩy khối `Hit`, và 1 khung hình ô đã đập `After Hit` (Empty Block).
- **Vùng Khối Gạch Thường (Bộ 1 — Upper `Brick Blocks`) (y = 112–175, X = 272–447)**:
  - Gồm 40 ô sprite 16×16 px đại diện cho khối gạch thường, gạch nẩy, mảnh gạch vỡ 4 góc và gạch rỗng.
- **Vùng Khối Gạch Thường (Bộ 2 — Lower `Brick Blocks`) (y = 192–255, X = 272–447)**:
  - Gồm 40 ô sprite 16×16 px chứa biến thể palette bổ sung cho các loại gạch trong các môi trường đặc biệt.

---

## II. Bảng Tọa Độ Chi Tiết Khối Hộp Hỏi Chấm (`Coin Blocks / Question Blocks`)

Các khối hộp quà `?` nẩy xu hoặc nấm, hoa đạn, sao bất tử. Mỗi loại có 3 khung animation chớp nháy + 1 khung nẩy (`Hit`) + 1 khung ô trống (`After Hit / Used Block`).

### 1. Nhóm Môi Trường Overworld & Underwater (y = 112–175, X = 80–159)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Palette | Ý Nghĩa / Trạng Thái Khung Hình |
|-----|-----------------------|------------------|----------------|-----------------|----------|---------|----------------------------------|
| **#1** | `(80, 112)` | `16 × 16 px` | `(80, 112)` | `16 × 16 px` | 254 px | OW Palette | Initial Tile (Khung 1) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 1 |
| **#2** | `(96, 112)` | `16 × 16 px` | `(96, 112)` | `16 × 16 px` | 254 px | OW Palette | Initial Tile (Khung 2) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 2 |
| **#3** | `(112, 112)` | `16 × 16 px` | `(112, 112)` | `16 × 16 px` | 254 px | OW Palette | Initial Tile (Khung 3) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 3 |
| **#4** | `(128, 112)` | `16 × 16 px` | `(128, 112)` | `16 × 16 px` | 252 px | OW Palette | Hit — Trạng thái khối '?' nẩy lên khi Mario húc đầu từ bên dưới (đang nẩy nấm/xu) |
| **#5** | `(144, 112)` | `16 × 16 px` | `(144, 112)` | `16 × 16 px` | 252 px | OW Palette | After Hit — Trạng thái ô đã bị đập mở (Empty / Used Block) — chuyển thành ô nâu phẳng |
| **#11** | `(80, 128)` | `16 × 16 px` | `(80, 128)` | `16 × 16 px` | 254 px | UG Palette | Initial Tile (Khung 1) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 1 |
| **#12** | `(96, 128)` | `16 × 16 px` | `(96, 128)` | `16 × 16 px` | 254 px | UG Palette | Initial Tile (Khung 2) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 2 |
| **#13** | `(112, 128)` | `16 × 16 px` | `(112, 128)` | `16 × 16 px` | 254 px | UG Palette | Initial Tile (Khung 3) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 3 |
| **#14** | `(128, 128)` | `16 × 16 px` | `(128, 128)` | `16 × 16 px` | 252 px | UG Palette | Hit — Trạng thái khối '?' nẩy lên khi Mario húc đầu từ bên dưới (đang nẩy nấm/xu) |
| **#15** | `(144, 128)` | `16 × 16 px` | `(144, 128)` | `16 × 16 px` | 252 px | UG Palette | After Hit — Trạng thái ô đã bị đập mở (Empty / Used Block) — chuyển thành ô nâu phẳng |
| **#21** | `(80, 144)` | `16 × 16 px` | `(80, 144)` | `16 × 16 px` | 254 px | Castle Palette | Initial Tile (Khung 1) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 1 |
| **#22** | `(96, 144)` | `16 × 16 px` | `(96, 144)` | `16 × 16 px` | 254 px | Castle Palette | Initial Tile (Khung 2) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 2 |
| **#23** | `(112, 144)` | `16 × 16 px` | `(112, 144)` | `16 × 16 px` | 254 px | Castle Palette | Initial Tile (Khung 3) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 3 |
| **#24** | `(128, 144)` | `16 × 16 px` | `(128, 144)` | `16 × 16 px` | 252 px | Castle Palette | Hit — Trạng thái khối '?' nẩy lên khi Mario húc đầu từ bên dưới (đang nẩy nấm/xu) |
| **#25** | `(144, 144)` | `16 × 16 px` | `(144, 144)` | `16 × 16 px` | 252 px | Castle Palette | After Hit — Trạng thái ô đã bị đập mở (Empty / Used Block) — chuyển thành ô nâu phẳng |
| **#31** | `(80, 160)` | `16 × 16 px` | `(80, 160)` | `16 × 16 px` | 254 px | UW Palette | Initial Tile (Khung 1) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 1 |
| **#32** | `(96, 160)` | `16 × 16 px` | `(96, 160)` | `16 × 16 px` | 254 px | UW Palette | Initial Tile (Khung 2) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 2 |
| **#33** | `(112, 160)` | `16 × 16 px` | `(112, 160)` | `16 × 16 px` | 254 px | UW Palette | Initial Tile (Khung 3) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 3 |
| **#34** | `(128, 160)` | `16 × 16 px` | `(128, 160)` | `16 × 16 px` | 252 px | UW Palette | Hit — Trạng thái khối '?' nẩy lên khi Mario húc đầu từ bên dưới (đang nẩy nấm/xu) |
| **#35** | `(144, 160)` | `16 × 16 px` | `(144, 160)` | `16 × 16 px` | 252 px | UW Palette | After Hit — Trạng thái ô đã bị đập mở (Empty / Used Block) — chuyển thành ô nâu phẳng |

### 2. Nhóm Môi Trường Underground & Castle (y = 112–175, X = 176–255)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Palette | Ý Nghĩa / Trạng Thái Khung Hình |
|-----|-----------------------|------------------|----------------|-----------------|----------|---------|----------------------------------|
| **#6** | `(176, 112)` | `16 × 16 px` | `(176, 112)` | `16 × 16 px` | 254 px | OW Palette | Initial Tile (Khung 1) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 1 |
| **#7** | `(192, 112)` | `16 × 16 px` | `(192, 112)` | `16 × 16 px` | 254 px | OW Palette | Initial Tile (Khung 2) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 2 |
| **#8** | `(208, 112)` | `16 × 16 px` | `(208, 112)` | `16 × 16 px` | 254 px | OW Palette | Initial Tile (Khung 3) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 3 |
| **#9** | `(224, 112)` | `16 × 16 px` | `(224, 112)` | `16 × 16 px` | 252 px | OW Palette | Hit — Trạng thái khối '?' nẩy lên khi Mario húc đầu từ bên dưới (đang nẩy nấm/xu) |
| **#10** | `(240, 112)` | `16 × 16 px` | `(240, 112)` | `16 × 16 px` | 252 px | OW Palette | After Hit — Trạng thái ô đã bị đập mở (Empty / Used Block) — chuyển thành ô nâu phẳng |
| **#16** | `(176, 128)` | `16 × 16 px` | `(176, 128)` | `16 × 16 px` | 254 px | UG Palette | Initial Tile (Khung 1) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 1 |
| **#17** | `(192, 128)` | `16 × 16 px` | `(192, 128)` | `16 × 16 px` | 254 px | UG Palette | Initial Tile (Khung 2) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 2 |
| **#18** | `(208, 128)` | `16 × 16 px` | `(208, 128)` | `16 × 16 px` | 254 px | UG Palette | Initial Tile (Khung 3) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 3 |
| **#19** | `(224, 128)` | `16 × 16 px` | `(224, 128)` | `16 × 16 px` | 252 px | UG Palette | Hit — Trạng thái khối '?' nẩy lên khi Mario húc đầu từ bên dưới (đang nẩy nấm/xu) |
| **#20** | `(240, 128)` | `16 × 16 px` | `(240, 128)` | `16 × 16 px` | 252 px | UG Palette | After Hit — Trạng thái ô đã bị đập mở (Empty / Used Block) — chuyển thành ô nâu phẳng |
| **#26** | `(176, 144)` | `16 × 16 px` | `(176, 144)` | `16 × 16 px` | 254 px | Castle Palette | Initial Tile (Khung 1) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 1 |
| **#27** | `(192, 144)` | `16 × 16 px` | `(192, 144)` | `16 × 16 px` | 254 px | Castle Palette | Initial Tile (Khung 2) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 2 |
| **#28** | `(208, 144)` | `16 × 16 px` | `(208, 144)` | `16 × 16 px` | 254 px | Castle Palette | Initial Tile (Khung 3) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 3 |
| **#29** | `(224, 144)` | `16 × 16 px` | `(224, 144)` | `16 × 16 px` | 252 px | Castle Palette | Hit — Trạng thái khối '?' nẩy lên khi Mario húc đầu từ bên dưới (đang nẩy nấm/xu) |
| **#30** | `(240, 144)` | `16 × 16 px` | `(240, 144)` | `16 × 16 px` | 252 px | Castle Palette | After Hit — Trạng thái ô đã bị đập mở (Empty / Used Block) — chuyển thành ô nâu phẳng |
| **#36** | `(176, 160)` | `16 × 16 px` | `(176, 160)` | `16 × 16 px` | 254 px | UW Palette | Initial Tile (Khung 1) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 1 |
| **#37** | `(192, 160)` | `16 × 16 px` | `(192, 160)` | `16 × 16 px` | 254 px | UW Palette | Initial Tile (Khung 2) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 2 |
| **#38** | `(208, 160)` | `16 × 16 px` | `(208, 160)` | `16 × 16 px` | 254 px | UW Palette | Initial Tile (Khung 3) — Dạng ô nghi vấn '?' ở trạng thái chờ/nhấp nháy — Khung 3 |
| **#39** | `(224, 160)` | `16 × 16 px` | `(224, 160)` | `16 × 16 px` | 252 px | UW Palette | Hit — Trạng thái khối '?' nẩy lên khi Mario húc đầu từ bên dưới (đang nẩy nấm/xu) |
| **#40** | `(240, 160)` | `16 × 16 px` | `(240, 160)` | `16 × 16 px` | 252 px | UW Palette | After Hit — Trạng thái ô đã bị đập mở (Empty / Used Block) — chuyển thành ô nâu phẳng |

---

## III. Bảng Tọa Độ Chi Tiết Khối Gạch Thường (Bộ 1 — Upper `Brick Blocks`)

Khối gạch thường có thể bị Mario to húc vỡ (tạo 4 mảnh gạch văng `Brick debris`) hoặc nẩy lên khi chứa quà/xu.

> 💡 **KỸ THUẬT CẮT SPRITE HỆ THỐNG MẢNH VỠ (`BlockDebris`):**
> Các ô `Brick debris` trên spritesheet được vẽ gộp chung trong 1 ô lưới **`16 × 16 px`** (gồm 4 mảnh nhỏ 4 góc). 
> Khi lập trình hiệu ứng vỡ gạch trong game engine, ô `16 × 16 px` này sẽ được **tách thành 4 sub-rect `8 × 8 px`** độc lập để gán cho 4 đối tượng mảnh vỡ văng ra 4 hướng khác nhau:
> - **Top-Left (Góc trên-trái)**: `sf::IntRect(X, Y, 8, 8)`
> - **Top-Right (Góc trên-phải)**: `sf::IntRect(X + 8, Y, 8, 8)`
> - **Bottom-Left (Góc dưới-trái)**: `sf::IntRect(X, Y + 8, 8, 8)`
> - **Bottom-Right (Góc dưới-phải)**: `sf::IntRect(X + 8, Y + 8, 8, 8)`

### 1. Gạch Bộ 1 — Overworld & Underwater (y = 112–175, X = 272–351)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Palette | Ý Nghĩa / Trạng Thái Khung Hình |
|-----|-----------------------|------------------|----------------|-----------------|----------|---------|----------------------------------|
| **#41** | `(272, 112)` | `16 × 16 px` | `(272, 112)` | `16 × 16 px` | 256 px | OW Palette | Initial Tile — Khối gạch nguyên vẹn chưa bị tác động |
| **#42** | `(288, 112)` | `16 × 16 px` | `(288, 112)` | `16 × 16 px` | 256 px | OW Palette | Hit (empty) — Trạng thái gạch bị Mario bé húc nẩy lên (không vỡ, không chứa quà) |
| **#43** | `(304, 112)` | `16 × 16 px` | `(304, 112)` | `16 × 16 px` | 184 px | OW Palette | Brick debris — 4 Mảnh vỡ gạch văng ra 4 góc khi Mario to húc vỡ gạch |
| **#44** | `(320, 112)` | `16 × 16 px` | `(320, 112)` | `16 × 16 px` | 252 px | OW Palette | Hit — Trạng thái gạch nẩy lên khi húc (chứa vật phẩm/xu) |
| **#45** | `(336, 112)` | `16 × 16 px` | `(336, 112)` | `16 × 16 px` | 252 px | OW Palette | After Hit — Trạng thái gạch sau khi dùng hết xu/vật phẩm — biến thành ô trống rỗng |
| **#51** | `(272, 128)` | `16 × 16 px` | `(272, 128)` | `16 × 16 px` | 256 px | UG Palette | Initial Tile — Khối gạch nguyên vẹn chưa bị tác động |
| **#52** | `(288, 128)` | `16 × 16 px` | `(288, 128)` | `16 × 16 px` | 256 px | UG Palette | Hit (empty) — Trạng thái gạch bị Mario bé húc nẩy lên (không vỡ, không chứa quà) |
| **#53** | `(304, 128)` | `16 × 16 px` | `(304, 128)` | `16 × 16 px` | 184 px | UG Palette | Brick debris — 4 Mảnh vỡ gạch văng ra 4 góc khi Mario to húc vỡ gạch |
| **#54** | `(320, 128)` | `16 × 16 px` | `(320, 128)` | `16 × 16 px` | 252 px | UG Palette | Hit — Trạng thái gạch nẩy lên khi húc (chứa vật phẩm/xu) |
| **#55** | `(336, 128)` | `16 × 16 px` | `(336, 128)` | `16 × 16 px` | 252 px | UG Palette | After Hit — Trạng thái gạch sau khi dùng hết xu/vật phẩm — biến thành ô trống rỗng |
| **#61** | `(272, 144)` | `16 × 16 px` | `(272, 144)` | `16 × 16 px` | 256 px | Castle Palette | Initial Tile — Khối gạch nguyên vẹn chưa bị tác động |
| **#62** | `(288, 144)` | `16 × 16 px` | `(288, 144)` | `16 × 16 px` | 256 px | Castle Palette | Hit (empty) — Trạng thái gạch bị Mario bé húc nẩy lên (không vỡ, không chứa quà) |
| **#63** | `(304, 144)` | `16 × 16 px` | `(304, 144)` | `16 × 16 px` | 184 px | Castle Palette | Brick debris — 4 Mảnh vỡ gạch văng ra 4 góc khi Mario to húc vỡ gạch |
| **#64** | `(320, 144)` | `16 × 16 px` | `(320, 144)` | `16 × 16 px` | 252 px | Castle Palette | Hit — Trạng thái gạch nẩy lên khi húc (chứa vật phẩm/xu) |
| **#65** | `(336, 144)` | `16 × 16 px` | `(336, 144)` | `16 × 16 px` | 252 px | Castle Palette | After Hit — Trạng thái gạch sau khi dùng hết xu/vật phẩm — biến thành ô trống rỗng |
| **#71** | `(272, 160)` | `16 × 16 px` | `(272, 160)` | `16 × 16 px` | 256 px | UW Palette | Initial Tile — Khối gạch nguyên vẹn chưa bị tác động |
| **#72** | `(288, 160)` | `16 × 16 px` | `(288, 160)` | `16 × 16 px` | 256 px | UW Palette | Hit (empty) — Trạng thái gạch bị Mario bé húc nẩy lên (không vỡ, không chứa quà) |
| **#73** | `(304, 160)` | `16 × 16 px` | `(304, 160)` | `16 × 16 px` | 184 px | UW Palette | Brick debris — 4 Mảnh vỡ gạch văng ra 4 góc khi Mario to húc vỡ gạch |
| **#74** | `(320, 160)` | `16 × 16 px` | `(320, 160)` | `16 × 16 px` | 252 px | UW Palette | Hit — Trạng thái gạch nẩy lên khi húc (chứa vật phẩm/xu) |
| **#75** | `(336, 160)` | `16 × 16 px` | `(336, 160)` | `16 × 16 px` | 252 px | UW Palette | After Hit — Trạng thái gạch sau khi dùng hết xu/vật phẩm — biến thành ô trống rỗng |

### 2. Gạch Bộ 1 — Underground & Castle (y = 112–175, X = 368–447)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Palette | Ý Nghĩa / Trạng Thái Khung Hình |
|-----|-----------------------|------------------|----------------|-----------------|----------|---------|----------------------------------|
| **#46** | `(368, 112)` | `16 × 16 px` | `(368, 112)` | `16 × 16 px` | 256 px | OW Palette | Initial Tile — Khối gạch nguyên vẹn chưa bị tác động |
| **#47** | `(384, 112)` | `16 × 16 px` | `(384, 112)` | `16 × 16 px` | 256 px | OW Palette | Hit (empty) — Trạng thái gạch bị Mario bé húc nẩy lên (không vỡ, không chứa quà) |
| **#48** | `(400, 112)` | `16 × 16 px` | `(400, 112)` | `16 × 16 px` | 184 px | OW Palette | Brick debris — 4 Mảnh vỡ gạch văng ra 4 góc khi Mario to húc vỡ gạch |
| **#49** | `(416, 112)` | `16 × 16 px` | `(416, 112)` | `16 × 16 px` | 252 px | OW Palette | Hit — Trạng thái gạch nẩy lên khi húc (chứa vật phẩm/xu) |
| **#50** | `(432, 112)` | `16 × 16 px` | `(432, 112)` | `16 × 16 px` | 252 px | OW Palette | After Hit — Trạng thái gạch sau khi dùng hết xu/vật phẩm — biến thành ô trống rỗng |
| **#56** | `(368, 128)` | `16 × 16 px` | `(368, 128)` | `16 × 16 px` | 256 px | UG Palette | Initial Tile — Khối gạch nguyên vẹn chưa bị tác động |
| **#57** | `(384, 128)` | `16 × 16 px` | `(384, 128)` | `16 × 16 px` | 256 px | UG Palette | Hit (empty) — Trạng thái gạch bị Mario bé húc nẩy lên (không vỡ, không chứa quà) |
| **#58** | `(400, 128)` | `16 × 16 px` | `(400, 128)` | `16 × 16 px` | 184 px | UG Palette | Brick debris — 4 Mảnh vỡ gạch văng ra 4 góc khi Mario to húc vỡ gạch |
| **#59** | `(416, 128)` | `16 × 16 px` | `(416, 128)` | `16 × 16 px` | 252 px | UG Palette | Hit — Trạng thái gạch nẩy lên khi húc (chứa vật phẩm/xu) |
| **#60** | `(432, 128)` | `16 × 16 px` | `(432, 128)` | `16 × 16 px` | 252 px | UG Palette | After Hit — Trạng thái gạch sau khi dùng hết xu/vật phẩm — biến thành ô trống rỗng |
| **#66** | `(368, 144)` | `16 × 16 px` | `(368, 144)` | `16 × 16 px` | 256 px | Castle Palette | Initial Tile — Khối gạch nguyên vẹn chưa bị tác động |
| **#67** | `(384, 144)` | `16 × 16 px` | `(384, 144)` | `16 × 16 px` | 256 px | Castle Palette | Hit (empty) — Trạng thái gạch bị Mario bé húc nẩy lên (không vỡ, không chứa quà) |
| **#68** | `(400, 144)` | `16 × 16 px` | `(400, 144)` | `16 × 16 px` | 184 px | Castle Palette | Brick debris — 4 Mảnh vỡ gạch văng ra 4 góc khi Mario to húc vỡ gạch |
| **#69** | `(416, 144)` | `16 × 16 px` | `(416, 144)` | `16 × 16 px` | 252 px | Castle Palette | Hit — Trạng thái gạch nẩy lên khi húc (chứa vật phẩm/xu) |
| **#70** | `(432, 144)` | `16 × 16 px` | `(432, 144)` | `16 × 16 px` | 252 px | Castle Palette | After Hit — Trạng thái gạch sau khi dùng hết xu/vật phẩm — biến thành ô trống rỗng |
| **#76** | `(368, 160)` | `16 × 16 px` | `(368, 160)` | `16 × 16 px` | 256 px | UW Palette | Initial Tile — Khối gạch nguyên vẹn chưa bị tác động |
| **#77** | `(384, 160)` | `16 × 16 px` | `(384, 160)` | `16 × 16 px` | 256 px | UW Palette | Hit (empty) — Trạng thái gạch bị Mario bé húc nẩy lên (không vỡ, không chứa quà) |
| **#78** | `(400, 160)` | `16 × 16 px` | `(400, 160)` | `16 × 16 px` | 184 px | UW Palette | Brick debris — 4 Mảnh vỡ gạch văng ra 4 góc khi Mario to húc vỡ gạch |
| **#79** | `(416, 160)` | `16 × 16 px` | `(416, 160)` | `16 × 16 px` | 252 px | UW Palette | Hit — Trạng thái gạch nẩy lên khi húc (chứa vật phẩm/xu) |
| **#80** | `(432, 160)` | `16 × 16 px` | `(432, 160)` | `16 × 16 px` | 252 px | UW Palette | After Hit — Trạng thái gạch sau khi dùng hết xu/vật phẩm — biến thành ô trống rỗng |

---

## IV. Bảng Tọa Độ Chi Tiết Khối Gạch Thường (Bộ 2 — Lower `Brick Blocks`)

Bộ gạch dòng dưới (y = 192–255) chứa các biến thể bảng màu mở rộng cho gạch trong các bản đồ đặc biệt.

### 1. Gạch Bộ 2 — Overworld & Underwater (y = 192–255, X = 272–351)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Palette | Ý Nghĩa / Trạng Thái Khung Hình |
|-----|-----------------------|------------------|----------------|-----------------|----------|---------|----------------------------------|
| **#81** | `(272, 192)` | `16 × 16 px` | `(272, 192)` | `16 × 16 px` | 256 px | OW Palette | Initial Tile — Khối gạch nguyên vẹn chưa bị tác động (Palette mở rộng 2) |
| **#82** | `(288, 192)` | `16 × 16 px` | `(288, 192)` | `16 × 16 px` | 256 px | OW Palette | Hit (empty) — Trạng thái gạch bị Mario bé húc nẩy lên (Palette mở rộng 2) |
| **#83** | `(304, 192)` | `16 × 16 px` | `(304, 192)` | `16 × 16 px` | 184 px | OW Palette | Brick debris — 4 Mảnh vỡ gạch văng ra khi bị húc vỡ (Palette mở rộng 2) |
| **#84** | `(320, 192)` | `16 × 16 px` | `(320, 192)` | `16 × 16 px` | 252 px | OW Palette | Hit — Trạng thái gạch nẩy lên chứa vật phẩm (Palette mở rộng 2) |
| **#85** | `(336, 192)` | `16 × 16 px` | `(336, 192)` | `16 × 16 px` | 252 px | OW Palette | After Hit — Trạng thái ô gạch sau khi nẩy hết quà (Palette mở rộng 2) |
| **#91** | `(272, 208)` | `16 × 16 px` | `(272, 208)` | `16 × 16 px` | 256 px | UG Palette | Initial Tile — Khối gạch nguyên vẹn chưa bị tác động (Palette mở rộng 2) |
| **#92** | `(288, 208)` | `16 × 16 px` | `(288, 208)` | `16 × 16 px` | 256 px | UG Palette | Hit (empty) — Trạng thái gạch bị Mario bé húc nẩy lên (Palette mở rộng 2) |
| **#93** | `(304, 208)` | `16 × 16 px` | `(304, 208)` | `16 × 16 px` | 184 px | UG Palette | Brick debris — 4 Mảnh vỡ gạch văng ra khi bị húc vỡ (Palette mở rộng 2) |
| **#94** | `(320, 208)` | `16 × 16 px` | `(320, 208)` | `16 × 16 px` | 252 px | UG Palette | Hit — Trạng thái gạch nẩy lên chứa vật phẩm (Palette mở rộng 2) |
| **#95** | `(336, 208)` | `16 × 16 px` | `(336, 208)` | `16 × 16 px` | 252 px | UG Palette | After Hit — Trạng thái ô gạch sau khi nẩy hết quà (Palette mở rộng 2) |
| **#101** | `(272, 224)` | `16 × 16 px` | `(272, 224)` | `16 × 16 px` | 256 px | Castle Palette | Initial Tile — Khối gạch nguyên vẹn chưa bị tác động (Palette mở rộng 2) |
| **#102** | `(288, 224)` | `16 × 16 px` | `(288, 224)` | `16 × 16 px` | 256 px | Castle Palette | Hit (empty) — Trạng thái gạch bị Mario bé húc nẩy lên (Palette mở rộng 2) |
| **#103** | `(304, 224)` | `16 × 16 px` | `(304, 224)` | `16 × 16 px` | 184 px | Castle Palette | Brick debris — 4 Mảnh vỡ gạch văng ra khi bị húc vỡ (Palette mở rộng 2) |
| **#104** | `(320, 224)` | `16 × 16 px` | `(320, 224)` | `16 × 16 px` | 252 px | Castle Palette | Hit — Trạng thái gạch nẩy lên chứa vật phẩm (Palette mở rộng 2) |
| **#105** | `(336, 224)` | `16 × 16 px` | `(336, 224)` | `16 × 16 px` | 252 px | Castle Palette | After Hit — Trạng thái ô gạch sau khi nẩy hết quà (Palette mở rộng 2) |
| **#111** | `(272, 240)` | `16 × 16 px` | `(272, 240)` | `16 × 16 px` | 256 px | UW Palette | Initial Tile — Khối gạch nguyên vẹn chưa bị tác động (Palette mở rộng 2) |
| **#112** | `(288, 240)` | `16 × 16 px` | `(288, 240)` | `16 × 16 px` | 256 px | UW Palette | Hit (empty) — Trạng thái gạch bị Mario bé húc nẩy lên (Palette mở rộng 2) |
| **#113** | `(304, 240)` | `16 × 16 px` | `(304, 240)` | `16 × 16 px` | 184 px | UW Palette | Brick debris — 4 Mảnh vỡ gạch văng ra khi bị húc vỡ (Palette mở rộng 2) |
| **#114** | `(320, 240)` | `16 × 16 px` | `(320, 240)` | `16 × 16 px` | 252 px | UW Palette | Hit — Trạng thái gạch nẩy lên chứa vật phẩm (Palette mở rộng 2) |
| **#115** | `(336, 240)` | `16 × 16 px` | `(336, 240)` | `16 × 16 px` | 252 px | UW Palette | After Hit — Trạng thái ô gạch sau khi nẩy hết quà (Palette mở rộng 2) |

### 2. Gạch Bộ 2 — Underground & Castle (y = 192–255, X = 368–447)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Palette | Ý Nghĩa / Trạng Thái Khung Hình |
|-----|-----------------------|------------------|----------------|-----------------|----------|---------|----------------------------------|
| **#86** | `(368, 192)` | `16 × 16 px` | `(368, 192)` | `16 × 16 px` | 256 px | OW Palette | Initial Tile — Khối gạch nguyên vẹn chưa bị tác động (Palette mở rộng 2) |
| **#87** | `(384, 192)` | `16 × 16 px` | `(384, 192)` | `16 × 16 px` | 256 px | OW Palette | Hit (empty) — Trạng thái gạch bị Mario bé húc nẩy lên (Palette mở rộng 2) |
| **#88** | `(400, 192)` | `16 × 16 px` | `(400, 192)` | `16 × 16 px` | 184 px | OW Palette | Brick debris — 4 Mảnh vỡ gạch văng ra khi bị húc vỡ (Palette mở rộng 2) |
| **#89** | `(416, 192)` | `16 × 16 px` | `(416, 192)` | `16 × 16 px` | 252 px | OW Palette | Hit — Trạng thái gạch nẩy lên chứa vật phẩm (Palette mở rộng 2) |
| **#90** | `(432, 192)` | `16 × 16 px` | `(432, 192)` | `16 × 16 px` | 252 px | OW Palette | After Hit — Trạng thái ô gạch sau khi nẩy hết quà (Palette mở rộng 2) |
| **#96** | `(368, 208)` | `16 × 16 px` | `(368, 208)` | `16 × 16 px` | 256 px | UG Palette | Initial Tile — Khối gạch nguyên vẹn chưa bị tác động (Palette mở rộng 2) |
| **#97** | `(384, 208)` | `16 × 16 px` | `(384, 208)` | `16 × 16 px` | 256 px | UG Palette | Hit (empty) — Trạng thái gạch bị Mario bé húc nẩy lên (Palette mở rộng 2) |
| **#98** | `(400, 208)` | `16 × 16 px` | `(400, 208)` | `16 × 16 px` | 184 px | UG Palette | Brick debris — 4 Mảnh vỡ gạch văng ra khi bị húc vỡ (Palette mở rộng 2) |
| **#99** | `(416, 208)` | `16 × 16 px` | `(416, 208)` | `16 × 16 px` | 252 px | UG Palette | Hit — Trạng thái gạch nẩy lên chứa vật phẩm (Palette mở rộng 2) |
| **#100** | `(432, 208)` | `16 × 16 px` | `(432, 208)` | `16 × 16 px` | 252 px | UG Palette | After Hit — Trạng thái ô gạch sau khi nẩy hết quà (Palette mở rộng 2) |
| **#106** | `(368, 224)` | `16 × 16 px` | `(368, 224)` | `16 × 16 px` | 256 px | Castle Palette | Initial Tile — Khối gạch nguyên vẹn chưa bị tác động (Palette mở rộng 2) |
| **#107** | `(384, 224)` | `16 × 16 px` | `(384, 224)` | `16 × 16 px` | 256 px | Castle Palette | Hit (empty) — Trạng thái gạch bị Mario bé húc nẩy lên (Palette mở rộng 2) |
| **#108** | `(400, 224)` | `16 × 16 px` | `(400, 224)` | `16 × 16 px` | 184 px | Castle Palette | Brick debris — 4 Mảnh vỡ gạch văng ra khi bị húc vỡ (Palette mở rộng 2) |
| **#109** | `(416, 224)` | `16 × 16 px` | `(416, 224)` | `16 × 16 px` | 252 px | Castle Palette | Hit — Trạng thái gạch nẩy lên chứa vật phẩm (Palette mở rộng 2) |
| **#110** | `(432, 224)` | `16 × 16 px` | `(432, 224)` | `16 × 16 px` | 252 px | Castle Palette | After Hit — Trạng thái ô gạch sau khi nẩy hết quà (Palette mở rộng 2) |
| **#116** | `(368, 240)` | `16 × 16 px` | `(368, 240)` | `16 × 16 px` | 256 px | UW Palette | Initial Tile — Khối gạch nguyên vẹn chưa bị tác động (Palette mở rộng 2) |
| **#117** | `(384, 240)` | `16 × 16 px` | `(384, 240)` | `16 × 16 px` | 256 px | UW Palette | Hit (empty) — Trạng thái gạch bị Mario bé húc nẩy lên (Palette mở rộng 2) |
| **#118** | `(400, 240)` | `16 × 16 px` | `(400, 240)` | `16 × 16 px` | 184 px | UW Palette | Brick debris — 4 Mảnh vỡ gạch văng ra khi bị húc vỡ (Palette mở rộng 2) |
| **#119** | `(416, 240)` | `16 × 16 px` | `(416, 240)` | `16 × 16 px` | 252 px | UW Palette | Hit — Trạng thái gạch nẩy lên chứa vật phẩm (Palette mở rộng 2) |
| **#120** | `(432, 240)` | `16 × 16 px` | `(432, 240)` | `16 × 16 px` | 252 px | UW Palette | After Hit — Trạng thái ô gạch sau khi nẩy hết quà (Palette mở rộng 2) |

---

## V. Tổng Kết Cấu Trúc Khối (`items_blocks.png`)

| Nhóm Khối Vật Thể | Số Môi Trường / Palette | Số Sprite (Khung hình) | Dải STT | Mục Đích Sử Dụng Trong Engine |
|-------------------|--------------------------|-------------------------|---------|--------------------------------|
| **Coin Blocks (`?`) - OW/UW** | 4 Palettes (OW/UG/CT/UW) | 20 | #1 – #20 | Khối ? nhấp nháy 3 khung, nẩy khi húc (`Hit`), biến thành ô rỗng phẳng nâu |
| **Coin Blocks (`?`) - UG/CT** | 4 Palettes (OW/UG/CT/UW) | 20 | #21 – #40 | Biến thể khối ? dùng riêng cho hầm tối / lâu đài |
| **Brick Blocks (Bộ 1) - OW/UW** | 4 Palettes (OW/UG/CT/UW) | 20 | #41 – #60 | Gạch thường ngoài trời, mảnh vỡ gạch 4 góc, nẩy nấm/xu |
| **Brick Blocks (Bộ 1) - UG/CT** | 4 Palettes (OW/UG/CT/UW) | 20 | #61 – #80 | Gạch hầm tối / lâu đài |
| **Brick Blocks (Bộ 2) - OW/UW** | 4 Palettes (OW/UG/CT/UW) | 20 | #81 – #100 | Biến thể gạch ngoài trời (dòng dưới) |
| **Brick Blocks (Bộ 2) - UG/CT** | 4 Palettes (OW/UG/CT/UW) | 20 | #101 – #120 | Biến thể gạch hầm tối (dòng dưới) |
| **TỔNG CỘNG** | **4 Palettes × 30 Cột** | **120 Sprites** | **#1 – #120** | **Toàn bộ hệ thống Block trong game** |


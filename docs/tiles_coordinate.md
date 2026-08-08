# Bảng Tọa Độ & Ý Nghĩa Chi Tiết Tileset (`tiles_coordinate.md`)

Tài liệu này mô tả các frame tile được dùng bởi `TileMap` từ atlas:

```text
assets/textures/tiles/tileset.png
```

Atlas hiện tại có kích thước **680 × 776 px**. Các tile gameplay chính dùng source rect **16 × 16 px**.

> **Quan trọng về scale**
>
> - `16 × 16 px` là kích thước **source frame trong atlas**.
> - Một ô level vẫn là **32 × 32 world pixels** (`TileMap::TILE_SIZE = 32`).
> - SFML map một source rect `16 × 16` lên quad `32 × 32`.
> - Không đổi physics/grid/camera sang 16 px chỉ vì sprite nguồn là 16 px.

---

## Quy ước tọa độ

Các bảng dùng cùng cách ghi với `enemies_coordinate.md`:

- **Tọa độ Ô Grid (X, Y)**: pixel trên góc trên-trái của frame trong atlas.
- **Kích thước Grid (W × H)**: kích thước source rect truyền vào `sf::IntRect`.
- Tọa độ bắt đầu từ `(0, 0)` ở góc trên-trái ảnh.

Ví dụ:

```cpp
sf::IntRect({0, 16}, {16, 16})
```

nghĩa là lấy vùng:

```text
x = 0
y = 16
width = 16
height = 16
```

---

# 1. Ground / Stone / Brick / Used Block — Overworld

Các frame này nằm trong khu vực **Palette 1 Tiles (Ground and Stone) / Overworld**.

| STT | Tọa độ Ô Grid | Kích thước Grid | Symbol | Ý nghĩa |
|---|---|---|---|---|
| **#1** | `(0, 16)` | `16 × 16 px` | `1` | Ground |
| **#2** | `(0, 33)` | `16 × 16 px` | `S` | Solid terrain |
| **#3** | `(17, 16)` | `16 × 16 px` | `B` | Breakable brick |
| **#4** | `(349, 78)` | `16 × 16 px` | `E` | Used / empty block |

### Runtime mapping

```cpp
GROUND  = sf::IntRect({0, 16},  {16, 16});
STONE   = sf::IntRect({0, 33},  {16, 16});
BRICK   = sf::IntRect({17, 16}, {16, 16});
USED    = sf::IntRect({78, 349}, {16, 16});
```

---

# 2. Question Block

Question Block sử dụng frame màu cam trong khu vực **Palette 3 (Question and Water)**.

| STT | Tọa độ Ô Grid | Kích thước Grid | Symbol | Ý nghĩa |
|---|---|---|---|---|
| **#4** | `(298, 78)` | `16 × 16 px` | `?`, `U`, `O` | Question Block active |
| **#5** | `(349, 78)` | `16 × 16 px` | `E` candidate | Question Block sau khi đã dùng |

> `?`, `U`, `O` vẫn là `QuestionBlock` entity và `QuestionBlock` là source of truth cho animation/item spawning.
>
> `TileMap` không được tạo item từ các symbol này lần thứ hai. Các tọa độ ở đây là nguồn tham chiếu visual, không thay đổi ownership của gameplay logic.

---

# 3. Pipe — Overworld

Bốn frame dưới đây tạo một pipe rộng 2 tile:

```text
[ ]
{ }
```

Trong đó:

```text
[ = top-left
] = top-right
{ = body-left
} = body-right
```

| STT | Tọa độ Ô Grid | Kích thước Grid | Symbol | Ý nghĩa |
|---|---|---|---|---|
| **#6** | `(119, 196)` | `16 × 16 px` | `[` | Pipe top-left |
| **#7** | `(136, 196)` | `16 × 16 px` | `]` | Pipe top-right |
| **#8** | `(119, 213)` | `16 × 16 px` | `{` | Pipe body-left |
| **#9** | `(136, 213)` | `16 × 16 px` | `}` | Pipe body-right |

### Level representation

```text
[]
{}
{}
```

Các pipe tile là solid terrain.

---

# 4. Finish Pole

Finish pole nằm trong khu vực **Assembled Structures**.

| STT | Tọa độ Ô Grid | Kích thước Grid | Symbol | Ý nghĩa |
|---|---|---|---|---|
| **#10** | `(136, 230)` | `16 × 16 px` | `F` | Flag/pole top |
| **#11** | `(136, 247)` | `16 × 16 px` | `|` | Pole body |

Pole có thể được kéo dài bằng cách lặp frame body:

```text
F
|
|
|
1
```

`F` là finish marker duy nhất. Các `|` phải nằm liên tục bên dưới `F` và kết thúc trên solid terrain theo level validator.

---

# 5. Symbol → Frame Mapping Dùng Cho TileMap

| Symbol | Frame | Render bởi |
|---|---|---|
| `1` | Ground `(0,16,16,16)` | `TileMap` |
| `S` | Stone `(0,33,16,16)` | `TileMap` |
| `B` | Brick `(17,16,16,16)` | `TileMap` |
| `E` | Used `(349,78,16,16)` | `TileMap` |
| `[` | Pipe top-left `(119, 196,16,16)` | `TileMap` |
| `]` | Pipe top-right `(136, 196,16,16)` | `TileMap` |
| `{` | Pipe body-left `(119, 213,16,16)` | `TileMap` |
| `}` | Pipe body-right `(136, 213,16,16)` | `TileMap` |
| `F` | Pole top `(136, 230,16,16)` | `TileMap` |
| `|` | Pole body `(136, 247,16,16)` | `TileMap` |
| `?` | Question `(298,78,16,16)` | `QuestionBlock` entity |
| `U` | Question `(298,78,16,16)` | `QuestionBlock` entity |
| `O` | Question `(298,78,16,16)` | `QuestionBlock` entity |

---

# 6. Suggested C++ Constants

```cpp
namespace TileFrames {

inline constexpr int SOURCE_TILE_SIZE = 16;

inline constexpr sf::IntRect GROUND{
    {0, 16},
    {SOURCE_TILE_SIZE, SOURCE_TILE_SIZE}
};
  
inline constexpr sf::IntRect STONE{
    {0, 33},
    {SOURCE_TILE_SIZE, SOURCE_TILE_SIZE}
};

inline constexpr sf::IntRect BRICK{
    {17, 16},
    {SOURCE_TILE_SIZE, SOURCE_TILE_SIZE}
};

inline constexpr sf::IntRect USED_BLOCK{
    (349, 78),
    {SOURCE_TILE_SIZE, SOURCE_TILE_SIZE}
};

inline constexpr sf::IntRect QUESTION{
    {298, 78},
    {SOURCE_TILE_SIZE, SOURCE_TILE_SIZE}
};

inline constexpr sf::IntRect PIPE_TOP_LEFT{
    {119, 196},
    {SOURCE_TILE_SIZE, SOURCE_TILE_SIZE}
};

inline constexpr sf::IntRect PIPE_TOP_RIGHT{
    {136, 196},
    {SOURCE_TILE_SIZE, SOURCE_TILE_SIZE}
};

inline constexpr sf::IntRect PIPE_BODY_LEFT{
    {119, 213},
    {SOURCE_TILE_SIZE, SOURCE_TILE_SIZE}
};

inline constexpr sf::IntRect PIPE_BODY_RIGHT{
    {136, 213},
    {SOURCE_TILE_SIZE, SOURCE_TILE_SIZE}
};

inline constexpr sf::IntRect FINISH_TOP{
    {136, 230},
    {SOURCE_TILE_SIZE, SOURCE_TILE_SIZE}
};

inline constexpr sf::IntRect FINISH_POLE{
    {136, 247},
    {SOURCE_TILE_SIZE, SOURCE_TILE_SIZE}
};

} // namespace TileFrames
```

---

# 7. Sprint 6 Scope

Tài liệu này phục vụ trực tiếp:

- **S6-TV4-09 — Chuyển TileMap sang tileset đúng**
  - ground có frame riêng;
  - stone có frame riêng
  - brick có frame riêng;
  - question có frame riêng;
  - used block có frame riêng;
  - pipe có bốn frame riêng.
- **S6-TV4-10 — Render flag/pole đúng**
  - `F` không còn dùng brick/empty placeholder;
  - `|` không còn dùng brick/empty placeholder.

Các palette Underground/Castle có thể được map riêng khi `TileMap` nhận `LevelTheme`. Không nên hard-code thêm theme-specific mapping vào gameplay trước khi theme routing được xác định.

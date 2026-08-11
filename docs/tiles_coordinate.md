# Compatibility note: tile coordinates

Tài liệu canonical hiện tại là [`tileset_coordinate.md`](tileset_coordinate.md).
File này được giữ vì một số tài liệu Sprint cũ còn tham chiếu tên
`tiles_coordinate.md`; các mapping cũ trong file này đã bị loại bỏ vì từng
trỏ nhầm terrain quartet sang question/used block.

## Runtime contract

- Sheet nguồn: `assets/textures/tiles/tileset.png` — `680 × 776 px`.
- Source frame: `16 × 16 px`; một ô level render thành `32 × 32 world px`.
- `TileMap` dùng `TileFrames.h` làm code source of truth.
- Component ID `#N` của atlas tham khảo không phải tọa độ pixel.

Các nhóm quan trọng:

| Nhóm | Overworld | Underground | Castle | Underwater |
|---|---|---|---|---|
| Ground | `(0,16)` | `(147,16)` | `(0,100)` | `(147,100)` |
| Brick | `(17,16)` | `(164,16)` | `(17,100)` | `(164,100)` |
| Brick variant (Underground) | — | `(181,16)` | — | — |
| Stone/solid | `(34,16)` | `(198,16)` | `(34,100)` | `(181,100)` |
| Question | `(298,78)` | `(394,78)` | `(490,78)` | `(586,78)` |
| Used/empty | `(349,78)` | `(445,78)` | `(541,78)` | `(637,78)` |
| Normal coin frame 1 | `(298,95)` | `(394,95)` | `(490,95)` | `(586,95)` |

Pipe, pole, coin animation và bbox lâu đài/pipe assembled được ghi đầy đủ ở
[`tileset_coordinate.md`](tileset_coordinate.md). `QuestionBlock` popup coin
vẫn lấy từ `items_objects.png`; coin đặt trực tiếp trên map lấy các frame coin
riêng của tileset.

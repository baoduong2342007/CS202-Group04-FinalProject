# Bảng Tọa Độ & Ý Nghĩa Chi Tiết Khung Hình Vật Thể & Items (`items_objects.png`)

Danh sách toàn bộ **266** thành phần vật thể, vật phẩm hỗ trợ (Items) và cấu trúc môi trường tách trực tiếp từ tệp nguồn gốc **`assets/textures/items/items_objects.png`** (kích thước `592 × 572 px`), phân tích theo thuật toán loang (connected component) và khớp khung ô Grid tiêu chuẩn.

> 📌 **NGUỒN DỮ LIỆU CHÍNH THỨC:**
> Tài liệu này (`items_objects_coordinate.md`) cùng tệp ảnh minh họa Atlas **`docs/assets/reference/items_objects_all_components_atlas_full.png`** được phân tích dựa trên vùng vật thể chứa khung tím của tệp **`items_objects.png`** (chứa Nấm Super/1-Up, Hoa Lửa Fire Flower, Sao Bất Tử Super Star, Đạn lửa Fireball, Lò xo Trampoline, Bệ nâng Moving Platform, Cờ đích Flagpole & Dây leo Beanstalk).

> ⚠️ **Lưu ý**: Các khung hình `Block Debris` (mảnh vỡ gạch) đã được loại bỏ khỏi danh sách này theo đúng logic vì chúng thuộc nhóm Blocks.

> **Giải thích hai cột tọa độ & kích thước:**
> - **Tọa độ Loang (X, Y) & Kích thước Loang (W × H)**: Bounding box thực tế ôm sát từng điểm ảnh (pixel) của vật thể do thuật toán loang tìm thấy.
> - **Tọa độ Ô Grid Khung (X, Y) & Kích thước Khung (W × H)**: Khung ô chuẩn trên Spritesheet dùng cho game engine (`sf::IntRect`).

> **Quy ước bảng màu NES (Palette):**
> - **OW (Overworld)**: Bảng màu ngoài trời (Màn 1-1, 1-3...)
> - **UG (Underground)**: Bảng màu lòng đất / hang động (Màn 1-2, 4-2...)
> - **Castle (CT)**: Bảng màu lâu đài (Màn 1-4, 2-4...)
> - **UW (Underwater)**: Bảng màu dưới nước (Màn 2-2, 7-2...)

---

## Overworld Objects (Môi trường ngoài trời)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang | Tọa độ Grid Khung (X, Y) | Kích thước Khung | Số Pixel | Palette | Ý Nghĩa / Trạng Thái Khung Hình |
|-----|-----------------------|------------------|----------------------|------------------|----------|---------|----------------------------------|
| **#1** | `(0, 8)` | `16 × 16 px` | `(0, 8)` | `16 × 16 px` | 176 px | OW Palette | Super Mushroom (Nấm đỏ / tăng kích thước) |
| **#2** | `(32, 8)` | `16 × 16 px` | `(32, 8)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — Bộ 1 (Khung hoạt họa #1) |
| **#3** | `(50, 8)` | `16 × 16 px` | `(50, 8)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — Bộ 1 (Khung hoạt họa #2) |
| **#4** | `(68, 8)` | `16 × 16 px` | `(68, 8)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — Bộ 1 (Khung hoạt họa #3) |
| **#5** | `(86, 8)` | `16 × 16 px` | `(86, 8)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — Bộ 1 (Khung hoạt họa #4) |
| **#6** | `(107, 8)` | `14 × 16 px` | `(106, 8)` | `16 × 16 px` | 122 px | OW Palette | Super Star — Bộ 1 (Khung hoạt họa #1) |
| **#7** | `(125, 8)` | `14 × 16 px` | `(124, 8)` | `16 × 16 px` | 122 px | OW Palette | Super Star — Bộ 1 (Khung hoạt họa #2) |
| **#8** | `(143, 8)` | `14 × 16 px` | `(142, 8)` | `16 × 16 px` | 122 px | OW Palette | Super Star — Bộ 1 (Khung hoạt họa #3) |
| **#9** | `(161, 8)` | `14 × 16 px` | `(160, 8)` | `16 × 16 px` | 122 px | OW Palette | Super Star — Bộ 1 (Khung hoạt họa #4) |
| **#10** | `(0, 26)` | `16 × 16 px` | `(0, 26)` | `16 × 16 px` | 176 px | OW Palette | 1-Up Mushroom (Nấm xanh lá / tăng 1 mạng) |
| **#11** | `(32, 26)` | `16 × 16 px` | `(32, 26)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — Bộ 2 (Khung hoạt họa biến thể #1) |
| **#12** | `(50, 26)` | `16 × 16 px` | `(50, 26)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — Bộ 2 (Khung hoạt họa biến thể #2) |
| **#13** | `(68, 26)` | `16 × 16 px` | `(68, 26)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — Bộ 2 (Khung hoạt họa biến thể #3) |
| **#14** | `(86, 26)` | `16 × 16 px` | `(86, 26)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — Bộ 2 (Khung hoạt họa biến thể #4) |
| **#15** | `(107, 26)` | `14 × 16 px` | `(106, 26)` | `16 × 16 px` | 122 px | OW Palette | Super Star — Bộ 2 (Khung hoạt họa biến thể #1) |
| **#16** | `(125, 26)` | `14 × 16 px` | `(124, 26)` | `16 × 16 px` | 122 px | OW Palette | Super Star — Bộ 2 (Khung hoạt họa biến thể #2) |
| **#17** | `(143, 26)` | `14 × 16 px` | `(142, 26)` | `16 × 16 px` | 122 px | OW Palette | Super Star — Bộ 2 (Khung hoạt họa biến thể #3) |
| **#18** | `(161, 26)` | `14 × 16 px` | `(160, 26)` | `16 × 16 px` | 122 px | OW Palette | Super Star — Bộ 2 (Khung hoạt họa biến thể #4) |
| **#19** | `(180, 37)` | `8 × 14 px` | `(180, 36)` | `8 × 16 px` | 84 px | OW Palette | Spinning Coin (Đồng xu nảy ra từ Block / Xu lơ lửng - Khung #1) |
| **#20** | `(192, 37)` | `4 × 14 px` | `(190, 36)` | `8 × 16 px` | 48 px | OW Palette | Spinning Coin (Đồng xu nảy ra từ Block / Xu lơ lửng - Khung #2) |
| **#21** | `(204, 37)` | `1 × 14 px` | `(200, 36)` | `8 × 16 px` | 14 px | OW Palette | Spinning Coin (Đồng xu nảy ra từ Block / Xu lơ lửng - Khung #3) |
| **#22** | `(212, 37)` | `4 × 14 px` | `(210, 36)` | `8 × 16 px` | 48 px | OW Palette | Spinning Coin (Đồng xu nảy ra từ Block / Xu lơ lửng - Khung #4) |
| **#23** | `(235, 26)` | `11 × 8 px` | `(234, 26)` | `16 × 8 px` | 43 px | OW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#24** | `(234, 36)` | `12 × 8 px` | `(234, 36)` | `16 × 8 px` | 46 px | OW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#25** | `(253, 26)` | `15 × 8 px` | `(252, 26)` | `16 × 8 px` | 58 px | OW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#26** | `(252, 36)` | `16 × 8 px` | `(252, 36)` | `16 × 8 px` | 61 px | OW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#27** | `(32, 44)` | `16 × 16 px` | `(32, 44)` | `16 × 16 px` | 162 px | OW Palette | Trampoline / Springboard (Trạng thái tĩnh nguyên vẹn) |
| **#28** | `(50, 44)` | `16 × 16 px` | `(50, 44)` | `16 × 16 px` | 162 px | OW Palette | Vật thể phụ kiện / Frame môi trường (OW Palette) |
| **#29** | `(68, 44)` | `16 × 16 px` | `(68, 44)` | `16 × 16 px` | 162 px | OW Palette | Vật thể phụ kiện / Frame môi trường (OW Palette) |
| **#30** | `(86, 44)` | `16 × 16 px` | `(86, 44)` | `16 × 16 px` | 162 px | OW Palette | Vật thể phụ kiện / Frame môi trường (OW Palette) |
| **#31** | `(107, 44)` | `14 × 16 px` | `(106, 44)` | `16 × 16 px` | 122 px | OW Palette | Vật thể phụ kiện / Frame môi trường (OW Palette) |
| **#32** | `(125, 44)` | `14 × 16 px` | `(124, 44)` | `16 × 16 px` | 122 px | OW Palette | Vật thể phụ kiện / Frame môi trường (OW Palette) |
| **#33** | `(143, 44)` | `14 × 16 px` | `(142, 44)` | `16 × 16 px` | 122 px | OW Palette | Vật thể phụ kiện / Frame môi trường (OW Palette) |
| **#34** | `(161, 44)` | `14 × 16 px` | `(160, 44)` | `16 × 16 px` | 122 px | OW Palette | Vật thể phụ kiện / Frame môi trường (OW Palette) |
| **#35** | `(180, 54)` | `8 × 8 px` | `(180, 54)` | `8 × 8 px` | 42 px | OW Palette | Mario Fireball (Khung đạn lửa xoay tròn #1) |
| **#36** | `(190, 54)` | `8 × 8 px` | `(190, 54)` | `8 × 8 px` | 43 px | OW Palette | Mario Fireball (Khung đạn lửa xoay tròn #2) |
| **#37** | `(200, 54)` | `8 × 8 px` | `(200, 54)` | `8 × 8 px` | 42 px | OW Palette | Mario Fireball (Khung đạn lửa xoay tròn #3) |
| **#38** | `(210, 54)` | `8 × 8 px` | `(210, 54)` | `8 × 8 px` | 43 px | OW Palette | Mario Fireball (Khung đạn lửa xoay tròn #4) |
| **#39** | `(234, 46)` | `12 × 8 px` | `(234, 46)` | `16 × 8 px` | 48 px | OW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#40** | `(234, 56)` | `12 × 8 px` | `(234, 56)` | `16 × 8 px` | 48 px | OW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#41** | `(252, 46)` | `16 × 8 px` | `(252, 46)` | `16 × 8 px` | 63 px | OW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#42** | `(252, 56)` | `16 × 8 px` | `(252, 56)` | `16 × 8 px` | 63 px | OW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#43** | `(32, 64)` | `16 × 8 px` | `(32, 64)` | `16 × 8 px` | 100 px | OW Palette | Trampoline / Springboard (Đệm lò xo nén dẹt) |
| **#44** | `(32, 75)` | `16 × 31 px` | `(32, 75)` | `16 × 31 px` | 232 px | OW Palette | Trampoline / Springboard (Đệm lò xo bung nảy cực đại) |
| **#45** | `(52, 64)` | `24 × 8 px` | `(52, 64)` | `24 × 8 px` | 150 px | OW Palette | Moving Platform (Mặt bệ nâng di động - Loại ngắn 24px) |
| **#46** | `(80, 64)` | `32 × 8 px` | `(80, 64)` | `32 × 8 px` | 200 px | OW Palette | Firebar (Thanh lửa quay trong Castle - Trạng thái ngang) |
| **#47** | `(80, 74)` | `32 × 8 px` | `(80, 74)` | `32 × 8 px` | 208 px | OW Palette | Firebar (Thanh lửa quay trong Castle - Trạng thái ngang) |
| **#48** | `(116, 64)` | `48 × 8 px` | `(116, 64)` | `48 × 8 px` | 300 px | OW Palette | Moving Platform (Mặt bệ nâng di động dầm ngang - Loại dài 48px) |
| **#49** | `(116, 74)` | `48 × 8 px` | `(116, 74)` | `48 × 8 px` | 312 px | OW Palette | Moving Platform (Mặt bệ nâng di động dầm ngang - Loại dài 48px) |
| **#50** | `(184, 68)` | `8 × 8 px` | `(180, 64)` | `16 × 16 px` | 44 px | OW Palette | Fireball Explosion (Khung hiệu ứng nổ đạn lửa/pháo hoa) |
| **#51** | `(200, 65)` | `12 × 14 px` | `(198, 64)` | `16 × 16 px` | 112 px | OW Palette | Fireball Explosion (Khung hiệu ứng nổ đạn lửa/pháo hoa) |
| **#52** | `(216, 64)` | `16 × 16 px` | `(216, 64)` | `16 × 16 px` | 200 px | OW Palette | Fireball Explosion (Khung hiệu ứng nổ đạn lửa/pháo hoa) |
| **#53** | `(234, 66)` | `12 × 8 px` | `(234, 66)` | `16 × 8 px` | 47 px | OW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#54** | `(252, 66)` | `16 × 8 px` | `(252, 66)` | `16 × 8 px` | 62 px | OW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#55** | `(252, 76)` | `16 × 7 px` | `(252, 76)` | `16 × 8 px` | 61 px | OW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#56** | `(50, 83)` | `16 × 23 px` | `(50, 83)` | `16 × 23 px` | 228 px | OW Palette | Trampoline / Springboard (Khung trạng thái chờ/trung gian của lò xo) |
| **#57** | `(68, 91)` | `16 × 15 px` | `(68, 91)` | `16 × 15 px` | 220 px | OW Palette | Flagpole Flag (Cờ đích kéo xuống ở cuối màn) |
| **#58** | `(92, 90)` | `16 × 16 px` | `(92, 90)` | `16 × 16 px` | 136 px | OW Palette | Castle Flag / Star Flag (Cờ trên đỉnh lâu đài) |
| **#59** | `(110, 90)` | `13 × 16 px` | `(110, 90)` | `16 × 16 px` | 117 px | OW Palette | Beanstalk / Vine Top (Đỉnh/ngọn dây leo mọc vút lên) |
| **#60** | `(131, 90)` | `14 × 16 px` | `(130, 90)` | `16 × 16 px` | 71 px | OW Palette | Beanstalk / Vine Stem 1 (Thân dây leo đoạn 1) |
| **#61** | `(149, 90)` | `14 × 16 px` | `(148, 90)` | `16 × 16 px` | 76 px | OW Palette | Beanstalk / Vine Stem 2 (Thân dây leo đoạn 2) |

---

## Underground Objects (Môi trường lòng đất)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang | Tọa độ Grid Khung (X, Y) | Kích thước Khung | Số Pixel | Palette | Ý Nghĩa / Trạng Thái Khung Hình |
|-----|-----------------------|------------------|----------------------|------------------|----------|---------|----------------------------------|
| **#62** | `(0, 116)` | `16 × 16 px` | `(0, 116)` | `16 × 16 px` | 176 px | UG Palette | Super Mushroom (Nấm đỏ / tăng kích thước) |
| **#63** | `(32, 116)` | `16 × 16 px` | `(32, 116)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — Bộ 1 (Khung hoạt họa #1) |
| **#64** | `(50, 116)` | `16 × 16 px` | `(50, 116)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — Bộ 1 (Khung hoạt họa #2) |
| **#65** | `(68, 116)` | `16 × 16 px` | `(68, 116)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — Bộ 1 (Khung hoạt họa #3) |
| **#66** | `(86, 116)` | `16 × 16 px` | `(86, 116)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — Bộ 1 (Khung hoạt họa #4) |
| **#67** | `(107, 116)` | `14 × 16 px` | `(106, 116)` | `16 × 16 px` | 122 px | UG Palette | Super Star — Bộ 1 (Khung hoạt họa #1) |
| **#68** | `(125, 116)` | `14 × 16 px` | `(124, 116)` | `16 × 16 px` | 122 px | UG Palette | Super Star — Bộ 1 (Khung hoạt họa #2) |
| **#69** | `(143, 116)` | `14 × 16 px` | `(142, 116)` | `16 × 16 px` | 122 px | UG Palette | Super Star — Bộ 1 (Khung hoạt họa #3) |
| **#70** | `(161, 116)` | `14 × 16 px` | `(160, 116)` | `16 × 16 px` | 122 px | UG Palette | Super Star — Bộ 1 (Khung hoạt họa #4) |
| **#71** | `(0, 134)` | `16 × 16 px` | `(0, 134)` | `16 × 16 px` | 176 px | UG Palette | 1-Up Mushroom (Nấm xanh lá / tăng 1 mạng) |
| **#72** | `(32, 134)` | `16 × 16 px` | `(32, 134)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — Bộ 2 (Khung hoạt họa biến thể #1) |
| **#73** | `(50, 134)` | `16 × 16 px` | `(50, 134)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — Bộ 2 (Khung hoạt họa biến thể #2) |
| **#74** | `(68, 134)` | `16 × 16 px` | `(68, 134)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — Bộ 2 (Khung hoạt họa biến thể #3) |
| **#75** | `(86, 134)` | `16 × 16 px` | `(86, 134)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — Bộ 2 (Khung hoạt họa biến thể #4) |
| **#76** | `(107, 134)` | `14 × 16 px` | `(106, 134)` | `16 × 16 px` | 122 px | UG Palette | Super Star — Bộ 2 (Khung hoạt họa biến thể #1) |
| **#77** | `(125, 134)` | `14 × 16 px` | `(124, 134)` | `16 × 16 px` | 122 px | UG Palette | Super Star — Bộ 2 (Khung hoạt họa biến thể #2) |
| **#78** | `(143, 134)` | `14 × 16 px` | `(142, 134)` | `16 × 16 px` | 122 px | UG Palette | Super Star — Bộ 2 (Khung hoạt họa biến thể #3) |
| **#79** | `(161, 134)` | `14 × 16 px` | `(160, 134)` | `16 × 16 px` | 122 px | UG Palette | Super Star — Bộ 2 (Khung hoạt họa biến thể #4) |
| **#80** | `(235, 134)` | `11 × 8 px` | `(234, 134)` | `16 × 8 px` | 43 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#81** | `(253, 134)` | `15 × 8 px` | `(252, 134)` | `16 × 8 px` | 58 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#82** | `(271, 134)` | `11 × 8 px` | `(270, 134)` | `16 × 8 px` | 43 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#83** | `(289, 134)` | `15 × 8 px` | `(288, 134)` | `16 × 8 px` | 58 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#84** | `(32, 152)` | `16 × 16 px` | `(32, 152)` | `16 × 16 px` | 162 px | UG Palette | Trampoline / Springboard (Trạng thái tĩnh nguyên vẹn) |
| **#85** | `(50, 152)` | `16 × 16 px` | `(50, 152)` | `16 × 16 px` | 162 px | UG Palette | Vật thể phụ kiện / Frame môi trường (UG Palette) |
| **#86** | `(68, 152)` | `16 × 16 px` | `(68, 152)` | `16 × 16 px` | 162 px | UG Palette | Vật thể phụ kiện / Frame môi trường (UG Palette) |
| **#87** | `(86, 152)` | `16 × 16 px` | `(86, 152)` | `16 × 16 px` | 162 px | UG Palette | Vật thể phụ kiện / Frame môi trường (UG Palette) |
| **#88** | `(107, 152)` | `14 × 16 px` | `(106, 152)` | `16 × 16 px` | 122 px | UG Palette | Vật thể phụ kiện / Frame môi trường (UG Palette) |
| **#89** | `(125, 152)` | `14 × 16 px` | `(124, 152)` | `16 × 16 px` | 122 px | UG Palette | Vật thể phụ kiện / Frame môi trường (UG Palette) |
| **#90** | `(143, 152)` | `14 × 16 px` | `(142, 152)` | `16 × 16 px` | 122 px | UG Palette | Vật thể phụ kiện / Frame môi trường (UG Palette) |
| **#91** | `(161, 152)` | `14 × 16 px` | `(160, 152)` | `16 × 16 px` | 122 px | UG Palette | Vật thể phụ kiện / Frame môi trường (UG Palette) |
| **#92** | `(180, 145)` | `8 × 14 px` | `(180, 144)` | `8 × 16 px` | 84 px | UG Palette | Spinning Coin (Đồng xu nảy ra từ Block / Xu lơ lửng - Khung #1) |
| **#93** | `(192, 145)` | `4 × 14 px` | `(190, 144)` | `8 × 16 px` | 48 px | UG Palette | Spinning Coin (Đồng xu nảy ra từ Block / Xu lơ lửng - Khung #2) |
| **#94** | `(204, 145)` | `1 × 14 px` | `(200, 144)` | `8 × 16 px` | 14 px | UG Palette | Spinning Coin (Đồng xu nảy ra từ Block / Xu lơ lửng - Khung #3) |
| **#95** | `(212, 145)` | `4 × 14 px` | `(210, 144)` | `8 × 16 px` | 48 px | UG Palette | Spinning Coin (Đồng xu nảy ra từ Block / Xu lơ lửng - Khung #4) |
| **#96** | `(234, 144)` | `12 × 8 px` | `(234, 144)` | `16 × 8 px` | 46 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#97** | `(234, 154)` | `12 × 8 px` | `(234, 154)` | `16 × 8 px` | 48 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#98** | `(252, 144)` | `16 × 8 px` | `(252, 144)` | `16 × 8 px` | 61 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#99** | `(252, 154)` | `16 × 8 px` | `(252, 154)` | `16 × 8 px` | 63 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#100** | `(270, 144)` | `12 × 8 px` | `(270, 144)` | `16 × 8 px` | 46 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#101** | `(270, 154)` | `12 × 8 px` | `(270, 154)` | `16 × 8 px` | 48 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#102** | `(288, 144)` | `16 × 8 px` | `(288, 144)` | `16 × 8 px` | 61 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#103** | `(288, 154)` | `16 × 8 px` | `(288, 154)` | `16 × 8 px` | 63 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#104** | `(32, 172)` | `16 × 8 px` | `(32, 172)` | `16 × 8 px` | 100 px | UG Palette | Trampoline / Springboard (Đệm lò xo nén dẹt) |
| **#105** | `(52, 172)` | `24 × 8 px` | `(52, 172)` | `24 × 8 px` | 150 px | UG Palette | Moving Platform (Mặt bệ nâng di động - Loại ngắn 24px) |
| **#106** | `(80, 172)` | `32 × 8 px` | `(80, 172)` | `32 × 8 px` | 200 px | UG Palette | Firebar (Thanh lửa quay trong Castle - Trạng thái ngang) |
| **#107** | `(116, 172)` | `48 × 8 px` | `(116, 172)` | `48 × 8 px` | 300 px | UG Palette | Moving Platform (Mặt bệ nâng di động dầm ngang - Loại dài 48px) |
| **#108** | `(180, 162)` | `8 × 8 px` | `(180, 162)` | `8 × 8 px` | 42 px | UG Palette | Mario Fireball (Khung đạn lửa xoay tròn #1) |
| **#109** | `(184, 176)` | `8 × 8 px` | `(180, 172)` | `16 × 16 px` | 44 px | UG Palette | Fireball Explosion (Khung hiệu ứng nổ đạn lửa/pháo hoa) |
| **#110** | `(190, 162)` | `8 × 8 px` | `(190, 162)` | `8 × 8 px` | 43 px | UG Palette | Mario Fireball (Khung đạn lửa xoay tròn #2) |
| **#111** | `(200, 173)` | `12 × 14 px` | `(198, 172)` | `16 × 16 px` | 112 px | UG Palette | Fireball Explosion (Khung hiệu ứng nổ đạn lửa/pháo hoa) |
| **#112** | `(200, 162)` | `8 × 8 px` | `(200, 162)` | `8 × 8 px` | 42 px | UG Palette | Mario Fireball (Khung đạn lửa xoay tròn #3) |
| **#113** | `(210, 162)` | `8 × 8 px` | `(210, 162)` | `8 × 8 px` | 43 px | UG Palette | Mario Fireball (Khung đạn lửa xoay tròn #4) |
| **#114** | `(216, 172)` | `16 × 16 px` | `(216, 172)` | `16 × 16 px` | 200 px | UG Palette | Fireball Explosion (Khung hiệu ứng nổ đạn lửa/pháo hoa) |
| **#115** | `(234, 164)` | `12 × 8 px` | `(234, 164)` | `16 × 8 px` | 48 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#116** | `(234, 174)` | `12 × 8 px` | `(234, 174)` | `16 × 8 px` | 47 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#117** | `(252, 164)` | `16 × 8 px` | `(252, 164)` | `16 × 8 px` | 63 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#118** | `(252, 174)` | `16 × 8 px` | `(252, 174)` | `16 × 8 px` | 62 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#119** | `(270, 164)` | `12 × 8 px` | `(270, 164)` | `16 × 8 px` | 48 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#120** | `(270, 174)` | `12 × 8 px` | `(270, 174)` | `16 × 8 px` | 47 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#121** | `(288, 164)` | `16 × 8 px` | `(288, 164)` | `16 × 8 px` | 63 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#122** | `(288, 174)` | `16 × 8 px` | `(288, 174)` | `16 × 8 px` | 62 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#123** | `(32, 183)` | `16 × 31 px` | `(32, 183)` | `16 × 31 px` | 232 px | UG Palette | Trampoline / Springboard (Đệm lò xo bung nảy cực đại) |
| **#124** | `(50, 191)` | `16 × 23 px` | `(50, 191)` | `16 × 23 px` | 228 px | UG Palette | Trampoline / Springboard (Khung trạng thái chờ/trung gian của lò xo) |
| **#125** | `(68, 199)` | `16 × 15 px` | `(68, 199)` | `16 × 15 px` | 220 px | UG Palette | Flagpole Flag (Cờ đích kéo xuống ở cuối màn) |
| **#126** | `(80, 182)` | `32 × 8 px` | `(80, 182)` | `32 × 8 px` | 208 px | UG Palette | Firebar (Thanh lửa quay trong Castle - Trạng thái ngang) |
| **#127** | `(92, 198)` | `16 × 16 px` | `(92, 198)` | `16 × 16 px` | 136 px | UG Palette | Castle Flag / Star Flag (Cờ trên đỉnh lâu đài) |
| **#128** | `(110, 198)` | `13 × 16 px` | `(110, 198)` | `16 × 16 px` | 117 px | UG Palette | Beanstalk / Vine Top (Đỉnh/ngọn dây leo mọc vút lên) |
| **#129** | `(116, 182)` | `48 × 8 px` | `(116, 182)` | `48 × 8 px` | 312 px | UG Palette | Moving Platform (Mặt bệ nâng di động dầm ngang - Loại dài 48px) |
| **#130** | `(131, 198)` | `14 × 16 px` | `(130, 198)` | `16 × 16 px` | 71 px | UG Palette | Beanstalk / Vine Stem 1 (Thân dây leo đoạn 1) |
| **#131** | `(149, 198)` | `14 × 16 px` | `(148, 198)` | `16 × 16 px` | 76 px | UG Palette | Beanstalk / Vine Stem 2 (Thân dây leo đoạn 2) |
| **#132** | `(252, 184)` | `16 × 7 px` | `(252, 184)` | `16 × 8 px` | 61 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#133** | `(288, 184)` | `16 × 7 px` | `(288, 184)` | `16 × 8 px` | 61 px | UG Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |

---

## Castle Objects (Môi trường lâu đài)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang | Tọa độ Grid Khung (X, Y) | Kích thước Khung | Số Pixel | Palette | Ý Nghĩa / Trạng Thái Khung Hình |
|-----|-----------------------|------------------|----------------------|------------------|----------|---------|----------------------------------|
| **#134** | `(0, 224)` | `16 × 16 px` | `(0, 224)` | `16 × 16 px` | 176 px | Castle Palette | Super Mushroom (Nấm đỏ / tăng kích thước) |
| **#135** | `(32, 224)` | `16 × 16 px` | `(32, 224)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — Bộ 1 (Khung hoạt họa #1) |
| **#136** | `(50, 224)` | `16 × 16 px` | `(50, 224)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — Bộ 1 (Khung hoạt họa #2) |
| **#137** | `(68, 224)` | `16 × 16 px` | `(68, 224)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — Bộ 1 (Khung hoạt họa #3) |
| **#138** | `(86, 224)` | `16 × 16 px` | `(86, 224)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — Bộ 1 (Khung hoạt họa #4) |
| **#139** | `(107, 224)` | `14 × 16 px` | `(106, 224)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — Bộ 1 (Khung hoạt họa #1) |
| **#140** | `(125, 224)` | `14 × 16 px` | `(124, 224)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — Bộ 1 (Khung hoạt họa #2) |
| **#141** | `(143, 224)` | `14 × 16 px` | `(142, 224)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — Bộ 1 (Khung hoạt họa #3) |
| **#142** | `(161, 224)` | `14 × 16 px` | `(160, 224)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — Bộ 1 (Khung hoạt họa #4) |
| **#143** | `(0, 242)` | `16 × 16 px` | `(0, 242)` | `16 × 16 px` | 176 px | Castle Palette | 1-Up Mushroom (Nấm xanh lá / tăng 1 mạng) |
| **#144** | `(32, 242)` | `16 × 16 px` | `(32, 242)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — Bộ 2 (Khung hoạt họa biến thể #1) |
| **#145** | `(50, 242)` | `16 × 16 px` | `(50, 242)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — Bộ 2 (Khung hoạt họa biến thể #2) |
| **#146** | `(68, 242)` | `16 × 16 px` | `(68, 242)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — Bộ 2 (Khung hoạt họa biến thể #3) |
| **#147** | `(86, 242)` | `16 × 16 px` | `(86, 242)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — Bộ 2 (Khung hoạt họa biến thể #4) |
| **#148** | `(107, 242)` | `14 × 16 px` | `(106, 242)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — Bộ 2 (Khung hoạt họa biến thể #1) |
| **#149** | `(125, 242)` | `14 × 16 px` | `(124, 242)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — Bộ 2 (Khung hoạt họa biến thể #2) |
| **#150** | `(143, 242)` | `14 × 16 px` | `(142, 242)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — Bộ 2 (Khung hoạt họa biến thể #3) |
| **#151** | `(161, 242)` | `14 × 16 px` | `(160, 242)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — Bộ 2 (Khung hoạt họa biến thể #4) |
| **#152** | `(180, 253)` | `8 × 14 px` | `(180, 252)` | `8 × 16 px` | 84 px | Castle Palette | Spinning Coin (Đồng xu nảy ra từ Block / Xu lơ lửng - Khung #1) |
| **#153** | `(192, 253)` | `4 × 14 px` | `(190, 252)` | `8 × 16 px` | 48 px | Castle Palette | Spinning Coin (Đồng xu nảy ra từ Block / Xu lơ lửng - Khung #2) |
| **#154** | `(204, 253)` | `1 × 14 px` | `(200, 252)` | `8 × 16 px` | 14 px | Castle Palette | Spinning Coin (Đồng xu nảy ra từ Block / Xu lơ lửng - Khung #3) |
| **#155** | `(212, 253)` | `4 × 14 px` | `(210, 252)` | `8 × 16 px` | 48 px | Castle Palette | Spinning Coin (Đồng xu nảy ra từ Block / Xu lơ lửng - Khung #4) |
| **#156** | `(235, 242)` | `11 × 8 px` | `(234, 242)` | `16 × 8 px` | 43 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#157** | `(234, 252)` | `12 × 8 px` | `(234, 252)` | `16 × 8 px` | 46 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#158** | `(253, 242)` | `15 × 8 px` | `(252, 242)` | `16 × 8 px` | 58 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#159** | `(252, 252)` | `16 × 8 px` | `(252, 252)` | `16 × 8 px` | 61 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#160** | `(271, 242)` | `11 × 8 px` | `(270, 242)` | `16 × 8 px` | 43 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#161** | `(270, 252)` | `12 × 8 px` | `(270, 252)` | `16 × 8 px` | 46 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#162** | `(289, 242)` | `15 × 8 px` | `(288, 242)` | `16 × 8 px` | 58 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#163** | `(288, 252)` | `16 × 8 px` | `(288, 252)` | `16 × 8 px` | 61 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#164** | `(32, 260)` | `16 × 16 px` | `(32, 260)` | `16 × 16 px` | 162 px | Castle Palette | Trampoline / Springboard (Trạng thái tĩnh nguyên vẹn) |
| **#165** | `(50, 260)` | `16 × 16 px` | `(50, 260)` | `16 × 16 px` | 162 px | Castle Palette | Vật thể phụ kiện / Frame môi trường (Castle Palette) |
| **#166** | `(68, 260)` | `16 × 16 px` | `(68, 260)` | `16 × 16 px` | 162 px | Castle Palette | Vật thể phụ kiện / Frame môi trường (Castle Palette) |
| **#167** | `(86, 260)` | `16 × 16 px` | `(86, 260)` | `16 × 16 px` | 162 px | Castle Palette | Vật thể phụ kiện / Frame môi trường (Castle Palette) |
| **#168** | `(107, 260)` | `14 × 16 px` | `(106, 260)` | `16 × 16 px` | 122 px | Castle Palette | Vật thể phụ kiện / Frame môi trường (Castle Palette) |
| **#169** | `(125, 260)` | `14 × 16 px` | `(124, 260)` | `16 × 16 px` | 122 px | Castle Palette | Vật thể phụ kiện / Frame môi trường (Castle Palette) |
| **#170** | `(143, 260)` | `14 × 16 px` | `(142, 260)` | `16 × 16 px` | 122 px | Castle Palette | Vật thể phụ kiện / Frame môi trường (Castle Palette) |
| **#171** | `(161, 260)` | `14 × 16 px` | `(160, 260)` | `16 × 16 px` | 122 px | Castle Palette | Vật thể phụ kiện / Frame môi trường (Castle Palette) |
| **#172** | `(180, 270)` | `8 × 8 px` | `(180, 270)` | `8 × 8 px` | 42 px | Castle Palette | Mario Fireball (Khung đạn lửa xoay tròn #1) |
| **#173** | `(190, 270)` | `8 × 8 px` | `(190, 270)` | `8 × 8 px` | 43 px | Castle Palette | Mario Fireball (Khung đạn lửa xoay tròn #2) |
| **#174** | `(200, 270)` | `8 × 8 px` | `(200, 270)` | `8 × 8 px` | 42 px | Castle Palette | Mario Fireball (Khung đạn lửa xoay tròn #3) |
| **#175** | `(210, 270)` | `8 × 8 px` | `(210, 270)` | `8 × 8 px` | 43 px | Castle Palette | Mario Fireball (Khung đạn lửa xoay tròn #4) |
| **#176** | `(234, 262)` | `12 × 8 px` | `(234, 262)` | `16 × 8 px` | 48 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#177** | `(234, 272)` | `12 × 8 px` | `(234, 272)` | `16 × 8 px` | 48 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#178** | `(252, 262)` | `16 × 8 px` | `(252, 262)` | `16 × 8 px` | 63 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#179** | `(252, 272)` | `16 × 8 px` | `(252, 272)` | `16 × 8 px` | 63 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#180** | `(270, 262)` | `12 × 8 px` | `(270, 262)` | `16 × 8 px` | 48 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#181** | `(270, 272)` | `12 × 8 px` | `(270, 272)` | `16 × 8 px` | 48 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#182** | `(288, 262)` | `16 × 8 px` | `(288, 262)` | `16 × 8 px` | 63 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#183** | `(288, 272)` | `16 × 8 px` | `(288, 272)` | `16 × 8 px` | 63 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#184** | `(32, 280)` | `16 × 8 px` | `(32, 280)` | `16 × 8 px` | 100 px | Castle Palette | Trampoline / Springboard (Đệm lò xo nén dẹt) |
| **#185** | `(32, 291)` | `16 × 31 px` | `(32, 291)` | `16 × 31 px` | 232 px | Castle Palette | Trampoline / Springboard (Đệm lò xo bung nảy cực đại) |
| **#186** | `(50, 299)` | `16 × 23 px` | `(50, 299)` | `16 × 23 px` | 228 px | Castle Palette | Trampoline / Springboard (Khung trạng thái chờ/trung gian của lò xo) |
| **#187** | `(52, 280)` | `24 × 8 px` | `(52, 280)` | `24 × 8 px` | 150 px | Castle Palette | Moving Platform (Mặt bệ nâng di động - Loại ngắn 24px) |
| **#188** | `(80, 280)` | `32 × 8 px` | `(80, 280)` | `32 × 8 px` | 200 px | Castle Palette | Firebar (Thanh lửa quay trong Castle - Trạng thái ngang) |
| **#189** | `(80, 290)` | `32 × 8 px` | `(80, 290)` | `32 × 8 px` | 208 px | Castle Palette | Firebar (Thanh lửa quay trong Castle - Trạng thái ngang) |
| **#190** | `(116, 280)` | `48 × 8 px` | `(116, 280)` | `48 × 8 px` | 300 px | Castle Palette | Moving Platform (Mặt bệ nâng di động dầm ngang - Loại dài 48px) |
| **#191** | `(116, 290)` | `48 × 8 px` | `(116, 290)` | `48 × 8 px` | 312 px | Castle Palette | Moving Platform (Mặt bệ nâng di động dầm ngang - Loại dài 48px) |
| **#192** | `(184, 284)` | `8 × 8 px` | `(180, 280)` | `16 × 16 px` | 44 px | Castle Palette | Fireball Explosion (Khung hiệu ứng nổ đạn lửa/pháo hoa) |
| **#193** | `(200, 281)` | `12 × 14 px` | `(198, 280)` | `16 × 16 px` | 112 px | Castle Palette | Fireball Explosion (Khung hiệu ứng nổ đạn lửa/pháo hoa) |
| **#194** | `(216, 280)` | `16 × 16 px` | `(216, 280)` | `16 × 16 px` | 200 px | Castle Palette | Fireball Explosion (Khung hiệu ứng nổ đạn lửa/pháo hoa) |
| **#195** | `(234, 282)` | `12 × 8 px` | `(234, 282)` | `16 × 8 px` | 47 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#196** | `(252, 282)` | `16 × 8 px` | `(252, 282)` | `16 × 8 px` | 62 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#197** | `(252, 292)` | `16 × 7 px` | `(252, 292)` | `16 × 8 px` | 61 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#198** | `(270, 282)` | `12 × 8 px` | `(270, 282)` | `16 × 8 px` | 47 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#199** | `(288, 282)` | `16 × 8 px` | `(288, 282)` | `16 × 8 px` | 62 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#200** | `(288, 292)` | `16 × 7 px` | `(288, 292)` | `16 × 8 px` | 61 px | Castle Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#201** | `(68, 307)` | `16 × 15 px` | `(68, 307)` | `16 × 15 px` | 220 px | Castle Palette | Flagpole Flag (Cờ đích kéo xuống ở cuối màn) |
| **#202** | `(92, 306)` | `16 × 16 px` | `(92, 306)` | `16 × 16 px` | 136 px | Castle Palette | Castle Flag / Star Flag (Cờ trên đỉnh lâu đài) |
| **#203** | `(110, 306)` | `13 × 16 px` | `(110, 306)` | `16 × 16 px` | 117 px | Castle Palette | Beanstalk / Vine Top (Đỉnh/ngọn dây leo mọc vút lên) |
| **#204** | `(131, 306)` | `14 × 16 px` | `(130, 306)` | `16 × 16 px` | 71 px | Castle Palette | Beanstalk / Vine Stem 1 (Thân dây leo đoạn 1) |
| **#205** | `(149, 306)` | `14 × 16 px` | `(148, 306)` | `16 × 16 px` | 76 px | Castle Palette | Beanstalk / Vine Stem 2 (Thân dây leo đoạn 2) |

---

## Underwater Objects (Môi trường dưới nước)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang | Tọa độ Grid Khung (X, Y) | Kích thước Khung | Số Pixel | Palette | Ý Nghĩa / Trạng Thái Khung Hình |
|-----|-----------------------|------------------|----------------------|------------------|----------|---------|----------------------------------|
| **#206** | `(0, 332)` | `16 × 16 px` | `(0, 332)` | `16 × 16 px` | 176 px | UW Palette | Super Mushroom (Nấm đỏ / tăng kích thước) |
| **#207** | `(32, 332)` | `16 × 16 px` | `(32, 332)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — Bộ 1 (Khung hoạt họa #1) |
| **#208** | `(50, 332)` | `16 × 16 px` | `(50, 332)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — Bộ 1 (Khung hoạt họa #2) |
| **#209** | `(68, 332)` | `16 × 16 px` | `(68, 332)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — Bộ 1 (Khung hoạt họa #3) |
| **#210** | `(86, 332)` | `16 × 16 px` | `(86, 332)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — Bộ 1 (Khung hoạt họa #4) |
| **#211** | `(107, 332)` | `14 × 16 px` | `(106, 332)` | `16 × 16 px` | 122 px | UW Palette | Super Star — Bộ 1 (Khung hoạt họa #1) |
| **#212** | `(125, 332)` | `14 × 16 px` | `(124, 332)` | `16 × 16 px` | 122 px | UW Palette | Super Star — Bộ 1 (Khung hoạt họa #2) |
| **#213** | `(143, 332)` | `14 × 16 px` | `(142, 332)` | `16 × 16 px` | 122 px | UW Palette | Super Star — Bộ 1 (Khung hoạt họa #3) |
| **#214** | `(161, 332)` | `14 × 16 px` | `(160, 332)` | `16 × 16 px` | 122 px | UW Palette | Super Star — Bộ 1 (Khung hoạt họa #4) |
| **#215** | `(0, 350)` | `16 × 16 px` | `(0, 350)` | `16 × 16 px` | 176 px | UW Palette | 1-Up Mushroom (Nấm xanh lá / tăng 1 mạng) |
| **#216** | `(32, 350)` | `16 × 16 px` | `(32, 350)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — Bộ 2 (Khung hoạt họa biến thể #1) |
| **#217** | `(50, 350)` | `16 × 16 px` | `(50, 350)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — Bộ 2 (Khung hoạt họa biến thể #2) |
| **#218** | `(68, 350)` | `16 × 16 px` | `(68, 350)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — Bộ 2 (Khung hoạt họa biến thể #3) |
| **#219** | `(86, 350)` | `16 × 16 px` | `(86, 350)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — Bộ 2 (Khung hoạt họa biến thể #4) |
| **#220** | `(107, 350)` | `14 × 16 px` | `(106, 350)` | `16 × 16 px` | 122 px | UW Palette | Super Star — Bộ 2 (Khung hoạt họa biến thể #1) |
| **#221** | `(125, 350)` | `14 × 16 px` | `(124, 350)` | `16 × 16 px` | 122 px | UW Palette | Super Star — Bộ 2 (Khung hoạt họa biến thể #2) |
| **#222** | `(143, 350)` | `14 × 16 px` | `(142, 350)` | `16 × 16 px` | 122 px | UW Palette | Super Star — Bộ 2 (Khung hoạt họa biến thể #3) |
| **#223** | `(161, 350)` | `14 × 16 px` | `(160, 350)` | `16 × 16 px` | 122 px | UW Palette | Super Star — Bộ 2 (Khung hoạt họa biến thể #4) |
| **#224** | `(235, 350)` | `11 × 8 px` | `(234, 350)` | `16 × 8 px` | 43 px | UW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#225** | `(253, 350)` | `15 × 8 px` | `(252, 350)` | `16 × 8 px` | 58 px | UW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#226** | `(32, 368)` | `16 × 16 px` | `(32, 368)` | `16 × 16 px` | 162 px | UW Palette | Trampoline / Springboard (Trạng thái tĩnh nguyên vẹn) |
| **#227** | `(50, 368)` | `16 × 16 px` | `(50, 368)` | `16 × 16 px` | 162 px | UW Palette | Vật thể phụ kiện / Frame môi trường (UW Palette) |
| **#228** | `(68, 368)` | `16 × 16 px` | `(68, 368)` | `16 × 16 px` | 162 px | UW Palette | Vật thể phụ kiện / Frame môi trường (UW Palette) |
| **#229** | `(86, 368)` | `16 × 16 px` | `(86, 368)` | `16 × 16 px` | 162 px | UW Palette | Vật thể phụ kiện / Frame môi trường (UW Palette) |
| **#230** | `(107, 368)` | `14 × 16 px` | `(106, 368)` | `16 × 16 px` | 122 px | UW Palette | Vật thể phụ kiện / Frame môi trường (UW Palette) |
| **#231** | `(125, 368)` | `14 × 16 px` | `(124, 368)` | `16 × 16 px` | 122 px | UW Palette | Vật thể phụ kiện / Frame môi trường (UW Palette) |
| **#232** | `(143, 368)` | `14 × 16 px` | `(142, 368)` | `16 × 16 px` | 122 px | UW Palette | Vật thể phụ kiện / Frame môi trường (UW Palette) |
| **#233** | `(161, 368)` | `14 × 16 px` | `(160, 368)` | `16 × 16 px` | 122 px | UW Palette | Vật thể phụ kiện / Frame môi trường (UW Palette) |
| **#234** | `(180, 361)` | `8 × 14 px` | `(180, 360)` | `8 × 16 px` | 84 px | UW Palette | Spinning Coin (Đồng xu nảy ra từ Block / Xu lơ lửng - Khung #1) |
| **#235** | `(180, 378)` | `8 × 8 px` | `(180, 378)` | `8 × 8 px` | 42 px | UW Palette | Mario Fireball (Khung đạn lửa xoay tròn #1) |
| **#236** | `(192, 361)` | `4 × 14 px` | `(190, 360)` | `8 × 16 px` | 48 px | UW Palette | Spinning Coin (Đồng xu nảy ra từ Block / Xu lơ lửng - Khung #2) |
| **#237** | `(190, 378)` | `8 × 8 px` | `(190, 378)` | `8 × 8 px` | 43 px | UW Palette | Mario Fireball (Khung đạn lửa xoay tròn #2) |
| **#238** | `(204, 361)` | `1 × 14 px` | `(200, 360)` | `8 × 16 px` | 14 px | UW Palette | Spinning Coin (Đồng xu nảy ra từ Block / Xu lơ lửng - Khung #3) |
| **#239** | `(200, 378)` | `8 × 8 px` | `(200, 378)` | `8 × 8 px` | 42 px | UW Palette | Mario Fireball (Khung đạn lửa xoay tròn #3) |
| **#240** | `(212, 361)` | `4 × 14 px` | `(210, 360)` | `8 × 16 px` | 48 px | UW Palette | Spinning Coin (Đồng xu nảy ra từ Block / Xu lơ lửng - Khung #4) |
| **#241** | `(210, 378)` | `8 × 8 px` | `(210, 378)` | `8 × 8 px` | 43 px | UW Palette | Mario Fireball (Khung đạn lửa xoay tròn #4) |
| **#242** | `(234, 360)` | `12 × 8 px` | `(234, 360)` | `16 × 8 px` | 46 px | UW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#243** | `(234, 370)` | `12 × 8 px` | `(234, 370)` | `16 × 8 px` | 48 px | UW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#244** | `(252, 360)` | `16 × 8 px` | `(252, 360)` | `16 × 8 px` | 61 px | UW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#245** | `(252, 370)` | `16 × 8 px` | `(252, 370)` | `16 × 8 px` | 63 px | UW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#246** | `(32, 388)` | `16 × 8 px` | `(32, 388)` | `16 × 8 px` | 100 px | UW Palette | Trampoline / Springboard (Đệm lò xo nén dẹt) |
| **#247** | `(32, 399)` | `16 × 30 px` | `(32, 399)` | `16 × 30 px` | 216 px | UW Palette | Trampoline / Springboard (Đệm lò xo bung nảy cực đại) |
| **#248** | `(52, 388)` | `24 × 8 px` | `(52, 388)` | `24 × 8 px` | 150 px | UW Palette | Moving Platform (Mặt bệ nâng di động - Loại ngắn 24px) |
| **#249** | `(80, 388)` | `32 × 8 px` | `(80, 388)` | `32 × 8 px` | 200 px | UW Palette | Firebar (Thanh lửa quay trong Castle - Trạng thái ngang) |
| **#250** | `(80, 398)` | `32 × 8 px` | `(80, 398)` | `32 × 8 px` | 208 px | UW Palette | Firebar (Thanh lửa quay trong Castle - Trạng thái ngang) |
| **#251** | `(116, 388)` | `48 × 8 px` | `(116, 388)` | `48 × 8 px` | 300 px | UW Palette | Moving Platform (Mặt bệ nâng di động dầm ngang - Loại dài 48px) |
| **#252** | `(116, 398)` | `48 × 8 px` | `(116, 398)` | `48 × 8 px` | 312 px | UW Palette | Moving Platform (Mặt bệ nâng di động dầm ngang - Loại dài 48px) |
| **#253** | `(184, 392)` | `8 × 8 px` | `(180, 388)` | `16 × 16 px` | 44 px | UW Palette | Fireball Explosion (Khung hiệu ứng nổ đạn lửa/pháo hoa) |
| **#254** | `(200, 389)` | `12 × 14 px` | `(198, 388)` | `16 × 16 px` | 112 px | UW Palette | Fireball Explosion (Khung hiệu ứng nổ đạn lửa/pháo hoa) |
| **#255** | `(216, 388)` | `16 × 16 px` | `(216, 388)` | `16 × 16 px` | 200 px | UW Palette | Fireball Explosion (Khung hiệu ứng nổ đạn lửa/pháo hoa) |
| **#256** | `(234, 380)` | `12 × 8 px` | `(234, 380)` | `16 × 8 px` | 48 px | UW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#257** | `(234, 390)` | `12 × 8 px` | `(234, 390)` | `16 × 8 px` | 47 px | UW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#258** | `(252, 380)` | `16 × 8 px` | `(252, 380)` | `16 × 8 px` | 63 px | UW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#259** | `(252, 390)` | `16 × 8 px` | `(252, 390)` | `16 × 8 px` | 62 px | UW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |
| **#260** | `(50, 407)` | `16 × 22 px` | `(50, 407)` | `16 × 22 px` | 212 px | UW Palette | Trampoline / Springboard (Khung trạng thái chờ/trung gian của lò xo) |
| **#261** | `(68, 415)` | `16 × 14 px` | `(68, 415)` | `16 × 14 px` | 204 px | UW Palette | Flagpole Flag (Cờ đích kéo xuống ở cuối màn) |
| **#262** | `(92, 414)` | `16 × 15 px` | `(92, 414)` | `16 × 15 px` | 135 px | UW Palette | Castle Flag / Star Flag (Cờ trên đỉnh lâu đài) |
| **#263** | `(110, 414)` | `13 × 15 px` | `(110, 414)` | `16 × 15 px` | 116 px | UW Palette | Beanstalk / Vine Top (Đỉnh/ngọn dây leo mọc vút lên) |
| **#264** | `(131, 414)` | `14 × 15 px` | `(130, 414)` | `16 × 15 px` | 69 px | UW Palette | Beanstalk / Vine Stem 1 (Thân dây leo đoạn 1) |
| **#265** | `(149, 414)` | `14 × 15 px` | `(148, 414)` | `16 × 15 px` | 74 px | UW Palette | Beanstalk / Vine Stem 2 (Thân dây leo đoạn 2) |
| **#266** | `(252, 400)` | `16 × 7 px` | `(252, 400)` | `16 × 8 px` | 61 px | UW Palette | Floating Score Text (Văn bản điểm số hiển thị khi ăn nấm/diệt quái: 100, 200, 1000, 1UP...) |

---

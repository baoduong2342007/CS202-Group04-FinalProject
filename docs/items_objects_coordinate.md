# Tọa Độ & Mô Tả Vật Thể Items & Objects (items_objects_coordinate.md)

> Tài liệu phân tích toàn bộ 266 vật thể (Items, Objects, Power-ups, Score Texts, Platforms) trên bảng Atlas tham chiếu:
> docs/assets/reference/items_objects_all_components_atlas_full.png (960×1840 px).
> Cấu trúc bảng chia làm 4 bộ Palette chuẩn Super Mario Bros NES: Overworld (#1–#61), Underground (#62–#133), Castle (#134–#205), Underwater (#206–#266).
> Lưu ý: Thứ tự `#` trong atlas đi theo thứ tự thành phần trong ảnh gốc, không chia đều 66 phần tử cho mỗi môi trường. Ranh giới theo tọa độ là OW `#1–#61`, UG `#62–#133`, Castle `#134–#205`, UW `#206–#266`.
> Các Score Text lặp ở UG/Castle là hai bản palette: bản score chuẩn và bản peach dùng cho điểm nhận từ cờ đích.

---

## 1. Overworld Objects (Môi trường ngoài trời — #1 đến #61)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang | Tọa độ Grid Khung (X, Y) | Kích thước Khung | Số Pixel | Palette | Ý Nghĩa / Trạng Thái Khung Hình |
|-----|-----------------------|------------------|----------------------|------------------|----------|---------|----------------------------------|
| **#1** | `(0, 8)` | `16 × 16 px` | `(0, 8)` | `16 × 16 px` | 176 px | OW Palette | Super Mushroom (nấm Super; tăng kích thước) |
| **#2** | `(32, 8)` | `16 × 16 px` | `(32, 8)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — chu kỳ bảng màu nhóm A (trạng thái #1/4) |
| **#3** | `(50, 8)` | `16 × 16 px` | `(50, 8)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — chu kỳ bảng màu nhóm A (trạng thái #2/4) |
| **#4** | `(68, 8)` | `16 × 16 px` | `(68, 8)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — chu kỳ bảng màu nhóm A (trạng thái #3/4) |
| **#5** | `(86, 8)` | `16 × 16 px` | `(86, 8)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — chu kỳ bảng màu nhóm A (trạng thái #4/4) |
| **#6** | `(107, 8)` | `14 × 16 px` | `(106, 8)` | `16 × 16 px` | 122 px | OW Palette | Super Star — chu kỳ bảng màu nhóm A (trạng thái #1/4) |
| **#7** | `(125, 8)` | `14 × 16 px` | `(124, 8)` | `16 × 16 px` | 122 px | OW Palette | Super Star — chu kỳ bảng màu nhóm A (trạng thái #2/4) |
| **#8** | `(143, 8)` | `14 × 16 px` | `(142, 8)` | `16 × 16 px` | 122 px | OW Palette | Super Star — chu kỳ bảng màu nhóm A (trạng thái #3/4) |
| **#9** | `(161, 8)` | `14 × 16 px` | `(160, 8)` | `16 × 16 px` | 122 px | OW Palette | Super Star — chu kỳ bảng màu nhóm A (trạng thái #4/4) |
| **#10** | `(0, 26)` | `16 × 16 px` | `(0, 26)` | `16 × 16 px` | 176 px | OW Palette | 1-Up Mushroom (nấm thêm 1 mạng) |
| **#11** | `(32, 26)` | `16 × 16 px` | `(32, 26)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — chu kỳ bảng màu nhóm B (trạng thái #1/4) |
| **#12** | `(50, 26)` | `16 × 16 px` | `(50, 26)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — chu kỳ bảng màu nhóm B (trạng thái #2/4) |
| **#13** | `(68, 26)` | `16 × 16 px` | `(68, 26)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — chu kỳ bảng màu nhóm B (trạng thái #3/4) |
| **#14** | `(86, 26)` | `16 × 16 px` | `(86, 26)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — chu kỳ bảng màu nhóm B (trạng thái #4/4) |
| **#15** | `(107, 26)` | `14 × 16 px` | `(106, 26)` | `16 × 16 px` | 122 px | OW Palette | Super Star — chu kỳ bảng màu nhóm B (trạng thái #1/4) |
| **#16** | `(125, 26)` | `14 × 16 px` | `(124, 26)` | `16 × 16 px` | 122 px | OW Palette | Super Star — chu kỳ bảng màu nhóm B (trạng thái #2/4) |
| **#17** | `(143, 26)` | `14 × 16 px` | `(142, 26)` | `16 × 16 px` | 122 px | OW Palette | Super Star — chu kỳ bảng màu nhóm B (trạng thái #3/4) |
| **#18** | `(161, 26)` | `14 × 16 px` | `(160, 26)` | `16 × 16 px` | 122 px | OW Palette | Super Star — chu kỳ bảng màu nhóm B (trạng thái #4/4) |
| **#19** | `(180, 37)` | `8 × 14 px` | `(180, 36)` | `8 × 16 px` | 84 px | OW Palette | Spinning Coin — xu quay — khung #1/4 |
| **#20** | `(192, 37)` | `4 × 14 px` | `(190, 36)` | `8 × 16 px` | 48 px | OW Palette | Spinning Coin — xu quay — khung #2/4 |
| **#21** | `(204, 37)` | `1 × 14 px` | `(200, 36)` | `8 × 16 px` | 14 px | OW Palette | Spinning Coin — xu quay — khung #3/4 |
| **#22** | `(212, 37)` | `4 × 14 px` | `(210, 36)` | `8 × 16 px` | 48 px | OW Palette | Spinning Coin — xu quay — khung #4/4 |
| **#23** | `(235, 26)` | `11 × 8 px` | `(234, 26)` | `16 × 8 px` | 43 px | OW Palette | Score Text — 100 điểm (palette score chuẩn) |
| **#24** | `(234, 36)` | `12 × 8 px` | `(234, 36)` | `16 × 8 px` | 46 px | OW Palette | Score Text — 1000 điểm (palette score chuẩn) |
| **#25** | `(253, 26)` | `15 × 8 px` | `(252, 26)` | `16 × 8 px` | 58 px | OW Palette | Score Text — 200 điểm (palette score chuẩn) |
| **#26** | `(252, 36)` | `16 × 8 px` | `(252, 36)` | `16 × 8 px` | 61 px | OW Palette | Score Text — 2000 điểm (palette score chuẩn) |
| **#27** | `(32, 44)` | `16 × 16 px` | `(32, 44)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — chu kỳ bảng màu nhóm C (trạng thái #1/4) |
| **#28** | `(50, 44)` | `16 × 16 px` | `(50, 44)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — chu kỳ bảng màu nhóm C (trạng thái #2/4) |
| **#29** | `(68, 44)` | `16 × 16 px` | `(68, 44)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — chu kỳ bảng màu nhóm C (trạng thái #3/4) |
| **#30** | `(86, 44)` | `16 × 16 px` | `(86, 44)` | `16 × 16 px` | 162 px | OW Palette | Fire Flower — chu kỳ bảng màu nhóm C (trạng thái #4/4) |
| **#31** | `(107, 44)` | `14 × 16 px` | `(106, 44)` | `16 × 16 px` | 122 px | OW Palette | Super Star — chu kỳ bảng màu nhóm C (trạng thái #1/4) |
| **#32** | `(125, 44)` | `14 × 16 px` | `(124, 44)` | `16 × 16 px` | 122 px | OW Palette | Super Star — chu kỳ bảng màu nhóm C (trạng thái #2/4) |
| **#33** | `(143, 44)` | `14 × 16 px` | `(142, 44)` | `16 × 16 px` | 122 px | OW Palette | Super Star — chu kỳ bảng màu nhóm C (trạng thái #3/4) |
| **#34** | `(161, 44)` | `14 × 16 px` | `(160, 44)` | `16 × 16 px` | 122 px | OW Palette | Super Star — chu kỳ bảng màu nhóm C (trạng thái #4/4) |
| **#35** | `(180, 54)` | `8 × 8 px` | `(180, 54)` | `8 × 8 px` | 42 px | OW Palette | Mario Fireball — đạn lửa xoay — khung #1/4 |
| **#36** | `(190, 54)` | `8 × 8 px` | `(190, 54)` | `8 × 8 px` | 43 px | OW Palette | Mario Fireball — đạn lửa xoay — khung #2/4 |
| **#37** | `(200, 54)` | `8 × 8 px` | `(200, 54)` | `8 × 8 px` | 42 px | OW Palette | Mario Fireball — đạn lửa xoay — khung #3/4 |
| **#38** | `(210, 54)` | `8 × 8 px` | `(210, 54)` | `8 × 8 px` | 43 px | OW Palette | Mario Fireball — đạn lửa xoay — khung #4/4 |
| **#39** | `(234, 46)` | `12 × 8 px` | `(234, 46)` | `16 × 8 px` | 48 px | OW Palette | Score Text — 400 điểm (palette score chuẩn) |
| **#40** | `(234, 56)` | `12 × 8 px` | `(234, 56)` | `16 × 8 px` | 48 px | OW Palette | Score Text — 500 điểm (palette score chuẩn) |
| **#41** | `(252, 46)` | `16 × 8 px` | `(252, 46)` | `16 × 8 px` | 63 px | OW Palette | Score Text — 800 điểm (palette score chuẩn) |
| **#42** | `(252, 56)` | `16 × 8 px` | `(252, 56)` | `16 × 8 px` | 63 px | OW Palette | Score Text — 8000 điểm (palette score chuẩn) |
| **#43** | `(32, 64)` | `16 × 8 px` | `(32, 64)` | `16 × 8 px` | 100 px | OW Palette | Springboard / Jumping Board (mặt đệm; trạng thái phẳng) |
| **#44** | `(32, 75)` | `16 × 31 px` | `(32, 75)` | `16 × 31 px` | 232 px | OW Palette | Springboard / Jumping Board (lò xo bung; crop kèm phần tile bên dưới) |
| **#45** | `(52, 64)` | `24 × 8 px` | `(52, 64)` | `24 × 8 px` | 150 px | OW Palette | Very Small Moving Platform (bệ nâng 24 px) |
| **#46** | `(80, 64)` | `32 × 8 px` | `(80, 64)` | `32 × 8 px` | 200 px | OW Palette | Small Moving Platform (bệ nâng 32 px) |
| **#47** | `(80, 74)` | `32 × 8 px` | `(80, 74)` | `32 × 8 px` | 208 px | OW Palette | Moving Cloud Platform (bệ mây 32 px) |
| **#48** | `(116, 64)` | `48 × 8 px` | `(116, 64)` | `48 × 8 px` | 300 px | OW Palette | Moving Platform (bệ nâng 48 px) |
| **#49** | `(116, 74)` | `48 × 8 px` | `(116, 74)` | `48 × 8 px` | 312 px | OW Palette | Moving Cloud Platform (bệ mây 48 px) |
| **#50** | `(184, 68)` | `8 × 8 px` | `(180, 64)` | `16 × 16 px` | 44 px | OW Palette | Fireball Hit / Impact — hiệu ứng va chạm — khung #1/3 |
| **#51** | `(200, 65)` | `12 × 14 px` | `(198, 64)` | `16 × 16 px` | 112 px | OW Palette | Fireball Hit / Impact — hiệu ứng va chạm — khung #2/3 |
| **#52** | `(216, 64)` | `16 × 16 px` | `(216, 64)` | `16 × 16 px` | 200 px | OW Palette | Fireball Hit / Impact — hiệu ứng va chạm — khung #3/3 |
| **#53** | `(234, 66)` | `12 × 8 px` | `(234, 66)` | `16 × 8 px` | 47 px | OW Palette | Score Text — 4000 điểm (palette score chuẩn) |
| **#54** | `(252, 66)` | `16 × 8 px` | `(252, 66)` | `16 × 8 px` | 62 px | OW Palette | Score Text — 5000 điểm (palette score chuẩn) |
| **#55** | `(252, 76)` | `16 × 7 px` | `(252, 76)` | `16 × 8 px` | 61 px | OW Palette | Score Text — 1UP (hiển thị cộng mạng; palette score chuẩn) |
| **#56** | `(50, 83)` | `16 × 23 px` | `(50, 83)` | `16 × 23 px` | 228 px | OW Palette | Springboard / Jumping Board (trạng thái trung gian) |
| **#57** | `(68, 91)` | `16 × 15 px` | `(68, 91)` | `16 × 15 px` | 220 px | OW Palette | Springboard / Jumping Board (trạng thái nén/thu gọn; crop kèm phần tile bên dưới) |
| **#58** | `(92, 90)` | `16 × 16 px` | `(92, 90)` | `16 × 16 px` | 136 px | OW Palette | Flag from Pole (cờ cột đích cuối màn) |
| **#59** | `(110, 90)` | `13 × 16 px` | `(110, 90)` | `16 × 16 px` | 117 px | OW Palette | Flag from Castle (cờ lâu đài) |
| **#60** | `(131, 90)` | `14 × 16 px` | `(130, 90)` | `16 × 16 px` | 71 px | OW Palette | Vine (phần trên) |
| **#61** | `(149, 90)` | `14 × 16 px` | `(148, 90)` | `16 × 16 px` | 76 px | OW Palette | Vine (thân/đoạn tiếp theo) |

---

## 2. Underground Objects (Môi trường lòng đất — #62 đến #133)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang | Tọa độ Grid Khung (X, Y) | Kích thước Khung | Số Pixel | Palette | Ý Nghĩa / Trạng Thái Khung Hình |
|-----|-----------------------|------------------|----------------------|------------------|----------|---------|----------------------------------|
| **#62** | `(0, 116)` | `16 × 16 px` | `(0, 116)` | `16 × 16 px` | 176 px | UG Palette | Super Mushroom (nấm Super; tăng kích thước) |
| **#63** | `(32, 116)` | `16 × 16 px` | `(32, 116)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — chu kỳ bảng màu nhóm A (trạng thái #1/4) |
| **#64** | `(50, 116)` | `16 × 16 px` | `(50, 116)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — chu kỳ bảng màu nhóm A (trạng thái #2/4) |
| **#65** | `(68, 116)` | `16 × 16 px` | `(68, 116)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — chu kỳ bảng màu nhóm A (trạng thái #3/4) |
| **#66** | `(86, 116)` | `16 × 16 px` | `(86, 116)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — chu kỳ bảng màu nhóm A (trạng thái #4/4) |
| **#67** | `(107, 116)` | `14 × 16 px` | `(106, 116)` | `16 × 16 px` | 122 px | UG Palette | Super Star — chu kỳ bảng màu nhóm A (trạng thái #1/4) |
| **#68** | `(125, 116)` | `14 × 16 px` | `(124, 116)` | `16 × 16 px` | 122 px | UG Palette | Super Star — chu kỳ bảng màu nhóm A (trạng thái #2/4) |
| **#69** | `(143, 116)` | `14 × 16 px` | `(142, 116)` | `16 × 16 px` | 122 px | UG Palette | Super Star — chu kỳ bảng màu nhóm A (trạng thái #3/4) |
| **#70** | `(161, 116)` | `14 × 16 px` | `(160, 116)` | `16 × 16 px` | 122 px | UG Palette | Super Star — chu kỳ bảng màu nhóm A (trạng thái #4/4) |
| **#71** | `(0, 134)` | `16 × 16 px` | `(0, 134)` | `16 × 16 px` | 176 px | UG Palette | 1-Up Mushroom (nấm thêm 1 mạng) |
| **#72** | `(32, 134)` | `16 × 16 px` | `(32, 134)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — chu kỳ bảng màu nhóm B (trạng thái #1/4) |
| **#73** | `(50, 134)` | `16 × 16 px` | `(50, 134)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — chu kỳ bảng màu nhóm B (trạng thái #2/4) |
| **#74** | `(68, 134)` | `16 × 16 px` | `(68, 134)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — chu kỳ bảng màu nhóm B (trạng thái #3/4) |
| **#75** | `(86, 134)` | `16 × 16 px` | `(86, 134)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — chu kỳ bảng màu nhóm B (trạng thái #4/4) |
| **#76** | `(107, 134)` | `14 × 16 px` | `(106, 134)` | `16 × 16 px` | 122 px | UG Palette | Super Star — chu kỳ bảng màu nhóm B (trạng thái #1/4) |
| **#77** | `(125, 134)` | `14 × 16 px` | `(124, 134)` | `16 × 16 px` | 122 px | UG Palette | Super Star — chu kỳ bảng màu nhóm B (trạng thái #2/4) |
| **#78** | `(143, 134)` | `14 × 16 px` | `(142, 134)` | `16 × 16 px` | 122 px | UG Palette | Super Star — chu kỳ bảng màu nhóm B (trạng thái #3/4) |
| **#79** | `(161, 134)` | `14 × 16 px` | `(160, 134)` | `16 × 16 px` | 122 px | UG Palette | Super Star — chu kỳ bảng màu nhóm B (trạng thái #4/4) |
| **#80** | `(235, 134)` | `11 × 8 px` | `(234, 134)` | `16 × 8 px` | 43 px | UG Palette | Score Text — 100 điểm (palette score chuẩn) |
| **#81** | `(253, 134)` | `15 × 8 px` | `(252, 134)` | `16 × 8 px` | 58 px | UG Palette | Score Text — 200 điểm (palette score chuẩn) |
| **#82** | `(271, 134)` | `11 × 8 px` | `(270, 134)` | `16 × 8 px` | 43 px | UG Palette | Score Text — 100 điểm (palette flagpole/peach) |
| **#83** | `(289, 134)` | `15 × 8 px` | `(288, 134)` | `16 × 8 px` | 58 px | UG Palette | Score Text — 200 điểm (palette flagpole/peach) |
| **#84** | `(32, 152)` | `16 × 16 px` | `(32, 152)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — chu kỳ bảng màu nhóm C (trạng thái #1/4) |
| **#85** | `(50, 152)` | `16 × 16 px` | `(50, 152)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — chu kỳ bảng màu nhóm C (trạng thái #2/4) |
| **#86** | `(68, 152)` | `16 × 16 px` | `(68, 152)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — chu kỳ bảng màu nhóm C (trạng thái #3/4) |
| **#87** | `(86, 152)` | `16 × 16 px` | `(86, 152)` | `16 × 16 px` | 162 px | UG Palette | Fire Flower — chu kỳ bảng màu nhóm C (trạng thái #4/4) |
| **#88** | `(107, 152)` | `14 × 16 px` | `(106, 152)` | `16 × 16 px` | 122 px | UG Palette | Super Star — chu kỳ bảng màu nhóm C (trạng thái #1/4) |
| **#89** | `(125, 152)` | `14 × 16 px` | `(124, 152)` | `16 × 16 px` | 122 px | UG Palette | Super Star — chu kỳ bảng màu nhóm C (trạng thái #2/4) |
| **#90** | `(143, 152)` | `14 × 16 px` | `(142, 152)` | `16 × 16 px` | 122 px | UG Palette | Super Star — chu kỳ bảng màu nhóm C (trạng thái #3/4) |
| **#91** | `(161, 152)` | `14 × 16 px` | `(160, 152)` | `16 × 16 px` | 122 px | UG Palette | Super Star — chu kỳ bảng màu nhóm C (trạng thái #4/4) |
| **#92** | `(180, 145)` | `8 × 14 px` | `(180, 144)` | `8 × 16 px` | 84 px | UG Palette | Spinning Coin — xu quay — khung #1/4 |
| **#93** | `(192, 145)` | `4 × 14 px` | `(190, 144)` | `8 × 16 px` | 48 px | UG Palette | Spinning Coin — xu quay — khung #2/4 |
| **#94** | `(204, 145)` | `1 × 14 px` | `(200, 144)` | `8 × 16 px` | 14 px | UG Palette | Spinning Coin — xu quay — khung #3/4 |
| **#95** | `(212, 145)` | `4 × 14 px` | `(210, 144)` | `8 × 16 px` | 48 px | UG Palette | Spinning Coin — xu quay — khung #4/4 |
| **#96** | `(234, 144)` | `12 × 8 px` | `(234, 144)` | `16 × 8 px` | 46 px | UG Palette | Score Text — 1000 điểm (palette score chuẩn) |
| **#97** | `(234, 154)` | `12 × 8 px` | `(234, 154)` | `16 × 8 px` | 48 px | UG Palette | Score Text — 400 điểm (palette score chuẩn) |
| **#98** | `(252, 144)` | `16 × 8 px` | `(252, 144)` | `16 × 8 px` | 61 px | UG Palette | Score Text — 2000 điểm (palette score chuẩn) |
| **#99** | `(252, 154)` | `16 × 8 px` | `(252, 154)` | `16 × 8 px` | 63 px | UG Palette | Score Text — 800 điểm (palette score chuẩn) |
| **#100** | `(270, 144)` | `12 × 8 px` | `(270, 144)` | `16 × 8 px` | 46 px | UG Palette | Score Text — 1000 điểm (palette flagpole/peach) |
| **#101** | `(270, 154)` | `12 × 8 px` | `(270, 154)` | `16 × 8 px` | 48 px | UG Palette | Score Text — 400 điểm (palette flagpole/peach) |
| **#102** | `(288, 144)` | `16 × 8 px` | `(288, 144)` | `16 × 8 px` | 61 px | UG Palette | Score Text — 2000 điểm (palette flagpole/peach) |
| **#103** | `(288, 154)` | `16 × 8 px` | `(288, 154)` | `16 × 8 px` | 63 px | UG Palette | Score Text — 800 điểm (palette flagpole/peach) |
| **#104** | `(32, 172)` | `16 × 8 px` | `(32, 172)` | `16 × 8 px` | 100 px | UG Palette | Springboard / Jumping Board (mặt đệm; trạng thái phẳng) |
| **#105** | `(52, 172)` | `24 × 8 px` | `(52, 172)` | `24 × 8 px` | 150 px | UG Palette | Very Small Moving Platform (bệ nâng 24 px) |
| **#106** | `(80, 172)` | `32 × 8 px` | `(80, 172)` | `32 × 8 px` | 200 px | UG Palette | Small Moving Platform (bệ nâng 32 px) |
| **#107** | `(116, 172)` | `48 × 8 px` | `(116, 172)` | `48 × 8 px` | 300 px | UG Palette | Moving Platform (bệ nâng 48 px) |
| **#108** | `(180, 162)` | `8 × 8 px` | `(180, 162)` | `8 × 8 px` | 42 px | UG Palette | Mario Fireball — đạn lửa xoay — khung #1/4 |
| **#109** | `(184, 176)` | `8 × 8 px` | `(180, 172)` | `16 × 16 px` | 44 px | UG Palette | Fireball Hit / Impact — hiệu ứng va chạm — khung #1/3 |
| **#110** | `(190, 162)` | `8 × 8 px` | `(190, 162)` | `8 × 8 px` | 43 px | UG Palette | Mario Fireball — đạn lửa xoay — khung #2/4 |
| **#111** | `(200, 173)` | `12 × 14 px` | `(198, 172)` | `16 × 16 px` | 112 px | UG Palette | Fireball Hit / Impact — hiệu ứng va chạm — khung #2/3 |
| **#112** | `(200, 162)` | `8 × 8 px` | `(200, 162)` | `8 × 8 px` | 42 px | UG Palette | Mario Fireball — đạn lửa xoay — khung #3/4 |
| **#113** | `(210, 162)` | `8 × 8 px` | `(210, 162)` | `8 × 8 px` | 43 px | UG Palette | Mario Fireball — đạn lửa xoay — khung #4/4 |
| **#114** | `(216, 172)` | `16 × 16 px` | `(216, 172)` | `16 × 16 px` | 200 px | UG Palette | Fireball Hit / Impact — hiệu ứng va chạm — khung #3/3 |
| **#115** | `(234, 164)` | `12 × 8 px` | `(234, 164)` | `16 × 8 px` | 48 px | UG Palette | Score Text — 500 điểm (palette score chuẩn) |
| **#116** | `(234, 174)` | `12 × 8 px` | `(234, 174)` | `16 × 8 px` | 47 px | UG Palette | Score Text — 4000 điểm (palette score chuẩn) |
| **#117** | `(252, 164)` | `16 × 8 px` | `(252, 164)` | `16 × 8 px` | 63 px | UG Palette | Score Text — 8000 điểm (palette score chuẩn) |
| **#118** | `(252, 174)` | `16 × 8 px` | `(252, 174)` | `16 × 8 px` | 62 px | UG Palette | Score Text — 5000 điểm (palette score chuẩn) |
| **#119** | `(270, 164)` | `12 × 8 px` | `(270, 164)` | `16 × 8 px` | 48 px | UG Palette | Score Text — 500 điểm (palette flagpole/peach) |
| **#120** | `(270, 174)` | `12 × 8 px` | `(270, 174)` | `16 × 8 px` | 47 px | UG Palette | Score Text — 4000 điểm (palette flagpole/peach) |
| **#121** | `(288, 164)` | `16 × 8 px` | `(288, 164)` | `16 × 8 px` | 63 px | UG Palette | Score Text — 8000 điểm (palette flagpole/peach) |
| **#122** | `(288, 174)` | `16 × 8 px` | `(288, 174)` | `16 × 8 px` | 62 px | UG Palette | Score Text — 5000 điểm (palette flagpole/peach) |
| **#123** | `(32, 183)` | `16 × 31 px` | `(32, 183)` | `16 × 31 px` | 232 px | UG Palette | Springboard / Jumping Board (lò xo bung; crop kèm phần tile bên dưới) |
| **#124** | `(50, 191)` | `16 × 23 px` | `(50, 191)` | `16 × 23 px` | 228 px | UG Palette | Springboard / Jumping Board (trạng thái trung gian) |
| **#125** | `(68, 199)` | `16 × 15 px` | `(68, 199)` | `16 × 15 px` | 220 px | UG Palette | Springboard / Jumping Board (trạng thái nén/thu gọn; crop kèm phần tile bên dưới) |
| **#126** | `(80, 182)` | `32 × 8 px` | `(80, 182)` | `32 × 8 px` | 208 px | UG Palette | Moving Cloud Platform (bệ mây 32 px) |
| **#127** | `(92, 198)` | `16 × 16 px` | `(92, 198)` | `16 × 16 px` | 136 px | UG Palette | Flag from Pole (cờ cột đích cuối màn) |
| **#128** | `(110, 198)` | `13 × 16 px` | `(110, 198)` | `16 × 16 px` | 117 px | UG Palette | Flag from Castle (cờ lâu đài) |
| **#129** | `(116, 182)` | `48 × 8 px` | `(116, 182)` | `48 × 8 px` | 312 px | UG Palette | Moving Cloud Platform (bệ mây 48 px) |
| **#130** | `(131, 198)` | `14 × 16 px` | `(130, 198)` | `16 × 16 px` | 71 px | UG Palette | Vine (phần trên) |
| **#131** | `(149, 198)` | `14 × 16 px` | `(148, 198)` | `16 × 16 px` | 76 px | UG Palette | Vine (thân/đoạn tiếp theo) |
| **#132** | `(252, 184)` | `16 × 7 px` | `(252, 184)` | `16 × 8 px` | 61 px | UG Palette | Score Text — 1UP (hiển thị cộng mạng; palette score chuẩn) |
| **#133** | `(288, 184)` | `16 × 7 px` | `(288, 184)` | `16 × 8 px` | 61 px | UG Palette | Score Text — 1UP (hiển thị cộng mạng; palette flagpole/peach) |

---

## 3. Castle Objects (Môi trường lâu đài — #134 đến #205)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang | Tọa độ Grid Khung (X, Y) | Kích thước Khung | Số Pixel | Palette | Ý Nghĩa / Trạng Thái Khung Hình |
|-----|-----------------------|------------------|----------------------|------------------|----------|---------|----------------------------------|
| **#134** | `(0, 224)` | `16 × 16 px` | `(0, 224)` | `16 × 16 px` | 176 px | Castle Palette | Super Mushroom (nấm Super; tăng kích thước) |
| **#135** | `(32, 224)` | `16 × 16 px` | `(32, 224)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — chu kỳ bảng màu nhóm A (trạng thái #1/4) |
| **#136** | `(50, 224)` | `16 × 16 px` | `(50, 224)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — chu kỳ bảng màu nhóm A (trạng thái #2/4) |
| **#137** | `(68, 224)` | `16 × 16 px` | `(68, 224)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — chu kỳ bảng màu nhóm A (trạng thái #3/4) |
| **#138** | `(86, 224)` | `16 × 16 px` | `(86, 224)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — chu kỳ bảng màu nhóm A (trạng thái #4/4) |
| **#139** | `(107, 224)` | `14 × 16 px` | `(106, 224)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — chu kỳ bảng màu nhóm A (trạng thái #1/4) |
| **#140** | `(125, 224)` | `14 × 16 px` | `(124, 224)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — chu kỳ bảng màu nhóm A (trạng thái #2/4) |
| **#141** | `(143, 224)` | `14 × 16 px` | `(142, 224)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — chu kỳ bảng màu nhóm A (trạng thái #3/4) |
| **#142** | `(161, 224)` | `14 × 16 px` | `(160, 224)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — chu kỳ bảng màu nhóm A (trạng thái #4/4) |
| **#143** | `(0, 242)` | `16 × 16 px` | `(0, 242)` | `16 × 16 px` | 176 px | Castle Palette | 1-Up Mushroom (nấm thêm 1 mạng) |
| **#144** | `(32, 242)` | `16 × 16 px` | `(32, 242)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — chu kỳ bảng màu nhóm B (trạng thái #1/4) |
| **#145** | `(50, 242)` | `16 × 16 px` | `(50, 242)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — chu kỳ bảng màu nhóm B (trạng thái #2/4) |
| **#146** | `(68, 242)` | `16 × 16 px` | `(68, 242)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — chu kỳ bảng màu nhóm B (trạng thái #3/4) |
| **#147** | `(86, 242)` | `16 × 16 px` | `(86, 242)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — chu kỳ bảng màu nhóm B (trạng thái #4/4) |
| **#148** | `(107, 242)` | `14 × 16 px` | `(106, 242)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — chu kỳ bảng màu nhóm B (trạng thái #1/4) |
| **#149** | `(125, 242)` | `14 × 16 px` | `(124, 242)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — chu kỳ bảng màu nhóm B (trạng thái #2/4) |
| **#150** | `(143, 242)` | `14 × 16 px` | `(142, 242)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — chu kỳ bảng màu nhóm B (trạng thái #3/4) |
| **#151** | `(161, 242)` | `14 × 16 px` | `(160, 242)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — chu kỳ bảng màu nhóm B (trạng thái #4/4) |
| **#152** | `(180, 253)` | `8 × 14 px` | `(180, 252)` | `8 × 16 px` | 84 px | Castle Palette | Spinning Coin — xu quay — khung #1/4 |
| **#153** | `(192, 253)` | `4 × 14 px` | `(190, 252)` | `8 × 16 px` | 48 px | Castle Palette | Spinning Coin — xu quay — khung #2/4 |
| **#154** | `(204, 253)` | `1 × 14 px` | `(200, 252)` | `8 × 16 px` | 14 px | Castle Palette | Spinning Coin — xu quay — khung #3/4 |
| **#155** | `(212, 253)` | `4 × 14 px` | `(210, 252)` | `8 × 16 px` | 48 px | Castle Palette | Spinning Coin — xu quay — khung #4/4 |
| **#156** | `(235, 242)` | `11 × 8 px` | `(234, 242)` | `16 × 8 px` | 43 px | Castle Palette | Score Text — 100 điểm (palette score chuẩn) |
| **#157** | `(234, 252)` | `12 × 8 px` | `(234, 252)` | `16 × 8 px` | 46 px | Castle Palette | Score Text — 1000 điểm (palette score chuẩn) |
| **#158** | `(253, 242)` | `15 × 8 px` | `(252, 242)` | `16 × 8 px` | 58 px | Castle Palette | Score Text — 200 điểm (palette score chuẩn) |
| **#159** | `(252, 252)` | `16 × 8 px` | `(252, 252)` | `16 × 8 px` | 61 px | Castle Palette | Score Text — 2000 điểm (palette score chuẩn) |
| **#160** | `(271, 242)` | `11 × 8 px` | `(270, 242)` | `16 × 8 px` | 43 px | Castle Palette | Score Text — 100 điểm (palette flagpole/peach) |
| **#161** | `(270, 252)` | `12 × 8 px` | `(270, 252)` | `16 × 8 px` | 46 px | Castle Palette | Score Text — 1000 điểm (palette flagpole/peach) |
| **#162** | `(289, 242)` | `15 × 8 px` | `(288, 242)` | `16 × 8 px` | 58 px | Castle Palette | Score Text — 200 điểm (palette flagpole/peach) |
| **#163** | `(288, 252)` | `16 × 8 px` | `(288, 252)` | `16 × 8 px` | 61 px | Castle Palette | Score Text — 2000 điểm (palette flagpole/peach) |
| **#164** | `(32, 260)` | `16 × 16 px` | `(32, 260)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — chu kỳ bảng màu nhóm C (trạng thái #1/4) |
| **#165** | `(50, 260)` | `16 × 16 px` | `(50, 260)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — chu kỳ bảng màu nhóm C (trạng thái #2/4) |
| **#166** | `(68, 260)` | `16 × 16 px` | `(68, 260)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — chu kỳ bảng màu nhóm C (trạng thái #3/4) |
| **#167** | `(86, 260)` | `16 × 16 px` | `(86, 260)` | `16 × 16 px` | 162 px | Castle Palette | Fire Flower — chu kỳ bảng màu nhóm C (trạng thái #4/4) |
| **#168** | `(107, 260)` | `14 × 16 px` | `(106, 260)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — chu kỳ bảng màu nhóm C (trạng thái #1/4) |
| **#169** | `(125, 260)` | `14 × 16 px` | `(124, 260)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — chu kỳ bảng màu nhóm C (trạng thái #2/4) |
| **#170** | `(143, 260)` | `14 × 16 px` | `(142, 260)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — chu kỳ bảng màu nhóm C (trạng thái #3/4) |
| **#171** | `(161, 260)` | `14 × 16 px` | `(160, 260)` | `16 × 16 px` | 122 px | Castle Palette | Super Star — chu kỳ bảng màu nhóm C (trạng thái #4/4) |
| **#172** | `(180, 270)` | `8 × 8 px` | `(180, 270)` | `8 × 8 px` | 42 px | Castle Palette | Mario Fireball — đạn lửa xoay — khung #1/4 |
| **#173** | `(190, 270)` | `8 × 8 px` | `(190, 270)` | `8 × 8 px` | 43 px | Castle Palette | Mario Fireball — đạn lửa xoay — khung #2/4 |
| **#174** | `(200, 270)` | `8 × 8 px` | `(200, 270)` | `8 × 8 px` | 42 px | Castle Palette | Mario Fireball — đạn lửa xoay — khung #3/4 |
| **#175** | `(210, 270)` | `8 × 8 px` | `(210, 270)` | `8 × 8 px` | 43 px | Castle Palette | Mario Fireball — đạn lửa xoay — khung #4/4 |
| **#176** | `(234, 262)` | `12 × 8 px` | `(234, 262)` | `16 × 8 px` | 48 px | Castle Palette | Score Text — 400 điểm (palette score chuẩn) |
| **#177** | `(234, 272)` | `12 × 8 px` | `(234, 272)` | `16 × 8 px` | 48 px | Castle Palette | Score Text — 500 điểm (palette score chuẩn) |
| **#178** | `(252, 262)` | `16 × 8 px` | `(252, 262)` | `16 × 8 px` | 63 px | Castle Palette | Score Text — 800 điểm (palette score chuẩn) |
| **#179** | `(252, 272)` | `16 × 8 px` | `(252, 272)` | `16 × 8 px` | 63 px | Castle Palette | Score Text — 8000 điểm (palette score chuẩn) |
| **#180** | `(270, 262)` | `12 × 8 px` | `(270, 262)` | `16 × 8 px` | 48 px | Castle Palette | Score Text — 400 điểm (palette flagpole/peach) |
| **#181** | `(270, 272)` | `12 × 8 px` | `(270, 272)` | `16 × 8 px` | 48 px | Castle Palette | Score Text — 500 điểm (palette flagpole/peach) |
| **#182** | `(288, 262)` | `16 × 8 px` | `(288, 262)` | `16 × 8 px` | 63 px | Castle Palette | Score Text — 800 điểm (palette flagpole/peach) |
| **#183** | `(288, 272)` | `16 × 8 px` | `(288, 272)` | `16 × 8 px` | 63 px | Castle Palette | Score Text — 8000 điểm (palette flagpole/peach) |
| **#184** | `(32, 280)` | `16 × 8 px` | `(32, 280)` | `16 × 8 px` | 100 px | Castle Palette | Springboard / Jumping Board (mặt đệm; trạng thái phẳng) |
| **#185** | `(32, 291)` | `16 × 31 px` | `(32, 291)` | `16 × 31 px` | 232 px | Castle Palette | Springboard / Jumping Board (lò xo bung; crop kèm phần tile bên dưới) |
| **#186** | `(50, 299)` | `16 × 23 px` | `(50, 299)` | `16 × 23 px` | 228 px | Castle Palette | Springboard / Jumping Board (trạng thái trung gian) |
| **#187** | `(52, 280)` | `24 × 8 px` | `(52, 280)` | `24 × 8 px` | 150 px | Castle Palette | Very Small Moving Platform (bệ nâng 24 px) |
| **#188** | `(80, 280)` | `32 × 8 px` | `(80, 280)` | `32 × 8 px` | 200 px | Castle Palette | Small Moving Platform (bệ nâng 32 px) |
| **#189** | `(80, 290)` | `32 × 8 px` | `(80, 290)` | `32 × 8 px` | 208 px | Castle Palette | Moving Cloud Platform (bệ mây 32 px) |
| **#190** | `(116, 280)` | `48 × 8 px` | `(116, 280)` | `48 × 8 px` | 300 px | Castle Palette | Moving Platform (bệ nâng 48 px) |
| **#191** | `(116, 290)` | `48 × 8 px` | `(116, 290)` | `48 × 8 px` | 312 px | Castle Palette | Moving Cloud Platform (bệ mây 48 px) |
| **#192** | `(184, 284)` | `8 × 8 px` | `(180, 280)` | `16 × 16 px` | 44 px | Castle Palette | Fireball Hit / Impact — hiệu ứng va chạm — khung #1/3 |
| **#193** | `(200, 281)` | `12 × 14 px` | `(198, 280)` | `16 × 16 px` | 112 px | Castle Palette | Fireball Hit / Impact — hiệu ứng va chạm — khung #2/3 |
| **#194** | `(216, 280)` | `16 × 16 px` | `(216, 280)` | `16 × 16 px` | 200 px | Castle Palette | Fireball Hit / Impact — hiệu ứng va chạm — khung #3/3 |
| **#195** | `(234, 282)` | `12 × 8 px` | `(234, 282)` | `16 × 8 px` | 47 px | Castle Palette | Score Text — 4000 điểm (palette score chuẩn) |
| **#196** | `(252, 282)` | `16 × 8 px` | `(252, 282)` | `16 × 8 px` | 62 px | Castle Palette | Score Text — 5000 điểm (palette score chuẩn) |
| **#197** | `(252, 292)` | `16 × 7 px` | `(252, 292)` | `16 × 8 px` | 61 px | Castle Palette | Score Text — 1UP (hiển thị cộng mạng; palette score chuẩn) |
| **#198** | `(270, 282)` | `12 × 8 px` | `(270, 282)` | `16 × 8 px` | 47 px | Castle Palette | Score Text — 4000 điểm (palette flagpole/peach) |
| **#199** | `(288, 282)` | `16 × 8 px` | `(288, 282)` | `16 × 8 px` | 62 px | Castle Palette | Score Text — 5000 điểm (palette flagpole/peach) |
| **#200** | `(288, 292)` | `16 × 7 px` | `(288, 292)` | `16 × 8 px` | 61 px | Castle Palette | Score Text — 1UP (hiển thị cộng mạng; palette flagpole/peach) |
| **#201** | `(68, 307)` | `16 × 15 px` | `(68, 307)` | `16 × 15 px` | 220 px | Castle Palette | Springboard / Jumping Board (trạng thái nén/thu gọn; crop kèm phần tile bên dưới) |
| **#202** | `(92, 306)` | `16 × 16 px` | `(92, 306)` | `16 × 16 px` | 136 px | Castle Palette | Flag from Pole (cờ cột đích cuối màn) |
| **#203** | `(110, 306)` | `13 × 16 px` | `(110, 306)` | `16 × 16 px` | 117 px | Castle Palette | Flag from Castle (cờ lâu đài) |
| **#204** | `(131, 306)` | `14 × 16 px` | `(130, 306)` | `16 × 16 px` | 71 px | Castle Palette | Vine (phần trên) |
| **#205** | `(149, 306)` | `14 × 16 px` | `(148, 306)` | `16 × 16 px` | 76 px | Castle Palette | Vine (thân/đoạn tiếp theo) |

---

## 4. Underwater Objects (Môi trường dưới nước — #206 đến #266)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang | Tọa độ Grid Khung (X, Y) | Kích thước Khung | Số Pixel | Palette | Ý Nghĩa / Trạng Thái Khung Hình |
|-----|-----------------------|------------------|----------------------|------------------|----------|---------|----------------------------------|
| **#206** | `(0, 332)` | `16 × 16 px` | `(0, 332)` | `16 × 16 px` | 176 px | UW Palette | Super Mushroom (nấm Super; tăng kích thước) |
| **#207** | `(32, 332)` | `16 × 16 px` | `(32, 332)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — chu kỳ bảng màu nhóm A (trạng thái #1/4) |
| **#208** | `(50, 332)` | `16 × 16 px` | `(50, 332)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — chu kỳ bảng màu nhóm A (trạng thái #2/4) |
| **#209** | `(68, 332)` | `16 × 16 px` | `(68, 332)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — chu kỳ bảng màu nhóm A (trạng thái #3/4) |
| **#210** | `(86, 332)` | `16 × 16 px` | `(86, 332)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — chu kỳ bảng màu nhóm A (trạng thái #4/4) |
| **#211** | `(107, 332)` | `14 × 16 px` | `(106, 332)` | `16 × 16 px` | 122 px | UW Palette | Super Star — chu kỳ bảng màu nhóm A (trạng thái #1/4) |
| **#212** | `(125, 332)` | `14 × 16 px` | `(124, 332)` | `16 × 16 px` | 122 px | UW Palette | Super Star — chu kỳ bảng màu nhóm A (trạng thái #2/4) |
| **#213** | `(143, 332)` | `14 × 16 px` | `(142, 332)` | `16 × 16 px` | 122 px | UW Palette | Super Star — chu kỳ bảng màu nhóm A (trạng thái #3/4) |
| **#214** | `(161, 332)` | `14 × 16 px` | `(160, 332)` | `16 × 16 px` | 122 px | UW Palette | Super Star — chu kỳ bảng màu nhóm A (trạng thái #4/4) |
| **#215** | `(0, 350)` | `16 × 16 px` | `(0, 350)` | `16 × 16 px` | 176 px | UW Palette | 1-Up Mushroom (nấm thêm 1 mạng) |
| **#216** | `(32, 350)` | `16 × 16 px` | `(32, 350)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — chu kỳ bảng màu nhóm B (trạng thái #1/4) |
| **#217** | `(50, 350)` | `16 × 16 px` | `(50, 350)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — chu kỳ bảng màu nhóm B (trạng thái #2/4) |
| **#218** | `(68, 350)` | `16 × 16 px` | `(68, 350)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — chu kỳ bảng màu nhóm B (trạng thái #3/4) |
| **#219** | `(86, 350)` | `16 × 16 px` | `(86, 350)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — chu kỳ bảng màu nhóm B (trạng thái #4/4) |
| **#220** | `(107, 350)` | `14 × 16 px` | `(106, 350)` | `16 × 16 px` | 122 px | UW Palette | Super Star — chu kỳ bảng màu nhóm B (trạng thái #1/4) |
| **#221** | `(125, 350)` | `14 × 16 px` | `(124, 350)` | `16 × 16 px` | 122 px | UW Palette | Super Star — chu kỳ bảng màu nhóm B (trạng thái #2/4) |
| **#222** | `(143, 350)` | `14 × 16 px` | `(142, 350)` | `16 × 16 px` | 122 px | UW Palette | Super Star — chu kỳ bảng màu nhóm B (trạng thái #3/4) |
| **#223** | `(161, 350)` | `14 × 16 px` | `(160, 350)` | `16 × 16 px` | 122 px | UW Palette | Super Star — chu kỳ bảng màu nhóm B (trạng thái #4/4) |
| **#224** | `(235, 350)` | `11 × 8 px` | `(234, 350)` | `16 × 8 px` | 43 px | UW Palette | Score Text — 100 điểm (palette score chuẩn) |
| **#225** | `(253, 350)` | `15 × 8 px` | `(252, 350)` | `16 × 8 px` | 58 px | UW Palette | Score Text — 200 điểm (palette score chuẩn) |
| **#226** | `(32, 368)` | `16 × 16 px` | `(32, 368)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — chu kỳ bảng màu nhóm C (trạng thái #1/4) |
| **#227** | `(50, 368)` | `16 × 16 px` | `(50, 368)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — chu kỳ bảng màu nhóm C (trạng thái #2/4) |
| **#228** | `(68, 368)` | `16 × 16 px` | `(68, 368)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — chu kỳ bảng màu nhóm C (trạng thái #3/4) |
| **#229** | `(86, 368)` | `16 × 16 px` | `(86, 368)` | `16 × 16 px` | 162 px | UW Palette | Fire Flower — chu kỳ bảng màu nhóm C (trạng thái #4/4) |
| **#230** | `(107, 368)` | `14 × 16 px` | `(106, 368)` | `16 × 16 px` | 122 px | UW Palette | Super Star — chu kỳ bảng màu nhóm C (trạng thái #1/4) |
| **#231** | `(125, 368)` | `14 × 16 px` | `(124, 368)` | `16 × 16 px` | 122 px | UW Palette | Super Star — chu kỳ bảng màu nhóm C (trạng thái #2/4) |
| **#232** | `(143, 368)` | `14 × 16 px` | `(142, 368)` | `16 × 16 px` | 122 px | UW Palette | Super Star — chu kỳ bảng màu nhóm C (trạng thái #3/4) |
| **#233** | `(161, 368)` | `14 × 16 px` | `(160, 368)` | `16 × 16 px` | 122 px | UW Palette | Super Star — chu kỳ bảng màu nhóm C (trạng thái #4/4) |
| **#234** | `(180, 361)` | `8 × 14 px` | `(180, 360)` | `8 × 16 px` | 84 px | UW Palette | Spinning Coin — xu quay — khung #1/4 |
| **#235** | `(180, 378)` | `8 × 8 px` | `(180, 378)` | `8 × 8 px` | 42 px | UW Palette | Mario Fireball — đạn lửa xoay — khung #1/4 |
| **#236** | `(192, 361)` | `4 × 14 px` | `(190, 360)` | `8 × 16 px` | 48 px | UW Palette | Spinning Coin — xu quay — khung #2/4 |
| **#237** | `(190, 378)` | `8 × 8 px` | `(190, 378)` | `8 × 8 px` | 43 px | UW Palette | Mario Fireball — đạn lửa xoay — khung #2/4 |
| **#238** | `(204, 361)` | `1 × 14 px` | `(200, 360)` | `8 × 16 px` | 14 px | UW Palette | Spinning Coin — xu quay — khung #3/4 |
| **#239** | `(200, 378)` | `8 × 8 px` | `(200, 378)` | `8 × 8 px` | 42 px | UW Palette | Mario Fireball — đạn lửa xoay — khung #3/4 |
| **#240** | `(212, 361)` | `4 × 14 px` | `(210, 360)` | `8 × 16 px` | 48 px | UW Palette | Spinning Coin — xu quay — khung #4/4 |
| **#241** | `(210, 378)` | `8 × 8 px` | `(210, 378)` | `8 × 8 px` | 43 px | UW Palette | Mario Fireball — đạn lửa xoay — khung #4/4 |
| **#242** | `(234, 360)` | `12 × 8 px` | `(234, 360)` | `16 × 8 px` | 46 px | UW Palette | Score Text — 1000 điểm (palette score chuẩn) |
| **#243** | `(234, 370)` | `12 × 8 px` | `(234, 370)` | `16 × 8 px` | 48 px | UW Palette | Score Text — 400 điểm (palette score chuẩn) |
| **#244** | `(252, 360)` | `16 × 8 px` | `(252, 360)` | `16 × 8 px` | 61 px | UW Palette | Score Text — 2000 điểm (palette score chuẩn) |
| **#245** | `(252, 370)` | `16 × 8 px` | `(252, 370)` | `16 × 8 px` | 63 px | UW Palette | Score Text — 800 điểm (palette score chuẩn) |
| **#246** | `(32, 388)` | `16 × 8 px` | `(32, 388)` | `16 × 8 px` | 100 px | UW Palette | Springboard / Jumping Board (mặt đệm; trạng thái phẳng) |
| **#247** | `(32, 399)` | `16 × 30 px` | `(32, 399)` | `16 × 30 px` | 216 px | UW Palette | Springboard / Jumping Board (lò xo bung; crop kèm phần tile bên dưới) |
| **#248** | `(52, 388)` | `24 × 8 px` | `(52, 388)` | `24 × 8 px` | 150 px | UW Palette | Very Small Moving Platform (bệ nâng 24 px) |
| **#249** | `(80, 388)` | `32 × 8 px` | `(80, 388)` | `32 × 8 px` | 200 px | UW Palette | Small Moving Platform (bệ nâng 32 px) |
| **#250** | `(80, 398)` | `32 × 8 px` | `(80, 398)` | `32 × 8 px` | 208 px | UW Palette | Moving Cloud Platform (bệ mây 32 px) |
| **#251** | `(116, 388)` | `48 × 8 px` | `(116, 388)` | `48 × 8 px` | 300 px | UW Palette | Moving Platform (bệ nâng 48 px) |
| **#252** | `(116, 398)` | `48 × 8 px` | `(116, 398)` | `48 × 8 px` | 312 px | UW Palette | Moving Cloud Platform (bệ mây 48 px) |
| **#253** | `(184, 392)` | `8 × 8 px` | `(180, 388)` | `16 × 16 px` | 44 px | UW Palette | Fireball Hit / Impact — hiệu ứng va chạm — khung #1/3 |
| **#254** | `(200, 389)` | `12 × 14 px` | `(198, 388)` | `16 × 16 px` | 112 px | UW Palette | Fireball Hit / Impact — hiệu ứng va chạm — khung #2/3 |
| **#255** | `(216, 388)` | `16 × 16 px` | `(216, 388)` | `16 × 16 px` | 200 px | UW Palette | Fireball Hit / Impact — hiệu ứng va chạm — khung #3/3 |
| **#256** | `(234, 380)` | `12 × 8 px` | `(234, 380)` | `16 × 8 px` | 48 px | UW Palette | Score Text — 500 điểm (palette score chuẩn) |
| **#257** | `(234, 390)` | `12 × 8 px` | `(234, 390)` | `16 × 8 px` | 47 px | UW Palette | Score Text — 4000 điểm (palette score chuẩn) |
| **#258** | `(252, 380)` | `16 × 8 px` | `(252, 380)` | `16 × 8 px` | 63 px | UW Palette | Score Text — 8000 điểm (palette score chuẩn) |
| **#259** | `(252, 390)` | `16 × 8 px` | `(252, 390)` | `16 × 8 px` | 62 px | UW Palette | Score Text — 5000 điểm (palette score chuẩn) |
| **#260** | `(50, 407)` | `16 × 22 px` | `(50, 407)` | `16 × 22 px` | 212 px | UW Palette | Springboard / Jumping Board (trạng thái trung gian) |
| **#261** | `(68, 415)` | `16 × 14 px` | `(68, 415)` | `16 × 14 px` | 204 px | UW Palette | Springboard / Jumping Board (trạng thái nén/thu gọn; crop kèm phần tile bên dưới) |
| **#262** | `(92, 414)` | `16 × 15 px` | `(92, 414)` | `16 × 15 px` | 135 px | UW Palette | Flag from Pole (cờ cột đích cuối màn) |
| **#263** | `(110, 414)` | `13 × 15 px` | `(110, 414)` | `16 × 15 px` | 116 px | UW Palette | Flag from Castle (cờ lâu đài) |
| **#264** | `(131, 414)` | `14 × 15 px` | `(130, 414)` | `16 × 15 px` | 69 px | UW Palette | Vine (phần trên) |
| **#265** | `(149, 414)` | `14 × 15 px` | `(148, 414)` | `16 × 15 px` | 74 px | UW Palette | Vine (thân/đoạn tiếp theo) |
| **#266** | `(252, 400)` | `16 × 7 px` | `(252, 400)` | `16 × 8 px` | 61 px | UW Palette | Score Text — 1UP (hiển thị cộng mạng; palette score chuẩn) |

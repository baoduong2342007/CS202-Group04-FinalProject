# Bảng Tọa Độ & Ý Nghĩa Chi Tiết Khung Hình Quái Vật (`enemies_coordinate.md`)

Danh sách toàn bộ **157** thành phần liên thông quái vật tách từ tệp `enemies_all_components_atlas.png`, đối chiếu lại trên atlas gốc `enemies.png` (436 × 530 px).

> **Quy ước palette (bảng màu NES):**
> - **Overworld (OW)**: Xanh lá `(12,147,0)` + Cam `(230,156,33)` — dùng cho Level 1-1, 1-3, 3-1, v.v.
> - **Underground (UG)**: Teal `(0,123,140)` + Nâu `(156,74,0)` + Hồng nhạt `(255,206,197)` — dùng cho Level 1-2, 4-2, v.v.
> - **Castle (CT)**: Xám `(173,173,173)` + Cam `(230,156,33)` — dùng cho Level 1-4, 2-4, v.v.
> - **Underwater (UW)**: Teal nhạt `(181,239,239)` — dùng cho Level 2-2, 7-2, v.v.
> - **Red / All Environments**: Đỏ `(181,49,33)` + Cam `(230,156,33)` — một số quái chỉ có 1 palette dùng chung mọi màn.

## Cách đọc tọa độ và ý nghĩa

- Gốc tọa độ `(0, 0)` nằm ở góc trên trái của `enemies.png`; mọi tọa độ đều là **pixel, zero-based**.
- **Tọa độ Ô Grid Khung + Kích thước Khung** là `sf::IntRect` dùng để cắt sprite runtime. Mép phải và mép dưới là biên ngoài (`x + W`, `y + H`).
- **Tọa độ Loang + Kích thước Loang** là bounding box của phần pixel sprite khi tách component. Nó có thể âm hoặc tràn ra ngoài ô grid (ví dụ `#1`, `#125`); **không dùng cột này làm `IntRect` runtime**.
- `Underground + Castle` ở khu vực Green Enemies là một hàng palette dùng chung, không phải hai frame khác nhau. Ngược lại, bốn cột ở khu vực Ground Enemies là bốn palette riêng.
- `In Castle 1 Enemies` và `In Castle 2 Enemies` là các biến thể palette/đặc biệt của sprite trong màn castle: `#137/#138` và `#141/#142` là **Bowser giơ búa**, còn `#139/#140` mới là **Hammer Bro**.
- Chuỗi animation phải đọc theo thứ tự khung trong từng nhóm; STT toàn atlas chỉ là số định danh, không phải thứ tự animation xuyên suốt các loài.

### Kết quả đối chiếu lần này

- Đủ **157/157** dòng, STT duy nhất và toàn bộ `IntRect` grid nằm trong giới hạn `436 × 530`; các ô grid đều chứa pixel sprite.
- Không đổi tùy tiện các tọa độ grid đã khớp ảnh. Các chỉnh sửa chính là sửa tên trạng thái, loại enemy và thứ tự animation; riêng các tọa độ runtime sai trong catalog C++ đã được đồng bộ theo cột Grid của bảng này.
- Các chuỗi cần giữ nguyên khi đưa vào code: `UW Koopa #61 → #62`, `UW Paratroopa #63 → #64`, `Red Koopa #143 → #144`, `Red Paratroopa #145 → #146`, `Red Spiny #153 → #154`.

---

| STT | Tọa độ Loang (X, Y) | Kích thước Loang (W × H) | Tọa độ Ô Grid Khung (X, Y) | Kích thước Khung | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|-----------------------|--------------------------|----------------------------|------------------|---------------------------------------|
| **#1** | (-1, 13) | 20 × 20 px | `(0, 16)` | `16 × 16 px` | Goomba — Overworld (Đi bộ - khung 1) |
| **#2** | (16, 13) | 21 × 20 px | `(18, 16)` | `16 × 16 px` | Goomba — Overworld (Đi bộ - khung 2) |
| **#3** | (73, 13) | 20 × 20 px | `(74, 16)` | `16 × 16 px` | Goomba — Underground (Đi bộ - khung 1) |
| **#4** | (90, 13) | 21 × 20 px | `(92, 16)` | `16 × 16 px` | Goomba — Underground (Đi bộ - khung 2) |
| **#5** | (147, 13) | 18 × 20 px | `(148, 16)` | `16 × 16 px` | Goomba — Castle (Đi bộ - khung 1) |
| **#6** | (165, 13) | 18 × 20 px | `(166, 16)` | `16 × 16 px` | Goomba — Castle (Đi bộ - khung 2) |
| **#7** | (221, 13) | 20 × 20 px | `(222, 16)` | `16 × 16 px` | Goomba — Underwater (Đi bộ - khung 1) |
| **#8** | (238, 13) | 21 × 20 px | `(240, 16)` | `16 × 16 px` | Goomba — Underwater (Đi bộ - khung 2) |
| **#9** | (35, 23) | 18 × 10 px | `(36, 24)` | `16 × 8 px` | Goomba — Overworld (Bị giẫm bẹp) |
| **#10** | (109, 23) | 18 × 10 px | `(110, 24)` | `16 × 8 px` | Goomba — Underground (Bị giẫm bẹp) |
| **#11** | (183, 23) | 18 × 10 px | `(184, 24)` | `16 × 8 px` | Goomba — Castle (Bị giẫm bẹp) |
| **#12** | (257, 23) | 18 × 10 px | `(258, 24)` | `16 × 8 px` | Goomba — Underwater (Bị giẫm bẹp) |
| **#13** | (-1, 34) | 18 × 17 px | `(0, 34)` | `16 × 16 px` | Buzzy Beetle — Overworld (bò, khung 1) |
| **#14** | (17, 33) | 18 × 18 px | `(18, 34)` | `16 × 16 px` | Buzzy Beetle — Overworld (bò, khung 2) |
| **#15** | (35, 34) | 18 × 17 px | `(36, 34)` | `16 × 16 px` | Buzzy Beetle — Overworld (rút vào vỏ) |
| **#16** | (73, 34) | 18 × 17 px | `(74, 34)` | `16 × 16 px` | Buzzy Beetle — Underground (bò, khung 1) |
| **#17** | (91, 33) | 18 × 18 px | `(92, 34)` | `16 × 16 px` | Buzzy Beetle — Underground (bò, khung 2) |
| **#18** | (109, 34) | 18 × 17 px | `(110, 34)` | `16 × 16 px` | Buzzy Beetle — Underground (rút vào vỏ) |
| **#19** | (147, 34) | 18 × 17 px | `(148, 34)` | `16 × 16 px` | Buzzy Beetle — Castle (bò, khung 1) |
| **#20** | (165, 33) | 18 × 18 px | `(166, 34)` | `16 × 16 px` | Buzzy Beetle — Castle (bò, khung 2) |
| **#21** | (183, 34) | 18 × 17 px | `(184, 34)` | `16 × 16 px` | Buzzy Beetle — Castle (rút vào vỏ) |
| **#22** | (221, 34) | 18 × 17 px | `(222, 34)` | `16 × 16 px` | Buzzy Beetle — Underwater (bò, khung 1) |
| **#23** | (239, 33) | 18 × 18 px | `(240, 34)` | `16 × 16 px` | Buzzy Beetle — Underwater (bò, khung 2) |
| **#24** | (257, 34) | 18 × 17 px | `(258, 34)` | `16 × 16 px` | Buzzy Beetle — Underwater (rút vào vỏ) |
| **#25** | (-1, 51) | 18 × 26 px | `(0, 52)` | `16 × 24 px` | Blooper — Overworld (duỗi dài, khung 1) |
| **#26** | (17, 51) | 18 × 18 px | `(18, 52)` | `16 × 16 px` | Blooper — Overworld (co người, khung 2) |
| **#27** | (53, 52) | 18 × 16 px | `(54, 52)` | `16 × 16 px` | Bullet Bill — Overworld |
| **#28** | (73, 51) | 18 × 26 px | `(74, 52)` | `16 × 24 px` | Blooper — Underground (duỗi dài, khung 1) |
| **#29** | (91, 51) | 18 × 18 px | `(92, 52)` | `16 × 16 px` | Blooper — Underground (co người, khung 2) |
| **#30** | (127, 52) | 18 × 16 px | `(128, 52)` | `16 × 16 px` | Bullet Bill — Underground |
| **#31** | (147, 51) | 18 × 26 px | `(148, 52)` | `16 × 24 px` | Blooper — Castle (duỗi dài, khung 1) |
| **#32** | (165, 51) | 18 × 18 px | `(166, 52)` | `16 × 16 px` | Blooper — Castle (co người, khung 2) |
| **#33** | (201, 52) | 18 × 16 px | `(202, 52)` | `16 × 16 px` | Bullet Bill — Castle |
| **#34** | (221, 51) | 18 × 26 px | `(222, 52)` | `16 × 24 px` | Blooper — Underwater (duỗi dài, khung 1) |
| **#35** | (239, 51) | 18 × 18 px | `(240, 52)` | `16 × 16 px` | Blooper — Underwater (co người, khung 2) |
| **#36** | (275, 52) | 18 × 16 px | `(276, 52)` | `16 × 16 px` | Bullet Bill — Underwater |
| **#37** | (3, 77) | 10 × 18 px | `(4, 78)` | `8 × 16 px` | Buzzy Beetle — Overworld (Leo tường trái) |
| **#38** | (39, 77) | 10 × 18 px | `(40, 78)` | `8 × 16 px` | Buzzy Beetle — Overworld (Leo tường phải) |
| **#39** | (77, 77) | 10 × 18 px | `(78, 78)` | `8 × 16 px` | Buzzy Beetle — Underground (Leo tường trái) |
| **#40** | (113, 77) | 10 × 18 px | `(114, 78)` | `8 × 16 px` | Buzzy Beetle — Underground (Leo tường phải) |
| **#41** | (151, 77) | 10 × 18 px | `(152, 78)` | `8 × 16 px` | Buzzy Beetle — Castle (Leo tường trái) |
| **#42** | (187, 77) | 10 × 18 px | `(188, 78)` | `8 × 16 px` | Buzzy Beetle — Castle (Leo tường phải) |
| **#43** | (225, 77) | 10 × 18 px | `(226, 78)` | `8 × 16 px` | Buzzy Beetle — Underwater (Leo tường trái) |
| **#44** | (261, 77) | 10 × 18 px | `(262, 78)` | `8 × 16 px` | Buzzy Beetle — Underwater (Leo tường phải) |
| **#45** | (18, 81) | 16 × 10 px | `(18, 82)` | `16 × 8 px` | Buzzy Beetle — Overworld (Bò trần nhà trái) |
| **#46** | (54, 81) | 16 × 10 px | `(54, 82)` | `16 × 8 px` | Buzzy Beetle — Overworld (Bò trần nhà phải) |
| **#47** | (92, 81) | 16 × 10 px | `(92, 82)` | `16 × 8 px` | Buzzy Beetle — Underground (Bò trần nhà trái) |
| **#48** | (128, 81) | 16 × 10 px | `(128, 82)` | `16 × 8 px` | Buzzy Beetle — Underground (Bò trần nhà phải) |
| **#49** | (166, 81) | 16 × 10 px | `(166, 82)` | `16 × 8 px` | Buzzy Beetle — Castle (Bò trần nhà trái) |
| **#50** | (202, 81) | 16 × 10 px | `(202, 82)` | `16 × 8 px` | Buzzy Beetle — Castle (Bò trần nhà phải) |
| **#51** | (240, 81) | 16 × 10 px | `(240, 82)` | `16 × 8 px` | Buzzy Beetle — Underwater (Bò trần nhà trái) |
| **#52** | (276, 81) | 16 × 10 px | `(276, 82)` | `16 × 8 px` | Buzzy Beetle — Underwater (Bò trần nhà phải) |
| **#53** | (-1, 109) | 20 × 28 px | `(0, 112)` | `16 × 24 px` | Koopa Troopa xanh — Overworld (đi bộ, khung 1) |
| **#54** | (16, 109) | 21 × 28 px | `(18, 112)` | `16 × 24 px` | Koopa Troopa xanh — Overworld (đi bộ, khung 2) |
| **#55** | (33, 109) | 20 × 28 px | `(36, 112)` | `16 × 24 px` | Koopa Paratroopa xanh — Overworld (bay, khung 1) |
| **#56** | (53, 111) | 18 × 26 px | `(54, 112)` | `16 × 24 px` | Koopa Paratroopa xanh — Overworld (bay, khung 2) |
| **#57** | (145, 109) | 19 × 28 px | `(146, 112)` | `16 × 24 px` | Koopa Troopa xanh — Underground + Castle (đi bộ, khung 1) |
| **#58** | (161, 109) | 21 × 28 px | `(164, 112)` | `16 × 24 px` | Koopa Troopa xanh — Underground + Castle (đi bộ, khung 2) |
| **#59** | (179, 109) | 22 × 28 px | `(182, 112)` | `16 × 24 px` | Koopa Paratroopa xanh — Underground + Castle (bay, khung 1) |
| **#60** | (198, 109) | 21 × 28 px | `(200, 112)` | `16 × 24 px` | Koopa Paratroopa xanh — Underground + Castle (bay, khung 2) |
| **#61** | (291, 109) | 20 × 28 px | `(292, 112)` | `16 × 24 px` | Koopa Troopa xanh — Underwater (đi bộ, khung 1) |
| **#62** | (308, 109) | 21 × 28 px | `(310, 112)` | `16 × 24 px` | Koopa Troopa xanh — Underwater (đi bộ, khung 2) |
| **#63** | (325, 109) | 20 × 28 px | `(328, 112)` | `16 × 24 px` | Koopa Paratroopa xanh — Underwater (bay, khung 1) |
| **#64** | (345, 111) | 18 × 26 px | `(346, 112)` | `16 × 24 px` | Koopa Paratroopa xanh — Underwater (bay, khung 2) |
| **#65** | (71, 119) | 18 × 16 px | `(72, 120)` | `16 × 16 px` | Vỏ Koopa xanh — Overworld (rút vào vỏ / shell idle) |
| **#66** | (89, 119) | 18 × 17 px | `(90, 120)` | `16 × 16 px` | Vỏ Koopa xanh — Overworld (thò chân / waking) |
| **#67** | (217, 119) | 18 × 16 px | `(218, 120)` | `16 × 16 px` | Vỏ Koopa xanh — Underground + Castle (rút vào vỏ) |
| **#68** | (235, 119) | 18 × 17 px | `(236, 120)` | `16 × 16 px` | Vỏ Koopa xanh — Underground + Castle (thò chân / waking) |
| **#69** | (363, 119) | 18 × 16 px | `(364, 120)` | `16 × 16 px` | Vỏ Koopa xanh — Underwater (rút vào vỏ) |
| **#70** | (381, 119) | 18 × 17 px | `(382, 120)` | `16 × 16 px` | Vỏ Koopa xanh — Underwater (thò chân / waking) |
| **#71** | (-1, 138) | 18 × 25 px | `(0, 138)` | `16 × 24 px` | Piranha Plant — Overworld (khép miệng) |
| **#72** | (17, 137) | 18 × 26 px | `(18, 138)` | `16 × 24 px` | Piranha Plant — Overworld (há miệng) |
| **#73** | (53, 137) | 18 × 26 px | `(54, 138)` | `16 × 24 px` | Lakitu — Overworld (nhô khỏi mây, chuẩn bị ném trứng Spiny) |
| **#74** | (145, 138) | 18 × 25 px | `(146, 138)` | `16 × 24 px` | Piranha Plant — Underground + Castle (khép miệng) |
| **#75** | (163, 137) | 18 × 26 px | `(164, 138)` | `16 × 24 px` | Piranha Plant — Underground + Castle (há miệng) |
| **#76** | (199, 137) | 18 × 26 px | `(200, 138)` | `16 × 24 px` | Lakitu — Underground + Castle (nhô khỏi mây, chuẩn bị ném trứng Spiny) |
| **#77** | (291, 138) | 18 × 25 px | `(292, 138)` | `16 × 24 px` | Piranha Plant — Underwater (khép miệng) |
| **#78** | (309, 137) | 18 × 26 px | `(310, 138)` | `16 × 24 px` | Piranha Plant — Underwater (há miệng) |
| **#79** | (345, 137) | 18 × 26 px | `(346, 138)` | `16 × 24 px` | Lakitu — Underwater (nhô khỏi mây, chuẩn bị ném trứng Spiny) |
| **#80** | (71, 145) | 18 × 18 px | `(72, 146)` | `16 × 16 px` | Lakitu — Overworld (ẩn trong mây / mây trống) |
| **#81** | (217, 145) | 18 × 18 px | `(218, 146)` | `16 × 16 px` | Lakitu — Underground + Castle (ẩn trong mây / mây trống) |
| **#82** | (363, 145) | 18 × 18 px | `(364, 146)` | `16 × 16 px` | Lakitu — Underwater (ẩn trong mây / mây trống) |
| **#83** | (-1, 163) | 18 × 18 px | `(0, 164)` | `16 × 16 px` | Cheep Cheep — Overworld (bơi, vây lên / khung 1) |
| **#84** | (17, 163) | 17 × 18 px | `(18, 164)` | `16 × 16 px` | Cheep Cheep — Overworld (bơi, vây xuống / khung 2) |
| **#85** | (145, 163) | 18 × 18 px | `(146, 164)` | `16 × 16 px` | Cheep Cheep — Underground + Castle (bơi, vây lên / khung 1) |
| **#86** | (163, 163) | 17 × 18 px | `(164, 164)` | `16 × 16 px` | Cheep Cheep — Underground + Castle (bơi, vây xuống / khung 2) |
| **#87** | (291, 163) | 18 × 18 px | `(292, 164)` | `16 × 16 px` | Cheep Cheep — Underwater (bơi, vây lên / khung 1) |
| **#88** | (309, 163) | 17 × 18 px | `(310, 164)` | `16 × 16 px` | Cheep Cheep — Underwater (bơi, vây xuống / khung 2) |
| **#89** | (72, 171) | 17 × 36 px | `(72, 172)` | `16 × 34 px` | Hammer Bro — Overworld (ném búa, khung 1) |
| **#90** | (90, 171) | 17 × 36 px | `(90, 172)` | `16 × 34 px` | Hammer Bro — Overworld (ném búa, khung 2) |
| **#91** | (107, 171) | 17 × 36 px | `(108, 172)` | `16 × 34 px` | Hammer Bro — Overworld (ném búa, khung 3) |
| **#92** | (125, 171) | 17 × 36 px | `(126, 172)` | `16 × 34 px` | Hammer Bro — Overworld (ném búa, khung 4) |
| **#93** | (218, 171) | 17 × 36 px | `(218, 172)` | `16 × 34 px` | Hammer Bro — Underground + Castle (ném búa, khung 1) |
| **#94** | (236, 171) | 17 × 36 px | `(236, 172)` | `16 × 34 px` | Hammer Bro — Underground + Castle (ném búa, khung 2) |
| **#95** | (253, 171) | 17 × 36 px | `(254, 172)` | `16 × 34 px` | Hammer Bro — Underground + Castle (ném búa, khung 3) |
| **#96** | (271, 171) | 17 × 36 px | `(272, 172)` | `16 × 34 px` | Hammer Bro — Underground + Castle (ném búa, khung 4) |
| **#97** | (364, 171) | 17 × 36 px | `(364, 172)` | `16 × 34 px` | Hammer Bro — Underwater (ném búa, khung 1) |
| **#98** | (382, 171) | 17 × 36 px | `(382, 172)` | `16 × 34 px` | Hammer Bro — Underwater (ném búa, khung 2) |
| **#99** | (399, 171) | 17 × 36 px | `(400, 172)` | `16 × 34 px` | Hammer Bro — Underwater (ném búa, khung 3) |
| **#100** | (417, 171) | 17 × 36 px | `(418, 172)` | `16 × 34 px` | Hammer Bro — Underwater (ném búa, khung 4) |
| **#101** | (-1, 181) | 18 × 26 px | `(0, 182)` | `16 × 24 px` | Hammer Bro — Overworld (đứng/đi bộ, khung 1) |
| **#102** | (17, 181) | 18 × 26 px | `(18, 182)` | `16 × 24 px` | Hammer Bro — Overworld (đứng/đi bộ, khung 2) |
| **#103** | (36, 181) | 17 × 26 px | `(36, 182)` | `16 × 24 px` | Hammer Bro — Overworld (đứng/đi bộ, khung 3) |
| **#104** | (54, 181) | 17 × 26 px | `(54, 182)` | `16 × 24 px` | Hammer Bro — Overworld (đứng/đi bộ, khung 4) |
| **#105** | (145, 181) | 18 × 26 px | `(146, 182)` | `16 × 24 px` | Hammer Bro — Underground + Castle (đứng/đi bộ, khung 1) |
| **#106** | (163, 181) | 18 × 26 px | `(164, 182)` | `16 × 24 px` | Hammer Bro — Underground + Castle (đứng/đi bộ, khung 2) |
| **#107** | (182, 181) | 17 × 26 px | `(182, 182)` | `16 × 24 px` | Hammer Bro — Underground + Castle (đứng/đi bộ, khung 3) |
| **#108** | (200, 181) | 17 × 26 px | `(200, 182)` | `16 × 24 px` | Hammer Bro — Underground + Castle (đứng/đi bộ, khung 4) |
| **#109** | (291, 181) | 18 × 26 px | `(292, 182)` | `16 × 24 px` | Hammer Bro — Underwater (đứng/đi bộ, khung 1) |
| **#110** | (309, 181) | 18 × 26 px | `(310, 182)` | `16 × 24 px` | Hammer Bro — Underwater (đứng/đi bộ, khung 2) |
| **#111** | (328, 181) | 17 × 26 px | `(328, 182)` | `16 × 24 px` | Hammer Bro — Underwater (đứng/đi bộ, khung 3) |
| **#112** | (346, 181) | 17 × 26 px | `(346, 182)` | `16 × 24 px` | Hammer Bro — Underwater (đứng/đi bộ, khung 4) |
| **#113** | (-1, 207) | 34 × 34 px | `(0, 208)` | `32 × 32 px` | Bowser — Overworld (đi bộ, miệng khép / khung 1) |
| **#114** | (33, 207) | 34 × 34 px | `(34, 208)` | `32 × 32 px` | Bowser — Overworld (đi bộ, miệng mở / khung 2) |
| **#115** | (67, 207) | 34 × 34 px | `(68, 208)` | `32 × 32 px` | Bowser — Overworld (tư thế khè lửa, khung 1) |
| **#116** | (101, 207) | 34 × 34 px | `(102, 208)` | `32 × 32 px` | Bowser — Overworld (tư thế khè lửa, khung 2) |
| **#117** | (145, 207) | 34 × 34 px | `(146, 208)` | `32 × 32 px` | Bowser — Underground + Castle (đi bộ, miệng khép / khung 1) |
| **#118** | (179, 207) | 34 × 34 px | `(180, 208)` | `32 × 32 px` | Bowser — Underground + Castle (đi bộ, miệng mở / khung 2) |
| **#119** | (213, 207) | 34 × 34 px | `(214, 208)` | `32 × 32 px` | Bowser — Underground + Castle (tư thế khè lửa, khung 1) |
| **#120** | (247, 207) | 34 × 34 px | `(248, 208)` | `32 × 32 px` | Bowser — Underground + Castle (tư thế khè lửa, khung 2) |
| **#121** | (291, 207) | 34 × 34 px | `(292, 208)` | `32 × 32 px` | Bowser — Underwater (đi bộ, miệng khép / khung 1) |
| **#122** | (325, 207) | 34 × 34 px | `(326, 208)` | `32 × 32 px` | Bowser — Underwater (đi bộ, miệng mở / khung 2) |
| **#123** | (359, 207) | 34 × 34 px | `(360, 208)` | `32 × 32 px` | Bowser — Underwater (tư thế khè lửa, khung 1) |
| **#124** | (393, 207) | 34 × 34 px | `(394, 208)` | `32 × 32 px` | Bowser — Underwater (tư thế khè lửa, khung 2) |
| **#125** | (33, 241) | 32 × 30 px | `(34, 242)` | `32 × 26 px` | Bowser — Overworld (giơ búa, quay trái) |
| **#126** | (69, 241) | 32 × 28 px | `(68, 242)` | `32 × 26 px` | Bowser — Overworld (giơ búa, quay phải) |
| **#127** | (101, 241) | 26 × 10 px | `(102, 242)` | `24 × 8 px` | Fireball của Bowser — Overworld (khung 1) |
| **#128** | (179, 241) | 32 × 30 px | `(180, 242)` | `32 × 26 px` | Bowser — Underground + Castle (giơ búa, quay trái) |
| **#129** | (215, 241) | 32 × 28 px | `(214, 242)` | `32 × 26 px` | Bowser — Underground + Castle (giơ búa, quay phải) |
| **#130** | (247, 241) | 26 × 10 px | `(248, 242)` | `24 × 8 px` | Fireball của Bowser — Underground + Castle (khung 1) |
| **#131** | (325, 241) | 32 × 28 px | `(326, 242)` | `32 × 26 px` | Bowser — Underwater (giơ búa, quay trái) |
| **#132** | (361, 241) | 32 × 28 px | `(360, 242)` | `32 × 26 px` | Bowser — Underwater (giơ búa, quay phải) |
| **#133** | (393, 241) | 26 × 10 px | `(394, 242)` | `24 × 8 px` | Fireball của Bowser — Underwater (khung 1) |
| **#134** | (101, 251) | 26 × 10 px | `(102, 252)` | `24 × 8 px` | Fireball của Bowser — Overworld (khung 2) |
| **#135** | (247, 251) | 26 × 10 px | `(248, 252)` | `24 × 8 px` | Fireball của Bowser — Underground + Castle (khung 2) |
| **#136** | (393, 251) | 26 × 10 px | `(394, 252)` | `24 × 8 px` | Fireball của Bowser — Underwater (khung 2) |
| **#137** | (32, 273) | 33 × 32 px | `(34, 276)` | `32 × 26 px` | Bowser — In Castle 1 (giơ búa, quay trái / khung 1) |
| **#138** | (69, 275) | 32 × 28 px | `(68, 276)` | `32 × 26 px` | Bowser — In Castle 1 (giơ búa, quay phải / khung 2) |
| **#139** | (144, 273) | 18 × 30 px | `(144, 276)` | `16 × 26 px` | Hammer Bro — In Castle 2 (ném búa, khung 1) |
| **#140** | (159, 273) | 22 × 30 px | `(162, 276)` | `16 × 26 px` | Hammer Bro — In Castle 2 (ném búa, khung 2) |
| **#141** | (178, 273) | 33 × 30 px | `(180, 276)` | `32 × 26 px` | Bowser — In Castle 2 (giơ búa, quay trái / khung 1) |
| **#142** | (215, 275) | 32 × 28 px | `(214, 276)` | `32 × 26 px` | Bowser — In Castle 2 (giơ búa, quay phải / khung 2) |
| **#143** | (-1, 315) | 20 × 28 px | `(0, 318)` | `16 × 24 px` | Red Koopa Troopa — đi bộ, khung 1 |
| **#144** | (16, 315) | 19 × 28 px | `(18, 318)` | `16 × 24 px` | Red Koopa Troopa — đi bộ, khung 2 |
| **#145** | (35, 315) | 19 × 28 px | `(36, 318)` | `16 × 24 px` | Red Koopa Paratroopa — bay, khung 1 |
| **#146** | (51, 315) | 20 × 28 px | `(54, 318)` | `16 × 24 px` | Red Koopa Paratroopa — bay, khung 2 |
| **#147** | (71, 325) | 18 × 16 px | `(72, 326)` | `16 × 16 px` | Vỏ Red Koopa — rút vào vỏ / shell idle |
| **#148** | (89, 325) | 18 × 17 px | `(90, 326)` | `16 × 16 px` | Vỏ Red Koopa — thò chân / waking |
| **#149** | (-1, 344) | 18 × 25 px | `(0, 344)` | `16 × 24 px` | Red Piranha Plant — khép miệng |
| **#150** | (17, 343) | 18 × 26 px | `(18, 344)` | `16 × 24 px` | Red Piranha Plant — há miệng |
| **#151** | (36, 351) | 16 × 18 px | `(36, 352)` | `16 × 16 px` | Red Spiny Egg — đang rơi/xoay, khung 1 |
| **#152** | (54, 351) | 16 × 18 px | `(54, 352)` | `16 × 16 px` | Red Spiny Egg — đang rơi/xoay, khung 2 |
| **#153** | (71, 352) | 18 × 17 px | `(72, 352)` | `16 × 16 px` | Red Spiny — đã đáp đất, bò, khung 1 |
| **#154** | (89, 351) | 18 × 18 px | `(90, 352)` | `16 × 16 px` | Red Spiny — đã đáp đất, bò, khung 2 |
| **#155** | (-1, 369) | 18 × 18 px | `(0, 370)` | `16 × 16 px` | Red Cheep Cheep — bơi, vây lên / khung 1 |
| **#156** | (17, 369) | 17 × 18 px | `(18, 370)` | `16 × 16 px` | Red Cheep Cheep — bơi, vây xuống / khung 2 |
| **#157** | (90, 369) | 16 × 18 px | `(90, 370)` | `16 × 16 px` | Lava Bubble (Podoboo) — nhảy/vọt lên từ lava |

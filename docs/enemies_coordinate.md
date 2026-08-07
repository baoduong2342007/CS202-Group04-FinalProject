# Bảng Tọa Độ & Ý Nghĩa Chi Tiết Khung Hình Quái Vật (`enemies_coordinate.md`)

Danh sách toàn bộ **157** thành phần liên thông quái vật tách từ tệp
`docs/assets/reference/enemies.png` (đã lọc bỏ nền xanh đậm `(0,41,140)` và
xanh trung `(146,144,255)`, loại trừ vùng Tilemap reference và text box):

> **Quy ước palette (bảng màu NES):**
> - **Overworld (OW)**: Xanh lá `(12,147,0)` + Cam `(230,156,33)` — dùng cho Level 1-1, 1-3, 3-1, v.v.
> - **Underground (UG)**: Teal `(0,123,140)` + Nâu `(156,74,0)` + Hồng nhạt `(255,206,197)` — dùng cho Level 1-2, 4-2, v.v.
> - **Castle (CT)**: Xám `(173,173,173)` + Cam `(230,156,33)` — dùng cho Level 1-4, 2-4, v.v.
> - **Underwater (UW)**: Teal nhạt `(181,239,239)` — dùng cho Level 2-2, 7-2, v.v.
> - **Red / All Environments**: Đỏ `(181,49,33)` + Cam `(230,156,33)` — một số quái chỉ có 1 palette dùng chung mọi màn.

---

## Hàng 1 — Goomba (y ≈ 16–32)

| STT | Tọa độ (X, Y) | Kích thước (W × H) | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|----------------|---------------------|---------------------------------------|
| **#1** | `(0, 16)` | `16 × 16 px` | Goomba — Overworld (Đi bộ khung 1) |
| **#2** | `(18, 16)` | `16 × 16 px` | Goomba — Overworld (Đi bộ khung 2) |
| **#3** | `(74, 16)` | `16 × 16 px` | Goomba — Underground (Đi bộ khung 1) |
| **#4** | `(92, 16)` | `16 × 16 px` | Goomba — Underground (Đi bộ khung 2) |
| **#5** | `(148, 16)` | `16 × 16 px` | Goomba — Castle (Đi bộ khung 1) |
| **#6** | `(166, 16)` | `16 × 16 px` | Goomba — Castle (Đi bộ khung 2) |
| **#7** | `(222, 16)` | `16 × 16 px` | Goomba — Underwater (Đi bộ khung 1) |
| **#8** | `(240, 16)` | `16 × 16 px` | Goomba — Underwater (Đi bộ khung 2) |

---

## Hàng 2 — Goomba bị dẹt (y ≈ 24)

| STT | Tọa độ (X, Y) | Kích thước (W × H) | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|----------------|---------------------|---------------------------------------|
| **#9** | `(36, 24)` | `16 × 8 px` | Goomba — Overworld (Bị giẫm dẹt) |
| **#10** | `(110, 24)` | `16 × 8 px` | Goomba — Underground (Bị giẫm dẹt) |
| **#11** | `(184, 24)` | `16 × 8 px` | Goomba — Castle (Bị giẫm dẹt) |
| **#12** | `(258, 24)` | `16 × 8 px` | Goomba — Underwater (Bị giẫm dẹt) |

---

## Hàng 3 — Buzzy Beetle (y ≈ 34–50)

| STT | Tọa độ (X, Y) | Kích thước (W × H) | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|----------------|---------------------|---------------------------------------|
| **#13** | `(18, 34)` | `16 × 16 px` | Buzzy Beetle — Overworld (Đi bộ khung 2) |
| **#14** | `(92, 34)` | `16 × 16 px` | Buzzy Beetle — Underground (Đi bộ khung 2) |
| **#15** | `(166, 34)` | `16 × 16 px` | Buzzy Beetle — Castle (Đi bộ khung 2) |
| **#16** | `(240, 34)` | `16 × 16 px` | Buzzy Beetle — Underwater (Đi bộ khung 2) |
| **#17** | `(0, 35)` | `16 × 15 px` | Buzzy Beetle — Overworld (Đi bộ khung 1) |
| **#18** | `(36, 35)` | `16 × 15 px` | Buzzy Beetle — Overworld (Mai rùa / Shell) |
| **#19** | `(74, 35)` | `16 × 15 px` | Buzzy Beetle — Underground (Đi bộ khung 1) |
| **#20** | `(110, 35)` | `16 × 15 px` | Buzzy Beetle — Underground (Mai rùa / Shell) |
| **#21** | `(148, 35)` | `16 × 15 px` | Buzzy Beetle — Castle (Đi bộ khung 1) |
| **#22** | `(184, 35)` | `16 × 15 px` | Buzzy Beetle — Castle (Mai rùa / Shell) |
| **#23** | `(222, 35)` | `16 × 15 px` | Buzzy Beetle — Underwater (Đi bộ khung 1) |
| **#24** | `(258, 35)` | `16 × 15 px` | Buzzy Beetle — Underwater (Mai rùa / Shell) |

---

## Hàng 4 — Blooper & Bullet Bill (y ≈ 52–67)

| STT | Tọa độ (X, Y) | Kích thước (W × H) | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|----------------|---------------------|---------------------------------------|
| **#25** | `(0, 52)` | `16 × 24 px` | Blooper — Overworld (Xòe tua bơi) |
| **#26** | `(18, 52)` | `16 × 16 px` | Blooper — Overworld (Khép tua bơi) |
| **#27** | `(74, 52)` | `16 × 24 px` | Blooper — Underground (Xòe tua bơi) |
| **#28** | `(92, 52)` | `16 × 16 px` | Blooper — Underground (Khép tua bơi) |
| **#29** | `(148, 52)` | `16 × 24 px` | Blooper — Castle (Xòe tua bơi) |
| **#30** | `(166, 52)` | `16 × 16 px` | Blooper — Castle (Khép tua bơi) |
| **#31** | `(222, 52)` | `16 × 24 px` | Blooper — Underwater (Xòe tua bơi) |
| **#32** | `(240, 52)` | `16 × 16 px` | Blooper — Underwater (Khép tua bơi) |
| **#33** | `(54, 53)` | `16 × 14 px` | Bullet Bill — Overworld |
| **#34** | `(128, 53)` | `16 × 14 px` | Bullet Bill — Underground |
| **#35** | `(202, 53)` | `16 × 14 px` | Bullet Bill — Castle |
| **#36** | `(276, 53)` | `16 × 14 px` | Bullet Bill — Underwater |

---

## Hàng 5 — Firebar / Bọt lửa (y ≈ 78–90)

| STT | Tọa độ (X, Y) | Kích thước (W × H) | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|----------------|---------------------|---------------------------------------|
| **#37** | `(4, 78)` | `8 × 16 px` | Firebar — Overworld (Dọc khung 1) |
| **#38** | `(40, 78)` | `8 × 16 px` | Firebar — Overworld (Dọc khung 2) |
| **#39** | `(78, 78)` | `8 × 16 px` | Firebar — Underground (Dọc khung 1) |
| **#40** | `(114, 78)` | `8 × 16 px` | Firebar — Underground (Dọc khung 2) |
| **#41** | `(152, 78)` | `8 × 16 px` | Firebar — Castle (Dọc khung 1) |
| **#42** | `(188, 78)` | `8 × 16 px` | Firebar — Castle (Dọc khung 2) |
| **#43** | `(226, 78)` | `8 × 16 px` | Firebar — Underwater (Dọc khung 1) |
| **#44** | `(262, 78)` | `8 × 16 px` | Firebar — Underwater (Dọc khung 2) |
| **#45** | `(19, 82)` | `14 × 8 px` | Firebar — Overworld (Ngang khung 1) |
| **#46** | `(55, 82)` | `14 × 8 px` | Firebar — Overworld (Ngang khung 2) |
| **#47** | `(93, 82)` | `14 × 8 px` | Firebar — Underground (Ngang khung 1) |
| **#48** | `(129, 82)` | `14 × 8 px` | Firebar — Underground (Ngang khung 2) |
| **#49** | `(167, 82)` | `14 × 8 px` | Firebar — Castle (Ngang khung 1) |
| **#50** | `(203, 82)` | `14 × 8 px` | Firebar — Castle (Ngang khung 2) |
| **#51** | `(241, 82)` | `14 × 8 px` | Firebar — Underwater (Ngang khung 1) |
| **#52** | `(277, 82)` | `14 × 8 px` | Firebar — Underwater (Ngang khung 2) |

---

## Hàng 6 — Spiny / Rùa gai (y ≈ 112–136) — "Green Enemies" section

Spritesheet chỉ có **3 nhóm palette** cho Spiny: Overworld, Underground+Castle (UG), Underwater (UW).

| STT | Tọa độ (X, Y) | Kích thước (W × H) | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|----------------|---------------------|---------------------------------------|
| **#53** | `(18, 112)` | `16 × 24 px` | Spiny — Overworld (Đi bộ khung 2) |
| **#54** | `(54, 112)` | `16 × 24 px` | Spiny — Overworld (Trứng lăn khung 2) |
| **#55** | `(164, 112)` | `16 × 24 px` | Spiny — Underground+Castle (Đi bộ khung 2) |
| **#56** | `(200, 112)` | `16 × 24 px` | Spiny — Underground+Castle (Trứng lăn khung 2) |
| **#57** | `(310, 112)` | `16 × 24 px` | Spiny — Underwater (Đi bộ khung 2) |
| **#58** | `(346, 112)` | `16 × 24 px` | Spiny — Underwater (Trứng lăn khung 2) |
| **#59** | `(0, 113)` | `16 × 23 px` | Spiny — Overworld (Đi bộ khung 1) |
| **#60** | `(36, 113)` | `16 × 23 px` | Spiny — Overworld (Trứng lăn khung 1) |
| **#61** | `(146, 113)` | `16 × 23 px` | Spiny — Underground+Castle (Đi bộ khung 1) |
| **#62** | `(182, 113)` | `16 × 23 px` | Spiny — Underground+Castle (Trứng lăn khung 1) |
| **#63** | `(292, 113)` | `16 × 23 px` | Spiny — Underwater (Đi bộ khung 1) |
| **#64** | `(328, 113)` | `16 × 23 px` | Spiny — Underwater (Trứng lăn khung 1) |
| **#65** | `(72, 120)` | `16 × 14 px` | Spiny — Overworld (Mai gai bị giẫm khung 1) |
| **#66** | `(90, 120)` | `16 × 15 px` | Spiny — Overworld (Mai gai bị giẫm khung 2) |
| **#67** | `(218, 120)` | `16 × 14 px` | Spiny — Underground+Castle (Mai gai bị giẫm khung 1) |
| **#68** | `(236, 120)` | `16 × 15 px` | Spiny — Underground+Castle (Mai gai bị giẫm khung 2) |
| **#69** | `(364, 120)` | `16 × 14 px` | Spiny — Underwater (Mai gai bị giẫm khung 1) |
| **#70** | `(382, 120)` | `16 × 15 px` | Spiny — Underwater (Mai gai bị giẫm khung 2) |

---

## Hàng 7 — Koopa Troopa (y ≈ 138–162) — "Green Enemies" section

3 nhóm palette: Overworld, Underground+Castle, Underwater.

| STT | Tọa độ (X, Y) | Kích thước (W × H) | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|----------------|---------------------|---------------------------------------|
| **#71** | `(18, 138)` | `16 × 24 px` | Koopa Troopa — Overworld (Đi bộ khung 2) |
| **#72** | `(54, 138)` | `16 × 24 px` | Koopa Troopa — Overworld (Mai rùa đang trượt / Kicked Shell) |
| **#73** | `(164, 138)` | `16 × 24 px` | Koopa Troopa — Underground+Castle (Đi bộ khung 2) |
| **#74** | `(200, 138)` | `16 × 24 px` | Koopa Troopa — Underground+Castle (Mai rùa đang trượt / Kicked Shell) |
| **#75** | `(310, 138)` | `16 × 24 px` | Koopa Troopa — Underwater (Đi bộ khung 2) |
| **#76** | `(346, 138)` | `16 × 24 px` | Koopa Troopa — Underwater (Mai rùa đang trượt / Kicked Shell) |
| **#77** | `(0, 139)` | `16 × 23 px` | Koopa Troopa — Overworld (Đi bộ khung 1) |
| **#78** | `(146, 139)` | `16 × 23 px` | Koopa Troopa — Underground+Castle (Đi bộ khung 1) |
| **#79** | `(292, 139)` | `16 × 23 px` | Koopa Troopa — Underwater (Đi bộ khung 1) |
| **#80** | `(72, 146)` | `16 × 16 px` | Koopa Troopa — Overworld (Mai rùa thụt đầu / Shell idle) |
| **#81** | `(218, 146)` | `16 × 16 px` | Koopa Troopa — Underground+Castle (Mai rùa thụt đầu / Shell idle) |
| **#82** | `(364, 146)` | `16 × 16 px` | Koopa Troopa — Underwater (Mai rùa thụt đầu / Shell idle) |

---

## Hàng 8 — Koopa Troopa lật ngửa (y ≈ 164–180)

| STT | Tọa độ (X, Y) | Kích thước (W × H) | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|----------------|---------------------|---------------------------------------|
| **#83** | `(0, 164)` | `16 × 16 px` | Koopa Troopa — Overworld (Mai rùa lật ngửa khung 1) |
| **#84** | `(18, 164)` | `15 × 16 px` | Koopa Troopa — Overworld (Mai rùa lật ngửa khung 2) |
| **#85** | `(146, 164)` | `16 × 16 px` | Koopa Troopa — Underground+Castle (Mai rùa lật ngửa khung 1) |
| **#86** | `(164, 164)` | `15 × 16 px` | Koopa Troopa — Underground+Castle (Mai rùa lật ngửa khung 2) |
| **#87** | `(292, 164)` | `16 × 16 px` | Koopa Troopa — Underwater (Mai rùa lật ngửa khung 1) |
| **#88** | `(310, 164)` | `15 × 16 px` | Koopa Troopa — Underwater (Mai rùa lật ngửa khung 2) |

---

## Hàng 9 — Koopa Paratroopa / Rùa cánh (y ≈ 172–206) — "Green Enemies" section

15×34 px — sprite cao kèm cánh. 3 nhóm palette × 4 khung hình = 12 sprite.

| STT | Tọa độ (X, Y) | Kích thước (W × H) | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|----------------|---------------------|---------------------------------------|
| **#89** | `(73, 172)` | `15 × 34 px` | Koopa Paratroopa — Overworld (Bay khung 1 — cánh lên) |
| **#90** | `(91, 172)` | `15 × 34 px` | Koopa Paratroopa — Overworld (Bay khung 2 — cánh xuống) |
| **#91** | `(108, 172)` | `15 × 34 px` | Koopa Paratroopa — Overworld (Bay khung 3 — cánh lên) |
| **#92** | `(126, 172)` | `15 × 34 px` | Koopa Paratroopa — Overworld (Bay khung 4 — cánh xuống) |
| **#93** | `(219, 172)` | `15 × 34 px` | Koopa Paratroopa — Underground+Castle (Bay khung 1) |
| **#94** | `(237, 172)` | `15 × 34 px` | Koopa Paratroopa — Underground+Castle (Bay khung 2) |
| **#95** | `(254, 172)` | `15 × 34 px` | Koopa Paratroopa — Underground+Castle (Bay khung 3) |
| **#96** | `(272, 172)` | `15 × 34 px` | Koopa Paratroopa — Underground+Castle (Bay khung 4) |
| **#97** | `(365, 172)` | `15 × 34 px` | Koopa Paratroopa — Underwater (Bay khung 1) |
| **#98** | `(383, 172)` | `15 × 34 px` | Koopa Paratroopa — Underwater (Bay khung 2) |
| **#99** | `(400, 172)` | `15 × 34 px` | Koopa Paratroopa — Underwater (Bay khung 3) |
| **#100** | `(418, 172)` | `15 × 34 px` | Koopa Paratroopa — Underwater (Bay khung 4) |

---

## Hàng 10 — Cheep Cheep / Cá chép bay (y ≈ 182–206) — "Green Enemies" section

3 nhóm palette × 4 khung hình = 12 sprite.

| STT | Tọa độ (X, Y) | Kích thước (W × H) | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|----------------|---------------------|---------------------------------------|
| **#101** | `(0, 182)` | `16 × 24 px` | Cheep Cheep — Overworld (Bơi khung 1) |
| **#102** | `(18, 182)` | `16 × 24 px` | Cheep Cheep — Overworld (Bơi khung 2) |
| **#103** | `(37, 182)` | `15 × 24 px` | Cheep Cheep — Overworld (Bơi khung 3 — vây mở) |
| **#104** | `(55, 182)` | `15 × 24 px` | Cheep Cheep — Overworld (Bơi khung 4 — vây khép) |
| **#105** | `(146, 182)` | `16 × 24 px` | Cheep Cheep — Underground+Castle (Bơi khung 1) |
| **#106** | `(164, 182)` | `16 × 24 px` | Cheep Cheep — Underground+Castle (Bơi khung 2) |
| **#107** | `(183, 182)` | `15 × 24 px` | Cheep Cheep — Underground+Castle (Bơi khung 3) |
| **#108** | `(201, 182)` | `15 × 24 px` | Cheep Cheep — Underground+Castle (Bơi khung 4) |
| **#109** | `(292, 182)` | `16 × 24 px` | Cheep Cheep — Underwater (Bơi khung 1) |
| **#110** | `(310, 182)` | `16 × 24 px` | Cheep Cheep — Underwater (Bơi khung 2) |
| **#111** | `(329, 182)` | `15 × 24 px` | Cheep Cheep — Underwater (Bơi khung 3) |
| **#112** | `(347, 182)` | `15 × 24 px` | Cheep Cheep — Underwater (Bơi khung 4) |

---

## Hàng 11 — Bowser / Trùm rùa (y ≈ 208–240) — 3 palette × 4 khung = 12 sprite

| STT | Tọa độ (X, Y) | Kích thước (W × H) | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|----------------|---------------------|---------------------------------------|
| **#113** | `(0, 208)` | `32 × 32 px` | Bowser — Overworld (Đi bộ khung 1 — khép miệng) |
| **#114** | `(34, 208)` | `32 × 32 px` | Bowser — Overworld (Đi bộ khung 2 — há miệng) |
| **#115** | `(68, 208)` | `32 × 32 px` | Bowser — Overworld (Khè lửa khung 1) |
| **#116** | `(102, 208)` | `32 × 32 px` | Bowser — Overworld (Khè lửa khung 2) |
| **#117** | `(146, 208)` | `32 × 32 px` | Bowser — Underground+Castle (Đi bộ khung 1) |
| **#118** | `(180, 208)` | `32 × 32 px` | Bowser — Underground+Castle (Đi bộ khung 2) |
| **#119** | `(214, 208)` | `32 × 32 px` | Bowser — Underground+Castle (Khè lửa khung 1) |
| **#120** | `(248, 208)` | `32 × 32 px` | Bowser — Underground+Castle (Khè lửa khung 2) |
| **#121** | `(292, 208)` | `32 × 32 px` | Bowser — Underwater (Đi bộ khung 1) |
| **#122** | `(326, 208)` | `32 × 32 px` | Bowser — Underwater (Đi bộ khung 2) |
| **#123** | `(360, 208)` | `32 × 32 px` | Bowser — Underwater (Khè lửa khung 1) |
| **#124** | `(394, 208)` | `32 × 32 px` | Bowser — Underwater (Khè lửa khung 2) |

---

## Hàng 12 — Hammer Bros & Hammer / Búa (y ≈ 242–268) — "In Castle levels"

| STT | Tọa độ (X, Y) | Kích thước (W × H) | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|----------------|---------------------|---------------------------------------|
| **#125** | `(34, 242)` | `30 × 26 px` | Hammer Bros — Overworld (Ném búa khung 1 — tay giơ) |
| **#126** | `(70, 242)` | `30 × 26 px` | Hammer Bros — Overworld (Ném búa khung 2 — tay hạ) |
| **#127** | `(102, 242)` | `24 × 8 px` | Hammer (Búa bay) — Overworld (Xoay khung 1) |
| **#128** | `(180, 242)` | `30 × 26 px` | Hammer Bros — Underground+Castle (Ném búa khung 1) |
| **#129** | `(216, 242)` | `30 × 26 px` | Hammer Bros — Underground+Castle (Ném búa khung 2) |
| **#130** | `(248, 242)` | `24 × 8 px` | Hammer (Búa bay) — Underground+Castle (Xoay khung 1) |
| **#131** | `(326, 242)` | `30 × 26 px` | Hammer Bros — Underwater (Ném búa khung 1) |
| **#132** | `(362, 242)` | `30 × 26 px` | Hammer Bros — Underwater (Ném búa khung 2) |
| **#133** | `(394, 242)` | `24 × 8 px` | Hammer (Búa bay) — Underwater (Xoay khung 1) |
| **#134** | `(102, 252)` | `24 × 8 px` | Hammer (Búa bay) — Overworld (Xoay khung 2) |
| **#135** | `(248, 252)` | `24 × 8 px` | Hammer (Búa bay) — Underground+Castle (Xoay khung 2) |
| **#136** | `(394, 252)` | `24 × 8 px` | Hammer (Búa bay) — Underwater (Xoay khung 2) |

---

## Hàng 13 — Lakitu & Piranha Plant Castle (y ≈ 276–302) — "In Castle levels"

| STT | Tọa độ (X, Y) | Kích thước (W × H) | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|----------------|---------------------|---------------------------------------|
| **#137** | `(34, 276)` | `30 × 26 px` | Lakitu — Overworld (Trên mây khung 1 — nhìn xuống) |
| **#138** | `(70, 276)` | `30 × 26 px` | Lakitu — Overworld (Trên mây khung 2 — ném Spiny) |
| **#139** | `(145, 276)` | `15 × 26 px` | Piranha Plant — Underground+Castle (Há miệng) |
| **#140** | `(162, 276)` | `15 × 26 px` | Piranha Plant — Underground+Castle (Khép miệng) |
| **#141** | `(180, 276)` | `30 × 26 px` | Lakitu — Underground+Castle (Trên mây khung 1) |
| **#142** | `(216, 276)` | `30 × 26 px` | Lakitu — Underground+Castle (Trên mây khung 2) |

---

## Hàng 14 — Red Spiny / Rùa gai đỏ (y ≈ 318–340) — "Red Enemies, All Environments"

Palette đỏ `(181,49,33)` — chỉ có 1 bộ palette dùng cho mọi màn.

| STT | Tọa độ (X, Y) | Kích thước (W × H) | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|----------------|---------------------|---------------------------------------|
| **#143** | `(18, 318)` | `16 × 24 px` | Red Spiny — All Environments (Đi bộ khung 2) |
| **#144** | `(54, 318)` | `16 × 24 px` | Red Spiny — All Environments (Trứng lăn khung 2) |
| **#145** | `(0, 319)` | `16 × 23 px` | Red Spiny — All Environments (Đi bộ khung 1) |
| **#146** | `(36, 319)` | `16 × 23 px` | Red Spiny — All Environments (Trứng lăn khung 1) |
| **#147** | `(72, 326)` | `16 × 14 px` | Red Spiny — All Environments (Mai gai bị giẫm khung 1) |
| **#148** | `(90, 326)` | `16 × 15 px` | Red Spiny — All Environments (Mai gai bị giẫm khung 2) |

---

## Hàng 15 — Red Koopa Troopa / Rùa đỏ (y ≈ 344–368) — "Red Enemies"

| STT | Tọa độ (X, Y) | Kích thước (W × H) | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|----------------|---------------------|---------------------------------------|
| **#149** | `(18, 344)` | `16 × 24 px` | Red Koopa Troopa — All Environments (Đi bộ khung 2) |
| **#150** | `(0, 345)` | `16 × 23 px` | Red Koopa Troopa — All Environments (Đi bộ khung 1) |
| **#151** | `(37, 352)` | `14 × 16 px` | Red Koopa Troopa — All Environments (Mai rùa đang trượt / Kicked Shell khung 1) |
| **#152** | `(55, 352)` | `14 × 16 px` | Red Koopa Troopa — All Environments (Mai rùa đang trượt / Kicked Shell khung 2) |
| **#153** | `(90, 352)` | `16 × 16 px` | Red Koopa Troopa — All Environments (Mai rùa thụt đầu / Shell idle khung 2) |
| **#154** | `(72, 353)` | `16 × 15 px` | Red Koopa Troopa — All Environments (Mai rùa thụt đầu / Shell idle khung 1) |

---

## Hàng 16 — Red Koopa lật ngửa & Red Piranha Plant (y ≈ 370–386) — "Red Enemies"

| STT | Tọa độ (X, Y) | Kích thước (W × H) | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|----------------|---------------------|---------------------------------------|
| **#155** | `(0, 370)` | `16 × 16 px` | Red Koopa Troopa — All Environments (Mai rùa lật ngửa khung 1) |
| **#156** | `(18, 370)` | `15 × 16 px` | Red Koopa Troopa — All Environments (Mai rùa lật ngửa khung 2) |
| **#157** | `(91, 370)` | `14 × 16 px` | Piranha Plant — Overworld (Há miệng, palette đỏ) |

---

## Tổng kết theo loại quái vật

| Loại quái vật | Số palette | Số sprite | STT |
|---------------|-----------|-----------|-----|
| Goomba | 4 (OW/UG/CT/UW) | 12 | #1–#12 |
| Buzzy Beetle | 4 | 12 | #13–#24 |
| Blooper | 4 | 8 | #25–#32 |
| Bullet Bill | 4 | 4 | #33–#36 |
| Firebar | 4 | 16 | #37–#52 |
| Spiny (Green) | 3 (OW/UG+CT/UW) | 18 | #53–#70 |
| Koopa Troopa (Green) | 3 | 12 | #71–#82 |
| Koopa Troopa lật ngửa | 3 | 6 | #83–#88 |
| Koopa Paratroopa | 3 | 12 | #89–#100 |
| Cheep Cheep | 3 | 12 | #101–#112 |
| Bowser | 3 | 12 | #113–#124 |
| Hammer Bros + Hammer | 3 | 12 | #125–#136 |
| Lakitu | 2 (OW/UG+CT) | 4 | #137–#138, #141–#142 |
| Piranha Plant (UG+CT) | 1 | 2 | #139–#140 |
| Red Spiny | 1 (All) | 6 | #143–#148 |
| Red Koopa Troopa | 1 (All) | 8 | #149–#156 |
| Piranha Plant (OW) | 1 | 1 | #157 |
| **Tổng** | — | **157** | — |

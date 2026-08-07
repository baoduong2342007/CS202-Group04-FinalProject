# Bảng Tọa Độ & Ý Nghĩa Chi Tiết Khung Hình Quái Vật (`enemies_coordinate.md`)

Danh sách toàn bộ **157** thành phần liên thông quái vật tách từ tệp `enemies.png` bằng thuật toán **Loang (Flood Fill)** trên khu vực mảng lớn nhất ($Y \in [0, 392]$):

> **Giải thích hai cột tọa độ & kích thước:**
> - **Tọa độ Loang (X, Y) & Kích thước Loang (W × H)**: Bounding box thực tế ôm sát từng điểm ảnh (pixel) của quái vật do thuật toán loang (connected component) tìm thấy.
> - **Tọa độ Ô Grid (X, Y) & Kích thước Grid (W × H)**: Khung ô chuẩn trên Spritesheet dùng cho game engine (`sf::IntRect`).

> **Quy ước palette (bảng màu NES):**
> - **Overworld (OW)**: Xanh lá `(12,147,0)` + Cam `(230,156,33)` — dùng cho Level 1-1, 1-3, 3-1, v.v.
> - **Underground (UG)**: Teal `(0,123,140)` + Nâu `(156,74,0)` + Hồng nhạt `(255,206,197)` — dùng cho Level 1-2, 4-2, v.v.
> - **Castle (CT)**: Xám `(173,173,173)` + Cam `(230,156,33)` — dùng cho Level 1-4, 2-4, v.v.
> - **Underwater (UW)**: Teal nhạt `(181,239,239)` — dùng cho Level 2-2, 7-2, v.v.
> - **Red / All Environments**: Đỏ `(181,49,33)` + Cam `(230,156,33)` — một số quái chỉ có 1 palette dùng chung mọi màn.

---

## Hàng 1 — Goomba (y ≈ 16–32)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang (W × H) | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|-----------------------|--------------------------|----------------|-----------------|----------|---------------------------------------|
 | **#1** | `(0, 16)` | `16 × 16 px` | `(0, 16)` | `16 × 16 px` | 180 px | Goomba — Overworld (Dạng đi bộ bình thường — bước lật đật chân trái) | 
 | **#2** | `(18, 16)` | `16 × 16 px` | `(18, 16)` | `16 × 16 px` | 180 px | Goomba — Overworld (Dạng đi bộ bình thường — bước lật đật chân phải) | 
 | **#3** | `(74, 16)` | `16 × 16 px` | `(74, 16)` | `16 × 16 px` | 180 px | Goomba — Underground (Dạng đi bộ bình thường — bước lật đật chân trái) | 
 | **#4** | `(92, 16)` | `16 × 16 px` | `(92, 16)` | `16 × 16 px` | 180 px | Goomba — Underground (Dạng đi bộ bình thường — bước lật đật chân phải) | 
 | **#5** | `(148, 16)` | `16 × 16 px` | `(148, 16)` | `16 × 16 px` | 136 px | Goomba — Castle (Dạng đi bộ bình thường — bước lật đật chân trái) | 
 | **#6** | `(166, 16)` | `16 × 16 px` | `(166, 16)` | `16 × 16 px` | 136 px | Goomba — Castle (Dạng đi bộ bình thường — bước lật đật chân phải) | 
 | **#7** | `(222, 16)` | `16 × 16 px` | `(222, 16)` | `16 × 16 px` | 136 px | Goomba — Underwater (Dạng đi bộ bình thường — bước lật đật chân trái) | 
 | **#8** | `(240, 16)` | `16 × 16 px` | `(240, 16)` | `16 × 16 px` | 136 px | Goomba — Underwater (Dạng đi bộ bình thường — bước lật đật chân phải) | 

## Hàng 2 — Goomba bị dẹt (y ≈ 24)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang (W × H) | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|-----------------------|--------------------------|----------------|-----------------|----------|---------------------------------------|
 | **#9** | `(36, 24)` | `16 × 8 px` | `(36, 24)` | `16 × 8 px` | 88 px | Goomba — Overworld (Trạng thái khi bị Mario nhảy lên giẫm bẹp) | 
 | **#10** | `(110, 24)` | `16 × 8 px` | `(110, 24)` | `16 × 8 px` | 88 px | Goomba — Underground (Trạng thái khi bị Mario nhảy lên giẫm bẹp) | 
 | **#11** | `(184, 24)` | `16 × 8 px` | `(184, 24)` | `16 × 8 px` | 62 px | Goomba — Castle (Trạng thái khi bị Mario nhảy lên giẫm bẹp) | 
 | **#12** | `(258, 24)` | `16 × 8 px` | `(258, 24)` | `16 × 8 px` | 62 px | Goomba — Underwater (Trạng thái khi bị Mario nhảy lên giẫm bẹp) | 

## Hàng 3 — Buzzy Beetle (y ≈ 34–50)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang (W × H) | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|-----------------------|--------------------------|----------------|-----------------|----------|---------------------------------------|
 | **#13** | `(18, 34)` | `16 × 16 px` | `(18, 34)` | `16 × 16 px` | 187 px | Buzzy Beetle — Overworld (Dạng bò bình thường — khung 2) | 
 | **#14** | `(92, 34)` | `16 × 16 px` | `(92, 34)` | `16 × 16 px` | 187 px | Buzzy Beetle — Underground (Dạng bò bình thường — khung 2) | 
 | **#15** | `(166, 34)` | `16 × 16 px` | `(166, 34)` | `16 × 16 px` | 170 px | Buzzy Beetle — Castle (Dạng bò bình thường — khung 2) | 
 | **#16** | `(240, 34)` | `16 × 16 px` | `(240, 34)` | `16 × 16 px` | 170 px | Buzzy Beetle — Underwater (Dạng bò bình thường — khung 2) | 
 | **#17** | `(0, 35)` | `16 × 15 px` | `(0, 35)` | `16 × 15 px` | 188 px | Buzzy Beetle — Overworld (Dạng bò bình thường — khung 1) | 
 | **#18** | `(36, 35)` | `16 × 15 px` | `(36, 35)` | `16 × 15 px` | 170 px | Buzzy Beetle — Overworld (Trạng thái vỏ bọ khi bị giẫm — thụt đầu & chân) | 
 | **#19** | `(74, 35)` | `16 × 15 px` | `(74, 35)` | `16 × 15 px` | 188 px | Buzzy Beetle — Underground (Dạng bò bình thường — khung 1) | 
 | **#20** | `(110, 35)` | `16 × 15 px` | `(110, 35)` | `16 × 15 px` | 170 px | Buzzy Beetle — Underground (Trạng thái vỏ bọ khi bị giẫm — thụt đầu & chân) | 
 | **#21** | `(148, 35)` | `16 × 15 px` | `(148, 35)` | `16 × 15 px` | 171 px | Buzzy Beetle — Castle (Dạng bò bình thường — khung 1) | 
 | **#22** | `(184, 35)` | `16 × 15 px` | `(184, 35)` | `16 × 15 px` | 168 px | Buzzy Beetle — Castle (Trạng thái vỏ bọ khi bị giẫm — thụt đầu & chân) | 
 | **#23** | `(222, 35)` | `16 × 15 px` | `(222, 35)` | `16 × 15 px` | 171 px | Buzzy Beetle — Underwater (Dạng bò bình thường — khung 1) | 
 | **#24** | `(258, 35)` | `16 × 15 px` | `(258, 35)` | `16 × 15 px` | 168 px | Buzzy Beetle — Underwater (Trạng thái vỏ bọ khi bị giẫm — thụt đầu & chân) | 

## Hàng 4 — Blooper & Bullet Bill (y ≈ 52–67)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang (W × H) | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|-----------------------|--------------------------|----------------|-----------------|----------|---------------------------------------|
 | **#25** | `(0, 52)` | `16 × 24 px` | `(0, 52)` | `16 × 24 px` | 218 px | Blooper — Overworld (Trạng thái duỗi thẳng thân và xúc xắc — chìm xuống/thả trôi) | 
 | **#26** | `(18, 52)` | `16 × 16 px` | `(18, 52)` | `16 × 16 px` | 172 px | Blooper — Overworld (Trạng thái co thân và xòe xúc xắc ra hai bên — búng vọt lên) | 
 | **#27** | `(74, 52)` | `16 × 24 px` | `(74, 52)` | `16 × 24 px` | 218 px | Blooper — Underground (Trạng thái duỗi thẳng thân và xúc xắc — chìm xuống/thả trôi) | 
 | **#28** | `(92, 52)` | `16 × 16 px` | `(92, 52)` | `16 × 16 px` | 172 px | Blooper — Underground (Trạng thái co thân và xòe xúc xắc ra hai bên — búng vọt lên) | 
 | **#29** | `(150, 52)` | `12 × 22 px` | `(148, 52)` | `16 × 24 px` | 64 px | Blooper — Castle (Trạng thái duỗi thẳng thân và xúc xắc — chìm xuống/thả trôi) | 
 | **#30** | `(166, 52)` | `16 × 14 px` | `(166, 52)` | `16 × 16 px` | 36 px | Blooper — Castle (Trạng thái co thân và xòe xúc xắc ra hai bên — búng vọt lên) | 
 | **#31** | `(224, 52)` | `12 × 22 px` | `(222, 52)` | `16 × 24 px` | 64 px | Blooper — Underwater (Trạng thái duỗi thẳng thân và xúc xắc — chìm xuống/thả trôi) | 
 | **#32** | `(240, 52)` | `16 × 14 px` | `(240, 52)` | `16 × 16 px` | 36 px | Blooper — Underwater (Trạng thái co thân và xòe xúc xắc ra hai bên — búng vọt lên) | 
 | **#33** | `(54, 53)` | `16 × 14 px` | `(54, 53)` | `16 × 14 px` | 188 px | Bullet Bill — Overworld (Tên đạn bắn ra từ pháo) | 
 | **#34** | `(128, 53)` | `16 × 14 px` | `(128, 53)` | `16 × 14 px` | 188 px | Bullet Bill — Underground (Tên đạn bắn ra từ pháo) | 
 | **#35** | `(202, 53)` | `16 × 14 px` | `(202, 53)` | `16 × 14 px` | 156 px | Bullet Bill — Castle (Tên đạn bắn ra từ pháo) | 
 | **#36** | `(276, 53)` | `16 × 14 px` | `(276, 53)` | `16 × 14 px` | 156 px | Bullet Bill — Underwater (Tên đạn bắn ra từ pháo) | 

## Hàng 5 — Firebar / Bọt lửa (y ≈ 78–90)

| STT | Tọa độ Loang (X, Y) | Kích thước Loang (W × H) | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|-----------------------|--------------------------|----------------|-----------------|----------|---------------------------------------|
| **#37** | `(0, 78)` | `16 × 16 px` | `(0, 78)` | `16 × 16 px` | ~ | Buzzy Beetle — Overworld (Leo dây / tường) |
| **#38** | `(36, 78)` | `16 × 16 px` | `(36, 78)` | `16 × 16 px` | ~ | Buzzy Beetle — Overworld (Leo dây / tường - ngược) |
| **#39** | `(74, 78)` | `16 × 16 px` | `(74, 78)` | `16 × 16 px` | ~ | Buzzy Beetle — Underground (Leo dây / tường) |
| **#40** | `(110, 78)` | `16 × 16 px` | `(110, 78)` | `16 × 16 px` | ~ | Buzzy Beetle — Underground (Leo dây / tường - ngược) |
| **#41** | `(148, 78)` | `16 × 16 px` | `(148, 78)` | `16 × 16 px` | ~ | Buzzy Beetle — Castle (Leo dây / tường) |
| **#42** | `(184, 78)` | `16 × 16 px` | `(184, 78)` | `16 × 16 px` | ~ | Buzzy Beetle — Castle (Leo dây / tường - ngược) |
| **#43** | `(222, 78)` | `16 × 16 px` | `(222, 78)` | `16 × 16 px` | ~ | Buzzy Beetle — Underwater (Leo dây / tường) |
| **#44** | `(258, 78)` | `16 × 16 px` | `(258, 78)` | `16 × 16 px` | ~ | Buzzy Beetle — Underwater (Leo dây / tường - ngược) |
| **#45** | `(18, 78)` | `16 × 16 px` | `(18, 78)` | `16 × 16 px` | ~ | Buzzy Beetle — Overworld (Bò trần nhà) |
| **#46** | `(54, 78)` | `16 × 16 px` | `(54, 78)` | `16 × 16 px` | ~ | Buzzy Beetle — Overworld (Bò trần nhà - ngược) |
| **#47** | `(92, 78)` | `16 × 16 px` | `(92, 78)` | `16 × 16 px` | ~ | Buzzy Beetle — Underground (Bò trần nhà) |
| **#48** | `(128, 78)` | `16 × 16 px` | `(128, 78)` | `16 × 16 px` | ~ | Buzzy Beetle — Underground (Bò trần nhà - ngược) |
| **#49** | `(166, 78)` | `16 × 16 px` | `(166, 78)` | `16 × 16 px` | ~ | Buzzy Beetle — Castle (Bò trần nhà) |
| **#50** | `(202, 78)` | `16 × 16 px` | `(202, 78)` | `16 × 16 px` | ~ | Buzzy Beetle — Castle (Bò trần nhà - ngược) |
| **#51** | `(240, 78)` | `16 × 16 px` | `(240, 78)` | `16 × 16 px` | ~ | Buzzy Beetle — Underwater (Bò trần nhà) |
| **#52** | `(276, 78)` | `16 × 16 px` | `(276, 78)` | `16 × 16 px` | ~ | Buzzy Beetle — Underwater (Bò trần nhà - ngược) |

## Hàng 6 — Spiny / Rùa gai (y ≈ 112–136) — "Green Enemies" section

| STT | Tọa độ Loang (X, Y) | Kích thước Loang (W × H) | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|-----------------------|--------------------------|----------------|-----------------|----------|---------------------------------------|
 | **#53** | `(18, 114)` | `15 × 22 px` | `(18, 112)` | `16 × 24 px` | 162 px | Koopa Troopa — Overworld (Dạng đi bộ bình thường — khung 2) | 
 | **#54** | `(54, 114)` | `15 × 22 px` | `(54, 112)` | `16 × 24 px` | 147 px | Koopa Paratroopa — Overworld (Dạng có cánh bay lên xuống / nhảy — khung 2) | 
 | **#55** | `(164, 112)` | `16 × 24 px` | `(164, 112)` | `16 × 24 px` | 208 px | Koopa Troopa — Underground & Castle (Dạng đi bộ bình thường — khung 2) | 
 | **#56** | `(200, 112)` | `16 × 24 px` | `(200, 112)` | `16 × 24 px` | 229 px | Koopa Paratroopa — Underground & Castle (Dạng có cánh bay — khung 2) | 
 | **#57** | `(310, 114)` | `15 × 22 px` | `(310, 112)` | `16 × 24 px` | 162 px | Koopa Troopa — Underwater (Dạng đi bộ bình thường — khung 2) | 
 | **#58** | `(346, 114)` | `15 × 22 px` | `(346, 112)` | `16 × 24 px` | 147 px | Koopa Paratroopa — Underwater (Dạng có cánh bay — khung 2) | 
 | **#59** | `(0, 115)` | `16 × 21 px` | `(0, 113)` | `16 × 23 px` | 169 px | Koopa Troopa — Overworld (Dạng đi bộ bình thường — khung 1) | 
 | **#60** | `(36, 115)` | `16 × 21 px` | `(36, 113)` | `16 × 23 px` | 166 px | Koopa Paratroopa — Overworld (Dạng có cánh bay lên xuống / nhảy — khung 1) | 
 | **#61** | `(146, 113)` | `16 × 23 px` | `(146, 113)` | `16 × 23 px` | 213 px | Koopa Troopa — Underground & Castle (Dạng đi bộ bình thường — khung 1) | 
 | **#62** | `(182, 113)` | `16 × 23 px` | `(182, 113)` | `16 × 23 px` | 261 px | Koopa Paratroopa — Underground & Castle (Dạng có cánh bay — khung 1) | 
 | **#63** | `(292, 115)` | `16 × 21 px` | `(292, 113)` | `16 × 23 px` | 169 px | Koopa Troopa — Underwater (Dạng đi bộ bình thường — khung 1) | 
 | **#64** | `(328, 115)` | `16 × 21 px` | `(328, 113)` | `16 × 23 px` | 166 px | Koopa Paratroopa — Underwater (Dạng có cánh bay — khung 1) | 
 | **#65** | `(73, 120)` | `14 × 12 px` | `(72, 120)` | `16 × 14 px` | 114 px | Mai Rùa — Overworld (Trạng thái chiếc mai khi bị giẫm — thụt vào mai) | 
 | **#66** | `(91, 120)` | `14 × 15 px` | `(90, 120)` | `16 × 15 px` | 134 px | Mai Rùa — Overworld (Trạng thái chiếc mai bắt đầu thò chân chuẩn bị tỉnh dậy) | 
 | **#67** | `(218, 120)` | `16 × 14 px` | `(218, 120)` | `16 × 14 px` | 146 px | Mai Rùa — Underground & Castle (Trạng thái chiếc mai khi bị giẫm — thụt vào mai) | 
 | **#68** | `(236, 120)` | `16 × 15 px` | `(236, 120)` | `16 × 15 px` | 166 px | Mai Rùa — Underground & Castle (Trạng thái chiếc mai bắt đầu thò chân chuẩn bị tỉnh dậy) | 
 | **#69** | `(365, 120)` | `14 × 12 px` | `(364, 120)` | `16 × 14 px` | 114 px | Mai Rùa — Underwater (Trạng thái chiếc mai khi bị giẫm — thụt vào mai) | 
 | **#70** | `(383, 120)` | `14 × 15 px` | `(382, 120)` | `16 × 15 px` | 134 px | Mai Rùa — Underwater (Trạng thái chiếc mai bắt đầu thò chân chuẩn bị tỉnh dậy) | 

## Hàng 7 — Koopa Troopa (y ≈ 138–162) — "Green Enemies" section

| STT | Tọa độ Loang (X, Y) | Kích thước Loang (W × H) | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|-----------------------|--------------------------|----------------|-----------------|----------|---------------------------------------|
 | **#71** | `(18, 138)` | `16 × 24 px` | `(18, 138)` | `16 × 24 px` | 192 px | Piranha Plant — Overworld (Trạng thái khép / ngậm miệng lại) | 
 | **#72** | `(54, 138)` | `16 × 24 px` | `(54, 138)` | `16 × 24 px` | 145 px | Lakitu — Overworld (Trạng thái nhô đầu lên khỏi mây quan sát & chuẩn bị ném Spiny) | 
 | **#73** | `(164, 138)` | `16 × 24 px` | `(164, 138)` | `16 × 24 px` | 192 px | Piranha Plant — Underground & Castle (Trạng thái khép / ngậm miệng lại) | 
 | **#74** | `(200, 138)` | `16 × 24 px` | `(200, 138)` | `16 × 24 px` | 302 px | Lakitu — Underground & Castle (Trạng thái nhô đầu lên khỏi mây quan sát & chuẩn bị ném Spiny) | 
 | **#75** | `(310, 138)` | `16 × 24 px` | `(310, 138)` | `16 × 24 px` | 192 px | Piranha Plant — Underwater (Trạng thái khép / ngậm miệng lại) | 
 | **#76** | `(346, 138)` | `16 × 24 px` | `(346, 138)` | `16 × 24 px` | 145 px | Lakitu — Underwater (Trạng thái nhô đầu lên khỏi mây quan sát & chuẩn bị ném Spiny) | 
 | **#77** | `(0, 139)` | `16 × 23 px` | `(0, 139)` | `16 × 23 px` | 202 px | Piranha Plant — Overworld (Trạng thái há rộng miệng để lộ răng đớp) | 
 | **#78** | `(146, 139)` | `16 × 23 px` | `(146, 139)` | `16 × 23 px` | 224 px | Piranha Plant — Underground & Castle (Trạng thái há rộng miệng để lộ răng đớp) | 
 | **#79** | `(292, 139)` | `16 × 23 px` | `(292, 139)` | `16 × 23 px` | 202 px | Piranha Plant — Underwater (Trạng thái há rộng miệng để lộ răng đớp) | 
 | **#80** | `(72, 146)` | `16 × 16 px` | `(72, 146)` | `16 × 16 px` | 82 px | Lakitu — Overworld (Trạng thái thụt đầu ẩn vào mây / Đám mây trống) | 
 | **#81** | `(218, 146)` | `16 × 16 px` | `(218, 146)` | `16 × 16 px` | 214 px | Lakitu — Underground & Castle (Trạng thái thụt đầu ẩn vào mây / Đám mây trống) | 
 | **#82** | `(364, 146)` | `16 × 16 px` | `(364, 146)` | `16 × 16 px` | 82 px | Lakitu — Underwater (Trạng thái thụt đầu ẩn vào mây / Đám mây trống) | 

## Hàng 8 — Cheep Cheep Green / Cá bay xanh (y ≈ 164–180) — "Green Enemies" section

| STT | Tọa độ Loang (X, Y) | Kích thước Loang (W × H) | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|-----------------------|--------------------------|----------------|-----------------|----------|---------------------------------------|
 | **#83** | `(0, 164)` | `16 × 16 px` | `(0, 164)` | `16 × 16 px` | 110 px | Cheep Cheep (Green) — Overworld (Trạng thái giơ vây/cánh hướng lên trên) | 
 | **#84** | `(18, 164)` | `15 × 15 px` | `(18, 164)` | `15 × 16 px` | 107 px | Cheep Cheep (Green) — Overworld (Trạng thái quạt vây/cánh hướng xuống dưới) | 
 | **#85** | `(146, 164)` | `16 × 16 px` | `(146, 164)` | `16 × 16 px` | 180 px | Cheep Cheep (Green) — Underground & Castle (Trạng thái giơ vây/cánh hướng lên trên) | 
 | **#86** | `(164, 164)` | `15 × 16 px` | `(164, 164)` | `15 × 16 px` | 177 px | Cheep Cheep (Green) — Underground & Castle (Trạng thái quạt vây/cánh hướng xuống dưới) | 
 | **#87** | `(292, 164)` | `16 × 16 px` | `(292, 164)` | `16 × 16 px` | 110 px | Cheep Cheep (Green) — Underwater (Trạng thái giơ vây/cánh hướng lên trên) | 
 | **#88** | `(310, 164)` | `15 × 15 px` | `(310, 164)` | `15 × 16 px` | 107 px | Cheep Cheep (Green) — Underwater (Trạng thái quạt vây/cánh hướng xuống dưới) | 

## Hàng 9 — Koopa Paratroopa / Rùa cánh (y ≈ 172–206) — "Green Enemies" section

| STT | Tọa độ Loang (X, Y) | Kích thước Loang (W × H) | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|-----------------------|--------------------------|----------------|-----------------|----------|---------------------------------------|
| **#89** | `(73, 172)` | `15 × 34 px` | `(73, 172)` | `15 × 34 px` | 242 px | Hammer Bro — Overworld (Lúc ném khung 1) |
| **#90** | `(91, 172)` | `15 × 34 px` | `(91, 172)` | `15 × 34 px` | 227 px | Hammer Bro — Overworld (Lúc ném khung 2) |
| **#91** | `(108, 172)` | `15 × 34 px` | `(108, 172)` | `15 × 34 px` | 239 px | Hammer Bro — Overworld (Lúc ném khung 3) |
| **#92** | `(126, 172)` | `15 × 34 px` | `(126, 172)` | `15 × 34 px` | 224 px | Hammer Bro — Overworld (Lúc ném khung 4) |
| **#93** | `(219, 172)` | `15 × 34 px` | `(219, 172)` | `15 × 34 px` | 277 px | Hammer Bro — Underground+Castle (Lúc ném khung 1) |
| **#94** | `(237, 172)` | `15 × 34 px` | `(237, 172)` | `15 × 34 px` | 262 px | Hammer Bro — Underground+Castle (Lúc ném khung 2) |
| **#95** | `(254, 172)` | `15 × 34 px` | `(254, 172)` | `15 × 34 px` | 274 px | Hammer Bro — Underground+Castle (Lúc ném khung 3) |
| **#96** | `(272, 172)` | `15 × 34 px` | `(272, 172)` | `15 × 34 px` | 259 px | Hammer Bro — Underground+Castle (Lúc ném khung 4) |
| **#97** | `(365, 173)` | `15 × 33 px` | `(365, 172)` | `15 × 34 px` | 230 px | Hammer Bro — Underwater (Lúc ném khung 1) |
| **#98** | `(383, 173)` | `15 × 33 px` | `(383, 172)` | `15 × 34 px` | 215 px | Hammer Bro — Underwater (Lúc ném khung 2) |
| **#99** | `(400, 173)` | `15 × 33 px` | `(400, 172)` | `15 × 34 px` | 230 px | Hammer Bro — Underwater (Lúc ném khung 3) |
| **#100** | `(418, 173)` | `15 × 33 px` | `(418, 172)` | `15 × 34 px` | 215 px | Hammer Bro — Underwater (Lúc ném khung 4) |

## Hàng 10 — Cheep Cheep / Cá chép bay (y ≈ 182–206) — "Green Enemies" section

| STT | Tọa độ Loang (X, Y) | Kích thước Loang (W × H) | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|-----------------------|--------------------------|----------------|-----------------|----------|---------------------------------------|
| **#101** | `(0, 182)` | `16 × 24 px` | `(0, 182)` | `16 × 24 px` | 191 px | Hammer Bro — Overworld (Chưa ném / Đi bộ khung 1) |
| **#102** | `(18, 182)` | `16 × 24 px` | `(18, 182)` | `16 × 24 px` | 172 px | Hammer Bro — Overworld (Chưa ném / Đi bộ khung 2) |
| **#103** | `(37, 182)` | `15 × 24 px` | `(37, 182)` | `15 × 24 px` | 192 px | Hammer Bro — Overworld (Chưa ném / Đi bộ khung 3) |
| **#104** | `(55, 182)` | `15 × 24 px` | `(55, 182)` | `15 × 24 px` | 177 px | Hammer Bro — Overworld (Chưa ném / Đi bộ khung 4) |
| **#105** | `(146, 182)` | `16 × 24 px` | `(146, 182)` | `16 × 24 px` | 238 px | Hammer Bro — Underground+Castle (Chưa ném / Đi bộ khung 1) |
| **#106** | `(164, 182)` | `16 × 24 px` | `(164, 182)` | `16 × 24 px` | 215 px | Hammer Bro — Underground+Castle (Chưa ném / Đi bộ khung 2) |
| **#107** | `(183, 182)` | `15 × 24 px` | `(183, 182)` | `15 × 24 px` | 227 px | Hammer Bro — Underground+Castle (Chưa ném / Đi bộ khung 3) |
| **#108** | `(201, 182)` | `15 × 24 px` | `(201, 182)` | `15 × 24 px` | 212 px | Hammer Bro — Underground+Castle (Chưa ném / Đi bộ khung 4) |
| **#109** | `(292, 182)` | `16 × 24 px` | `(292, 182)` | `16 × 24 px` | 191 px | Hammer Bro — Underwater (Chưa ném / Đi bộ khung 1) |
| **#110** | `(310, 182)` | `16 × 24 px` | `(310, 182)` | `16 × 24 px` | 172 px | Hammer Bro — Underwater (Chưa ném / Đi bộ khung 2) |
| **#111** | `(329, 182)` | `15 × 24 px` | `(329, 182)` | `15 × 24 px` | 192 px | Hammer Bro — Underwater (Chưa ném / Đi bộ khung 3) |
| **#112** | `(347, 182)` | `15 × 24 px` | `(347, 182)` | `15 × 24 px` | 177 px | Hammer Bro — Underwater (Chưa ném / Đi bộ khung 4) |

## Hàng 11 — Bowser / Trùm rùa (y ≈ 208–240) — 3 palette × 4 khung = 12 sprite

| STT | Tọa độ Loang (X, Y) | Kích thước Loang (W × H) | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|-----------------------|--------------------------|----------------|-----------------|----------|---------------------------------------|
| **#113** | `(0, 209)` | `31 × 31 px` | `(0, 208)` | `32 × 32 px` | 371 px | Bowser — Overworld (Đi bộ khung 1 — khép miệng) |
| **#114** | `(34, 209)` | `32 × 31 px` | `(34, 208)` | `32 × 32 px` | 370 px | Bowser — Overworld (Đi bộ khung 2 — há miệng) |
| **#115** | `(68, 209)` | `31 × 31 px` | `(68, 208)` | `32 × 32 px` | 387 px | Bowser — Overworld (Khè lửa khung 1) |
| **#116** | `(102, 209)` | `32 × 31 px` | `(102, 208)` | `32 × 32 px` | 386 px | Bowser — Overworld (Khè lửa khung 2) |
| **#117** | `(146, 208)` | `32 × 32 px` | `(146, 208)` | `32 × 32 px` | 520 px | Bowser — Underground+Castle (Đi bộ khung 1) |
| **#118** | `(180, 208)` | `32 × 32 px` | `(180, 208)` | `32 × 32 px` | 518 px | Bowser — Underground+Castle (Đi bộ khung 2) |
| **#119** | `(214, 208)` | `32 × 32 px` | `(214, 208)` | `32 × 32 px` | 531 px | Bowser — Underground+Castle (Khè lửa khung 1) |
| **#120** | `(248, 208)` | `32 × 32 px` | `(248, 208)` | `32 × 32 px` | 529 px | Bowser — Underground+Castle (Khè lửa khung 2) |
| **#121** | `(292, 209)` | `31 × 31 px` | `(292, 208)` | `32 × 32 px` | 371 px | Bowser — Underwater (Đi bộ khung 1) |
| **#122** | `(326, 209)` | `32 × 31 px` | `(326, 208)` | `32 × 32 px` | 370 px | Bowser — Underwater (Đi bộ khung 2) |
| **#123** | `(360, 209)` | `31 × 31 px` | `(360, 208)` | `32 × 32 px` | 387 px | Bowser — Underwater (Khè lửa khung 1) |
| **#124** | `(394, 209)` | `32 × 31 px` | `(394, 208)` | `32 × 32 px` | 386 px | Bowser — Underwater (Khè lửa khung 2) |

## Hàng 12 — Bowser giơ búa & Luồng lửa (y ≈ 242–268) — 12 sprite

| STT | Tọa độ Loang (X, Y) | Kích thước Loang (W × H) | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|-----------------------|--------------------------|----------------|-----------------|----------|---------------------------------------|
| **#125** | `(34, 242)` | `29 × 26 px` | `(34, 242)` | `30 × 26 px` | 226 px | Bowser — Overworld (Há miệng gầm/phun lửa, giơ búa màu đen trên đầu, quay sang trái) |
| **#126** | `(71, 242)` | `29 × 26 px` | `(70, 242)` | `30 × 26 px` | 228 px | Bowser — Overworld (Há miệng gầm/phun lửa, giơ búa màu đen trên đầu, quay sang phải) |
| **#127** | `(102, 242)` | `24 × 8 px` | `(102, 242)` | `24 × 8 px` | 125 px | Luồng lửa (Fireball) do Bowser phun ra — Overworld (Khung 1) |
| **#128** | `(180, 242)` | `30 × 26 px` | `(180, 242)` | `30 × 26 px` | 301 px | Bowser — Underground+Castle (Há miệng gầm/phun lửa, giơ búa màu xám trên đầu, quay sang trái) |
| **#129** | `(216, 242)` | `30 × 26 px` | `(216, 242)` | `30 × 26 px` | 303 px | Bowser — Underground+Castle (Há miệng gầm/phun lửa, giơ búa màu xám trên đầu, quay sang phải) |
| **#130** | `(248, 242)` | `24 × 8 px` | `(248, 242)` | `24 × 8 px` | 125 px | Luồng lửa (Fireball) do Bowser phun ra — Underground+Castle (Khung 1) |
| **#131** | `(326, 243)` | `29 × 25 px` | `(326, 242)` | `30 × 26 px` | 216 px | Bowser — Underwater (Há miệng gầm/phun lửa, giơ búa trên đầu, quay sang trái) |
| **#132** | `(363, 243)` | `29 × 25 px` | `(362, 242)` | `30 × 26 px` | 216 px | Bowser — Underwater (Há miệng gầm/phun lửa, giơ búa trên đầu, quay sang phải) |
| **#133** | `(394, 242)` | `24 × 8 px` | `(394, 242)` | `24 × 8 px` | 125 px | Luồng lửa (Fireball) do Bowser phun ra — Underwater (Khung 1) |
| **#134** | `(102, 252)` | `24 × 8 px` | `(102, 252)` | `24 × 8 px` | 125 px | Luồng lửa (Fireball) do Bowser phun ra — Overworld (Khung 2) |
| **#135** | `(248, 252)` | `24 × 8 px` | `(248, 252)` | `24 × 8 px` | 125 px | Luồng lửa (Fireball) do Bowser phun ra — Underground+Castle (Khung 2) |
| **#136** | `(394, 252)` | `24 × 8 px` | `(394, 252)` | `24 × 8 px` | 125 px | Luồng lửa (Fireball) do Bowser phun ra — Underwater (Khung 2) |

## Hàng 13 — Hammer Bros (giơ búa lên cao) & Piranha Plant (y ≈ 276–302) — 6 sprite

| STT | Tọa độ Loang (X, Y) | Kích thước Loang (W × H) | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|-----------------------|--------------------------|----------------|-----------------|----------|---------------------------------------|
| **#137** | `(34, 277)` | `30 × 25 px` | `(34, 276)` | `30 × 26 px` | 291 px | Hammer Bro — Trạng thái cầm búa giơ lên cao chuẩn bị ném, hướng về phía bên trái (Khung 1) |
| **#138** | `(70, 277)` | `30 × 25 px` | `(70, 276)` | `30 × 26 px` | 291 px | Hammer Bro — Trạng thái cầm búa giơ lên cao chuẩn bị ném, hướng về phía bên trái (Khung 2) |
| **#139** | `(145, 277)` | `15 × 25 px` | `(145, 276)` | `15 × 26 px` | 189 px | Hammer Bro — Trạng thái ném gì đó (Khung 1) |
| **#140** | `(162, 277)` | `15 × 25 px` | `(162, 276)` | `15 × 26 px` | 189 px | Hammer Bro — Trạng thái ném gì đó (Khung 2) |
| **#141** | `(180, 277)` | `30 × 25 px` | `(180, 276)` | `30 × 26 px` | 291 px | Hammer Bro — Trạng thái cầm búa giơ lên cao chuẩn bị ném, hướng về phía bên phải (Khung 1) |
| **#142** | `(216, 277)` | `30 × 25 px` | `(216, 276)` | `30 × 26 px` | 291 px | Hammer Bro — Trạng thái cầm búa giơ lên cao chuẩn bị ném, hướng về phía bên phải (Khung 2) |

## Hàng 14 — Red Koopa Troopa, Paratroopa & Red Shell (y ≈ 318–340) — "Red Enemies"

| STT | Tọa độ Loang (X, Y) | Kích thước Loang (W × H) | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|-----------------------|--------------------------|----------------|-----------------|----------|---------------------------------------|
 | **#143** | `(18, 320)` | `15 × 22 px` | `(18, 318)` | `16 × 24 px` | 162 px | Red Koopa Troopa — Dạng đi bộ bình thường (luân phiên đổi chân — khung 2) | 
 | **#144** | `(54, 320)` | `15 × 22 px` | `(54, 318)` | `16 × 24 px` | 147 px | Red Koopa Paratroopa — Dạng có cánh vẫy cánh bay (khung 2) | 
 | **#145** | `(0, 321)` | `16 × 21 px` | `(0, 319)` | `16 × 23 px` | 169 px | Red Koopa Troopa — Dạng đi bộ bình thường (luân phiên đổi chân — khung 1) | 
 | **#146** | `(36, 321)` | `16 × 21 px` | `(36, 319)` | `16 × 23 px` | 166 px | Red Koopa Paratroopa — Dạng có cánh vẫy cánh bay (khung 1) | 
 | **#147** | `(73, 326)` | `14 × 12 px` | `(72, 326)` | `16 × 14 px` | 114 px | Red Mai Rùa — Trạng thái chiếc mai khi bị giẫm (thụt hoàn toàn vào mai) | 
 | **#148** | `(91, 326)` | `14 × 15 px` | `(90, 326)` | `16 × 15 px` | 134 px | Red Mai Rùa — Trạng thái chiếc mai thò chân chuẩn bị tỉnh dậy | 

## Hàng 15 — Red Piranha Plant, Red Spiny Egg & Red Spiny (y ≈ 344–368) — "Red Enemies"

| STT | Tọa độ Loang (X, Y) | Kích thước Loang (W × H) | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|-----------------------|--------------------------|----------------|-----------------|----------|---------------------------------------|
 | **#149** | `(18, 344)` | `16 × 24 px` | `(18, 344)` | `16 × 24 px` | 192 px | Red Piranha Plant — Hoa ăn thịt đỏ ở trạng thái ngậm miệng | 
 | **#150** | `(0, 345)` | `16 × 23 px` | `(0, 345)` | `16 × 23 px` | 202 px | Red Piranha Plant — Hoa ăn thịt đỏ ở trạng thái há rộng miệng lộ răng đớp | 
 | **#151** | `(37, 352)` | `14 × 16 px` | `(37, 352)` | `14 × 16 px` | 158 px | Red Spiny Egg — Quả cầu gai đỏ đang xoay tròn khi Lakitu thả (khung 1) | 
 | **#152** | `(55, 352)` | `14 × 16 px` | `(55, 352)` | `14 × 16 px` | 146 px | Red Spiny Egg — Quả cầu gai đỏ đang xoay tròn khi Lakitu thả (khung 2) | 
 | **#153** | `(90, 354)` | `16 × 14 px` | `(90, 352)` | `16 × 16 px` | 111 px | Red Spiny — Rùa gai đỏ đáp đất đang bò lật đật (khung 2) | 
 | **#154** | `(72, 355)` | `16 × 13 px` | `(72, 353)` | `16 × 15 px` | 110 px | Red Spiny — Rùa gai đỏ đáp đất đang bò lật đật (khung 1) | 

## Hàng 16 — Red Cheep Cheep & Podoboo / Lava Bubble (y ≈ 370–386) — "Red Enemies"

| STT | Tọa độ Loang (X, Y) | Kích thước Loang (W × H) | Tọa độ Ô Grid | Kích thước Grid | Số Pixel | Ý Nghĩa / Tên Quái Vật & Trạng Thái |
|-----|-----------------------|--------------------------|----------------|-----------------|----------|---------------------------------------|
 | **#155** | `(0, 370)` | `16 × 16 px` | `(0, 370)` | `16 × 16 px` | 110 px | Red Cheep Cheep — Cá đỏ dạng vẫy vây/cánh hướng lên trên (bơi/nhảy khỏi mặt nước) | 
 | **#156** | `(18, 370)` | `15 × 15 px` | `(18, 370)` | `15 × 16 px` | 107 px | Red Cheep Cheep — Cá đỏ dạng quạt vây/cánh hướng xuống dưới (bơi/nhảy khỏi mặt nước) | 
 | **#157** | `(91, 370)` | `14 × 16 px` | `(91, 370)` | `14 × 16 px` | 152 px | Podoboo / Lava Bubble — Cầu lửa dung nham phụt vọt lên từ lòng dung nham | 

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
| Bowser (giơ búa) & Luồng lửa | 3 | 12 | #125–#136 |
| Hammer Bro | 2 (OW/UG+CT) | 6 | #137–#142 |
| Piranha Plant (UG+CT) | 1 | 2 | #139–#140 |
| Red Spiny | 1 (All) | 6 | #143–#148 |
| Red Koopa Troopa | 1 (All) | 8 | #149–#156 |
| Piranha Plant (OW) | 1 | 1 | #157 |
| **Tổng** | — | **157** | — |

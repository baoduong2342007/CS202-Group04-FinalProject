# Items & Objects Coordinates and Descriptions (items_objects_coordinate.md)

> Analysis document for all 266 objects (Items, Objects, Power-ups, Score Texts, Platforms) on the reference Atlas:
> docs/assets/reference/items_objects_all_components_atlas_full.png (960x1840 px).
> The table is organized into the 4 standard Super Mario Bros NES palettes: Overworld (#1-#61), Underground (#62-#133), Castle (#134-#205), Underwater (#206-#266).
> Note: the `#` ordering in the atlas follows component order in the source image; it does not split evenly into 66 elements per environment. The coordinate-based boundaries are OW `#1-#61`, UG `#62-#133`, Castle `#134-#205`, UW `#206-#266`.
> The Score Texts repeated in UG/Castle are two palette versions: the standard score version and the peach version used for points earned from the goal flag.

---

## 1. Overworld Objects (outdoor environment — #1 to #61)

| # | Flood-fill coords (X, Y) | Flood size | Frame grid cell coords (X, Y) | Frame size | Pixel count | Palette | Meaning / frame state |
|-----|-----------------------|------------------|----------------------|------------------|----------|---------|----------------------------------|
| **#1** | `(0, 8)` | `16 x 16 px` | `(0, 8)` | `16 x 16 px` | 176 px | OW Palette | Super Mushroom (power-up; grows Mario) |
| **#2** | `(32, 8)` | `16 x 16 px` | `(32, 8)` | `16 x 16 px` | 162 px | OW Palette | Fire Flower — palette cycle group A (state #1/4) |
| **#3** | `(50, 8)` | `16 x 16 px` | `(50, 8)` | `16 x 16 px` | 162 px | OW Palette | Fire Flower — palette cycle group A (state #2/4) |
| **#4** | `(68, 8)` | `16 x 16 px` | `(68, 8)` | `16 x 16 px` | 162 px | OW Palette | Fire Flower — palette cycle group A (state #3/4) |
| **#5** | `(86, 8)` | `16 x 16 px` | `(86, 8)` | `16 x 16 px` | 162 px | OW Palette | Fire Flower — palette cycle group A (state #4/4) |
| **#6** | `(107, 8)` | `14 x 16 px` | `(106, 8)` | `16 x 16 px` | 122 px | OW Palette | Super Star — palette cycle group A (state #1/4) |
| **#7** | `(125, 8)` | `14 x 16 px` | `(124, 8)` | `16 x 16 px` | 122 px | OW Palette | Super Star — palette cycle group A (state #2/4) |
| **#8** | `(143, 8)` | `14 x 16 px` | `(142, 8)` | `16 x 16 px` | 122 px | OW Palette | Super Star — palette cycle group A (state #3/4) |
| **#9** | `(161, 8)` | `14 x 16 px` | `(160, 8)` | `16 x 16 px` | 122 px | OW Palette | Super Star — palette cycle group A (state #4/4) |
| **#10** | `(0, 26)` | `16 x 16 px` | `(0, 26)` | `16 x 16 px` | 176 px | OW Palette | 1-Up Mushroom (grants one extra life) |
| **#11** | `(32, 26)` | `16 x 16 px` | `(32, 26)` | `16 x 16 px` | 162 px | OW Palette | Fire Flower — palette cycle group B (state #1/4) |
| **#12** | `(50, 26)` | `16 x 16 px` | `(50, 26)` | `16 x 16 px` | 162 px | OW Palette | Fire Flower — palette cycle group B (state #2/4) |
| **#13** | `(68, 26)` | `16 x 16 px` | `(68, 26)` | `16 x 16 px` | 162 px | OW Palette | Fire Flower — palette cycle group B (state #3/4) |
| **#14** | `(86, 26)` | `16 x 16 px` | `(86, 26)` | `16 x 16 px` | 162 px | OW Palette | Fire Flower — palette cycle group B (state #4/4) |
| **#15** | `(107, 26)` | `14 x 16 px` | `(106, 26)` | `16 x 16 px` | 122 px | OW Palette | Super Star — palette cycle group B (state #1/4) |
| **#16** | `(125, 26)` | `14 x 16 px` | `(124, 26)` | `16 x 16 px` | 122 px | OW Palette | Super Star — palette cycle group B (state #2/4) |
| **#17** | `(143, 26)` | `14 x 16 px` | `(142, 26)` | `16 x 16 px` | 122 px | OW Palette | Super Star — palette cycle group B (state #3/4) |
| **#18** | `(161, 26)` | `14 x 16 px` | `(160, 26)` | `16 x 16 px` | 122 px | OW Palette | Super Star — palette cycle group B (state #4/4) |
| **#19** | `(180, 37)` | `8 x 14 px` | `(180, 36)` | `8 x 16 px` | 84 px | OW Palette | Spinning Coin — spinning animation — frame #1/4 |
| **#20** | `(192, 37)` | `4 x 14 px` | `(190, 36)` | `8 x 16 px` | 48 px | OW Palette | Spinning Coin — spinning animation — frame #2/4 |
| **#21** | `(204, 37)` | `1 x 14 px` | `(200, 36)` | `8 x 16 px` | 14 px | OW Palette | Spinning Coin — spinning animation — frame #3/4 |
| **#22** | `(212, 37)` | `4 x 14 px` | `(210, 36)` | `8 x 16 px` | 48 px | OW Palette | Spinning Coin — spinning animation — frame #4/4 |
| **#23** | `(235, 26)` | `11 x 8 px` | `(234, 26)` | `16 x 8 px` | 43 px | OW Palette | Score Text — 100 points (standard score palette) |
| **#24** | `(234, 36)` | `12 x 8 px` | `(234, 36)` | `16 x 8 px` | 46 px | OW Palette | Score Text — 1000 points (standard score palette) |
| **#25** | `(253, 26)` | `15 x 8 px` | `(252, 26)` | `16 x 8 px` | 58 px | OW Palette | Score Text — 200 points (standard score palette) |
| **#26** | `(252, 36)` | `16 x 8 px` | `(252, 36)` | `16 x 8 px` | 61 px | OW Palette | Score Text — 2000 points (standard score palette) |
| **#27** | `(32, 44)` | `16 x 16 px` | `(32, 44)` | `16 x 16 px` | 162 px | OW Palette | Fire Flower — palette cycle group C (state #1/4) |
| **#28** | `(50, 44)` | `16 x 16 px` | `(50, 44)` | `16 x 16 px` | 162 px | OW Palette | Fire Flower — palette cycle group C (state #2/4) |
| **#29** | `(68, 44)` | `16 x 16 px` | `(68, 44)` | `16 x 16 px` | 162 px | OW Palette | Fire Flower — palette cycle group C (state #3/4) |
| **#30** | `(86, 44)` | `16 x 16 px` | `(86, 44)` | `16 x 16 px` | 162 px | OW Palette | Fire Flower — palette cycle group C (state #4/4) |
| **#31** | `(107, 44)` | `14 x 16 px` | `(106, 44)` | `16 x 16 px` | 122 px | OW Palette | Super Star — palette cycle group C (state #1/4) |
| **#32** | `(125, 44)` | `14 x 16 px` | `(124, 44)` | `16 x 16 px` | 122 px | OW Palette | Super Star — palette cycle group C (state #2/4) |
| **#33** | `(143, 44)` | `14 x 16 px` | `(142, 44)` | `16 x 16 px` | 122 px | OW Palette | Super Star — palette cycle group C (state #3/4) |
| **#34** | `(161, 44)` | `14 x 16 px` | `(160, 44)` | `16 x 16 px` | 122 px | OW Palette | Super Star — palette cycle group C (state #4/4) |
| **#35** | `(180, 54)` | `8 x 8 px` | `(180, 54)` | `8 x 8 px` | 42 px | OW Palette | Mario Fireball — spinning fireball — frame #1/4 |
| **#36** | `(190, 54)` | `8 x 8 px` | `(190, 54)` | `8 x 8 px` | 43 px | OW Palette | Mario Fireball — spinning fireball — frame #2/4 |
| **#37** | `(200, 54)` | `8 x 8 px` | `(200, 54)` | `8 x 8 px` | 42 px | OW Palette | Mario Fireball — spinning fireball — frame #3/4 |
| **#38** | `(210, 54)` | `8 x 8 px` | `(210, 54)` | `8 x 8 px` | 43 px | OW Palette | Mario Fireball — spinning fireball — frame #4/4 |
| **#39** | `(234, 46)` | `12 x 8 px` | `(234, 46)` | `16 x 8 px` | 48 px | OW Palette | Score Text — 400 points (standard score palette) |
| **#40** | `(234, 56)` | `12 x 8 px` | `(234, 56)` | `16 x 8 px` | 48 px | OW Palette | Score Text — 500 points (standard score palette) |
| **#41** | `(252, 46)` | `16 x 8 px` | `(252, 46)` | `16 x 8 px` | 63 px | OW Palette | Score Text — 800 points (standard score palette) |
| **#42** | `(252, 56)` | `16 x 8 px` | `(252, 56)` | `16 x 8 px` | 63 px | OW Palette | Score Text — 8000 points (standard score palette) |
| **#43** | `(32, 64)` | `16 x 8 px` | `(32, 64)` | `16 x 8 px` | 100 px | OW Palette | Springboard / Jumping Board (pad top; flat state) |
| **#44** | `(32, 75)` | `16 x 31 px` | `(32, 75)` | `16 x 31 px` | 232 px | OW Palette | Springboard / Jumping Board (spring extended; crop includes the tile part below) |
| **#45** | `(52, 64)` | `24 x 8 px` | `(52, 64)` | `24 x 8 px` | 150 px | OW Palette | Very Small Moving Platform (24 px lift) |
| **#46** | `(80, 64)` | `32 x 8 px` | `(80, 64)` | `32 x 8 px` | 200 px | OW Palette | Small Moving Platform (32 px lift) |
| **#47** | `(80, 74)` | `32 x 8 px` | `(80, 74)` | `32 x 8 px` | 208 px | OW Palette | Moving Cloud Platform (32 px cloud) |
| **#48** | `(116, 64)` | `48 x 8 px` | `(116, 64)` | `48 x 8 px` | 300 px | OW Palette | Moving Platform (48 px lift) |
| **#49** | `(116, 74)` | `48 x 8 px` | `(116, 74)` | `48 x 8 px` | 312 px | OW Palette | Moving Cloud Platform (48 px cloud) |
| **#50** | `(184, 68)` | `8 x 8 px` | `(180, 64)` | `16 x 16 px` | 44 px | OW Palette | Fireball Hit / Impact — impact effect — frame #1/3 |
| **#51** | `(200, 65)` | `12 x 14 px` | `(198, 64)` | `16 x 16 px` | 112 px | OW Palette | Fireball Hit / Impact — impact effect — frame #2/3 |
| **#52** | `(216, 64)` | `16 x 16 px` | `(216, 64)` | `16 x 16 px` | 200 px | OW Palette | Fireball Hit / Impact — impact effect — frame #3/3 |
| **#53** | `(234, 66)` | `12 x 8 px` | `(234, 66)` | `16 x 8 px` | 47 px | OW Palette | Score Text — 4000 points (standard score palette) |
| **#54** | `(252, 66)` | `16 x 8 px` | `(252, 66)` | `16 x 8 px` | 62 px | OW Palette | Score Text — 5000 points (standard score palette) |
| **#55** | `(252, 76)` | `16 x 7 px` | `(252, 76)` | `16 x 8 px` | 61 px | OW Palette | Score Text — 1UP (extra-life display; standard score palette) |
| **#56** | `(50, 83)` | `16 x 23 px` | `(50, 83)` | `16 x 23 px` | 228 px | OW Palette | Springboard / Jumping Board (intermediate state) |
| **#57** | `(68, 91)` | `16 x 15 px` | `(68, 91)` | `16 x 15 px` | 220 px | OW Palette | Springboard / Jumping Board (compressed state; crop includes the tile part below) |
| **#58** | `(92, 90)` | `16 x 16 px` | `(92, 90)` | `16 x 16 px` | 136 px | OW Palette | Flag from Pole (end-of-level goal pole flag) |
| **#59** | `(110, 90)` | `13 x 16 px` | `(110, 90)` | `16 x 16 px` | 117 px | OW Palette | Flag from Castle (castle flag) |
| **#60** | `(131, 90)` | `14 x 16 px` | `(130, 90)` | `16 x 16 px` | 71 px | OW Palette | Vine (top part) |
| **#61** | `(149, 90)` | `14 x 16 px` | `(148, 90)` | `16 x 16 px` | 76 px | OW Palette | Vine (stem/next segment) |

---

## 2. Underground Objects (underground environment — #62 to #133)

| # | Flood-fill coords (X, Y) | Flood size | Frame grid cell coords (X, Y) | Frame size | Pixel count | Palette | Meaning / frame state |
|-----|-----------------------|------------------|----------------------|------------------|----------|---------|----------------------------------|
| **#62** | `(0, 116)` | `16 x 16 px` | `(0, 116)` | `16 x 16 px` | 176 px | UG Palette | Super Mushroom (power-up; grows Mario) |
| **#63** | `(32, 116)` | `16 x 16 px` | `(32, 116)` | `16 x 16 px` | 162 px | UG Palette | Fire Flower — palette cycle group A (state #1/4) |
| **#64** | `(50, 116)` | `16 x 16 px` | `(50, 116)` | `16 x 16 px` | 162 px | UG Palette | Fire Flower — palette cycle group A (state #2/4) |
| **#65** | `(68, 116)` | `16 x 16 px` | `(68, 116)` | `16 x 16 px` | 162 px | UG Palette | Fire Flower — palette cycle group A (state #3/4) |
| **#66** | `(86, 116)` | `16 x 16 px` | `(86, 116)` | `16 x 16 px` | 162 px | UG Palette | Fire Flower — palette cycle group A (state #4/4) |
| **#67** | `(107, 116)` | `14 x 16 px` | `(106, 116)` | `16 x 16 px` | 122 px | UG Palette | Super Star — palette cycle group A (state #1/4) |
| **#68** | `(125, 116)` | `14 x 16 px` | `(124, 116)` | `16 x 16 px` | 122 px | UG Palette | Super Star — palette cycle group A (state #2/4) |
| **#69** | `(143, 116)` | `14 x 16 px` | `(142, 116)` | `16 x 16 px` | 122 px | UG Palette | Super Star — palette cycle group A (state #3/4) |
| **#70** | `(161, 116)` | `14 x 16 px` | `(160, 116)` | `16 x 16 px` | 122 px | UG Palette | Super Star — palette cycle group A (state #4/4) |
| **#71** | `(0, 134)` | `16 x 16 px` | `(0, 134)` | `16 x 16 px` | 176 px | UG Palette | 1-Up Mushroom (grants one extra life) |
| **#72** | `(32, 134)` | `16 x 16 px` | `(32, 134)` | `16 x 16 px` | 162 px | UG Palette | Fire Flower — palette cycle group B (state #1/4) |
| **#73** | `(50, 134)` | `16 x 16 px` | `(50, 134)` | `16 x 16 px` | 162 px | UG Palette | Fire Flower — palette cycle group B (state #2/4) |
| **#74** | `(68, 134)` | `16 x 16 px` | `(68, 134)` | `16 x 16 px` | 162 px | UG Palette | Fire Flower — palette cycle group B (state #3/4) |
| **#75** | `(86, 134)` | `16 x 16 px` | `(86, 134)` | `16 x 16 px` | 162 px | UG Palette | Fire Flower — palette cycle group B (state #4/4) |
| **#76** | `(107, 134)` | `14 x 16 px` | `(106, 134)` | `16 x 16 px` | 122 px | UG Palette | Super Star — palette cycle group B (state #1/4) |
| **#77** | `(125, 134)` | `14 x 16 px` | `(124, 134)` | `16 x 16 px` | 122 px | UG Palette | Super Star — palette cycle group B (state #2/4) |
| **#78** | `(143, 134)` | `14 x 16 px` | `(142, 134)` | `16 x 16 px` | 122 px | UG Palette | Super Star — palette cycle group B (state #3/4) |
| **#79** | `(161, 134)` | `14 x 16 px` | `(160, 134)` | `16 x 16 px` | 122 px | UG Palette | Super Star — palette cycle group B (state #4/4) |
| **#80** | `(235, 134)` | `11 x 8 px` | `(234, 134)` | `16 x 8 px` | 43 px | UG Palette | Score Text — 100 points (standard score palette) |
| **#81** | `(253, 134)` | `15 x 8 px` | `(252, 134)` | `16 x 8 px` | 58 px | UG Palette | Score Text — 200 points (standard score palette) |
| **#82** | `(271, 134)` | `11 x 8 px` | `(270, 134)` | `16 x 8 px` | 43 px | UG Palette | Score Text — 100 points (flagpole/peach palette) |
| **#83** | `(289, 134)` | `15 x 8 px` | `(288, 134)` | `16 x 8 px` | 58 px | UG Palette | Score Text — 200 points (flagpole/peach palette) |
| **#84** | `(32, 152)` | `16 x 16 px` | `(32, 152)` | `16 x 16 px` | 162 px | UG Palette | Fire Flower — palette cycle group C (state #1/4) |
| **#85** | `(50, 152)` | `16 x 16 px` | `(50, 152)` | `16 x 16 px` | 162 px | UG Palette | Fire Flower — palette cycle group C (state #2/4) |
| **#86** | `(68, 152)` | `16 x 16 px` | `(68, 152)` | `16 x 16 px` | 162 px | UG Palette | Fire Flower — palette cycle group C (state #3/4) |
| **#87** | `(86, 152)` | `16 x 16 px` | `(86, 152)` | `16 x 16 px` | 162 px | UG Palette | Fire Flower — palette cycle group C (state #4/4) |
| **#88** | `(107, 152)` | `14 x 16 px` | `(106, 152)` | `16 x 16 px` | 122 px | UG Palette | Super Star — palette cycle group C (state #1/4) |
| **#89** | `(125, 152)` | `14 x 16 px` | `(124, 152)` | `16 x 16 px` | 122 px | UG Palette | Super Star — palette cycle group C (state #2/4) |
| **#90** | `(143, 152)` | `14 x 16 px` | `(142, 152)` | `16 x 16 px` | 122 px | UG Palette | Super Star — palette cycle group C (state #3/4) |
| **#91** | `(161, 152)` | `14 x 16 px` | `(160, 152)` | `16 x 16 px` | 122 px | UG Palette | Super Star — palette cycle group C (state #4/4) |
| **#92** | `(180, 145)` | `8 x 14 px` | `(180, 144)` | `8 x 16 px` | 84 px | UG Palette | Spinning Coin — spinning animation — frame #1/4 |
| **#93** | `(192, 145)` | `4 x 14 px` | `(190, 144)` | `8 x 16 px` | 48 px | UG Palette | Spinning Coin — spinning animation — frame #2/4 |
| **#94** | `(204, 145)` | `1 x 14 px` | `(200, 144)` | `8 x 16 px` | 14 px | UG Palette | Spinning Coin — spinning animation — frame #3/4 |
| **#95** | `(212, 145)` | `4 x 14 px` | `(210, 144)` | `8 x 16 px` | 48 px | UG Palette | Spinning Coin — spinning animation — frame #4/4 |
| **#96** | `(234, 144)` | `12 x 8 px` | `(234, 144)` | `16 x 8 px` | 46 px | UG Palette | Score Text — 1000 points (standard score palette) |
| **#97** | `(234, 154)` | `12 x 8 px` | `(234, 154)` | `16 x 8 px` | 48 px | UG Palette | Score Text — 400 points (standard score palette) |
| **#98** | `(252, 144)` | `16 x 8 px` | `(252, 144)` | `16 x 8 px` | 61 px | UG Palette | Score Text — 2000 points (standard score palette) |
| **#99** | `(252, 154)` | `16 x 8 px` | `(252, 154)` | `16 x 8 px` | 63 px | UG Palette | Score Text — 800 points (standard score palette) |
| **#100** | `(270, 144)` | `12 x 8 px` | `(270, 144)` | `16 x 8 px` | 46 px | UG Palette | Score Text — 1000 points (flagpole/peach palette) |
| **#101** | `(270, 154)` | `12 x 8 px` | `(270, 154)` | `16 x 8 px` | 48 px | UG Palette | Score Text — 400 points (flagpole/peach palette) |
| **#102** | `(288, 144)` | `16 x 8 px` | `(288, 144)` | `16 x 8 px` | 61 px | UG Palette | Score Text — 2000 points (flagpole/peach palette) |
| **#103** | `(288, 154)` | `16 x 8 px` | `(288, 154)` | `16 x 8 px` | 63 px | UG Palette | Score Text — 800 points (flagpole/peach palette) |
| **#104** | `(32, 172)` | `16 x 8 px` | `(32, 172)` | `16 x 8 px` | 100 px | UG Palette | Springboard / Jumping Board (pad top; flat state) |
| **#105** | `(52, 172)` | `24 x 8 px` | `(52, 172)` | `24 x 8 px` | 150 px | UG Palette | Very Small Moving Platform (24 px lift) |
| **#106** | `(80, 172)` | `32 x 8 px` | `(80, 172)` | `32 x 8 px` | 200 px | UG Palette | Small Moving Platform (32 px lift) |
| **#107** | `(116, 172)` | `48 x 8 px` | `(116, 172)` | `48 x 8 px` | 300 px | UG Palette | Moving Platform (48 px lift) |
| **#108** | `(180, 162)` | `8 x 8 px` | `(180, 162)` | `8 x 8 px` | 42 px | UG Palette | Mario Fireball — spinning fireball — frame #1/4 |
| **#109** | `(184, 176)` | `8 x 8 px` | `(180, 172)` | `16 x 16 px` | 44 px | UG Palette | Fireball Hit / Impact — impact effect — frame #1/3 |
| **#110** | `(190, 162)` | `8 x 8 px` | `(190, 162)` | `8 x 8 px` | 43 px | UG Palette | Mario Fireball — spinning fireball — frame #2/4 |
| **#111** | `(200, 173)` | `12 x 14 px` | `(198, 172)` | `16 x 16 px` | 112 px | UG Palette | Fireball Hit / Impact — impact effect — frame #2/3 |
| **#112** | `(200, 162)` | `8 x 8 px` | `(200, 162)` | `8 x 8 px` | 42 px | UG Palette | Mario Fireball — spinning fireball — frame #3/4 |
| **#113** | `(210, 162)` | `8 x 8 px` | `(210, 162)` | `8 x 8 px` | 43 px | UG Palette | Mario Fireball — spinning fireball — frame #4/4 |
| **#114** | `(216, 172)` | `16 x 16 px` | `(216, 172)` | `16 x 16 px` | 200 px | UG Palette | Fireball Hit / Impact — impact effect — frame #3/3 |
| **#115** | `(234, 164)` | `12 x 8 px` | `(234, 164)` | `16 x 8 px` | 48 px | UG Palette | Score Text — 500 points (standard score palette) |
| **#116** | `(234, 174)` | `12 x 8 px` | `(234, 174)` | `16 x 8 px` | 47 px | UG Palette | Score Text — 4000 points (standard score palette) |
| **#117** | `(252, 164)` | `16 x 8 px` | `(252, 164)` | `16 x 8 px` | 63 px | UG Palette | Score Text — 8000 points (standard score palette) |
| **#118** | `(252, 174)` | `16 x 8 px` | `(252, 174)` | `16 x 8 px` | 62 px | UG Palette | Score Text — 5000 points (standard score palette) |
| **#119** | `(270, 164)` | `12 x 8 px` | `(270, 164)` | `16 x 8 px` | 48 px | UG Palette | Score Text — 500 points (flagpole/peach palette) |
| **#120** | `(270, 174)` | `12 x 8 px` | `(270, 174)` | `16 x 8 px` | 47 px | UG Palette | Score Text — 4000 points (flagpole/peach palette) |
| **#121** | `(288, 164)` | `16 x 8 px` | `(288, 164)` | `16 x 8 px` | 63 px | UG Palette | Score Text — 8000 points (flagpole/peach palette) |
| **#122** | `(288, 174)` | `16 x 8 px` | `(288, 174)` | `16 x 8 px` | 62 px | UG Palette | Score Text — 5000 points (flagpole/peach palette) |
| **#123** | `(32, 183)` | `16 x 31 px` | `(32, 183)` | `16 x 31 px` | 232 px | UG Palette | Springboard / Jumping Board (spring extended; crop includes the tile part below) |
| **#124** | `(50, 191)` | `16 x 23 px` | `(50, 191)` | `16 x 23 px` | 228 px | UG Palette | Springboard / Jumping Board (intermediate state) |
| **#125** | `(68, 199)` | `16 x 15 px` | `(68, 199)` | `16 x 15 px` | 220 px | UG Palette | Springboard / Jumping Board (compressed state; crop includes the tile part below) |
| **#126** | `(80, 182)` | `32 x 8 px` | `(80, 182)` | `32 x 8 px` | 208 px | UG Palette | Moving Cloud Platform (32 px cloud) |
| **#127** | `(92, 198)` | `16 x 16 px` | `(92, 198)` | `16 x 16 px` | 136 px | UG Palette | Flag from Pole (end-of-level goal pole flag) |
| **#128** | `(110, 198)` | `13 x 16 px` | `(110, 198)` | `16 x 16 px` | 117 px | UG Palette | Flag from Castle (castle flag) |
| **#129** | `(116, 182)` | `48 x 8 px` | `(116, 182)` | `48 x 8 px` | 312 px | UG Palette | Moving Cloud Platform (48 px cloud) |
| **#130** | `(131, 198)` | `14 x 16 px` | `(130, 198)` | `16 x 16 px` | 71 px | UG Palette | Vine (top part) |
| **#131** | `(149, 198)` | `14 x 16 px` | `(148, 198)` | `16 x 16 px` | 76 px | UG Palette | Vine (stem/next segment) |
| **#132** | `(252, 184)` | `16 x 7 px` | `(252, 184)` | `16 x 8 px` | 61 px | UG Palette | Score Text — 1UP (extra-life display; standard score palette) |
| **#133** | `(288, 184)` | `16 x 7 px` | `(288, 184)` | `16 x 8 px` | 61 px | UG Palette | Score Text — 1UP (extra-life display; flagpole/peach palette) |

---

## 3. Castle Objects (castle environment — #134 to #205)

| # | Flood-fill coords (X, Y) | Flood size | Frame grid cell coords (X, Y) | Frame size | Pixel count | Palette | Meaning / frame state |
|-----|-----------------------|------------------|----------------------|------------------|----------|---------|----------------------------------|
| **#134** | `(0, 224)` | `16 x 16 px` | `(0, 224)` | `16 x 16 px` | 176 px | Castle Palette | Super Mushroom (power-up; grows Mario) |
| **#135** | `(32, 224)` | `16 x 16 px` | `(32, 224)` | `16 x 16 px` | 162 px | Castle Palette | Fire Flower — palette cycle group A (state #1/4) |
| **#136** | `(50, 224)` | `16 x 16 px` | `(50, 224)` | `16 x 16 px` | 162 px | Castle Palette | Fire Flower — palette cycle group A (state #2/4) |
| **#137** | `(68, 224)` | `16 x 16 px` | `(68, 224)` | `16 x 16 px` | 162 px | Castle Palette | Fire Flower — palette cycle group A (state #3/4) |
| **#138** | `(86, 224)` | `16 x 16 px` | `(86, 224)` | `16 x 16 px` | 162 px | Castle Palette | Fire Flower — palette cycle group A (state #4/4) |
| **#139** | `(107, 224)` | `14 x 16 px` | `(106, 224)` | `16 x 16 px` | 122 px | Castle Palette | Super Star — palette cycle group A (state #1/4) |
| **#140** | `(125, 224)` | `14 x 16 px` | `(124, 224)` | `16 x 16 px` | 122 px | Castle Palette | Super Star — palette cycle group A (state #2/4) |
| **#141** | `(143, 224)` | `14 x 16 px` | `(142, 224)` | `16 x 16 px` | 122 px | Castle Palette | Super Star — palette cycle group A (state #3/4) |
| **#142** | `(161, 224)` | `14 x 16 px` | `(160, 224)` | `16 x 16 px` | 122 px | Castle Palette | Super Star — palette cycle group A (state #4/4) |
| **#143** | `(0, 242)` | `16 x 16 px` | `(0, 242)` | `16 x 16 px` | 176 px | Castle Palette | 1-Up Mushroom (grants one extra life) |
| **#144** | `(32, 242)` | `16 x 16 px` | `(32, 242)` | `16 x 16 px` | 162 px | Castle Palette | Fire Flower — palette cycle group B (state #1/4) |
| **#145** | `(50, 242)` | `16 x 16 px` | `(50, 242)` | `16 x 16 px` | 162 px | Castle Palette | Fire Flower — palette cycle group B (state #2/4) |
| **#146** | `(68, 242)` | `16 x 16 px` | `(68, 242)` | `16 x 16 px` | 162 px | Castle Palette | Fire Flower — palette cycle group B (state #3/4) |
| **#147** | `(86, 242)` | `16 x 16 px` | `(86, 242)` | `16 x 16 px` | 162 px | Castle Palette | Fire Flower — palette cycle group B (state #4/4) |
| **#148** | `(107, 242)` | `14 x 16 px` | `(106, 242)` | `16 x 16 px` | 122 px | Castle Palette | Super Star — palette cycle group B (state #1/4) |
| **#149** | `(125, 242)` | `14 x 16 px` | `(124, 242)` | `16 x 16 px` | 122 px | Castle Palette | Super Star — palette cycle group B (state #2/4) |
| **#150** | `(143, 242)` | `14 x 16 px` | `(142, 242)` | `16 x 16 px` | 122 px | Castle Palette | Super Star — palette cycle group B (state #3/4) |
| **#151** | `(161, 242)` | `14 x 16 px` | `(160, 242)` | `16 x 16 px` | 122 px | Castle Palette | Super Star — palette cycle group B (state #4/4) |
| **#152** | `(180, 253)` | `8 x 14 px` | `(180, 252)` | `8 x 16 px` | 84 px | Castle Palette | Spinning Coin — spinning animation — frame #1/4 |
| **#153** | `(192, 253)` | `4 x 14 px` | `(190, 252)` | `8 x 16 px` | 48 px | Castle Palette | Spinning Coin — spinning animation — frame #2/4 |
| **#154** | `(204, 253)` | `1 x 14 px` | `(200, 252)` | `8 x 16 px` | 14 px | Castle Palette | Spinning Coin — spinning animation — frame #3/4 |
| **#155** | `(212, 253)` | `4 x 14 px` | `(210, 252)` | `8 x 16 px` | 48 px | Castle Palette | Spinning Coin — spinning animation — frame #4/4 |
| **#156** | `(235, 242)` | `11 x 8 px` | `(234, 242)` | `16 x 8 px` | 43 px | Castle Palette | Score Text — 100 points (standard score palette) |
| **#157** | `(234, 252)` | `12 x 8 px` | `(234, 252)` | `16 x 8 px` | 46 px | Castle Palette | Score Text — 1000 points (standard score palette) |
| **#158** | `(253, 242)` | `15 x 8 px` | `(252, 242)` | `16 x 8 px` | 58 px | Castle Palette | Score Text — 200 points (standard score palette) |
| **#159** | `(252, 252)` | `16 x 8 px` | `(252, 252)` | `16 x 8 px` | 61 px | Castle Palette | Score Text — 2000 points (standard score palette) |
| **#160** | `(271, 242)` | `11 x 8 px` | `(270, 242)` | `16 x 8 px` | 43 px | Castle Palette | Score Text — 100 points (flagpole/peach palette) |
| **#161** | `(270, 252)` | `12 x 8 px` | `(270, 252)` | `16 x 8 px` | 46 px | Castle Palette | Score Text — 1000 points (flagpole/peach palette) |
| **#162** | `(289, 242)` | `15 x 8 px` | `(288, 242)` | `16 x 8 px` | 58 px | Castle Palette | Score Text — 200 points (flagpole/peach palette) |
| **#163** | `(288, 252)` | `16 x 8 px` | `(288, 252)` | `16 x 8 px` | 61 px | Castle Palette | Score Text — 2000 points (flagpole/peach palette) |
| **#164** | `(32, 260)` | `16 x 16 px` | `(32, 260)` | `16 x 16 px` | 162 px | Castle Palette | Fire Flower — palette cycle group C (state #1/4) |
| **#165** | `(50, 260)` | `16 x 16 px` | `(50, 260)` | `16 x 16 px` | 162 px | Castle Palette | Fire Flower — palette cycle group C (state #2/4) |
| **#166** | `(68, 260)` | `16 x 16 px` | `(68, 260)` | `16 x 16 px` | 162 px | Castle Palette | Fire Flower — palette cycle group C (state #3/4) |
| **#167** | `(86, 260)` | `16 x 16 px` | `(86, 260)` | `16 x 16 px` | 162 px | Castle Palette | Fire Flower — palette cycle group C (state #4/4) |
| **#168** | `(107, 260)` | `14 x 16 px` | `(106, 260)` | `16 x 16 px` | 122 px | Castle Palette | Super Star — palette cycle group C (state #1/4) |
| **#169** | `(125, 260)` | `14 x 16 px` | `(124, 260)` | `16 x 16 px` | 122 px | Castle Palette | Super Star — palette cycle group C (state #2/4) |
| **#170** | `(143, 260)` | `14 x 16 px` | `(142, 260)` | `16 x 16 px` | 122 px | Castle Palette | Super Star — palette cycle group C (state #3/4) |
| **#171** | `(161, 260)` | `14 x 16 px` | `(160, 260)` | `16 x 16 px` | 122 px | Castle Palette | Super Star — palette cycle group C (state #4/4) |
| **#172** | `(180, 270)` | `8 x 8 px` | `(180, 270)` | `8 x 8 px` | 42 px | Castle Palette | Mario Fireball — spinning fireball — frame #1/4 |
| **#173** | `(190, 270)` | `8 x 8 px` | `(190, 270)` | `8 x 8 px` | 43 px | Castle Palette | Mario Fireball — spinning fireball — frame #2/4 |
| **#174** | `(200, 270)` | `8 x 8 px` | `(200, 270)` | `8 x 8 px` | 42 px | Castle Palette | Mario Fireball — spinning fireball — frame #3/4 |
| **#175** | `(210, 270)` | `8 x 8 px` | `(210, 270)` | `8 x 8 px` | 43 px | Castle Palette | Mario Fireball — spinning fireball — frame #4/4 |
| **#176** | `(234, 262)` | `12 x 8 px` | `(234, 262)` | `16 x 8 px` | 48 px | Castle Palette | Score Text — 400 points (standard score palette) |
| **#177** | `(234, 272)` | `12 x 8 px` | `(234, 272)` | `16 x 8 px` | 48 px | Castle Palette | Score Text — 500 points (standard score palette) |
| **#178** | `(252, 262)` | `16 x 8 px` | `(252, 262)` | `16 x 8 px` | 63 px | Castle Palette | Score Text — 800 points (standard score palette) |
| **#179** | `(252, 272)` | `16 x 8 px` | `(252, 272)` | `16 x 8 px` | 63 px | Castle Palette | Score Text — 8000 points (standard score palette) |
| **#180** | `(270, 262)` | `12 x 8 px` | `(270, 262)` | `16 x 8 px` | 48 px | Castle Palette | Score Text — 400 points (flagpole/peach palette) |
| **#181** | `(270, 272)` | `12 x 8 px` | `(270, 272)` | `16 x 8 px` | 48 px | Castle Palette | Score Text — 500 points (flagpole/peach palette) |
| **#182** | `(288, 262)` | `16 x 8 px` | `(288, 262)` | `16 x 8 px` | 63 px | Castle Palette | Score Text — 800 points (flagpole/peach palette) |
| **#183** | `(288, 272)` | `16 x 8 px` | `(288, 272)` | `16 x 8 px` | 63 px | Castle Palette | Score Text — 8000 points (flagpole/peach palette) |
| **#184** | `(32, 280)` | `16 x 8 px` | `(32, 280)` | `16 x 8 px` | 100 px | Castle Palette | Springboard / Jumping Board (pad top; flat state) |
| **#185** | `(32, 291)` | `16 x 31 px` | `(32, 291)` | `16 x 31 px` | 232 px | Castle Palette | Springboard / Jumping Board (spring extended; crop includes the tile part below) |
| **#186** | `(50, 299)` | `16 x 23 px` | `(50, 299)` | `16 x 23 px` | 228 px | Castle Palette | Springboard / Jumping Board (intermediate state) |
| **#187** | `(52, 280)` | `24 x 8 px` | `(52, 280)` | `24 x 8 px` | 150 px | Castle Palette | Very Small Moving Platform (24 px lift) |
| **#188** | `(80, 280)` | `32 x 8 px` | `(80, 280)` | `32 x 8 px` | 200 px | Castle Palette | Small Moving Platform (32 px lift) |
| **#189** | `(80, 290)` | `32 x 8 px` | `(80, 290)` | `32 x 8 px` | 208 px | Castle Palette | Moving Cloud Platform (32 px cloud) |
| **#190** | `(116, 280)` | `48 x 8 px` | `(116, 280)` | `48 x 8 px` | 300 px | Castle Palette | Moving Platform (48 px lift) |
| **#191** | `(116, 290)` | `48 x 8 px` | `(116, 290)` | `48 x 8 px` | 312 px | Castle Palette | Moving Cloud Platform (48 px cloud) |
| **#192** | `(184, 284)` | `8 x 8 px` | `(180, 280)` | `16 x 16 px` | 44 px | Castle Palette | Fireball Hit / Impact — impact effect — frame #1/3 |
| **#193** | `(200, 281)` | `12 x 14 px` | `(198, 280)` | `16 x 16 px` | 112 px | Castle Palette | Fireball Hit / Impact — impact effect — frame #2/3 |
| **#194** | `(216, 280)` | `16 x 16 px` | `(216, 280)` | `16 x 16 px` | 200 px | Castle Palette | Fireball Hit / Impact — impact effect — frame #3/3 |
| **#195** | `(234, 282)` | `12 x 8 px` | `(234, 282)` | `16 x 8 px` | 47 px | Castle Palette | Score Text — 4000 points (standard score palette) |
| **#196** | `(252, 282)` | `16 x 8 px` | `(252, 282)` | `16 x 8 px` | 62 px | Castle Palette | Score Text — 5000 points (standard score palette) |
| **#197** | `(252, 292)` | `16 x 7 px` | `(252, 292)` | `16 x 8 px` | 61 px | Castle Palette | Score Text — 1UP (extra-life display; standard score palette) |
| **#198** | `(270, 282)` | `12 x 8 px` | `(270, 282)` | `16 x 8 px` | 47 px | Castle Palette | Score Text — 4000 points (flagpole/peach palette) |
| **#199** | `(288, 282)` | `16 x 8 px` | `(288, 282)` | `16 x 8 px` | 62 px | Castle Palette | Score Text — 5000 points (flagpole/peach palette) |
| **#200** | `(288, 292)` | `16 x 7 px` | `(288, 292)` | `16 x 8 px` | 61 px | Castle Palette | Score Text — 1UP (extra-life display; flagpole/peach palette) |
| **#201** | `(68, 307)` | `16 x 15 px` | `(68, 307)` | `16 x 15 px` | 220 px | Castle Palette | Springboard / Jumping Board (compressed state; crop includes the tile part below) |
| **#202** | `(92, 306)` | `16 x 16 px` | `(92, 306)` | `16 x 16 px` | 136 px | Castle Palette | Flag from Pole (end-of-level goal pole flag) |
| **#203** | `(110, 306)` | `13 x 16 px` | `(110, 306)` | `16 x 16 px` | 117 px | Castle Palette | Flag from Castle (castle flag) |
| **#204** | `(131, 306)` | `14 x 16 px` | `(130, 306)` | `16 x 16 px` | 71 px | Castle Palette | Vine (top part) |
| **#205** | `(149, 306)` | `14 x 16 px` | `(148, 306)` | `16 x 16 px` | 76 px | Castle Palette | Vine (stem/next segment) |

---

## 4. Underwater Objects (underwater environment — #206 to #266)

| # | Flood-fill coords (X, Y) | Flood size | Frame grid cell coords (X, Y) | Frame size | Pixel count | Palette | Meaning / frame state |
|-----|-----------------------|------------------|----------------------|------------------|----------|---------|----------------------------------|
| **#206** | `(0, 332)` | `16 x 16 px` | `(0, 332)` | `16 x 16 px` | 176 px | UW Palette | Super Mushroom (power-up; grows Mario) |
| **#207** | `(32, 332)` | `16 x 16 px` | `(32, 332)` | `16 x 16 px` | 162 px | UW Palette | Fire Flower — palette cycle group A (state #1/4) |
| **#208** | `(50, 332)` | `16 x 16 px` | `(50, 332)` | `16 x 16 px` | 162 px | UW Palette | Fire Flower — palette cycle group A (state #2/4) |
| **#209** | `(68, 332)` | `16 x 16 px` | `(68, 332)` | `16 x 16 px` | 162 px | UW Palette | Fire Flower — palette cycle group A (state #3/4) |
| **#210** | `(86, 332)` | `16 x 16 px` | `(86, 332)` | `16 x 16 px` | 162 px | UW Palette | Fire Flower — palette cycle group A (state #4/4) |
| **#211** | `(107, 332)` | `14 x 16 px` | `(106, 332)` | `16 x 16 px` | 122 px | UW Palette | Super Star — palette cycle group A (state #1/4) |
| **#212** | `(125, 332)` | `14 x 16 px` | `(124, 332)` | `16 x 16 px` | 122 px | UW Palette | Super Star — palette cycle group A (state #2/4) |
| **#213** | `(143, 332)` | `14 x 16 px` | `(142, 332)` | `16 x 16 px` | 122 px | UW Palette | Super Star — palette cycle group A (state #3/4) |
| **#214** | `(161, 332)` | `14 x 16 px` | `(160, 332)` | `16 x 16 px` | 122 px | UW Palette | Super Star — palette cycle group A (state #4/4) |
| **#215** | `(0, 350)` | `16 x 16 px` | `(0, 350)` | `16 x 16 px` | 176 px | UW Palette | 1-Up Mushroom (grants one extra life) |
| **#216** | `(32, 350)` | `16 x 16 px` | `(32, 350)` | `16 x 16 px` | 162 px | UW Palette | Fire Flower — palette cycle group B (state #1/4) |
| **#217** | `(50, 350)` | `16 x 16 px` | `(50, 350)` | `16 x 16 px` | 162 px | UW Palette | Fire Flower — palette cycle group B (state #2/4) |
| **#218** | `(68, 350)` | `16 x 16 px` | `(68, 350)` | `16 x 16 px` | 162 px | UW Palette | Fire Flower — palette cycle group B (state #3/4) |
| **#219** | `(86, 350)` | `16 x 16 px` | `(86, 350)` | `16 x 16 px` | 162 px | UW Palette | Fire Flower — palette cycle group B (state #4/4) |
| **#220** | `(107, 350)` | `14 x 16 px` | `(106, 350)` | `16 x 16 px` | 122 px | UW Palette | Super Star — palette cycle group B (state #1/4) |
| **#221** | `(125, 350)` | `14 x 16 px` | `(124, 350)` | `16 x 16 px` | 122 px | UW Palette | Super Star — palette cycle group B (state #2/4) |
| **#222** | `(143, 350)` | `14 x 16 px` | `(142, 350)` | `16 x 16 px` | 122 px | UW Palette | Super Star — palette cycle group B (state #3/4) |
| **#223** | `(161, 350)` | `14 x 16 px` | `(160, 350)` | `16 x 16 px` | 122 px | UW Palette | Super Star — palette cycle group B (state #4/4) |
| **#224** | `(235, 350)` | `11 x 8 px` | `(234, 350)` | `16 x 8 px` | 43 px | UW Palette | Score Text — 100 points (standard score palette) |
| **#225** | `(253, 350)` | `15 x 8 px` | `(252, 350)` | `16 x 8 px` | 58 px | UW Palette | Score Text — 200 points (standard score palette) |
| **#226** | `(32, 368)` | `16 x 16 px` | `(32, 368)` | `16 x 16 px` | 162 px | UW Palette | Fire Flower — palette cycle group C (state #1/4) |
| **#227** | `(50, 368)` | `16 x 16 px` | `(50, 368)` | `16 x 16 px` | 162 px | UW Palette | Fire Flower — palette cycle group C (state #2/4) |
| **#228** | `(68, 368)` | `16 x 16 px` | `(68, 368)` | `16 x 16 px` | 162 px | UW Palette | Fire Flower — palette cycle group C (state #3/4) |
| **#229** | `(86, 368)` | `16 x 16 px` | `(86, 368)` | `16 x 16 px` | 162 px | UW Palette | Fire Flower — palette cycle group C (state #4/4) |
| **#230** | `(107, 368)` | `14 x 16 px` | `(106, 368)` | `16 x 16 px` | 122 px | UW Palette | Super Star — palette cycle group C (state #1/4) |
| **#231** | `(125, 368)` | `14 x 16 px` | `(124, 368)` | `16 x 16 px` | 122 px | UW Palette | Super Star — palette cycle group C (state #2/4) |
| **#232** | `(143, 368)` | `14 x 16 px` | `(142, 368)` | `16 x 16 px` | 122 px | UW Palette | Super Star — palette cycle group C (state #3/4) |
| **#233** | `(161, 368)` | `14 x 16 px` | `(160, 368)` | `16 x 16 px` | 122 px | UW Palette | Super Star — palette cycle group C (state #4/4) |
| **#234** | `(180, 361)` | `8 x 14 px` | `(180, 360)` | `8 x 16 px` | 84 px | UW Palette | Spinning Coin — spinning animation — frame #1/4 |
| **#235** | `(180, 378)` | `8 x 8 px` | `(180, 378)` | `8 x 8 px` | 42 px | UW Palette | Mario Fireball — spinning fireball — frame #1/4 |
| **#236** | `(192, 361)` | `4 x 14 px` | `(190, 360)` | `8 x 16 px` | 48 px | UW Palette | Spinning Coin — spinning animation — frame #2/4 |
| **#237** | `(190, 378)` | `8 x 8 px` | `(190, 378)` | `8 x 8 px` | 43 px | UW Palette | Mario Fireball — spinning fireball — frame #2/4 |
| **#238** | `(204, 361)` | `1 x 14 px` | `(200, 360)` | `8 x 16 px` | 14 px | UW Palette | Spinning Coin — spinning animation — frame #3/4 |
| **#239** | `(200, 378)` | `8 x 8 px` | `(200, 378)` | `8 x 8 px` | 42 px | UW Palette | Mario Fireball — spinning fireball — frame #3/4 |
| **#240** | `(212, 361)` | `4 x 14 px` | `(210, 360)` | `8 x 16 px` | 48 px | UW Palette | Spinning Coin — spinning animation — frame #4/4 |
| **#241** | `(210, 378)` | `8 x 8 px` | `(210, 378)` | `8 x 8 px` | 43 px | UW Palette | Mario Fireball — spinning fireball — frame #4/4 |
| **#242** | `(234, 360)` | `12 x 8 px` | `(234, 360)` | `16 x 8 px` | 46 px | UW Palette | Score Text — 1000 points (standard score palette) |
| **#243** | `(234, 370)` | `12 x 8 px` | `(234, 370)` | `16 x 8 px` | 48 px | UW Palette | Score Text — 400 points (standard score palette) |
| **#244** | `(252, 360)` | `16 x 8 px` | `(252, 360)` | `16 x 8 px` | 61 px | UW Palette | Score Text — 2000 points (standard score palette) |
| **#245** | `(252, 370)` | `16 x 8 px` | `(252, 370)` | `16 x 8 px` | 63 px | UW Palette | Score Text — 800 points (standard score palette) |
| **#246** | `(32, 388)` | `16 x 8 px` | `(32, 388)` | `16 x 8 px` | 100 px | UW Palette | Springboard / Jumping Board (pad top; flat state) |
| **#247** | `(32, 399)` | `16 x 30 px` | `(32, 399)` | `16 x 30 px` | 216 px | UW Palette | Springboard / Jumping Board (spring extended; crop includes the tile part below) |
| **#248** | `(52, 388)` | `24 x 8 px` | `(52, 388)` | `24 x 8 px` | 150 px | UW Palette | Very Small Moving Platform (24 px lift) |
| **#249** | `(80, 388)` | `32 x 8 px` | `(80, 388)` | `32 x 8 px` | 200 px | UW Palette | Small Moving Platform (32 px lift) |
| **#250** | `(80, 398)` | `32 x 8 px` | `(80, 398)` | `32 x 8 px` | 208 px | UW Palette | Moving Cloud Platform (32 px cloud) |
| **#251** | `(116, 388)` | `48 x 8 px` | `(116, 388)` | `48 x 8 px` | 300 px | UW Palette | Moving Platform (48 px lift) |
| **#252** | `(116, 398)` | `48 x 8 px` | `(116, 398)` | `48 x 8 px` | 312 px | UW Palette | Moving Cloud Platform (48 px cloud) |
| **#253** | `(184, 392)` | `8 x 8 px` | `(180, 388)` | `16 x 16 px` | 44 px | UW Palette | Fireball Hit / Impact — impact effect — frame #1/3 |
| **#254** | `(200, 389)` | `12 x 14 px` | `(198, 388)` | `16 x 16 px` | 112 px | UW Palette | Fireball Hit / Impact — impact effect — frame #2/3 |
| **#255** | `(216, 388)` | `16 x 16 px` | `(216, 388)` | `16 x 16 px` | 200 px | UW Palette | Fireball Hit / Impact — impact effect — frame #3/3 |
| **#256** | `(234, 380)` | `12 x 8 px` | `(234, 380)` | `16 x 8 px` | 48 px | UW Palette | Score Text — 500 points (standard score palette) |
| **#257** | `(234, 390)` | `12 x 8 px` | `(234, 390)` | `16 x 8 px` | 47 px | UW Palette | Score Text — 4000 points (standard score palette) |
| **#258** | `(252, 380)` | `16 x 8 px` | `(252, 380)` | `16 x 8 px` | 63 px | UW Palette | Score Text — 8000 points (standard score palette) |
| **#259** | `(252, 390)` | `16 x 8 px` | `(252, 390)` | `16 x 8 px` | 62 px | UW Palette | Score Text — 5000 points (standard score palette) |
| **#260** | `(50, 407)` | `16 x 22 px` | `(50, 407)` | `16 x 22 px` | 212 px | UW Palette | Springboard / Jumping Board (intermediate state) |
| **#261** | `(68, 415)` | `16 x 14 px` | `(68, 415)` | `16 x 14 px` | 204 px | UW Palette | Springboard / Jumping Board (compressed state; crop includes the tile part below) |
| **#262** | `(92, 414)` | `16 x 15 px` | `(92, 414)` | `16 x 15 px` | 135 px | UW Palette | Flag from Pole (end-of-level goal pole flag) |
| **#263** | `(110, 414)` | `13 x 15 px` | `(110, 414)` | `16 x 15 px` | 116 px | UW Palette | Flag from Castle (castle flag) |
| **#264** | `(131, 414)` | `14 x 15 px` | `(130, 414)` | `16 x 15 px` | 69 px | UW Palette | Vine (top part) |
| **#265** | `(149, 414)` | `14 x 15 px` | `(148, 414)` | `16 x 15 px` | 74 px | UW Palette | Vine (stem/next segment) |
| **#266** | `(252, 400)` | `16 x 7 px` | `(252, 400)` | `16 x 8 px` | 61 px | UW Palette | Score Text — 1UP (extra-life display; standard score palette) |

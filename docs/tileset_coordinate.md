# Tileset Coordinate Audit (`tileset_coordinate.md`)

## Audit conclusions - treat this section as the source of truth

`assets/textures/tiles/tileset.png` is the source sheet, **680 x 356 px**. The image `docs/assets/reference/tileset_all_components_atlas_full.png` is an analysis image relaid out at **720 x 2448 px**; it does not share a coordinate system with the source sheet.

The `#1...#345` labels in the atlas are just crop order numbers; there are 340 actual crop rows (skipping `#78`, `#84`, `#111`, `#173`, `#179`). Never use a `#N` number directly as a coordinate.

The old component table below is kept for tracing the crops that were measured, but its `Legacy meaning` column is **deprecated**: many labels were previously guessed from color or position, so bricks/questions/coins/castles were misnamed. The verified gameplay and object mappings are in the audit tables below.

The source sheet's dimensions and object groups (coin, question block, pipe, tileset) were cross-checked against the [SMB1 Tileset on The Spriters Resource](https://www.spriters-resource.com/nes/supermariobros/asset/52571/?source=genre).

### 1. Terrain 16 x 16 - four environment palettes

The Overworld/Castle groups contain ground, brick, stone and hard-block frames. Underground additionally has a brick variant; do not confuse that frame with the solid block slot used for `S` in Level 2.

| Component | Overworld | Underground | Castle | Underwater |
|---|---|---|---|---|
| Ground | `(0,16)` | `(147,16)` | `(0,151)` | `(147,134)` |
| Brick | `(17,16)` | `(164,16)` | `(17,100)` | `(164,100)` |
| Brick variant (Underground only) | — | `(181,16)` | — | — |
| Stone/solid | `(0,33)` | `(147,33)` | `(0,151)` | `(215,297)` |
| Hard block | `(349,78)` | `(445,78)` | `(541,78)` | `(637,78)` |

**Level 2 contract:** the `S` symbol renders `TileFrames::STONE_UNDERGROUND` at `(147,33)` (see `src/level/TileMap.cpp`). The crop at `(181,16)` is only the underground brick variant and is never used to render `S` columns.

### 2. Question/used block - Palette 3

These frames are the tileset's actual question/used blocks; the cells at `(85,16)` and `(198,16)` belong to the terrain/palette 1 group and must not be reassigned as question/used blocks.

| Environment | Question animation (3 frames) | Used/empty |
|---|---|---|
| Overworld | `(298,78)`, `(315,78)`, `(332,78)` | `(349,78)` |
| Underground | `(394,78)`, `(411,78)`, `(428,78)` | `(445,78)` |
| Castle | `(490,78)`, `(507,78)`, `(524,78)` | `(541,78)` |
| Underwater | `(586,78)`, `(603,78)`, `(620,78)` | `(637,78)` |

At runtime `QuestionBlock` still owns the animation/item spawn; the table above is only a coordinate catalog of the tileset and does not create a second question block.

### 3. Regular coins outside question blocks - a previously missing asset

The map coin is its own 16 x 16 animation in Palette 3, distinct from the `QuestionBlock` popup coin that comes from `items_objects.png`.

| Environment | Frame 1 | Frame 2 | Frame 3 |
|---|---:|---:|---:|
| Overworld | `(298,95)` | `(315,95)` | `(332,95)` |
| Underground | `(394,95)` | `(411,95)` | `(428,95)` |
| Castle | `(490,95)` | `(507,95)` | `(524,95)` |
| Underwater | `(586,95)` | `(603,95)` | `(620,95)` |

`CoinType::COLLECTIBLE` uses the three Overworld frames above; `QUESTION_POPUP` keeps using the existing item sheet so the two coin kinds are never mixed.

### 4. Pipes and flag pole, 16 x 16

Each pipe is two tiles wide. The Castle/Underwater frames were previously skipped so Level 3 fell back to the Overworld pipe; they have been added to `TileFrames.h`.

| Environment | Pipe top L/R | Pipe body L/R | Flag top | Pole body |
|---|---|---|---|---|
| Overworld | `(119,196)`, `(136,196)` | `(119,213)`, `(136,213)` | `(136,230)` | `(136,247)` |
| Underground | `(283,196)`, `(300,196)` | `(283,213)`, `(300,213)` | `(300,230)` | `(300,247)` |
| Castle | `(119,280)`, `(136,280)` | `(119,297)`, `(136,297)` | `(136,314)` | `(136,331)` |
| Underwater | `(283,280)`, `(300,280)` | `(283,297)`, `(300,297)` | `(300,314)` | `(300,331)` |

### 5. Composite scenery in the runtime tileset

Some scenery that is awkward to express with 16x16 tiles is stored directly as composite sprites in the runtime `tileset.png`.

| Object | Bbox `(x,y,w,h)` | Level marker |
|---|---|---|
| Castle | `(328,196,80,80)` | `L` |
| Horizontal pipe | `(328,298,48,32)` | `H` |

`L` is the bottom-left anchor of the 5x5-world-tile castle.

`H` is the bottom-left anchor of the 3x2-world-tile horizontal pipe.

Both objects are rendered as one complete texture rectangle instead of being split into multiple 16x16 frames.

The flag pole is still built from the `T`, `F` and `|` markers, so no separate assembled flag-pole sprite is needed.

### 6. Crop backgrounds and scope of use

The source sheet is a composited image without alpha. The three background colors that must be made transparent are `RGB(0,41,140)`, `RGB(146,144,255)` and `RGB(148,148,255)`.

`TextureManager` already chroma-keys entity textures; `TileMap` applies the same mask when uploading the tileset, so pipes/poles/castles no longer drag along the purple/blue background cells. Do not blindly strip every blue pixel in the image file: key only these three background colors, and keep black/castle-hole colors and the sprites' real palette colors intact.

`#214...#279` are Palette 0 Overworld/Underground components (pipes and scenery), and `#280...#345` are Palette 0 Castle/Underwater components. Therefore the old labels such as "White Slopes" and "Peach/Toad Room" for those regions are no longer used. The sheet has a bush in the assembled Overworld region, but `levels/level3.txt` is a Castle level with no bush symbol; do not add bushes to Level 3 merely because they appear on the reference sheet.

---

## Legacy component crop table (coordinates kept; the legacy-meaning column is not used)

| # | Coordinates (X, Y) | Size (W x H) | File | Legacy meaning (deprecated) |
|-----|---------------|--------------------|----------|---------|
| **#1** | (0, 16) | 16 x 16 px | `object_001.png` | Ground Tile - Overworld |
| **#2** | (17, 16) | 16 x 16 px | `object_002.png` | Brick Block - Overworld |
| **#3** | (34, 16) | 16 x 16 px | `object_003.png` | Stone / Solid Block - Overworld |
| **#4** | (51, 16) | 16 x 16 px | `object_004.png` | Solid / Hard Block - Overworld |
| **#5** | (68, 16) | 16 x 16 px | `object_005.png` | Brick Block Item Variant - Overworld |
| **#6** | (85, 16) | 16 x 16 px | `object_006.png` | Question Block - Overworld |
| **#7** | (102, 16) | 16 x 16 px | `object_007.png` | Castle - Black Window Cell |
| **#8** | (119, 16) | 16 x 16 px | `object_008.png` | Castle - Door Top |
| **#9** | (147, 16) | 16 x 16 px | `object_009.png` | Ground Tile - Underground (teal palette) |
| **#10** | (164, 16) | 16 x 16 px | `object_010.png` | Brick Block Variant 1 - Underground (teal palette) |
| **#11** | (181, 16) | 16 x 16 px | `object_011.png` | Brick Block Variant 2 - Underground (teal palette) |
| **#12** | (198, 16) | 16 x 16 px | `object_012.png` | Stone / Solid Block - Underground (teal palette) |
| **#13** | (215, 16) | 16 x 16 px | `object_013.png` | Castle - Door Top |
| **#14** | (232, 16) | 16 x 16 px | `object_014.png` | Castle - Brick Wall (blue) |
| **#15** | (249, 16) | 16 x 16 px | `object_015.png` | Castle - Black Window Cell |
| **#16** | (266, 16) | 13 x 16 px | `object_016.png` | Castle - Door Bottom |
| **#17** | (298, 16) | 16 x 16 px | `object_017.png` | Decoration - Cloud & Bush Parts |
| **#18** | (315, 16) | 16 x 16 px | `object_018.png` | Decoration - Cloud & Bush Parts |
| **#19** | (332, 16) | 16 x 16 px | `object_019.png` | Decoration - Cloud & Bush Parts |
| **#20** | (349, 16) | 16 x 16 px | `object_020.png` | Decoration - Cloud & Bush Parts |
| **#21** | (366, 16) | 16 x 16 px | `object_021.png` | Decoration - Cloud & Bush Parts |
| **#22** | (394, 16) | 16 x 16 px | `object_022.png` | Decoration - Cloud & Bush Parts |
| **#23** | (411, 16) | 16 x 16 px | `object_023.png` | Decoration - Cloud & Bush Parts |
| **#24** | (428, 16) | 16 x 16 px | `object_024.png` | Decoration - Cloud & Bush Parts |
| **#25** | (445, 16) | 16 x 16 px | `object_025.png` | Decoration - Cloud & Bush Parts |
| **#26** | (462, 16) | 16 x 16 px | `object_026.png` | Decoration - Cloud & Bush Parts |
| **#27** | (490, 16) | 16 x 16 px | `object_027.png` | Decoration - Cloud & Bush Parts |
| **#28** | (507, 16) | 16 x 16 px | `object_028.png` | Decoration - Cloud & Bush Parts |
| **#29** | (524, 16) | 16 x 16 px | `object_029.png` | Decoration - Cloud & Bush Parts |
| **#30** | (541, 16) | 16 x 16 px | `object_030.png` | Decoration - Cloud & Bush Parts |
| **#31** | (558, 16) | 16 x 16 px | `object_031.png` | Decoration - Cloud & Bush Parts |
| **#32** | (586, 16) | 16 x 16 px | `object_032.png` | Decoration - Cloud & Bush Parts |
| **#33** | (603, 16) | 16 x 16 px | `object_033.png` | Decoration - Cloud & Bush Parts |
| **#34** | (620, 16) | 16 x 16 px | `object_034.png` | Decoration - Cloud & Bush Parts |
| **#35** | (637, 16) | 16 x 16 px | `object_035.png` | Decoration - Cloud & Bush Parts |
| **#36** | (654, 16) | 16 x 16 px | `object_036.png` | Decoration - Cloud & Bush Parts |
| **#37** | (0, 33) | 16 x 16 px | `object_037.png` | Castle - Battlement |
| **#38** | (17, 33) | 16 x 16 px | `object_038.png` | Mushroom Stem / Bridge Pillar |
| **#39** | (34, 33) | 16 x 16 px | `object_039.png` | Castle - Wall Brick |
| **#40** | (51, 33) | 16 x 16 px | `object_040.png` | Castle - Wall Brick |
| **#41** | (68, 33) | 16 x 16 px | `object_041.png` | Castle - Wall Brick |
| **#42** | (85, 33) | 16 x 16 px | `object_042.png` | Castle - Wall Brick |
| **#43** | (102, 33) | 16 x 16 px | `object_043.png` | Solid Filler - black (used to line under dungeons/doors) |
| **#44** | (119, 33) | 16 x 16 px | `object_044.png` | Mushroom Platform Top |
| **#45** | (147, 33) | 16 x 16 px | `object_045.png` | Mushroom Platform Top |
| **#46** | (164, 33) | 16 x 16 px | `object_046.png` | Mushroom Platform Top |
| **#47** | (181, 33) | 16 x 16 px | `object_047.png` | Underwater Coral |
| **#48** | (198, 33) | 16 x 16 px | `object_048.png` | Underwater Coral |
| **#49** | (215, 33) | 16 x 16 px | `object_049.png` | Castle - Wall Brick |
| **#50** | (232, 33) | 16 x 16 px | `object_050.png` | Castle - Wall Brick |
| **#51** | (249, 33) | 16 x 16 px | `object_051.png` | Solid Filler - black (used to line under dungeons/doors) |
| **#52** | (266, 33) | 13 x 16 px | `object_052.png` | Castle - Wall Brick |
| **#53** | (298, 33) | 16 x 16 px | `object_053.png` | Solid Filler - sky/water background blue |
| **#54** | (315, 33) | 16 x 16 px | `object_054.png` | Water Surface |
| **#55** | (332, 33) | 16 x 16 px | `object_055.png` | Water Surface |
| **#56** | (349, 33) | 16 x 16 px | `object_056.png` | Solid Filler - sky/water background blue |
| **#57** | (366, 33) | 16 x 16 px | `object_057.png` | Underwater Fence / Gate |
| **#58** | (394, 33) | 16 x 16 px | `object_058.png` | Underwater Fence / Gate |
| **#59** | (411, 33) | 16 x 16 px | `object_059.png` | Underwater Fence / Gate |
| **#60** | (428, 33) | 16 x 16 px | `object_060.png` | Underwater Fence / Gate |
| **#61** | (445, 33) | 16 x 16 px | `object_061.png` | Underwater Fence / Gate |
| **#62** | (462, 33) | 16 x 16 px | `object_062.png` | Underwater Fence / Gate |
| **#63** | (490, 33) | 16 x 16 px | `object_063.png` | Solid Filler - light blue |
| **#64** | (507, 33) | 16 x 16 px | `object_064.png` | Water Surface |
| **#65** | (524, 33) | 16 x 16 px | `object_065.png` | Decoration - cloud / bush parts |
| **#66** | (541, 33) | 16 x 16 px | `object_066.png` | Solid Filler - red |
| **#67** | (558, 33) | 16 x 16 px | `object_067.png` | Underwater Fence / Gate |
| **#68** | (586, 33) | 16 x 16 px | `object_068.png` | Underwater Fence / Gate |
| **#69** | (603, 33) | 16 x 16 px | `object_069.png` | Underwater Fence / Gate |
| **#70** | (620, 33) | 16 x 16 px | `object_070.png` | Underwater Fence / Gate |
| **#71** | (637, 33) | 16 x 16 px | `object_071.png` | Underwater Fence / Gate |
| **#72** | (654, 33) | 16 x 16 px | `object_072.png` | Underwater Fence / Gate |
| **#73** | (0, 50) | 16 x 16 px | `object_073.png` | Stone Stair Block |
| **#74** | (17, 50) | 16 x 16 px | `object_074.png` | Bullet Bill Cannon - top, barrel, base |
| **#75** | (34, 50) | 16 x 16 px | `object_075.png` | Bullet Bill Cannon - top, barrel, base |
| **#76** | (51, 50) | 16 x 16 px | `object_076.png` | Bullet Bill Cannon - top, barrel, base |
| **#77** | (68, 50) | 16 x 16 px | `object_077.png` | Trampoline / Spring - (owned by an Entity) |
| **#79** | (147, 50) | 16 x 16 px | `object_079.png` | Flag Pole - Flag (scaled down) |
| **#80** | (164, 50) | 16 x 16 px | `object_080.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#81** | (181, 50) | 16 x 16 px | `object_081.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#82** | (198, 50) | 16 x 16 px | `object_082.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#83** | (215, 50) | 16 x 16 px | `object_083.png` | Water Surface |
| **#85** | (0, 67) | 16 x 16 px | `object_085.png` | Question/Brick Block (animation frames - blinking) |
| **#86** | (17, 67) | 16 x 16 px | `object_086.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#87** | (34, 67) | 16 x 16 px | `object_087.png` | Flag Pole - Pole Shaft (tileable) |
| **#88** | (51, 67) | 16 x 16 px | `object_088.png` | Flag Pole Shaft |
| **#89** | (68, 67) | 16 x 16 px | `object_089.png` | Question/Brick Block (animation frames - blinking) |
| **#90** | (147, 67) | 16 x 16 px | `object_090.png` | Question/Brick Block (animation frames - blinking) |
| **#91** | (164, 67) | 16 x 16 px | `object_091.png` | Question/Brick Block (animation frames - blinking) |
| **#92** | (181, 67) | 16 x 16 px | `object_092.png` | Flag Pole - Pole Shaft (tileable) |
| **#93** | (198, 67) | 16 x 16 px | `object_093.png` | Solid Filler - sky/water background blue |
| **#94** | (215, 67) | 16 x 16 px | `object_094.png` | Used / Empty Block |
| **#95** | (298, 78) | 16 x 16 px | `object_095.png` | Question Block - Underground |
| **#96** | (315, 78) | 16 x 16 px | `object_096.png` | Question/Brick Block (animation frames - blinking) |
| **#97** | (332, 78) | 16 x 16 px | `object_097.png` | Question/Brick Block (animation frames - blinking) |
| **#98** | (349, 78) | 16 x 16 px | `object_098.png` | Question/Brick Block (animation frames - blinking) |
| **#99** | (394, 78) | 16 x 16 px | `object_099.png` | Question/Brick Block (animation frames - blinking) |
| **#100** | (411, 78) | 16 x 16 px | `object_100.png` | Used / Empty Block |
| **#101** | (428, 78) | 16 x 16 px | `object_101.png` | Question/Brick Block (animation frames - blinking) |
| **#102** | (445, 78) | 16 x 16 px | `object_102.png` | Question/Brick Block (animation frames - blinking) |
| **#103** | (490, 78) | 16 x 16 px | `object_103.png` | Question/Brick Block (animation frames - blinking) |
| **#104** | (507, 78) | 16 x 16 px | `object_104.png` | Used / Empty Block |
| **#105** | (524, 78) | 16 x 16 px | `object_105.png` | Static Coin (Map Coin) - Overworld ground (different from the block popup coin) |
| **#106** | (541, 78) | 16 x 16 px | `object_106.png` | Static Coin (Map Coin) - Overworld ground (different from the block popup coin) |
| **#107** | (586, 78) | 16 x 16 px | `object_107.png` | Static Coin (Map Coin) - Overworld ground (different from the block popup coin) |
| **#108** | (603, 78) | 16 x 16 px | `object_108.png` | Static Coin (Map Coin) - Overworld ground (different from the block popup coin) |
| **#109** | (620, 78) | 16 x 16 px | `object_109.png` | Static Coin (Map Coin) - Overworld ground (different from the block popup coin) |
| **#110** | (637, 78) | 16 x 16 px | `object_110.png` | Static Coin (Map Coin) - Overworld ground (different from the block popup coin) |
| **#112** | (298, 95) | 16 x 16 px | `object_112.png` | Static Coin (Map Coin) - Overworld ground (different from the block popup coin) |
| **#113** | (315, 95) | 16 x 16 px | `object_113.png` | Static Coin (Map Coin) - Overworld ground (different from the block popup coin) |
| **#114** | (332, 95) | 16 x 16 px | `object_114.png` | Castle - Wall Brick |
| **#115** | (394, 95) | 16 x 16 px | `object_115.png` | Castle - Wall Brick |
| **#116** | (411, 95) | 16 x 16 px | `object_116.png` | Castle - Wall Brick |
| **#117** | (428, 95) | 16 x 16 px | `object_117.png` | Used / Empty Block |
| **#118** | (490, 95) | 16 x 16 px | `object_118.png` | Castle - Battlement |
| **#119** | (507, 95) | 16 x 16 px | `object_119.png` | Castle - Battlement |
| **#120** | (524, 95) | 16 x 16 px | `object_120.png` | Used / Empty Block |
| **#121** | (586, 95) | 16 x 16 px | `object_121.png` | Castle - Door Top |
| **#122** | (603, 95) | 16 x 16 px | `object_122.png` | Static Coin (Map Coin) - cave / castle (black background) |
| **#123** | (620, 95) | 16 x 16 px | `object_123.png` | Static Coin (Map Coin) - cave / castle (black background) |
| **#124** | (0, 100) | 16 x 16 px | `object_124.png` | Static Coin (Map Coin) - cave / castle (black background) |
| **#125** | (17, 100) | 16 x 16 px | `object_125.png` | Static Coin (Map Coin) - cave / castle (black background) |
| **#126** | (34, 100) | 16 x 16 px | `object_126.png` | Static Coin (Map Coin) - cave / castle (black background) |
| **#127** | (51, 100) | 16 x 16 px | `object_127.png` | Static Coin (Map Coin) - cave / castle (black background) |
| **#128** | (68, 100) | 16 x 16 px | `object_128.png` | Castle - Black Window Cell |
| **#129** | (85, 100) | 16 x 16 px | `object_129.png` | Castle - Door Top |
| **#130** | (102, 100) | 16 x 16 px | `object_130.png` | Castle - Gray Brick Wall |
| **#131** | (119, 100) | 16 x 16 px | `object_131.png` | Castle - Door Bottom |
| **#132** | (147, 100) | 16 x 16 px | `object_132.png` | Static Coin (Map Coin) - cave / castle (black background, dark frame) |
| **#133** | (164, 100) | 16 x 16 px | `object_133.png` | Static Coin (Map Coin) - cave / castle (black background, dark frame) |
| **#134** | (181, 100) | 16 x 16 px | `object_134.png` | Static Coin (Map Coin) - cave / castle (black background, dark frame) |
| **#135** | (198, 100) | 16 x 16 px | `object_135.png` | Static Coin (Map Coin) - cave / castle (black background, dark frame) |
| **#136** | (215, 100) | 16 x 16 px | `object_136.png` | Static Coin (Map Coin) - cave / castle (black background, dark frame) |
| **#137** | (232, 100) | 16 x 16 px | `object_137.png` | Static Coin (Map Coin) - cave / castle (black background, dark frame) |
| **#138** | (249, 100) | 16 x 16 px | `object_138.png` | Castle - Black Window Cell |
| **#139** | (266, 100) | 13 x 16 px | `object_139.png` | Castle - Gray Brick Wall |
| **#140** | (0, 117) | 16 x 16 px | `object_140.png` | Used / Empty Block |
| **#141** | (17, 117) | 16 x 16 px | `object_141.png` | Castle - Wall Brick |
| **#142** | (34, 117) | 16 x 16 px | `object_142.png` | Castle - Wall Brick |
| **#143** | (51, 117) | 16 x 16 px | `object_143.png` | Castle - Wall Brick |
| **#144** | (68, 117) | 16 x 16 px | `object_144.png` | Castle - Wall Brick |
| **#145** | (85, 117) | 16 x 16 px | `object_145.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#146** | (102, 117) | 16 x 16 px | `object_146.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#147** | (119, 117) | 16 x 16 px | `object_147.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#148** | (147, 117) | 16 x 16 px | `object_148.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#149** | (164, 117) | 16 x 16 px | `object_149.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#150** | (181, 117) | 16 x 16 px | `object_150.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#151** | (198, 117) | 16 x 16 px | `object_151.png` | Castle - Wall Brick |
| **#152** | (215, 117) | 16 x 16 px | `object_152.png` | Castle - Wall Brick |
| **#153** | (232, 117) | 16 x 16 px | `object_153.png` | Castle - Wall Brick |
| **#154** | (249, 117) | 16 x 16 px | `object_154.png` | Solid Filler - black (used to line under dungeons/doors) |
| **#155** | (266, 117) | 13 x 16 px | `object_155.png` | Skull Block - Castle |
| **#156** | (298, 112) | 16 x 16 px | `object_156.png` | Static Coin (Map Coin) - orange/brown background variant |
| **#157** | (315, 112) | 16 x 16 px | `object_157.png` | Static Coin (Map Coin) - orange/brown background variant |
| **#158** | (332, 112) | 16 x 16 px | `object_158.png` | Static Coin (Map Coin) - orange/brown background variant |
| **#159** | (394, 112) | 16 x 16 px | `object_159.png` | Static Coin (Map Coin) - orange/brown background variant |
| **#160** | (411, 112) | 16 x 16 px | `object_160.png` | Static Coin (Map Coin) - orange/brown background variant |
| **#161** | (428, 112) | 16 x 16 px | `object_161.png` | Static Coin (Map Coin) - orange/brown background variant |
| **#162** | (490, 112) | 16 x 16 px | `object_162.png` | Static Coin (Map Coin) - orange/brown background variant |
| **#163** | (507, 112) | 16 x 16 px | `object_163.png` | Static Coin (Map Coin) - orange/brown background variant |
| **#164** | (524, 112) | 16 x 16 px | `object_164.png` | Static Coin (Map Coin) - orange/brown background variant |
| **#165** | (586, 112) | 16 x 16 px | `object_165.png` | Static Coin (Map Coin) - orange/brown background variant |
| **#166** | (603, 112) | 16 x 16 px | `object_166.png` | Static Coin (Map Coin) - orange/brown background variant |
| **#167** | (620, 112) | 16 x 16 px | `object_167.png` | Star Item - (note: already provided by the Item/Entity) |
| **#168** | (0, 134) | 16 x 16 px | `object_168.png` | Castle Bridge Chain - (owned by an Entity) |
| **#169** | (17, 134) | 16 x 16 px | `object_169.png` | Trampoline / Spring - (owned by an Entity) |
| **#170** | (34, 134) | 16 x 16 px | `object_170.png` | Trampoline / Spring - (owned by an Entity) |
| **#171** | (51, 134) | 16 x 16 px | `object_171.png` | Water Surface - animation frame |
| **#172** | (68, 134) | 16 x 16 px | `object_172.png` | Water Surface - animation frame |
| **#174** | (147, 134) | 16 x 16 px | `object_174.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#175** | (164, 134) | 16 x 16 px | `object_175.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#176** | (181, 134) | 16 x 16 px | `object_176.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#177** | (198, 134) | 16 x 16 px | `object_177.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#178** | (215, 134) | 16 x 16 px | `object_178.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#180** | (298, 129) | 16 x 16 px | `object_180.png` | Bowser Axe (level-end trigger) - (owned by an Entity) |
| **#181** | (315, 129) | 16 x 16 px | `object_181.png` | Bowser Axe (level-end trigger) - (owned by an Entity) |
| **#182** | (332, 129) | 16 x 16 px | `object_182.png` | Bowser Axe (level-end trigger) - (owned by an Entity) |
| **#183** | (394, 129) | 16 x 16 px | `object_183.png` | Bowser Axe (level-end trigger) - (owned by an Entity) |
| **#184** | (411, 129) | 16 x 16 px | `object_184.png` | Bowser Axe (level-end trigger) - (owned by an Entity) |
| **#185** | (428, 129) | 16 x 16 px | `object_185.png` | Bowser Axe (level-end trigger) - (owned by an Entity) |
| **#186** | (490, 129) | 16 x 16 px | `object_186.png` | Bowser Axe (level-end trigger) - (owned by an Entity) |
| **#187** | (507, 129) | 16 x 16 px | `object_187.png` | Flag Pole - Pole Shaft (tileable) |
| **#188** | (524, 129) | 16 x 16 px | `object_188.png` | Bowser Axe (level-end trigger) - (owned by an Entity) |
| **#189** | (586, 129) | 16 x 16 px | `object_189.png` | Bowser Axe (level-end trigger) - (owned by an Entity) |
| **#190** | (603, 129) | 16 x 16 px | `object_190.png` | Bowser Axe (level-end trigger) - (owned by an Entity) |
| **#191** | (620, 129) | 16 x 16 px | `object_191.png` | Bridge / Castle Platform |
| **#192** | (0, 151) | 16 x 16 px | `object_192.png` | Bridge / Castle Platform |
| **#193** | (17, 151) | 16 x 16 px | `object_193.png` | Bridge / Castle Platform |
| **#194** | (34, 151) | 16 x 16 px | `object_194.png` | Environment Tile / Building Block (Tilemap Component) |
| **#195** | (51, 151) | 16 x 16 px | `object_195.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#196** | (68, 151) | 16 x 16 px | `object_196.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#197** | (147, 151) | 16 x 16 px | `object_197.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#198** | (164, 151) | 16 x 16 px | `object_198.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#199** | (181, 151) | 16 x 16 px | `object_199.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#200** | (198, 151) | 16 x 16 px | `object_200.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#201** | (215, 151) | 16 x 16 px | `object_201.png` | Solid Filler - pale green |
| **#202** | (298, 146) | 16 x 16 px | `object_202.png` | Elevator / Lift Pulley - flying platform system |
| **#203** | (315, 146) | 16 x 16 px | `object_203.png` | Elevator / Lift Pulley - flying platform system |
| **#204** | (332, 146) | 16 x 16 px | `object_204.png` | Elevator / Lift Pulley - flying platform system |
| **#205** | (394, 146) | 16 x 16 px | `object_205.png` | Elevator / Lift Pulley - flying platform system |
| **#206** | (411, 146) | 16 x 16 px | `object_206.png` | Elevator / Lift Pulley - flying platform system |
| **#207** | (428, 146) | 16 x 16 px | `object_207.png` | Elevator / Lift Pulley - flying platform system |
| **#208** | (490, 146) | 16 x 16 px | `object_208.png` | Elevator / Lift Pulley - flying platform system |
| **#209** | (507, 146) | 16 x 16 px | `object_209.png` | Elevator / Lift Pulley - flying platform system |
| **#210** | (524, 146) | 16 x 16 px | `object_210.png` | Elevator / Lift Pulley - flying platform system |
| **#211** | (586, 146) | 16 x 16 px | `object_211.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#212** | (603, 146) | 16 x 16 px | `object_212.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#213** | (620, 146) | 16 x 16 px | `object_213.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#214** | (0, 196) | 16 x 16 px | `object_214.png` | Green Hill / Slopes - hillside and crest parts |
| **#215** | (17, 196) | 16 x 16 px | `object_215.png` | Green Hill / Slopes - hillside and crest parts |
| **#216** | (34, 196) | 16 x 16 px | `object_216.png` | Green Hill / Slopes - hillside and crest parts |
| **#217** | (51, 196) | 16 x 16 px | `object_217.png` | Green Hill / Slopes - hillside and crest parts |
| **#218** | (68, 196) | 16 x 16 px | `object_218.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#219** | (85, 196) | 16 x 16 px | `object_219.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#220** | (102, 196) | 16 x 16 px | `object_220.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#221** | (119, 196) | 16 x 16 px | `object_221.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#222** | (136, 196) | 16 x 16 px | `object_222.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#223** | (164, 196) | 16 x 16 px | `object_223.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#224** | (181, 196) | 16 x 16 px | `object_224.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#225** | (198, 196) | 16 x 16 px | `object_225.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#226** | (215, 196) | 16 x 16 px | `object_226.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#227** | (232, 196) | 16 x 16 px | `object_227.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#228** | (249, 196) | 16 x 16 px | `object_228.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#229** | (266, 196) | 16 x 16 px | `object_229.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#230** | (283, 196) | 16 x 16 px | `object_230.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#231** | (300, 196) | 16 x 16 px | `object_231.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#232** | (0, 213) | 16 x 16 px | `object_232.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#233** | (17, 213) | 16 x 16 px | `object_233.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#234** | (34, 213) | 16 x 16 px | `object_234.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#235** | (51, 213) | 16 x 16 px | `object_235.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#236** | (85, 213) | 16 x 16 px | `object_236.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#237** | (102, 213) | 16 x 16 px | `object_237.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#238** | (119, 213) | 16 x 16 px | `object_238.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#239** | (136, 213) | 16 x 16 px | `object_239.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#240** | (164, 213) | 16 x 16 px | `object_240.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#241** | (181, 213) | 16 x 16 px | `object_241.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#242** | (198, 213) | 16 x 16 px | `object_242.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#243** | (215, 213) | 16 x 16 px | `object_243.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#244** | (249, 213) | 16 x 16 px | `object_244.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#245** | (266, 213) | 16 x 16 px | `object_245.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#246** | (283, 213) | 16 x 16 px | `object_246.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#247** | (300, 213) | 16 x 16 px | `object_247.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#248** | (0, 230) | 16 x 16 px | `object_248.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#249** | (17, 230) | 16 x 16 px | `object_249.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#250** | (34, 230) | 16 x 16 px | `object_250.png` | Pipe - rim, barrel and base pieces (green/white) |
| **#251** | (85, 230) | 16 x 16 px | `object_251.png` | White Slopes / Mushroom parts |
| **#252** | (102, 230) | 16 x 16 px | `object_252.png` | White Slopes / Mushroom parts |
| **#253** | (119, 230) | 16 x 16 px | `object_253.png` | White Slopes / Mushroom parts |
| **#254** | (136, 230) | 16 x 16 px | `object_254.png` | White Slopes / Mushroom parts |
| **#255** | (164, 230) | 16 x 16 px | `object_255.png` | White Slopes / Mushroom parts |
| **#256** | (181, 230) | 16 x 16 px | `object_256.png` | White Slopes / Mushroom parts |
| **#257** | (198, 230) | 16 x 16 px | `object_257.png` | White Slopes / Mushroom parts |
| **#258** | (249, 230) | 16 x 16 px | `object_258.png` | White Slopes / Mushroom parts |
| **#259** | (266, 230) | 16 x 16 px | `object_259.png` | White Slopes / Mushroom parts |
| **#260** | (283, 230) | 16 x 16 px | `object_260.png` | White Slopes / Mushroom parts |
| **#261** | (300, 230) | 16 x 16 px | `object_261.png` | White Slopes / Mushroom parts |
| **#262** | (0, 247) | 16 x 16 px | `object_262.png` | White Slopes / Mushroom parts |
| **#263** | (17, 247) | 16 x 16 px | `object_263.png` | White Slopes / Mushroom parts |
| **#264** | (34, 247) | 16 x 16 px | `object_264.png` | White Slopes / Mushroom parts |
| **#265** | (51, 247) | 16 x 16 px | `object_265.png` | White Slopes / Mushroom parts |
| **#266** | (68, 247) | 16 x 16 px | `object_266.png` | White Slopes / Mushroom parts |
| **#267** | (85, 247) | 16 x 16 px | `object_267.png` | White Slopes / Mushroom parts |
| **#268** | (102, 247) | 16 x 16 px | `object_268.png` | White Slopes / Mushroom parts |
| **#269** | (119, 247) | 16 x 16 px | `object_269.png` | White Slopes / Mushroom parts |
| **#270** | (136, 247) | 16 x 16 px | `object_270.png` | White Slopes / Mushroom parts |
| **#271** | (164, 247) | 16 x 16 px | `object_271.png` | White Slopes / Mushroom parts |
| **#272** | (181, 247) | 16 x 16 px | `object_272.png` | White Slopes / Mushroom parts |
| **#273** | (198, 247) | 16 x 16 px | `object_273.png` | White Slopes / Mushroom parts |
| **#274** | (215, 247) | 16 x 16 px | `object_274.png` | White Slopes / Mushroom parts |
| **#275** | (232, 247) | 16 x 16 px | `object_275.png` | White Slopes / Mushroom parts |
| **#276** | (249, 247) | 16 x 16 px | `object_276.png` | White Slopes / Mushroom parts |
| **#277** | (266, 247) | 16 x 16 px | `object_277.png` | White Slopes / Mushroom parts |
| **#278** | (283, 247) | 16 x 16 px | `object_278.png` | White Slopes / Mushroom parts |
| **#279** | (300, 247) | 16 x 16 px | `object_279.png` | White Slopes / Mushroom parts |
| **#280** | (0, 280) | 16 x 16 px | `object_280.png` | White Slopes / Mushroom parts |
| **#281** | (17, 280) | 16 x 16 px | `object_281.png` | White Slopes / Mushroom parts |
| **#282** | (34, 280) | 16 x 16 px | `object_282.png` | White Slopes / Mushroom parts |
| **#283** | (51, 280) | 16 x 16 px | `object_283.png` | White Slopes / Mushroom parts |
| **#284** | (68, 280) | 16 x 16 px | `object_284.png` | White Slopes / Mushroom parts |
| **#285** | (85, 280) | 16 x 16 px | `object_285.png` | White Mushroom Platform Parts |
| **#286** | (102, 280) | 16 x 16 px | `object_286.png` | White Mushroom Platform Parts |
| **#287** | (119, 280) | 16 x 16 px | `object_287.png` | White Mushroom Platform Parts |
| **#288** | (136, 280) | 16 x 16 px | `object_288.png` | White Mushroom Platform Parts |
| **#289** | (164, 280) | 16 x 16 px | `object_289.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#290** | (181, 280) | 16 x 16 px | `object_290.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#291** | (198, 280) | 16 x 16 px | `object_291.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#292** | (215, 280) | 16 x 16 px | `object_292.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#293** | (232, 280) | 16 x 16 px | `object_293.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#294** | (249, 280) | 16 x 16 px | `object_294.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#295** | (266, 280) | 16 x 16 px | `object_295.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#296** | (283, 280) | 16 x 16 px | `object_296.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#297** | (300, 280) | 16 x 16 px | `object_297.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#298** | (0, 297) | 16 x 16 px | `object_298.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#299** | (17, 297) | 16 x 16 px | `object_299.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#300** | (34, 297) | 16 x 16 px | `object_300.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#301** | (51, 297) | 16 x 16 px | `object_301.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#302** | (85, 297) | 16 x 16 px | `object_302.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#303** | (102, 297) | 16 x 16 px | `object_303.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#304** | (119, 297) | 16 x 16 px | `object_304.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#305** | (136, 297) | 16 x 16 px | `object_305.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#306** | (164, 297) | 16 x 16 px | `object_306.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#307** | (181, 297) | 16 x 16 px | `object_307.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#308** | (198, 297) | 16 x 16 px | `object_308.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#309** | (215, 297) | 16 x 16 px | `object_309.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#310** | (249, 297) | 16 x 16 px | `object_310.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#311** | (266, 297) | 16 x 16 px | `object_311.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#312** | (283, 297) | 16 x 16 px | `object_312.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#313** | (300, 297) | 16 x 16 px | `object_313.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#314** | (0, 314) | 16 x 16 px | `object_314.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#315** | (17, 314) | 16 x 16 px | `object_315.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#316** | (34, 314) | 16 x 16 px | `object_316.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#317** | (85, 314) | 16 x 16 px | `object_317.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#318** | (102, 314) | 16 x 16 px | `object_318.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#319** | (119, 314) | 16 x 16 px | `object_319.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#320** | (136, 314) | 16 x 16 px | `object_320.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#321** | (164, 314) | 16 x 16 px | `object_321.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#322** | (181, 314) | 16 x 16 px | `object_322.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#323** | (198, 314) | 16 x 16 px | `object_323.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#324** | (249, 314) | 16 x 16 px | `object_324.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#325** | (266, 314) | 16 x 16 px | `object_325.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#326** | (283, 314) | 16 x 16 px | `object_326.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#327** | (300, 314) | 16 x 16 px | `object_327.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#328** | (0, 331) | 16 x 16 px | `object_328.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#329** | (17, 331) | 16 x 16 px | `object_329.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#330** | (34, 331) | 16 x 16 px | `object_330.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#331** | (51, 331) | 16 x 16 px | `object_331.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#332** | (68, 331) | 16 x 16 px | `object_332.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#333** | (85, 331) | 16 x 16 px | `object_333.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#334** | (102, 331) | 16 x 16 px | `object_334.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#335** | (119, 331) | 16 x 16 px | `object_335.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#336** | (136, 331) | 16 x 16 px | `object_336.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#337** | (164, 331) | 16 x 16 px | `object_337.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#338** | (181, 331) | 16 x 16 px | `object_338.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#339** | (198, 331) | 16 x 16 px | `object_339.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#340** | (215, 331) | 16 x 16 px | `object_340.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#341** | (232, 331) | 16 x 16 px | `object_341.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#342** | (249, 331) | 16 x 16 px | `object_342.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#343** | (266, 331) | 16 x 16 px | `object_343.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#344** | (283, 331) | 16 x 16 px | `object_344.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |
| **#345** | (300, 331) | 16 x 16 px | `object_345.png` | Peach/Toad Room - wall/background decoration (white, red, purple) |

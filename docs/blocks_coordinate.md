# Brick Block & Question Block Coordinate and Meaning Table (`blocks_coordinate.md`)

List of all **120** connected block components (`Block Spritesheet`) extracted directly from the source file **`assets/textures/items/items_blocks.png`** (size `448 x 256 px`), analyzed with a connected-component (flood-fill) algorithm and matched to the standard `16 x 16 px` grid cells.

> **OFFICIAL DATA SOURCE:**
> This document (`blocks_coordinate.md`) and the illustration Atlas image **`docs/assets/reference/blocks_all_components_atlas_full.png`** were analyzed and built from the original image file **`assets/textures/items/items_blocks.png`** (containing all Brick Blocks, Question Blocks `?`, Brick Debris fragments, and the 4 NES environment palettes: Overworld, Underground, Castle, Underwater).

> **Explanation of the two coordinate/size column pairs:**
> - **Flood-fill coords (X, Y) & Flood size (W x H)**: the tight bounding box around the block's actual pixels, as found by the flood-fill algorithm.
> - **Grid cell coords (X, Y) & Grid size (W x H)**: the standard cell frame on the spritesheet used by the game engine (`sf::IntRect(x, y, 16, 16)`).

> **NES palette conventions:**
> - **OW (Overworld)**: outdoor / ground palette (Levels 1-1, 1-3, 3-1...) — orange/brown bricks, `?` blocks with black border and orange glyph.
> - **UG (Underground)**: underground / cave palette (Levels 1-2, 4-2...) — teal bricks, blue `?` blocks.
> - **Castle (CT)**: castle palette (Levels 1-4, 2-4...) — gray stone bricks, light gray/brown `?` blocks.
> - **UW (Underwater)**: underwater palette (Levels 2-2, 7-2...) — bright green bricks, `?` blocks with blue border.

---

## I. Overall Spritesheet Layout Analysis (`items_blocks.png`)

The `items_blocks.png` spritesheet measures **448 x 256 pixels** and is divided into 2 main horizontal regions, laid out in a visual structure that includes both an **information/label region** and a **game object (block) region**:

### 1. Information & Guide Regions
- **Main Headers (y ~ 0-32)**:
  - `Coin Blocks` (X: 80-240): title text for the question / coin-containing blocks (`?` Question Blocks).
  - `Brick Blocks` (X: 272-432): title text for the brick blocks.
  - Environment split below: `Overworld / Underwater` and `Underground / Castle`.
- **Vertical Column Labels (y ~ 48-104)**:
  - Vertically rotated text explaining each column's function: `Initial Tile` (initial frame), `Hit (empty)` (bumped empty), `Brick debris` (broken brick fragments), `Hit` (bump that pops an item), `After Hit` (empty cell after bumping).
- **Palette Row Labels (X ~ 0-64 & X ~ 192-256)**:
  - Name the 4 NES palettes corresponding to the 4 sprite rows: `OW Palette`, `UG Palette`, `Castle Palette`, `UW Palette`.

### 2. Game Block Regions
- **Question Block Region (`Coin Blocks / Question Blocks`) (y = 112-175, X = 80-255)**:
  - 40 sprite cells of 16x16 px containing 3 blinking `?` animation frames, 1 block-bump `Hit` frame, and 1 opened `After Hit` (Empty Block) frame.
- **Brick Block Region (Set 1 — Upper `Brick Blocks`) (y = 112-175, X = 272-447)**:
  - 40 sprite cells of 16x16 px representing regular bricks, bumping bricks, 4-corner brick fragments, and empty bricks.
- **Brick Block Region (Set 2 — Lower `Brick Blocks`) (y = 192-255, X = 272-447)**:
  - 40 sprite cells of 16x16 px holding supplementary palette variants of the brick types for special environments.

---

## II. Question Block Coordinate Table (`Coin Blocks / Question Blocks`)

`?` blocks that pop coins, mushrooms, fire flowers, or invincibility stars. Each type has 3 blinking animation frames + 1 bump frame (`Hit`) + 1 empty-cell frame (`After Hit / Used Block`).

### 1. Overworld & Underwater Group (y = 112-175, X = 80-159)

| # | Flood-fill coords (X, Y) | Flood size | Grid cell coords | Grid size | Pixel count | Palette | Meaning / frame state |
|-----|-----------------------|------------------|----------------|-----------------|----------|---------|----------------------------------|
| **#1** | `(80, 112)` | `16 x 16 px` | `(80, 112)` | `16 x 16 px` | 254 px | OW Palette | Initial Tile (Frame 1) — '?' question block in idle/blinking state — Frame 1 |
| **#2** | `(96, 112)` | `16 x 16 px` | `(96, 112)` | `16 x 16 px` | 254 px | OW Palette | Initial Tile (Frame 2) — '?' question block in idle/blinking state — Frame 2 |
| **#3** | `(112, 112)` | `16 x 16 px` | `(112, 112)` | `16 x 16 px` | 254 px | OW Palette | Initial Tile (Frame 3) — '?' question block in idle/blinking state — Frame 3 |
| **#4** | `(128, 112)` | `16 x 16 px` | `(128, 112)` | `16 x 16 px` | 252 px | OW Palette | Hit — '?' block bouncing up when Mario headbutts it from below (currently popping a mushroom/coin) |
| **#5** | `(144, 112)` | `16 x 16 px` | `(144, 112)` | `16 x 16 px` | 252 px | OW Palette | After Hit — opened block state (Empty / Used Block) — becomes a flat brown cell |
| **#11** | `(80, 128)` | `16 x 16 px` | `(80, 128)` | `16 x 16 px` | 254 px | UG Palette | Initial Tile (Frame 1) — '?' question block in idle/blinking state — Frame 1 |
| **#12** | `(96, 128)` | `16 x 16 px` | `(96, 128)` | `16 x 16 px` | 254 px | UG Palette | Initial Tile (Frame 2) — '?' question block in idle/blinking state — Frame 2 |
| **#13** | `(112, 128)` | `16 x 16 px` | `(112, 128)` | `16 x 16 px` | 254 px | UG Palette | Initial Tile (Frame 3) — '?' question block in idle/blinking state — Frame 3 |
| **#14** | `(128, 128)` | `16 x 16 px` | `(128, 128)` | `16 x 16 px` | 252 px | UG Palette | Hit — '?' block bouncing up when Mario headbutts it from below (currently popping a mushroom/coin) |
| **#15** | `(144, 128)` | `16 x 16 px` | `(144, 128)` | `16 x 16 px` | 252 px | UG Palette | After Hit — opened block state (Empty / Used Block) — becomes a flat brown cell |
| **#21** | `(80, 144)` | `16 x 16 px` | `(80, 144)` | `16 x 16 px` | 254 px | Castle Palette | Initial Tile (Frame 1) — '?' question block in idle/blinking state — Frame 1 |
| **#22** | `(96, 144)` | `16 x 16 px` | `(96, 144)` | `16 x 16 px` | 254 px | Castle Palette | Initial Tile (Frame 2) — '?' question block in idle/blinking state — Frame 2 |
| **#23** | `(112, 144)` | `16 x 16 px` | `(112, 144)` | `16 x 16 px` | 254 px | Castle Palette | Initial Tile (Frame 3) — '?' question block in idle/blinking state — Frame 3 |
| **#24** | `(128, 144)` | `16 x 16 px` | `(128, 144)` | `16 x 16 px` | 252 px | Castle Palette | Hit — '?' block bouncing up when Mario headbutts it from below (currently popping a mushroom/coin) |
| **#25** | `(144, 144)` | `16 x 16 px` | `(144, 144)` | `16 x 16 px` | 252 px | Castle Palette | After Hit — opened block state (Empty / Used Block) — becomes a flat brown cell |
| **#31** | `(80, 160)` | `16 x 16 px` | `(80, 160)` | `16 x 16 px` | 254 px | UW Palette | Initial Tile (Frame 1) — '?' question block in idle/blinking state — Frame 1 |
| **#32** | `(96, 160)` | `16 x 16 px` | `(96, 160)` | `16 x 16 px` | 254 px | UW Palette | Initial Tile (Frame 2) — '?' question block in idle/blinking state — Frame 2 |
| **#33** | `(112, 160)` | `16 x 16 px` | `(112, 160)` | `16 x 16 px` | 254 px | UW Palette | Initial Tile (Frame 3) — '?' question block in idle/blinking state — Frame 3 |
| **#34** | `(128, 160)` | `16 x 16 px` | `(128, 160)` | `16 x 16 px` | 252 px | UW Palette | Hit — '?' block bouncing up when Mario headbutts it from below (currently popping a mushroom/coin) |
| **#35** | `(144, 160)` | `16 x 16 px` | `(144, 160)` | `16 x 16 px` | 252 px | UW Palette | After Hit — opened block state (Empty / Used Block) — becomes a flat brown cell |

### 2. Underground & Castle Group (y = 112-175, X = 176-255)

| # | Flood-fill coords (X, Y) | Flood size | Grid cell coords | Grid size | Pixel count | Palette | Meaning / frame state |
|-----|-----------------------|------------------|----------------|-----------------|----------|---------|----------------------------------|
| **#6** | `(176, 112)` | `16 x 16 px` | `(176, 112)` | `16 x 16 px` | 254 px | OW Palette | Initial Tile (Frame 1) — '?' question block in idle/blinking state — Frame 1 |
| **#7** | `(192, 112)` | `16 x 16 px` | `(192, 112)` | `16 x 16 px` | 254 px | OW Palette | Initial Tile (Frame 2) — '?' question block in idle/blinking state — Frame 2 |
| **#8** | `(208, 112)` | `16 x 16 px` | `(208, 112)` | `16 x 16 px` | 254 px | OW Palette | Initial Tile (Frame 3) — '?' question block in idle/blinking state — Frame 3 |
| **#9** | `(224, 112)` | `16 x 16 px` | `(224, 112)` | `16 x 16 px` | 252 px | OW Palette | Hit — '?' block bouncing up when Mario headbutts it from below (currently popping a mushroom/coin) |
| **#10** | `(240, 112)` | `16 x 16 px` | `(240, 112)` | `16 x 16 px` | 252 px | OW Palette | After Hit — opened block state (Empty / Used Block) — becomes a flat brown cell |
| **#16** | `(176, 128)` | `16 x 16 px` | `(176, 128)` | `16 x 16 px` | 254 px | UG Palette | Initial Tile (Frame 1) — '?' question block in idle/blinking state — Frame 1 |
| **#17** | `(192, 128)` | `16 x 16 px` | `(192, 128)` | `16 x 16 px` | 254 px | UG Palette | Initial Tile (Frame 2) — '?' question block in idle/blinking state — Frame 2 |
| **#18** | `(208, 128)` | `16 x 16 px` | `(208, 128)` | `16 x 16 px` | 254 px | UG Palette | Initial Tile (Frame 3) — '?' question block in idle/blinking state — Frame 3 |
| **#19** | `(224, 128)` | `16 x 16 px` | `(224, 128)` | `16 x 16 px` | 252 px | UG Palette | Hit — '?' block bouncing up when Mario headbutts it from below (currently popping a mushroom/coin) |
| **#20** | `(240, 128)` | `16 x 16 px` | `(240, 128)` | `16 x 16 px` | 252 px | UG Palette | After Hit — opened block state (Empty / Used Block) — becomes a flat brown cell |
| **#26** | `(176, 144)` | `16 x 16 px` | `(176, 144)` | `16 x 16 px` | 254 px | Castle Palette | Initial Tile (Frame 1) — '?' question block in idle/blinking state — Frame 1 |
| **#27** | `(192, 144)` | `16 x 16 px` | `(192, 144)` | `16 x 16 px` | 254 px | Castle Palette | Initial Tile (Frame 2) — '?' question block in idle/blinking state — Frame 2 |
| **#28** | `(208, 144)` | `16 x 16 px` | `(208, 144)` | `16 x 16 px` | 254 px | Castle Palette | Initial Tile (Frame 3) — '?' question block in idle/blinking state — Frame 3 |
| **#29** | `(224, 144)` | `16 x 16 px` | `(224, 144)` | `16 x 16 px` | 252 px | Castle Palette | Hit — '?' block bouncing up when Mario headbutts it from below (currently popping a mushroom/coin) |
| **#30** | `(240, 144)` | `16 x 16 px` | `(240, 144)` | `16 x 16 px` | 252 px | Castle Palette | After Hit — opened block state (Empty / Used Block) — becomes a flat brown cell |
| **#36** | `(176, 160)` | `16 x 16 px` | `(176, 160)` | `16 x 16 px` | 254 px | UW Palette | Initial Tile (Frame 1) — '?' question block in idle/blinking state — Frame 1 |
| **#37** | `(192, 160)` | `16 x 16 px` | `(192, 160)` | `16 x 16 px` | 254 px | UW Palette | Initial Tile (Frame 2) — '?' question block in idle/blinking state — Frame 2 |
| **#38** | `(208, 160)` | `16 x 16 px` | `(208, 160)` | `16 x 16 px` | 254 px | UW Palette | Initial Tile (Frame 3) — '?' question block in idle/blinking state — Frame 3 |
| **#39** | `(224, 160)` | `16 x 16 px` | `(224, 160)` | `16 x 16 px` | 252 px | UW Palette | Hit — '?' block bouncing up when Mario headbutts it from below (currently popping a mushroom/coin) |
| **#40** | `(240, 160)` | `16 x 16 px` | `(240, 160)` | `16 x 16 px` | 252 px | UW Palette | After Hit — opened block state (Empty / Used Block) — becomes a flat brown cell |

---

## III. Brick Block Coordinate Table (Set 1 — Upper `Brick Blocks`)

Regular bricks can be smashed by Big Mario (producing 4 flying `Brick debris` fragments) or bumped upward when they contain items/coins.

> **SYSTEMATIC DEBRIS SPRITE CUTTING TECHNIQUE (`BlockDebris`):**
> The `Brick debris` cells on the spritesheet are drawn merged into a single **`16 x 16 px`** grid cell (containing 4 small quarter pieces).
> When programming the brick-break effect in the game engine, this `16 x 16 px` cell is **split into 4 independent `8 x 8 px` sub-rects** assigned to 4 fragment objects flying in 4 different directions:
> - **Top-Left**: `sf::IntRect(X, Y, 8, 8)`
> - **Top-Right**: `sf::IntRect(X + 8, Y, 8, 8)`
> - **Bottom-Left**: `sf::IntRect(X, Y + 8, 8, 8)`
> - **Bottom-Right**: `sf::IntRect(X + 8, Y + 8, 8, 8)`

### 1. Brick Set 1 — Overworld & Underwater (y = 112-175, X = 272-351)

| # | Flood-fill coords (X, Y) | Flood size | Grid cell coords | Grid size | Pixel count | Palette | Meaning / frame state |
|-----|-----------------------|------------------|----------------|-----------------|----------|---------|----------------------------------|
| **#41** | `(272, 112)` | `16 x 16 px` | `(272, 112)` | `16 x 16 px` | 256 px | OW Palette | Initial Tile — intact brick block, untouched |
| **#42** | `(288, 112)` | `16 x 16 px` | `(288, 112)` | `16 x 16 px` | 256 px | OW Palette | Hit (empty) — brick bounced up by Small Mario's headbutt (does not break, no contents) |
| **#43** | `(304, 112)` | `16 x 16 px` | `(304, 112)` | `16 x 16 px` | 184 px | OW Palette | Brick debris — 4 fragments flying to the 4 corners when Big Mario smashes the brick |
| **#44** | `(320, 112)` | `16 x 16 px` | `(320, 112)` | `16 x 16 px` | 252 px | OW Palette | Hit — brick bouncing up when bumped (contains an item/coins) |
| **#45** | `(336, 112)` | `16 x 16 px` | `(336, 112)` | `16 x 16 px` | 252 px | OW Palette | After Hit — brick after its coins/items are exhausted — becomes an empty cell |
| **#51** | `(272, 128)` | `16 x 16 px` | `(272, 128)` | `16 x 16 px` | 256 px | UG Palette | Initial Tile — intact brick block, untouched |
| **#52** | `(288, 128)` | `16 x 16 px` | `(288, 128)` | `16 x 16 px` | 256 px | UG Palette | Hit (empty) — brick bounced up by Small Mario's headbutt (does not break, no contents) |
| **#53** | `(304, 128)` | `16 x 16 px` | `(304, 128)` | `16 x 16 px` | 184 px | UG Palette | Brick debris — 4 fragments flying to the 4 corners when Big Mario smashes the brick |
| **#54** | `(320, 128)` | `16 x 16 px` | `(320, 128)` | `16 x 16 px` | 252 px | UG Palette | Hit — brick bouncing up when bumped (contains an item/coins) |
| **#55** | `(336, 128)` | `16 x 16 px` | `(336, 128)` | `16 x 16 px` | 252 px | UG Palette | After Hit — brick after its coins/items are exhausted — becomes an empty cell |
| **#61** | `(272, 144)` | `16 x 16 px` | `(272, 144)` | `16 x 16 px` | 256 px | Castle Palette | Initial Tile — intact brick block, untouched |
| **#62** | `(288, 144)` | `16 x 16 px` | `(288, 144)` | `16 x 16 px` | 256 px | Castle Palette | Hit (empty) — brick bounced up by Small Mario's headbutt (does not break, no contents) |
| **#63** | `(304, 144)` | `16 x 16 px` | `(304, 144)` | `16 x 16 px` | 184 px | Castle Palette | Brick debris — 4 fragments flying to the 4 corners when Big Mario smashes the brick |
| **#64** | `(320, 144)` | `16 x 16 px` | `(320, 144)` | `16 x 16 px` | 252 px | Castle Palette | Hit — brick bouncing up when bumped (contains an item/coins) |
| **#65** | `(336, 144)` | `16 x 16 px` | `(336, 144)` | `16 x 16 px` | 252 px | Castle Palette | After Hit — brick after its coins/items are exhausted — becomes an empty cell |
| **#71** | `(272, 160)` | `16 x 16 px` | `(272, 160)` | `16 x 16 px` | 256 px | UW Palette | Initial Tile — intact brick block, untouched |
| **#72** | `(288, 160)` | `16 x 16 px` | `(288, 160)` | `16 x 16 px` | 256 px | UW Palette | Hit (empty) — brick bounced up by Small Mario's headbutt (does not break, no contents) |
| **#73** | `(304, 160)` | `16 x 16 px` | `(304, 160)` | `16 x 16 px` | 184 px | UW Palette | Brick debris — 4 fragments flying to the 4 corners when Big Mario smashes the brick |
| **#74** | `(320, 160)` | `16 x 16 px` | `(320, 160)` | `16 x 16 px` | 252 px | UW Palette | Hit — brick bouncing up when bumped (contains an item/coins) |
| **#75** | `(336, 160)` | `16 x 16 px` | `(336, 160)` | `16 x 16 px` | 252 px | UW Palette | After Hit — brick after its coins/items are exhausted — becomes an empty cell |

### 2. Brick Set 1 — Underground & Castle (y = 112-175, X = 368-447)

| # | Flood-fill coords (X, Y) | Flood size | Grid cell coords | Grid size | Pixel count | Palette | Meaning / frame state |
|-----|-----------------------|------------------|----------------|-----------------|----------|---------|----------------------------------|
| **#46** | `(368, 112)` | `16 x 16 px` | `(368, 112)` | `16 x 16 px` | 256 px | OW Palette | Initial Tile — intact brick block, untouched |
| **#47** | `(384, 112)` | `16 x 16 px` | `(384, 112)` | `16 x 16 px` | 256 px | OW Palette | Hit (empty) — brick bounced up by Small Mario's headbutt (does not break, no contents) |
| **#48** | `(400, 112)` | `16 x 16 px` | `(400, 112)` | `16 x 16 px` | 184 px | OW Palette | Brick debris — 4 fragments flying to the 4 corners when Big Mario smashes the brick |
| **#49** | `(416, 112)` | `16 x 16 px` | `(416, 112)` | `16 x 16 px` | 252 px | OW Palette | Hit — brick bouncing up when bumped (contains an item/coins) |
| **#50** | `(432, 112)` | `16 x 16 px` | `(432, 112)` | `16 x 16 px` | 252 px | OW Palette | After Hit — brick after its coins/items are exhausted — becomes an empty cell |
| **#56** | `(368, 128)` | `16 x 16 px` | `(368, 128)` | `16 x 16 px` | 256 px | UG Palette | Initial Tile — intact brick block, untouched |
| **#57** | `(384, 128)` | `16 x 16 px` | `(384, 128)` | `16 x 16 px` | 256 px | UG Palette | Hit (empty) — brick bounced up by Small Mario's headbutt (does not break, no contents) |
| **#58** | `(400, 128)` | `16 x 16 px` | `(400, 128)` | `16 x 16 px` | 184 px | UG Palette | Brick debris — 4 fragments flying to the 4 corners when Big Mario smashes the brick |
| **#59** | `(416, 128)` | `16 x 16 px` | `(416, 128)` | `16 x 16 px` | 252 px | UG Palette | Hit — brick bouncing up when bumped (contains an item/coins) |
| **#60** | `(432, 128)` | `16 x 16 px` | `(432, 128)` | `16 x 16 px` | 252 px | UG Palette | After Hit — brick after its coins/items are exhausted — becomes an empty cell |
| **#66** | `(368, 144)` | `16 x 16 px` | `(368, 144)` | `16 x 16 px` | 256 px | Castle Palette | Initial Tile — intact brick block, untouched |
| **#67** | `(384, 144)` | `16 x 16 px` | `(384, 144)` | `16 x 16 px` | 256 px | Castle Palette | Hit (empty) — brick bounced up by Small Mario's headbutt (does not break, no contents) |
| **#68** | `(400, 144)` | `16 x 16 px` | `(400, 144)` | `16 x 16 px` | 184 px | Castle Palette | Brick debris — 4 fragments flying to the 4 corners when Big Mario smashes the brick |
| **#69** | `(416, 144)` | `16 x 16 px` | `(416, 144)` | `16 x 16 px` | 252 px | Castle Palette | Hit — brick bouncing up when bumped (contains an item/coins) |
| **#70** | `(432, 144)` | `16 x 16 px` | `(432, 144)` | `16 x 16 px` | 252 px | Castle Palette | After Hit — brick after its coins/items are exhausted — becomes an empty cell |
| **#76** | `(368, 160)` | `16 x 16 px` | `(368, 160)` | `16 x 16 px` | 256 px | UW Palette | Initial Tile — intact brick block, untouched |
| **#77** | `(384, 160)` | `16 x 16 px` | `(384, 160)` | `16 x 16 px` | 256 px | UW Palette | Hit (empty) — brick bounced up by Small Mario's headbutt (does not break, no contents) |
| **#78** | `(400, 160)` | `16 x 16 px` | `(400, 160)` | `16 x 16 px` | 184 px | UW Palette | Brick debris — 4 fragments flying to the 4 corners when Big Mario smashes the brick |
| **#79** | `(416, 160)` | `16 x 16 px` | `(416, 160)` | `16 x 16 px` | 252 px | UW Palette | Hit — brick bouncing up when bumped (contains an item/coins) |
| **#80** | `(432, 160)` | `16 x 16 px` | `(432, 160)` | `16 x 16 px` | 252 px | UW Palette | After Hit — brick after its coins/items are exhausted — becomes an empty cell |

---

## IV. Brick Block Coordinate Table (Set 2 — Lower `Brick Blocks`)

The lower-row brick set (y = 192-255) contains extended palette variants of bricks for special maps.

### 1. Brick Set 2 — Overworld & Underwater (y = 192-255, X = 272-351)

| # | Flood-fill coords (X, Y) | Flood size | Grid cell coords | Grid size | Pixel count | Palette | Meaning / frame state |
|-----|-----------------------|------------------|----------------|-----------------|----------|---------|----------------------------------|
| **#81** | `(272, 192)` | `16 x 16 px` | `(272, 192)` | `16 x 16 px` | 256 px | OW Palette | Initial Tile — intact brick block, untouched (extended palette 2) |
| **#82** | `(288, 192)` | `16 x 16 px` | `(288, 192)` | `16 x 16 px` | 256 px | OW Palette | Hit (empty) — brick bounced up by Small Mario's headbutt (extended palette 2) |
| **#83** | `(304, 192)` | `16 x 16 px` | `(304, 192)` | `16 x 16 px` | 184 px | OW Palette | Brick debris — 4 fragments flying out when the brick is smashed (extended palette 2) |
| **#84** | `(320, 192)` | `16 x 16 px` | `(320, 192)` | `16 x 16 px` | 252 px | OW Palette | Hit — brick bouncing up, containing an item (extended palette 2) |
| **#85** | `(336, 192)` | `16 x 16 px` | `(336, 192)` | `16 x 16 px` | 252 px | OW Palette | After Hit — brick cell after all items are popped (extended palette 2) |
| **#91** | `(272, 208)` | `16 x 16 px` | `(272, 208)` | `16 x 16 px` | 256 px | UG Palette | Initial Tile — intact brick block, untouched (extended palette 2) |
| **#92** | `(288, 208)` | `16 x 16 px` | `(288, 208)` | `16 x 16 px` | 256 px | UG Palette | Hit (empty) — brick bounced up by Small Mario's headbutt (extended palette 2) |
| **#93** | `(304, 208)` | `16 x 16 px` | `(304, 208)` | `16 x 16 px` | 184 px | UG Palette | Brick debris — 4 fragments flying out when the brick is smashed (extended palette 2) |
| **#94** | `(320, 208)` | `16 x 16 px` | `(320, 208)` | `16 x 16 px` | 252 px | UG Palette | Hit — brick bouncing up, containing an item (extended palette 2) |
| **#95** | `(336, 208)` | `16 x 16 px` | `(336, 208)` | `16 x 16 px` | 252 px | UG Palette | After Hit — brick cell after all items are popped (extended palette 2) |
| **#101** | `(272, 224)` | `16 x 16 px` | `(272, 224)` | `16 x 16 px` | 256 px | Castle Palette | Initial Tile — intact brick block, untouched (extended palette 2) |
| **#102** | `(288, 224)` | `16 x 16 px` | `(288, 224)` | `16 x 16 px` | 256 px | Castle Palette | Hit (empty) — brick bounced up by Small Mario's headbutt (extended palette 2) |
| **#103** | `(304, 224)` | `16 x 16 px` | `(304, 224)` | `16 x 16 px` | 184 px | Castle Palette | Brick debris — 4 fragments flying out when the brick is smashed (extended palette 2) |
| **#104** | `(320, 224)` | `16 x 16 px` | `(320, 224)` | `16 x 16 px` | 252 px | Castle Palette | Hit — brick bouncing up, containing an item (extended palette 2) |
| **#105** | `(336, 224)` | `16 x 16 px` | `(336, 224)` | `16 x 16 px` | 252 px | Castle Palette | After Hit — brick cell after all items are popped (extended palette 2) |
| **#111** | `(272, 240)` | `16 x 16 px` | `(272, 240)` | `16 x 16 px` | 256 px | UW Palette | Initial Tile — intact brick block, untouched (extended palette 2) |
| **#112** | `(288, 240)` | `16 x 16 px` | `(288, 240)` | `16 x 16 px` | 256 px | UW Palette | Hit (empty) — brick bounced up by Small Mario's headbutt (extended palette 2) |
| **#113** | `(304, 240)` | `16 x 16 px` | `(304, 240)` | `16 x 16 px` | 184 px | UW Palette | Brick debris — 4 fragments flying out when the brick is smashed (extended palette 2) |
| **#114** | `(320, 240)` | `16 x 16 px` | `(320, 240)` | `16 x 16 px` | 252 px | UW Palette | Hit — brick bouncing up, containing an item (extended palette 2) |
| **#115** | `(336, 240)` | `16 x 16 px` | `(336, 240)` | `16 x 16 px` | 252 px | UW Palette | After Hit — brick cell after all items are popped (extended palette 2) |

### 2. Brick Set 2 — Underground & Castle (y = 192-255, X = 368-447)

| # | Flood-fill coords (X, Y) | Flood size | Grid cell coords | Grid size | Pixel count | Palette | Meaning / frame state |
|-----|-----------------------|------------------|----------------|-----------------|----------|---------|----------------------------------|
| **#86** | `(368, 192)` | `16 x 16 px` | `(368, 192)` | `16 x 16 px` | 256 px | OW Palette | Initial Tile — intact brick block, untouched (extended palette 2) |
| **#87** | `(384, 192)` | `16 x 16 px` | `(384, 192)` | `16 x 16 px` | 256 px | OW Palette | Hit (empty) — brick bounced up by Small Mario's headbutt (extended palette 2) |
| **#88** | `(400, 192)` | `16 x 16 px` | `(400, 192)` | `16 x 16 px` | 184 px | OW Palette | Brick debris — 4 fragments flying out when the brick is smashed (extended palette 2) |
| **#89** | `(416, 192)` | `16 x 16 px` | `(416, 192)` | `16 x 16 px` | 252 px | OW Palette | Hit — brick bouncing up, containing an item (extended palette 2) |
| **#90** | `(432, 192)` | `16 x 16 px` | `(432, 192)` | `16 x 16 px` | 252 px | OW Palette | After Hit — brick cell after all items are popped (extended palette 2) |
| **#96** | `(368, 208)` | `16 x 16 px` | `(368, 208)` | `16 x 16 px` | 256 px | UG Palette | Initial Tile — intact brick block, untouched (extended palette 2) |
| **#97** | `(384, 208)` | `16 x 16 px` | `(384, 208)` | `16 x 16 px` | 256 px | UG Palette | Hit (empty) — brick bounced up by Small Mario's headbutt (extended palette 2) |
| **#98** | `(400, 208)` | `16 x 16 px` | `(400, 208)` | `16 x 16 px` | 184 px | UG Palette | Brick debris — 4 fragments flying out when the brick is smashed (extended palette 2) |
| **#99** | `(416, 208)` | `16 x 16 px` | `(416, 208)` | `16 x 16 px` | 252 px | UG Palette | Hit — brick bouncing up, containing an item (extended palette 2) |
| **#100** | `(432, 208)` | `16 x 16 px` | `(432, 208)` | `16 x 16 px` | 252 px | UG Palette | After Hit — brick cell after all items are popped (extended palette 2) |
| **#106** | `(368, 224)` | `16 x 16 px` | `(368, 224)` | `16 x 16 px` | 256 px | Castle Palette | Initial Tile — intact brick block, untouched (extended palette 2) |
| **#107** | `(384, 224)` | `16 x 16 px` | `(384, 224)` | `16 x 16 px` | 256 px | Castle Palette | Hit (empty) — brick bounced up by Small Mario's headbutt (extended palette 2) |
| **#108** | `(400, 224)` | `16 x 16 px` | `(400, 224)` | `16 x 16 px` | 184 px | Castle Palette | Brick debris — 4 fragments flying out when the brick is smashed (extended palette 2) |
| **#109** | `(416, 224)` | `16 x 16 px` | `(416, 224)` | `16 x 16 px` | 252 px | Castle Palette | Hit — brick bouncing up, containing an item (extended palette 2) |
| **#110** | `(432, 224)` | `16 x 16 px` | `(432, 224)` | `16 x 16 px` | 252 px | Castle Palette | After Hit — brick cell after all items are popped (extended palette 2) |
| **#116** | `(368, 240)` | `16 x 16 px` | `(368, 240)` | `16 x 16 px` | 256 px | UW Palette | Initial Tile — intact brick block, untouched (extended palette 2) |
| **#117** | `(384, 240)` | `16 x 16 px` | `(384, 240)` | `16 x 16 px` | 256 px | UW Palette | Hit (empty) — brick bounced up by Small Mario's headbutt (extended palette 2) |
| **#118** | `(400, 240)` | `16 x 16 px` | `(400, 240)` | `16 x 16 px` | 184 px | UW Palette | Brick debris — 4 fragments flying out when the brick is smashed (extended palette 2) |
| **#119** | `(416, 240)` | `16 x 16 px` | `(416, 240)` | `16 x 16 px` | 252 px | UW Palette | Hit — brick bouncing up, containing an item (extended palette 2) |
| **#120** | `(432, 240)` | `16 x 16 px` | `(432, 240)` | `16 x 16 px` | 252 px | UW Palette | After Hit — brick cell after all items are popped (extended palette 2) |

---

## V. Block Structure Summary (`items_blocks.png`)

| Block group | Environments / Palettes | Sprite count (frames) | # range | Engine usage |
|-------------------|--------------------------|-------------------------|---------|--------------------------------|
| **Coin Blocks (`?`) - OW/UW** | 4 Palettes (OW/UG/CT/UW) | 20 | #1 - #20 | `?` block with 3 blinking frames, bounces when hit (`Hit`), becomes a flat brown empty cell |
| **Coin Blocks (`?`) - UG/CT** | 4 Palettes (OW/UG/CT/UW) | 20 | #21 - #40 | `?` block variants specific to dungeons / castles |
| **Brick Blocks (Set 1) - OW/UW** | 4 Palettes (OW/UG/CT/UW) | 20 | #41 - #60 | Outdoor bricks, 4-corner brick fragments, mushroom/coin pops |
| **Brick Blocks (Set 1) - UG/CT** | 4 Palettes (OW/UG/CT/UW) | 20 | #61 - #80 | Dungeon / castle bricks |
| **Brick Blocks (Set 2) - OW/UW** | 4 Palettes (OW/UG/CT/UW) | 20 | #81 - #100 | Outdoor brick variants (lower row) |
| **Brick Blocks (Set 2) - UG/CT** | 4 Palettes (OW/UG/CT/UW) | 20 | #101 - #120 | Dungeon brick variants (lower row) |
| **TOTAL** | **4 Palettes x 30 columns** | **120 Sprites** | **#1 - #120** | **The entire block system in the game** |

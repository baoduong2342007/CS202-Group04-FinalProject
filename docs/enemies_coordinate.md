# Enemy Frame Coordinate and Meaning Table (`enemies_coordinate.md`)

List of all **157** connected enemy components extracted from `enemies_all_components_atlas.png`, cross-checked against the original atlas `enemies.png` (436 x 530 px).

> **Palette conventions (NES):**
> - **Overworld (OW)**: Green `(12,147,0)` + Orange `(230,156,33)` — used in Level 1-1, 1-3, 3-1, etc.
> - **Underground (UG)**: Teal `(0,123,140)` + Brown `(156,74,0)` + Light pink `(255,206,197)` — used in Level 1-2, 4-2, etc.
> - **Castle (CT)**: Gray `(173,173,173)` + Orange `(230,156,33)` — used in Level 1-4, 2-4, etc.
> - **Underwater (UW)**: Light teal `(181,239,239)` — used in Level 2-2, 7-2, etc.
> - **Red / All Environments**: Red `(181,49,33)` + Orange `(230,156,33)` — some enemies have a single palette shared across all levels.

## How to read the coordinates and meanings

- The coordinate origin `(0, 0)` is at the top-left corner of `enemies.png`; all coordinates are **zero-based pixels**.
- **Frame grid cell coords + frame size** form the `sf::IntRect` used to cut the runtime sprite. The right and bottom edges are exclusive boundaries (`x + W`, `y + H`).
- **Flood-fill coords + flood size** are the bounding box of the sprite pixels found when extracting components. They can be negative or overflow the grid cell (e.g. `#1`, `#125`); **do not use this column as a runtime `IntRect`**.
- `Underground + Castle` in the Green Enemies area is a single shared palette row, not two different frames. In contrast, the four columns in the Ground Enemies area are four separate palettes.
- `In Castle 1 Enemies` and `In Castle 2 Enemies` are palette/special variants of sprites in the castle level: `#137/#138` and `#141/#142` are **Bowser raising his hammer**, while only `#139/#140` are the **Hammer Bro**.
- Animation sequences must be read in frame order within each group; the atlas-wide index numbers are only identifiers, not a continuous animation order across species.

### Results of this cross-check

- All **157/157** rows are present, the index numbers are unique, and every grid `IntRect` lies within the `436 x 530` bounds; all grid cells contain sprite pixels.
- Grid coordinates that already matched the image were not arbitrarily changed. The edits mainly fix state names, enemy types, and animation order; runtime coordinates that were wrong in the C++ catalog were synced to the Grid column of this table.
- Sequences that must be preserved when porting to code: `UW Koopa #61 -> #62`, `UW Paratroopa #63 -> #64`, `Red Koopa #143 -> #144`, `Red Paratroopa #145 -> #146`, `Red Spiny #153 -> #154`.

---

| # | Flood-fill coords (X, Y) | Flood size (W x H) | Frame grid cell coords (X, Y) | Frame size | Meaning / enemy name & state |
|-----|-----------------------|--------------------------|----------------------------|------------------|---------------------------------------|
| **#1** | (-1, 13) | 20 x 20 px | `(0, 16)` | `16 x 16 px` | Goomba — Overworld (walking - frame 1) |
| **#2** | (16, 13) | 21 x 20 px | `(18, 16)` | `16 x 16 px` | Goomba — Overworld (walking - frame 2) |
| **#3** | (73, 13) | 20 x 20 px | `(74, 16)` | `16 x 16 px` | Goomba — Underground (walking - frame 1) |
| **#4** | (90, 13) | 21 x 20 px | `(92, 16)` | `16 x 16 px` | Goomba — Underground (walking - frame 2) |
| **#5** | (147, 13) | 18 x 20 px | `(148, 16)` | `16 x 16 px` | Goomba — Castle (walking - frame 1) |
| **#6** | (165, 13) | 18 x 20 px | `(166, 16)` | `16 x 16 px` | Goomba — Castle (walking - frame 2) |
| **#7** | (221, 13) | 20 x 20 px | `(222, 16)` | `16 x 16 px` | Goomba — Underwater (walking - frame 1) |
| **#8** | (238, 13) | 21 x 20 px | `(240, 16)` | `16 x 16 px` | Goomba — Underwater (walking - frame 2) |
| **#9** | (35, 23) | 18 x 10 px | `(36, 24)` | `16 x 8 px` | Goomba — Overworld (stomped flat) |
| **#10** | (109, 23) | 18 x 10 px | `(110, 24)` | `16 x 8 px` | Goomba — Underground (stomped flat) |
| **#11** | (183, 23) | 18 x 10 px | `(184, 24)` | `16 x 8 px` | Goomba — Castle (stomped flat) |
| **#12** | (257, 23) | 18 x 10 px | `(258, 24)` | `16 x 8 px` | Goomba — Underwater (stomped flat) |
| **#13** | (-1, 34) | 18 x 17 px | `(0, 34)` | `16 x 16 px` | Buzzy Beetle — Overworld (crawling, frame 1) |
| **#14** | (17, 33) | 18 x 18 px | `(18, 34)` | `16 x 16 px` | Buzzy Beetle — Overworld (crawling, frame 2) |
| **#15** | (35, 34) | 18 x 17 px | `(36, 34)` | `16 x 16 px` | Buzzy Beetle — Overworld (retracted into shell) |
| **#16** | (73, 34) | 18 x 17 px | `(74, 34)` | `16 x 16 px` | Buzzy Beetle — Underground (crawling, frame 1) |
| **#17** | (91, 33) | 18 x 18 px | `(92, 34)` | `16 x 16 px` | Buzzy Beetle — Underground (crawling, frame 2) |
| **#18** | (109, 34) | 18 x 17 px | `(110, 34)` | `16 x 16 px` | Buzzy Beetle — Underground (retracted into shell) |
| **#19** | (147, 34) | 18 x 17 px | `(148, 34)` | `16 x 16 px` | Buzzy Beetle — Castle (crawling, frame 1) |
| **#20** | (165, 33) | 18 x 18 px | `(166, 34)` | `16 x 16 px` | Buzzy Beetle — Castle (crawling, frame 2) |
| **#21** | (183, 34) | 18 x 17 px | `(184, 34)` | `16 x 16 px` | Buzzy Beetle — Castle (retracted into shell) |
| **#22** | (221, 34) | 18 x 17 px | `(222, 34)` | `16 x 16 px` | Buzzy Beetle — Underwater (crawling, frame 1) |
| **#23** | (239, 33) | 18 x 18 px | `(240, 34)` | `16 x 16 px` | Buzzy Beetle — Underwater (crawling, frame 2) |
| **#24** | (257, 34) | 18 x 17 px | `(258, 34)` | `16 x 16 px` | Buzzy Beetle — Underwater (retracted into shell) |
| **#25** | (-1, 51) | 18 x 26 px | `(0, 52)` | `16 x 24 px` | Blooper — Overworld (stretched out, frame 1) |
| **#26** | (17, 51) | 18 x 18 px | `(18, 52)` | `16 x 16 px` | Blooper — Overworld (contracted, frame 2) |
| **#27** | (53, 52) | 18 x 16 px | `(54, 52)` | `16 x 16 px` | Bullet Bill — Overworld |
| **#28** | (73, 51) | 18 x 26 px | `(74, 52)` | `16 x 24 px` | Blooper — Underground (stretched out, frame 1) |
| **#29** | (91, 51) | 18 x 18 px | `(92, 52)` | `16 x 16 px` | Blooper — Underground (contracted, frame 2) |
| **#30** | (127, 52) | 18 x 16 px | `(128, 52)` | `16 x 16 px` | Bullet Bill — Underground |
| **#31** | (147, 51) | 18 x 26 px | `(148, 52)` | `16 x 24 px` | Blooper — Castle (stretched out, frame 1) |
| **#32** | (165, 51) | 18 x 18 px | `(166, 52)` | `16 x 16 px` | Blooper — Castle (contracted, frame 2) |
| **#33** | (201, 52) | 18 x 16 px | `(202, 52)` | `16 x 16 px` | Bullet Bill — Castle |
| **#34** | (221, 51) | 18 x 26 px | `(222, 52)` | `16 x 24 px` | Blooper — Underwater (stretched out, frame 1) |
| **#35** | (239, 51) | 18 x 18 px | `(240, 52)` | `16 x 16 px` | Blooper — Underwater (contracted, frame 2) |
| **#36** | (275, 52) | 18 x 16 px | `(276, 52)` | `16 x 16 px` | Bullet Bill — Underwater |
| **#37** | (3, 77) | 10 x 18 px | `(4, 78)` | `8 x 16 px` | Buzzy Beetle — Overworld (climbing left wall) |
| **#38** | (39, 77) | 10 x 18 px | `(40, 78)` | `8 x 16 px` | Buzzy Beetle — Overworld (climbing right wall) |
| **#39** | (77, 77) | 10 x 18 px | `(78, 78)` | `8 x 16 px` | Buzzy Beetle — Underground (climbing left wall) |
| **#40** | (113, 77) | 10 x 18 px | `(114, 78)` | `8 x 16 px` | Buzzy Beetle — Underground (climbing right wall) |
| **#41** | (151, 77) | 10 x 18 px | `(152, 78)` | `8 x 16 px` | Buzzy Beetle — Castle (climbing left wall) |
| **#42** | (187, 77) | 10 x 18 px | `(188, 78)` | `8 x 16 px` | Buzzy Beetle — Castle (climbing right wall) |
| **#43** | (225, 77) | 10 x 18 px | `(226, 78)` | `8 x 16 px` | Buzzy Beetle — Underwater (climbing left wall) |
| **#44** | (261, 77) | 10 x 18 px | `(262, 78)` | `8 x 16 px` | Buzzy Beetle — Underwater (climbing right wall) |
| **#45** | (18, 81) | 16 x 10 px | `(18, 82)` | `16 x 8 px` | Buzzy Beetle — Overworld (crawling on ceiling, left) |
| **#46** | (54, 81) | 16 x 10 px | `(54, 82)` | `16 x 8 px` | Buzzy Beetle — Overworld (crawling on ceiling, right) |
| **#47** | (92, 81) | 16 x 10 px | `(92, 82)` | `16 x 8 px` | Buzzy Beetle — Underground (crawling on ceiling, left) |
| **#48** | (128, 81) | 16 x 10 px | `(128, 82)` | `16 x 8 px` | Buzzy Beetle — Underground (crawling on ceiling, right) |
| **#49** | (166, 81) | 16 x 10 px | `(166, 82)` | `16 x 8 px` | Buzzy Beetle — Castle (crawling on ceiling, left) |
| **#50** | (202, 81) | 16 x 10 px | `(202, 82)` | `16 x 8 px` | Buzzy Beetle — Castle (crawling on ceiling, right) |
| **#51** | (240, 81) | 16 x 10 px | `(240, 82)` | `16 x 8 px` | Buzzy Beetle — Underwater (crawling on ceiling, left) |
| **#52** | (276, 81) | 16 x 10 px | `(276, 82)` | `16 x 8 px` | Buzzy Beetle — Underwater (crawling on ceiling, right) |
| **#53** | (-1, 109) | 20 x 28 px | `(0, 112)` | `16 x 24 px` | Green Koopa Troopa — Overworld (walking, frame 1) |
| **#54** | (16, 109) | 21 x 28 px | `(18, 112)` | `16 x 24 px` | Green Koopa Troopa — Overworld (walking, frame 2) |
| **#55** | (33, 109) | 20 x 28 px | `(36, 112)` | `16 x 24 px` | Green Koopa Paratroopa — Overworld (flying, frame 1) |
| **#56** | (53, 111) | 18 x 26 px | `(54, 112)` | `16 x 24 px` | Green Koopa Paratroopa — Overworld (flying, frame 2) |
| **#57** | (145, 109) | 19 x 28 px | `(146, 112)` | `16 x 24 px` | Green Koopa Troopa — Underground + Castle (walking, frame 1) |
| **#58** | (161, 109) | 21 x 28 px | `(164, 112)` | `16 x 24 px` | Green Koopa Troopa — Underground + Castle (walking, frame 2) |
| **#59** | (179, 109) | 22 x 28 px | `(182, 112)` | `16 x 24 px` | Green Koopa Paratroopa — Underground + Castle (flying, frame 1) |
| **#60** | (198, 109) | 21 x 28 px | `(200, 112)` | `16 x 24 px` | Green Koopa Paratroopa — Underground + Castle (flying, frame 2) |
| **#61** | (291, 109) | 20 x 28 px | `(292, 112)` | `16 x 24 px` | Green Koopa Troopa — Underwater (walking, frame 1) |
| **#62** | (308, 109) | 21 x 28 px | `(310, 112)` | `16 x 24 px` | Green Koopa Troopa — Underwater (walking, frame 2) |
| **#63** | (325, 109) | 20 x 28 px | `(328, 112)` | `16 x 24 px` | Green Koopa Paratroopa — Underwater (flying, frame 1) |
| **#64** | (345, 111) | 18 x 26 px | `(346, 112)` | `16 x 24 px` | Green Koopa Paratroopa — Underwater (flying, frame 2) |
| **#65** | (71, 119) | 18 x 16 px | `(72, 120)` | `16 x 16 px` | Green Koopa shell — Overworld (retracted into shell / shell idle) |
| **#66** | (89, 119) | 18 x 17 px | `(90, 120)` | `16 x 16 px` | Green Koopa shell — Overworld (legs out / waking) |
| **#67** | (217, 119) | 18 x 16 px | `(218, 120)` | `16 x 16 px` | Green Koopa shell — Underground + Castle (retracted into shell) |
| **#68** | (235, 119) | 18 x 17 px | `(236, 120)` | `16 x 16 px` | Green Koopa shell — Underground + Castle (legs out / waking) |
| **#69** | (363, 119) | 18 x 16 px | `(364, 120)` | `16 x 16 px` | Green Koopa shell — Underwater (retracted into shell) |
| **#70** | (381, 119) | 18 x 17 px | `(382, 120)` | `16 x 16 px` | Green Koopa shell — Underwater (legs out / waking) |
| **#71** | (-1, 138) | 18 x 25 px | `(0, 138)` | `16 x 24 px` | Piranha Plant — Overworld (mouth open) |
| **#72** | (17, 137) | 18 x 26 px | `(18, 138)` | `16 x 24 px` | Piranha Plant — Overworld (mouth closed) |
| **#73** | (53, 137) | 18 x 26 px | `(54, 138)` | `16 x 24 px` | Lakitu — Overworld (peeking out of cloud, ready to throw a Spiny egg) |
| **#74** | (145, 138) | 18 x 25 px | `(146, 138)` | `16 x 24 px` | Piranha Plant — Underground + Castle (mouth open) |
| **#75** | (163, 137) | 18 x 26 px | `(164, 138)` | `16 x 24 px` | Piranha Plant — Underground + Castle (mouth closed) |
| **#76** | (199, 137) | 18 x 26 px | `(200, 138)` | `16 x 24 px` | Lakitu — Underground + Castle (peeking out of cloud, ready to throw a Spiny egg) |
| **#77** | (291, 138) | 18 x 25 px | `(292, 138)` | `16 x 24 px` | Piranha Plant — Underwater (mouth open) |
| **#78** | (309, 137) | 18 x 26 px | `(310, 138)` | `16 x 24 px` | Piranha Plant — Underwater (mouth closed) |
| **#79** | (345, 137) | 18 x 26 px | `(346, 138)` | `16 x 24 px` | Lakitu — Underwater (peeking out of cloud, ready to throw a Spiny egg) |
| **#80** | (71, 145) | 18 x 18 px | `(72, 146)` | `16 x 16 px` | Lakitu — Overworld (hidden in cloud / empty cloud) |
| **#81** | (217, 145) | 18 x 18 px | `(218, 146)` | `16 x 16 px` | Lakitu — Underground + Castle (hidden in cloud / empty cloud) |
| **#82** | (363, 145) | 18 x 18 px | `(364, 146)` | `16 x 16 px` | Lakitu — Underwater (hidden in cloud / empty cloud) |
| **#83** | (-1, 163) | 18 x 18 px | `(0, 164)` | `16 x 16 px` | Cheep Cheep — Overworld (swimming, fin up / frame 1) |
| **#84** | (17, 163) | 17 x 18 px | `(18, 164)` | `16 x 16 px` | Cheep Cheep — Overworld (swimming, fin down / frame 2) |
| **#85** | (145, 163) | 18 x 18 px | `(146, 164)` | `16 x 16 px` | Cheep Cheep — Underground + Castle (swimming, fin up / frame 1) |
| **#86** | (163, 163) | 17 x 18 px | `(164, 164)` | `16 x 16 px` | Cheep Cheep — Underground + Castle (swimming, fin down / frame 2) |
| **#87** | (291, 163) | 18 x 18 px | `(292, 164)` | `16 x 16 px` | Cheep Cheep — Underwater (swimming, fin up / frame 1) |
| **#88** | (309, 163) | 17 x 18 px | `(310, 164)` | `16 x 16 px` | Cheep Cheep — Underwater (swimming, fin down / frame 2) |
| **#89** | (72, 171) | 17 x 36 px | `(72, 172)` | `16 x 34 px` | Hammer Bro — Overworld (throwing hammer, frame 1) |
| **#90** | (90, 171) | 17 x 36 px | `(90, 172)` | `16 x 34 px` | Hammer Bro — Overworld (throwing hammer, frame 2) |
| **#91** | (107, 171) | 17 x 36 px | `(108, 172)` | `16 x 34 px` | Hammer Bro — Overworld (throwing hammer, frame 3) |
| **#92** | (125, 171) | 17 x 36 px | `(126, 172)` | `16 x 34 px` | Hammer Bro — Overworld (throwing hammer, frame 4) |
| **#93** | (218, 171) | 17 x 36 px | `(218, 172)` | `16 x 34 px` | Hammer Bro — Underground + Castle (throwing hammer, frame 1) |
| **#94** | (236, 171) | 17 x 36 px | `(236, 172)` | `16 x 34 px` | Hammer Bro — Underground + Castle (throwing hammer, frame 2) |
| **#95** | (253, 171) | 17 x 36 px | `(254, 172)` | `16 x 34 px` | Hammer Bro — Underground + Castle (throwing hammer, frame 3) |
| **#96** | (271, 171) | 17 x 36 px | `(272, 172)` | `16 x 34 px` | Hammer Bro — Underground + Castle (throwing hammer, frame 4) |
| **#97** | (364, 171) | 17 x 36 px | `(364, 172)` | `16 x 34 px` | Hammer Bro — Underwater (throwing hammer, frame 1) |
| **#98** | (382, 171) | 17 x 36 px | `(382, 172)` | `16 x 34 px` | Hammer Bro — Underwater (throwing hammer, frame 2) |
| **#99** | (399, 171) | 17 x 36 px | `(400, 172)` | `16 x 34 px` | Hammer Bro — Underwater (throwing hammer, frame 3) |
| **#100** | (417, 171) | 17 x 36 px | `(418, 172)` | `16 x 34 px` | Hammer Bro — Underwater (throwing hammer, frame 4) |
| **#101** | (-1, 181) | 18 x 26 px | `(0, 182)` | `16 x 24 px` | Hammer Bro — Overworld (standing/walking, frame 1) |
| **#102** | (17, 181) | 18 x 26 px | `(18, 182)` | `16 x 24 px` | Hammer Bro — Overworld (standing/walking, frame 2) |
| **#103** | (36, 181) | 17 x 26 px | `(36, 182)` | `16 x 24 px` | Hammer Bro — Overworld (standing/walking, frame 3) |
| **#104** | (54, 181) | 17 x 26 px | `(54, 182)` | `16 x 24 px` | Hammer Bro — Overworld (standing/walking, frame 4) |
| **#105** | (145, 181) | 18 x 26 px | `(146, 182)` | `16 x 24 px` | Hammer Bro — Underground + Castle (standing/walking, frame 1) |
| **#106** | (163, 181) | 18 x 26 px | `(164, 182)` | `16 x 24 px` | Hammer Bro — Underground + Castle (standing/walking, frame 2) |
| **#107** | (182, 181) | 17 x 26 px | `(182, 182)` | `16 x 24 px` | Hammer Bro — Underground + Castle (standing/walking, frame 3) |
| **#108** | (200, 181) | 17 x 26 px | `(200, 182)` | `16 x 24 px` | Hammer Bro — Underground + Castle (standing/walking, frame 4) |
| **#109** | (291, 181) | 18 x 26 px | `(292, 182)` | `16 x 24 px` | Hammer Bro — Underwater (standing/walking, frame 1) |
| **#110** | (309, 181) | 18 x 26 px | `(310, 182)` | `16 x 24 px` | Hammer Bro — Underwater (standing/walking, frame 2) |
| **#111** | (328, 181) | 17 x 26 px | `(328, 182)` | `16 x 24 px` | Hammer Bro — Underwater (standing/walking, frame 3) |
| **#112** | (346, 181) | 17 x 26 px | `(346, 182)` | `16 x 24 px` | Hammer Bro — Underwater (standing/walking, frame 4) |
| **#113** | (-1, 207) | 34 x 34 px | `(0, 208)` | `32 x 32 px` | Bowser — Overworld (walking, mouth closed / frame 1) |
| **#114** | (33, 207) | 34 x 34 px | `(34, 208)` | `32 x 32 px` | Bowser — Overworld (walking, mouth open / frame 2) |
| **#115** | (67, 207) | 34 x 34 px | `(68, 208)` | `32 x 32 px` | Bowser — Overworld (fire-breathing pose, frame 1) |
| **#116** | (101, 207) | 34 x 34 px | `(102, 208)` | `32 x 32 px` | Bowser — Overworld (fire-breathing pose, frame 2) |
| **#117** | (145, 207) | 34 x 34 px | `(146, 208)` | `32 x 32 px` | Bowser — Underground + Castle (walking, mouth closed / frame 1) |
| **#118** | (179, 207) | 34 x 34 px | `(180, 208)` | `32 x 32 px` | Bowser — Underground + Castle (walking, mouth open / frame 2) |
| **#119** | (213, 207) | 34 x 34 px | `(214, 208)` | `32 x 32 px` | Bowser — Underground + Castle (fire-breathing pose, frame 1) |
| **#120** | (247, 207) | 34 x 34 px | `(248, 208)` | `32 x 32 px` | Bowser — Underground + Castle (fire-breathing pose, frame 2) |
| **#121** | (291, 207) | 34 x 34 px | `(292, 208)` | `32 x 32 px` | Bowser — Underwater (walking, mouth closed / frame 1) |
| **#122** | (325, 207) | 34 x 34 px | `(326, 208)` | `32 x 32 px` | Bowser — Underwater (walking, mouth open / frame 2) |
| **#123** | (359, 207) | 34 x 34 px | `(360, 208)` | `32 x 32 px` | Bowser — Underwater (fire-breathing pose, frame 1) |
| **#124** | (393, 207) | 34 x 34 px | `(394, 208)` | `32 x 32 px` | Bowser — Underwater (fire-breathing pose, frame 2) |
| **#125** | (33, 241) | 32 x 30 px | `(34, 242)` | `32 x 26 px` | Bowser — Overworld (raising hammer, facing left) |
| **#126** | (69, 241) | 32 x 28 px | `(68, 242)` | `32 x 26 px` | Bowser — Overworld (raising hammer, facing right) |
| **#127** | (101, 241) | 26 x 10 px | `(102, 242)` | `24 x 8 px` | Bowser's fireball — Overworld (frame 1) |
| **#128** | (179, 241) | 32 x 30 px | `(180, 242)` | `32 x 26 px` | Bowser — Underground + Castle (raising hammer, facing left) |
| **#129** | (215, 241) | 32 x 28 px | `(214, 242)` | `32 x 26 px` | Bowser — Underground + Castle (raising hammer, facing right) |
| **#130** | (247, 241) | 26 x 10 px | `(248, 242)` | `24 x 8 px` | Bowser's fireball — Underground + Castle (frame 1) |
| **#131** | (325, 241) | 32 x 28 px | `(326, 242)` | `32 x 26 px` | Bowser — Underwater (raising hammer, facing left) |
| **#132** | (361, 241) | 32 x 28 px | `(360, 242)` | `32 x 26 px` | Bowser — Underwater (raising hammer, facing right) |
| **#133** | (393, 241) | 26 x 10 px | `(394, 242)` | `24 x 8 px` | Bowser's fireball — Underwater (frame 1) |
| **#134** | (101, 251) | 26 x 10 px | `(102, 252)` | `24 x 8 px` | Bowser's fireball — Overworld (frame 2) |
| **#135** | (247, 251) | 26 x 10 px | `(248, 252)` | `24 x 8 px` | Bowser's fireball — Underground + Castle (frame 2) |
| **#136** | (393, 251) | 26 x 10 px | `(394, 252)` | `24 x 8 px` | Bowser's fireball — Underwater (frame 2) |
| **#137** | (32, 273) | 33 x 32 px | `(34, 276)` | `32 x 26 px` | Bowser — In Castle 1 (raising hammer, facing left / frame 1) |
| **#138** | (69, 275) | 32 x 28 px | `(68, 276)` | `32 x 26 px` | Bowser — In Castle 1 (raising hammer, facing right / frame 2) |
| **#139** | (144, 273) | 18 x 30 px | `(144, 276)` | `16 x 26 px` | Hammer Bro — In Castle 2 (throwing hammer, frame 1) |
| **#140** | (159, 273) | 22 x 30 px | `(162, 276)` | `16 x 26 px` | Hammer Bro — In Castle 2 (throwing hammer, frame 2) |
| **#141** | (178, 273) | 33 x 30 px | `(180, 276)` | `32 x 26 px` | Bowser — In Castle 2 (raising hammer, facing left / frame 1) |
| **#142** | (215, 275) | 32 x 28 px | `(214, 276)` | `32 x 26 px` | Bowser — In Castle 2 (raising hammer, facing right / frame 2) |
| **#143** | (-1, 315) | 20 x 28 px | `(0, 318)` | `16 x 24 px` | Red Koopa Troopa — walking, frame 1 |
| **#144** | (16, 315) | 19 x 28 px | `(18, 318)` | `16 x 24 px` | Red Koopa Troopa — walking, frame 2 |
| **#145** | (35, 315) | 19 x 28 px | `(36, 318)` | `16 x 24 px` | Red Koopa Paratroopa — flying, frame 1 |
| **#146** | (51, 315) | 20 x 28 px | `(54, 318)` | `16 x 24 px` | Red Koopa Paratroopa — flying, frame 2 |
| **#147** | (71, 325) | 18 x 16 px | `(72, 326)` | `16 x 16 px` | Red Koopa shell — retracted into shell / shell idle |
| **#148** | (89, 325) | 18 x 17 px | `(90, 326)` | `16 x 16 px` | Red Koopa shell — legs out / waking |
| **#149** | (-1, 344) | 18 x 25 px | `(0, 344)` | `16 x 24 px` | Red Piranha Plant — mouth closed |
| **#150** | (17, 343) | 18 x 26 px | `(18, 344)` | `16 x 24 px` | Red Piranha Plant — mouth open |
| **#151** | (36, 351) | 16 x 18 px | `(36, 352)` | `16 x 16 px` | Red Spiny Egg — falling/spinning, frame 1 |
| **#152** | (54, 351) | 16 x 18 px | `(54, 352)` | `16 x 16 px` | Red Spiny Egg — falling/spinning, frame 2 |
| **#153** | (71, 352) | 18 x 17 px | `(72, 352)` | `16 x 16 px` | Red Spiny — landed, crawling, frame 1 |
| **#154** | (89, 351) | 18 x 18 px | `(90, 352)` | `16 x 16 px` | Red Spiny — landed, crawling, frame 2 |
| **#155** | (-1, 369) | 18 x 18 px | `(0, 370)` | `16 x 16 px` | Red Cheep Cheep — swimming, fin up / frame 1 |
| **#156** | (17, 369) | 17 x 18 px | `(18, 370)` | `16 x 16 px` | Red Cheep Cheep — swimming, fin down / frame 2 |
| **#157** | (90, 369) | 16 x 18 px | `(90, 370)` | `16 x 16 px` | Lava Bubble (Podoboo) — leaping up from lava |

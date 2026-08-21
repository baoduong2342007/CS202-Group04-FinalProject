# Sprint 6 Asset Manifest

This is the source of truth for asset paths and classifications. Runtime paths are relative to the executable directory after CMake synchronization. Reference paths are relative to the repository root.

| Label | Meaning |
|---|---|
| `Runtime` | Loaded by current code or packaged for a current game-loop path |
| `Reference` | Source/measurement/documentation material outside the runtime package |
| `Future` | Valid asset with no Sprint 6 release gameplay path |
| `Remove` | Must not remain in the repository or package |

## Textures and font

| Path | Dimensions | Usage | Runtime caller or note |
|---|---:|---|---|
| `assets/fonts/mario.ttf` | 116,008 bytes | `Runtime` | HUD and state UI font |
| `assets/textures/enemies/enemies.png` | 436×530 | `Runtime` | Goomba/Koopa atlas cropped through `SpriteFrames` |
| `assets/textures/enemies/goomba.png` | 96×32 | `Future` | Standalone sheet without a release loader |
| `assets/textures/enemies/koopa.png` | 128×48 | `Future` | Standalone sheet without a release loader |
| `assets/textures/items/items_blocks.png` | 448×256 | `Runtime` | QuestionBlock and block frames |
| `assets/textures/items/items_objects.png` | 592×572 | `Runtime` | Mushroom, Coin, FireFlower, and Star frames |
| `assets/textures/mario/MarioLuigi.png` | 584×469 | `Runtime` | Mario/Luigi state spritesheet |
| `assets/textures/tiles/tileset.png` | 680×356 | `Runtime` | Theme-specific tiles selected by `TileFrames` |
| `assets/ui/bg_world.png` | 1857×847 | `Runtime` | Overworld-only bitmap background; scaled in the logical world view |
| `assets/textures/ui/stage_1.png` | 1206 bytes | `Runtime` | Level Select preview for World 1-1 |
| `assets/textures/ui/stage_2.png` | 978 bytes | `Runtime` | Level Select preview for World 1-2 |
| `assets/textures/ui/stage_3.png` | 1039 bytes | `Runtime` | Level Select preview for World 1-3 |
| `assets/textures/ui/stage_4.png` | 1427 bytes | `Runtime` | Level Select preview for World 1-4 |
| `assets/textures/ui/bg_clouds.png` | 768×1129 | `Future` | No Sprint 6 runtime caller |
| `assets/textures/ui/bg_mountains.png` | 768×1660 | `Future` | Legacy overworld reference |
| `assets/textures/ui/bg_trees.png` | 768×1660 | `Future` | No Sprint 6 runtime caller |
| `assets/textures/ui/hud.png` | 784×948 | `Runtime` | Menu and HUD bitmap UI |
| `assets/textures/backgrounds/bg_overworld.png` | 640×360 | `Runtime` | Procedural themed pixel-art background for Overworld |
| `assets/textures/backgrounds/bg_underground.png` | 640×360 | `Runtime` | Procedural themed pixel-art background for Underground |
| `assets/textures/backgrounds/bg_underwater.png` | 640×360 | `Runtime` | Procedural themed pixel-art background for Underwater |
| `assets/textures/backgrounds/bg_castle.png` | 640×360 | `Runtime` | Procedural themed pixel-art background for Castle |

Underground, Underwater, and Castle are distinct renderer branches; they do not reuse
`bg_world.png`. Underwater music and all four Level Select previews are included in the
S7 runtime package. The S7 package manifest records the corresponding level/config files;
`levels/level0.txt` remains a test fixture and is excluded.

`.gitkeep` files only preserve intentionally empty directories and are not runtime assets.

## Sound effects

| Path | Usage |
|---|---|
| `assets/sounds/effects/1up.wav` | `Runtime` — one-up reward |
| `assets/sounds/effects/beep.wav` | `Future` — no event mapping |
| `assets/sounds/effects/billfirework.wav` | `Future` — no Bullet Bill path |
| `assets/sounds/effects/bowserfall.wav` | `Future` — no Bowser path |
| `assets/sounds/effects/brick.wav` | `Runtime` — `BRICK_BROKEN` |
| `assets/sounds/effects/bump.wav` | `Runtime` — `BLOCK_BUMPED` |
| `assets/sounds/effects/coin.wav` | `Runtime` — `COIN_COLLECTED` |
| `assets/sounds/effects/death.wav` | `Runtime` — `PLAYER_DIED` |
| `assets/sounds/effects/fire.wav` | `Future` — reserved effect |
| `assets/sounds/effects/fireball.wav` | `Runtime` — `FIREBALL_SHOT` after projectile creation |
| `assets/sounds/effects/flagpole.wav` | `Runtime` — `LEVEL_COMPLETED` |
| `assets/sounds/effects/gameover.wav` | `Runtime` — GameOver cue/catalog |
| `assets/sounds/effects/hurryup.wav` | `Runtime` — HUD time warning |
| `assets/sounds/effects/item.wav` | `Runtime` — `ITEM_EMERGED` |
| `assets/sounds/effects/jump.wav` | `Runtime` — `PLAYER_JUMPED`, including accepted Springboard launches |
| `assets/sounds/effects/jumpsmall.wav` | `Runtime` — small-jump catalog variant |
| `assets/sounds/effects/kickkill.wav` | `Runtime` — source sample for separate shell-kick and shell-defeat event cues |
| `assets/sounds/effects/pause.wav` | `Runtime` — `GAME_PAUSED` |
| `assets/sounds/effects/pipepowerdown.wav` | `Runtime` — `PLAYER_POWER_DOWN` |
| `assets/sounds/effects/powerup.wav` | `Runtime` — `PLAYER_POWER_UP` |
| `assets/sounds/effects/stompswim.wav` | `Runtime` — `ENEMY_STOMPED` |
| `assets/sounds/effects/vine.wav` | `Future` — no event mapping |

SoundManager is the only SFX playback authority. Event and request-counter tests protect one logical cue per accepted gameplay transaction.

## Music

| Path | MusicId | Usage |
|---|---|---|
| `assets/sounds/music/overworld.flac` | `OVERWORLD` | `Runtime` — Level 1 |
| `assets/sounds/music/underground.flac` | `UNDERGROUND` | `Runtime` — Level 2 |
| `assets/sounds/music/castle.flac` | `CASTLE` | `Runtime` — Level 4 Castle |
| `assets/sounds/music/invincible.flac` | `STAR` | `Runtime` — Star override |
| `assets/sounds/music/death.flac` | `DEATH` | `Runtime` — player death |
| `assets/sounds/music/gameover.flac` | `GAME_OVER` | `Runtime` — terminal GameOver state |
| `assets/sounds/music/level_complete.flac` | `WIN` | `Runtime` — final Win state |
| `assets/sounds/music/castle_complete.flac` | — | `Future` — optional variant |
| `assets/sounds/music/pipe.flac` | — | `Future` — optional pipe transition |
| `assets/sounds/music/underwater.flac` | `UNDERWATER` | `Runtime` — Level 3 Underwater |

## Sprite crop contract

- QuestionBlock uses `items_blocks.png`: 16×16 frames at `(80,112)`, `(96,112)`, `(112,112)`; the used block starts at `(128,112)`.
- Coin uses `items_objects.png`: four 8×16 frames at `(180,36)`, `(190,36)`, `(200,36)`, `(210,36)`.
- Tile catalog uses `assets/textures/tiles/tileset.png`; `TileMap` selects named ground, stone, brick, used, question, pipe, and flag frames from `TileFrames.h`.
- Do not create standalone crop files such as `idle.png`, `coin.png`, `mushroom.png`, or `star.png`; runtime animation uses named spritesheet rectangles.

## Reference files outside the runtime package

| Path | Dimensions | Usage |
|---|---:|---|
| `docs/assets/reference/enemies.png` | 436×530 | `Reference` — source enemy atlas |
| `docs/assets/reference/enemies_all_components_atlas.png` | 1072×1160 | `Reference` — indexed enemy analysis |
| `docs/assets/reference/enemies_all_components_atlas_full.png` | 800×1280 | `Reference` — historical component atlas |
| `docs/assets/reference/enemies_candidate.png` | 128×96 | `Reference` — candidate without runtime loader |
| `docs/assets/reference/blocks_all_components_atlas_full.png` | 800×800 | `Reference` — block analysis |
| `docs/assets/reference/items_objects_all_components_atlas_full.png` | 960×1840 | `Reference` — item analysis |
| `docs/assets/reference/items_blocks_candidate.png` | 96×96 | `Reference` — candidate without runtime loader |
| `docs/assets/reference/items_objects_candidate.png` | 128×64 | `Reference` — candidate without runtime loader |
| `docs/assets/reference/tileset_candidate.png` | 160×32 | `Reference` — unselected candidate |
| `docs/assets/reference/tileset_all_components_atlas_full.png` | 720×2448 | `Reference` — indexed component atlas |
| `docs/assets/reference/bg_world_candidate.png` | 1857×847 | `Reference` — background candidate/source |
| `docs/assets/reference/general_tile.png` | 680×776 | `Reference` — duplicate/source tile sheet |
| `docs/assets/reference/tileset(v2).png` | 680×776 | `Reference` — legacy tile source |

Production code must not use absolute asset paths, and CMake must not copy `docs/assets/reference/` into the runtime package.

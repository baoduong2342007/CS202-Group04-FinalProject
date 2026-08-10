# Asset manifest — Sprint 6

Đây là source of truth duy nhất cho asset trong project. Đường dẫn `Runtime`
tính từ thư mục executable sau khi CMake copy runtime assets; đường dẫn
`Reference` tính từ repository root.

## Nhãn sử dụng

| Nhãn | Ý nghĩa |
|---|---|
| `Runtime` | Được code hiện tại load hoặc được đóng gói để dùng trong game loop. |
| `Reference` | Nguồn đo/cắt sprite hoặc tài liệu hình ảnh; nằm ngoài gói runtime. |
| `Future` | Asset hợp lệ nhưng chưa có gameplay path hiện tại. |
| `Remove` | Không được giữ trong repository/runtime package. |

## Runtime textures và font

| Path | Kích thước | Usage | Ghi chú |
|---|---:|---|---|
| `assets/fonts/mario.ttf` | 116,008 bytes | `Runtime` | Font dùng cho HUD và các state UI. |
| `assets/textures/enemies/enemies.png` | 436×530 | `Runtime` | Atlas thật đang được `Goomba`/`Koopa` cắt qua `SpriteFrames`. |
| `assets/textures/enemies/goomba.png` | 96×32 | `Future` | Standalone Goomba sheet; chưa có runtime loader riêng. |
| `assets/textures/enemies/koopa.png` | 128×48 | `Future` | Standalone Koopa sheet; chưa có runtime loader riêng. |
| `assets/textures/items/items_blocks.png` | 448×256 | `Runtime` | QuestionBlock và block frames; không phải atlas runtime của Mushroom. |
| `assets/textures/items/items_objects.png` | 592×572 | `Runtime` | Mushroom, Coin, FireFlower và Star frames; được các item caller load trực tiếp. |
| `assets/textures/mario/MarioLuigi.png` | 584×469 | `Runtime` | Mario/Luigi state spritesheet. |
| `assets/textures/tiles/tileset.png` | 680×776 | `Runtime` | Tileset thật; `TileMap` dùng các frame catalog trong `TileFrames.h`. |
| `assets/ui/bg_world.png` | 1857×847 | `Runtime` | Full-frame modern pixel-art overworld; uniformly scaled to the logical view and repeated with alternating mirroring behind every level. |
| `assets/textures/ui/bg_clouds.png` | 768×1129 | `Future` | Không có caller runtime trong release hiện tại; giữ để tham khảo cho background tương lai. |
| `assets/textures/ui/bg_mountains.png` | 768×1660 | `Future` | Legacy overworld background kept as a reference asset. |
| `assets/textures/ui/bg_trees.png` | 768×1660 | `Future` | Chưa có runtime render path. |
| `assets/textures/ui/hud.png` | 784×948 | `Runtime` | Menu/HUD bitmap UI asset. |

Các file `assets/**/.gitkeep` chỉ là placeholder cho thư mục rỗng, không phải
runtime asset.

## Sound effects

| Path | Usage |
|---|---|
| `assets/sounds/effects/1up.wav` | `Runtime` — one-up reward. |
| `assets/sounds/effects/beep.wav` | `Future` — chưa có event mapping. |
| `assets/sounds/effects/billfirework.wav` | `Future` — chưa có Bullet Bill path. |
| `assets/sounds/effects/bowserfall.wav` | `Future` — chưa có Bowser path. |
| `assets/sounds/effects/brick.wav` | `Runtime` — `BRICK_BROKEN`. |
| `assets/sounds/effects/bump.wav` | `Runtime` — `BLOCK_BUMPED`. |
| `assets/sounds/effects/coin.wav` | `Runtime` — `COIN_COLLECTED`. |
| `assets/sounds/effects/death.wav` | `Runtime` — `PLAYER_DIED`. |
| `assets/sounds/effects/fire.wav` | `Future` — reserved fire effect. |
| `assets/sounds/effects/fireball.wav` | `Runtime` — `FIREBALL_SHOT` khi một FireBall được tạo. |
| `assets/sounds/effects/flagpole.wav` | `Runtime` — `LEVEL_COMPLETED`. |
| `assets/sounds/effects/gameover.wav` | `Runtime` — GameOver state SFX/catalog. |
| `assets/sounds/effects/hurryup.wav` | `Runtime` — HUD time-warning callback. |
| `assets/sounds/effects/item.wav` | `Runtime` — `ITEM_EMERGED` khi item rời block. |
| `assets/sounds/effects/jump.wav` | `Runtime` — `PLAYER_JUMPED`. |
| `assets/sounds/effects/jumpsmall.wav` | `Runtime` catalog — small-Mario jump variant. |
| `assets/sounds/effects/kickkill.wav` | `Runtime` — nguồn sample cho hai logical cues `shell_kick` và `shell_kill`; mỗi cue đi qua event riêng và chỉ phát một lần cho state transition/defeat. |
| `assets/sounds/effects/pause.wav` | `Runtime` — `GAME_PAUSED`. |
| `assets/sounds/effects/pipepowerdown.wav` | `Runtime` — `PLAYER_POWER_DOWN`. |
| `assets/sounds/effects/powerup.wav` | `Runtime` — `PLAYER_POWER_UP`. |
| `assets/sounds/effects/stompswim.wav` | `Runtime` — `ENEMY_STOMPED`. |
| `assets/sounds/effects/vine.wav` | `Future` — chưa có vine mechanic. |

Legacy duplicate GameOver effect đã bị xóa vì không có reference hợp lệ.

## Music

| Path | MusicId | Usage |
|---|---|---|
| `assets/sounds/music/overworld.flac` | `OVERWORLD` | `Runtime` — Level 1. |
| `assets/sounds/music/underground.flac` | `UNDERGROUND` | `Runtime` — Level 2. |
| `assets/sounds/music/castle.flac` | `CASTLE` | `Runtime` — Level 3. |
| `assets/sounds/music/invincible.flac` | `STAR` | `Runtime` catalog — Star override. |
| `assets/sounds/music/death.flac` | `DEATH` | `Runtime` catalog — death state. |
| `assets/sounds/music/gameover.flac` | `GAME_OVER` | `Runtime` catalog — GameOver state. |
| `assets/sounds/music/level_complete.flac` | `WIN` | `Runtime` catalog — Win/level complete. |
| `assets/sounds/music/castle_complete.flac` | — | `Future` — optional castle completion variant. |
| `assets/sounds/music/pipe.flac` | — | `Future` — pipe transition variant. |
| `assets/sounds/music/underwater.flac` | — | `Future` — underwater theme not in release catalog. |

## Sprite crop notes

Các ghi chú crop trước đây đã được hợp nhất tại đây và cập
nhật theo `include/core/SpriteFrames.h`:

- QuestionBlock dùng `items_blocks.png`, các frame 16×16 tại
  `(80,112)`, `(96,112)`, `(112,112)`; used block bắt đầu tại `(128,112)`.
- Coin dùng `items_objects.png`, bốn frame 8×16 tại
  `(180,36)`, `(190,36)`, `(200,36)`, `(210,36)`; khoảng cách giữa frame là
  2 pixel.
- Tile catalog dùng `assets/textures/tiles/tileset.png`: `TileMap` đọc các
  frame ground/stone/brick/used/question/pipe/flag từ `TileFrames.h`.
- Không tạo file crop riêng như `idle.png`, `coin.png`, `mushroom.png` hoặc
  `star.png`; animation phải cắt từ spritesheet bằng named frame constants.

## Reference/source files ngoài runtime

Các file dùng để đo/cắt hoặc kiểm tra component đã chuyển ra khỏi `assets/`
để CMake không package chúng:

| Path | Kích thước | Usage |
|---|---:|---|
| `docs/assets/reference/enemies.png` | 436×530 | `Reference` — atlas enemy dùng để đo/cắt. |
| `docs/assets/reference/enemies_all_components_atlas.png` | 880×1400 | `Reference` — atlas phân tích component enemy cũ. |
| `docs/assets/reference/enemies_all_components_atlas_full.png` | 800×1280 | `Reference` — atlas component lấy từ runtime history; không package. |
| `docs/assets/reference/enemies_candidate.png` | 128×96 | `Reference` — candidate sheet chưa có loader runtime. |
| `docs/assets/reference/blocks_all_components_atlas_full.png` | 800×800 | `Reference` — atlas phân tích block; không package. |
| `docs/assets/reference/items_objects_all_components_atlas_full.png` | 960×1840 | `Reference` — atlas phân tích item; không package. |
| `docs/assets/reference/items_blocks_candidate.png` | 96×96 | `Reference` — candidate sheet chưa có loader runtime. |
| `docs/assets/reference/items_objects_candidate.png` | 128×64 | `Reference` — candidate sheet chưa có loader runtime. |
| `docs/assets/reference/tileset_candidate.png` | 160×32 | `Reference` — candidate tileset chưa được chọn cho release. |
| `docs/assets/reference/bg_world_candidate.png` | 1857×847 | `Reference` — candidate background trùng kích thước nhưng chưa có runtime path. |
| `docs/assets/reference/general_tile.png` | 680×776 | `Reference` — bản duplicate/source tile. |
| `docs/assets/reference/tileset(v2).png` | 680×776 | `Reference` — legacy tile source. |

Không dùng absolute path trong code và không copy reference files vào build
runtime package.

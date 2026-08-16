# Develop Integration Log

This file summarizes important integration checkpoints. Git history remains the authoritative record of individual commits and merges. Historical test counts are not current release evidence.

| Date | Checkpoint | Result | Notes |
|---|---|---|---|
| 2026-08-08 | Initial Sprint 6 integration | 7/7 CTest | Superseded by later implementation |
| 2026-08-09 | Evaluation v3 integration | 12/12 CTest | Superseded by later implementation |
| 2026-08-11 | `origin/develop` at `3047252` | 14/14 CTest | Review v4 reopened 40 tasks |
| 2026-08-12 | Sprint 6 remediation based on `3047252` | Clean Debug/Release/Tests PASS; CTest 17/17 | no-op asset build PASS; final commit/manual evidence pending |
| 2026-08-12 | BUG-042 body-tier FireFlower/transform fix | Debug/Release PASS; CTest 17/17 | Small/Super Fire forms retain body size; transformation frames update continuously |
| 2026-08-13 | Merge `feature/sound-input` into `develop` | Fast-forward merge `b9622fb` | Updated local `feature/sound-input` from `origin` and merged into `develop` |
| 2026-08-13 | Underground Castle sprite support for Level 2 | CTest 17/17 PASS | Added `TileFrames::CASTLE_UNDERGROUND` (419,196,80,80) for `LevelTheme::UNDERGROUND` in Level 2 |
| 2026-08-13 | Remove all blue background shades from tileset | Image processed | Converted all 112,463 background pixels matching blue shades `(r<=5, 35<=g<=45, 135<=b<=145)` in `assets/textures/tiles/tileset.png` to transparent (0, 0, 0, 0) |
| 2026-08-13 | Update HARD_BLOCK_UNDERGROUND texture rect | CTest 17/17 PASS | Changed `HARD_BLOCK_UNDERGROUND` sprite rect to `(147, 33)` {16, 16} in `TileFrames.h` and test suite |
| 2026-08-13 | Fix IDE warnings & directly reference facade symbol | Clean compile | Added `SpriteFrames::IS_FACADE_ACTIVE`, annotated headers with `IWYU pragma: export` and suppressed unused diagnostics inside `SpriteFrames.h` so IDE linter recognizes it cleanly |
| 2026-08-13 | Elevator (moving platform) feature | CTest 18/18 PASS | New `Elevator` kinematic entity riding on `^`/`~` tile markers (vertical/horizontal, end-pause) + external `levels/elevators.txt` routes + theme-specific platform sprite frames + `ElevatorTests`. Map markers must use empty cells and should not duplicate a config route; the active level theme selects the platform palette. |
| 2026-08-14 | Cheep Cheep enemy & external spawn feature | CTest 19/19 PASS | Added `CheepCheep` enemy class with swimming and jumping mechanics adhering to SMB1 NES canon; added external spawn registry `levels/cheep_cheep.txt` & camera leap generator without modifying any existing map files (`level*.txt`); added `CheepCheepTests` with 100% pass rate. |
| 2026-08-14 | Merge `feature/sound-input` (Mario/Luigi Character Select) | CTest 20/20 PASS | Integrated character selection flow (`CharacterSelectState`), Mario vs. Luigi physics profiles, Luigi Grow/Shrink/Fire animation atlases, session persistence in `GameProgress`, GameOver retry retention, and `character_flow_tests`. |
| 2026-08-14 | Fix Mario & Luigi Animation & Invincibility Logic | CTest 20/20 PASS | Added transform presentation sequence for same-body-tier Fire transitions (`SMALL<->FIRE_SMALL`, `SUPER<->FIRE_SUPER`), preserved fireball shoot pose (`action`) during movement, resolved Starman rainbow timer underflow bug, and prevented transform completion from overriding active damage grace blinking. |
| 2026-08-14 | Fix Luigi Fire Transformation & Shared Fire Form Sprites | CTest 20/20 PASS | Unified FIRE states (`FIRE_SMALL`, `FIRE_SUPER`) to use canonical Fire Mario sprite sheets for both Mario & Luigi; corrected Luigi FireFlower upgrade animation to use `FireMario::growSequence()` (fire flash palette) instead of mistakenly playing `Luigi::growSequence()` (small-to-big mushroom growth). |
| 2026-08-14 | Pipe Warp Smooth Entry Animation & Transition Delay | CTest 20/20 PASS | Added smooth pipe descent/entry animation (0.5s slide at 48 px/s behind foreground tiles), authentic pipe travel audio cue (`powerdown`), and deliberate 0.35s warp transition delay before Mario emerges at destination. |
| 2026-08-14 | Fix Fireball Shooting Poses & Natural Pipe Crouch/Walk Animations | CTest 20/20 PASS | Replaced fireball projectile frames with authentic NES character throw poses (`FireBigMario::ACTION`, `FireSmallMario::WALK3`); added dynamic `crouch` state for Super/Fire forms; enabled live animation advancing (`updateVisuals`) during pipe entry and smooth horizontal alignment during pipe descent. |
| 2026-08-14 | Pipe Exit / Emerging Animation & Complete Luigi Warp Parity | CTest 20/20 PASS | Added smooth pipe emergence animation (`EXITING_VERTICAL`, rising UP out of destination pipe with `powerdown` audio cue); verified 100% full parity for Luigi (including Luigi crouch/walk/idle and sprite frames during pipe entrance/exit). |
| 2026-08-15 | Sprint 7 TV1 4-Level Release Contract (`S7-TV1-01`..`04`) | CTest 20/20 PASS (historical snapshot; not current RC evidence) | Formalized 4-level release graph: `1-1 Overworld` -> `1-2 Underground` -> `1-3 Underwater` -> `1-4 Castle` -> `Win`. Updated `LevelCatalog.h`, `level4.txt`, `LevelCatalogTests.cpp`, `PlayStateTests.cpp`, `Gate0ContractTests.cpp`, and `TV5IntegrationTests.cpp`. |
| 2026-08-15 | Stage Select UI & Flow Integration (`LevelSelectState`) | CTest 20/20 PASS (historical snapshot; not current RC evidence) | Created `LevelSelectState` presenting 4 themed cards (Overworld, Underground, Underwater, Castle) with keyboard and mouse interaction. Wired Title Menu `START GAME` -> `LevelSelectState` -> `CharacterSelectState(selectedLevel)` -> `PlayState(selectedLevel, characterType)`. |
| 2026-08-15 | Code Review Remediation for `cc80466` (P0/P1/P2 fixes) | CTest 20/20 PASS (historical snapshot; not current RC evidence) | Eliminated hot-loop `dynamic_cast` via virtual `isCoin()`; decomposed `Mario::update()` (~40 lines) and `Level::update()` (~45 lines) into focused helpers; enforced Box2D physics sync at line 1 of update loop; decomposed `onEnter()` in `LevelSelectState` & `CharacterSelectState`; cleaned `level4.txt` trailing lines; added automated validation for all 4 release levels in `LevelValidatorTests.cpp`. |
| 2026-08-16 | Fix Fire State Growth/Shrink Transformation Animation | CTest 21/21 PASS | Fixed `Mario::applyStateTransition` to prioritize `usesFireTransition` over `m_characterType == LUIGI` so Fire Small -> Fire Super uses Fire transformation frames instead of normal green Luigi frames; added regression tests. |
| 2026-08-16 | Multi-kill Combo Scoring & Defeat Score Popup Fixes | CTest 21/21 PASS | Fixed missing score popups for FireBall, sliding Koopa shell, Star, and block bump kills; implemented sliding shell combo kill streak (200..8000, 1UP); verified airborne stomp multi-kill handling. |
| 2026-08-16 | Item/Enemy Theme Support, Pixel Sharpness & Title Screen Redesign | CTest 21/21 PASS | Added `LevelTheme` to `Coin` and `Mushroom`; fixed `CheepCheep` red swim frames across all themes; redesigned `levels/level3.txt` (1-3 Underwater); disabled font smoothing for crystal-clear UI; redesigned `MenuState` Title Screen with full scenery, HUD, and live character animation. |
| 2026-08-16 | Fix Score Popup Texture Rect Mapping in Shared Atlas | CTest 21/21 PASS | Corrected completely scrambled score popup texture rect coordinates in `SpriteFrames_shared.h` (SCORE_200..8000, 1UP) so displayed visual score popups precisely match the points awarded to Mario; added `SCORE_500` support. |
| 2026-08-16 | Multi-Theme Score Popups & Title Screen Layout Fine-tuning | CTest 21/21 PASS | Implemented theme-aware score popups across all 4 environments (Overworld, Underground, Castle, Underwater); removed clunky bounding boxes and overlapping blocks from `MenuState` Title Screen for authentic NES presentation; verified with render snapshot tests. |
| 2026-08-16 | Title Cover Screen Branding & Clutter Cleanup | CTest 21/21 PASS | Removed gameplay HUD banner (`MARIO 000000 | x00 WORLD 1-1 TIME 400`) and arbitrary high-score line from Title cover screen; added project credit `CS202 - OOP FINAL PROJECT` and team name `GROUP 04`; polished vertical layout and spacing. |
| 2026-08-16 | Title Screen Pipe Symmetry & Title Punctuation Cleanup | CTest 21/21 PASS | Symmetrically balanced left pipe (`x=32..96`) and right pipe (`x=544..608`) with exact 32px screen edge margins; centered Goomba (`x=320`) and balanced Mario (`x=200`) and Luigi (`x=440`); removed trailing dot from `SUPER MARIO BROS`. |
| 2026-08-16 | Authentic NES Underwater Cheep Cheep Sprite Configuration | CTest 21/21 PASS | Configured `SpriteFrames_udw.h` and `Level.cpp` to use the authentic NES 1985 grey/white underwater palette (`UW_SWIM_UP {292, 164}`) for Cheep Cheeps swimming in World 1-3. |

## 2026-08-12 remediation scope

- Restored the locked three-level graph and Level 3 Castle -> Win boundary.
- Restored deterministic adaptive QuestionBlocks and explicit required item routes.
- Preserved Small Fire versus Super Fire body forms and fixed the transition renderer to advance atlas frames while movement is frozen.
- Unified Mario growth/fixture transitions, independent timers, death completion, and respawn reset.
- Consolidated FireBall requests/storage/events and added world-lock coverage.
- Completed collision order/idempotence and one-event/one-SFX coverage.
- Added metadata-driven camera Y policy, full X/Y clamp, integer letterboxing, and physical-to-logical input mapping.
- Synchronized shared UI behavior, final/high-score screens, theme backgrounds, tests, and release documentation.

## Evidence policy

The active status is maintained in:

- [Sprint 6 audit tracker](management/S6_AUDIT_TRACKER.md)
- [Sprint 6 bug register](management/S6_BUG_REGISTER.md)
- [TV1 integration report](management/TV1_CHANGES_SUMMARY.md)
- [TV4 release playthrough log](testing/TV4_PLAYTHROUGH_LOG.md)
- [TV5 audio/HUD/item checklist](management/TV5_AUDIO_HUD_ITEM_CHECKLIST.md)

A working-tree result may support review, but release sign-off requires one immutable commit hash shared by clean builds and every manual evidence record.
---

## 3. MODIFIED FILES LOG (Chi tiết các file đã sửa)

- **[Mario.cpp](../src/entities/Mario.cpp)**:
  - `Dòng 230`: Bổ sung lời gọi `updateInvincibility(dt)` trong hàm `update(float dt)`.
- **[EntityFactory.cpp](../src/patterns/EntityFactory.cpp)**:
  - Khởi tạo mã tile `?` cố định thành `QuestionBlockContent::SUPER_MUSHROOM` (chắc chắn luôn ra Nấm Super Mushroom khi đập).
- **[QuestionBlock.h](../include/entities/QuestionBlock.h)**:
  - Thêm enum `BlockTheme` và cập nhật constructor nhận tham số `BlockTheme theme = BlockTheme::OVERWORLD`.
- **[QuestionBlock.cpp](../src/entities/QuestionBlock.cpp)**:
  - Nạp chuỗi animation `idle` và sprite `empty` linh hoạt theo `BlockTheme` từ `SpriteFrames::Blocks`.
- **[TileMap.cpp](../src/level/TileMap.cpp)**:
  - Cập nhật hàm `TileMap::hitTile()` truyền `TextureManager` và gắn 4 sub-rect mảnh vỡ gạch 4 góc (`DEBRIS_TOP_LEFT`, `DEBRIS_TOP_RIGHT`, `DEBRIS_BOTTOM_LEFT`, `DEBRIS_BOTTOM_RIGHT`) trực tiếp vào 4 đối tượng `BlockDebris` khi Mario to húc vỡ gạch Brick Block.
- **[SpriteFrames.h](../include/core/SpriteFrames.h)**:
  - Cập nhật namespace `SpriteFrames::Blocks` với đầy đủ tọa độ chuẩn của 120 ô khối gạch (`items_blocks.png`) qua 4 bảng màu NES (Overworld, Underground, Castle, Underwater). Bổ sung 4 sub-rect 8×8 px (`DEBRIS_TOP_LEFT`, `DEBRIS_TOP_RIGHT`, `DEBRIS_BOTTOM_LEFT`, `DEBRIS_BOTTOM_RIGHT`) và các hàm animation helper `ugQuestionBlockFrames()`, `castleQuestionBlockFrames()`, `uwQuestionBlockFrames()`.
- **[BlockDebris.h](../include/entities/BlockDebris.h)**:
  - Cập nhật constructor nhận tham số `sf::IntRect frame` tùy chọn.
- **[BlockDebris.cpp](../src/entities/BlockDebris.cpp)**:
  - Đổi rect mặc định của `BlockDebris` sang `SpriteFrames::Blocks::DEBRIS_TOP_LEFT`, bổ sung `updateAnimation(dt)` và thiết lập tâm xoay `origin` + tỉ lệ phóng `scale 2.0x` cho hạt mảnh vỡ.
- **[blocks_coordinate.md](blocks_coordinate.md)**:
  - Phân tích toàn bộ 120 ô khối gạch & hộp quà nghi vấn `?` (Brick Blocks, Question Blocks, Brick Debris) trích xuất trực tiếp từ tệp hình ảnh gốc **`assets/textures/items/items_blocks.png`** (kích thước 448×256 px). Bổ sung hướng dẫn kỹ thuật tách 4 sub-rect 8×8 px cho 16 ô mảnh vỡ gạch (`BlockDebris`).
- **[level3.txt](../levels/level3.txt)**:
  - Khắc phục cấu trúc cột cờ về đích (`F` và `|` cột cờ) bị thiếu nối xuống ô đất `1`, làm hàm `TileMap::loadFromFile()` ném lỗi validation và thoát về Menu thay vì vào màn Level 3 / YOU WIN.
- **[blocks_all_components_atlas_full.png](assets/reference/blocks_all_components_atlas_full.png)**:
  - Tệp ảnh Atlas minh họa trực quan (880×1050 px) hiển thị 120 ô khối được trích xuất và gán nhãn chỉ số #1–#120 trực tiếp từ tệp **`assets/textures/items/items_blocks.png`**.
- **[Mario.h](../include/entities/Mario.h)**:
  - Thêm enum `CharacterType` (`MARIO`, `LUIGI`), thiết lập nhân vật mặc định là `CharacterType::LUIGI`, bổ sung `getCharacterType()` và `setCharacterType()`.
- **[Mario.cpp](../src/entities/Mario.cpp)**:
  - Cập nhật `setupAnimationsForState` hỗ trợ nạp bộ clip animation của Luigi (`SmallLuigi`, `BigLuigi`) khi `charType == CharacterType::LUIGI`. Trạng thái Lửa `MarioState::FIRE` dùng chung bộ frame `SpriteFrames::FireBigMario`.
  - Cập nhật các constructors, `powerUp()`, `respawn()`, `setMarioState()`, và hàm `setCharacterType()`.
  - Khắc phục chuỗi animation `transform` trong `applyStateTransition()` cho các biến đổi cùng thể lực (`SMALL <-> FIRE_SMALL`, `SUPER <-> FIRE_SUPER`).
  - Bảo lưu clip animation `"action"` trong `update()` khi vừa di chuyển vừa ném đạn lửa.
  - Sửa chu kỳ màu Sao bất tử trong `updateInvincibility()` tránh tràn số âm và đồng bộ màu nhấp nháy với `m_isTransforming`.

---

## 4. DETAILED LOGIC CHANGE LOG (For Opus Review)

### Entry #47: [Feature & Visual Polish] - Hoạt Ảnh Chui Ra Khỏi Cống (Pipe Exit / Emerging) & Đồng Bộ Hoàn Hảo Cho Luigi
- **Trạng thái:** Đã hoàn thành 100%, 20/20 CTest passed.
- **File ảnh hưởng:** `include/level/Level.h`, `src/level/Level.cpp`, `docs/change_in_develop.md`.
- **Mô tả:**
  1. **Bổ sung pha diễn hoạt Chui Ra Khỏi Cống (`PipeWarpPhase::EXITING_VERTICAL`)**:
     - Khi điểm đến (`R`) nằm trên miệng ống nước (tile phía dưới là `[` / `]` / `p` / `r` / `{` / `}`):
       * Nhân vật khởi tạo ở vị trí ngập sâu bên trong ống nước (`target.y + marioSize.y`).
       * Kích hoạt âm thanh chui cống (`pipepowerdown.wav`).
       * Nhân vật trượt từ từ vươn lên (`rising UP`) với tốc độ $48\text{ px/s}$ trong $0.45\text{s}$ cho đến khi hai chân đặt vững chắc trên mép miệng cống.
       * Khi hoàn tất, hệ thống tự động hoàn trả trọng lực và trao lại quyền điều khiển mượt mà cho người chơi.
     - Khi điểm đến là phòng trống/trần nhà (như bonus underground room `R1`): Nhân vật xuất hiện tại vị trí và rơi tự do theo trọng lực bình thường.
  2. **Đồng bộ toàn diện cho Luigi**:
     - Luigi thừa hưởng trọn vẹn toàn bộ chuỗi animation chui cống / ra cống:
       * Cúi khom người (`SpriteFrames::shared::BigLuigi::CROUCH` / `FireBigMario::CROUCH`) khi trượt vào cống đứng thể lớn.
       * Bước đi (`SmallLuigi` / `BigLuigi` / `FireMario`) với nhịp chân cập nhật theo thời gian thực khi vào cống ngang.
       * Trượt vươn lên mượt mà khi chui ra khỏi cống ở điểm đến.

### Entry #46: [Bugfix & Visual Polish] - Sửa Động Tác Bắn Đạn Lửa (Throwing Pose) & Tinh Chỉnh Hoạt Ảnh Chui Cống (Crouch / Walk Animation)
- **Trạng thái:** Đã hoàn thành 100%, 20/20 CTest passed.
- **File ảnh hưởng:** `include/entities/Mario.h`, `src/entities/Mario.cpp`, `include/level/Level.h`, `src/level/Level.cpp`, `tests/MarioPhysicsTests.cpp`, `docs/change_in_develop.md`.
- **Mô tả:**
  1. **Khắc phục lỗi nhân vật biến thành đốm lửa khi ném đạn (`setupAnimationsForState`)**:
     - Thay thế các frame nhầm lẫn tọa độ đạn lửa/đốm nổ `SMALL_SHOOT1..3` và `BIG_SHOOT` bằng các tư thế ném đạn chính thống của Mario/Luigi:
       * `FIRE_SUPER`: Dùng `SpriteFrames::shared::FireBigMario::ACTION` (Frame 14, `{264, 139}, {16, 32}`) - tư thế vung tay ném cầu lửa chuẩn NES.
       * `FIRE_SMALL`: Dùng `SpriteFrames::shared::FireSmallMario::WALK3` (Frame 3, `{56, 116}, {16, 16}`) - tư thế đưa tay ra trước ném đạn.
     - Cập nhật kiểm thử `testLuigiFireBodyUsesLuigiRows` trong `tests/MarioPhysicsTests.cpp`.
  2. **Bổ sung tư thế cúi ngồi (`crouch`) cho Mario & Luigi**:
     - Nạp animation `"crouch"` với các frame `BigMario::CROUCH`, `BigLuigi::CROUCH` và `FireBigMario::CROUCH`.
     - Kích hoạt `"crouch"` khi người chơi giữ phím Xuống trên mặt đất ở thể Lớn/Lửa trong `Mario::update`.
  3. **Nâng cấp hoạt ảnh chui cống sống động, tự nhiên**:
     - Khi chui vào cống ngang (`H`): Mario bước đi liên tục (`"walk"` với các frame chân chuyển động theo thời gian thực nhờ `updateVisuals(dt)`).
     - Khi chui xuống cống đứng (`[` / `p`): Mario thể lớn tự động cúi người (`"crouch"`), Mario thể nhỏ bước/đứng tự nhiên, vị trí ngang được nội suy mượt mà vào tâm miệng cống thay vì dịch chuyển giật cục.
     - Diễn hoạt liên tục cập nhật sprite frames trong suốt quá trình chui cống.

### Entry #45: [Feature & Visual Polish] - Hoạt Ảnh Chui Cống Mượt Mà, Âm Thanh & Độ Trễ Chuyển Cảnh Khi Warp
- **Trạng thái:** Đã hoàn thành 100%, 20/20 CTest passed.
- **File ảnh hưởng:** `include/level/Level.h`, `src/level/Level.cpp`, `docs/change_in_develop.md`.
- **Mô tả:**
  1. **Bổ sung State Machine Chui Cống (`PipeWarpPhase`)**: Thiết kế các pha chuyển tiếp cống `ENTERING_VERTICAL`, `ENTERING_HORIZONTAL` và `WARPING_DELAY`.
  2. **Hoạt ảnh thụt vào cống mượt mà (`startPipeWarp` & `updatePipeWarp`)**:
     - Cống thẳng đứng (`[` / `p`): Khi đứng trên miệng cống và nhấn phím Xuống, Mario được căn giữa miệng cống, phát âm thanh chui cống kinh điển (`pipepowerdown.wav`), và trượt dần xuống miệng cống với tốc độ $48\text{ px/s}$ trong $0.5\text{s}$ (ẩn dần phía sau lớp foreground tile của cống).
     - Cống nằm ngang (`H`): Khi đi sang phải chạm miệng cống ngang, Mario chơi hoạt ảnh bước đi và trượt dần vào trong lòng cống trong $0.5\text{s}$.
  3. **Độ trễ chuyển cảnh tự nhiên (`WARPING_DELAY`)**: Sau khi chui vào cống, game tạo khoảng delay $0.35\text{s}$ để camera và vị trí dịch chuyển tới điểm đến (`R`), loại bỏ hoàn toàn cảm giác giật dịch chuyển tức thời. Tổng thời gian diễn hoạt chuyển cống là $\approx 0.85\text{s}$ chuẩn mực theo phong cách NES SMB1.
  4. **Bảo toàn tương tác vật lý & va chạm**: Khóa di chuyển vật lý và miễn nhiễm sát thương trong suốt thời gian diễn hoạt chui cống, tự động phục hồi trọng lực và quyền điều khiển ngay khi kết thúc warp.

### Entry #44: [Bugfix & Visual Polish] - Sửa Lỗi Hoạt Ảnh Biến Hình Hoa Lửa (Giữ Đúng Cỡ Thân) & Đồng Bộ Dạng Lửa (Fire Form) Cho Luigi
- **Trạng thái:** Đã hoàn thành 100%, 20/20 CTest passed.
- **File ảnh hưởng:** `include/core/SpriteFrames_shared.h`, `src/entities/Mario.cpp`, `tests/MarioPhysicsTests.cpp`, `docs/change_in_develop.md`.
- **Mô tả:**
  1. **Đồng bộ hóa bộ Sprite Dạng Lửa (`MarioState::FIRE_SMALL`, `MarioState::FIRE_SUPER`)**: Quy chuẩn Luigi và Mario dùng chung bộ sprite form Lửa (`SpriteFrames::FireSmallMario` và `SpriteFrames::FireBigMario`). Khắc phục lỗi Luigi khi ở trạng thái Lửa lại dùng frame `SmallLuigi`/`BigLuigi` màu xanh lá thông thường.
  2. **Tách chuỗi biến hình Lửa theo cấp thân (`smallFireSequence` & `bigFireSequence`)**: Bổ sung các chuỗi chớp nháy màu lửa đồng cấp kích thước trong `SpriteFrames_shared.h` (dùng các frame màu thay thế `ALT1`, `ALT2`):
     - `smallFireSequence()`: Chuỗi frame 16x16 `{SMALL, SMALL_ALT1, SMALL_ALT2, ...}` cho thể nhỏ.
     - `bigFireSequence()`: Chuỗi frame 16x32 `{BIG, BIG_ALT1, BIG_ALT2, ...}` cho thể lớn.
  3. **Khắc phục lỗi "Khổng Lồ Hóa" khi ăn Hoa Lửa (`Mario::applyStateTransition`)**: Tách biệt rõ ràng điều kiện Fire Transition (`isFireUpgrade` / `isFireDowngrade`) với Body Growth (`isGrowth` / `isShrink`). Khi Mario/Luigi ở thể nhỏ ăn Hoa Lửa, game kích hoạt đúng `smallFireSequence()` giữ nguyên chiều cao 16px và chỉ chớp màu lửa, triệt tiêu hoàn toàn hiện tượng bị phóng to lên Big/Medium rồi mới thu nhỏ về.
  4. **Cập nhật Unit Test kiểm thử (`tests/MarioPhysicsTests.cpp`)**: Điều chỉnh `testLuigiFireBodyUsesLuigiRows` kiểm tra chính xác các frame Lửa (`FireSmallMario`, `FireBigMario`) và khả năng bắn đạn lửa cho Luigi. 100% test suite (20/20 CTest) vượt qua thành công.

### Entry #43: [Bugfix & Visual Polish] - Khắc Phục Hoạt Ảnh Biến Hình Cùng Thể Thân, Động Tác Ném Lửa & Logic Bất Tử Cho Mario / Luigi
- **Trạng thái:** Đã hoàn thành 100%, 20/20 CTest passed.
- **File ảnh hưởng:** `src/entities/Mario.cpp`, `tests/ElevatorTests.cpp`, `tests/CheepCheepTests.cpp`, `docs/change_in_develop.md`.
- **Mô tả:**
  1. **Hoạt ảnh biến hình khi ăn hoa lửa (`Mario::applyStateTransition`)**: Bổ sung xử lý `isFireUpgrade` và `isFireDowngrade` khi chuyển trạng thái giữa các dạng cùng thể lực (`SMALL <-> FIRE_SMALL`, `SUPER <-> FIRE_SUPER`). Đảm bảo animation `transform` luôn được nạp chuỗi `growSequence()` / `shrinkSequence()` tương ứng thay vì bị bỏ qua làm nhân vật bị đơ cứng ở thế đứng `idle`.
  2. **Bảo toàn động tác ném đạn lửa (`Mario::update`)**: Trong thang ưu tiên animation của `update()`, bảo lưu clip `"action"` (ném lửa) khi `!m_animationSystem->isFinished()`. Khắc phục lỗi khi Mario/Luigi vừa chạy/nhảy vừa bắn đạn thì clip ném lửa bị frame `walk`/`jump` ghi đè ngay ở frame kế tiếp.
  3. **Sửa lỗi tính chu kỳ màu Sao Bất Tử (`Mario::updateInvincibility`)**: Thay thế phép tính hardcode `(10.f - m_starInvincibilityTimer)` bằng công thức modulo dương `(static_cast<int>(m_starInvincibilityTimer * 15.f) % 6 + 6) % 6`. Loại bỏ hoàn toàn lỗi tràn số âm khiến nhân vật bị kẹt cứng ở màu đỏ khi thời gian sao $>10$s.
  4. **Đồng bộ nhấp nháy Biến hình và Bất tử do sát thương**: Bổ sung guard khi kết thúc biến hình (`m_transformTimer <= 0.f`), chỉ reset màu về `White` nếu nhân vật không còn trong thời gian bất tử `isInvincible` / `isStarInvincible`.
  5. Cập nhật và đồng bộ tọa độ kiểm thử `ElevatorTests.cpp` và `CheepCheepTests.cpp`, đảm bảo toàn bộ 20/20 bài kiểm thử CTest đạt 100%.

### Entry #42: [Feature & Map Layout] - Tinh Chỉnh Cụm Elevator, Bổ Sung Tuyến Cheep Cheep Thứ 2 & Tăng Tốc Độ Chim Bay
- **Trạng thái:** Đã hoàn thành 100%, 20/20 CTest passed.
- **File ảnh hưởng:** `levels/level2.txt`, `src/level/Level.cpp`, `tests/ElevatorTests.cpp`, `tests/CheepCheepTests.cpp`, `docs/change_in_develop.md`.
- **Mô tả:**
  1. **Lùi cụm thang máy trước sang phải:** Dịch chuyển Elevator 1 sang cột `191` (Start `(191, 13)`, End `(191, 7)`) và Cheep Cheep 1 sang cột `189` (Start `(189, 13)`, End `(189, 7)`), tạo khoảng cách nhảy cách đều và thẩm mỹ ở hố vực thứ nhất.
  2. **Bổ sung cụm Cheep Cheep thứ 2 trước thang máy sau:** Đặt Cheep Cheep 2 tại cột `203` (Start `(203, 13)`, End `(203, 4)`) ngay trước Elevator 2 (cột `205`, Start `(205, 12)`, End `(205, 4)`), tạo nhịp thử thách đồng bộ và đối xứng cho người chơi khi vượt qua 2 cụm vực sâu liên tiếp trong Level 2.
  3. **Tăng tốc độ bay cho Cheep Cheep:** Trong `Level::spawnCheepCheepRoutesFromTileMap()` (`src/level/Level.cpp`), tăng tốc độ route của Cheep Cheep từ $40\text{ px/s}$ lên $75\text{ px/s}$ (nhanh hơn tốc độ mặc định $60\text{ px/s}$ của Elevator), giúp cá bay lượn linh hoạt và tạo độ nhịp điệu cao hơn thang máy.
  4. Cập nhật các bộ kiểm thử `tests/ElevatorTests.cpp` và `tests/CheepCheepTests.cpp`, đồng bộ file map và kiểm thử toàn diện đạt 100% (20/20 CTest passed).

### Entry #41: [Feature & State Flow] - Tích Hợp Luồng Chọn Nhân Vật Mario / Luigi & Đặc Tính Vật Lý Riêng Biệt
- **Trạng thái:** Đã hoàn thành 100%, 20/20 CTest passed.
- **File ảnh hưởng:** `include/states/CharacterSelectState.h`, `src/states/CharacterSelectState.cpp`, `include/core/GameProgress.h`, `include/entities/Mario.h`, `src/entities/Mario.cpp`, `include/level/Level.h`, `src/level/Level.cpp`, `include/states/GameOverState.h`, `src/states/GameOverState.cpp`, `include/states/MenuState.h`, `src/states/MenuState.cpp`, `include/states/PlayState.h`, `src/states/PlayState.cpp`, `CMakeLists.txt`, `tests/CharacterFlowTests.cpp`, `tests/Gate0ContractTests.cpp`, `tests/LevelCatalogTests.cpp`, `tests/MarioPhysicsTests.cpp`.
- **Mô tả:**
  1. Thêm màn hình `CharacterSelectState` cho phép người chơi chọn Mario hoặc Luigi từ Main Menu trước khi vào `PlayState`.
  2. Bổ sung profile vật lý riêng biệt `CharacterProfile` cho Mario và Luigi:
     - **Mario:** Chạy nhanh hơn (walk: 175 px/s, run: 280 px/s), lực nhảy tiêu chuẩn (460).
     - **Luigi:** Nhảy cao hơn (+10.8% lực nhảy: 510), chạy chậm hơn (~ -10% tốc độ: walk 160 px/s, run 250 px/s), tốc độ dưới nước cũng được scale tương ứng.
  3. Cập nhật Sprite atlas cho Luigi: nạp đúng chuỗi biến hình Grow/Shrink từ `SpriteFrames::shared::GrowShrink::Luigi`, các animation Lửa (`FIRE_SMALL`, `FIRE_SUPER`) giữ thân Luigi và nạp động tác bắn lửa `SpriteFrames::shared::FireShooting::Luigi`.
  4. Lưu trữ danh tính `CharacterType` trong `GameProgress`, tự động duy trì qua các màn (Level 1..4), khi respawn và khi nhấn **RETRY** ở màn `GameOverState`.
  5. Thêm cờ chống double-transition (`m_transitioning`) trong các UI States để tránh lỗi duplicate state khi spam phím/chuột.
  6. Bổ sung bộ test `character_flow_tests` và mở rộng các test contract, kiểm thử đạt 100% (20/20 CTest suites).

### Entry #40: [Feature] - Triển Khai Quái Cheep Cheep (Swimming & Jumping) & Cơ Chế Spawn Ngoài Không Đổi Map
- **Trạng thái:** Đã hoàn thành 100%, 19/19 CTest passed.
- **File ảnh hưởng:** `include/entities/CheepCheep.h`, `src/entities/CheepCheep.cpp`, `include/level/CheepCheepConfig.h`, `src/level/CheepCheepConfig.cpp`, `levels/cheep_cheep.txt`, `include/core/SpriteFrames_ovw.h`, `include/core/SpriteFrames_udg.h`, `include/core/SpriteFrames_castle.h`, `include/core/SpriteFrames_udw.h`, `include/entities/Entity.h`, `include/patterns/EntityFactory.h`, `src/patterns/EntityFactory.cpp`, `src/physics/CollisionManager.cpp`, `include/level/Level.h`, `src/level/Level.cpp`, `tests/CheepCheepTests.cpp`, `CMakeLists.txt`.
- **Mô tả:**
  1. Thêm lớp `CheepCheep` kế thừa từ `Enemy` hỗ trợ 2 hành vi chuẩn NES canon:
     - `SWIMMING`: Bơi ngang dưới nước (vận tốc đều cho cá xanh, vận tốc sóng sin $y(t) = y_0 + A\sin(\omega t)$ cho cá đỏ), không thể giẫm (chạm vào gây sát thương cho Mario), tiêu diệt được bằng FireBall và Starman.
     - `JUMPING`: Nhảy vọt từ đáy viewport camera lên theo parabol trọng lực, Mario có thể giẫm (`onStomp`) để nảy lên và cộng 200 điểm, hoặc bắn FireBall.
  2. Bổ sung hỗ trợ cặp ký hiệu tuyến đường bay trực tiếp trong bản đồ map `.txt` (như thang máy `^`/`~`): dùng `'c'` làm điểm bắt đầu (Start) và `'x'` làm điểm kết thúc (End). Nếu cùng cột sẽ tự động bay lên $\leftrightarrow$ xuống tuần hoàn; nếu cùng hàng sẽ bơi/bay ngang qua $\leftrightarrow$ lại tuần hoàn với tốc độ vừa phải ($40\text{ px/s}$). Nếu đứng đơn lẻ `'c'` sẽ là cá bơi tự do, `'H'` là cá nhảy từ đáy vực.
  3. Cập nhật `TileMap.h`, `TileMap.cpp` (`VALID_TILE_SYMBOLS`, `parseAndNormalizeCheepCheeps`), `Level.h`, `Level.cpp` và các file tài liệu (`FILE_STRUCTURE.md`, `CODING_RULES.md`, `.agents/rules/filestructure.md`, `levels/cheep_cheep.txt`).
  4. Chuẩn hóa bộ palette sprite theme cho Cheep Cheep: tự động lấy đúng màu xám xanh Underground `(146, 164)` khi ở màn 2/Underground, màu đỏ/xanh lá ở Overworld, và màu Underwater khi ở môi trường nước.
  5. Sửa lỗi xung đột ký tự `'H'`: xóa `'H'` khỏi `SPAWN_CODES` để bảo toàn tuyệt đối các cửa cống chuyển cảnh ngang (`H1`, `H3`, `H4`) trong Level 2, tránh hiện tượng sinh quái nhầm ở đầu cống.
  6. Mở rộng bộ unit test `cheep_cheep_tests` kiểm thử toàn diện cả tuyến đường (routes) từ file map & file config, vận tốc, va chạm và vượt qua 100% (19/19 tests).

### Entry #39: [Feature & Asset Tuning] - Bổ Sung Sprite Lâu Đài Underground (`CASTLE_UNDERGROUND`) Cho Level 2
- **Trạng thái:** Đã hoàn thành 100%, 17/17 CTest passed.
- **File ảnh hưởng:** `include/level/TileFrames.h`, `src/level/TileMap.cpp`
- **Mô tả:**
  1. Thêm định nghĩa `TileFrames::CASTLE_UNDERGROUND` với sub-rect `(419, 196, 80, 80)` trong `TileFrames.h`.
  2. Cập nhật logic vẽ ký tự `'L'` trong `TileMap.cpp`: kiểm tra nếu `m_theme == LevelTheme::UNDERGROUND` (như Level 2) thì sử dụng sprite lâu đài màu xanh xám `CASTLE_UNDERGROUND`, các level khác vẫn giữ nguyên lâu đài `CASTLE` màu cam đỏ truyền thống.
  3. Biên dịch thành công và vượt qua 100% bộ kiểm thử tự động 17/17 CTest.

### Entry #38: [Branch Merge] - Merge Branch `feature/sound-input` Vào `develop`
- **Trạng thái:** Đã hoàn thành 100%.
- **Commit hash:** `b9622fb`
- **Mô tả:**
  1. Cập nhật nhánh địa phương `feature/sound-input` bằng cách kéo (pull) 19 commits mới nhất từ `origin/feature/sound-input`.
  2. Thực hiện fast-forward merge thành công branch `feature/sound-input` vào branch `develop`.
  3. Cập nhật toàn bộ các thay đổi về camera, tilemaps, level design, physics và audio integration từ `feature/sound-input` vào `develop`.

### Entry #37: [Branch Merge & Test Sync] - Merge Branch `feature/sound-input` Vào `develop`
- **Trạng thái:** Đã hoàn thành 100%, 14/14 CTest passed.
- **File ảnh hưởng:** `tests/MarioPhysicsTests.cpp`, và các file từ `feature/sound-input` branch merge.
- **Mô tả:**
  1. Fast-forward merge thành công branch `feature/sound-input` vào branch `develop`.
  2. Cập nhật `tests/MarioPhysicsTests.cpp` để khởi tạo thứ tự state `SUPER` trước `initPhysics` với kích thước `SUPER_MARIO_SIZE` (28x60) trong test `testSuperMarioTraversesTwoBlockPassage`, đồng thời đồng bộ physics foot Y calculation trong test `testGrowthFootAnchorAndClearance`.
  3. Biên dịch 100% không cảnh báo/lỗi và vượt qua toàn bộ 14/14 unit test ctest suite.

### Entry #36: [Asset & Transparency Fix] - Tách Nền Trong Suốt Cho Vùng Mushroom & Sheet Backdrop (Bảo Tồn Tile Underwater)
- **Trạng thái:** Đã hoàn thành 100%, 13/13 CTest passed.
- **File ảnh hưởng:** `assets/textures/enemies/enemies.png`, `assets/textures/items/items_objects.png`, `assets/textures/mario/MarioLuigi.png`, `assets/textures/tiles/tileset.png`
- **Mô tả:**
  1. Loại bỏ toàn bộ nền xanh đậm `RGB(12, 69, 176)` và lề `RGB(0, 0, 168)`, `RGB(0, 41, 140)` trong khu vực Mushroom Platforms (`X: 0..323, Y: 356..440`) của `tileset.png`, đưa 14,093 pixel nền về `(0, 0, 0, 0)`.
  2. **Bảo tồn 100% các tile nội dung**: Giữ nguyên toàn bộ đồ họa nấm, thân nấm, đồi núi cũng như khu vực Underwater (san hô, đồi nước, ống nước dưới nước).
  3. Biên dịch và kiểm thử tự động thành công 100% (13/13 CTest passed).

### Superseding audit — Tileset coordinates and assembled scenery (2026-08-10)

Entry #35 below is historical and is superseded by the current audit in
[`docs/tileset_coordinate.md`](tileset_coordinate.md). The current contract
separates the Palette 1 terrain quartet from Palette 3 question/used blocks,
adds the standalone map-coin frames, selects Castle palette pipe/pole frames,
and records assembled bboxes for the full pole, tall pipes, and small/large
castles. `TileMap` chroma-keys only the two known sheet backdrop colors before
uploading the tileset; it does not remove gameplay colors such as castle holes.

### Entry #35: [Doc & Asset Sync] - Rà Soát Tọa Độ Tileset Bàn Đồ (`tileset_coordinate.md`) & Khắc Phục Nhãn Sai
- **Trạng thái:** Đã hoàn thành 100%.
- **File ảnh hưởng:** `docs/tileset_coordinate.md`, `include/level/TileFrames.h`, `docs/assets/tileset/object_001.png` -> `object_345.png`
- **Mô tả:**
  1. Tiến hành rà soát trực quan toàn bộ 340 object của `tileset.png` ($680 \times 776 \text{ px}$) đối soát với `TileFrames.h` trong C++ engine.
  2. Phát hiện và sửa lỗi nhãn sai trong `tileset_coordinate.md` đối với nhóm STT #1–#4 (vốn bị ghi nhầm là 4 khối Ground cho 4 môi trường, thực tế là 4 khối Overworld: Ground, Brick, Stone, Hard) và nhóm STT #9–#12 (khối Underground: Ground Teal, Brick Teal, Stone, Used Block).
  3. Khẳng định code C++ (`TileFrames.h`) đã chọn chuẩn xác $100\%$ tọa độ của toàn bộ các tile (Ground, Brick, Question Block, Castle Window/Door, Pipes, Finish Pole).
  4. Đã dọn dẹp các script Python phụ trợ.

### Entry #34: [Asset & Doc Sync] - Khôi Phục Đủ 157 Khung Ảnh Quái Vật & Đồng Bộ Tọa Độ Chuẩn
- **Trạng thái:** Đã hoàn thành 100%.
- **File ảnh hưởng:** `docs/enemies_coordinate.md`, `include/core/SpriteFrames_shared.h`, `docs/assets/enemies/enemy_001.png` -> `enemy_157.png`
- **Mô tả:**
  1. Xác minh lại tệp ảnh atlas gốc `enemies.png`: Dòng Y=24 thực tế chứa đầy đủ **4 khung Goomba bị giẫm bẹp** (`STOMPED`) ứng với 4 môi trường NES (Overworld, Underground, Castle, Underwater) với giá trị kênh Alpha opaque 255.
  2. Khôi phục lại danh sách chuẩn xác **157 thành phần quái vật** (`enemy_001.png` đến `enemy_157.png`) trong `docs/enemies_coordinate.md`.
  3. Đồng bộ chính xác tọa độ Koopa Xanh dưới nước (`UW_WALK1`/`UW_WALK2` và `UW_PARATROOPA_FLY1`/`UW_PARATROOPA_FLY2`) và vỏ Buzzy Beetle trong `SpriteFrames_shared.h`.
  4. Đã tự động dọn dẹp toàn bộ các script Python phụ trợ.
### Entry #1: [Bugfix] - Khắc Phục Lỗi Đồng Hồ Bất Tử 10s Bị Đóng Băng & Quy Tắc Trùng Lặp Hiệu Ứng NES
- **Trạng thái:** Đã hoàn thành, build & test pass 100%.
- **File ảnh hưởng:** [Mario.cpp](../src/entities/Mario.cpp)
- **Vấn đề cũ (Before):** Hàm `updateInvincibility(dt)` không được gọi trong `Mario::update()`, dẫn đến việc biến `m_starInvincibilityTimer` không bao giờ giảm và Mario bị bất tử vĩnh viễn.
- **Phương pháp áp dụng (Methodology):** Thêm `updateInvincibility(dt)` vào vòng lặp `Mario::update()`, đồng thời áp dụng quy tắc nạp hiệu ứng chuẩn NES gốc (reset 10s khi ăn tiếp Ngôi Sao, giữ nguyên dạng Mario To / Fire Mario).
- **Kết quả hiện tại (Impact):** Bất tử Ngôi Sao chạy chuẩn 10 giây, đổi màu Cầu Vồng sắc nét và hết giờ đúng thời hạn.

### Entry #2: [Refactoring & Particle Rendering Fix] - Nâng Cấp Multi-Theme Cho `QuestionBlock` & Khắc Phục Lỗi Hiển Thị Mảnh Vỡ `BlockDebris`
- **Trạng thái:** Đã hoàn thành, build pass 100%.
- **File ảnh hưởng:** [SpriteFrames.h](../include/core/SpriteFrames.h), [QuestionBlock.h](../include/entities/QuestionBlock.h), [QuestionBlock.cpp](../src/entities/QuestionBlock.cpp), [BlockDebris.h](../include/entities/BlockDebris.h), [BlockDebris.cpp](../src/entities/BlockDebris.cpp), [TileMap.cpp](../src/level/TileMap.cpp), [blocks_coordinate.md](blocks_coordinate.md)
- **Mô tả:** 
  1. **Khắc phục lỗi hiển thị `BlockDebris`**: Do `TileMap::hitTile()` khi sinh 4 mảnh vỡ không gọi `setTextureManager(*textureManager)`, dẫn tới `m_sprite` bị rỗng (`nullopt`) và hạt vỡ gạch không hiển thị trên màn hình. Đã bổ sung gắn `TextureManager`, gọi `updateAnimation(dt)` và thiết lập tâm xoay origin + scale 2.0x cho hạt mảnh vỡ.
  2. **Đồng bộ `SpriteFrames::Blocks`**: Đồng bộ 120 ô khối trong `items_blocks.png` từ `blocks_coordinate.md` vào `SpriteFrames::Blocks` hỗ trợ cả 4 môi trường NES.
  3. **Nâng cấp `QuestionBlock`**: Thêm enum `BlockTheme` tự động đổi bộ 3-frame animation chớp nháy và sprite ô phẳng `empty` phù hợp môi trường.

### Entry #3: [Feature & Testing] - Mặc Định Nhân Vật Luigi (Luigi Character Skin Support)
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Mario.h](../include/entities/Mario.h), [Mario.cpp](../src/entities/Mario.cpp)
- **Mô tả:** Đã thiết lập nhân vật mặc định là Luigi (`CharacterType::LUIGI`) theo yêu cầu chạy thử nghiệm. Tự động nạp bộ frame set của Luigi (Small Luigi, Big Luigi) từ spritesheet `MarioLuigi.png`. Trạng thái Fire (`MarioState::FIRE`) được dùng chung bộ frame `SpriteFrames::FireBigMario` và bảng màu Lửa NES (tint da cam) chuẩn xác cho cả Mario và Luigi, giữ file `SpriteFrames.h` vô cùng gọn sạch.


### Entry #4: [Documentation & Asset Analysis] - Phân Tích & Bóc Tách 282 Objects Trong `items_objects.png`
- **Trạng thái:** Đã hoàn thành 100%.
- **File ảnh hưởng:** [items_objects_coordinate.md](items_objects_coordinate.md), [items_objects_all_components_atlas_full.png](assets/reference/items_objects_all_components_atlas_full.png)
- **Mô tả:** 
  1. **Bóc tách 282 Objects trong `items_objects.png`**: Phân tích toàn bộ 282 vật thể/vật phẩm nằm trong vùng khung tím thuộc 4 môi trường bảng màu NES (Overworld, Underground, Castle, Underwater).
  2. **Tạo tài liệu `items_objects_coordinate.md`**: Cập nhật đầy đủ tọa độ Loang, kích thước Loang, tọa độ Grid Khung, kích thước Khung, số Pixel, Palette môi trường và mô tả ý nghĩa chi tiết (Super/1-Up Mushroom, Fire Flower, Super Star, Spinning Coin, Fireball, Firework Explosion, Spring/Trampoline, Moving Platform, Flagpole Flag, Castle/Star Flag, Beanstalk Vine).
  3. **Tạo ảnh Atlas minh họa `items_objects_atlas.png`**: Gen ảnh Atlas dạng lưới 8 cột (chuẩn phong cách `blocks_all_components_atlas.png`), ghi nhãn STT kèm tọa độ Khung và vẽ bounding box màu xanh lơ ôm sát từng vật thể.

### Entry #5: [Bugfix & Code Sync] - Bổ Sung Inline Vector Functions Cho `SpriteFrames::Items` & Sửa Lỗi Biên Dịch Build
- **Trạng thái:** Đã hoàn thành, build pass 100%.
- **File ảnh hưởng:** [SpriteFrames.h](../include/core/SpriteFrames.h), [TV5_CHANGES_SUMMARY.md](management/TV5_CHANGES_SUMMARY.md)
- **Mô tả:** 
  1. **Khắc phục lỗi biên dịch `Coin.cpp`, `FireFlower.cpp`, `Star.cpp`**: Do `SpriteFrames::Items` trước đó khai báo biến dạng vector tĩnh (`spinningCoinFrames`, `superStarFrames`, `fireFlowerFrames`), trong khi code `Coin.cpp`, `FireFlower.cpp`, `Star.cpp` gọi dưới dạng hàm (`coinFrames()`, `starFrames()`, `fireFlowerFrames()`).
  2. **Bổ sung các hàm helper `inline const std::vector<sf::IntRect>&`**: Thêm các hàm `coinFrames()`, `spinningCoinFrames()`, `starFrames()`, `superStarFrames()`, `fireFlowerFrames()`, `fireballFrames()`, `fireballExplosionFrames()` trong namespace `SpriteFrames::Items` tuân thủ đúng chuẩn của các namespace khác trong `SpriteFrames.h`.
  3. **Xác nhận tọa độ**: Xác nhận 100% tọa độ `IntRect` của toàn bộ item trong `SpriteFrames::Items` trùng khớp hoàn toàn với bảng tọa độ chuẩn `docs/items_objects_coordinate.md`.
### Entry #6: [Bugfix & Rendering Sync] - Sửa Lỗi Hiển Thị Sprite Khi Chết & Loại Bỏ Delay Load Nhân Vật Đặc Biệt
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Mario.cpp](../src/entities/Mario.cpp), [PlayState.cpp](../src/states/PlayState.cpp)
- **Mô tả:** 
  1. **Sửa lỗi hiển thị sprite khi chết (`Mario::loseLife()`)**: Khi ở dạng Fire Mario/Big Mario bị chết, hàm `loseLife()` lập tức chuyển trạng thái về `SMALL`, gọi `setupAnimationsForState()` để nạp đúng sprite animation chết của Luigi/Mario mặc định (`DEATH`), đồng thời reset màu sprite về `sf::Color::White` ngắt màu tint da cam.
  2. **Triệt tiêu độ trễ 1-frame khi load nhân vật đặc biệt (`PlayState::restoreProgress()`)**: Thêm `m_level->update(0.f)` ngay trong `restoreProgress()` sau khi phục hồi trạng thái `FIRE` / `SUPER`, giúp vị trí, tỉ lệ phóng 2x và camera đồng bộ ngay lập tức trước khi Render frame 0.

### Entry #7: [Feature & Keybinding] - Kích Hoạt Chiêu Bắn Đạn Lửa (Fireball Shooting) Cho Fire Mario
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Level.h](../include/level/Level.h), [Level.cpp](../src/level/Level.cpp), [PlayState.cpp](../src/states/PlayState.cpp), [FireBall.cpp](../src/entities/FireBall.cpp)
- **Mô tả:** 
  1. **Tích hợp `Level::shootFireBall()`**: Gọi `Mario::shootFireBall(b2World*)` sinh ra `FireBall` projectile, gắn `TextureManager`, thêm vào `m_entities` và phát âm thanh `fireball.wav` qua `SoundManager`.
  2. **Gán phím bắn đạn lửa (`ShootCommand`)**: Phím **`Shift`** (Cả Left Shift & Right Shift), **`J`**, **`F`**, **`X`**, **`Left Control`** được gán lệnh bắn đạn lửa qua `InputHandler` trong `PlayState::rebindCommands()`.
  3. **Hoàn thiện Animation & Rendering `FireBall`**: Thêm animation `spin` (4-frame xoay tròn đạn lửa 8×8), tự động nạp scale 2.0x và va chạm nảy tâng tâng khi chạm đất, tiêu diệt quái Goomba/Koopa khi trúng đích.

### Entry #8: [Bugfix & Animation] - Tiêu Diệt Quái Khi Trúng Đạn Lửa & Đồng Bộ Dáng Ném Đạn Lửa Cho Cả 2 Nhân Vật
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [CollisionManager.cpp](../src/physics/CollisionManager.cpp), [Mario.cpp](../src/entities/Mario.cpp)
- **Mô tả:** 
  1. **Xử lý tiêu diệt quái tức thì (`CollisionManager::resolve()`)**: Thêm `enemy->markForRemoval()` và thông báo `EventType::ENEMY_STOMPED` khi `FireBall` va chạm với Quái (`isEnemy()`), giúp quái biến mất và cộng điểm ngay lập tức khi dính đạn lửa.
  2. **Dáng ném đạn lửa đồng bộ (`setupAnimationsForState()`)**: Bổ sung clip animation `action` (dáng giơ tay ném đạn lửa `ACTION` frame từ `SpriteFrames::FireBigMario`) cho cả Mario và Luigi khi ở trạng thái `FIRE`. Tự động kích hoạt khi nhấn phím bắn.

### Entry #9: [Architecture & Fix] - Xử Lý Hàng Đợi Bắn Đạn Lửa Tránh Trùng Lặp Khóa Vật Lý Box2D (World Locked Fix)
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Level.h](../include/level/Level.h), [Level.cpp](../src/level/Level.cpp)
- **Mô tả:** 
  1. **Giải quyết triệt để thông báo `PhysicsEngine world null or locked!`**: Khi bấm phím bắn đạn lửa trùng đúng thời điểm Box2D đang tính toán va chạm (`m_world->IsLocked()`), `Level::shootFireBall()` sẽ lưu yêu cầu vào hàng đợi `m_pendingFireBallRequests`.
  2. **Tạo vật thể đạn lửa an toàn (`processPendingFireballs()`)**: Ngay khi Box2D kết thúc bước tính toán (`PhysicsEngine::update()`), hàm `processPendingFireballs()` sẽ khởi tạo thân vật lý đạn lửa khi thế giới ở trạng thái an toàn (!IsLocked). Đảm bảo 100% các phím nhấn bắn đạn đều thành công và không bao giờ bị nốt bỏ.

### Entry #10: [Feature & Visual Fix] - Hiệu Ứng Lật Bụng Khi Bị Bắn Đạn Lửa & Hỗ Trợ Dạng Lửa Nhỏ (Fire Small Mario)
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [Enemy.h](../include/entities/Enemy.h), [Enemy.cpp](../src/entities/Enemy.cpp), [Goomba.h](../include/entities/Goomba.h), [Goomba.cpp](../src/entities/Goomba.cpp), [Koopa.h](../include/entities/Koopa.h), [Koopa.cpp](../src/entities/Koopa.cpp), [FireFlower.cpp](../src/items/FireFlower.cpp), [Mario.h](../include/entities/Mario.h), [Mario.cpp](../src/entities/Mario.cpp), [HUD.cpp](../src/ui/HUD.cpp)
- **Mô tả:** 
  1. **Hiệu ứng quái lật bụng (`onFireHit()`)**: Khi Goomba hoặc Koopa dính đạn lửa, quái sẽ **lật ngược bụng lên trên** (`setScale(2.f, -2.f)`), nảy nhẹ lên trên rồi nảy văng rơi tự do xuống dưới màn hình kèm âm thanh `kickkill.wav` đúng chuẩn NES gốc.
  2. **Contract hiện tại:** FireFlower luôn đưa Mario về state `FIRE`; không có state thử nghiệm riêng trong release.

### Entry #11: [LEGACY] FireBall capability theo Mario state
- **Trạng thái:** LEGACY — đã được thay bằng release contract `SMALL/SUPER/FIRE`.
- **File ảnh hưởng:** [Mario.cpp](../src/entities/Mario.cpp)
- **Mô tả:** Release chỉ cho phép `canShootFireBall()` khi `m_marioState == MarioState::FIRE`.

### Entry #12: [Clean Log & Physics] - Triệt Tiêu Log Cảnh Báo Giả `PhysicsEngine world null or locked!`
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [FireBall.cpp](../src/entities/FireBall.cpp), [Entity.cpp](../src/entities/Entity.cpp)
- **Mô tả:** 
  1. **[LEGACY] FireBall constructor claim**: Entry lịch sử này từng mô tả một thay đổi ở constructor mặc định; nội dung đã được supersede và không phải bằng chứng cho release contract TV5. FireBall/physics contract thuộc owner TV3.
  2. **[LEGACY] Physics logging claim**: Mô tả logging cũ không được dùng để kết luận DoD hiện tại; FireBall/physics contract thuộc owner TV3.

### Entry #13: [LEGACY] Giới hạn FireBall thử nghiệm
- **Trạng thái:** LEGACY — con số trong entry cũ không phải release contract.
- **File ảnh hưởng:** [Level.cpp](../src/level/Level.cpp)
- **Mô tả:** Release contract FireBall được khóa riêng trong tài liệu Sprint 6; entry cũ không được dùng làm bằng chứng DoD.

### Entry #14: [Fix Spawn Clipping & Grace Period] - Nâng Độ Cao Spawn Đạn Lửa & Tạo Khoảng Đệm 50ms Tránh Nổ Tức Thì
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (6/6 ctest passed).
- **File ảnh hưởng:** [FireBall.h](../include/entities/FireBall.h), [CollisionManager.cpp](../src/physics/CollisionManager.cpp), [Mario.cpp](../src/entities/Mario.cpp), [Level.cpp](../src/level/Level.cpp)
- **Mô tả:** 
  1. **Tăng độ cao vị trí sinh đạn lửa (`spawnY = m_position.y + 4.f`)**: Phóng đạn từ vị trí bàn tay Mario (ngay trên ngực), giúp viên đạn sinh ra lơ lửng cách mặt đất 10px thay vì bị dính vào mép gạch dưới chân.
  2. **Tạo khoảng đệm an toàn 50ms (`getLifetime() > 0.05f`)**: Bỏ qua va chạm với thân Mario (`target->isMario()`) và yêu cầu đạn tồn tại ít nhất 50ms trước khi chấp nhận va chạm hủy đạn vào tường đứng. Giúp đạn lửa bay ra mượt mà 100% ở bất kỳ ngóc ngách hay khu vực gạch nào.



### Entry #15: [LEGACY] Merge TV5 branch và assertion cũ
- **Trạng thái:** LEGACY — assertion cũ đã được thay bằng một state `FIRE` duy nhất.
- **File ảnh hưởng:** [develop branch], [TV5IntegrationTests.cpp](../tests/TV5IntegrationTests.cpp), [TV5_CHANGES_SUMMARY.md](management/TV5_CHANGES_SUMMARY.md)
- **Mô tả:** 
  1. **Merge nhánh TV5**: Chuyển sang nhánh `develop` và merge phiên bản mới nhất từ `origin/feature/sound-input` của TV5 (bao gồm SoundManager, HUD, Audio/SFX Pool, ScoreRules và bộ test tích hợp TV5).
  2. **Cập nhật assertion test tích hợp:** test hiện yêu cầu đúng `MarioState::FIRE` sau FireFlower.

### Entry #16: [Merge & Save System] - Merge Branch TV4 (`feature/level-and-enemy`) Về `develop`, Bảo Tồn SpriteFrames & Giải Quyết 9 Conflicts
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [CMakeLists.txt](../CMakeLists.txt), [SaveManager.h](../include/core/SaveManager.h), [SaveManager.cpp](../src/core/SaveManager.cpp), [SaveManagerTests.cpp](../tests/SaveManagerTests.cpp), [TileFrames.h](../include/level/TileFrames.h), [Enemy.h](../include/entities/Enemy.h), [Enemy.cpp](../src/entities/Enemy.cpp), [Goomba.cpp](../src/entities/Goomba.cpp), [Koopa.cpp](../src/entities/Koopa.cpp), [TileMap.cpp](../src/level/TileMap.cpp), [CollisionManager.cpp](../src/physics/CollisionManager.cpp), [level1.txt](../levels/level1.txt), [level3.txt](../levels/level3.txt), [TV5_CHANGES_SUMMARY.md](management/TV5_CHANGES_SUMMARY.md)
- **Mô tả:** 
  1. **Bảo tồn 100% `SpriteFrames.h`**: Giữ nguyên toàn bộ định nghĩa sprite animation mới nhất trên `develop` (SuperStar, FireBall, Score Text, Block Debris), không làm mất bất kỳ dòng code nào.
  2. **Tích hợp hệ thống lưu game (`SaveManager`)**: Nạp module `SaveManager` ghi file an toàn nguyên tử (atomic write `.tmp`), lưu high score tăng đơn điệu, lưu cài đặt âm thanh volume và bổ sung bộ test `SaveManagerTests` (gồm 6 bài test regression).
  3. **Tối ưu hóa tầm kích hoạt quái (`Enemy Activation Zone`)**: Quái vật chỉ kích hoạt khi camera tiến tới gần (256px) và dọn dẹp khi trôi xa phía sau (512px).
  4. **Fix Koopa Stomp State & Double-Hit**: Sửa logic va chạm giẫm Koopa lần 1 chỉ chuyển về `SHELL_IDLE`, không bị vừa biến thành vỏ vừa bị đá bay trong cùng 1 frame.
  5. **[LEGACY] Level 3 layout claim**: Đây là claim lịch sử của merge log, không phải bằng chứng Level 3 hiện tại hợp lệ; dùng `level_validator_tests` và log của owner TV4 để kết luận.
  6. **Giải quyết xung đột 9 file**: Xử lý mượt mà toàn bộ 9 file conflict và thêm block Header Comment cho `TileFrames.h` cùng sửa include order cho `Goomba.cpp`.
  7. **Biên dịch & CTest 9/9 Pass**: Dự án biên dịch sạch sẽ 100%, vượt qua toàn bộ 9/9 bài test CTest tự động.

### Entry #17: [Merge Engine Core Systems & Clean Code] - Merge Branch TV2 (`origin/feature/engine-core-systems`) Về `develop` & Sửa Warning `Koopa.cpp`
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [Koopa.cpp](../src/entities/Koopa.cpp), [UILayoutHelper.h](../include/ui/UILayoutHelper.h), [UILayoutHelper.cpp](../src/ui/UILayoutHelper.cpp), [UIMenuWidget.h](../include/ui/UIMenuWidget.h), [UIMenuWidget.cpp](../src/ui/UIMenuWidget.cpp), [Game.cpp](../src/core/Game.cpp), [PhysicsEngine.h](../include/physics/PhysicsEngine.h), [PhysicsEngine.cpp](../src/physics/PhysicsEngine.cpp), [TileMap.h](../include/level/TileMap.h), [TileMap.cpp](../src/level/TileMap.cpp), [Level.cpp](../src/level/Level.cpp), [Mario.cpp](../src/entities/Mario.cpp), [PauseState.h](../include/states/PauseState.h), [PauseState.cpp](../src/states/PauseState.cpp), [GameOverState.h](../include/states/GameOverState.h), [GameOverState.cpp](../src/states/GameOverState.cpp), [WinState.h](../include/states/WinState.h), [WinState.cpp](../src/states/WinState.cpp)
- **Mô tả:** 
  1. **Sửa warning IDE `Koopa.cpp`**: Loại bỏ `#include "core/SpriteFrames.h"` không sử dụng trực tiếp trong `src/entities/Koopa.cpp`.
  2. **Tích hợp bộ helper giao diện UI (`UILayoutHelper` & `UIMenuWidget`)**: Thêm module định vị UI theo điểm neo (`UIAnchor`) và widget menu tương tác hỗ trợ phím mũi tên (`Up`/`Down`/`W`/`S`) + `Enter`.
  3. **Tối ưu hóa game loop (`Game.cpp`)**: Bổ sung `MAX_DELTA_TIME` clamping (0.1s), xử lý an toàn khi đóng cửa sổ (`m_window.close(); return;`), và bắt buộc khóa tỉ lệ màn hình 16:9 khi resize window.
  4. **Tách lớp Render Foreground (`TileMap` & `Level`)**: Bổ sung `renderForeground()` vẽ khối gạch, ống nước, cờ đè lên entity và Mario theo đúng chuẩn NES.
  5. **Giới hạn Substep Physics (`PhysicsEngine`)**: Thêm giới hạn `MAX_SUBSTEPS = 8` để ngăn ngừa lặp vô hạn catch-up lag khi giật khung hình.
  6. **Giải quyết xung đột 5 file**: Xử lý mượt mà conflict ở `PauseState.h/.cpp`, `GameOverState.cpp`, `WinState.cpp`, `TileMap.cpp`. Kết hợp menu `UIMenuWidget` với chức năng chỉnh âm lượng Music/SFX bằng phím Trái/Phải.
  7. **Biên dịch & CTest 9/9 Pass**: Biên dịch Debug/Release thành công 100%, tất cả 9/9 bài test CTest đều vượt qua.

### Entry #18: [Clean Code & Warnings Fix] - Loại Bỏ `#include "core/DisplayConfig.h"` Thừa Trong Các State UI
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [GameOverState.cpp](../src/states/GameOverState.cpp), [PauseState.cpp](../src/states/PauseState.cpp), [WinState.cpp](../src/states/WinState.cpp)
- **Mô tả:** Loại bỏ các câu lệnh `#include "core/DisplayConfig.h"` thừa không được sử dụng trực tiếp trong `GameOverState.cpp`, `PauseState.cpp`, và `WinState.cpp`, dọn sạch cảnh báo IDE và tối ưu hóa biên dịch.

### Entry #19: [LEGACY] Clean code cho HUD state label
- **Trạng thái:** LEGACY — HUD hiện chỉ switch qua các state release.
- **File ảnh hưởng:** [HUD.cpp](../src/ui/HUD.cpp)
- **Mô tả:** HUD hiện hiển thị `SMALL`, `SUPER`, `FIRE` và overlay `STAR`; không có nhánh state ngoài contract.


### Entry #20: [Gate 0 Contract Compliance] - Purge legacy extra state, Fix PLAYER_LOST_LIFE & Integrate SaveManager
- **Trạng thái:** Đã hoàn thành, build & test pass 100%.
- **File ảnh hưởng:** `Mario.h`, `Mario.cpp`, `HUD.cpp`, `PlayState.cpp`, `FireFlower.cpp`, `Mushroom.cpp`, `GameManager.h`, `GameManager.cpp`, `MenuState.cpp`, `GameOverState.cpp`, `WinState.cpp`, `CMakeLists.txt`
- **Mô tả:** 
  1. **Purge legacy extra state**: Khi nhặt FireFlower ở trạng thái `SMALL`, Mario chuyển trực tiếp sang `FIRE`.
  2. **Remove PLAYER_LOST_LIFE**: Xóa `PLAYER_LOST_LIFE` event để tránh race condition, thay vào đó `HUD` dùng callback timeout trỏ thẳng tới `Mario::loseLife()`, và `PlayState` xử lý mạng trong loop.
  3. **SaveManager Integration**: Đưa `SaveManager` vào trong `GameManager` singleton. Mọi truy cập vào save data đều thông qua `GameManager::getInstance().getSaveManager()`. `MenuState` hiển thị High Score, `WinState`/`GameOverState` cập nhật High Score.
  4. **Rename target**: Đổi tên target từ `main` sang `SuperMario` trong `CMakeLists.txt`.

### Entry #21: [Fix HUD & Character State Desync] - Đồng Bộ Trạng Thái Nhân Vật Với HUD Góc Trên Bên Trái
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [HUD.cpp](../src/ui/HUD.cpp), [PlayState.cpp](../src/states/PlayState.cpp), [Mario.h](../include/entities/Mario.h), [TV5IntegrationTests.cpp](../tests/TV5IntegrationTests.cpp), [TV5_CHANGES_SUMMARY.md](management/TV5_CHANGES_SUMMARY.md)
- **Mô tả:** 
  1. **Khắc phục lỗi tồn đọng cờ Star Power (`m_starPowerActive`)**: Đăng ký `HUD` lắng nghe sự kiện `PLAYER_POWER_DOWN`. Tự động reset `m_starPowerActive = false` khi nhận sự kiện `PLAYER_DIED`, `PLAYER_POWER_DOWN`, hoặc `LEVEL_STARTED`. Giúp loại bỏ hoàn toàn lỗi góc trái hiển thị `POWER STAR` sau khi nhân vật qua đời / mất mạng / giảm trạng thái.
  2. **Đồng bộ HUD khi restore progress (`PlayState::restoreProgress`)**: Thêm lời gọi `m_hud->update()` ngay sau khi gán lại điểm, xu, mạng và `MarioState` trong `restoreProgress()`, đảm bảo HUD hiển thị đúng ngay tại frame 0.
  3. **Đồng bộ default initializer `m_characterType`**: Đưa giá trị mặc định của `m_characterType` trong `Mario.h` về `CharacterType::MARIO` khớp chuẩn xác với các constructor trong `Mario.cpp`.
  4. **Bổ sung regression unit tests (`TV5IntegrationTests.cpp`)**: Thêm assertion kiểm thử tự động xác nhận `PLAYER_DIED` và `PLAYER_POWER_DOWN` reset nhãn `POWER` về đúng `SMALL`. Toàn bộ 9/9 bộ test CTest đều vượt qua.

### Entry #22: [Fix Dead Enemy Collision] - Bỏ Qua Va Chạm & Gây Sát Thương Khi Quái Đã Chết/Đang Rơi
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [Enemy.h](../include/entities/Enemy.h), [Goomba.h](../include/entities/Goomba.h), [Koopa.h](../include/entities/Koopa.h), [CollisionManager.cpp](../src/physics/CollisionManager.cpp), [TV5_CHANGES_SUMMARY.md](management/TV5_CHANGES_SUMMARY.md)
- **Mô tả:** 
  1. **Thêm giao diện `Enemy::isDying()`**: Khai báo `virtual bool isDying() const` ở `Enemy.h` và override ở `Goomba.h` (`m_isStomped || m_isFlippedDead || isDead() || !isActive()`) cũng như `Koopa.h` (`m_isFlippedDead || isDead() || !isActive()`).
  2. **Tắt va chạm vật lý Box2D (`CollisionManager::preSolve`)**: Gọi `contact->SetEnabled(false)` khi một trong hai entity va chạm là quái đang dying/dead, giúp Mario và các vật thể khác đi qua quái đang lơ lửng/rơi tự do mà không bị đẩy hay nảy.
  3. **Bỏ qua gây sát thương cho Mario (`CollisionManager::handleMarioCollision`)**: Thêm kiểm tra `if (enemy->isDying()) return;` ngay đầu luồng xử lý va chạm Mario-quái. Mario không còn bị mất mạng hay bị trừ máu khi chạm trúng quái đã bị đạp bẹp hoặc quái đang ngửa bụng lơ lửng rơi xuống vực.

### Entry #23: [Fix Goomba Squished Sprite Position] - Sửa Lỗi Lệch Vị Trí Frame Bẹp Của Goomba
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [Goomba.cpp](../src/entities/Goomba.cpp), [TV5_CHANGES_SUMMARY.md](management/TV5_CHANGES_SUMMARY.md)
- **Mô tả:** 
  1. **Sửa tính toán chân Sprite Goomba (`syncSpriteToFeet`)**: Đổi tính toán vị trí đáy sprite từ `m_position.y + GOOMBA_SIZE.y` (32px cố định) thành `m_position.y + m_size.y` (16px khi bị đạp).
  2. **Kết quả**: Loại bỏ hoàn toàn hiện tượng sprite Goomba bị dậm bẹp bị chìm 16px sâu bên dưới mặt đất/gạch. Sprite squished phẳng lì nằm chuẩn xác ngay trên mặt sàn.

### Entry #24: [Fix Enemy Sprite Orientation] - Lật Sprite Hướng Mặt Quái Theo Hướng Di Chuyển
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [Koopa.cpp](../src/entities/Koopa.cpp), [Goomba.cpp](../src/entities/Goomba.cpp), [TV5_CHANGES_SUMMARY.md](management/TV5_CHANGES_SUMMARY.md)
- **Mô tả:** 
  1. **Lật tỉ lệ scale & origin theo hướng di chuyển (`syncSpriteToFeet`)**: Khi `getFacingDirection() == Direction::RIGHT`, thiết lập `m_sprite->setScale({-SPRITE_SCALE, SPRITE_SCALE})` và `setOrigin({rect.size.x, 0.f})`. Khi hướng `LEFT`, giữ `setScale({SPRITE_SCALE, SPRITE_SCALE})` và `setOrigin({0.f, 0.f})`.
  2. **Kết quả**: Loại bỏ hoàn toàn lỗi Koopa / Goomba bị đi giật lùi (moonwalk) khi di chuyển sang phải. Quái luôn quay mặt về đúng hướng di chuyển 100%.

### Entry #25: [Fix Star Invincibility Expiration Event] - Phát Sự Kiện PLAYER_INVINCIBILITY_EXPIRED Khi Hết Hạn Ngôi Sao
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [Mario.cpp](../src/entities/Mario.cpp), [HUD.cpp](../src/ui/HUD.cpp), [TV5IntegrationTests.cpp](../tests/TV5IntegrationTests.cpp), [TV5_CHANGES_SUMMARY.md](management/TV5_CHANGES_SUMMARY.md)
- **Mô tả:** 
  1. **Bổ sung phát sự kiện hết hạn tàng hình Sao (`Mario::updateInvincibility`)**: Thêm `EventBus::getInstance().notify(EventType::PLAYER_INVINCIBILITY_EXPIRED)` khi bộ đếm 10 giây Starman (`m_starInvincibilityTimer`) đếm về 0.
  2. **Ràng buộc kiểm tra hai lớp ở HUD (`HUD::getPowerLabel`)**: Đổi điều kiện nhãn `STAR` thành `m_starPowerActive && m_mario.isStarInvincible()`.
  3. **Kết quả**: Khi hiệu ứng Sao 10 giây kết thúc, HUD tự động chuyển nhãn `POWER STAR` về lại trạng thái chuẩn (`POWER SMALL` / `POWER SUPER` / `POWER FIRE`) ngay tức thì, loại bỏ dứt điểm hiện tượng treo nhãn `POWER STAR` vĩnh viễn.

### Entry #26: [Tune Mario Skid Duration] - Tăng Thời Gian Hiệu Ứng Phanh Lại Khi Chạy Nhanh
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (9/9 ctest passed).
- **File ảnh hưởng:** [Mario.cpp](../src/entities/Mario.cpp), [TV5_CHANGES_SUMMARY.md](management/TV5_CHANGES_SUMMARY.md)
- **Mô tả:** 
  1. **Điều chỉnh ma sát phanh (`SKID_FRICTION`)**: Đổi hằng số `SKID_FRICTION` từ `2200.f` về `1200.f` trong `Mario.cpp`.
  2. **Kết quả**: Khi Mario đang chạy nhanh (tốc độ tối đa 340 px/s) và bấm phím ngược hướng để phanh lại, thời gian trượt phanh và hiển thị animation `skid` kéo dài từ ~0.15s lên ~0.28s, mang lại cảm giác phanh trượt mượt mà và đúng chuẩn game Mario gốc.

### Entry #27: [Merge TV4 Branch & Finalize Level/Enemy System] - Integrated feature/level-and-enemy into develop
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (10/10 ctest passed).
- **File ảnh hưởng:** `CMakeLists.txt`, `docs/management/TV4_CHANGES_SUMMARY.md`, [TileMap.h](../include/level/TileMap.h), [CollisionManager.h](../include/physics/CollisionManager.h), [level0.txt](../levels/level0.txt), [level1.txt](../levels/level1.txt), [level2.txt](../levels/level2.txt), [level3.txt](../levels/level3.txt), [Goomba.cpp](../src/entities/Goomba.cpp), [Koopa.cpp](../src/entities/Koopa.cpp), [Level.cpp](../src/level/Level.cpp), [TileMap.cpp](../src/level/TileMap.cpp), [CollisionManager.cpp](../src/physics/CollisionManager.cpp), [ContactListener.cpp](../src/physics/ContactListener.cpp), [LevelValidatorTests.cpp](../tests/LevelValidatorTests.cpp)
- **Mô tả:** 
  1. **Level File Validation**: Bổ sung cơ chế validate nghiêm ngặt file level trong `TileMap::loadFromFile()` (đúng 1 Mario spawn `M`, 1 finish point `F`, cột cờ `|` liên tục kết thúc ở ô solid, và cùng chiều rộng hàng). Thêm bộ test `LevelValidatorTests.cpp` với 8 bài unit test tự động (100% pass).
  2. **Release Level Normalization**: Chuẩn hóa cấu trúc và layout 3 màn chơi chính Level 1 (183×12), Level 2 (250×12), Level 3 (281×12) và fixture Level 0. Bổ sung các cụm pipe traversal và phần thưởng Fire Flower ở giữa Level 2.
  3. **Cải tiến Tile & Physics Collision Pipeline**: Tinh chỉnh AI quái Goomba/Koopa với ledge detection và bỏ qua va chạm mép gạch (`isWalkableSupportSeam`). Dependency injection truyền `TileMap&` qua `ContactListener` và `CollisionManager`. Chuẩn hóa `QuestionBlock` làm Single Source of Truth cho tile hits. Mở rộng finish trigger bao phủ toàn bộ chiều cao cột cờ.
  4. **Merge & Giải Quyết Xung Đột**: Hợp nhất thành công nhánh `feature/level-and-enemy` vào `develop`. Giải quyết xung đột merge tại `Koopa.cpp` và `CollisionManager.cpp`, giữ nguyên cả logic bỏ qua va chạm quái dying/dead lẫn logic seam check. Xóa `#include "entities/FireBall.h"` bị lặp trong `Level.cpp`. Toàn bộ 10/10 ctest đều vượt qua.

### Entry #28: [Merge TV5 Sound & Visual Enhancements] - Integrated feature/sound-input into develop
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (10/10 ctest passed).
- **File ảnh hưởng:** [ASSETS_LIST.md](../assets/ASSETS_LIST.md), `assets/ui/bg_world.png`, `docs/management/s6_plan.md`, `docs/management/s7_plan.md`, [DisplayConfig.h](../include/core/DisplayConfig.h), [SpriteFrames.h](../include/core/SpriteFrames.h), [Camera.cpp](../src/level/Camera.cpp), [Level.cpp](../src/level/Level.cpp)
- **Mô tả:** 
  1. **Background Pixel-Art Mới (`bg_world.png`)**: Bổ sung hình nền pixel-art thế giới Overworld sinh động (`assets/ui/bg_world.png`, 1857×847 px) và khai báo hằng số `WORLD_PATH` trong `SpriteFrames::Backgrounds`.
  2. **Căn Chỉnh Background & Camera**: Điều chỉnh thuật toán `calculateBackgroundTop` dựa trên hàng mặt đất `findGroundSurfaceRow` trong `Level.cpp`, render các dải nền đan xen lật gương (`mirrored scale`) phủ kín chiều rộng level. Cập nhật `BOTTOM_TILE_PADDING` trong `Camera.cpp` về `0.0f` để mép dưới camera vừa khít mặt map.
  3. **Màu Nền Trời Tươi Sáng**: Cập nhật `BACKGROUND_COLOR` trong `DisplayConfig.h` từ CornflowerBlue `(100, 149, 237)` thành Sky Blue `(69, 197, 250)`.
  4. **Merge Tự Động**: Merge hoàn toàn tự động không xung đột (`ort strategy`) nhánh `feature/sound-input` của TV5 vào `develop`. Biên dịch thành công 100%, tất cả 10/10 bộ ctest đều passed.
### Entry #29: [Round 2 — Gate 0 Contract Tests + Docs Sync] - TV1 evidences the release contract and refreshes stale management docs
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (12/12 ctest passed).
- **Task liên quan:** S6-TV1-01/02/03/14/18/19/28/30/31/32/33/35 + Gate 0 test contract.
- **File ảnh hưởng:** `CMakeLists.txt`, `tests/Gate0ContractTests.cpp` (mới), `tests/PlayStateTests.cpp` (mở rộng), `tests/SaveSessionTests.cpp` (mới), `README.md`, `FILE_STRUCTURE.md`, `docs/class_diagram.md`, `docs/management/S6_AUDIT_TRACKER.md`, `docs/management/S6_BUG_REGISTER.md`
- **Mô tả:**
  1. **Gate 0 test contract (`gate0_contract_tests`)**: `static_assert` khóa `MarioState = {SMALL, SUPER, FIRE}` (FIRE_SMALL tái xuất ⇒ build fail); FireFlower luôn cho `FIRE` từ cả SMALL và SUPER; Mushroom không downgrade (SUPER/FIRE giữ nguyên); `CharacterType` mặc định `MARIO`; cả 3 release level load được qua validator + tileset, đúng 1 `M` và 1 `F`. Lưu ý: guard giới hạn FireBall = 2 chờ TV3-19 (dòng NOTE trong file).
  2. **PlayState death/Win regression (`play_state_tests`)**: một death phát đúng 1 `PLAYER_DIED` và trừ đúng 1 life; death thứ hai khi đang dying bị chặn; respawn re-arm death chain; Win-decision (Level 3 → hết level → Win) đi đúng một đường (`m_transitionIsWin`). Playthrough full-loop chờ TV4 (S6-TV4-40).
  3. **Save restart-session validation (`save_session_tests`)**: hai phiên SaveManager độc lập trên cùng file tạm — mọi field (high score, unlock, hai volume) còn đúng sau "restart"; monotonic score/unlock giữ sau restart; `GameManager::getSaveManager()` luôn trả về cùng một instance (composition root).
  4. **Docs sync**: README controls đúng `Shift = Run`, `X = Shoot` và đủ 12 test suite; FILE_STRUCTURE bỏ file phantom (`implementation_plan_sprint5_error.md`, `docs/PLAN_TV1.md`, `ui/Button.*`) và khớp cây file thật; class_diagram bổ sung `GameManager::getSaveManager()`/`m_saveManager`, `SaveManager` API thật, default character MARIO; tracker/bug register cập nhật bằng chứng thật (12/12, target `SuperMario`, SaveManager REVIEW).
  5. **S6-TV1-33**: Sprint 5 error plan không tồn tại trong repo — đã gỡ mọi link/claim (FILE_STRUCTURE) và ghi lý do vào tracker, không bịa file/banner.

### Entry #30: [Fix Death & Completion High Score Persistence + Save Logging] - Save high score immediately on death/completion (BUG-027 / S6-TV1-19)
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (12/12 ctest passed).
- **Task liên quan:** S6-TV1-19 / BUG-027.
- **File ảnh hưởng:** [PlayState.cpp](../src/states/PlayState.cpp), [SaveManager.cpp](../src/core/SaveManager.cpp), [GameManager.cpp](../src/core/GameManager.cpp), [SaveSessionTests.cpp](../tests/SaveSessionTests.cpp), [S6_AUDIT_TRACKER.md](management/S6_AUDIT_TRACKER.md), [S6_BUG_REGISTER.md](management/S6_BUG_REGISTER.md)
- **Mô tả:**
  1. **Fix 1 (Lưu high score khi chết)**: Trong `PlayState::onNotify(PLAYER_DIED)`, gọi `GameManager::getInstance().getSaveManager().updateHighScore(m_level->getMario()->getScore())` ngay khi sự kiện chết xảy ra. Mario chết dù còn mạng vẫn được lưu lại điểm cao vào đĩa đòn bẩy.
  2. **Fix 2 (Lưu high score khi xong level)**: Trong `PlayState::onNotify(LEVEL_COMPLETED)`, gọi `GameManager::getInstance().getSaveManager().updateHighScore(m_progress.score)` để đảm bảo điểm số màn vừa qua không bị rơi mất nếu thoát game trước khi vào GameOver/Win.
  3. **Fix 3 (Save error & CWD load logging)**: Gỡ bỏ `#ifdef DEBUG` quanh thông báo `std::cerr` khi `replaceSaveFile` hoặc `writeSaveFile` thất bại trong `SaveManager.cpp`. Bổ sung log CWD path và data đã load ở constructor `GameManager.cpp` trong chế độ Debug.
  4. **Regression test (`save_session_tests`)**: Bổ sung unit test `testMidSessionDeathSavesHighScore` mô phỏng người chơi chết còn mạng -> thoát game -> restart executable -> load lại đĩa save kiểm tra điểm cao vẫn được bảo toàn (100% pass, 12/12 CTest pass).

### Entry #31: [Merge TV5 Sound & Input Enhancements Branch] - Merged feature/sound-input into develop
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (12/12 ctest passed).
- **File ảnh hưởng:** `ASSETS_LIST.md`, `FILE_STRUCTURE.md`, `README.md`, `docs/blocks_coordinate.md`, `docs/items_objects_coordinate.md`, `docs/management/TV5_CHANGES_SUMMARY.md`, `include/core/ScoreRules.h`, `src/core/ScoreRules.cpp`, `include/core/SoundManager.h`, `src/core/SoundManager.cpp`, `include/patterns/InputHandler.h`, `src/patterns/InputHandler.cpp`, `include/ui/HUD.h`, `src/ui/HUD.cpp`, `src/core/Game.cpp`, `src/entities/Mario.cpp`, `src/items/Coin.cpp`, `src/patterns/EntityFactory.cpp`, `src/states/PauseState.cpp`, `tests/TV5IntegrationTests.cpp`
- **Mô tả:**
  1. Hợp nhất nhánh `feature/sound-input` của TV5 vào `develop`.
  2. Bổ sung hệ thống quản lý âm thanh `SoundManager`, voice pool cho SFX, named tracks cho Music, và khôi phục nhạc level sau khi Star expired.
  3. Cập nhật `InputHandler` với cờ suppression `gameplayEnabled`, khóa phím `Shift` chỉ chạy và `X` chỉ bắn.
  4. Chuẩn hóa `HUD` với timer callbacks, font degrade control, và nhãn `POWER` đồng bộ.
  5. Chuyển toàn bộ reference atlases từ `assets/` ra `docs/assets/reference/` để tối ưu hóa dung lượng gói release.

### Entry #32: [Merge TV5 Feature Branch - Audio Cues, Defeat Centralization & Integration Tests] - Merged feature/sound-input into develop
- **Trạng thái:** Đã hoàn thành, build & test pass 100% (12/12 ctest passed).
- **File ảnh hưởng:** `assets/ASSETS_LIST.md`, `docs/management/TV5_AUDIO_HUD_ITEM_CHECKLIST.md`, `docs/management/TV5_CHANGES_SUMMARY.md`, `include/core/ScoreRules.h`, `include/core/SpriteFrames.h`, `include/entities/Enemy.h`, `include/entities/Koopa.h`, `include/patterns/EventType.h`, `include/physics/CollisionManager.h`, `src/core/ScoreRules.cpp`, `src/core/SoundManager.cpp`, `src/entities/Enemy.cpp`, `src/entities/Goomba.cpp`, `src/entities/Koopa.cpp`, `src/entities/Mario.cpp`, `src/physics/CollisionManager.cpp`, `src/states/PlayState.cpp`, `tests/TV5IntegrationTests.cpp`
- **Mô tả:**
  1. Hợp nhất nhánh `origin/feature/sound-input` của TV5 vào `develop` theo yêu cầu của Architect (TV1).
  2. Tập trung xử lý tiêu diệt kẻ địch và tính điểm trong `CollisionManager` với các sự kiện per-cause (`STOMP`, `SHELL`, `FIREBALL`, `STAR`).
  3. Bổ sung âm thanh SFX đặc thù theo nguyên nhân tiêu diệt (shell kill, fireball hit, star kill) và xử lý phát nhạc khi resume game từ PauseState.
  4. Mở rộng `TV5IntegrationTests` với kiểm thử cho runtime defeat paths, state audio switching, và asset manifest verification.
  5. Toàn bộ 12 test suite đều thông qua (100% pass) trên `develop`.

### Entry #33: [Assets & Core] - Tileset & TileMap Multi-Theme Support Integration
- **Trạng thái:** Đã hoàn thành, build & test pass 100%.
- **File ảnh hưởng:** `docs/tileset_coordinate.md`, `include/core/SpriteFrames.h`, `include/level/TileFrames.h`, `include/level/Level.h`, `src/level/Level.cpp`, `src/level/TileMap.cpp`, `src/states/PlayState.cpp`
- **Mô tả:**
  1. **Tọa độ Tileset Hoàn Chỉnh (`tileset_coordinate.md`)**: Phân tích toàn bộ 345 object từ `assets/textures/tiles/tileset.png`. Cập nhật tọa độ chuẩn vào `TileFrames.h` cho gạch nền (Ground, Brick, Question Block, Coin...) theo 3 môi trường: Overworld, Underground, Castle. 
  2. **Tọa độ SpriteFrames Bổ Sung**: Tích hợp tọa độ Map Coins tĩnh, Trampoline, Castle Axe, Bridge Chain và Elevator Pulley vào `SpriteFrames::LevelEntities` ở cuối `SpriteFrames.h`. Cắt bỏ tệp Python dư thừa.
  3. **TileMap Multi-Theme Engine (`TileMap.cpp`)**: Thay đổi kiến trúc render gạch nền từ hardcode Overworld sang Dynamic Multi-Theme. TileMap giờ đây đọc `LevelTheme` (được truyền vào qua `Level::setTheme()` từ `PlayState`) để render chính xác màu sắc khối gạch (Background, Brick, Question) theo từng màn chơi.

### Entry #34: [Merge TV5 Sound-Input & SpriteFrames Theme Refactoring] - Merged feature/sound-input into develop
- **Trạng thái:** Đã hoàn thành, build & test pass 100%.
- **File ảnh hưởng:** `CMakeLists.txt`, `docs/management/TV5_CHANGES_SUMMARY.md`, `include/core/SpriteFrames.h`, `include/core/SpriteFrames_castle.h`, `include/core/SpriteFrames_ovw.h`, `include/core/SpriteFrames_shared.h`, `include/core/SpriteFrames_udg.h`, `include/core/SpriteFrames_udw.h`, `include/entities/BlockDebris.h`, `include/entities/Goomba.h`, `include/entities/Koopa.h`, `include/level/Level.h`, `include/level/TileFrames.h`, `include/level/TileMap.h`, `include/patterns/EntityFactory.h`, `src/entities/FireBall.cpp`, `src/entities/Goomba.cpp`, `src/entities/Koopa.cpp`, `src/entities/Mario.cpp`, `src/entities/QuestionBlock.cpp`, `src/items/Coin.cpp`, `src/items/FireFlower.cpp`, `src/items/Mushroom.cpp`, `src/items/Star.cpp`, `src/level/Level.cpp`, `src/level/TileMap.cpp`, `src/patterns/EntityFactory.cpp`, `tests/SpriteFramesThemeTests.cpp`
- **Mô tả:**
  1. Hợp nhất thành công nhánh `feature/sound-input` mới nhất (commit `2099602`) vào nhánh `develop`.
  2. **Refactor SpriteFrames theo Theme Catalog**: Tách tệp header `SpriteFrames.h` thành các catalog riêng biệt theo môi trường (`SpriteFrames_shared.h`, `SpriteFrames_ovw.h`, `SpriteFrames_udg.h`, `SpriteFrames_castle.h`, `SpriteFrames_udw.h`), đồng thời giữ `SpriteFrames.h` làm aggregator thống nhất.
  3. **Lan truyền LevelTheme**: Truyền `LevelTheme` từ `Level` tới `TileMap`, `EntityFactory` và các thực thể game (`Goomba`, `Koopa`, `QuestionBlock`, `BlockDebris`...) giúp tự động render đúng bảng màu sprite tương ứng.
  4. **Bổ sung Theme Unit Tests**: Thêm bộ test `SpriteFramesThemeTests.cpp` kiểm thử tính chính xác của các thuộc tính và tọa độ frame theo từng chủ đề màn chơi.

### Entry #35: [Documentation & Assets] - Export Enemy PNG Sprites & Update Enemies Coordinate Documentation
- **Trạng thái:** Đã hoàn thành, 157/157 PNG frames exported & documented.
- **File ảnh hưởng:** `docs/enemies_coordinate.md`, `docs/assets/reference/enemies_all_components_atlas.png`, `docs/assets/reference/enemies_all_components_atlas_full.png`, `docs/assets/enemies/enemy_001.png`...`enemy_157.png`, `docs/assets/enemies_loang/enemy_001_loang.png`...`enemy_157_loang.png`, `scripts/export_enemy_pngs.py`, `scripts/update_enemies_md.py`
- **Mô tả:**
  1. **Xuất tệp PNG riêng cho từng khung hình quái vật (`docs/assets/enemies/`)**: Trích xuất toàn bộ 157 thành phần quái vật theo tọa độ Grid khung chuẩn (16×16, 16×24, 32×32, v.v.) từ tệp gốc `assets/textures/enemies/enemies.png`, tự động khử màu nền xanh NES `(0, 41, 140)` và `(146, 144, 255)` sang nền trong suốt Alpha.
  2. **Cập nhật Bảng Tọa Độ & Hình Ảnh (`enemies_coordinate.md`)**: Bổ sung cột `Ảnh PNG` nhúng trực tiếp hình ảnh xem trước (integer scaled) cho tất cả 157 khung hình quái vật từ `#1` đến `#157`.
  3. **Tạo mới Ảnh minh họa Atlas Tổng hợp (`docs/assets/reference/`)**: Tải lại và tổng hợp tệp Atlas `enemies_all_components_atlas.png` và `enemies_all_components_atlas_full.png` với nét vẽ bounding box và đánh nhãn số STT trực quan.




### 36. Semantic Fix for Enemy Sprite Frames and Animations
- **Date**: 2026-08-10
- **Author**: AI Assistant
- **Modified Files**: 
  - `docs/enemies_coordinate.md`
  - `include/core/SpriteFrames_shared.h`
  - `src/entities/Koopa.cpp`
  - `src/entities/Goomba.cpp`
- **Logic Changes**:
  - Performed a visual deep scan of `enemies.png` and discovered severe semantic misalignments.
  - Corrected `enemies_coordinate.md` to properly label Underwater Paratroopas and Koopas (STT #61-#64) since their layout in the spritesheet was reversed compared to Overworld.
  - Overhauled `SpriteFrames_shared.h` to fix critical copy-paste errors across the board:
    - Fixed Buzzy Beetle shell references (Overworld and Underground were pointing to incorrect or walking frames).
    - Fixed Blooper and Bullet Bill (Coordinates were totally misaligned for Underground, Castle, and Underwater).
    - Fixed Piranha Plant (Open and Closed frames were swapped and scrambled across themes).
    - Fixed Red Koopa and Red Paratroopa (Walk 2 and Fly 2 were swapped).
    - Fixed Red Spiny Walk frames (inverted animation).
  - Updated `Koopa.cpp` and `Goomba.cpp` to correctly resolve `LevelTheme::UNDERWATER` animations.

## Entry 36: Fixed TileFrames.h Coordinates & TileMap Texture Bleeding

- **Author**: TV1 (Dương)
- **Modified Files**:
  - `include/level/TileFrames.h`
  - `src/level/TileMap.cpp`
- **Logic Changes**:
  - Corrected `STONE` in `TileFrames.h` from `{0, 33}` (which was erroneously pointing to Castle Battlement) to `{34, 16}` (Object #3 - Stone / Solid Stair Block).
  - Standardized `QUESTION` to `{85, 16}` (Object #6 - Row 1 Question Block) and `USED_BLOCK` to `{51, 16}` (Object #4 - Row 1 Used Block).
  - Fixed `CASTLE_WALL` from `{102, 33}` (Solid Black Filler) to `{34, 33}` (Object #39 - Castle Brick Wall).
  - Added `STONE_UNDERGROUND` (`{198, 16}` - Object #12, Khối đá vuông 4 đinh Teal) definition in `TileFrames.h` and updated `TileMap.cpp` (`getTilesetRect`) so symbol `'S'` correctly renders Underground Teal Stone in Level 2 instead of Overworld Brown Stone or Teal Brick.
  - Added a `0.02f` pixel UV inset in `TileMap::buildVertices` to completely eliminate 1-pixel border texture bleeding caused by subpixel camera position sampling.
  - Verified full build clean success and 13/13 ctest pass.

### 37. Implemented Fireball Explosion Visual Particle Effect
- **Date**: 2026-08-11
- **Author**: TV3 & TV1
- **Modified Files**:
  - `include/entities/FireballExplosion.h`
  - `src/entities/FireballExplosion.cpp`
  - `include/entities/FireBall.h`
  - `src/entities/FireBall.cpp`
  - `include/level/Level.h`
  - `src/level/Level.cpp`
  - `src/physics/CollisionManager.cpp`
- **Logic Changes**:
  - Implemented `FireballExplosion` visual effect entity displaying 3 animation frames (`fireballExplosionFrames`) from `assets/textures/items/items_objects.png` over 0.15 seconds before self-removal.
  - Set `FireballExplosion::getType()` to `EntityType::TERRAIN` so explosion particles are not mistakenly identified as `isFireBall()`, preventing invalid pointer casting and preventing particles from blocking Mario's maximum active fireball limit.
  - Updated `Level::update()` to collect explosion positions into a temporary list prior to spawning `FireballExplosion` entities, completely preventing `std::vector` iterator invalidation during iteration.
  - Added `CollisionManager.cpp` overhead ceiling contact check (`normal.y < -0.5f`) so fireballs bouncing into low ceilings/overhead blocks explode immediately rather than clipping or gliding through ceilings.
  - Fixed missing `setOwner(m_mario.get())` calls in `Level::shootFireBall()` and `Level::processPendingFireballs()` ensuring Mario receives full score points when enemies are defeated by fireballs.
  - Verified clean build and 13/13 test suite pass (`ctest`).

### 38. Implemented Piranha Plant Enemy Class & Sensor Physics Architecture
- **Date**: 2026-08-11
- **Author**: TV4 & TV1
- **Modified Files**:
  - `include/entities/Entity.h`
  - `include/entities/PiranhaPlant.h`
  - `src/entities/PiranhaPlant.cpp`
  - `include/patterns/EntityFactory.h`
  - `src/patterns/EntityFactory.cpp`
  - `src/physics/CollisionManager.cpp`
  - `src/level/Level.cpp`
  - `src/level/TileMap.cpp`
  - `levels/level2.txt`
- **Logic Changes**:
  - Implemented `PiranhaPlant` enemy class featuring a 4-stage vertical emergence state machine (`EMERGING`, `WAITING_TOP`, `RETRACTING`, `WAITING_BOTTOM`).
  - Configured NES-accurate 2.0x scale factor constants: NES sprite 16x24px @ 2.0x scale = 32x48px entity size, with `TRAVEL_DISTANCE = 64.f` px (2 full tiles emergence height) and `PROXIMITY_RADIUS = 44.f` px (1 tile width).
  - Wired real-time `updateMarioProximity(marioPos)` inside `Level::update` so Piranha Plant dynamically detects Mario's distance before emerging from the pipe.
  - Configured Box2D physics body as `b2_kinematicBody` with sensor fixture (`isSensor = true`), completely eliminating physics tile collision jittering and tunneling inside solid pipe tiles.
  - Fixed sprite orientation and pipe fitting: Used upright sprite frames (`CLOSED` `{0, 138}` and `OPEN` `{18, 138}`) ensuring head points UP into the sky, and set retracted base position to `(position.x + 16.f, position.y + 16.f)` with `TRAVEL_DISTANCE = 64.f` so the 48px plant fits 100% inside the 64px pipe (0px exposed below), and emerges 48px high (full plant height) into the air above the pipe rim.
  - Adjusted `PROXIMITY_RADIUS` to `20.f` px so Piranha Plant aggressive emergence is active as Mario approaches from the sides to attack/bite Mario, only pausing if Mario stands directly centered on top of the pipe rim.
  - Configured theme-responsive mouth animations (`OPEN` and `CLOSED` frames) supporting Overworld, Underground, Castle, and Underwater palettes from `SpriteFrames_shared.h`.
  - Updated `CollisionManager` to ensure Mario takes damage (`queuePowerDown()`) when stomping or colliding with Piranha Plant, while allowing `FireBall` projectiles and `Star` invincibility to defeat the plant and award 200 score points.
  - Configured `p` and `r` tile codes in `TileMap.cpp` (`getTilesetRect`, `isSolidTileSymbol`, `isRenderableTile`, `isForegroundTile`) to render directly as Pipe Top-Left (`PIPE_TOP_LEFT`) and Pipe Top-Right (`PIPE_TOP_RIGHT`) tiles.
  - Set `'p'` as the sole PiranhaPlant spawn code in `SPAWN_CODES` (removing `'r'`), guaranteeing exactly ONE Piranha Plant per pipe when using `pr` on pipe tops in `levels/level2.txt`.
  - Verified clean build and 13/13 test suite pass (`ctest`).

### 39. Merged TV5 Spritesheet Coordinate Fixes & Integrated Theme Rects
- **Date**: 2026-08-11
- **Author**: TV5 & TV1
- **Modified Files**:
  - `docs/enemies_coordinate.md`
  - `include/core/SpriteFrames_shared.h`
  - `src/entities/PiranhaPlant.cpp`
- **Logic Changes**:
  - Merged TV5's commit `2fec5e5be7867b31602f2bc62e8b16e8ff2e7b32` from `origin/feature/sound-input` into `develop`.
  - Integrated TV5's corrected PiranhaPlant sprite coordinates in `SpriteFrames_shared.h` (`UG_CASTLE_OPEN` `{146, 138}` and `UG_CASTLE_CLOSED` `{164, 138}`), updating `PiranhaPlant::initAnimations` to use theme-responsive Underground/Castle brown palette rects.
### 40. Fixed Springboard Texture Path, 2.0x Scaling & Origin Alignment
- **Date**: 2026-08-11
- **Author**: TV1
- **Modified Files**:
  - `src/entities/Springboard.cpp`
  - `include/entities/Springboard.h`
  - `docs/change_in_develop.md`
- **Logic Changes**:
  - Fixed invalid texture ID `"items"` in `Springboard::Springboard` constructors to point to `"assets/textures/items/items_objects.png"`. Resolves texture loading failure log `ERROR: Failed to load texture 'items'` and restores real sprite rendering.
  - Added 2.0x sprite scaling (`setScale({2.0f, 2.0f})`) in `Springboard::initTheme` and `Springboard::update`, scaling NES 16px wide sprites to 32px world tile width.
  - Fixed sprite origin to bottom-left `(0.f, rect.height)` and set position to `{m_position.x, m_position.y + m_size.y}` in `Springboard::draw`, anchoring the springboard base to the ground cell while expanding upward during compression (`15px`), mid (`23px`), and launching (`31px`) states.
  - Cleaned up unused `#include <vector>` header in `include/entities/Springboard.h`.
  - Verified 14/14 test suites pass (`ctest`), including `springboard_tests`.

### 41. Added Level 4 Castle Mechanics Test Level
- **Date:** 2026-08-11
- **Author:** Codex
- **Status:** Completed; clean build and 14/14 CTest suites passed.
- **Modified Files:**
  - `levels/level4.txt`
  - `include/core/LevelCatalog.h`
  - `include/core/AnimationSystem.h`
  - `include/entities/Mario.h`, `src/entities/Mario.cpp`
  - `include/level/Level.h`, `src/level/Level.cpp`
  - `include/level/TileMap.h`, `src/level/TileMap.cpp`
  - `include/patterns/InputHandler.h`, `src/patterns/InputHandler.cpp`
  - `src/states/PlayState.cpp`
  - `src/entities/Goomba.cpp`, `src/entities/Koopa.cpp`
  - `src/entities/PiranhaPlant.cpp`
  - `tests/LevelCatalogTests.cpp`, `tests/PlayStateTests.cpp`, `tests/MarioPhysicsTests.cpp`, `tests/TV5IntegrationTests.cpp`, `tests/Gate0ContractTests.cpp`
- **Logic Changes:**
  1. Added release progression `1-1 Overworld -> 1-2 Underground -> 1-3 Underwater -> 1-4 Castle -> Win`, with Castle music assigned to Level 4 and unlock persistence capped at the playable catalog size.
  2. Added `levels/level4.txt` as a 96x16 integration map containing blocks, question blocks, coins, power-ups, Goomba, Koopa, Piranha pipe symbols, springboard, solid Castle terrain, vine markers and a validated `F/|` finish pole.
  3. Added non-solid `V` climbable tiles and a textured vine render layer using `items_objects.png`. Mario can attach while holding W/Up or S/Down, climb at a fixed speed, and detach with A/D.
  4. Added Mario `swim` and `climb` animation clips for Mario/Luigi power states. Existing Underwater physics now selects the swim animation while submerged.
  5. Replaced immediate finish completion with a one-shot flagpole sequence: input and gravity are locked, Mario slides to the pole base at a fixed speed, and exactly one `LEVEL_COMPLETED` event is emitted afterward.
  6. Added Underwater Goomba, Koopa and Piranha Plant frame selection so Level 3 no longer falls back to Overworld enemy sprites.
  7. Made Level cleanup explicitly destroy entity/tile Box2D bodies before the owning world, preventing stale-world destruction during reload/destruction.
  8. Updated catalog, progression, audio-track, Level 4 marker/V checks and flag-sequence integration tests for all four release levels. Verified `cmake --build build-tests --parallel 4` and `ctest --test-dir build-tests --output-on-failure` with 14/14 tests passing.
  9. Added direct Mario physics regression coverage for mid-air swim strokes and vine climbing (vertical velocity, gravity lock and horizontal detach), prevented simultaneous horizontal input from reattaching Mario to a vine, and verified Level theme wiring plus Level 4 enemy/item spawning.
  10. Added a procedural wave phase to every themed `F` flag quad, kept the pole-side edge anchored, and added `climb_up`, `climb_down`, and `climb_idle` clips for all Mario/Luigi power states. Off-screen rendering now verifies flag motion across Overworld, Underground, Underwater, and Castle.

### 42. Removed Background Color Pixels (108, 106, 255)
- **Date:** 2026-08-12
- **Author:** Antigravity
- **Status:** Completed; verified background transparency.
- **Modified Files:**
  - `assets/textures/items/items_blocks.png`
  - `assets/textures/items/items_objects.png`
  - `assets/textures/mario/MarioLuigi.png`
- **Logic Changes:**
  1. Replaced background pixels matching color `RGB(108, 106, 255)` with fully transparent pixels `RGBA(0, 0, 0, 0)` in `items_objects.png` (2,019 pixels) and `MarioLuigi.png` (1,327 pixels). `items_blocks.png` was verified to already be free of `(108, 106, 255)` background pixels.
  2. Verified all 3 texture files contain 0 remaining opaque pixels with color `(108, 106, 255)`.

> Entries 43–46 below are dated implementation snapshots. Their 20/20 and
> four-level-completion statements are historical and are not current RC
> evidence; the current 21/21 Debug result and remaining gates are recorded in
> Entry 47.

### 43. Sprint 7 TV1 4-Level Release Contract & Test Synchronization (`S7-TV1-01`..`04`)
- **Date:** 2026-08-15
- **Author:** TV1 (Dương)
- **Status:** Historical snapshot — completed then; clean build and 20/20 CTest suites passed. Not current RC evidence.
- **Modified Files:**
  - `include/core/LevelCatalog.h`
  - `levels/level4.txt`
  - `tests/LevelCatalogTests.cpp`
  - `tests/PlayStateTests.cpp`
  - `tests/TV5IntegrationTests.cpp`
  - `tests/Gate0ContractTests.cpp`
- **Logic Changes:**
  1. Formalized Sprint 7 4-level release contract in `LevelCatalog.h`: `1 (1-1 Overworld, OVERWORLD)` -> `2 (1-2 Underground, UNDERGROUND)` -> `3 (1-3 Underwater, UNDERWATER)` -> `4 (1-4 Castle, CASTLE)` -> `Win boundary at 5`.
  2. Synchronized `LevelCatalogTests.cpp` to verify 4 release levels, world labels `1-1` to `1-4`, music IDs, and boundary `isPastFinalLevel(5) == true`.
  3. Synchronized `PlayStateTests.cpp` progression simulation to test `1 -> 2 -> 3 -> 4 -> Win`.
  4. Synchronized `TV5IntegrationTests.cpp` audio progression assertions across all 4 levels (`OVERWORLD`, `UNDERGROUND`, `UNDERWATER`, `CASTLE`) to `WIN`.
  5. Updated `levels/level4.txt` with valid Castle finale layout meeting all block route and validator constraints; updated `Gate0ContractTests.cpp` release level loading to include `level4.txt`.
  6. Verified full 20/20 test suites pass with 0 failures under CTest at that snapshot; this is not the current registered-suite result.

### 44. Level / Stage Select UI & Navigation Integration (`LevelSelectState`)
- **Date:** 2026-08-15
- **Author:** TV1 (Dương)
- **Status:** Historical snapshot — completed then; clean build and 20/20 CTest suites passed. Not current RC evidence.
- **Modified Files:**
  - `include/states/LevelSelectState.h`
  - `src/states/LevelSelectState.cpp`
  - `include/states/CharacterSelectState.h`
  - `src/states/CharacterSelectState.cpp`
### 45. Stage Select UI Polish: Clean Snapshot Previews & Streamlined Layout
- **Date:** 2026-08-15
- **Author:** TV1 (Dương)
- **Status:** Historical snapshot — completed then; clean build and 20/20 CTest suites passed. Not current RC evidence.
- **Modified Files:**
  - `include/states/LevelSelectState.h`
  - `src/states/LevelSelectState.cpp`
  - `tests/DisplayCameraUITests.cpp`
  - `assets/textures/ui/stage_1.png`
  - `assets/textures/ui/stage_2.png`
  - `assets/textures/ui/stage_3.png`
  - `assets/textures/ui/stage_4.png`
- **Logic Changes:**
  1. Streamlined `LevelSelectState` layout according to "Less, but better" design aesthetics, removing redundant metadata lines (`DIFF`, `STYLE`, `ENEMY`, `STATUS`).
  2. Downloaded and integrated authentic retro NES gameplay screenshots for all 4 release stages (1-1 Overworld, 1-2 Underground, 1-3 Underwater, 1-4 Castle).
  3. Added smart top-HUD cropping (`topCrop = 32px`) to eliminate squished score/time numbers, presenting pristine in-game scenery.
  4. Aligned screen header (`SELECT WORLD & STAGE`, `Choose a stage to begin your adventure`) and footer navigation hints (`[A / D] SELECT STAGE    [ENTER / CLICK] START GAME    [ESC] BACK`) with balanced margins and zero clipping.
  5. Added dynamic action tags (`> PLAY <` in gold on active card, `STAGE N` on inactive cards) with animated golden pulsing borders.
  6. Verified via offscreen rendered snapshot inspection and 20/20 passing CTest suites.

### 46. Code Review Remediation for Commit `cc80466` (P0/P1/P2 Quality & Performance Fixes)
- **Date:** 2026-08-15
- **Author:** TV1 (Dương)
- **Status:** Historical snapshot — completed then; clean build with 0 warnings, 20/20 CTest suites passed (100% pass rate). Not current RC evidence.
- **Modified Files:**
  - `include/items/Item.h`
  - `include/items/Coin.h`
  - `include/level/Level.h`
  - `src/level/Level.cpp`
  - `include/entities/Mario.h`
  - `src/entities/Mario.cpp`
  - `include/states/LevelSelectState.h`
  - `src/states/LevelSelectState.cpp`
  - `include/states/CharacterSelectState.h`
  - `src/states/CharacterSelectState.cpp`
  - `levels/level4.txt`
  - `tests/LevelValidatorTests.cpp`
  - `docs/change_in_develop.md`
- **Logic Changes:**
  1. **P0/P1 — Dynamic Cast Elimination in Hot Loop (`Level.cpp`)**:
     - Added `virtual bool isCoin() const { return false; }` in `include/items/Item.h`.
     - Overrode `bool isCoin() const override { return true; }` in `include/items/Coin.h`.
     - Replaced `dynamic_cast<const Coin*>` inside `Level::checkItemCollisions()` with `item->isCoin()` virtual check and safe `static_cast<const Coin*>`, completely removing RTTI overhead in the hot collision loop (adhering to `AGENTS.md` Rule #5).
  2. **P0/P1 — Mario Update Function Length & Physics Sync Order (`Mario.cpp`)**:
     - Moved `syncPhysics()` unconditionally to line 1 of `Mario::update(float dt)`, ensuring Box2D physics solver state is always synchronized before any movement logic runs (adhering to `AGENTS.md` Rule #3).
     - Decomposed the ~176-line `Mario::update()` into 5 clean, focused helper methods: `handleDeathPhase(dt)`, `handleTransformPhase(dt)`, `handleSpawnPhase(dt)`, `updateMovementAnimations(dt)`, and `applyWorldBoundsClamp()`.
     - Reduced `Mario::update()` to ~40 lines, strictly complying with the 40-50 line convention.
  3. **P0/P1 — Level Update Function Length Deconstruction (`Level.cpp`)**:
     - Decomposed the ~235-line `Level::update()` by extracting `updateFlagSequence(dt)`, `updateEntities(dt)`, and `updateExplosions()`.
     - Reduced `Level::update()` to ~45 lines.
  4. **P2 — Decomposed `onEnter()` in Select States (`LevelSelectState.cpp`, `CharacterSelectState.cpp`)**:
     - Split `LevelSelectState::onEnter()` into `initStageTextures()`, `initBackdropPanel()`, `initTextLabels()`, and `initStageCards()`.
     - Split `CharacterSelectState::onEnter()` into `initBackdropPanel()`, `initCards()`, `initAvatars()`, `initTextLabels()`, and `initMenu()`.
     - Silenced nodiscard compiler warning for image loading in fallback preview path.
  5. **P2 — Cleaned Trailing Empty Lines in `levels/level4.txt`**:
     - Removed trailing empty lines 26-27 so `level4.txt` conforms cleanly to rectangular map bounds.
  6. **P2 — Extended `LevelValidatorTests.cpp` Coverage**:
     - Added `testProductionLevelsLoad()` ensuring all 4 release levels (`level1.txt`, `level2.txt`, `level3.txt`, `level4.txt`) are validated for valid non-zero dimensions, single Mario spawn (`M`), single Flagpole (`F`/`T`), and valid parsing.
  7. **Release Gate Verification**:
     - Ran full CTest suite: all 20/20 test suites passed in 8.79s with 0 failures at that historical snapshot; this is not current RC evidence.

### 47. Sprint 7 verification correction and release-evidence status
- **Date:** 2026-08-16
- **Author:** Documentation update from independent tester evidence
- **Status:** Current evidence is mixed; no final RC sign-off.
- **Current facts:** The catalog metadata contract is four entries (`1-1`
  Overworld, `1-2` Underground, `1-3` Underwater, `1-4` Castle). Retry,
  `highestUnlockedLevel` Level Select bounds, and v1 monotonic save bounds are
  covered by current state/save tests. Fresh writable isolated MinGW Debug and
  Release `BUILD_TESTING=ON` CTest runs each passed `21/21`; a Release
  `BUILD_TESTING=OFF` production build also passed. Test-target-only NDEBUG
  restoration gives test targets `-DNDEBUG` then `-UNDEBUG`, while
  `game_lib`/`SuperMario` retain only `-DNDEBUG`; no assert-disabled or
  production compiler warnings were observed apart from external Box2D CMake
  deprecation. MSVC was statically reviewed but not executed.
- **TV5 facts:** Four music tracks and Star/death/GameOver/Win/volume/HUD and
  controlled missing/invalid-track behavior pass automated integration tests.
  The package allowlist is 37 assets plus 6 levels/configs (43 entries), with
  clean Debug/Release inventory and negative missing-file configure evidence.
- **Limitations:** `levels/level3.txt` remains Castle-style and its semantic
  Underwater acceptance/playthrough is a TV4-owned `BLOCKED` gate. Manual
  four-level playthrough, screenshots, device-audio, source/license
  attribution, `S6-TV5-43/44` dispositions, `BUG-038`, a same-final-RC hash,
  and final TV1/TV5 sign-off remain `REVIEW`/`PENDING`.
- **References:** [S7_TV1_TV5_STATUS.md](management/S7_TV1_TV5_STATUS.md),
  [S7_TV5_PACKAGE_MANIFEST.md](management/S7_TV5_PACKAGE_MANIFEST.md),
  [TV1_CHANGES_SUMMARY.md](management/TV1_CHANGES_SUMMARY.md), and
  [TV5_CHANGES_SUMMARY.md](management/TV5_CHANGES_SUMMARY.md).

### 48. Fix Fire State Growth/Shrink Transformation Animation Selection
- **Date:** 2026-08-16
- **Author:** TV1 (Dương)
- **Status:** Completed; clean build and 21/21 CTest suites passed (100% pass rate).
- **Modified Files:**
  - `src/entities/Mario.cpp`
  - `tests/MarioPhysicsTests.cpp`
- **Detailed Logic Changes:**
  1. **Fixed Priority in `isGrowth` / `isShrink` in `Mario::applyStateTransition()`**:
     - Previously, `m_characterType == CharacterType::LUIGI` was evaluated before `usesFireTransition`, causing Luigi in `FIRE_SMALL` upgrading to `FIRE_SUPER` (e.g., collecting a Mushroom) to mistakenly play `GrowShrink::Luigi::growSequence()` (normal green Luigi small->medium->big frames) instead of Fire transformation frames.
     - Changed the sequence selector to prioritize `usesFireTransition` (`GrowShrink::FireMario::growSequence()` / `shrinkSequence()`) for both Mario and Luigi whenever transitioning between Fire body states, and preserved character-specific normal sequences (`Luigi::growSequence()` / `Mario::growSequence()`) for standard non-fire state transitions.
  2. **Character-Aware Fire Upgrade/Downgrade Sequence**:
     - Updated `isFireUpgrade || isFireDowngrade` presentation branch so Luigi properly uses `GrowShrink::Luigi::bigFireSequence()` / `smallFireSequence()`, while Mario uses `GrowShrink::FireMario::bigFireSequence()` / `smallFireSequence()`.
  3. **Regression Test Coverage in `MarioPhysicsTests.cpp`**:
     - Extended `testLuigiTransformationUsesLuigiRows()` to verify that `FIRE_SMALL` + Mushroom -> `FIRE_SUPER` explicitly renders frames from `SpriteFrames::shared::GrowShrink::FireMario::BIG` instead of the green normal Luigi row.
  4. **Release Gate Verification**:
     - Ran full CTest suite: all 21/21 test suites passed in 9.50s with 0 failures.

### 49. Fix Defeat Score Popups (Fireball, Shell, Star, Block Bump) & Implement Shell Kill Streak
- **Date:** 2026-08-16
- **Author:** TV3 (Bảo) & TV1 (Dương)
- **Status:** Completed; clean build and 21/21 CTest suites passed (100% pass rate).
- **Modified Files:**
  - `include/entities/Mario.h`
  - `src/entities/Mario.cpp`
  - `include/entities/Koopa.h`
  - `src/entities/Koopa.cpp`
  - `include/physics/CollisionManager.h`
  - `src/physics/CollisionManager.cpp`
  - `src/level/Level.cpp`
  - `tests/StompScoreTests.cpp`
- **Detailed Logic Changes:**
  1. **Generic Score Award Queueing (`Mario::queueScoreAward`)**:
     - Added `Mario::queueScoreAward(position, points, grantsLife)` to handle score point awards, 1UP lives addition + `EventType::ONE_UP_COLLECTED` notification, and queueing of `StompScoreAward` for `Level` to render as `ScorePopup`.
  2. **Fireball Defeat Score Popup (`CollisionManager::defeatEnemy`)**:
     - Fixed missing score popup when enemies are defeated by FireBall (`DefeatCause::FIREBALL`). It now queues a 200-point `ScorePopup` at the defeated enemy's center position.
  3. **Koopa Shell Sliding Streak & Combo Scoring (`Koopa` & `CollisionManager`)**:
     - Added `m_shellKillStreak` to `Koopa` class to track consecutive kills by the sliding shell; reset on kick, stomp, and death.
     - Updated `CollisionManager::defeatEnemy` with `streakIndex` parameter applying canonical SMB1 progression: `200 -> 400 -> 800 -> 1000 -> 2000 -> 4000 -> 5000 -> 8000 -> 1UP` and spawning appropriate `ScorePopup`s at each victim's position.
     - Updated `CollisionManager::tryShellKill` lambda to query and increment shell kill streak on successful defeat transactions.
  4. **Star & Block Bump Score Popups**:
     - Added score popup generation for Starman kills (200 pts) and Block bump kills (100 pts).
  5. **Immediate Score Popup Processing in `Level::update()`**:
     - Added a second drain call to `processPendingStompScorePopups()` after `updateEntities()` and `removeDeadEntities()` so popups generated during entity/block updates appear in the very same frame without 1-frame latency.
  6. **Unit Test Suite Expansion (`StompScoreTests.cpp`)**:
     - Added `testFireballDefeatScoreAndPopup()`, `testSlidingShellDefeatChainAndPopups()` (testing full 8-step score progression + 1UP), and `testStarAndBlockBumpScoreAndPopups()`.
  7. **Release Gate Verification**:
     - Ran full CTest suite: all 21/21 test suites passed in 5.81s with 0 failures.

### 50. Item/Enemy Theme Support, Pixel Sharpness & Title Screen Redesign
- **Date:** 2026-08-16
- **Author:** TV1 (Dương), TV2 (Nhật), TV5 (Truyền)
- **Status:** Completed; clean build and 21/21 CTest suites passed (100% pass rate).
- **Modified Files:**
  - `include/items/Coin.h`
  - `src/items/Coin.cpp`
  - `include/items/Mushroom.h`
  - `src/items/Mushroom.cpp`
  - `src/entities/QuestionBlock.cpp`
  - `include/patterns/EntityFactory.h`
  - `src/patterns/EntityFactory.cpp`
  - `src/entities/CheepCheep.cpp`
  - `levels/level3.txt`
  - `levels/cheep_cheep.txt`
  - `levels/elevators.txt`
  - `include/states/MenuState.h`
  - `src/states/MenuState.cpp`
  - `src/states/LevelSelectState.cpp`
  - `src/states/CharacterSelectState.cpp`
  - `src/states/PauseState.cpp`
  - `src/states/GameOverState.cpp`
  - `src/states/WinState.cpp`
  - `src/ui/HUD.cpp`
  - `tests/SpriteFramesThemeTests.cpp`
- **Detailed Logic Changes:**
  1. **Item Theme Propagation (`Coin` & `Mushroom`)**:
     - Added `LevelTheme` support to `Coin` and `Mushroom`. Map coins (`'C'`) and mushrooms now resolve palette-accurate sprites corresponding to the active `LevelTheme` (`OVERWORLD`, `UNDERGROUND`, `CASTLE`, `UNDERWATER`).
     - `QuestionBlock::onHit()` now maps its `BlockTheme` to `LevelTheme` and passes it to spawned `Coin` and `Mushroom` entities so power-ups emerging from blocks match the level palette.
     - Updated `EntityFactory::createItem` and `createFromTileCode` to forward `LevelTheme`.
  2. **Cheep Cheep Red/Green Swim Animation Fix (`CheepCheep.cpp`)**:
     - Fixed `CheepCheep::initAnimations` so red Cheep Cheeps (`m_color == CheepCheepColor::RED`) use `redSwimFrames()` across all themes (`UNDERGROUND`, `CASTLE`, `UNDERWATER`, `OVERWORLD`) instead of defaulting to green swim frames in non-overworld themes.
  3. **Level 3 (World 1-3 Underwater) Authentic Redesign (`levels/level3.txt`)**:
     - Converted `levels/level3.txt` from a legacy Castle duplicate into an authentic, rich Underwater coral reef stage featuring swimming Cheep Cheeps (`c`, `x`), coral formations (`1`), question blocks (`?`, `U`, `f`, `O`), and floating coin paths (`C`), fully compliant with `Gate0ContractTests` and `LevelValidatorTests`.
     - Updated `levels/cheep_cheep.txt` and `levels/elevators.txt` documentation and route comments.
  4. **Global UI Pixel Sharpness & Font Smoothing Elimination**:
     - Added `m_font.setSmooth(false)` across all UI states (`MenuState`, `LevelSelectState`, `CharacterSelectState`, `PauseState`, `GameOverState`, `WinState`, `HUD`) to eliminate font texture blurriness and achieve razor-sharp retro pixel typography.
     - Disabled texture smoothing on preview cards and character avatars (`tex.setSmooth(false)`).
  5. **Polished Title / Cover Screen Overhaul (`MenuState.cpp`)**:
     - Replaced the squished fractional-scaled 256px NES banner with a full-canvas (640x360) presentation:
       - Double-row ground tiles, classic green pipes, and floating question/brick blocks.
       - Top arcade HUD (`MARIO`, `WORLD 1-1`, `TIME 400`, `x00` spinning coin).
       - Floating 3D Title Plaque (`SUPER MARIO - CS202 REMASTERED`) with gold/crimson styling and subtle bobbing animation.
       - Live animated Mario (left), Luigi (right), and Goomba.
       - Arcade `TOP - 000000` high-score display and blinking `PRESS ENTER OR CLICK TO START`.
       - Interactive menu with animated Super Mushroom cursor.
  6. **Release Gate Verification**:
     - Ran full CTest suite: all 21/21 test suites passed in 5.37s with 0 failures.

### 51. Fix Score Popup Texture Rect Mapping in Shared Atlas
- **Date:** 2026-08-16
- **Author:** TV5 (Truyền) & TV1 (Dương)
- **Status:** Completed; clean build and 21/21 CTest suites passed (100% pass rate).
- **Modified Files:**
  - `include/core/SpriteFrames_shared.h`
  - `src/entities/ScorePopup.cpp`
  - `tests/SpriteFramesThemeTests.cpp`
- **Detailed Logic Changes:**
  1. **Atlas Coordinate Grid Alignment in `SpriteFrames_shared.h`**:
     - Discovered that score frame coordinates in `items_objects.png` were completely misaligned with the texture atlas:
       - `SCORE_200` was previously pointing to `(252, 26)` which is actually `1000`.
       - `SCORE_400` & `SCORE_800` were pointing to `(270, 26)` & `(288, 26)` which are empty/transparent areas.
       - `SCORE_1000` was pointing to `(234, 36)` which is actually `200`.
       - `SCORE_4000` & `SCORE_5000` were pointing to empty areas.
       - `SCORE_8000` was pointing to `(234, 46)` which is `400`.
       - `SCORE_1UP` was pointing to `(252, 46)` which is `4000`.
     - Corrected the entire score mapping to match the authentic 2-column vertical grid:
       - Column X=234: `SCORE_100` (Y=26), `SCORE_200` (Y=36), `SCORE_400` (Y=46), `SCORE_500` (Y=56), `SCORE_800` (Y=66).
       - Column X=252: `SCORE_1000` (Y=26), `SCORE_2000` (Y=36), `SCORE_4000` (Y=46), `SCORE_5000` (Y=56), `SCORE_8000` (Y=66), `SCORE_1UP` (Y=76).
  2. **Added `SCORE_500` Frame Support (`ScorePopup.cpp`)**:
     - Added `case 500: return SCORE_500;` to `ScorePopup::frameFor` to handle 500-point awards cleanly.
  3. **Expanded Regression Tests (`SpriteFramesThemeTests.cpp`)**:
     - Added `testScoreCoordinates()` asserting exact pixel positions for all 11 score text frames (`SCORE_100` .. `SCORE_8000`, `SCORE_1UP`).
  4. **Release Gate Verification**:
     - Ran full CTest suite: all 21/21 test suites passed in 10.09s with 0 failures.

### 52. Multi-Theme Score Popups & Title Screen Layout Fine-tuning
- **Date:** 2026-08-16
- **Author:** TV1 (Dương), TV2 (Nhật), TV5 (Truyền)
- **Status:** Completed; clean build and 21/21 CTest suites passed (100% pass rate).
- **Modified Files:**
  - `include/entities/ScorePopup.h`
  - `src/entities/ScorePopup.cpp`
  - `src/level/Level.cpp`
  - `include/states/MenuState.h`
  - `src/states/MenuState.cpp`
  - `tests/DisplayCameraUITests.cpp`
- **Detailed Logic Changes:**
  1. **Multi-Theme Score Popups Support (`ScorePopup` & `Level`)**:
     - Extended `ScorePopup` to take `LevelTheme` and resolve palette-themed score digits across all 4 environments:
       - Overworld: base Y offsets (Y=26..76)
       - Underground: cyan palette Y offsets (+108 -> Y=134..184)
       - Castle: stone grey palette Y offsets (+216 -> Y=242..292)
       - Underwater: teal palette Y offsets (+324 -> Y=350..400)
     - `Level::spawnScorePopup` forwards `m_theme` so floating defeat score numbers visually match the environment palette.
  2. **Title Screen Polish & Clutter Removal (`MenuState.cpp`)**:
     - Removed arbitrary bounding boxes and floating blocks that overlapped with menu text.
     - Replaced with authentic, clean NES Super Mario Bros title composition:
       - Large gold `SUPER MARIO BROS.` 3D title with crimson shadow.
       - Centered `(C) 1985 NINTENDO` and `TOP - 000000` high score.
       - Perfectly aligned `1 PLAYER GAME` and `STAGE SELECT` with animated Super Mushroom cursor.
       - Blinking `PRESS ENTER OR CLICK TO START` prompt.
       - Clear, unobstructed ground with animated Mario, Luigi, and Goomba.
  3. **Visual Verification (`DisplayCameraUITests.cpp`)**:
     - Added `testMenuStateRenderSnapshot()` to render and export full 640x360 frame snapshots for automated visual validation.
  4. **Release Gate Verification**:
     - Ran full CTest suite: all 21/21 test suites passed in 9.43s with 0 failures.

### 53. Title Cover Screen Branding & Clutter Cleanup
- **Date:** 2026-08-16
- **Author:** TV1 (Dương) & TV2 (Nhật)
- **Status:** Completed; clean build and 21/21 CTest suites passed (100% pass rate).
- **Modified Files:**
  - `include/states/MenuState.h`
  - `src/states/MenuState.cpp`
- **Detailed Logic Changes:**
  1. **Removed In-Game HUD Elements from Title Screen (`MenuState.cpp`)**:
     - Removed the in-game gameplay HUD bar (`MARIO 000000 | x00 WORLD 1-1 TIME 400`) and the `TOP - 000000` text to make the title screen clean, authentic, and uncluttered.
  2. **Added Course & Group Branding**:
     - Added `CS202 - OOP FINAL PROJECT` in crisp white retro pixel font directly under the 3D Title logo (`Y=108`).
     - Added `GROUP 04` in arcade gold retro pixel font (`Y=126`).
  3. **Optimized Spacing & Vertical Hierarchy**:
     - Balanced menu options `1 PLAYER GAME` and `STAGE SELECT` (`Y=182`) with aligned animated mushroom cursor.
     - Positioned copyright `(C) 1985 NINTENDO` cleanly below the menu (`Y=228`).
     - Maintained clear open sky for blinking `PRESS ENTER OR CLICK TO START` (`Y=262`).
  4. **Release Gate Verification**:
     - Ran full CTest suite: all 21/21 test suites passed in 5.70s with 0 failures.

### 54. Title Screen Pipe Symmetry & Title Punctuation Cleanup
- **Date:** 2026-08-16
- **Author:** TV1 (Dương) & TV2 (Nhật)
- **Status:** Completed; clean build and 21/21 CTest suites passed (100% pass rate).
- **Modified Files:**
  - `src/states/MenuState.cpp`
- **Detailed Logic Changes:**
  1. **Centered & Symmetrized Green Pipes (`MenuState.cpp`)**:
     - Adjusted right pipe position from `x=576` (which touched the right edge) to `x=544..608`.
     - Now both the left pipe (`x=32..96`) and right pipe (`x=544..608`) maintain an identical 32px margin from the screen borders for flawless horizontal symmetry.
  2. **Balanced Ground Characters**:
     - Centered Goomba precisely at `x=304` (center at `x=320`).
     - Balanced Mario at `x=184` (center at `x=200`, -120px from center) and Luigi at `x=424` (center at `x=440`, +120px from center).
  3. **Removed Trailing Dot in Title**:
     - Changed `"SUPER MARIO BROS."` to `"SUPER MARIO BROS"` in both shadow and main text.
  4. **Release Gate Verification**:
     - Ran full CTest suite: all 21/21 test suites passed in 9.70s with 0 failures.

### 55. Authentic NES Underwater Cheep Cheep Sprite Configuration
- **Date:** 2026-08-16
- **Author:** TV4 (Vy) & TV1 (Dương)
- **Status:** Completed; clean build and 21/21 CTest suites passed (100% pass rate).
- **Modified Files:**
  - `include/core/SpriteFrames_udw.h`
  - `src/level/Level.cpp`
- **Detailed Logic Changes:**
  1. **Maintained Authentic NES Underwater Palette (`SpriteFrames_udw.h`)**:
     - Preserved `UW_SWIM_UP ({292, 164})` and `UW_SWIM_DOWN ({310, 164})` as canonical underwater swimming fish frames, honoring the authentic grey/white NES 1985 underwater hardware palette for World 1-3 / World 2-2.
     - Kept `RED_SWIM_UP ({0, 370})` available for red jumping fish variants.
  2. **Configured Level Cheep Cheep Routes (`Level.cpp`)**:
     - Set `Level::spawnCheepCheepRoutesFromTileMap` to spawn `CheepCheepColor::GREEN` so underwater levels render the authentic NES grey/white Cheep Cheeps swimming along reef routes.
  3. **Release Gate Verification**:
     - Ran full CTest suite: all 21/21 test suites passed in 9.12s with 0 failures.

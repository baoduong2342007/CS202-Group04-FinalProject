# Plan: Chế độ PvP "Stomp Duel" — Best-of-3 + Tranh chấp Hoa Lửa

## Tổng quan

Thêm chế độ 2 người chơi trên cùng bàn phím:

```
MenuState ("2 PLAYER VERSUS") → PvpCharacterSelectState (P1 chọn, rồi P2 chọn)
  → PvpPlayState: arena 1 màn hình, camera cố định, best-of-3
```

**Cơ chế:**
- **Đạp lên đầu đối thủ = thắng round** (cơ chế win duy nhất); thắng 2/3 round thắng match. Enter = rematch, Esc = menu.
- **Hoa lửa**: sau khoảng random 6–12s (chỉ chạy bộ đếm khi *không có hoa trên sân* VÀ *không ai đang trong trạng thái lửa*), hoa spawn tại bục **trên cao chính giữa** arena, 2 bên có bậc thang đối xứng để leo lên tranh. Hoa tồn tại đến khi bị ăn. Ăn hoa → **trạng thái lửa 5 giây** (không vô hạn như game gốc), hết 5s quay về SMALL. Trong trạng thái lửa bắn được fireball (X / "/").
- **Fireball trúng đối thủ**: hất tung lên + stun ~0.5s (không điều khiển được) — tạo cơ hội nhảy lên đạp đầu, KHÔNG trực tiếp thắng round. Sau stun có 1s miễn nhiễm fireball để chống juggle vô hạn.
- Chỉ số nhân vật dùng nguyên `characterProfileFor()` (Mario.h:47): Mario chạy nhanh (280/175) nhảy 460, Luigi nhảy cao (510) chạy chậm (250/160). Cho phép trùng nhân vật, phân biệt nhãn P1/P2 trên đầu. Không nấm, không item khác.

## Files mới

| File | Vai trò |
|---|---|
| `include/states/PvpCharacterSelectState.h` + `src/states/PvpCharacterSelectState.cpp` | UI chọn nhân vật 2 người (tuần tự P1 → P2) |
| `include/states/PvpPlayState.h` + `src/states/PvpPlayState.cpp` | Gameplay PvP: round FSM, fire director, stun, HUD overlay |
| `levels/pvp_arena.txt` | Arena 20 cột x 11 hàng có bục giữa + 2 dãy bậc thang |
| `tests/PvpArenaTests.cpp`, `tests/PvpStompTests.cpp`, `tests/PvpFlowTests.cpp` | Đăng ký explicit trong CMakeLists.txt |

## Files sửa

1. `src/states/MenuState.cpp` — thêm item menu
2. `include/level/Level.h` + `src/level/Level.cpp` — PvP mode, 2 Mario, spawn hoa, drain sự kiện PvP
3. `include/level/TileMap.h` + `src/level/TileMap.cpp` — LayoutMode PVP_ARENA, marker `'L'`
4. `include/physics/CollisionManager.h` + `.cpp` — nhánh Mario-vs-Mario, fireball-vs-Mario PvP, fireball-vs-fireball
5. `include/entities/Mario.h` + `src/entities/Mario.cpp` — API group filter fixture (additive, mặc định giữ nguyên)
6. Tạo FireBall trong Level — set group theo owner
7. `CMakeLists.txt` + docs (`FILE_STRUCTURE.md`, README điều khiển PvP)

## Chi tiết kỹ thuật

### 1. TileMap — LayoutMode PVP_ARENA
- `enum class LayoutMode { CAMPAIGN, PVP_ARENA }`, mặc định CAMPAIGN — hành vi campaign giữ nguyên 100%.
- `isValidTileSymbol` chấp nhận `'L'` (spawn P2). PVP_ARENA yêu cầu đúng 1 `'M'` + đúng 1 `'L'`, không yêu cầu `F`/`T`/flagpole (bỏ `validateFlagPole` cho mode này).

### 2. `levels/pvp_arena.txt` — 20x11 tile (640x352 < view 640x360 → `Camera::clampCenter` tự ghim giữa, không sửa Camera)
- Nền đất 2 hàng dưới, tường biên `S` 2 bên.
- **Bục giữa**: platform `S` ngang ~4 tile ở hàng cao (row ~4, cột 8–11); hoa spawn trên đỉnh bục (điểm pedestal cột 9–10, hàng ngay trên platform).
- **2 dãy bậc thang đối xứng** kiểu SMB staircase (`S`, mỗi bậc cao hơn 1 tile) leo từ mặt đất 2 bên lên gần bục giữa, chừa 1 khoảng nhảy nhỏ để phải canh thời điểm — cả Mario lẫn Luigi đều leo được bằng đường thang.
- `'M'` spawn trái, `'L'` spawn phải, không quái, không `?` block.

### 3. CollisionManager — các nhánh PvP (đặt TRƯỚC dispatch đơn-Mario trong `resolve()`)
- **Mario vs Mario (stomp)**: xác định attacker bằng đúng 2 test stomp quái hiện có (normal `normal.y > 0.8 && |normal.x| < 0.5` + fallback feet-above-midpoint, CollisionManager.cpp:866-911), **bổ sung điều kiện attacker đang rơi (`vy > 0`)**; bỏ qua nếu loser đang dying/collision-locked. Va đối xứng: chọn người feet cao hơn, bằng nhau không KO. Queue defer (không mutate world trong callback): `s_pendingPvpHits` chứa `{Cause::STOMP|FIREBALL, Mario* attacker, Mario* victim, pos}` + API `consumePendingPvpHits()`.
- **Fireball vs Mario**: hiện fireball và Mario cùng group -1 (Entity.h:27) nên *không bao giờ* chạm nhau. Giải pháp: `Mario::setFixtureCollisionGroup(int16_t)` (member mới, `rebuildFixture()` tiêu thụ, mặc định -1 → campaign không đổi). PvpPlayState set P1 = -1, P2 = -2; fireball kế thừa group của owner khi Level tạo body → fireball **luồn qua chủ, trúng đối thủ** (Box2D: cùng group âm = không chạm, khác = chạm). Nhánh resolve: fireball trúng Mario khác owner → nổ + queue `{FIREBALL, owner, victim, pos}`.
- **Fireball vs fireball** (giờ có thể chạm nhau vì group khác): cả hai nổ, không hiệu ứng gameplay — chặn đạn bằng đạn.
- Contact Mario–Mario vẫn solid (đứng trên đầu nhau — kinh điển); `loseLife()` tự chuyển fixture loser thành sensor.

### 4. Level — PvP mode
- Thêm `m_mario2` (nullable), `m_pvpMode`, `getMario2()`, `loadPvpArena(path, CharacterType p1, CharacterType p2)`: setup world/ContactListener/theme dùng chung helper tách từ `loadFromFile` (contract cũ giữ nguyên), spawn P1 tại `'M'`, P2 tại `'L'`, `setCharacterType` từng bên, set collision group -1/-2.
- `update(dt)` pvpMode: cả 2 Mario qua `preparePhysics → PhysicsEngine::update → refreshGroundedState → update`; drain `s_pendingPvpHits`: **STOMP** → `loseLife(loser)` + bounce winner 300px/s + camera shake; **FIREBALL** → forward list cho PvpPlayState qua `consumePvpFireballHits()`. Guard tường minh bỏ flag sequence, pipe warp, cheep generators; mở rộng `checkItemCollisions` cho cả 2 Mario. Drain popup cả 2 bên.
- `spawnPvpFireFlower(const sf::Vector2f&)`: dùng constructor sẵn `FireFlower(position, world)` (FireFlower.h:16), thêm vào `m_entities`, trả raw ptr để PvpPlayState theo dõi; `removePvpFireFlower()` cho reset round.
- `requestFireBallShot(Mario&)` owner-aware sẵn (giới hạn 2 fireball chung + cooldown 0.25s — giữ nguyên).

### 5. PvpPlayState — state độc lập (không kế thừa PlayState, không đụng GameProgress locked)
- **Round FSM**: `ROUND_INTRO` ("ROUND 1 — READY… GO!" ~1.5s, khóa input) → `FIGHT` → khi có KO: `KO_SETTLE` (chờ death anim ~1.2s) → `ROUND_END` banner → wins==2 ? `MATCH_END` (winner + "ENTER: REMATCH / ESC: MENU") : reset cả 2 bằng `Mario::respawn(spawn)` (có sẵn, chưa dùng) → round mới.
- **Fire director** (chạy trong FIGHT): bộ đếm random 6–12s (uniform, `<random>`) chỉ tick khi `!flowerOnField && !anyInFireState`; về 0 → `Level::spawnPvpFireFlower(pedestalPos)` + sound cue. Theo dõi hoa bằng ptr (khi `isPendingDestroy()` = đã ăn). Ăn hoa (detect qua `getMarioState()` trở thành `FIRE_SMALL`) → `fireTimer[player] = 5s`; hết giờ → `setMarioState(SMALL)` (API public sẵn, Mario.cpp:1257). Round reset: xóa hoa, reset fire state, đếm random mới.
- **Stun/knockback** (policy layer, thuộc PvpPlayState): drain `consumePvpFireballHits()` → bỏ qua nếu victim đang stun/miễn nhiễm/dying; nếu hợp lệ: set velocity victim (hất lên ~-350 + đẩy ngang xa hướng bắn ~150), `stunTimer=0.5s`, sau stun `immunityTimer=1s`. Khi stun: không dispatch input của player đó (zero intents). Bị stun VẪN bị đạp đầu bình thường → KO.
- **Input — 2 `InputHandler`** (Command pattern sẵn có): P1 = `A/D` + `W` nhảy + `LShift` chạy + `X` bắn; P2 = `←/→` + `↑` nhảy + `RShift` chạy + `/` bắn. Short-hop release poll riêng mỗi bên (pattern PlayState.cpp:252-256). Space không bind. Esc push `PauseState` (verify generic khi implement, nếu dính PlayState thì làm overlay riêng).
- **HUD tự vẽ** (`sf::Text` default view, không dùng class `HUD` vì bind 1 `const Mario&`): tên + tỉ số round (P1 ● ● ○ — P2), đồng hồ lửa đếm ngược 5s của người đang giữ (vd "FIRE 3.2s"), nhãn "P1"/"P2" trên đầu nhân vật vẽ trong camera view.

### 6. PvpCharacterSelectState — UI chọn 2 người
- Pattern `CharacterSelectState` hiện có: panel, 2 card avatar Big+Small từ `MarioLuigi.png` + `SpriteFrames_shared.h`, `UIMenuWidget` (mouse hover/click sẵn).
- Chọn tuần tự: "PLAYER 1 — CHOOSE!" (Enter, card gắn badge "P1") → "PLAYER 2 — CHOOSE!" (chọn trùng được) → splash "P1 MARIO vs P2 LUIGI — FIGHT!" → `changeState(PvpPlayState(type1, type2))`. Esc: phase P2 → P1, phase P1 → MenuState.
- Card ghi chỉ số thật: "MARIO — SPEED 280 · JUMP 460" / "LUIGI — SPEED 250 · JUMP 510".

### 7. MenuState
- Thêm `"2 PLAYER VERSUS"` (MenuState.cpp ~198-215) → `changeState(make_unique<PvpCharacterSelectState>())`, guard `m_transitioning`.

### 8. Tests (đăng ký tay trong CMakeLists.txt theo template 21 suite hiện có)
- **PvpArenaTests**: validate PVP_ARENA ('L' OK, không cần flagpole; chặn 2 'M'/0 'L'); load arena → 2 Mario đúng type + đúng profile; layout có bục giữa + 2 thang đối xứng.
- **PvpStompTests**: thế giới Box2D thật — (a) rơi lên đầu → KO queued → drain: loser dying, winner bounce; (b) nhảy từ dưới (vy<0) → không KO; (c) va đối xứng deterministic; (d) fireball của P1 luồn qua P1, trúng P2 → event FIREBALL; (e) fireball vs fireball → cả hai nổ; (f) stun: victim bị hất tung + input lock, hết stun miễn nhiễm 1s, stun vẫn bị đạp KO được.
- **PvpFlowTests**: FSM round/wins/rematch; fire director (không spawn khi hoa trên sân hoặc ai đang FIRE; spawn sau countdown; hết 5s về SMALL); PvpCharacterSelectState chọn tuần tự → transition đúng.

## Rủi ro & nguyên tắc
- **Locked contracts** (IGameState, GameProgress, LevelCatalog): chỉ *thêm* state mới; arena load trực tiếp bằng path không qua LevelCatalog; flow single-player (PlayState/HUD/campaign) không đổi hành vi.
- `setFixtureCollisionGroup` mặc định -1 nên campaign/CO-OP tương lai không bị ảnh hưởng; fixture rebuild (khi đổi state) vẫn giữ group vì group nằm trong member Mario.
- Luigi sprite trạng thái lửa: campaign đã cho Luigi ăn hoa lửa (setupAnimationsForState xử lý sẵn) — kế thừa, chỉ cần smoke test hình ảnh.
- CMake auto-glob `src/*.cpp`; test executables đăng ký tay.

## Thứ tự triển khai
1. TileMap PVP_ARENA + file arena + PvpArenaTests
2. Mario collision-group API + CollisionManager (stomp PvP, fireball-vs-Mario, fireball-vs-fireball) + PvpStompTests — core mechanic trước
3. Level pvp mode (2 Mario, loadPvpArena, drain hits, spawn hoa)
4. PvpPlayState (round FSM, fire director 5s, stun/immunity, input 2 tay, HUD) + PvpFlowTests
5. PvpCharacterSelectState + item menu + tests còn lại
6. CMakeLists, FILE_STRUCTURE.md, README; build mingw-debug + full ctest; smoke test 2 người thật trên bàn phím

*Ghi chú AGENTS.md: `agent_docs/project_progress.md`/`latest_session_work.md` chỉ cập nhật trong deployment state hoặc khi bạn yêu cầu — bỏ qua trừ khi bạn muốn.*
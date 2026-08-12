# Sprint 6 — Full Game Loop và đóng backlog Sprint 1–6

**Thời gian:** 02/08/2026–08/08/2026  
**Nguồn đối chiếu:** `WEEKLY_PLAN.md`, `ROLES.md` và kết quả audit toàn bộ codebase ngày 05/08/2026.  
**Mục tiêu:** Menu → Level 1 → Level 2 → Level 3 → Win hoạt động liên tục; death, GameOver, retry, pause, save, power-up, audio và HUD hoạt động; toàn bộ lỗi đã biết từ Sprint 1–6 được xử lý.

> Sprint 6 sử dụng chiến lược **hoàn tất toàn bộ cam kết**. Task chưa đạt Definition of Done phải giữ trạng thái `BLOCKED` hoặc `NOT DONE`; không được âm thầm chuyển sang Sprint 7 dưới tên “polish”.

## 1. Quy tắc theo dõi

### Trạng thái task

- `TODO`: chưa bắt đầu.
- `DOING`: đang thực hiện; mỗi task chỉ có một người chịu trách nhiệm chính.
- `BLOCKED`: bị chặn; phải ghi rõ blocker, thời điểm phát hiện và người cần phản hồi.
- `REVIEW`: implementation đã xong, đang chờ review hoặc verification.
- `DONE`: đã merge, test pass và đạt đầy đủ Definition of Done.
- `REOPENED`: bug tái hiện lại sau khi từng được đánh dấu `DONE`.

### Nội dung bắt buộc của Pull Request

- Task ID và owner.
- Deliverable Sprint 1–6 hoặc bug mà PR xử lý.
- Root cause, không chỉ mô tả triệu chứng.
- Danh sách module bị ảnh hưởng.
- Các bước tái hiện trước khi sửa.
- Cách kiểm tra sau khi sửa.
- Automated test được thêm; nếu không thể tự động hóa phải ghi lý do và manual checklist.
- Screenshot/video nếu thay đổi camera, render, HUD, state UI hoặc level data.

## 2. Definition of Done của Sprint 6

Sprint 6 chỉ được xem là hoàn thành khi thỏa tất cả điều kiện:

- Debug và Release build thành công trên clean build directory.
- Production target không còn compiler warning.
- CTest pass 100%.
- New Game bắt đầu từ Level 1, không bắt đầu từ test Level 0.
- Có đủ Level 1, Level 2 và Level 3, mỗi level load được và có thể hoàn thành.
- Chơi liên tục từ Menu đến Win không crash.
- Score, coin và lives không reset khi chuyển level.
- Pause/resume không làm mất music, timer hoặc input state.
- Fire Mario bắn được FireBall trong game chính.
- Mushroom, FireFlower, Star và QuestionBlock hoạt động đúng.
- GameOver và Win hiển thị final score/high score.
- High score và volume được lưu, load lại sau khi khởi động game.
- Không còn bug P0/P1 đã biết từ audit Sprint 1–6.
- README, cấu trúc file, class diagram và asset documentation không mô tả sai implementation.

## 3. Interface phải khóa trong Sprint 6

### Progress và level catalog

```cpp
struct GameProgress {
    int currentLevel = 1;
    int score = 0;
    int coins = 0;
    int lives = 3;
    MarioState power = MarioState::SMALL;
};

struct LevelDefinition {
    int number;
    std::string filePath;
    std::string worldLabel;
    LevelTheme theme;
    MusicId music;
};
```

### Save data

```cpp
struct SaveData {
    int version = 1;
    int highScore = 0;
    int highestUnlockedLevel = 1;
    float soundVolume = 80.0f;
    float musicVolume = 70.0f;
};
```

### State lifecycle

```cpp
virtual void onEnter();
virtual void onExit();
virtual void onPause();
virtual void onResume();
```

### Input và Command

- `InputState` phân biệt `Pressed`, `Held`, `Released`.
- Bổ sung `RunCommand` và `ShootCommand`.
- Loại `undo()` khỏi `ICommand` vì runtime không có undo stack.

### Enemy defeat

```cpp
enum class DefeatCause {
    STOMP,
    SHELL,
    FIREBALL,
    STAR,
    PIT
};
```

### Ownership

- `EntityFactory` trả `std::unique_ptr<Entity>`.
- Raw pointer chỉ được dùng làm non-owning reference.
- Box2D body không được destroy trực tiếp trong contact callback.

## 4. TV1 — Dương — Architect, State và Integration

### A. Traceability và điều phối

- [ ] **S6-TV1-01 — Tạo Sprint 1–6 audit tracker** — Liệt kê từng deliverable trong `WEEKLY_PLAN.md`, trạng thái thực tế, task xử lý và bằng chứng kiểm thử. **DoD:** Không có deliverable nào thiếu trạng thái hoặc owner. **Ước lượng:** 45 phút.
- [ ] **S6-TV1-02 — Tạo bug register P0/P1/P2** — Gộp bug camera, progress, state, physics, enemy, item, audio, save, build và documentation. **DoD:** Mỗi bug có severity, reproduction, owner và task ID. **Ước lượng:** 45 phút.
- [ ] **S6-TV1-03 — Khóa public interfaces** — Chốt `GameProgress`, `LevelDefinition`, `SaveData`, lifecycle và event contract với cả nhóm. **DoD:** Thay đổi interface sau khi khóa phải được TV1 approve. **Ước lượng:** 45 phút.
- [ ] **S6-TV1-04 — Lập dependency board** — Ghi rõ task nào chặn task nào giữa năm module. **DoD:** Các task cross-module đều có producer/consumer và thời hạn bàn giao. **Ước lượng:** 30 phút.

### B. Level progression và GameProgress

- [ ] **S6-TV1-05 — Tạo LevelCatalog** — Khai báo tập trung ba release level, world label, theme và music. **DoD:** Không còn path, `MAX_LEVELS` hoặc level number hard-code rải rác. **Phụ thuộc:** TV4 xác nhận metadata. **Ước lượng:** 60 phút.
- [ ] **S6-TV1-06 — Loại Level 0 khỏi release flow** — `PlayState` bắt đầu ở Level 1. **DoD:** New Game không thể load Level 0. **Phụ thuộc:** S6-TV1-05. **Ước lượng:** 20 phút.
- [ ] **S6-TV1-07 — Chuẩn hóa level number one-based** — Sửa khác biệt giữa file number, current level và HUD. **DoD:** World 1-1/1-2/1-3 hiển thị đúng. **Ước lượng:** 30 phút.
- [ ] **S6-TV1-08 — Tạo GameProgress độc lập với Level** — Progress sống ở game session/PlayState. **DoD:** Hủy `Level` không hủy score, coin, lives. **Ước lượng:** 60 phút.
- [ ] **S6-TV1-09 — Snapshot progress trước transition** — Chụp score, coin, lives và power đúng một lần. **DoD:** Không đọc dữ liệu từ Mario đã bị hủy. **Phụ thuộc:** S6-TV1-08. **Ước lượng:** 30 phút.
- [ ] **S6-TV1-10 — Restore progress sau load** — Apply progress vào Mario/HUD mới. **DoD:** Chuyển level không reset dữ liệu ngoài policy. **Phụ thuộc:** S6-TV1-09. **Ước lượng:** 45 phút.
- [ ] **S6-TV1-11 — Kiểm tra `loadFromFile()` result** — Không bỏ qua lỗi load trong constructor/reload. **DoD:** File level sai đưa người chơi về Menu/error state có message, không crash. **Ước lượng:** 45 phút.
- [ ] **S6-TV1-12 — Viết transition transaction** — Freeze → snapshot → fade → load → restore → `LEVEL_STARTED`. **DoD:** Không update hai level hoặc hai Mario trong cùng transition. **Phụ thuộc:** S6-TV1-09–11 và TV2 fade. **Ước lượng:** 90 phút.
- [ ] **S6-TV1-13 — Ngăn death/completion race** — Chỉ một terminal result được commit trong một frame. **DoD:** Chết khi chạm flag không thể vừa GameOver vừa sang level. **Ước lượng:** 45 phút.
- [ ] **S6-TV1-14 — Hoàn thiện final Win transition** — Level 3 completion chuyển Win sau flag sequence. **DoD:** Win chỉ push một lần, final score đúng. **Phụ thuộc:** TV4 Level 3. **Ước lượng:** 30 phút.

### C. State machine

- [ ] **S6-TV1-15 — Bổ sung `onPause()`/`onResume()`** — Hoàn thiện lifecycle `IGameState`. **DoD:** Push Pause gọi `PlayState::onPause`; pop gọi `onResume`. **Ước lượng:** 60 phút.
- [ ] **S6-TV1-16 — Deferred state operations** — Queue push/pop/change đến safe point cuối update. **DoD:** Không hủy state đang nằm trên call stack. **Ước lượng:** 75 phút.
- [ ] **S6-TV1-17 — Sửa Pause lifecycle** — Gameplay, timer, animation và input dừng; resume phục hồi đúng. **DoD:** Pause/resume 20 lần không sai state. **Phụ thuộc:** TV5 music resume. **Ước lượng:** 45 phút.
- [ ] **S6-TV1-18 — Chuẩn hóa death và GameOver event** — `PLAYER_DIED` phát ở mọi lần chết; GameOver là kết quả riêng. **DoD:** Một death chỉ phát một event, life trừ một lần. **Phụ thuộc:** TV3 death flow. **Ước lượng:** 30 phút.
- [ ] **S6-TV1-19 — Kết nối SaveManager vào states** — TV4 sở hữu SaveManager; TV1 gọi load ở Menu và save ở Win/GameOver. **DoD:** Không có implementation SaveManager trùng. **Phụ thuộc:** TV4 SaveManager. **Ước lượng:** 45 phút.

### D. Patterns và ownership

- [ ] **S6-TV1-20 — Sửa EventBus observer lifetime** — Trước callback từ snapshot phải xác nhận observer còn đăng ký. **DoD:** Observer bị callback khác hủy không bị gọi tiếp. **Ước lượng:** 60 phút.
- [ ] **S6-TV1-21 — Audit unsubscribe trong destructor** — Kiểm tra Menu, Play, HUD, Sound và các state observers. **DoD:** Chuyển state lặp không tăng số callback. **Ước lượng:** 45 phút.
- [ ] **S6-TV1-22 — Đổi EntityFactory sang `unique_ptr`** — Loại raw owning pointer. **DoD:** Không còn `new` raw ở factory call sites. **Phụ thuộc:** TV4 entity constructors. **Ước lượng:** 45 phút.
- [ ] **S6-TV1-23 — Chuẩn hóa tên Simple Factory** — Không gọi sai thành canonical Factory Method. **DoD:** Code comments và tài liệu nhất quán. **Ước lượng:** 20 phút.
- [ ] **S6-TV1-24 — Xóa `ICommand::undo()`** — Xóa các implementation undo rỗng. **DoD:** Production build và input tests pass. **Phụ thuộc:** TV5 InputHandler. **Ước lượng:** 30 phút.
- [ ] **S6-TV1-25 — Xóa dependency thừa của PauseCommand** — Bỏ member/constructor parameter không dùng. **DoD:** Command chỉ nhận dependency cần thiết. **Ước lượng:** 15 phút.

### E. Build và tài liệu Sprint 1–6

- [ ] **S6-TV1-26 — Loại demo khỏi production target** — Không để glob kéo `TV3Demo.cpp` vào executable. **DoD:** Không còn deprecated warning. **Phụ thuộc:** TV3 chuyển scenario sang tests. **Ước lượng:** 30 phút.
- [ ] **S6-TV1-27 — Cứng hóa SFML bootstrap** — Không tự xóa SFML folder; bật TLS verification; kiểm tra hash archive. **DoD:** Configure failure không phá dữ liệu. **Ước lượng:** 60 phút.
- [ ] **S6-TV1-28 — Chuẩn hóa CMake targets/presets** — Có Debug, Release, Tests; executable tên rõ ràng. **DoD:** Ba preset chạy từ clean directory. **Ước lượng:** 60 phút.
- [ ] **S6-TV1-29 — Sửa asset copy rule** — Không copy toàn bộ asset ở mọi invocation. **DoD:** Incremental build không copy thừa. **Ước lượng:** 30 phút.
- [ ] **S6-TV1-30 — Sửa README** — Cập nhật số sprint, build/run/test, controls và links. **DoD:** Không còn link tới file không tồn tại. **Ước lượng:** 45 phút.
- [ ] **S6-TV1-31 — Sửa FILE_STRUCTURE** — Phân biệt file thật, archive và planned. **DoD:** Tên file, owner, asset dimensions đúng repository. **Ước lượng:** 45 phút.
- [ ] **S6-TV1-32 — Cập nhật class diagram** — Phản ánh ownership/state/factory/EventBus/Box2D hiện tại. **DoD:** Không còn API draft Week 1 đã sai. **Ước lượng:** 75 phút.
- [ ] **S6-TV1-33 — Archive Sprint 5 error plan cũ** — Thêm legacy banner, bỏ đường dẫn tuyệt đối. **DoD:** Tài liệu cũ không được xem là instruction hiện hành. **Ước lượng:** 20 phút.
- [ ] **S6-TV1-34 — Dọn Git metadata** — Đánh dấu FLAC/PDF binary; xóa `.gitkeep` trong thư mục không rỗng. **DoD:** Chỉ giữ placeholder cần thiết. **Ước lượng:** 30 phút.
- [ ] **S6-TV1-35 — Integration review cuối mỗi ngày** — Merge, build, test, resolve conflict. **DoD:** Integration branch không hỏng qua ngày tiếp theo. **Ước lượng:** 45 phút/ngày.

## 5. TV2 — Nhật — Engine, Render và UI States

### A. Camera và display

- [ ] **S6-TV2-01 — Tạo DisplayConfig** — Tập trung window 1280×720 và logical world 640×360. **DoD:** Không còn cấu hình window/camera trùng. **Ước lượng:** 30 phút.
- [ ] **S6-TV2-02 — Sửa world camera thành 640×360** — **DoD:** Map lấp khung hợp lý, Mario và tile không còn quá nhỏ. **Phụ thuộc:** S6-TV2-01. **Ước lượng:** 30 phút.
- [ ] **S6-TV2-03 — Giữ HUD ở default view** — **DoD:** HUD không di chuyển hoặc scale theo camera world. **Ước lượng:** 20 phút.
- [ ] **S6-TV2-04 — Thêm camera X dead-zone** — **DoD:** Camera không bám từng pixel Mario. **Ước lượng:** 45 phút.
- [ ] **S6-TV2-05 — Khóa camera Y cho overworld** — **DoD:** Mario nhảy không làm toàn màn hình nảy. **Ước lượng:** 30 phút.
- [ ] **S6-TV2-06 — Hỗ trợ Y cho map cao** — **DoD:** Map cao hơn viewport vẫn theo và clamp đúng. **Ước lượng:** 30 phút.
- [ ] **S6-TV2-07 — Clamp camera đầu/cuối level** — **DoD:** Không nhìn ra ngoài world bounds. **Ước lượng:** 45 phút.
- [ ] **S6-TV2-08 — Clamp sau camera shake** — **DoD:** Shake không lộ vùng ngoài map. **Ước lượng:** 20 phút.
- [ ] **S6-TV2-09 — Kết nối shake vào damage/death** — **DoD:** Không trigger hai lần từ cùng event. **Phụ thuộc:** TV3 event. **Ước lượng:** 30 phút.
- [ ] **S6-TV2-10 — Thêm letterbox 16:9** — **DoD:** Resize/fullscreen không kéo giãn pixel art. **Ước lượng:** 60 phút.
- [ ] **S6-TV2-11 — Bảo đảm integer scaling** — Tắt smoothing và dùng 2× ở cửa sổ mặc định. **DoD:** Pixel không blur. **Ước lượng:** 30 phút.

### B. Game loop và render order

- [ ] **S6-TV2-12 — Sửa close-event ordering** — **DoD:** Không update/render frame sau khi window đóng. **Ước lượng:** 20 phút.
- [ ] **S6-TV2-13 — Clamp delta time** — **DoD:** Alt-tab không gây physics spike. **Phụ thuộc:** TV3 max substeps. **Ước lượng:** 25 phút.
- [ ] **S6-TV2-14 — Xác minh 60 FPS cap** — **DoD:** Frame cap khai báo một chỗ, không cấu hình lặp. **Ước lượng:** 20 phút.
- [ ] **S6-TV2-15 — Chuẩn hóa render layers** — Background → tiles → items/entities → Mario → foreground → HUD → transition. **DoD:** Không còn z-order sai đã biết. **Ước lượng:** 45 phút.

### C. UI states và animations

- [ ] **S6-TV2-16 — Tạo shared UI layout helper** — **DoD:** Menu/Pause/GameOver/Win dùng chung anchors. **Ước lượng:** 45 phút.
- [ ] **S6-TV2-17 — Sửa MenuState** — Keyboard/mouse, Start và high score. **DoD:** High score lấy từ SaveManager, không hard-code. **Ước lượng:** 60 phút.
- [ ] **S6-TV2-18 — Hoàn thiện PauseState** — Overlay, Resume, Quit to Menu. **DoD:** World không update phía sau. **Ước lượng:** 60 phút.
- [ ] **S6-TV2-19 — Hoàn thiện GameOverState** — Final score, high score, Retry, Menu. **DoD:** Keyboard/mouse đều hoạt động. **Ước lượng:** 60 phút.
- [ ] **S6-TV2-20 — Hoàn thiện WinState** — Final score/high score và Return to Menu. **DoD:** State không bị push nhiều lần. **Ước lượng:** 60 phút.
- [ ] **S6-TV2-21 — Sửa transition fade** — Dùng delta time và freeze gameplay. **DoD:** Fade không phụ thuộc FPS. **Ước lượng:** 45 phút.
- [ ] **S6-TV2-22 — Hoàn thiện death/spawn clips** — **DoD:** Clip tồn tại, có completion signal và được TV3 gọi. **Ước lượng:** 60 phút.
- [ ] **S6-TV2-23 — Hoàn thiện grow/shrink transition** — **DoD:** Không pop sprite hoặc lệch chân. **Phụ thuộc:** TV3 fixture policy. **Ước lượng:** 45 phút.

### D. Resource và visual QA

- [ ] **S6-TV2-24 — Loại absolute font fallback** — **DoD:** Chỉ dùng packaged font; load failure có log rõ. **Ước lượng:** 15 phút.
- [ ] **S6-TV2-25 — Xóa `TextureManager::deleteTexture()`** — **DoD:** Không còn public API có thể tạo dangling texture. **Ước lượng:** 20 phút.
- [ ] **S6-TV2-26 — Audit AnimationSystem API** — `stop/pause/resume` phải có caller hoặc bị loại. **DoD:** Không còn public method giả. **Ước lượng:** 30 phút.
- [ ] **S6-TV2-27 — Background theo LevelTheme** — **DoD:** Overworld, underground và castle có visual riêng. **Phụ thuộc:** TV1 catalog, TV5 assets. **Ước lượng:** 60 phút.
- [ ] **S6-TV2-28 — Resolution test matrix** — 1280×720, 1600×900, 1920×1080, 4:3. **DoD:** Không stretch/cắt HUD/sai click target. **Ước lượng:** 60 phút.

## 6. TV3 — Bảo — Mario, Physics và Collision

### A. Physics loop và movement

- [ ] **S6-TV3-01 — Chuyển physics accumulator thành instance** — **DoD:** Mỗi world có accumulator độc lập. **Ước lượng:** 45 phút.
- [ ] **S6-TV3-02 — Thêm maximum substeps** — **DoD:** Frame spike không catch-up vô hạn. **Ước lượng:** 30 phút.
- [ ] **S6-TV3-03 — Kết nối run state** — **DoD:** Shift tăng acceleration/max speed; thả về walk. **Phụ thuộc:** TV5 RunCommand. **Ước lượng:** 45 phút.
- [ ] **S6-TV3-04 — Nhận jump release** — **DoD:** Mario nhận đúng frame Space release. **Phụ thuộc:** TV5 input state. **Ước lượng:** 30 phút.
- [ ] **S6-TV3-05 — Variable jump** — **DoD:** Tap tạo short hop, giữ tạo full jump. **Ước lượng:** 45 phút.
- [ ] **S6-TV3-06 — Chặn double jump** — **DoD:** Jump chỉ bắt đầu khi grounded/policy hợp lệ. **Ước lượng:** 30 phút.
- [ ] **S6-TV3-07 — Thay pit threshold hard-code** — **DoD:** Pit death lấy theo Level bounds. **Phụ thuộc:** TV4 bounds. **Ước lượng:** 25 phút.
- [ ] **S6-TV3-08 — Chặn mép trái world** — **DoD:** Mario không rung hoặc thoát map. **Ước lượng:** 25 phút.

### B. Mario power và death

- [ ] **S6-TV3-09 — Chuẩn hóa `MarioState`** — Dùng SMALL/SUPER/FIRE; BIG trong docs tương ứng SUPER. **DoD:** Không tồn tại enum trùng. **Ước lượng:** 20 phút.
- [ ] **S6-TV3-10 — Giữ chân khi grow/shrink** — **DoD:** Đổi hitbox không nhấc/lún Mario. **Ước lượng:** 60 phút.
- [ ] **S6-TV3-11 — Growth clearance check** — **DoD:** Power-up dưới trần thấp không xuyên tile/kẹt. **Ước lượng:** 60 phút.
- [ ] **S6-TV3-12 — Sửa damage chain** — FIRE → SUPER → SMALL → death. **DoD:** Một contact chỉ gây một bước damage. **Ước lượng:** 45 phút.
- [ ] **S6-TV3-13 — Tách damage grace và Star timer** — **DoD:** Damage grace không giết enemy; Star có thể giết. **Ước lượng:** 60 phút.
- [ ] **S6-TV3-14 — Damage blink timer** — **DoD:** Visual và miễn damage cùng duration. **Phụ thuộc:** TV2 render hook. **Ước lượng:** 30 phút.
- [ ] **S6-TV3-15 — Death sequence** — Khóa input, animation/event, trừ life. **DoD:** Không trừ nhiều lives. **Ước lượng:** 75 phút.
- [ ] **S6-TV3-16 — Respawn sequence** — Reset body, velocity, animation, input và power policy. **DoD:** Respawn deterministic. **Ước lượng:** 45 phút.

### C. FireBall

- [ ] **S6-TV3-17 — Thêm Mario shoot API** — **DoD:** Chỉ FIRE state bắn được. **Ước lượng:** 30 phút.
- [ ] **S6-TV3-18 — Cooldown 250 ms** — **DoD:** Giữ phím không spawn mỗi frame. **Ước lượng:** 20 phút.
- [ ] **S6-TV3-19 — Giới hạn hai FireBall active** — **DoD:** Projectile inactive không còn được đếm. **Ước lượng:** 20 phút.
- [ ] **S6-TV3-20 — Spawn offset theo hướng** — **DoD:** FireBall không overlap Mario. **Ước lượng:** 30 phút.
- [ ] **S6-TV3-21 — Collision filter owner** — **DoD:** FireBall không tự hủy vì chạm Mario. **Ước lượng:** 30 phút.
- [ ] **S6-TV3-22 — Bounce và wall collision** — **DoD:** Bounce ground, hủy khi chạm wall. **Ước lượng:** 45 phút.
- [ ] **S6-TV3-23 — Out-of-bounds cleanup** — **DoD:** Projectile không sống vô hạn ngoài map. **Ước lượng:** 20 phút.
- [ ] **S6-TV3-24 — FireBall render hook** — **DoD:** Projectile trong game chính có sprite/animation. **Phụ thuộc:** TV2. **Ước lượng:** 30 phút.

### D. Collision và entity death

- [ ] **S6-TV3-25 — Tách contact normalization** — **DoD:** Kết quả không phụ thuộc fixture A/B order. **Ước lượng:** 45 phút.
- [ ] **S6-TV3-26 — Tách Mario–Enemy handler** — **DoD:** Stomp và side hit có test riêng. **Ước lượng:** 45 phút.
- [ ] **S6-TV3-27 — Tách Projectile–Enemy handler** — **DoD:** Dùng `DefeatCause::FIREBALL`. **Ước lượng:** 30 phút.
- [ ] **S6-TV3-28 — Tách Shell–Enemy handler** — **DoD:** Không multi-hit cùng frame. **Phụ thuộc:** TV4 shell. **Ước lượng:** 30 phút.
- [ ] **S6-TV3-29 — Deferred body destruction** — **DoD:** Không destroy body khi world locked/contact callback. **Ước lượng:** 60 phút.
- [ ] **S6-TV3-30 — Xóa invisible colliders** — **DoD:** Enemy chết không chặn Mario. **Ước lượng:** 30 phút.
- [ ] **S6-TV3-31 — Score theo DefeatCause** — **DoD:** Mỗi kill phát đúng một score event. **Phụ thuộc:** TV5 scoring. **Ước lượng:** 30 phút.

### E. Cleanup và tests

- [ ] **S6-TV3-32 — Xóa `Mario::handleInput()` deprecated** — **DoD:** Không còn caller. **Ước lượng:** 20 phút.
- [ ] **S6-TV3-33 — Xóa dead Mario members** — Kiểm tra `m_moveSpeed`, `m_wasJumpPressed` và state thừa. **DoD:** Movement tests vẫn pass. **Ước lượng:** 20 phút.
- [ ] **S6-TV3-34 — Một source of truth cho bounding box** — **DoD:** Không vừa cache vừa tính lại không nhất quán. **Ước lượng:** 30 phút.
- [ ] **S6-TV3-35 — Chuyển demo scenario sang tests** — **DoD:** Có thể xóa demo mà không mất coverage. **Ước lượng:** 45 phút.
- [ ] **S6-TV3-36 — Mario physics test suite** — Walk/run/jump/short-hop/growth/damage/death. **DoD:** Deterministic trên clean build. **Ước lượng:** 90 phút.
- [ ] **S6-TV3-37 — Collision regression suite** — Stomp, side, shell, FireBall, Star. **DoD:** Mỗi bug collision đã biết có test. **Ước lượng:** 90 phút.

## 7. TV4 — Vy — Level, Enemy và SaveManager

### A. Level format và TileMap

- [ ] **S6-TV4-01 — Chuyển Level 0 thành test fixture** — **DoD:** Không nằm trong release catalog. **Ước lượng:** 15 phút.
- [ ] **S6-TV4-02 — Validate đúng một Mario spawn** — **DoD:** Thiếu/trùng `M` load fail. **Ước lượng:** 20 phút.
- [ ] **S6-TV4-03 — Validate đúng một finish** — **DoD:** Thiếu/trùng `F` load fail. **Ước lượng:** 20 phút.
- [ ] **S6-TV4-04 — Validate flag pole** — **DoD:** `F` kết nối với `|` hợp lệ. **Ước lượng:** 20 phút.
- [ ] **S6-TV4-05 — Validate row width/symbol** — **DoD:** Error có row/column. **Ước lượng:** 30 phút.
- [ ] **S6-TV4-06 — Sửa ground/breakable semantics** — `1` không break; chỉ `B` break. **DoD:** Có regression test. **Ước lượng:** 30 phút.
- [ ] **S6-TV4-07 — Pending tile hit thành member** — **DoD:** Không còn static state giữa level. **Ước lượng:** 30 phút.
- [ ] **S6-TV4-08 — Xóa U/O path trùng trong TileMap** — **DoD:** QuestionBlock là source of truth. **Phụ thuộc:** TV5. **Ước lượng:** 25 phút.
- [ ] **S6-TV4-09 — Chuyển TileMap sang tileset đúng** — **DoD:** Ground/brick/question/used/pipe có frame riêng. **Ước lượng:** 60 phút.
- [ ] **S6-TV4-10 — Render flag/pole đúng** — **DoD:** Không dùng brick/empty placeholder. **Ước lượng:** 45 phút.
- [ ] **S6-TV4-11 — Full-height finish trigger** — **DoD:** Chạm vùng hợp lệ bất kỳ trên pole đều finish. **Ước lượng:** 45 phút.
- [ ] **S6-TV4-12 — Chặn finish trigger lặp** — **DoD:** `LEVEL_COMPLETED` chỉ phát một lần. **Ước lượng:** 25 phút.

### B. Ba release levels

- [ ] **S6-TV4-13 — Hoàn thiện Level 1 tutorial flow** — Movement, jump, coin, QuestionBlock, Mushroom, Goomba, flag. **DoD:** Hoàn thành được không cần damage boost. **Ước lượng:** 90 phút.
- [ ] **S6-TV4-14 — Kiểm tra Level 1 gaps** — **DoD:** Gap trong jump envelope; nơi cần run có chỉ dẫn. **Ước lượng:** 45 phút.
- [ ] **S6-TV4-15 — Hoàn thiện Level 2 underground** — Pipe, Koopa, shell, FireFlower. **DoD:** Theme metadata và completion route hoàn chỉnh. **Ước lượng:** 120 phút.
- [ ] **S6-TV4-16 — Kiểm tra Level 2 spawns** — **DoD:** Koopa không rơi hoặc attack trước khi thấy. **Ước lượng:** 45 phút.
- [ ] **S6-TV4-17 — Tạo Level 3 castle/sky** — Mixed enemies, Star, gauntlet cuối. **DoD:** Load và complete được. **Ước lượng:** 150 phút.
- [ ] **S6-TV4-18 — Kiểm tra Level 3 fairness** — **DoD:** Không frame-perfect jump/blind landing/unavoidable damage. **Ước lượng:** 60 phút.
- [ ] **S6-TV4-19 — Chuẩn hóa coin/power-up distribution** — **DoD:** Mỗi item mechanic xuất hiện có chủ đích. **Ước lượng:** 45 phút.
- [ ] **S6-TV4-20 — Level validator tests** — **DoD:** Invalid width/symbol/spawn/finish/pole đều có test. **Ước lượng:** 60 phút.

### C. Enemy behavior

- [ ] **S6-TV4-21 — Enemy activation radius** — **DoD:** Enemy xa camera chưa update. **Ước lượng:** 45 phút.
- [ ] **S6-TV4-22 — Off-screen update policy** — **DoD:** Không reset enemy sai, không chạy AI toàn map. **Ước lượng:** 45 phút.
- [ ] **S6-TV4-23 — Xác minh Goomba ledge detection** — **DoD:** Không rơi khỏi platform ngoài chủ đích. **Ước lượng:** 30 phút.
- [ ] **S6-TV4-24 — Koopa ledge detection** — **DoD:** Không biến mất trước khi Mario tới. **Ước lượng:** 45 phút.
- [ ] **S6-TV4-25 — Resize shell fixture** — **DoD:** Hitbox khớp shell sprite. **Phụ thuộc:** TV3 deferred fixture changes. **Ước lượng:** 45 phút.
- [ ] **S6-TV4-26 — Walking → shell idle** — **DoD:** Stomp đầu không làm shell chạy ngay. **Ước lượng:** 30 phút.
- [ ] **S6-TV4-27 — Shell idle → moving** — **DoD:** Kick theo hướng Mario. **Ước lượng:** 30 phút.
- [ ] **S6-TV4-28 — Shell wall response** — **DoD:** Đổi hướng, không xuyên wall. **Ước lượng:** 30 phút.
- [ ] **S6-TV4-29 — Shell giết enemy khác** — **DoD:** Dùng shared defeat path, score/event đúng. **Phụ thuộc:** TV3 collision. **Ước lượng:** 45 phút.
- [ ] **S6-TV4-30 — Enemy pit cleanup** — **DoD:** Body/entity được remove an toàn. **Ước lượng:** 25 phút.
- [ ] **S6-TV4-31 — Generic item/entity cleanup** — **DoD:** Mushroom, Star, projectile không tồn tại dưới map. **Ước lượng:** 30 phút.

### D. SaveManager

- [ ] **S6-TV4-32 — Tạo SaveManager version 1** — **DoD:** Có default data khi chưa có save. **Ước lượng:** 60 phút.
- [ ] **S6-TV4-33 — Load high score** — **DoD:** Menu đọc đúng sau restart. **Ước lượng:** 30 phút.
- [ ] **S6-TV4-34 — High score monotonic update** — **DoD:** Score thấp không ghi đè score cao. **Ước lượng:** 20 phút.
- [ ] **S6-TV4-35 — Lưu highest unlocked level** — **DoD:** Hoàn thành màn mở màn kế. **Ước lượng:** 30 phút.
- [ ] **S6-TV4-36 — Lưu audio settings** — **DoD:** SFX/music volume còn đúng sau restart. **Ước lượng:** 30 phút.
- [ ] **S6-TV4-37 — Atomic save** — Ghi temporary rồi replace. **DoD:** Không để file nửa chừng. **Ước lượng:** 45 phút.
- [ ] **S6-TV4-38 — Corrupted save fallback** — **DoD:** Không crash; default data và log rõ. **Ước lượng:** 30 phút.
- [ ] **S6-TV4-39 — SaveManager tests** — Missing, valid, corrupt, version mismatch, lower score. **DoD:** Tất cả pass. **Ước lượng:** 75 phút.
- [ ] **S6-TV4-40 — Năm playthrough mỗi level** — Normal, no-damage, death/respawn, power-up, flag route. **DoD:** Có log kết quả từng run. **Ước lượng:** 150 phút.

## 8. TV5 — Truyền — Input, Items, HUD, Sound và Assets

### A. Input/Command

- [ ] **S6-TV5-01 — Thêm Released state** — **DoD:** Press/Held/Release phân biệt rõ. **Ước lượng:** 45 phút.
- [ ] **S6-TV5-02 — Bind RunCommand** — **DoD:** Shift giữ/chạy, thả/walk. **Ước lượng:** 30 phút.
- [ ] **S6-TV5-03 — Bind ShootCommand** — **DoD:** X phát request, không tự spawn projectile trong InputHandler. **Ước lượng:** 30 phút.
- [ ] **S6-TV5-04 — Truyền jump release** — **DoD:** TV3 nhận đúng release frame. **Ước lượng:** 20 phút.
- [ ] **S6-TV5-05 — Chặn gameplay commands ngoài Play** — Pause/dead/transition không nhận movement/shoot. **DoD:** Không buffer input ngoài ý muốn. **Ước lượng:** 30 phút.
- [ ] **S6-TV5-06 — Input regression tests** — Simultaneous keys, repeat, release, pause. **DoD:** Tất cả pass. **Ước lượng:** 60 phút.

### B. Items và QuestionBlock

- [ ] **S6-TV5-07 — Sửa duplicate power-up event** — **DoD:** Một pickup có một event/SFX/score update. **Ước lượng:** 30 phút.
- [ ] **S6-TV5-08 — Hoàn thiện Mushroom** — SMALL→SUPER; SUPER/FIRE không downgrade. **DoD:** State và animation đúng. **Ước lượng:** 30 phút.
- [ ] **S6-TV5-09 — Hoàn thiện FireFlower** — **DoD:** Small Mario vào Small FIRE, Super Mario vào Super FIRE; cả hai giữ đúng kích thước thân và bắn được. **Phụ thuộc:** TV3 shoot. **Ước lượng:** 30 phút.
- [ ] **S6-TV5-10 — Sửa Star semantics** — **DoD:** Không grow vĩnh viễn; chỉ timed invincibility. **Phụ thuộc:** TV3 timer. **Ước lượng:** 45 phút.
- [ ] **S6-TV5-11 — Adaptive `?` block** — SMALL nhận Mushroom; SUPER/FIRE nhận FireFlower. **DoD:** Content xác định một lần khi hit. **Ước lượng:** 45 phút.
- [ ] **S6-TV5-12 — Hoàn thiện `U` 1-Up block** — **DoD:** Spawn một lần và cộng một life. **Ước lượng:** 30 phút.
- [ ] **S6-TV5-13 — Hoàn thiện `O` Star block** — **DoD:** Spawn một lần; Star movement/collect đúng. **Ước lượng:** 30 phút.
- [ ] **S6-TV5-14 — Item emerge delay** — **DoD:** Không collect xuyên block ngay frame spawn. **Ước lượng:** 45 phút.
- [ ] **S6-TV5-15 — 100 coin = 1 life** — **DoD:** Coin remainder và `ONE_UP` đúng. **Ước lượng:** 30 phút.
- [ ] **S6-TV5-16 — Chuẩn hóa score rules** — Coin +100; power-up +1000; stomp +100; shell/FireBall/Star +200. **DoD:** Một score API/event thống nhất. **Ước lượng:** 45 phút.

### C. HUD

- [ ] **S6-TV5-17 — Level timer** — Countdown từ 400. **DoD:** Không chạy khi pause/transition. **Ước lượng:** 45 phút.
- [ ] **S6-TV5-18 — Time warning** — **DoD:** Warning phát một lần khi qua threshold. **Ước lượng:** 25 phút.
- [ ] **S6-TV5-19 — Timeout death** — **DoD:** Timer 0 gây death đúng một lần. **Phụ thuộc:** TV3 death. **Ước lượng:** 30 phút.
- [ ] **S6-TV5-20 — Current level indicator** — **DoD:** World 1-1/1-2/1-3 đúng. **Ước lượng:** 20 phút.
- [ ] **S6-TV5-21 — Power indicator** — **DoD:** SMALL/SUPER/FIRE/STAR hiển thị rõ. **Ước lượng:** 30 phút.
- [ ] **S6-TV5-22 — Score formatting** — **DoD:** Zero-padding nhất quán, không overflow. **Ước lượng:** 20 phút.
- [ ] **S6-TV5-23 — Lives/coins refresh** — **DoD:** Cập nhật sau collect/death/respawn/transition. **Ước lượng:** 30 phút.

### D. Sound và music

- [ ] **S6-TV5-24 — Tạo MusicId** — **DoD:** Không truyền path music rải rác. **Ước lượng:** 30 phút.
- [ ] **S6-TV5-25 — Overworld music mapping** — **DoD:** Level 1 loop đúng. **Ước lượng:** 20 phút.
- [ ] **S6-TV5-26 — Underground music mapping** — **DoD:** Level 2 dùng track riêng. **Ước lượng:** 20 phút.
- [ ] **S6-TV5-27 — Castle music mapping** — **DoD:** Level 3 dùng track riêng. **Ước lượng:** 20 phút.
- [ ] **S6-TV5-28 — Star music override** — **DoD:** Hết Star trở lại đúng level track. **Ước lượng:** 45 phút.
- [ ] **S6-TV5-29 — Death/GameOver/Win music** — **DoD:** Track không overlap sai. **Ước lượng:** 45 phút.
- [ ] **S6-TV5-30 — Pause/resume music** — **DoD:** Pause 20 lần không mất music. **Ước lượng:** 30 phút.
- [ ] **S6-TV5-31 — FireBall SFX** — **DoD:** Mỗi shot được tạo phát một lần. **Ước lượng:** 20 phút.
- [ ] **S6-TV5-32 — Shell/kick SFX** — **DoD:** Không lặp do persistent contact. **Ước lượng:** 20 phút.
- [ ] **S6-TV5-33 — Bump/brick/item/1-Up/flag SFX** — **DoD:** Có bảng event-to-sound mapping. **Ước lượng:** 45 phút.
- [ ] **S6-TV5-34 — SFX voice pool** — Tối thiểu bốn voices cho sound lặp nhanh. **DoD:** Coin/stomp liên tục không cắt nhau. **Ước lượng:** 60 phút.
- [ ] **S6-TV5-35 — Clamp volume** — **DoD:** API luôn trả `[0,100]`. **Ước lượng:** 20 phút.
- [ ] **S6-TV5-36 — Volume controls trong Pause** — **DoD:** SFX/music chỉnh riêng và phản hồi ngay. **Phụ thuộc:** TV2 Pause UI, TV4 SaveManager. **Ước lượng:** 45 phút.

### E. Asset cleanup

- [ ] **S6-TV5-37 — Asset usage manifest** — Gắn nhãn Runtime/Reference/Future/Remove cho mọi asset. **DoD:** Không có asset không rõ mục đích. **Ước lượng:** 60 phút.
- [ ] **S6-TV5-38 — Xóa `gameoverunused.wav`** — **DoD:** Không còn reference. **Ước lượng:** 10 phút.
- [ ] **S6-TV5-39 — Chuyển source sprite sheet khỏi runtime assets** — **DoD:** Source/reference không bị package như runtime. **Ước lượng:** 20 phút.
- [ ] **S6-TV5-40 — Đổi tên tài liệu crop sprite** — **DoD:** Tên mới phản ánh đúng nội dung; link được cập nhật. **Ước lượng:** 15 phút.
- [ ] **S6-TV5-41 — Cập nhật ASSETS_LIST** — **DoD:** Dimensions/path/usage đúng file thật. **Ước lượng:** 45 phút.
- [ ] **S6-TV5-42 — Hợp nhất `asset.md` chưa track** — **DoD:** Không mất ghi chú hữu ích, chỉ còn một source of truth. **Ước lượng:** 20 phút.
- [ ] **S6-TV5-43 — Review TV5 commit report chưa track** — **DoD:** Nội dung đúng được đưa vào engineering docs; nội dung sai bị bỏ. **Ước lượng:** 30 phút.
- [ ] **S6-TV5-44 — Audio/HUD/item integration tests** — **DoD:** Event quan trọng có automated test hoặc manual checklist tái lập. **Ước lượng:** 75 phút.

## 9. Lịch tích hợp Sprint 6

### Ngày 1 — Khóa interfaces và xử lý blocker nền tảng

- TV1 khóa progress/state/event interfaces.
- TV2 sửa camera 640×360.
- TV3 sửa accumulator và movement input contract.
- TV4 khóa symbols/validator và release level catalog.
- TV5 khóa input triggers, item events và audio IDs.
- Không merge implementation dựa trên interface cũ sau cuối ngày.

### Ngày 2 — Full loop cơ bản

- Level 1–3 load được.
- Progress giữ qua level.
- Death/respawn/GameOver chạy.
- Pause/resume chạy.
- Mushroom/FireFlower/Star có runtime path.

### Ngày 3 — Combat, HUD, audio và save

- FireBall hoạt động trong main game.
- Goomba/Koopa/shell hoàn chỉnh.
- Save/high score load được.
- HUD timer/power và per-level music hoạt động.

### Ngày 4 — Cleanup và regression

- Xử lý code thừa Sprint 1–6.
- Hoàn thiện docs/build configuration.
- Bổ sung automated tests.
- Chơi liên tục ba level.

### Ngày cuối — Sprint 6 demo gate

- Clean Debug/Release build.
- CTest pass.
- Menu → Level 1 → Level 2 → Level 3 → Win.
- Death → Respawn → GameOver → Retry/Menu.
- Restart executable → high score và volume còn đúng.

## 10. Test matrix Sprint 6

### Automated

- Input Pressed/Held/Released, Run, Shoot và Pause suppression.
- Walk/run/short-hop/full-jump/double-jump prevention.
- Growth anchor và low-ceiling clearance.
- Damage chain, grace period, Star timer, death/respawn.
- Stomp, side hit, shell, FireBall và Star collision.
- Deferred body destruction và entity cleanup.
- Tile ground/brick behavior, QuestionBlock single-use và flag trigger.
- Level validation cho width/symbol/spawn/finish/pole.
- Progress qua level transition.
- State pause/resume và terminal-result race.
- EventBus observer destruction.
- Save missing/valid/corrupt/version mismatch/high-score monotonicity.
- Volume clamp và timer pause behavior.

### Manual

- Hoàn thành ba level liên tục.
- Mỗi level có normal, no-damage và death/respawn route.
- Pause/resume tối thiểu ba lần mỗi level.
- Collect Mushroom, FireFlower, Star và 1-Up.
- Stomp Goomba, đá shell, shell kill và FireBall kill.
- Chạm flag ở phần thấp và cao của pole.
- Resize, alt-tab và close window trong gameplay.
- GameOver/Retry/Menu và Win/Menu.
- Restart game để xác minh save/high score/volume.

## 11. Rủi ro và nguyên tắc xử lý

- Khối lượng Sprint 6 vượt capacity thông thường của ba ngày còn lại; nhóm đã chọn giữ toàn bộ cam kết nên phải parallel hóa và review hằng ngày.
- Nếu task P0/P1 chưa xong, Sprint 6 không đạt DoD; không đổi tên thành polish để đẩy sang Sprint 7.
- TV4 sở hữu SaveManager theo `ROLES.md`; TV1 chỉ tích hợp state.
- Thành viên không sửa module của người khác nếu chưa thông báo owner.
- Không thêm Luigi, level editor, Bowser, multiplayer hoặc underwater mechanics trong Sprint 6.

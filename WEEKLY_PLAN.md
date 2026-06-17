# Kế hoạch 6 tuần — Công việc từng thành viên

> **Demo checkpoint cuối mỗi tuần:** cả nhóm ngồi lại, chạy game, verify bằng mắt — không chấp nhận "xong rồi nhưng chưa test".

---

## Tuần 1 — Setup & Architecture

**Demo checkpoint:** Mọi người clone repo → `cmake .. && cmake --build .` → chạy được → thấy **cửa sổ SFML trống màu đen**, không báo lỗi.

| | TV1 (Architect) | TV2 (Engine) | TV3 (Physics) | TV4 (Level) | TV5 (Sound/UI) |
|---|---|---|---|---|---|
| **Việc chính** | Tạo repo, README, CMakeLists.txt (tải SFML), phân công | SFML window + game loop skeleton | Thiết kế `Entity` và `Character` base class | Định nghĩa format file level `.txt` | SoundManager skeleton + thu thập assets |
| **Chi tiết** | Vẽ class diagram sơ bộ trên draw.io; chọn và ghi lại 5 design pattern sẽ dùng; viết CMakeLists.txt (dùng FetchContent tải SFML); tạo branch `develop` | Viết `Game.h / Game.cpp`: init window, game loop với delta time, xử lý sf::Event; test build trên máy mọi người | Viết `Entity.h` (position, velocity, sprite); `Character.h` (health, direction, abstract `update()`); chưa implement | Viết spec format `.txt` cho level (ghi vào ROLES.md mục TV4); tạo `levels/level1.txt` placeholder | Tạo `SoundManager.h` (Singleton skeleton); lên danh sách asset cần (sprite sheet Mario, tileset, âm thanh); tìm asset free hoặc dùng placeholder |
| **Deliverable** | `README.md`, `ROLES.md`, `WEEKLY_PLAN.md`, class diagram draft | Project build được, cửa sổ 1280x720 mở ra | `Entity.h`, `Character.h` với stub methods | `levels/level1.txt` (placeholder), spec format ghi vào doc | `SoundManager.h`, danh sách asset đã có |
| **Thời gian ước tính** | ~5–6h | ~6–8h | ~3–4h | ~2–3h | ~4–5h |

---

## Tuần 2 — Core Mechanics

**Demo checkpoint:** Chạy game → thấy **sprite Mario trên màn hình** → bấm ←/→ Mario di chuyển → bấm Space Mario nhảy → Mario rơi xuống đất không xuyên qua nền.

| | TV1 (Architect) | TV2 (Engine) | TV3 (Physics) | TV4 (Level) | TV5 (Sound/UI) |
|---|---|---|---|---|---|
| **Việc chính** | Implement `Observer` / `EventBus` + `EventBus::publish/subscribe` | `TextureManager`, `AnimationSystem`, `Camera` | `Mario`: movement, jump, gravity; `PhysicsEngine`; collision với ground | `TileMap`: parse file `.txt` → render tile, load static level 1 | `InputHandler` + Command pattern (4 commands: MoveLeft, MoveRight, Jump, Pause) |
| **Chi tiết** | Viết `IObserver.h`, `ISubject.h`, `EventBus.h` (Singleton); test với 1 event giả; đảm bảo TV3/TV4/TV5 có thể dùng | `TextureManager`: load, cache sf::Texture; `AnimationSystem`: sprite sheet, frame switching theo thời gian; `Camera`: sf::View follow player, clamp boundary | `Mario.cpp`: `handleInput()` → `CommandHandler`; velocity += gravity * dt; AABB collision với tile sàn; không cho xuyên tile; chưa cần enemy | `TileMap`: đọc file, map ký tự → tile type; render bằng sf::VertexArray hoặc từng sprite; connect với `Camera` của TV2 | `ICommand.h`, `InputHandler.h`; map sf::Keyboard::Key → ICommand; expose `getCommand(key)` cho TV3 dùng |
| **Deliverable** | `EventBus.h/.cpp` hoạt động, có test nhỏ | Mario sprite hiện lên, animation walk chạy | Mario di chuyển, nhảy, không xuyên sàn | Level 1 render đúng tile, camera follow | InputHandler map phím → action |
| **Thời gian ước tính** | ~5–6h | ~8–10h | ~10–12h | ~8–10h | ~6–7h |

---

## Tuần 3 — Level 1 Playable

**Demo checkpoint:** Chơi từ đầu Level 1 → gặp Goomba → giẫm chết hoặc bị chết → restart → đến cuối level → qua màn.

| | TV1 (Architect) | TV2 (Engine) | TV3 (Physics) | TV4 (Level) | TV5 (Sound/UI) |
|---|---|---|---|---|---|
| **Việc chính** | Implement `EntityFactory` (Factory pattern); review & merge tuần 2 | Death animation, spawn animation, level transition (màn hình "fade") | Collision với enemy: giẫm → enemy chết; chạm bên → Mario mất mạng | Level 1 hoàn chỉnh + `Goomba` AI patrol | `Coin` item, sound effects cơ bản |
| **Chi tiết** | `EntityFactory`: `createEnemy(type, pos)`, `createItem(type, pos)` — trả về `Entity*` polymorphically; TV4 dùng Factory này để spawn enemy từ file level | `AnimationSystem` thêm clip "death", "spawn"; `Camera` shake nhẹ khi Mario chết; transition fade-to-black 0.5s khi qua màn | `CollisionManager::resolve()`: nếu Mario hit top của enemy → enemy die; nếu hit side → Mario lose life; `LivesManager` quản lý số mạng | Level 1 dựng xong (đủ dài, có ít nhất 3 nhóm enemy, có chướng ngại); `Goomba.cpp`: move left until hit wall → reverse; `Goomba` dies khi bị giẫm | `Coin.h/.cpp` kế thừa `Item`; collect khi Mario overlap; publish `COIN_COLLECTED` event; sound jump.wav, coin.wav, die.wav load được |
| **Deliverable** | `EntityFactory.h/.cpp`; merge branch tuần 2 clean | Death + spawn animation; level transition | Collision enemy hoạt động đúng | Level 1 playable end-to-end | Coin hoạt động; 3 sound effects chạy |
| **Thời gian ước tính** | ~5–6h | ~7–8h | ~8–10h | ~10–12h | ~6–8h |

---

## Tuần 4 — Full Game Loop (3 levels, Game State, Items)

**Demo checkpoint:** Từ menu → chọn Start → chơi 3 level liên tiếp → Game Over / Win → quay về menu. Mushroom làm Mario to.

| | TV1 (Architect) | TV2 (Engine) | TV3 (Physics) | TV4 (Level) | TV5 (Sound/UI) |
|---|---|---|---|---|---|
| **Việc chính** | `GameState` machine (State pattern) | Menu screen, GameOver, Win screen | Power-up system: Small/Big/Fire Mario | Level 2 + Level 3; Koopa enemy | FireFlower + Star items; background music; HUD đầy đủ |
| **Chi tiết** | `IGameState.h` (interface: init, update, render, exit); `MenuState`, `PlayState`, `PauseState`, `GameOverState`, `WinState`; `GameManager` lưu stack state và gọi changeState | Menu: background + text + sf::Sound click; GameOver: score hiển thị; Win: "Congratulations" screen; Pause: overlay mờ | `MarioState` enum (Small/Big/Fire); khi collect Mushroom → grow animation; khi bị đánh khi Big → shrink; khi collect FireFlower → Fire form + có thể bắn FireBall | Level 2 và Level 3 design (khó hơn: nhiều enemy hơn, platform cao hơn, ít coin hơn); `Koopa.cpp`: bị giẫm → thành shell → shell trượt khi bị đá | `Mushroom.h/.cpp` (collect → notify TV3 grow); `FireFlower.h/.cpp`; `Star.h/.cpp` (invincibility timer); background music ogg loop; HUD: score, lives, level name, thời gian đếm ngược |
| **Deliverable** | Game state machine hoạt động: menu → play → gameover | Menu + GameOver + Win screens | Power-up đúng hành vi | Level 2 + 3 playable; Koopa | Items + music + HUD đầy đủ |
| **Thời gian ước tính** | ~8–10h | ~8–10h | ~8–10h | ~10–12h | ~10–12h |

---

## Tuần 5 — Design Patterns & Polish

**Demo checkpoint:** Code review nội bộ: TV1 mở code và chỉ đúng chỗ implement từng trong 5 pattern. Chạy game: có âm thanh đầy đủ, score lưu lại sau khi thoát.

| | TV1 (Architect) | TV2 (Engine) | TV3 (Physics) | TV4 (Level) | TV5 (Sound/UI) |
|---|---|---|---|---|---|
| **Việc chính** | Code review 5 patterns; viết `docs/design_patterns.md`; `SaveManager` | Visual polish: particle, smooth transitions | Physics edge cases; gameplay feel tuning | Enemy AI cải thiện; spawn system qua Factory | Full sound integration; save/load highscore |
| **Chi tiết** | Đọc toàn bộ code, xác nhận 5 pattern implement đúng (Factory, Singleton, Observer, State, Command); viết doc mô tả từng pattern: class áp dụng, lý do chọn, UML nhỏ; `SaveManager`: ghi score + progress vào `saves/save.dat` | Thêm particle đơn giản khi enemy chết (sf::CircleShape bắn ra); screen shake khi Mario bị đánh; animation transition mượt hơn (flip sprite khi đổi hướng) | Kiểm tra và fix: Mario bị kẹt góc tường, nhảy đôi, xuyên tile mỏng; tinh chỉnh jump height và walk speed cho cảm giác tốt hơn | AI: Goomba nhìn thấy Mario trong range → tăng speed; spawn enemy đúng qua EntityFactory (không còn hard-code `new Goomba`); test toàn bộ enemy behavior | Tất cả sound effect gắn đúng event (subscribe EventBus); `SoundManager` quản lý volume; `SaveManager` ghi/đọc highscore; hiển thị highscore trên menu |
| **Deliverable** | `docs/design_patterns.md` hoàn chỉnh; `SaveManager` | Visual cải thiện rõ rệt | Gameplay mượt, không glitch thường gặp | Enemy AI + spawn qua Factory | Sound đầy đủ; highscore lưu được |
| **Thời gian ước tính** | ~8–10h | ~6–8h | ~6–8h | ~6–8h | ~6–8h |

---

## Tuần 6 — Polish, Test & Nộp bài

**Demo checkpoint:** Chơi toàn bộ game từ menu đến win screen không crash. Video demo quay xong. Tài liệu đầy đủ.

| | TV1 (Architect) | TV2 (Engine) | TV3 (Physics) | TV4 (Level) | TV5 (Sound/UI) |
|---|---|---|---|---|---|
| **Việc chính** | Class diagram final; check rubric; tổng hợp tài liệu | Fix render bug; ổn định frame rate | Bug fix gameplay; test 3 level | Level balance; bug fix enemy | Quay video demo; đóng gói nộp |
| **Chi tiết** | Vẽ class diagram chính thức (draw.io) có đủ class, kế thừa, association; kiểm từng dòng rubric (65+35+15 điểm), đánh dấu done/missing; compile tài liệu cho nộp | Cap frame rate 60fps; fix bất kỳ flicker, z-order sai; test trên máy có màn hình khác kích thước | Chơi 3 level liên tiếp 3 lần không crash; log bug → fix bug | Chỉnh lại vị trí enemy/coin/platform để độ khó mượt hơn; test enemy không bị stuck | Quay video demo (OBS): gameplay 3 level + thuyết minh ngắn về pattern đã dùng; zip project, kiểm tra build từ đầu trên máy sạch |
| **Deliverable** | `docs/class_diagram.png`; checklist rubric | Build release ổn định | Zero critical bug | 3 level balanced | Video demo + project zip |
| **Thời gian ước tính** | ~8–10h | ~4–6h | ~6–8h | ~4–6h | ~6–8h |

---

## Tóm tắt milestone theo tuần

```
Tuần 1 │ ▪ Cửa sổ SFML mở được (mọi máy)
Tuần 2 │ ▪ Mario di chuyển + nhảy trên platform
Tuần 3 │ ▪ Level 1 chơi được đầu → cuối (có enemy)
Tuần 4 │ ▪ 3 level + menu + game over + items
Tuần 5 │ ▪ 5 pattern hoàn chỉnh + sound + save
Tuần 6 │ ▪ Nộp bài (source + doc + video)
```

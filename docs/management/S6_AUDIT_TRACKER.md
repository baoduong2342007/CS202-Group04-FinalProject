# Sprint 1–6 Audit Tracker

> **Owner:** TV1 (Dương) — S6-TV1-01
> **Mục đích:** Liệt kê từng deliverable trong `WEEKLY_PLAN.md`, trạng thái thực tế, task xử lý và bằng chứng kiểm thử.
> **DoD:** Không có deliverable nào thiếu trạng thái hoặc owner.

---

## Cách dùng

- Cột **Trạng thái**: `DONE` / `DOING` / `BLOCKED` / `NOT DONE`.
- Cột **Task xử lý**: ID task Sprint 6 (ví dụ `S6-TV1-08`) hoặc bug ID.
- Cột **Bằng chứng / Blocker**: test, build log, manual checklist, hoặc nguyên nhân block.

---

## Sprint 1 (17/06 – 04/07)

| Deliverable | Owner | Trạng thái | Task xử lý | Bằng chứng / Blocker |
|---|---|---|---|---|
| `README.md`, `ROLES.md`, `WEEKLY_PLAN.md`, class diagram draft | TV1 | DONE | S6-TV1-30 | Đã có trên develop |
| Project compiles, 1280x720 window opens | TV2 | DONE | — | 3/3 CMake preset pass |
| `Entity.h`, `Character.h` with stub methods | TV3 | DONE | — | Đã phát triển thành Entity full |
| `levels/level1.txt` (placeholder), format spec in doc | TV4 | DONE | — | Đã thành TileMap thật |
| `SoundManager.h`, list of acquired assets | TV5 | DONE | S6-TV5-24 | Assets đã commit đủ |

## Sprint 2 (06/07 – 11/07)

| Deliverable | Owner | Trạng thái | Task xử lý | Bằng chứng / Blocker |
|---|---|---|---|---|
| Functional `EventBus.h/.cpp` with safe iteration | TV1 | DONE | S6-TV1-20 | `event_bus_tests` pass |
| `Game.h/.cpp` refactored | TV2 | DONE | — | Bỏ magic number |
| Box2D integrated, `PhysicsEngine`+`ContactListener` working | TV3 | DOING | S6-TV1-36 | Blocker: BUG-030 (Static Accumulator) |
| `TileMap.cpp` parses level files | TV4 | DONE | — | `TileCollisionSpanTests` pass |
| Command pattern headers complete | TV5 | DONE | S6-TV1-24 | `InputStateTests` pass |

## Sprint 4 (19/07 – 25/07)

| Deliverable | Owner | Trạng thái | Task xử lý | Bằng chứng / Blocker |
|---|---|---|---|---|
| `EntityFactory.h/.cpp` functional; all Sprint 2 branches merged | TV1 | DONE | S6-TV1-22 | Factory dùng `unique_ptr` |
| Mario sprite visible with walk/jump animation; camera follows player | TV2 | DOING | S6-TV2-10 | Blocker: BUG-028 (Camera thiếu Y-clamp, ép 16:9) |
| Physics feels correct; no magic numbers; collision directions work | TV3 | DOING | S6-TV3-16/20 | Blocker: BUG-029 (Growth clearance), BUG-032 (Double-Dispatch) |
| Level 1 renders correctly; Goomba walks back and forth | TV4 | DONE | S6-TV4-13 | Manual test pass |
| `InputHandler` maps keys to actions; `SoundManager` plays sounds; `Coin` collectable | TV5 | DONE | S6-TV5-07/24 | Manual test pass |

## Sprint 5 (26/07 – 01/08)

| Deliverable | Owner | Trạng thái | Task xử lý | Bằng chứng / Blocker |
|---|---|---|---|---|
| Game state machine works: can transition between states | TV1 | DONE | S6-TV1-15/16 | `game_manager_tests` pass |
| Death/spawn animations; level transition works | TV2 | DONE | S6-TV1-12 | Freeze input khi chuyển màn thành công |
| Enemy collision behavior correct; FireBall works | TV3 | DOING | S6-TV3-19 | Blocker: BUG-031 (Duplicate FireBall API thiếu limit) |
| Level 1 playable end-to-end; Koopa works; Level 2 started | TV4 | DOING | S6-TV4-21 | Đang design map 2 |
| Items work correctly; 3+ sound effects active; HUD displays info | TV5 | DOING | S6-TV5-17/26 | Blocker: BUG-033 (UI update ngoài map 1) |

## Sprint 6 (02/08 – 08/08)

| Deliverable | Owner | Trạng thái | Task xử lý | Bằng chứng / Blocker |
|---|---|---|---|---|
| All 5 game states transition correctly; high scores save/load | TV1 | DONE | S6-TV1-08/19 | `save_session_tests` pass, BUG-027 fixed |
| Menu + GameOver + Win + Pause screens functional | TV2 | DONE | S6-TV1-14 | Bố cục text căn giữa màn hình WinState ok |
| Power-ups work correctly with visual feedback | TV3 | DOING | S6-TV3-16 | Đợi fix lỗi Growth Clearance |
| Levels 2 & 3 playable; Koopa works in all levels | TV4 | DOING | S6-TV4-40 | Chờ full end-to-end manual playthrough |
| Background music loops; full HUD with timer; all SFX bound | TV5 | DOING | S6-TV5-24 | Chờ kiểm tra lại SFX |

---

## Log integration review (S6-TV1-35)

| Ngày | Commit HEAD | Debug | Release | CTest | Warning production | Blocker |
|---|---|---|---|---|---|---|
| 08/08/2026 | `8114ab5` + working tree | ✅ build-debug pass | ✅ build-release pass | ✅ 7/7 pass | CMake deprecation Box2D | SaveManager chưa nối state |
| 09/08/2026 | S6 Evaluate v3 | ✅ build-debug pass | ✅ build-release pass | ✅ **12/12 pass** | CMake deprecation Box2D | S6-TV1-14 chờ TV4 test tay Level 3 |
# Sprint 1–6 Audit Tracker

> **Owner:** TV1 (Dương) — S6-TV1-01
> **Mục đích:** Liệt kê từng deliverable trong `WEEKLY_PLAN.md`, trạng thái thực tế, task xử lý và bằng chứng kiểm thử.
> **DoD:** Không có deliverable nào thiếu trạng thái hoặc owner.

---

## Cách dùng

- Cột **Trạng thái**: `DONE` / `DOING` / `BLOCKED` / `NOT DONE`.
- Cột **Task xử lý**: ID task Sprint 6 (ví dụ `S6-TV1-08`) hoặc bug ID.
- Cột **Bằng chứng**: test, build log, hoặc manual checklist.

---

## Deliverable theo Sprint

### Sprint 1–2 (Nền tảng)

| Deliverable | Owner | Trạng thái | Task xử lý | Bằng chứng |
|---|---|---|---|---|
| CMake + SFML bootstrap | TV1 | DONE | S6-TV1-27 | Build pass |
| Game loop + window | TV2 | DONE | — | Build pass |
| State machine cơ bản | TV1 | DONE | S6-TV1-15/16 | Build pass |
| EventBus + Observer | TV1 | DONE | S6-TV1-20/21 | CTest pass |
| Command pattern | TV5 | DONE | S6-TV1-24/25 | CTest pass |
| EntityFactory | TV1 | DONE | S6-TV1-22/23 | Build pass |

### Sprint 3 (Physics & Mario)

| Deliverable | Owner | Trạng thái | Task xử lý | Bằng chứng |
|---|---|---|---|---|
| Mario movement physics | TV3 | DONE | — | MarioPhysicsTests pass |
| Box2D integration | TV3 | DONE | — | Build pass |
| Collision handling | TV3 | DOING | S6-TV3-25→31 | — |

### Sprint 4 (Level & Items)

| Deliverable | Owner | Trạng thái | Task xử lý | Bằng chứng |
|---|---|---|---|---|
| TileMap parse/render | TV4 | DONE | — | TileCollisionSpanTests pass |
| Level 1 | TV4 | DOING | S6-TV4-13/14 | — |
| Items (Coin/Mushroom) | TV5 | DOING | S6-TV5-07→16 | — |

### Sprint 5 (Enemy, HUD, Sound)

| Deliverable | Owner | Trạng thái | Task xử lý | Bằng chứng |
|---|---|---|---|---|
| Goomba/Koopa | TV4 | DOING | S6-TV4-21→31 | — |
| HUD | TV5 | DOING | S6-TV5-17→23 | — |
| SoundManager | TV5 | DOING | S6-TV5-24→36 | — |

### Sprint 6 (Full loop)

| Deliverable | Owner | Trạng thái | Task xử lý | Bằng chứng |
|---|---|---|---|---|
| GameProgress session | TV1 | DONE | S6-TV1-08/09/10 | Snapshot trước khi hủy Level; `setMarioState()` dựng lại animation → giữ SUPER/FIRE visual qua level (build pass) |
| Level progression 1-based | TV1 | DONE | S6-TV1-07 | LevelCatalog one-based; New Game bắt đầu Level 1 (build pass) |
| Load error handling | TV1 | DONE | S6-TV1-11 | Load chuyển về `onEnter()`, queue Menu đúng FIFO; không để failed PlayState (build pass) |
| Transition freeze + input block | TV1 | DONE | S6-TV1-12 | `processInput` chặn trong transition; reload fail không vào FADE_IN, không phát `LEVEL_STARTED` (build pass) |
| Terminal-result race guard | TV1 | PARTIAL | S6-TV1-13/14 | Guard + Win đã viết (PlayState). PlayStateTests vẫn chỉ test `isPastFinalLevel()`; test thật cần SFML/audio context |
| Deferred state ops (state stack) | TV1 | DONE | S6-TV1-16 | GameManager dùng `m_stateStack` + snapshot queue; `game_manager_tests` pass (stack lifecycle, deferred-op, overlay teardown) |
| Pause/Resume lifecycle | TV1 | DONE | S6-TV1-15/17 | IGameState lifecycle đủ; Pause dừng update/input/music; subscriber-growth chưa có test 20 vòng riêng (dùng event_bus_tests) |
| `PLAYER_DIED` mỗi lần chết | TV1 | DONE | S6-TV1-18 | `Mario::loseLife()` phát event mỗi lần trừ life; GameOver theo lives còn lại (build pass) |
| Observer lifetime / subscriber growth | TV1 | DONE | S6-TV1-20/21 | `event_bus_tests` pass: unsubscribe giữa callback, không duplicate subscriber, không stale callback sau nhiều vòng |
| SaveManager | TV4 | BLOCKED | S6-TV1-19 | Owner TV4; TV1 giữ `BLOCKED`, chỉ tích hợp khi interface sẵn sàng |
| CMake: SHA256 + target name | TV1 | DONE | S6-TV1-27/28 | `EXPECTED_HASH` thật (SHA256) cho SFML zips; target đổi `main` → `SuperMario`; 3 preset clean build |
| CMake: CopyAssets per-file | TV1 | DONE | S6-TV1-29 | `copy_if_different` per-file + `make_directory`; no-op build không re-copy assets (đã kiểm) |
| LevelCatalog (centralized levels) | TV1 | DONE | S6-TV1-05 | PlayState dùng catalog, không còn hard-code path/`MAX_LEVELS` |
| EntityFactory → unique_ptr | TV1 | DONE | S6-TV1-22 | Không còn raw `new` ở factory; call site dùng `std::make_unique` |
| Docs/metadata/diagram cleanup | TV1 | DONE | S6-TV1-23/30→34 | CODING_RULES = Simple Factory; class_diagram sửa API sai; README test/controls; FILE_STRUCTURE; xóa 26 `.gitkeep`dư |
| Demo exclusion (CMake) | TV1 | DONE | S6-TV1-26 | `list(FILTER SOURCES EXCLUDE REGEX)` loại `src/demo/` |

---

## Ghi chú

- Cập nhật hằng ngày trong Sprint 6.
- Task nào chưa đạt DoD phải giữ `BLOCKED`/`NOT DONE`, không âm thầm chuyển sang Sprint 7.
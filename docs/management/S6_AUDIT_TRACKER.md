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
| GameProgress session | TV1 | DONE | S6-TV1-08/09/10 | Build pass |
| Level progression 1-based | TV1 | DONE | S6-TV1-07 | Build pass |
| Load error handling | TV1 | DONE | S6-TV1-11 | Build pass |
| Terminal-result race guard | TV1 | DONE | S6-TV1-13 | Build pass |
| Deferred state ops | TV1 | DONE | S6-TV1-16 | Build pass |
| Pause/Resume lifecycle | TV1 | DONE | S6-TV1-15 | Build pass |
| CMake presets | TV1 | DONE | S6-TV1-28 | Build pass |
| Docs cleanup | TV1 | DONE | S6-TV1-30→34 | — |
| Unsubscribe audit (all observers) | TV1 | DONE | S6-TV1-21 | PlayState ✅, HUD ✅, SoundManager ✅ (Singleton safe) |
| LevelCatalog (centralized levels) | TV1 | DONE | S6-TV1-05 | PlayState dùng catalog, không còn hard-code path/MAX_LEVELS |
| EntityFactory → unique_ptr | TV1 | DONE | S6-TV1-22 | Không còn raw `new` ở factory; Level.cpp dùng `std::move` |
| Transition freeze + fade | TV1 | DONE | S6-TV1-12 | FADE_OUT→LOADING→FADE_IN state machine, gameplay frozen |
| Demo exclusion (CMake) | TV1 | DONE | S6-TV1-26 | `list(FILTER SOURCES EXCLUDE REGEX)` loại src/demo/ |

---

## Ghi chú

- Cập nhật hằng ngày trong Sprint 6.
- Task nào chưa đạt DoD phải giữ `BLOCKED`/`NOT DONE`, không âm thầm chuyển sang Sprint 7.
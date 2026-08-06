# Sprint 6 — Dependency Board

> **Owner:** TV1 (Dương) — S6-TV1-04
> **Mục đích:** Ghi rõ task nào chặn task nào giữa năm module.
> **DoD:** Các task cross-module đều có producer/consumer và thời hạn bàn giao.

---

## Ký hiệu

- `A → B`: A phải xong trước B (A là producer, B là consumer).
- **Thời hạn bàn giao:** ngày trong Sprint 6 (Ngày 1–5).

---

## Cross-module dependencies

| Producer | Consumer | Dependency | Thời hạn |
|---|---|---|---|
| TV4 (metadata) | TV1 | S6-TV1-05 LevelCatalog → S6-TV1-06/07 | Ngày 1 |
| TV1 (GameProgress) | TV1 | S6-TV1-08 → S6-TV1-09/10 | Ngày 1 |
| TV1 (snapshot/restore) | TV1 | S6-TV1-09/10 → S6-TV1-12 transition | Ngày 2 |
| TV2 (fade) | TV1 | TV2 fade → S6-TV1-12 transition | Ngày 2 |
| TV4 (Level 3) | TV1 | TV4 Level 3 → S6-TV1-14 Win transition | Ngày 3 |
| TV5 (music resume) | TV1 | TV5 → S6-TV1-17 Pause lifecycle | Ngày 2 |
| TV3 (death flow) | TV1 | TV3 → S6-TV1-18 death/GameOver event | Ngày 2 |
| TV4 (SaveManager) | TV1 | TV4 → S6-TV1-19 SaveManager integration | Ngày 3 |
| TV4 (entity constructors) | TV1 | TV4 → S6-TV1-22 unique_ptr factory | Ngày 1 |
| TV5 (InputHandler) | TV1 | TV5 → S6-TV1-24 xóa undo | Ngày 1 |
| TV3 (scenario→tests) | TV1 | TV3 → S6-TV1-26 loại demo | Ngày 4 |
| TV5 (RunCommand) | TV3 | TV5 → S6-TV3-03 run state | Ngày 2 |
| TV5 (input state) | TV3 | TV5 → S6-TV3-04 jump release | Ngày 2 |
| TV4 (bounds) | TV3 | TV4 → S6-TV3-07 pit threshold | Ngày 2 |
| TV2 (render hook) | TV3 | TV2 → S6-TV3-14 damage blink | Ngày 3 |
| TV4 (shell) | TV3 | TV4 → S6-TV3-28 shell-enemy | Ngày 3 |
| TV5 (scoring) | TV3 | TV5 → S6-TV3-31 score by cause | Ngày 3 |
| TV1 (catalog) | TV2 | TV1 → S6-TV2-27 background theme | Ngày 2 |
| TV5 (assets) | TV2 | TV5 → S6-TV2-27 background theme | Ngày 2 |
| TV3 (fixture policy) | TV2 | TV3 → S6-TV2-23 grow/shrink | Ngày 3 |
| TV3 (event) | TV2 | TV3 → S6-TV2-09 shake | Ngày 2 |
| TV3 (max substeps) | TV2 | TV3 → S6-TV2-13 clamp dt | Ngày 1 |
| TV5 (shoot) | TV3 | TV5 → S6-TV3-17 shoot API | Ngày 2 |
| TV3 (shoot) | TV5 | TV3 → S6-TV5-09 FireFlower | Ngày 3 |
| TV3 (timer) | TV5 | TV3 → S6-TV5-10 Star semantics | Ngày 3 |
| TV3 (death) | TV5 | TV3 → S6-TV5-19 timeout death | Ngày 3 |
| TV2 (Pause UI) | TV5 | TV2 → S6-TV5-36 volume controls | Ngày 3 |
| TV4 (SaveManager) | TV5 | TV4 → S6-TV5-36 volume controls | Ngày 3 |

---

## Ghi chú

- Cập nhật hằng ngày khi có task hoàn thành.
- Task bị chặn phải giữ `BLOCKED` và ghi rõ blocker trong tracker.
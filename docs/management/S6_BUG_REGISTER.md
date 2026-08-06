# Bug Register P0/P1/P2 — Sprint 1–6

> **Owner:** TV1 (Dương) — S6-TV1-02
> **Mục đích:** Gộp bug camera, progress, state, physics, enemy, item, audio, save, build và documentation.
> **DoD:** Mỗi bug có severity, reproduction, owner và task ID.

---

## Cách dùng

- **Severity:** `P0` (crash/blocker), `P1` (major), `P2` (minor).
- **Trạng thái:** `OPEN` / `FIXED` / `REOPENED`.
- **Task ID:** ID task Sprint 6 xử lý bug này.

---

## P0 — Crash / Blocker

| ID | Mô tả | Reproduction | Owner | Task ID | Trạng thái |
|---|---|---|---|---|---|
| BUG-001 | Level load fail bị bỏ qua → crash | Xóa level1.txt rồi New Game | TV1 | S6-TV1-11 | FIXED |
| BUG-002 | Observer bị hủy vẫn được gọi trong notify | Unsubscribe trong callback | TV1 | S6-TV1-20 | FIXED |
| BUG-003 | State bị hủy khi đang trên call stack | pushState trong onNotify | TV1 | S6-TV1-16 | FIXED |

## P1 — Major

| ID | Mô tả | Reproduction | Owner | Task ID | Trạng thái |
|---|---|---|---|---|---|
| BUG-004 | Score/coin/lives reset khi chuyển level | Hoàn thành level 1 | TV1 | S6-TV1-08/09/10 | FIXED |
| BUG-005 | New Game bắt đầu từ Level 0 | New Game | TV1 | S6-TV1-07 | FIXED |
| BUG-006 | Death và completion race cùng frame | Chết khi chạm flag | TV1 | S6-TV1-13 | FIXED |
| BUG-007 | Pause không gọi onPause/onResume | ESC trong gameplay | TV1 | S6-TV1-15 | FIXED |
| BUG-008 | `ICommand::undo()` rỗng gây nhầm lẫn | — | TV1 | S6-TV1-24 | FIXED |
| BUG-009 | PauseCommand giữ dependency thừa | — | TV1 | S6-TV1-25 | FIXED |
| BUG-010 | SFML download dùng TLS_VERIFY OFF | Configure lần đầu | TV1 | S6-TV1-27 | FIXED |
| BUG-011 | Asset copy toàn bộ mỗi build | Build lặp | TV1 | S6-TV1-29 | FIXED |

## P2 — Minor

| ID | Mô tả | Reproduction | Owner | Task ID | Trạng thái |
|---|---|---|---|---|---|
| BUG-012 | README link tới file không tồn tại | Mở README | TV1 | S6-TV1-30 | FIXED |
| BUG-013 | FILE_STRUCTURE mô tả file không có | Đọc FILE_STRUCTURE | TV1 | S6-TV1-31 | FIXED |
| BUG-014 | Class diagram dùng API draft cũ | Đọc class_diagram | TV1 | S6-TV1-32 | FIXED |
| BUG-015 | Sprint 5 error plan còn là instruction | Đọc doc cũ | TV1 | S6-TV1-33 | FIXED |
| BUG-016 | .gitattributes thiếu FLAC/PDF binary | Git add | TV1 | S6-TV1-34 | FIXED |
| BUG-017 | EntityFactory gọi sai tên Factory Method | Code review | TV1 | S6-TV1-23 | FIXED |

---

## Ghi chú

- Bug mới phát hiện trong Sprint 6 phải được thêm vào đây với đầy đủ severity/reproduction/owner.
- Bug P0/P1 chưa xong → Sprint 6 không đạt DoD.
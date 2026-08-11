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
| BUG-022 | Death phát cặp `PLAYER_DIED` + `PLAYER_LOST_LIFE` và Mario inactive lúc chết (level cũ) | Chết trong gameplay (commit 881335d) | TV1 | S6-TV1-18 | FIXED — `loseLife()` chỉ phát 1 event, trừ 1 life, giữ `m_active`; có `play_state_tests` |
| BUG-023 | Giới hạn FireBall còn `MAX_ACTIVE_FIREBALLS = 4` trái contract max 2 | Bắn 3–4 viên | TV3 | S6-TV3-19 | OPEN |
| BUG-027 | High score không lưu khi chết còn mạng hoặc qua màn trước khi GameOver/Win | Chết khi còn mạng -> thoát game -> restart | TV1 | S6-TV1-19 | FIXED — `PlayState::onNotify` cập nhật high score tức thì cho `PLAYER_DIED` & `LEVEL_COMPLETED`; log lỗi save không điều kiện; có `save_session_tests` |
| BUG-028 | Camera ép 16:9 liên tục gây mờ pixel, thiếu Y-clamp | Kéo resize cửa sổ hoặc nhảy lên cao | TV2 | S6-TV2-10 | OPEN |
| BUG-029 | Lỗi kẹt tường/gạch khi ăn Mushroom (Growth clearance) | Ăn Mushroom trong không gian hẹp (thiếu trần) | TV3 | S6-TV3-16 | OPEN |
| BUG-030 | Lỗi physics lố nhịp do Static Accumulator | Chuyển level hoặc reload map liên tục | TV1 | S6-TV1-36 | OPEN |
| BUG-031 | Duplicate FireBall API (bỏ qua max limit = 2) | Gọi spawnFireBall thay vì shootFireBall | TV3 | S6-TV3-19 | OPEN |
| BUG-032 | Collision Double-Dispatch bypass rules trung tâm | Entity bị hit bởi Shell/Fireball | TV3 | S6-TV3-20 | OPEN |

## P2 — Minor

| ID | Mô tả | Reproduction | Owner | Task ID | Trạng thái |
|---|---|---|---|---|---|
| BUG-012 | README link tới file không tồn tại | Mở README | TV1 | S6-TV1-30 | FIXED |
| BUG-013 | FILE_STRUCTURE mô tả file không có | Đọc FILE_STRUCTURE | TV1 | S6-TV1-31 | FIXED |
| BUG-014 | Class diagram dùng API draft cũ | Đọc class_diagram | TV1 | S6-TV1-32 | FIXED |
| BUG-015 | Sprint 5 error plan còn là instruction | Đọc doc cũ | TV1 | S6-TV1-33 | FIXED |
| BUG-016 | .gitattributes thiếu FLAC/PDF binary | Git add | TV1 | S6-TV1-34 | FIXED |
| BUG-017 | EntityFactory gọi sai tên Factory Method | Code review | TV1 | S6-TV1-23 | FIXED |
| BUG-018 | Transition: 1-frame mờ dần nháy sáng | Chuyển level (LOADING->FADE_IN) | TV1 | S6-TV1-12 | FIXED |
| BUG-019 | Transition: Camera giật & Sprite thu nhỏ | Quan sát 0.5s đầu FADE_IN | TV1 | S6-TV1-12 | FIXED |
| BUG-020 | Ghost Collisions: Mario kẹt khi trượt ngang trên Question Block | Spam trái/phải trên Question Block | TV3 | S6-TV3-25 (contact normalization) | OPEN — chờ TV3-25 merge + regression test |
| BUG-021 | WinState bị mất chữ (màn hình đen thui) | Hoàn thành World 3 | TV1 | S6-TV1-12 | FIXED |
| BUG-024 | README ghi Shoot = `J/F/X/Ctrl/Shift` — Shift vừa Run vừa Shoot, sai contract | Đọc README / Controls | TV1 | S6-TV1-30 | FIXED — Shift=Run (hold), X=Shoot (press) |
| BUG-025 | FILE_STRUCTURE liệt kê file không tồn tại: `implementation_plan_sprint5_error.md`, `docs/PLAN_TV1.md`, `ui/Button.h/.cpp` | Đọc FILE_STRUCTURE | TV1 | S6-TV1-31/33 | FIXED — gỡ mọi entry phantom; Sprint 5 error plan không còn trong repo (lý do ghi tracker) |
| BUG-026 | Tracker ghi "7/7 pass", SaveManager `BLOCKED`, quyết định giữ tên `main` — không khớp code thật | Đọc S6_AUDIT_TRACKER | TV1 | S6-TV1-01/35 | FIXED — 12/12 pass, SaveManager REVIEW, target `SuperMario` |
| BUG-033 | UI High score / Mouse không cập nhật đúng ngoài màn 1 | Chơi sang Level 2 và kiểm tra UI/Mouse click | TV5 | S6-TV5-26 | OPEN |
---

## Ghi chú

- Bug mới phát hiện trong Sprint 6 phải được thêm vào đây với đầy đủ severity/reproduction/owner.
- Bug P0/P1 chưa xong → Sprint 6 không đạt DoD.
- Mỗi dòng `FIXED` phải có bằng chứng build/CTest hoặc manual checklist tương ứng; nếu chưa xác minh thì giữ `OPEN`/`REVIEW`.
- `BUG-008`/`BUG-009` (reproduction `—`) và `BUG-014/015` được cập nhật tại `class_diagram.md`/archive ngày 08/08/2026.
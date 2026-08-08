# Sprint 7 — Design Patterns, Polish, AI và săn bug ẩn

**Thời gian:** 09/08/2026–15/08/2026  
**Nguồn đối chiếu:** `WEEKLY_PLAN.md`, `ROLES.md`, kết quả audit codebase và Definition of Done của `s6_plan.md`.  
**Mục tiêu:** Xác minh đúng năm design patterns; kiểm thử sâu toàn bộ gameplay; cải thiện render, animation, audio và enemy AI; cân bằng ba level; đưa game đến trạng thái release candidate ổn định.

> Sprint 7 không dùng để âm thầm hoàn tất backlog Sprint 6. Nếu một task đã biết từ Sprint 1–6 chưa xong, task đó vẫn giữ ID/trạng thái Sprint 6. Sprint 7 chỉ tạo task mới cho bug chưa biết được phát hiện qua review, stress test hoặc exploratory playtest.

## 1. Điều kiện bắt đầu Sprint 7

- Menu → Level 1 → Level 2 → Level 3 → Win đã chạy được.
- GameOver, Retry, Pause và Return to Menu hoạt động.
- Score, coin và lives giữ đúng qua level transition.
- Save/high score load được sau restart.
- FireBall, Mushroom, FireFlower, Star và shell có runtime path hoàn chỉnh.
- Debug/Release build thành công.
- Mọi task Sprint 6 có trạng thái rõ ràng; không có task “đã làm gần xong” nhưng không có owner.

Nếu một điều kiện trên không đạt, TV1 phải ghi rõ Sprint 6 chưa hoàn thành trước khi tiếp tục Sprint 7.

## 2. Quy tắc quản lý bug Sprint 7

Mỗi bug mới tạo task theo mẫu:

```text
ID: S7-BUG-<OWNER>-<NUMBER>
Severity: P0 | P1 | P2 | P3
Environment:
Level/state:
Preconditions:
Steps to reproduce:
Expected behavior:
Actual behavior:
Reproduction rate:
Root cause:
Regression test:
Fix commit/PR:
Verification evidence:
```

Quy tắc severity:

- **P0:** crash, corrupt save, không thể hoàn thành level, kẹt state, mất progress. Dừng polish và sửa ngay.
- **P1:** gameplay sai rõ ràng, collision sai, audio/state hỏng, exploit làm bỏ qua level. Sửa trong ngày.
- **P2:** visual/audio/UI lỗi không chặn hoàn thành game. Triage hằng ngày.
- **P3:** góp ý nhỏ, refactor hoặc improvement không ảnh hưởng release. Chỉ làm khi P0–P2 đã đóng.

Bug không được đóng nếu thiếu reproduction hoặc verification. “Không tái hiện được” không phải `DONE`; phải ghi môi trường, số lần thử và bằng chứng.

## 3. Definition of Done Sprint 7

- Năm design patterns được mô tả đúng code thực tế.
- Không gọi Simple Factory hiện tại là canonical Factory Method.
- Không còn bug P0/P1.
- P2 còn lại phải có waiver và lý do rõ ràng từ TV1.
- Ba level pass automated validator và manual playthrough matrix.
- Mỗi level được chơi ít nhất 10 lần trong Sprint 7.
- Physics, render và audio soak test 30 phút không crash hoặc tăng resource vô hạn.
- Reload/respawn/state transition lặp 100 lần không tăng Box2D bodies/subscribers bất thường.
- Frame rate ổn định ở cấu hình mặc định.
- Pause/resume, Star music và level music transition đúng.
- Clean clone/configure/build/test/run thành công với relative asset paths.
- Tài liệu, class diagram và code reference nhất quán.

## 4. Chính sách public API Sprint 7

- Public API được freeze từ cuối Sprint 6.
- Không thêm API mới chỉ để phục vụ một call site.
- Mọi public API mới phải có ít nhất hai consumers hoặc lý do extension rõ ràng.
- Refactor nội bộ phải giữ behavior và có regression tests trước khi thay đổi.
- Thay đổi interface cross-module cần TV1 và owner của cả producer/consumer duyệt.

## 5. TV1 — Dương — Architecture Review và Design Patterns

### A. Design patterns documentation

- [ ] **S7-TV1-01 — Lập inventory năm patterns** — Factory, Singleton, Observer, State, Command. **DoD:** Mỗi pattern có participants, intent, runtime path và owner. **Ước lượng:** 45 phút.
- [ ] **S7-TV1-02 — Review Simple Factory** — Kiểm tra creation API, smart pointer ownership và supported spawn types. **DoD:** Tài liệu gọi đúng Simple Factory và nêu cách mở rộng. **Ước lượng:** 45 phút.
- [ ] **S7-TV1-03 — Review Singleton GameManager** — Kiểm tra lifetime, responsibility và state ownership. **DoD:** Không để GameManager thành global container cho logic không liên quan. **Ước lượng:** 30 phút.
- [ ] **S7-TV1-04 — Review Singleton SoundManager** — Kiểm tra initialization order, shutdown và resource lifetime. **DoD:** Không access sau destruction. **Ước lượng:** 30 phút.
- [ ] **S7-TV1-05 — Review Observer/EventBus** — Vẽ flow subscribe/notify/unsubscribe và event ownership. **DoD:** Có lifetime/reentrancy rules và ví dụ thực tế. **Ước lượng:** 60 phút.
- [ ] **S7-TV1-06 — Review State pattern** — Lập transition table Menu/Play/Pause/GameOver/Win. **DoD:** Mỗi transition có trigger, old-state callback và new-state callback. **Ước lượng:** 60 phút.
- [ ] **S7-TV1-07 — Review Command pattern** — Kiểm tra input mapping tách khỏi Mario behavior. **DoD:** Không còn command chứa dead dependency hoặc giả undo behavior. **Ước lượng:** 45 phút.
- [ ] **S7-TV1-08 — Viết phần Factory trong design patterns doc** — **DoD:** Có intent, class roles, sequence và extension example. **Ước lượng:** 45 phút.
- [ ] **S7-TV1-09 — Viết phần Singleton** — **DoD:** Nêu lợi ích, test limitation và lifecycle. **Ước lượng:** 45 phút.
- [ ] **S7-TV1-10 — Viết phần Observer** — **DoD:** Có event flow và safe unsubscribe policy. **Ước lượng:** 45 phút.
- [ ] **S7-TV1-11 — Viết phần State** — **DoD:** Có transition table và lifecycle contract. **Ước lượng:** 45 phút.
- [ ] **S7-TV1-12 — Viết phần Command** — **DoD:** Có key→command→receiver flow và giải thích Released/Held. **Ước lượng:** 45 phút.
- [ ] **S7-TV1-13 — Cross-check code references trong tài liệu** — **DoD:** Tất cả file/symbol được nhắc tới tồn tại. **Ước lượng:** 30 phút.

### B. Senior code review

- [ ] **S7-TV1-14 — Review dependency cycles** — Level/entities/physics/items/patterns. **DoD:** Chỉ refactor cycle gây lifetime/test bug; còn lại được ghi risk. **Ước lượng:** 75 phút.
- [ ] **S7-TV1-15 — Review ownership toàn codebase** — **DoD:** Mọi owning pointer dùng RAII; raw pointer được xác định non-owning. **Ước lượng:** 75 phút.
- [ ] **S7-TV1-16 — Review EventBus subscriber growth** — **DoD:** Chuyển state 50 lần không tăng subscribers. **Ước lượng:** 45 phút.
- [ ] **S7-TV1-17 — Review public API không có caller** — **DoD:** API thừa được xóa/private hóa hoặc có extension rationale. **Ước lượng:** 60 phút.
- [ ] **S7-TV1-18 — Review const-correctness** — **DoD:** Query không mutate; không trả mutable collection không cần thiết. **Ước lượng:** 45 phút.
- [ ] **S7-TV1-19 — Review error handling** — Level, asset, audio và save. **DoD:** Lỗi quan trọng không bị bỏ qua âm thầm. **Ước lượng:** 60 phút.
- [ ] **S7-TV1-20 — Review magic numbers còn lại** — **DoD:** Gameplay/render constants có tên và owner rõ. **Ước lượng:** 45 phút.
- [ ] **S7-TV1-21 — Review duplicate logic** — Item collection, block hit, state transition và score update. **DoD:** Mỗi behavior có một source of truth. **Ước lượng:** 60 phút.
- [ ] **S7-TV1-22 — Review documentation consistency** — README, roles, weekly plan, class diagram, patterns và asset docs. **DoD:** Không có nội dung mâu thuẫn. **Ước lượng:** 60 phút.

### C. Release governance

- [ ] **S7-TV1-23 — Tạo release gate dashboard** — **DoD:** Build, test, P0/P1, playthrough, performance và docs có trạng thái hàng ngày. **Ước lượng:** 30 phút.
- [ ] **S7-TV1-24 — Daily cross-module review** — Mỗi ngày review ít nhất một module ngoài TV1. **DoD:** Có comment và verification record. **Ước lượng:** 45 phút/ngày.
- [ ] **S7-TV1-25 — Fresh-clone verification** — **DoD:** Configure/build/test/run không cần sửa path. **Ước lượng:** 60 phút.
- [ ] **S7-TV1-26 — Sprint 7 architecture sign-off** — **DoD:** Không còn architecture issue có khả năng gây crash, data loss hoặc state corruption. **Ước lượng:** 60 phút.

## 6. TV2 — Nhật — Visual Polish và Render Stability

### A. Performance và resource lifetime

- [ ] **S7-TV2-01 — Profile frame time theo render layer** — **DoD:** Có min/average/max cho Level 1–3. **Ước lượng:** 60 phút.
- [ ] **S7-TV2-02 — Xác minh 60 FPS ổn định** — **DoD:** Không spike lặp do asset load/entity render. **Ước lượng:** 45 phút.
- [ ] **S7-TV2-03 — Preload runtime textures** — **DoD:** Không đọc disk trong gameplay frame bình thường. **Ước lượng:** 45 phút.
- [ ] **S7-TV2-04 — Audit texture lifetime qua state reload** — **DoD:** Không có sprite tham chiếu texture đã hủy. **Ước lượng:** 45 phút.
- [ ] **S7-TV2-05 — Render soak 30 phút** — **DoD:** Memory, sprite, animation và particle counts ổn định. **Ước lượng:** 45 phút chạy + 30 phút phân tích.

### B. Z-order và animation polish

- [ ] **S7-TV2-06 — Audit z-order từng entity type** — Coin/item/enemy/projectile/Mario/flag/HUD. **DoD:** Không bị che sai ở cả ba level. **Ước lượng:** 45 phút.
- [ ] **S7-TV2-07 — Polish Mario direction flip** — **DoD:** Không pop position hoặc origin. **Ước lượng:** 30 phút.
- [ ] **S7-TV2-08 — Polish Koopa/shell orientation** — **DoD:** Visual phản ánh đúng direction/state. **Phụ thuộc:** TV4. **Ước lượng:** 30 phút.
- [ ] **S7-TV2-09 — Polish FireBall animation** — **DoD:** Rotation/frame timing ổn định ở hai hướng. **Ước lượng:** 30 phút.
- [ ] **S7-TV2-10 — Polish death animation** — **DoD:** Animation, camera, audio và transition timing khớp. **Ước lượng:** 45 phút.
- [ ] **S7-TV2-11 — Polish spawn animation** — **DoD:** Input chỉ mở sau sequence. **Ước lượng:** 45 phút.
- [ ] **S7-TV2-12 — Polish grow/shrink animation** — **DoD:** Không flicker/lệch chân/sai power frame. **Ước lượng:** 45 phút.
- [ ] **S7-TV2-13 — Polish flag sequence** — **DoD:** Camera, Mario, score và fade đúng thứ tự. **Ước lượng:** 60 phút.

### C. Effects và camera feel

- [ ] **S7-TV2-14 — Enemy defeat particles** — Dùng pool có giới hạn. **DoD:** Particle tự cleanup và không cấp phát vô hạn. **Ước lượng:** 75 phút.
- [ ] **S7-TV2-15 — Brick break particles** — **DoD:** Chỉ spawn khi brick thực sự bị phá. **Ước lượng:** 45 phút.
- [ ] **S7-TV2-16 — Tuning camera shake** — Hit/death/shell có intensity khác nhau. **DoD:** Không vượt bounds hoặc làm khó nhìn. **Ước lượng:** 45 phút.
- [ ] **S7-TV2-17 — Tuning camera dead-zone** — **DoD:** Walk/run/flag sequence đều mượt, không lag khó chịu. **Ước lượng:** 30 phút.

### D. Window và UI stress

- [ ] **S7-TV2-18 — Kiểm tra pause render freeze** — **DoD:** Animation world không chạy sau overlay. **Ước lượng:** 30 phút.
- [ ] **S7-TV2-19 — Xử lý focus lost/gained** — **DoD:** Clear held input hoặc auto-pause; Mario không chạy kẹt. **Ước lượng:** 45 phút.
- [ ] **S7-TV2-20 — Extended resolution matrix** — Window/fullscreen/DPI/4:3/ultrawide. **DoD:** Không stretch/cắt HUD/sai click mapping. **Ước lượng:** 75 phút.
- [ ] **S7-TV2-21 — Transition input spam** — Spam Enter/Escape/click. **DoD:** Không tạo duplicate states/overlays. **Ước lượng:** 45 phút.
- [ ] **S7-TV2-22 — Visual regression baseline** — Ảnh đầu/giữa/cuối mỗi level và bốn UI states. **DoD:** Có checklist so sánh trước RC. **Ước lượng:** 60 phút.
- [ ] **S7-TV2-23 — Visual bug closure pass** — **DoD:** Mọi visual P1/P2 có evidence trước/sau. **Ước lượng:** 60 phút.

## 7. TV3 — Bảo — Physics Edge Cases và Gameplay Feel

### A. Corner, wall và platform edges

- [ ] **S7-TV3-01 — Test Mario kẹt góc** — **DoD:** Chạy/nhảy vào mọi corner không kẹt vĩnh viễn. **Ước lượng:** 45 phút.
- [ ] **S7-TV3-02 — Test thin-wall clipping** — Mario, shell, FireBall ở tốc độ tối đa. **DoD:** Không xuyên wall. **Ước lượng:** 60 phút.
- [ ] **S7-TV3-03 — Test ceiling collision khi grow** — **DoD:** Không rung/lún/mất grounded state. **Ước lượng:** 45 phút.
- [ ] **S7-TV3-04 — Test platform-edge grounding** — **DoD:** Không grounded khi chỉ còn overlap không hợp lệ. **Ước lượng:** 45 phút.
- [ ] **S7-TV3-05 — Test landing đúng frame nhấn jump** — **DoD:** Không mất jump hoặc double jump. **Ước lượng:** 45 phút.

### B. Simultaneous contacts

- [ ] **S7-TV3-06 — Mario chạm hai enemies cùng frame** — **DoD:** Chỉ nhận damage theo grace policy một lần. **Ước lượng:** 45 phút.
- [ ] **S7-TV3-07 — Enemy và item cùng frame** — **DoD:** Thứ tự xử lý deterministic và đúng product rule. **Ước lượng:** 45 phút.
- [ ] **S7-TV3-08 — Stomp tại cạnh hitbox** — **DoD:** Không phụ thuộc fixture order; tolerance có test. **Ước lượng:** 60 phút.
- [ ] **S7-TV3-09 — Shell chạm nhiều enemies cùng frame** — **DoD:** Mỗi enemy nhận một defeat; score không double. **Ước lượng:** 45 phút.
- [ ] **S7-TV3-10 — Star hết hạn trong contact frame** — **DoD:** Kết quả deterministic, không vừa kill vừa damage. **Ước lượng:** 60 phút.
- [ ] **S7-TV3-11 — Death đúng frame level completion** — **DoD:** Chỉ một terminal state được commit. **Ước lượng:** 45 phút.

### C. High-speed và frame-time stress

- [ ] **S7-TV3-12 — Rapid direction changes** — **DoD:** Không vượt max speed hoặc sai animation direction. **Ước lượng:** 30 phút.
- [ ] **S7-TV3-13 — Frame-time spike test** — **DoD:** Không spiral, tunnel hoặc teleport. **Ước lượng:** 45 phút.
- [ ] **S7-TV3-14 — FireBall tại screen edge** — **DoD:** Cleanup đúng, không access body đã hủy. **Ước lượng:** 45 phút.
- [ ] **S7-TV3-15 — FireBall chạm nhiều targets** — **DoD:** Chỉ defeat target hợp lệ theo projectile policy. **Ước lượng:** 45 phút.
- [ ] **S7-TV3-16 — Shell high-speed tunneling test** — **DoD:** Không xuyên enemy/wall. **Ước lượng:** 60 phút.

### D. Leak, determinism và tuning

- [ ] **S7-TV3-17 — Box2D body-count audit** — Reload/respawn 100 lần. **DoD:** Body count trở về baseline. **Ước lượng:** 60 phút.
- [ ] **S7-TV3-18 — Contact callback-count audit** — **DoD:** Không tăng listener/contact handler theo reload. **Ước lượng:** 45 phút.
- [ ] **S7-TV3-19 — Tuning walk/run acceleration** — **DoD:** Chuyển hướng có quán tính hợp lý, không trượt khó kiểm soát. **Ước lượng:** 45 phút.
- [ ] **S7-TV3-20 — Tuning jump force** — **DoD:** Required gaps có safety margin, không frame-perfect. **Phụ thuộc:** TV4 gap metrics. **Ước lượng:** 45 phút.
- [ ] **S7-TV3-21 — Tuning bounce forces** — Stomp và FireBall. **DoD:** Không tạo velocity vượt giới hạn. **Ước lượng:** 30 phút.
- [ ] **S7-TV3-22 — Physics deterministic soak** — Cùng scripted inputs chạy nhiều lần. **DoD:** Kết quả nằm trong tolerance. **Ước lượng:** 75 phút.
- [ ] **S7-TV3-23 — Debug hitbox audit** — Mario states, enemy, shell, item, FireBall. **DoD:** Hitbox khớp visual và documented. **Ước lượng:** 60 phút.
- [ ] **S7-TV3-24 — Zero-known-collision-glitch gate** — **DoD:** Regression và exploratory checklist pass. **Ước lượng:** 60 phút.

## 8. TV4 — Vy — Enemy AI, Level Balance và Save Stress

### A. AI polish

- [ ] **S7-TV4-01 — Goomba awareness range** — Tăng patrol speed khi Mario trong vùng hợp lý. **DoD:** Không activate toàn map. **Ước lượng:** 45 phút.
- [ ] **S7-TV4-02 — Goomba wall/ledge soak** — **DoD:** Patrol 10 phút không kẹt/rơi sai. **Ước lượng:** 30 phút chạy + 30 phút phân tích.
- [ ] **S7-TV4-03 — Koopa awareness behavior** — **DoD:** Không tạo difficulty spike unfair. **Ước lượng:** 45 phút.
- [ ] **S7-TV4-04 — Shell bounce consistency** — **DoD:** Mọi wall type phản xạ cùng rule. **Ước lượng:** 45 phút.
- [ ] **S7-TV4-05 — Shell multi-enemy interaction** — **DoD:** Score/event/death đúng qua chuỗi enemies. **Ước lượng:** 45 phút.
- [ ] **S7-TV4-06 — Enemy activation reset** — Quay camera lại sau khi enemy đã active. **DoD:** Không duplicate/reset sai. **Ước lượng:** 45 phút.
- [ ] **S7-TV4-07 — Enemy patrol bounds visualization** — **DoD:** Debug mode hiển thị activation/patrol ranges. **Phụ thuộc:** TV2 debug rendering. **Ước lượng:** 45 phút.

### B. Level 1 balance

- [ ] **S7-TV4-08 — Beginner onboarding review** — **DoD:** Có safe introduction trước mechanic mới. **Ước lượng:** 45 phút.
- [ ] **S7-TV4-09 — Coin path review** — **DoD:** Coin hướng dẫn movement/jump, không đặt ngẫu nhiên. **Ước lượng:** 45 phút.
- [ ] **S7-TV4-10 — Goomba placement review** — **DoD:** Người chơi nhìn thấy và có thời gian phản ứng. **Ước lượng:** 30 phút.
- [ ] **S7-TV4-11 — Level 1 timer budget** — **DoD:** Beginner route có buffer ít nhất 30% thời gian. **Ước lượng:** 30 phút.

### C. Level 2 balance

- [ ] **S7-TV4-12 — Koopa learning space** — **DoD:** Có vùng an toàn để học stomp/kick shell. **Ước lượng:** 45 phút.
- [ ] **S7-TV4-13 — Gap/landing visibility review** — **DoD:** Enemy không che landing zone bắt buộc. **Ước lượng:** 45 phút.
- [ ] **S7-TV4-14 — Pipe geometry review** — **DoD:** Pipe visuals/colliders không tạo invisible snag. **Ước lượng:** 30 phút.
- [ ] **S7-TV4-15 — Level 2 timer budget** — **DoD:** Normal route có buffer, không bắt speedrun. **Ước lượng:** 30 phút.

### D. Level 3 balance

- [ ] **S7-TV4-16 — Mixed-enemy fairness** — **DoD:** Không có unavoidable damage. **Ước lượng:** 60 phút.
- [ ] **S7-TV4-17 — Star placement review** — **DoD:** Star xuất hiện trước đoạn có giá trị và không bắt buộc để sống. **Ước lượng:** 30 phút.
- [ ] **S7-TV4-18 — Final gauntlet review** — **DoD:** Khó hơn nhưng có readable enemy/platform pattern. **Ước lượng:** 60 phút.
- [ ] **S7-TV4-19 — Level 3 timer pressure** — **DoD:** Average player còn buffer hợp lý. **Ước lượng:** 30 phút.

### E. Cross-level và save validation

- [ ] **S7-TV4-20 — Power-up placement matrix** — **DoD:** Mushroom/Flower/Star xuất hiện có chủ đích, không quá dày. **Ước lượng:** 45 phút.
- [ ] **S7-TV4-21 — Pit readability review** — **DoD:** Mép platform và landing surface rõ. **Ước lượng:** 45 phút.
- [ ] **S7-TV4-22 — Flag approach review** — **DoD:** Không unfair death trong forced flag sequence. **Ước lượng:** 30 phút.
- [ ] **S7-TV4-23 — Save data 100-cycle stress** — **DoD:** Không corrupt/giảm high score/mất volume. **Ước lượng:** 60 phút.
- [ ] **S7-TV4-24 — Mười playthrough mỗi level** — Normal, no-damage, speed route, intentional death. **DoD:** Có log time/death/bug cho từng run. **Ước lượng:** 240 phút.
- [ ] **S7-TV4-25 — Final level-data validation** — **DoD:** Ba level pass validator; không có unsupported spawn symbol. **Ước lượng:** 30 phút.

## 9. TV5 — Truyền — Audio, HUD, Items và Asset Packaging

### A. Audio coverage

- [ ] **S7-TV5-01 — Event-to-audio coverage matrix** — **DoD:** Mỗi event có SFX/music hoặc “no sound by design”. **Ước lượng:** 45 phút.
- [ ] **S7-TV5-02 — Rapid coin audio test** — **DoD:** Không cắt hoặc vượt voice pool. **Ước lượng:** 30 phút.
- [ ] **S7-TV5-03 — Rapid stomp/shell audio test** — **DoD:** Không phát lặp do contact kéo dài. **Ước lượng:** 30 phút.
- [ ] **S7-TV5-04 — FireBall audio rate test** — **DoD:** Chỉ phát khi projectile được tạo. **Ước lượng:** 20 phút.
- [ ] **S7-TV5-05 — Power-up duplication test** — **DoD:** Mỗi pickup đúng một SFX. **Ước lượng:** 30 phút.
- [ ] **S7-TV5-06 — Music transition matrix** — Menu, Level 1–3, Star, death, Win, GameOver. **DoD:** Không overlap hoặc im lặng sai. **Ước lượng:** 60 phút.
- [ ] **S7-TV5-07 — Star interruption test** — Pause/death/level complete khi Star active. **DoD:** Chuyển/khôi phục track đúng. **Ước lượng:** 45 phút.
- [ ] **S7-TV5-08 — Audio device/load failure** — **DoD:** Thiếu audio không crash; log asset lỗi. **Ước lượng:** 45 phút.
- [ ] **S7-TV5-09 — Volume boundary test** — **DoD:** Negative, >100 và save sai đều clamp. **Ước lượng:** 30 phút.
- [ ] **S7-TV5-10 — Audio memory soak 30 phút** — **DoD:** Voice/music/buffer count không tăng vô hạn. **Ước lượng:** 60 phút.

### B. HUD và item edge cases

- [ ] **S7-TV5-11 — HUD resolution audit** — **DoD:** Score/coin/world/time/lives/power không overlap. **Ước lượng:** 60 phút.
- [ ] **S7-TV5-12 — HUD time-warning polish** — **DoD:** Rõ ràng, không flash hoặc phát event lặp sai. **Ước lượng:** 30 phút.
- [ ] **S7-TV5-13 — Collect item khi Mario chết** — **DoD:** Không vừa power-up vừa GameOver/double score. **Ước lượng:** 45 phút.
- [ ] **S7-TV5-14 — QuestionBlock rapid-hit test** — **DoD:** Chỉ spawn một content. **Ước lượng:** 45 phút.
- [ ] **S7-TV5-15 — Simultaneous coin/power-up test** — **DoD:** Counters/events deterministic. **Ước lượng:** 30 phút.
- [ ] **S7-TV5-16 — Multi-100-coin threshold test** — **DoD:** Lives và coin remainder chính xác. **Ước lượng:** 30 phút.
- [ ] **S7-TV5-17 — Star re-collection policy test** — **DoD:** Timer extend/reset behavior nhất quán và documented. **Ước lượng:** 30 phút.
- [ ] **S7-TV5-18 — Item out-of-bounds soak** — **DoD:** Không tăng active item count sau nhiều block hits. **Ước lượng:** 45 phút.

### C. Assets và packaging readiness

- [ ] **S7-TV5-19 — Runtime package audit** — **DoD:** Chỉ chứa runtime assets cần thiết; mọi path tương đối. **Ước lượng:** 60 phút.
- [ ] **S7-TV5-20 — Asset source/license audit** — **DoD:** Source/reference/license được ghi rõ, không trộn runtime manifest. **Ước lượng:** 60 phút.
- [ ] **S7-TV5-21 — Missing-asset simulation** — **DoD:** Texture/font/audio thiếu có log và fallback/controlled failure. **Ước lượng:** 45 phút.
- [ ] **S7-TV5-22 — Case-sensitive path audit** — **DoD:** Asset paths khớp đúng chữ hoa/thường để portable sang hệ thống khác. **Ước lượng:** 45 phút.
- [ ] **S7-TV5-23 — Controls/HUD/audio documentation** — **DoD:** README và asset docs khớp runtime. **Ước lượng:** 45 phút.
- [ ] **S7-TV5-24 — Final audio/HUD/item sign-off** — **DoD:** Không còn P0/P1, P2 có waiver hoặc fix. **Ước lượng:** 60 phút.

## 10. Lịch Sprint 7

### 09/08 — Pattern và regression baseline

- Chạy toàn bộ tests.
- Chụp visual baseline.
- Ghi frame time, memory, Box2D body count và subscriber count baseline.
- TV1 khóa cấu trúc tài liệu design patterns.

### 10/08 — Physics và render edge cases

- Corner, wall, ceiling, simultaneous contact, frame spike.
- Resolution, focus, input spam và resource lifetime.
- P0/P1 phát hiện trong ngày phải được triage ngay.

### 11/08 — AI và level balance

- Enemy activation, patrol, ledge detection và shell interactions.
- Review gap, coin, power-up, enemy placement và timer của cả ba level.

### 12/08 — Audio, HUD và state stress

- Music transition matrix.
- Pause/state spam.
- Item/death/transition simultaneous cases.
- Save/high-score repeated cycles.

### 13/08 — Soak và clean-build verification

- Physics/render/audio soak 30 phút.
- 100 reload/respawn/state/save cycles.
- Clean configure/build/test/run.

### 14/08 — Full regression

- Mỗi thành viên chơi cả ba level.
- TV4 hoàn tất 10 playthrough cho mỗi level.
- Chỉ còn P2 có waiver bằng văn bản.

### 15/08 — Release candidate

- Debug/Release build sạch.
- Automated tests pass.
- Menu→Win và GameOver routes pass.
- Năm patterns được tài liệu hóa đầy đủ.
- Không còn P0/P1.
- Chốt candidate cho Sprint 8 packaging/demo.

## 11. Full regression matrix

### State và progress

- New Game → Level 1 → Level 2 → Level 3 → Win.
- New Game → death → respawn → complete level.
- Hết lives → GameOver → Retry.
- GameOver → Menu → New Game.
- Pause/resume tại từng level và khi Star đang active.
- Spam Enter/Escape/click trong transition.
- Chết đúng frame chạm flag.
- Save/restart/high-score/volume verification.

### Mario và physics

- Walk, run, reverse direction.
- Tap/hold jump.
- Jump tại platform edge.
- Growth dưới trần thấp.
- FIRE→SUPER→SMALL→death.
- Damage grace khi chạm nhiều enemies.
- Star hết hạn đúng contact frame.
- Frame-time spike và focus loss.

### Enemy và projectiles

- Goomba wall/ledge patrol.
- Koopa walking/shell idle/shell moving.
- Shell bounce và multi-kill.
- FireBall cooldown, max count, bounce, kill và bounds cleanup.
- Enemy activation khi camera đến gần.
- Quay camera lại sau khi enemy đã active.

### Items và tiles

- Coin, Mushroom, FireFlower, Star và 1-Up.
- Adaptive QuestionBlock.
- Rapid multi-hit QuestionBlock.
- Ground không break, brick break đúng power.
- 100 coin và nhiều mốc 100 coin.
- Item rơi xuống pit và cleanup.

### UI, camera và audio

- Camera đầu/giữa/cuối ba level.
- Resize/fullscreen/4:3/DPI.
- Menu/Pause/GameOver/Win keyboard và mouse.
- HUD không overlap.
- Music Level 1/2/3, Star, death, GameOver, Win.
- Rapid SFX và volume limits.
- Missing audio/texture/font simulation.

## 12. Release gates

Không chốt Sprint 7 release candidate nếu còn bất kỳ điều kiện nào:

- Có P0 hoặc P1 mở.
- Có automated test fail.
- Debug hoặc Release build có error/warning production.
- Có level không thể hoàn thành bằng input bình thường.
- Có crash, Box2D locked-world violation, dangling observer hoặc corrupt save.
- Camera nhìn ra ngoài world ở cửa sổ mặc định.
- Pause làm mất music, timer hoặc input state.
- Score/lives reset qua level.
- Enemy chết để lại invisible collider.
- Runtime package thiếu asset hoặc phụ thuộc absolute path.
- Documentation nhắc tới file/symbol không tồn tại.
- Soak test cho thấy memory, body, subscriber, voice hoặc particle count tăng không giới hạn.

## 13. Ngoài phạm vi Sprint 7

- Luigi/multiple players.
- Level editor.
- Bowser boss.
- Underwater/swimming mechanics.
- Multiplayer/networking.
- 3D rendering.
- Rewrite toàn bộ EventBus/state/physics architecture khi implementation hiện tại có thể ổn định bằng thay đổi nhỏ.

Các hạng mục trên chỉ được xem xét ở Sprint 8 buffer nếu release candidate đã đạt toàn bộ gates.

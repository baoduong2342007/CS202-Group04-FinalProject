# Sprint 7 — Kế hoạch Release Candidate trong 5 ngày làm việc

**Thời gian/capacity:** 11/08/2026–15/08/2026, đúng năm ngày làm việc (Day 1–Day 5).<br>
**Phạm vi:** hoàn thiện release graph đã được phê duyệt: **Level 1 Overworld → Level 2 Underground → Level 3 Underwater → Level 4 Castle → Win**.<br>
**Đầu ra:** một release candidate (RC) có thể tải, chơi và hoàn thành cả bốn level bằng input bình thường, với bằng chứng automated và manual cùng một commit.<br>
**Trạng thái tài liệu:** đây là kế hoạch; không mục nào dưới đây tự tuyên bố implementation, test, playthrough hoặc audit đã `DONE/PASS`.

Sprint 6 hiện là baseline kỹ thuật, không phải bằng chứng đóng Sprint 7. Catalog/test hiện tại vẫn mô tả ba entry và level4 là fixture tham chiếu; việc đổi contract sang bốn entry là **work mới của Sprint 7**, cần TV1 phê duyệt và test tương ứng. Underwater runtime hooks đã tồn tại trong `Level.cpp`/`Mario`, nhưng phải được kiểm chứng cùng map, input và collision mới. Không thêm symbol hoặc mechanic ngoài contract validator hiện có chỉ để làm đẹp kế hoạch.

## 1. Release contract, ưu tiên và nguyên tắc làm việc

### 1.1 Graph và boundary

| Release number | World label dự kiến | Theme | Vai trò trong Sprint 7 |
|---:|---|---|---|
| 1 | `1-1` | Overworld | Regression baseline; không đổi theme nếu không có P0/P1 |
| 2 | `1-2` | Underground | Regression baseline; giữ completion route |
| 3 | `1-3` | Underwater | Map mới/được remap, swim/physics và balance là critical path |
| 4 | `1-4` | Castle | Map mới/được remap, castle balance và final route là critical path |
| — | — | Win | Level 4 completion phải đưa tới Win đúng một lần |

Graph trên là quyết định planning đã được duyệt. Public contract trong source, save format/bounds, progression state và test assertions chỉ được thay đổi sau review của TV1 cùng owner module liên quan. Kế hoạch không coi việc catalog hiện tại trả về ba entry hoặc test đang kỳ vọng `find(4) == nullptr` là lỗi đã sửa.

### 1.2 Priority rules

- **P0 — release blocker:** crash, corrupt/mất save, state kẹt, không load/complete được level, mất progress hoặc đường tắt làm hỏng graph. Dừng polish để xử lý và retest.
- **P1 — release critical:** collision/swim sai làm route không công bằng, audio/state/HUD sai rõ ràng, map invalid, test/build fail, hoặc thiếu bằng chứng bắt buộc. Phải có owner và kế hoạch sửa trong cùng sprint.
- **P2 — quality:** visual, audio, camera, balance hoặc tài liệu không chặn completion. Chỉ xử lý sau P0/P1 và phải có verification.
- **P3 — defer:** polish không ảnh hưởng RC, refactor sở thích cá nhân và improvement không cần cho bốn-level route. Ghi rõ defer, không lẫn vào gate.

Mọi task phải có owner, dependency, DoD và evidence path. Khi implementation làm thay đổi contract, TV1 là người quyết định tiếp tục, rollback hoặc ghi waiver; không dùng tài liệu Sprint 7 để tự phê duyệt API.

### 1.3 Design-pattern và release governance

TV1 vẫn duy trì inventory/reference của Factory, Singleton, Observer, State và Command ở mức đủ để tài liệu khớp code thực tế. Đây là P2 nếu không ảnh hưởng build, state, save hoặc completion; không được chiếm capacity của game-completion path. Daily gate, commit/hash, test total, manual evidence và waiver là release governance bắt buộc, không phải mục polish tùy chọn.

## 2. Sprint 6 carry-over audit (giữ nguyên ID và sự thật trạng thái)

Các dòng sau đều vẫn là `REVIEW` theo `docs/management/S6_AUDIT_TRACKER.md` và `docs/management/S6_BUG_REGISTER.md`. Sprint 7 chỉ tạo hành động đóng evidence hoặc disposition; không đổi ID, không gọi chúng là `DONE/PASS` trước khi evidence được tạo.

| Original ID | Status hiện tại | Evidence gap phải đóng | Ngày / owner / action trong Sprint 7 |
|---|---|---|---|
| `S6-TV1-35` | `REVIEW` | Integration log mới nhất còn thiếu commit hash và manual Level 1/item-route trên final RC; các hàng lịch sử không đủ. | Day 1 TV1 lập evidence contract; Day 5 TV1 ghi hash, Debug/Release/test result và disposition. |
| `S6-TV2-28` | `REVIEW` | Resolution matrix có automated coverage nhưng còn thiếu screenshots và interactive UI run trên candidate cần chốt. | Day 4 TV2 chạy matrix, chụp ảnh cùng hash; Day 5 TV1 kiểm tra tính nhất quán. |
| `S6-TV4-18` | `REVIEW` | Fairness của **Castle khi còn mang identity Level 3 cũ** chưa có same-RC human evidence. Việc remap graph làm Castle thành Level 4 không tự thỏa task cũ. | Day 1 TV1 ghi disposition `REVIEW/superseded-pending-disposition`; Day 3–4 TV4 chạy acceptance riêng cho Level 4 Castle. |
| `S6-TV4-40` | `REVIEW` | Năm playthrough cho mỗi level của S6 và 15 same-RC entries chưa được chạy/ghi. | Day 4 TV4 thu log theo final candidate; Day 5 TV1 kiểm tra hash và route. Bốn-level playthrough của S7 là evidence bổ sung. |
| `S6-TV5-43` | `REVIEW` | TV5 commit report chưa được review/đưa đúng vào engineering docs; nội dung sai phải loại bỏ. | Day 1 TV5 audit tài liệu; Day 5 TV1/TV5 ghi source và disposition, không sửa lịch sử S6. |
| `S6-TV5-44` | `REVIEW` | Audio/HUD/item integration cần checklist tái lập và audio-device result trên final RC. | Day 4 TV5 chạy device/audio/HUD/item checks; Day 5 đóng bằng log cùng hash hoặc giữ `REVIEW`. |
| `BUG-026` | `REVIEW` | Tracker còn cần suite count và final build/hash row chính xác. | Day 1 TV1 dùng baseline **19 suite đang đăng ký**; Day 5 cập nhật theo CMake/CTest thực tế, không dùng count cũ. |
| `BUG-038` | `REVIEW` | Các manual `PASS` lịch sử đã bị invalidated; cần rerun playthrough, screenshots và audio trên final RC. | Day 4 các owner tạo evidence; Day 5 TV1 chỉ accept khi tất cả artifact cùng candidate hash. |

### Same-final-RC evidence rule

Evidence đóng carry-over và release gate phải được tạo từ **cùng một final RC commit** chứa toàn bộ thay đổi bốn-level. Build Debug, build Release, test output, playthrough log, screenshots và audio-device result phải ghi candidate hash hoặc artifact manifest tương ứng. Evidence của commit cũ, worktree khác hash, hoặc ảnh/audio không truy được candidate chỉ là historical/context và không được dùng để tuyên bố `PASS`. Nếu code/map/assets đổi sau khi thu evidence, chạy lại phần bị ảnh hưởng.

## 3. Work packages và Definition of Done theo owner

### 3.1 TV1 — Dương: contract, progression, save bounds và release approval

| Task | Priority / dependency | DoD và evidence |
|---|---|---|
| `S7-TV1-01` Baseline và contract review | P0; Day 1; không phụ thuộc | Ghi graph 1→2→3→4→Win, catalog hiện trạng, save/state assumptions, 19-suite baseline và carry-over disposition. TV1 phê duyệt interface trước implementation. |
| `S7-TV1-02` Catalog 1..4 và metadata | P0; sau `S7-TV1-01`, phối hợp TV4/TV5 | Contract map đúng Overworld/Underground/Underwater/Castle, world labels, camera và music metadata; `Level 4` không còn là fixture ngoài catalog sau khi code được duyệt. Có diff/test evidence, không chỉ sửa comment. |
| `S7-TV1-03` Progression, save-level bounds và Win | P0; sau catalog | Normal completion tăng 1→4, save/reload không lùi hoặc nhảy ngoài bounds, Level 4 phát Win đúng một lần, Retry/GameOver không làm mất progress. Có state/save regression evidence. |
| `S7-TV1-04` LevelCatalog/state tests và integration gate | P0/P1; song song Day 2–5 | Cập nhật assertions từ contract cũ sang bốn entry; thêm test cần thiết cho `find`, metadata, transitions và bounds. Chạy tất cả suite hiện hành; cập nhật tracker chỉ bằng kết quả thật. |
| `S7-TV1-05` RC governance và design-pattern consistency | P1/P2; sau integration | Daily gate có owner/status/hash; tài liệu pattern, README, roles, plan và API references không mâu thuẫn. Không broad rewrite. |

### 3.2 TV2 — Nhật: Underwater/Castle visuals, camera và screenshots

| Task | Priority / dependency | DoD và evidence |
|---|---|---|
| `S7-TV2-01` Theme render audit | P1; sau metadata của TV1 | Underwater và Castle có background/tiles/flag/entity presentation đúng theme từ asset hiện có hoặc asset được package hợp lệ; không thêm mechanic không có contract. |
| `S7-TV2-02` Camera và viewport bốn level | P1; sau map bounds của TV4 | Camera theo đúng mode, không lộ ngoài world, không cắt flag/HUD ở các resolution mục tiêu; có focused check và manual notes. |
| `S7-TV2-03` Visual regression matrix | P1/P2; sau map load | Ảnh đầu/giữa/cuối của bốn level và Menu/Pause/GameOver/Win; z-order Mario/entity/flag/HUD rõ; diff checklist chỉ ra issue hoặc accepted variance. |
| `S7-TV2-04` Resolution/UI carry-over closure | P1; Day 4–5 | Chạy `S6-TV2-28` matrix, screenshots cùng final hash, kiểm tra input target/focus; lỗi P1 sửa hoặc giữ gate đỏ. |
| `S7-TV2-05` Final visual sign-off | P1; sau TV1/TV4 route | Gói screenshot có world label, theme, camera và completion frame cho Level 1→4→Win; artifact manifest ghi candidate hash. |

### 3.3 TV3 — Bảo: swim/physics/collision và gameplay audit

| Task | Priority / dependency | DoD và evidence |
|---|---|---|
| `S7-TV3-01` Underwater swim physics audit | P0/P1; Day 1–2, sau map theme | Kiểm chứng `setUnderwater`, gravity/damping, vertical swim intent, movement cap và animation/input path bằng code/test/manual evidence; không giả định phím hoặc mechanic chưa có trong runtime. |
| `S7-TV3-02` Underwater collision/contact audit | P0; sau dữ liệu `S7-TV4-UW-01` | Mario không kẹt/xuyên tile, collision với solid/pit/flag deterministic, completion không bị chặn bởi contact order; cập nhật focused regression nếu cần. |
| `S7-TV3-03` Castle và cross-level physics audit | P1; sau `S7-TV4-CA-01` | Castle dùng collision/physics phù hợp theme, không phá behavior Level 1/2; kiểm tra edge, ceiling, enemy/projectile và flag route. |
| `S7-TV3-04` Determinism/soak gate | P1; Day 3–4 | Scripted input lặp lại cho swim, death/respawn, transition và collision; body/contact counts về baseline, không crash hoặc Box2D locked-world violation. |
| `S7-TV3-05` Final gameplay sign-off | P0/P1; Day 5 | TV3 ký checklist normal input cho bốn level, ghi fix commit/test output và các residual P2 (nếu có waiver). |

### 3.4 TV4 — Vy: map design/data/balance — hai nhóm bắt buộc

TV4 là owner duy nhất của **cả Level 3 Underwater và Level 4 Castle**, bao gồm layout, data, balance, fairness, timer, item, enemy và playthrough. TV2/TV3/TV5 hỗ trợ render, physics và audio nhưng không thay quyền sở hữu map acceptance của TV4.

#### Nhóm A — `S7-TV4-UW-*`: Level 3 Underwater

| Task | Priority / dependency | DoD và evidence |
|---|---|---|
| `S7-TV4-UW-01` Underwater map data | P0; Day 1–2; sau `S7-TV1-01` | Map rectangular: mọi row dữ liệu cùng width; mọi symbol thuộc tập được `TileMap` validator hỗ trợ; **chính xác một `M`, một `F`, một `T`**; pole có `T` ngay trên `F`, chuỗi `|` liên tục bên dưới kết thúc trên solid, không có pole rời. `level_validator_tests`/loader pass. |
| `S7-TV4-UW-02` Underwater route và balance | P0/P1; sau TV3 swim audit | Có load route từ catalog tới finish bằng input bình thường; swim space, gravity, enemy/item placement, timer và safety margin công bằng; theme visual/audio được xác nhận với TV2/TV5; không yêu cầu symbol/mechanic mới ngoài runtime. |
| `S7-TV4-UW-03` Underwater playthrough evidence | P1; Day 4–5 | Log normal route, death/respawn và completion (kèm time/death/bug notes), screenshots và candidate hash; một route fail giữ gate đỏ. |

#### Nhóm B — `S7-TV4-CA-*`: Level 4 Castle

| Task | Priority / dependency | DoD và evidence |
|---|---|---|
| `S7-TV4-CA-01` Castle map data | P0; Day 1–2; sau `S7-TV1-01` | Map rectangular, chỉ dùng symbol validator hỗ trợ, chính xác một `M/F/T`, pole `T`/`F`/`|` contiguous và kết thúc trên solid; map load qua catalog, không coi fixture cũ là release evidence. |
| `S7-TV4-CA-02` Castle route và balance | P0/P1; sau TV3 collision audit | Castle gameplay/visual/audio theme rõ; gauntlet, enemy/item, timer, gaps và flag approach readable, không blind landing, frame-perfect jump hoặc unavoidable damage; normal route reaches Win trigger. |
| `S7-TV4-CA-03` Castle playthrough evidence | P1; Day 4–5 | Log normal route, death/respawn, power/item route và final completion; screenshots/audio references cùng final hash. Đây là acceptance mới cho Level 4 và không tự đóng `S6-TV4-18`. |

#### Nhóm C — `S7-TV4-X-01`: regression và balance matrix

TV4 kiểm tra Level 1 Overworld và Level 2 Underground vẫn load/complete, enemy/item/timer không bị thay đổi ngoài ý muốn, save transition không tạo skip. DoD là một matrix bốn level với owner, route, time/death, lỗi và disposition; không dùng cụm “đã pass” nếu chưa có log.

### 3.5 TV5 — Truyền: theme music, HUD, assets và package evidence

| Task | Priority / dependency | DoD và evidence |
|---|---|---|
| `S7-TV5-01` Music mapping bốn level | P1; sau `S7-TV1-02` | Level 1→Overworld, Level 2→Underground, Level 3→Underwater, Level 4→Castle; Star/death/GameOver/Win interruption và resume đúng; asset thiếu phải controlled failure/log, không crash. |
| `S7-TV5-02` HUD world labels và timer | P1; sau catalog metadata | HUD hiển thị đúng `WORLD 1-1` tới `WORLD 1-4`, score/coin/lives/time không overlap và không reset sai khi transition/reload. Có screenshot và focused integration evidence. |
| `S7-TV5-03` Underwater/Castle asset package | P1; phối hợp TV2/TV4 | Runtime package có đúng texture/font/audio/map cần thiết, path tương đối/case đúng, source/license notes riêng; không lẫn asset thử nghiệm vào RC. |
| `S7-TV5-04` Audio/HUD/item verification | P1; sau route và event wiring | Device-audio checklist, one-event/one-SFX, item pickup/death/flag/Win và volume persistence tái lập được; đóng hoặc giữ `S6-TV5-44` theo evidence thật. |
| `S7-TV5-05` Final package/sign-off | P1; Day 5 | Package manifest, screenshots/audio references, `S6-TV5-43/44` disposition và no-P0/P1 report cùng candidate hash. |

## 4. Dependency và integration order

| Bước | Producer → consumer | Điều kiện mở gate |
|---:|---|---|
| 1 | TV1 contract → TV4 map data, TV5 metadata | Graph, numbering, theme/music/world-label mapping và validator rule được ghi; TV1 approval có owner/date. |
| 2 | TV4 map data → TV3/TV2/TV5 | Cả hai map nhóm pass rectangular/symbol/marker/pole checks và loader có route tới finish. |
| 3 | TV3 swim/collision → TV4 balance/playthrough | Underwater và Castle không có P0/P1 physics blocker; input route được kiểm chứng. |
| 4 | TV2 visuals + TV5 audio/HUD/assets → manual RC | Four-level route có đúng theme, camera, world label, music và package paths; missing asset không crash. |
| 5 | All owners → TV1 final gate | Debug/Release/tests/manual artifacts cùng final hash; carry-over chỉ đóng khi đủ evidence. |

Không merge một map/data change sau khi đã chụp evidence mà không rerun phần bị ảnh hưởng. Contract conflict, save migration risk hoặc symbol/mechanic chưa được hỗ trợ phải dừng tại TV1 approval gate và báo parent; không tự mở rộng ownership.

## 5. Lịch năm ngày (mỗi ngày TV1–TV5 đều có deliverable và gate)

| Ngày | TV1 — Dương | TV2 — Nhật | TV3 — Bảo | TV4 — Vy | TV5 — Truyền | Daily gate |
|---|---|---|---|---|---|---|
| **Day 1 — 11/08** | `S7-TV1-01`: baseline 19 suite, graph, carry-over disposition và contract approval. | `S7-TV2-01`: inventory theme/camera assets, visual/screenshot acceptance spec cho Underwater/Castle. | `S7-TV3-01`: audit swim/physics hooks và baseline collision/soak checklist. | `S7-TV4-UW-01` + `S7-TV4-CA-01`: draft cả hai map theo validator/pole contract, xác định route và balance metrics. | `S7-TV5-01/03`: mapping music/HUD/assets và review `S6-TV5-43` source. | Không có graph/symbol assumption chưa owner; baseline và dependency sheet được TV1 ghi, mọi carry-over vẫn `REVIEW`. |
| **Day 2 — 12/08** | `S7-TV1-02/03`: implement catalog/progression/save bounds sau approval; cập nhật focused tests. | `S7-TV2-01/02`: wire theme render/camera và asset paths cho hai map. | `S7-TV3-01/02`: implement/fix swim input, gravity/damping và collision regression. | **TV4** hoàn thiện data `S7-TV4-UW-01`/`S7-TV4-CA-01`, chạy validator/loader, sửa marker/pole/layout lỗi. | `S7-TV5-01/02/03`: wire four-theme music, HUD labels `1-1..1-4`, package assets. | Build và focused tests chạy; cả hai map parse/load; contract diff chưa có approval thì không tiến gate. |
| **Day 3 — 13/08** | `S7-TV1-03/04`: state transition 1→4→Win, save/reload và LevelCatalog assertions. | `S7-TV2-02/03`: integrate camera, flag/HUD z-order, visual pass theo route. | `S7-TV3-02/03/04`: collision/contact, Castle regression, deterministic/soak checks. | `S7-TV4-UW-02` + `S7-TV4-CA-02`: balance swim/gauntlet, timer, enemy/item, fairness; review với TV2/TV3/TV5. | `S7-TV5-01/02/04`: music transitions, HUD timer/world labels, item/audio events trên integrated route. | Four-level route load/complete smoke path; P0/P1 triage có owner; chưa tạo manual PASS nếu chưa final hash. |
| **Day 4 — 14/08** | `S7-TV1-04` + `BUG-026`: full registered-suite run, resolution/manual artifact review, prepare RC candidate. | `S7-TV2-03/04`: resolution matrix và screenshots đầu/giữa/cuối bốn level; đóng evidence `S6-TV2-28` nếu đủ. | `S7-TV3-04`: repeated input, respawn, body/contact, focus/pause và no-regression run. | `S7-TV4-UW-03` + `S7-TV4-CA-03` + `S6-TV4-40`: playthrough logs cho bốn level, fairness evidence và old Castle disposition input. | `S7-TV5-04`: audio-device, HUD/item checklist, package manifest và `S6-TV5-44` evidence. | Freeze candidate hash; mọi screenshot/audio/playthrough từ hash này; BUG-038 evidence packet bắt đầu, P0/P1 không được tồn tại khi rời ngày. |
| **Day 5 — 15/08** | `S7-TV1-04/05`, `S6-TV1-35`, `BUG-026`: final hash, Debug/Release/tests, docs consistency và release decision. | `S7-TV2-05`: final screenshot/camera/theme sign-off; verify no stale world label. | `S7-TV3-05`: final physics/collision/swim sign-off và residual-risk list. | `S7-TV4-X-01`, `S6-TV4-18/40`, `BUG-038`: rerun changed routes, complete four-level evidence and fairness disposition. | `S7-TV5-05`, `S6-TV5-43/44`, `BUG-038`: final audio/HUD/assets/package sign-off cùng hash. | RC chỉ được sign-off khi toàn bộ release gates ở §7 đạt; nếu thiếu evidence thì ghi `REVIEW`, không đổi lịch sử thành `PASS`. |

## 6. Verification và regression matrix

| Area | Check bắt buộc | Primary owner / supporting owners | Artifact |
|---|---|---|---|
| Catalog/progression/save | `find/count` theo contract 1..4; Menu→1→2→3→4→Win; reload/Retry/GameOver không mất hoặc nhảy progress. | TV1 / TV5 | LevelCatalog/state/save test output, candidate hash. |
| Map data/validator | Rectangular rows; supported symbols; đúng một `M`, `F`, `T`; `T` ngay trên `F`; pole `|` contiguous kết thúc trên solid; no disconnected pole; load và finish route. | TV4 / TV1, TV3 | Validator output, map path, loader log, route notes. |
| Underwater movement | Swim input, gravity/damping, horizontal/vertical control, collision với surface/pit/enemy và flag theo runtime behavior thật. | TV3 / TV4 | Focused regression/manual notes; không invent control semantics. |
| Castle gameplay | Standard movement/collision, enemy/item/gauntlet, timer, readable gaps, flag route và transition Win. | TV4 / TV3 | Playthrough time/death/bug log và screenshot. |
| Visual/camera/UI | Theme assets, camera bounds, z-order, resolution/focus matrix, `WORLD 1-1..1-4`, Menu/Pause/GameOver/Win. | TV2 / TV5 | Screenshot set và visual checklist. |
| Audio/assets | Four theme music mapping, Star/death/GameOver/Win, device output, volume persistence, relative/case-sensitive paths, missing-asset behavior. | TV5 / TV2 | Device log, audio matrix, package manifest. |
| Automated/build | All **19 currently registered suites** pass as the current baseline; newly added suites may increase total and must also pass. Debug/Release configure-build clean, no production warnings/errors. | TV1 / all | CTest/CMake output, build directories/artifact manifest. |
| Final manual | Normal-input completion of all four levels, progress preserved 1→4→Win, screenshots/playthrough/audio from one final hash. | TV1 / TV2–TV5 | Signed RC evidence index; unresolved item remains `REVIEW`. |

## 7. Definition of Done và release gates

Sprint 7 RC chỉ đạt DoD khi tất cả điều kiện sau được chứng minh; đây là gates, không phải checkbox để điền trước:

- Level 1 Overworld, Level 2 Underground, Level 3 Underwater và Level 4 Castle đều load và complete bằng input bình thường; Level 4 completion chuyển tới Win đúng một lần.
- Progress được giữ đúng qua 1→2→3→4→Win, save/reload, Retry và GameOver; không có corruption hoặc out-of-bounds level.
- Tất cả **19 suite đang đăng ký hiện tại** pass; đây là current baseline, không phải maximum cố định. Nếu implementation thêm suite, tổng count tăng và toàn bộ suite mới cũng phải pass.
- Debug và Release configure/build/test sạch, không có production error/warning; asset path tương đối và package chạy được từ cấu hình sạch.
- Không còn P0/P1 mở. P2 chỉ được tồn tại nếu TV1 ghi waiver, impact, owner và kế hoạch; P3 được defer rõ ràng.
- Automated evidence, playthrough log, screenshots, device-audio result và build/test output đều truy được cùng final RC commit/hash.
- Có manual playthrough evidence cho route normal và death/respawn/fairness của cả hai map mới; `S6-TV4-18` vẫn có disposition riêng, không bị coi là tự đóng bởi remap.
- HUD world labels/music/theme/camera nhất quán với graph; tài liệu, tracker references và package manifest không nhắc symbol/file/claim không tồn tại.
- Không có crash, Box2D locked-world violation, dangling observer/resource, invisible collider sau death/transition, camera ra ngoài world hoặc memory/body/subscriber/voice/particle count tăng vô hạn trong check phù hợp.

## 8. Explicit defer / out of scope

Các mục sau không được lấy capacity của release graph và không làm thay đổi acceptance:

- Multiplayer/networking và đồng bộ nhiều người chơi.
- Level editor hoặc tool chỉnh map tương tác.
- 3D rendering/gameplay.
- Bowser boss hoặc boss encounter mới.
- Broad architecture rewrite (EventBus/state/physics) khi local fix và regression đủ an toàn.
- Noncritical P3 polish, refactor không phục vụ gate, và content ngoài bốn level đã nêu.

Underwater/swimming **không** nằm trong out-of-scope; đó là critical path của Level 3 và phải đi qua map, physics, visual, audio, balance và evidence gates ở trên.

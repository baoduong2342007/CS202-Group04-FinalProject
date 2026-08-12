# Đánh giá Sprint 6 — vòng 4

## 1. Kết luận

Đánh giá lại toàn bộ 184 task trong [`s6_plan.md`](docs/management/s6_plan.md) tại commit `3047252` — commit hiện tại của `origin/develop` — cho kết quả **144/184 task (78,3%)** đạt mức `Tốt` hoặc `Rất tốt`; còn **40 task phải mở lại**.

So với [`Evaluate_v3.md`](Evaluate_v3.md), số task đạt tăng từ **139 lên 144** (`+5`). Phần tăng chủ yếu đến từ physics accumulator, FireBall limit, shared defeat/score path, cleanup API của TV2 và collision integration tests. Kết quả không tăng nhiều hơn vì branch đồng thời tạo regression lớn: release catalog bị đổi từ ba level thành bốn level, Level 3 bị đổi sang underwater, Win bị dời sau Level 4, adaptive `?` block không còn đúng contract và FireBall SFX bị phát qua hai đường.

Kết quả xác minh kỹ thuật:

- Configure và build tương đương clean Debug, Release và Tests trong ba thư mục mới `build-review-v4-*`: thành công. Nguồn Box2D đã có trong workspace được tái sử dụng để không phụ thuộc network.
- CTest: **14/14 pass**.
- Không có compiler warning từ production code; chỉ có CMake deprecation warning từ dependency Box2D.
- Build lần hai không chạy lại bước đồng bộ asset, nên incremental asset-copy đạt yêu cầu.
- `git diff --check` pass.
- Link checker trên Markdown đã track phát hiện **41 link hỏng**, đều nằm trong `docs/management/TV1_CHANGES_SUMMARY.md` do dùng path tính từ repository root thay vì từ thư mục tài liệu; hai target trong số đó còn không tồn tại.

Các kết quả xanh chưa đủ để đóng Sprint. `level_catalog_tests`, `play_state_tests` và `tv5_integration_tests` hiện bảo vệ contract **bốn level** thay vì contract ba level trong `s6_plan.md`. Test growth gọi `Mario::setMarioState()` trong khi pickup thật gọi `Mario::powerUp()`, nên không phát hiện lỗi clearance/foot anchor của đường runtime.

### Quy ước phân loại

- **Rất tốt:** Đạt toàn bộ DoD, implementation rõ ràng và có bằng chứng build/test mạnh.
- **Tốt:** Đạt DoD; bằng chứng đủ để chấp nhận, dù coverage hoặc polish chưa mạnh bằng nhóm `Rất tốt`.
- **Chưa tốt:** Có implementation đáng kể nhưng vẫn sai một số trường hợp, thiếu tích hợp, coverage hoặc bằng chứng nghiệm thu.
- **Hoàn toàn sai:** Đường chạy chính đi ngược contract/DoD, kể cả khi helper hoặc test bề mặt đang pass.
- **Chưa làm:** Không tìm thấy implementation hay bằng chứng có ý nghĩa.

Task được tính cho owner gốc trong `s6_plan.md`, không theo người tạo commit regression. Một task chỉ được tính đạt DoD khi ở mức `Tốt` hoặc `Rất tốt`.

## 2. Tổng hợp

| Thành viên | Rất tốt | Tốt | Chưa tốt | Hoàn toàn sai | Chưa làm | Đạt DoD |
|---|---:|---:|---:|---:|---:|---:|
| TV1 — Dương | 15 | 12 | 6 | 2 | 0 | 27/35 — 77,1% |
| TV2 — Nhật | 7 | 8 | 9 | 3 | 1 | 15/28 — 53,6% |
| TV3 — Bảo | 13 | 16 | 7 | 1 | 0 | 29/37 — 78,4% |
| TV4 — Vy | 16 | 19 | 4 | 1 | 0 | 35/40 — 87,5% |
| TV5 — Truyền | 19 | 19 | 3 | 3 | 0 | 38/44 — 86,4% |
| **Tổng** | **70** | **74** | **29** | **10** | **1** | **144/184 — 78,3%** |

### Thay đổi so với vòng 3

| Thành viên | Vòng 3 | Vòng 4 | Chênh lệch task đạt |
|---|---:|---:|---:|
| TV1 | 29/35 | 27/35 | -2 |
| TV2 | 13/28 | 15/28 | +2 |
| TV3 | 20/37 | 29/37 | +9 |
| TV4 | 38/40 | 35/40 | -3 |
| TV5 | 39/44 | 38/44 | -1 |
| **Tổng** | **139/184** | **144/184** | **+5** |

TV3 tiến bộ nhiều nhất nhờ bỏ static accumulator, giới hạn hai FireBall, gom defeat theo `DefeatCause`, thêm Star/shell/FireBall runtime path và mở rộng test. TV2 hoàn thành đúng việc xóa API resource/animation thừa. Phần giảm điểm của TV1, TV4 và TV5 chủ yếu là regression release contract cùng tài liệu/bằng chứng không được cập nhật theo HEAD.

## 3. Blocker release còn lại

1. **Release contract ba level bị phá vỡ.** `LevelCatalog` chứa Level 1–4; Level 3 là underwater, Level 4 mới là castle và Win chỉ xảy ra sau Level 4. Điều này đi ngược S6-TV1-05, S6-TV1-14, S6-TV4-17 và S6-TV5-27.
2. **Test gate đang xác nhận contract sai.** `LevelCatalogTests` ghi rõ “exactly four release levels”; `PlayStateTests` mô phỏng Level 4 → Win dù phần mô tả file vẫn nói Level 3 → Win.
3. **Growth runtime vẫn sai.** Mushroom/FireFlower gọi `powerUp()`. Hàm này không defer khi thiếu trần và còn dịch body lên trước khi `rebuildFixture()` dịch thêm lần nữa. Test hiện chỉ gọi `setMarioState()` nên cho false green.
4. **Camera/scaling chưa đạt.** Game vẫn ép cửa sổ 16:9, scale liên tục, không letterbox/integer scale/mouse remap; Camera không có Y policy theo theme và không clamp Y sau shake.
5. **FireBall audio/cooldown contract chưa sạch.** Level vừa notify `FIREBALL_SHOT` vừa gọi `playSound("fireball")`, trong khi SoundManager cũng phát SFX khi nhận event. Queue lúc world locked còn bypass việc consume cooldown và public legacy wrappers vẫn tồn tại.
6. **Adaptive QuestionBlock đi ngược DoD.** Normal `?` block trả Coin 75% thay vì SMALL luôn nhận Mushroom và SUPER/FIRE luôn nhận FireFlower. Level 2 vì thế không có FireFlower route deterministic.
7. **Manual acceptance không đại diện HEAD.** Log TV4 dùng commit `7a7ed97` trên branch khác, trước thay đổi Level 3/4; L2-05 thiếu dòng Result và L2-EX01 còn `NEEDS RECHECK` dù summary ghi 15/15 PASS.
8. **Tài liệu vẫn stale.** Tracker bỏ hẳn Sprint 3, bug register chưa phản ánh trạng thái hiện tại, README thiếu hai suite mới, file structure/class diagram/asset manifest/report mô tả sai code và TV1 summary có 41 link hỏng.

## 4. Phân loại chi tiết theo ID

### TV1 — Dương

- **Rất tốt:** `06, 15, 16, 18, 19, 20, 22, 23, 24, 25, 26, 27, 28, 29, 34`
- **Tốt:** `03, 04, 07, 08, 09, 10, 11, 12, 13, 17, 21, 33`
- **Chưa tốt:** `01, 02, 30, 31, 32, 35`
- **Hoàn toàn sai:** `05, 14`
- **Chưa làm:** không có

| ID | Mức | Lý do mở lại |
|---|---|---|
| 01 | Chưa tốt | Tracker bỏ Sprint 3, còn task ID không tồn tại/sai owner và giữ nhiều trạng thái/blocker từ vòng cũ. |
| 02 | Chưa tốt | Register vẫn đánh dấu accumulator/collision đã sửa là `OPEN`, đánh dấu docs là `FIXED` dù còn sai, đồng thời bỏ sót Level 4 scope regression, duplicate SFX và adaptive block. |
| 05 | Hoàn toàn sai | DoD yêu cầu catalog tập trung đúng ba release level; code và test hiện khóa bốn level. |
| 14 | Hoàn toàn sai | Hoàn thành Level 3 chuyển sang Level 4, không vào Win. Test chỉ Win sau Level 4 và không quan sát transition thật từ flag Level 3. |
| 30 | Chưa tốt | README vẫn liệt kê 12 suite thay vì 14, lặp hai dòng Pause và chưa mô tả release flow hiện tại một cách nhất quán. |
| 31 | Chưa tốt | FILE_STRUCTURE thiếu Level 4/Piranha/Springboard/FireballExplosion/theme headers/tests mới, đồng thời vẫn ghi Level 3 là castle trong khi code dùng underwater. |
| 32 | Chưa tốt | Class diagram còn nhiều API/member cũ của SoundManager, Entity, Level, TextureManager, AnimationSystem, PhysicsEngine và thiếu enum/event/entity/theme mới. |
| 35 | Chưa tốt | Integration log dừng ở 09/08, không ghi chuỗi merge/build/test 10–11/08; TV1 summary còn 41 link tương đối hỏng. |

### TV2 — Nhật

- **Rất tốt:** `03, 04, 12, 13, 14, 25, 26`
- **Tốt:** `01, 02, 09, 15, 17, 18, 21, 24`
- **Chưa tốt:** `06, 07, 08, 16, 19, 20, 22, 23, 27`
- **Hoàn toàn sai:** `05, 10, 11`
- **Chưa làm:** `28`

| ID | Mức | Lý do mở lại |
|---|---|---|
| 05 | Hoàn toàn sai | Camera vẫn thay đổi Y theo target khi Mario lên đủ cao; không có overworld Y-lock policy. |
| 06 | Chưa tốt | Có Y-follow nhưng không có Y clamp hay mode dành riêng cho map cao. |
| 07 | Chưa tốt | Chỉ X được clamp; Y vẫn có thể nhìn ra ngoài world bounds. |
| 08 | Chưa tốt | Shake được áp dụng cả X/Y nhưng bước clamp sau đó chỉ sửa X. |
| 10 | Hoàn toàn sai | Resize handler cưỡng ép kích thước cửa sổ về 16:9 thay vì dùng viewport letterbox. |
| 11 | Hoàn toàn sai | Render texture được scale theo số thực từ chiều rộng cửa sổ; không tính integer scale và không center phần dư. |
| 16 | Chưa tốt | Pause/GameOver/Win dùng helper, nhưng Menu vẫn tự tính layout và không có shared physical-to-logical mouse mapping. |
| 19 | Chưa tốt | GameOver có final score và keyboard/mouse menu, nhưng không render high score. |
| 20 | Chưa tốt | Win có final score và keyboard/mouse menu, nhưng không render high score; final transition hiện còn ở sai Level 4. |
| 22 | Chưa tốt | `AnimationSystem::isFinished()` tồn tại, nhưng PlayState không dùng signal; reload/GameOver vẫn dựa vào timer camera cố định. |
| 23 | Chưa tốt | Runtime `powerUp()` dịch body hai lần khi grow, còn `m_isTransforming` không bao giờ được bật nên transition presentation chưa tồn tại đúng nghĩa. |
| 27 | Chưa tốt | Underground/underwater chỉ dùng màu nền tối; castle rơi vào nhánh dùng lại overworld background, nên ba release theme không có background riêng. |
| 28 | Chưa làm | Không có log/screenshot/test matrix cho 1280×720, 1600×900, 1920×1080 và 4:3. |

### TV3 — Bảo

- **Rất tốt:** `01, 04, 05, 06, 07, 08, 09, 27, 29, 30, 31, 32, 33`
- **Tốt:** `02, 03, 12, 14, 15, 17, 18, 19, 20, 21, 22, 23, 24, 25, 28, 35`
- **Chưa tốt:** `10, 13, 16, 26, 34, 36, 37`
- **Hoàn toàn sai:** `11`
- **Chưa làm:** không có

| ID | Mức | Lý do mở lại |
|---|---|---|
| 10 | Chưa tốt | `powerUp()` tự dịch body lên rồi `rebuildFixture()` lại foot-anchor lần nữa; pickup thật có thể nhấc chân, trong khi test chỉ đi qua `setMarioState()`. |
| 11 | Hoàn toàn sai | Mushroom/FireFlower gọi `powerUp()`, hàm này vẫn đổi state và dựng fixture cao dù thiếu clearance; deferred path chỉ tồn tại trong helper khác. |
| 13 | Chưa tốt | Star kill đã đúng, nhưng `updateInvincibility()` return khi Star active nên damage-grace clock bị freeze nếu hai timer cùng tồn tại. |
| 16 | Chưa tốt | `respawn()` đã reset phần lớn state nhưng còn bỏ sót vertical/climb/flagpole/pending-transform/gravity state; runtime chính lại reload cả Level thay vì kiểm thử method này. |
| 26 | Chưa tốt | Stomp có runtime test, nhưng side-hit riêng và fixture-order A/B chưa có regression test như DoD. |
| 34 | Chưa tốt | `m_boundingBox` cache vẫn tồn tại và được cập nhật, trong khi getter lại tính mới từ position/size; vẫn còn hai representation. |
| 36 | Chưa tốt | Suite tăng mạnh nhưng growth dùng sai entry point, timer không test overlap và death/respawn test chưa bảo vệ completion/full reset. |
| 37 | Chưa tốt | Có Box2D runtime test cho stomp/shell/FireBall/Star, nhưng thiếu side hit và fixture-order matrix. |

### TV4 — Vy

- **Rất tốt:** `01, 02, 03, 04, 05, 06, 07, 12, 20, 21, 22, 29, 32, 34, 38, 39`
- **Tốt:** `08, 09, 10, 11, 13, 14, 16, 23, 24, 25, 26, 27, 28, 30, 31, 33, 35, 36, 37`
- **Chưa tốt:** `15, 18, 19, 40`
- **Hoàn toàn sai:** `17`
- **Chưa làm:** không có

| ID | Mức | Lý do mở lại |
|---|---|---|
| 15 | Chưa tốt | Level 2 vẫn là underground và có Koopa/pipe, nhưng không có FireFlower deterministic: các `?` block có thể trả Coin 75% và map không có explicit FireFlower tile. |
| 17 | Hoàn toàn sai | Level 3 hiện là underwater; castle/gauntlet bị chuyển sang Level 4, trái trực tiếp DoD Level 3 castle/sky. |
| 18 | Chưa tốt | Fairness evidence thuộc commit cũ trước khi Level 3 bị thay hoàn toàn; chưa có nghiệm thu current HEAD. |
| 19 | Chưa tốt | Weighted random `?` làm distribution không còn deterministic theo player state và có thể khiến mechanic dự kiến không xuất hiện trong một run. |
| 40 | Chưa tốt | Log dùng commit cũ/branch khác, header còn placeholder, L2-05 thiếu Result, L2-EX01 còn `NEEDS RECHECK`, nhưng summary vẫn ghi 15/15 PASS. |

### TV5 — Truyền

- **Rất tốt:** `01, 02, 04, 06, 07, 09, 10, 15, 16, 19, 21, 22, 24, 28, 32, 34, 35, 38, 39`
- **Tốt:** `03, 05, 08, 12, 13, 14, 17, 18, 20, 23, 25, 26, 29, 30, 33, 36, 37, 40, 42`
- **Chưa tốt:** `41, 43, 44`
- **Hoàn toàn sai:** `11, 27, 31`
- **Chưa làm:** không có

| ID | Mức | Lý do mở lại |
|---|---|---|
| 11 | Hoàn toàn sai | Normal `?` block trả Coin cho 75/100 roll; DoD yêu cầu SMALL nhận Mushroom và SUPER/FIRE nhận FireFlower, resolve đúng một lần. |
| 27 | Hoàn toàn sai | `LevelCatalog` map Level 3 sang `UNDERWATER`; Castle music chỉ phát ở Level 4. |
| 31 | Hoàn toàn sai | Mỗi shot vừa notify `FIREBALL_SHOT` để SoundManager phát SFX, vừa gọi `playSound("fireball")` trực tiếp, tạo hai lần phát cho một projectile. |
| 41 | Chưa tốt | Manifest ghi Castle = Level 3 và Underwater là Future, trái code hiện tại; mô tả `bg_world` chạy mọi level cũng không đúng render branch. |
| 43 | Chưa tốt | Report còn ở commit `d18fce8 + working tree`, ghi 13/13 và “chưa commit release”; HEAD hiện là `3047252` với 14/14. |
| 44 | Chưa tốt | Runtime tests đã tốt hơn, nhưng đang encode flow bốn level sai contract; manual checklist vẫn mô tả Level 3 castle, chưa có kết quả thực thi và không bắt duplicate FireBall/death SFX. |

## 5. Đối chiếu test với đường runtime

- `level_catalog_tests` và `play_state_tests` pass vì assertion đã đổi sang bốn level; đây là false green so với Sprint 6, không phải bằng chứng S6-TV1-05/14 đạt.
- `testGrowthFootAnchorAndClearance` pass qua `setMarioState()`. Item runtime gọi `powerUp()`, nơi clearance và foot-anchor vẫn sai; test không bảo vệ bug người chơi gặp.
- `testDefeatScoreAndShellEventsThroughCollisionRuntime` đi qua Box2D contact thật và đủ mạnh để chấp nhận shared score/defeat path cho stomp/shell/FireBall/Star.
- `testFireBallActiveLimitOfTwo` đi qua `Level::requestFireBallShot()` và xác nhận active limit, nhưng không kiểm tra world-locked queue, cooldown reservation hoặc số lần SoundManager phát SFX.
- 20 vòng Pause push/pop trong TV5 integration test là bằng chứng tốt cho music lifecycle, nhưng không thay thế camera/resolution/manual input matrix.
- Log playthrough hiện không cùng commit với release candidate và tự mâu thuẫn ở Level 2, nên không thể đóng TV4-40 hay full-loop gate.

## 6. Kết luận cuối

Branch đã cải thiện rõ về combat/physics và automated integration: build sạch tương đương ba cấu hình, 14/14 test pass, FireBall limit và shared defeat path đã tiến bộ thực chất. Dù vậy Sprint 6 vẫn **chưa đạt Definition of Done** vì còn 40 task mở và release flow hiện sai ở cấp contract: Level 3 không còn là final level, test gate bảo vệ bốn level, growth pickup vẫn lỗi, camera/letterbox chưa xong và manual/docs evidence không đại diện HEAD.

Thứ tự sửa nên là: khôi phục contract ba level → sửa growth/FireBall runtime → camera/UI → test gate đúng contract → tài liệu và manual release acceptance. Chi tiết nằm trong [`s6_fix_plan_v4.md`](s6_fix_plan_v4.md).

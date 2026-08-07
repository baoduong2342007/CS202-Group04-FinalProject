# Audit Sprint 6 và kế hoạch hoàn thiện — chỉ TV1

## Kết luận

Theo DoD trong [s6_plan.md](D:/Mario/CS202-Group04-FinalProject/docs/management/s6_plan.md:117), TV1 hiện:

- **11/35 task đạt yêu cầu ở mức code/build:** `05, 06, 07, 08, 09, 15, 20, 22, 24, 25, 26`.
- **21/35 task đã làm nhưng chưa đủ DoD hoặc cần sửa:** `01–04, 10–14, 16–17, 21, 23, 27–28, 30–35`.
- **3/35 task chưa hoàn thành về mặt chức năng:** `18, 19, 29`.

Không đánh giá công việc TV2–TV5; dependency của họ chỉ được nhắc khi đang chặn task TV1.

Kết quả xác minh hiện tại:

- Clean Debug, Release và Tests build đều thành công.
- CTest pass **4/4**.
- Không có compiler warning từ production source.
- Configure vẫn có `CMake Deprecation Warning` từ Box2D.
- `s6_plan.md` hiện là file untracked.
- Không có file tracked nào bị thay đổi trong quá trình audit.

## TV1 đã hoàn thành

- `S6-TV1-05–07`: Có `LevelCatalog` tập trung ba release level, New Game bắt đầu Level 1 và HUD dùng numbering one-based.
- `S6-TV1-08–09`: `GameProgress` sống trong `PlayState`; snapshot được thực hiện trước khi hủy level.
- `S6-TV1-15`: `IGameState` đã có `onPause()`/`onResume()` và `GameManager` gọi đúng khi push/pop.
- `S6-TV1-20`: `EventBus` kiểm tra observer còn đăng ký trước khi callback.
- `S6-TV1-22`: `EntityFactory` và call site đã chuyển sang `std::unique_ptr`.
- `S6-TV1-24–25`: Đã xóa `undo()` và dependency thừa của `PauseCommand`; input tests pass.
- `S6-TV1-26`: `src/demo` không còn bị kéo vào production target; clean build không phát sinh deprecated compiler warning từ demo.

## Đã làm nhưng chưa tốt hoặc chưa đạt DoD

### Quản lý và điều phối — `01–04`

- Audit tracker chưa liệt kê đầy đủ từng deliverable trong `WEEKLY_PLAN.md`; bằng chứng chủ yếu chỉ ghi “Build pass”, thiếu ngày, commit và test cụ thể.
- Bug register có reproduction là `—`, gán `BUG-020` vào task tạo register thay vì task sửa lỗi, và đánh dấu toàn bộ `FIXED` mà không có verification.
- Locked interface chưa có dấu vết sign-off; `SaveData`, `DefeatCause`, Run/Shoot contract chưa xuất hiện trong implementation.
- Dependency board thiếu một số dependency đã ghi trong plan như `S6-TV3-24`, `S6-TV4-08`, `S6-TV4-25`, `S6-TV4-29`.

### Progress và transition — `10–14`

- `S6-TV1-10`: Restore score/coin/lives được, nhưng restore SUPER/FIRE qua `setMarioState()` chỉ đổi state/hitbox, không dựng lại animation clip tương ứng ([PlayState.cpp](D:/Mario/CS202-Group04-FinalProject/src/states/PlayState.cpp:168), [Mario.cpp](D:/Mario/CS202-Group04-FinalProject/src/entities/Mario.cpp:456)).
- `S6-TV1-11`: Initial load failure đang sai thứ tự queue. Constructor `PlayState` queue Menu trước, sau đó caller mới queue failed `PlayState`, nên trạng thái cuối có thể vẫn là PlayState rỗng; ngoài ra chỉ có message ở `stderr`, không có message cho người chơi.
- `S6-TV1-12`: Transition cơ bản đã có, nhưng input vẫn được xử lý trong fade; khi reload fail, code vẫn chuyển sang `FADE_IN` và phát `LEVEL_STARTED`.
- `S6-TV1-13–14`: Guard và Win transition đã được viết, nhưng [PlayStateTests.cpp](D:/Mario/CS202-Group04-FinalProject/tests/PlayStateTests.cpp:14) chỉ kiểm tra `LevelCatalog::isPastFinalLevel()`. Test không tạo `PlayState`, không phát hai terminal event và không kiểm tra WinState/final score thật.

### State machine và Pause — `16–17`

- Deferred operation đã có nhưng [GameManager.cpp](D:/Mario/CS202-Group04-FinalProject/src/core/GameManager.cpp:27) duyệt trực tiếp `m_pendingOps`; lifecycle callback thêm operation mới có thể làm invalid vector iteration.
- Manager chỉ giữ `current/previous`, chưa phải state stack thực sự. `changeState()` khi đang ở overlay có thể hủy state nền mà không gọi `onExit()`.
- Pause dừng update/input và pause music, nhưng chưa có test pause/resume 20 lần, chưa kiểm tra subscriber growth và hiện cũng chưa có timer để xác minh yêu cầu timer.

### Observer và tên pattern — `21, 23`

- Destructor của PlayState/HUD/SoundManager có unsubscribe, nhưng chưa có regression test chuyển state lặp hoặc observer count.
- EntityFactory đã được gọi đúng là Simple Factory trong code mới, nhưng [CODING_RULES.md](D:/Mario/CS202-Group04-FinalProject/CODING_RULES.md:158) vẫn mô tả raw pointer và “Factory Method”, nên tài liệu chưa nhất quán.

### Build — `27–28`

- TLS verification đã bật và không còn tự xóa SFML, nhưng SHA256 archive vẫn được ghi rõ là “intentionally omitted” trong [CMakeLists.txt](D:/Mario/CS202-Group04-FinalProject/CMakeLists.txt:23).
- Ba preset đều clean build thành công, nhưng executable vẫn tên `main`, chưa đạt yêu cầu “tên rõ ràng”; Debug và Tests gần như trùng cấu hình.
- Configure sạch vẫn có warning từ CMakeLists cũ của Box2D.

### Tài liệu và Git metadata — `30–35`

- README có build/run nhưng thiếu hướng dẫn test và controls.
- [FILE_STRUCTURE.md](D:/Mario/CS202-Group04-FinalProject/FILE_STRUCTURE.md:76) vẫn ghi Level 3 “planned”, font “cần bổ sung”, thiếu nhiều file thật như `GameProgress`, `LevelCatalog`, `PlayStateTests`.
- [class_diagram.md](D:/Mario/CS202-Group04-FinalProject/docs/class_diagram.md:1) vẫn mang header “Draft Week 1” và chứa nhiều API sai: EntityFactory trả raw pointer, InputHandler giữ raw command, SoundManager/HUD/PhysicsEngine sai interface, có `Button` không tồn tại.
- Sprint 5 legacy plan đã có banner nhưng vẫn giữ hàng loạt `file:///f:/...`, trái DoD bỏ absolute path.
- `.gitattributes` đã đánh dấu FLAC/PDF binary, nhưng còn **26 `.gitkeep` trong thư mục đã có file**.
- Chưa có log integration review theo từng ngày. Clean build/CTest ngày 06/08 là bằng chứng tốt nhưng chưa đủ đóng task kéo dài cả sprint.
- `TV1_CHANGES_SUMMARY.md` dùng absolute local links, ghi phạm vi 11 commit đã lỗi thời và tuyên bố transition tests mạnh hơn coverage thực tế.

## Chưa hoàn thành

- `S6-TV1-18`: `PLAYER_DIED` hiện chỉ phát khi lives về 0 ([Mario.cpp](D:/Mario/CS202-Group04-FinalProject/src/entities/Mario.cpp:416)); các lần mất mạng trước đó tự respawn và không phát event. Nhánh non-terminal death trong PlayState vì thế gần như không được dùng; chưa có kết quả GameOver riêng theo contract.
- `S6-TV1-19`: Không có `SaveManager`/`SaveData` implementation và Menu/Win/GameOver chưa gọi load/save. TV1 phải ghi task này `BLOCKED` cho đến khi interface do owner cung cấp sẵn sàng.
- `S6-TV1-29`: `CopyAssets ALL` vẫn chạy hai lệnh `copy_directory` trong mọi no-op build ([CMakeLists.txt](D:/Mario/CS202-Group04-FinalProject/CMakeLists.txt:183)); comment “incremental” không phản ánh hành vi thật.

## Thứ tự TV1 cần làm tiếp

1. Sửa `S6-TV1-11/12`: không load level trong constructor; propagate kết quả load, chỉ phát `LEVEL_STARTED` khi thành công, hiển thị lỗi và chặn input toàn bộ transition.
2. Cứng hóa `S6-TV1-16`: dùng state stack; xử lý một bản sao pending queue để operation phát sinh từ lifecycle callback được hoãn sang safe point kế tiếp; bảo đảm mọi state bị bỏ đều nhận `onExit()`.
3. Hoàn thiện `S6-TV1-10/13/14/17/18`: restore đúng animation power state, chuẩn hóa death/GameOver contract và bổ sung test thật cho progress, race, Win và 20 vòng pause.
4. Giữ `S6-TV1-19` ở `BLOCKED`; khi SaveManager sẵn sàng, TV1 chỉ tích hợp load tại Menu và monotonic save tại Win/GameOver, không tạo implementation trùng.
5. Sửa build: thêm SHA256 cho SFML, đổi target `main` thành `SuperMario`, thay `CopyAssets ALL` bằng per-file `copy_if_different` có dependency.
6. Sửa toàn bộ tracker/docs/diagram/absolute links và xóa 26 `.gitkeep` thừa; cập nhật status theo bằng chứng thật thay vì commit title.
7. Mỗi ngày ghi commit HEAD, Debug/Release/Tests result, CTest result, warning và blocker để có bằng chứng cho `S6-TV1-35`.

## Test chấp nhận bắt buộc

- Missing/invalid Level 1 đưa về Menu/error có message; không để failed PlayState và không phát `LEVEL_STARTED`.
- Progress gồm score, coin, lives và FIRE/SUPER visual giữ đúng qua Level 1→2→3.
- Death và completion cùng frame, ở cả hai thứ tự, chỉ tạo một terminal result.
- Mỗi lần mất mạng phát đúng một `PLAYER_DIED`; life trừ đúng một lần; mạng cuối chỉ tạo một GameOver.
- Level 3 chỉ tạo một WinState với final score đúng.
- Pause/resume 20 lần và chuyển state 50 lần không tăng callback.
- Observer bị callback trước unsubscribe/destroy không được gọi tiếp.
- No-op build không thực thi copy toàn bộ assets.
- Clean Debug, Release, Tests build thành công và toàn bộ CTest mới pass.

## Giả định

- Trạng thái được chấm theo code tại `0c86521`, không theo checkbox hoặc chữ `DONE` tự ghi trong tracker.
- Các thay đổi cần thiết trong module TV2/TV3/TV4 chỉ được TV1 thực hiện sau khi có xác nhận owner; nếu chưa có thì task TV1 phải ghi `BLOCKED`.

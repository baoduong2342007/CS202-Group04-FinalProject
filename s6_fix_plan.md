# Kế hoạch hoàn tất Sprint 6

## 1. Nguyên tắc thực hiện

- Sprint 6 chỉ đóng khi toàn bộ task đạt Definition of Done; không chuyển phần còn thiếu sang Sprint 7.
- Giữ nguyên người phụ trách module. Những phần giao nhau phải có thành viên liên quan cùng review.
- Các task đã đạt “Rất tốt/Tốt” được giữ nguyên, chỉ kiểm tra regression.
- Mỗi PR phải ghi task ID, cách kiểm thử và bằng chứng hoàn thành. TV1 chỉ merge khi build và toàn bộ test xanh.
- Thứ tự ưu tiên: lỗi chặn luồng chơi → gameplay → save/UI/audio → tài liệu và nghiệm thu.

## 2. Phân công theo thành viên

### TV1 – Dương: Integration, State và tài liệu

Mở lại: `01, 02, 14, 18, 19, 28, 31, 32, 33, 35`.

- Chuẩn hóa luồng chết cùng TV3:
  - Mỗi lần chết chỉ trừ một mạng và phát đúng một `PLAYER_DIED`.
  - `PlayState` đọc số mạng còn lại: còn mạng thì reload level, hết mạng mới sang GameOver.
  - Mario vẫn hiển thị trong death animation; khóa input và chỉ chuyển state sau khi animation kết thúc.
  - Xử lý dứt điểm race condition giữa chết, hết giờ và chạm cờ.
- Hoàn thiện Win flow sau Level 3, GameOver Retry/Menu và tích hợp tiến độ với `SaveManager`.
- Đổi tên executable từ `main` thành `SuperMario`; đồng bộ preset và hướng dẫn chạy.
- Sửa tracker, bug register, README, file structure và class diagram theo code thực tế.
- Khôi phục tài liệu Sprint 5 vào thư mục archive, thêm nhãn `LEGACY` và loại bỏ đường dẫn tuyệt đối.
- Phối hợp:
  - TV3: death/state contract.
  - TV4: save và level progression.
  - TV2: Pause/GameOver/Win.
  - TV5: input, HUD timeout và audio event.
- Điều kiện đạt: test trực tiếp luồng `Playing → Death → Reload`, `Playing → GameOver`, `Level 3 → Win`; tài liệu không còn claim sai hoặc task DONE thiếu bằng chứng.

### TV2 – Nhật: Display, Camera, Animation và UI

Mở lại: `01–03, 05–08, 10–11, 15–20, 22–28`.

- Chuyển logical canvas về đúng `640×360`; cửa sổ mặc định `1280×720`.
- Render qua viewport letterbox với hệ số integer:
  - `1280×720`: 2×.
  - `1600×900`: 2× và letterbox.
  - `1920×1080`: 3×.
  - Màn hình 4:3 hoặc resize vẫn giữ đúng tỉ lệ, không crop.
- Chuyển tọa độ chuột qua viewport trước khi đưa vào UI/gameplay.
- Hoàn thiện camera:
  - Dead-zone theo trục X.
  - Khóa Y ở level ngang, theo Mario ở level cao.
  - Clamp cả X/Y sau camera shake.
- Đưa toàn bộ HUD và Pause/GameOver/Win về anchor theo `640×360`; không dùng tọa độ cố định vượt canvas.
- Hoàn thiện UI:
  - Pause: Resume, Restart, Menu, Music Volume, SFX Volume.
  - GameOver: Retry bắt đầu session mới từ Level 1; Menu quay về menu chính.
  - Win: hiển thị score/high score và Return Menu.
- Hoàn thiện death/grow/shrink animation; cung cấp tín hiệu animation hoàn thành cho state flow.
- Giữ chân Mario cố định khi đổi kích thước; phối hợp TV3 xử lý fixture và trần thấp.
- Chuẩn hóa render layer; xóa các API texture/animation không còn sử dụng.
- Tạo background riêng cho Overworld, Underground và Castle từ asset hiện có.
- Điều kiện đạt: HUD/UI nhìn thấy đầy đủ ở mọi độ phân giải; camera không lộ ngoài level; animation/state transition không bị nhảy hoặc treo.

### TV3 – Bảo: Physics, Mario, Projectile và Collision

Mở lại: `01–03, 10–11, 13, 15–16, 19, 21, 23–28, 31–37`.

- Chuyển physics accumulator thành state của từng Level/PhysicsEngine instance; reset khi load level và giới hạn tối đa 5 substep/frame.
- Hoàn thiện Run:
  - `Shift` ở chế độ Held.
  - PlayState reset trạng thái chạy trước mỗi lần xử lý input.
  - Mario phân biệt walk/run speed nhưng vẫn dùng acceleration và friction.
- Hoàn thiện Shoot/FireBall:
  - `X` bắn khi Mario ở Fire state.
  - Level sở hữu pool tối đa hai FireBall đang hoạt động.
  - Update, render, bounce, despawn ngoài biên và tái sử dụng pool đầy đủ.
  - Projectile chỉ va chạm terrain/enemy; không gây tác động lên Mario, item hoặc projectile khác.
- Tách riêng:
  - Damage grace timer: chỉ ngăn Mario nhận sát thương liên tiếp.
  - Star timer: làm Mario bất tử và tiêu diệt enemy khi chạm.
- Growth dưới trần thấp: giữ trạng thái nâng cấp chờ và chỉ đổi fixture khi đủ khoảng trống; luôn giữ nguyên vị trí bàn chân.
- Chuẩn hóa collision:
  - `CollisionManager` là nơi duy nhất giải quyết gameplay collision.
  - Mỗi contact pair chỉ xử lý một lần.
  - Koopa không được vừa chuyển thành shell vừa bị kick trong cùng contact.
  - Mỗi enemy chỉ phát một defeat/score event.
- Dùng `DefeatCause` chung: Stomp, Shell, FireBall, Star.
- Phối hợp TV1 sửa death flow, TV4 sửa Koopa shell và TV5 kết nối Run/Shoot/Star.
- Điều kiện đạt: có regression test cho timestep, fixture swap, owner-filter, hai FireBall, Star/damage grace, Koopa double-dispatch và duplicate score.

### TV4 – Vy: Level, Enemy và Save System

Mở lại: `09, 13–22, 24–27, 29, 31–40`.

- Sửa Level 3 để có cả flag top `F` và pole `|`; cả ba level phải qua validator.
- Hoàn thiện validator test cho:
  - Thiếu spawn, flag hoặc pole.
  - Kích thước hàng không đồng nhất.
  - Ký hiệu không hợp lệ.
  - Spawn/goal nằm ngoài biên.
- Hoàn thiện tileset và ký hiệu cho ground, brick, question, used block, pipe, flag và pole; phối hợp TV5 tạo các runtime sprite còn thiếu.
- Cân chỉnh ba level:
  - Level 1: tutorial và cơ chế cơ bản.
  - Level 2: pipe, vertical section và độ khó trung bình.
  - Level 3: castle/final challenge và đường tới cờ hợp lệ.
- Enemy chỉ được kích hoạt khi nằm trong 64 px phía trước viewport; sau khi kích hoạt thì tiếp tục hoạt động. Enemy cách phía sau camera hơn một viewport được cleanup.
- Hoàn thiện Koopa:
  - Ledge detection.
  - Shell fixture neo theo chân.
  - Stomp → shell idle → kicked shell.
  - Va tường đổi hướng; shell hạ enemy khác đúng một lần.
- Cleanup entity khi ra ngoài level với margin 64 px.
- Xây dựng `SaveManager` với `SaveData` hiện có:
  - High score, highest unlocked level, Music Volume và SFX Volume.
  - Versioned save file; đường dẫn có thể inject trong test.
  - Ghi file tạm rồi thay thế an toàn; giữ bản cũ nếu ghi thất bại.
  - File thiếu, hỏng hoặc sai version phải dùng mặc định và không crash.
  - Score thấp hơn không được ghi đè high score.
- Phối hợp TV1 tích hợp state/progress, TV3 kiểm tra shell collision và TV5 lưu volume.
- Điều kiện đạt: validator xanh, save test xanh và ghi đủ 5 lượt playthrough cho mỗi level.

### TV5 – Truyền: Input, Item, HUD, Audio và Asset

Mở lại: `02, 03, 06, 09, 15, 16, 19, 21, 27–29, 31–33, 36, 37, 41, 43`.

- Kết nối Run/Shoot command vào PlayState cùng TV3; input không được buffer xuyên Pause, death animation hoặc state transition.
- Hoàn thiện item:
  - Mushroom/FireFlower đổi form đúng luật.
  - Star kích hoạt đúng Star timer.
  - 1-Up tăng mạng và phát event/SFX.
  - Item ra khỏi block, di chuyển, va chạm và cleanup đúng.
- Chuẩn hóa score:
  - Stomp: 100.
  - Shell, FireBall hoặc Star: 200.
  - Coin/item giữ luật hiện tại nhưng chỉ cộng một lần.
- Kết nối HUD timeout với death flow; callback chỉ được kích hoạt một lần khi timer về 0.
- Hoàn thiện audio:
  - Music theo từng level.
  - Star tạm thay level music và khôi phục đúng track khi hết hiệu lực.
  - Death, GameOver và Win music đúng state.
  - Pause/resume không làm mất track.
  - SFX cho jump, coin, item, block, brick, fireball, shell, 1-Up và flag.
- Cung cấp điều khiển Music/SFX volume cho Pause UI; clamp `0–100%` và lưu qua SaveManager.
- Khôi phục runtime texture Goomba/Koopa và các tile/pipe còn thiếu; cập nhật asset manifest, kích thước và code path đồng nhất.
- Không đưa source/reference asset vào runtime.
- Điều kiện đạt: không còn đường dẫn asset bị thiếu; checklist audio/item/HUD được điền bằng kết quả thực tế; volume còn nguyên sau khi restart game.

## 3. Thứ tự tích hợp

### Gate 1 – Có thể chơi trọn luồng

Thực hiện song song:

- TV1 + TV3: death/respawn/GameOver.
- TV2: logical canvas, viewport và UI visibility.
- TV4: Level 3, validator và enemy asset.
- TV3 + TV5: Run/Shoot/FireBall.
- TV4 + TV1: SaveManager skeleton và state integration.

Gate đạt khi cả ba level load được, Level 3 tới được Win, chết khi còn mạng không sang GameOver, và không còn missing runtime asset.

### Gate 2 – Hoàn thiện gameplay

- Physics, collision, Koopa shell, enemy activation.
- Growth dưới trần thấp, Star/damage grace.
- Item, score, HUD timeout.
- Camera, animation và background theme.
- Save progress, high score và volume.

Gate đạt khi toàn bộ automated regression test của các module đều xanh.

### Gate 3 – Release candidate

- Hoàn thiện Pause/GameOver/Win, music/SFX và volume.
- Sửa build target, tracker, README, class diagram và archive.
- Chạy toàn bộ manual checklist và playthrough.
- Chỉ sửa regression; không thêm feature ngoài Sprint 6.

## 4. Kiểm thử và điều kiện đóng Sprint

Automated gate:

- Clean Debug build.
- Clean Release build.
- 100% CTest pass.
- Không có warning mới trong production target.
- State tests bao phủ death, reload, GameOver, timeout, flag/death race và Win.
- Có validator, physics, collision, input, save, HUD/item và audio-mapping tests.

Manual gate:

- Chơi hoàn chỉnh Level 1 → Level 2 → Level 3 → Win.
- Ít nhất 5 lượt mỗi level, gồm normal run, no-damage run và death/respawn.
- Kiểm tra Pause 20 lần ở các trạng thái khác nhau.
- Kiểm tra `1280×720`, `1600×900`, `1920×1080`, 4:3 và resize liên tục.
- Kiểm tra FireBall limit, Star timeout, shell chain kill, trần thấp và timer timeout.
- Restart ứng dụng để xác nhận high score, unlocked level và volume được lưu.

Sprint 6 chỉ được đánh dấu hoàn tất khi mọi task trong `docs/management/s6_plan.md` có bằng chứng tương ứng và không còn task ở mức “Chưa tốt”, “Hoàn toàn sai” hoặc “Chưa làm”.

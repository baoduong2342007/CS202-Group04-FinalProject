# Kịch bản video demo — Mario Remake

> **Project:** Super Mario Remake — CS202 Group 04
>
> **Thời lượng đề xuất:** 5–6 phút
>
> **Mục tiêu:** giới thiệu ngắn gọn công nghệ, kiến trúc, bốn màn chơi, cơ chế
> gameplay nổi bật, lưu tiến trình và hai chế độ chơi 2 người có thật trong
> project.

## 1. Chuẩn bị trước khi quay

- [ ] Build bản Release và chạy thử file thực thi ít nhất một lần.
- [ ] Quay đúng cửa sổ game, giữ tỉ lệ 16:9; xuất video 1280x720, 60 fps,
      H.264.
- [ ] Bật cả nhạc nền và hiệu ứng âm thanh; thử trước các âm thanh nhảy, ăn
      coin, dẫm quái và bắn fireball.
- [ ] Chuẩn bị một lượt chơi mới để quay quá trình mở khóa Level 1 → 4 và một
      save đã mở khóa các màn để quay nhanh những cảnh khó.
- [ ] Tập trước hai cách vượt Bowser: chạm rìu làm sập cầu và bắn trúng bằng
      5 fireball. Chỉ dùng **một cách** trong timeline chính; cách còn lại quay
      thành clip bổ sung.
- [ ] Nếu một tình huống khó tái hiện ổn định, quay thành clip riêng rồi chèn
      vào đúng chương; không khẳng định tính năng chưa xuất hiện trong footage.

## 2. Timeline và lời thuyết minh

| Thời gian | Phần | Hình ảnh/thao tác cần quay | Lời thuyết minh gợi ý |
|---|---|---|---|
| 0:00–0:20 | Mở đầu | Logo/tên project, tên môn học và thành viên nhóm; sau đó mở game đến Main Menu. | “Đây là **Super Mario Remake**, đồ án cuối kỳ CS202 của Nhóm 04. Nhóm xây dựng lại trải nghiệm Mario màn hình ngang bằng C++, SFML và Box2D, đồng thời bổ sung hệ thống lưu tiến trình và các chế độ chơi hai người.” |
| 0:20–0:45 | Công nghệ và kiến trúc | Chèn một slide ngắn: `C++ / SFML / Box2D`; tiếp theo là `Game States / Event Bus / State Pattern / Object Pooling / Save System`. Không cần chiếu code quá lâu. | “Project tách vòng lặp game, quản lý state, level, entity, vật lý, input và âm thanh thành các module riêng. State Pattern quản lý trạng thái sức mạnh của Mario; Game State điều phối menu, gameplay, pause, game over và win; Event Bus giúp gameplay phát sự kiện âm thanh; fireball được tái sử dụng bằng Object Pooling.” |
| 0:45–1:10 | Menu và chọn nhân vật | Mở phần chỉnh music/SFX, thay đổi âm lượng; vào Level Select và Character Select; chuyển qua lại giữa Mario và Luigi. | “Từ menu, người chơi có thể chọn màn, chọn nhân vật và điều chỉnh âm lượng. Mario là nhân vật cân bằng; Luigi chạy chậm hơn nhưng nhảy cao hơn và có độ bám thấp hơn. Tiến trình mở khóa, high score và thiết lập âm thanh được lưu lại.” |
| 1:10–2:15 | World 1-1 — Overworld | Đi/chạy, skid, nhảy ngắn và nhảy cao; dẫm Goomba; đập `?` block; ăn Mushroom rồi Fire Flower; phá brick khi ở dạng lớn; bắn fireball; dẫm Koopa và đá shell; vào bonus pipe; kết thúc bằng cột cờ. | “Màn 1-1 giới thiệu các cơ chế cốt lõi của Mario Remake: di chuyển có gia tốc, chạy nhanh, nhảy với độ cao phụ thuộc thời gian giữ phím, tương tác block và hệ thống điểm. Mushroom biến Mario thành Super Mario; Fire Flower mở khóa khả năng bắn fireball. Koopa có vòng đời đi bộ, rút vào mai và biến thành shell khi bị đá. Điểm cột cờ phụ thuộc vào độ cao khi Mario chạm cờ.” |
| 2:15–2:55 | World 1-2 — Overworld → Underground | Quay đoạn mở đầu ngoài trời, đi qua pipe để chuyển xuống khu Underground; cho thấy nhạc/màu sắc thay đổi; đứng trên elevator; né Hammer Bro; ăn Star và chạy xuyên qua quái; đi pipe trở lại khu cột cờ ngoài trời. | “Level 1-2 bắt đầu ở Overworld, chuyển xuống Underground qua pipe rồi trở lại Overworld ở đoạn kết. Màn chơi có moving elevator, Hammer Bro ném búa và Star tạo trạng thái bất tử tạm thời, đồng thời đổi nhạc trong thời gian hiệu lực.” |
| 2:55–3:25 | World 1-3 — Underwater | Vào pipe để chuyển từ đoạn mở đầu ngoài trời xuống nước; nhấn nhảy theo nhịp để bơi, thả phím để chìm; né Blooper và Cheep Cheep; quay bubbles/parallax; trở lại Overworld ở cuối màn. | “Ở 1-3, hệ thống chuyển sang vật lý dưới nước. Người chơi bơi bằng các nhịp nhấn, chịu lực chìm và đối đầu với Blooper cùng Cheep Cheep có hành vi khác quái trên cạn. Camera và phần nhìn vẫn theo sát Mario trong không gian có chiều cao lớn hơn.” |
| 3:25–4:20 | World 1-4 — Castle và Bowser | Né Firebar, Podoboo và Bullet Bill Launcher; vào đấu trường Bowser; cho thấy Bowser tuần tra, nhảy, phun lửa và tạo shockwave khi tiếp đất. Trong timeline chính, né Bowser rồi chạm **rìu** để cầu sập; chờ Bowser rơi xuống lava; đi tiếp đến Toad và màn Win. | “Màn cuối là lâu đài với các chướng ngại chuyên biệt và trận boss Bowser. Bowser không thể bị dẫm hoặc hạ bằng Star. Người chơi có thể thắng bằng 5 fireball hoặc chạm rìu để làm sập cầu. Trong lượt demo này, nhóm chọn cách dùng rìu. Sau khi Bowser bị hạ, lối đến Toad mới hoàn tất màn và chuyển sang Win State.” |
| 4:20–4:40 | Lưu và tải tiến trình | Từ Win/Menu, thoát hẳn game rồi mở lại; vào Level Select và phần âm lượng. Quay rõ các level đã mở, high score và volume vẫn còn. | “Save Manager lưu high score, level cao nhất đã mở và thiết lập âm thanh. Vì vậy dữ liệu vẫn được phục hồi sau khi đóng và chạy lại chương trình.” |
| 4:40–5:10 | 2 Player Co-op | Chọn `2 PLAYER CO-OP`, chọn nhân vật lần lượt cho P1 và P2, chọn một level rồi chơi khoảng 20–30 giây. Quay cảnh cả hai nhân vật cùng xuất hiện, camera giữ cả đội trong khung hình và một người hỗ trợ người còn lại. | “Co-op cho hai người cùng chơi chiến dịch trên một màn. Hai nhân vật dùng bộ phím riêng, chia sẻ tiến trình của đội và camera được điều chỉnh để cả hai vẫn tham gia cùng một không gian chơi.” |
| 5:10–5:40 | 2 Player Versus | Chọn `2 PLAYER VERSUS`; quay phần chọn hai nhân vật, Round intro, một pha head-stomp tính KO, chuyển round và màn hình kết quả. Nếu xuất hiện Fire Flower, quay thêm fireball hất đối thủ. | “Versus là đấu trường best-of-three. Head-stomp quyết định người thắng round; Fire Flower chỉ tồn tại tạm thời và fireball dùng để hất đối thủ thay vì trực tiếp tính điểm hạ gục. Round và kết quả trận được quản lý bằng một state machine riêng.” |
| 5:40–5:55 | Pause và kết | Nhấn Esc, thay đổi một mức âm lượng, Resume; kết bằng title card có tên project, nhóm và repository/video link. | “Project hoàn thiện luồng menu, pause, game over, win, âm thanh và lưu dữ liệu bên cạnh gameplay chính. Đây là Super Mario Remake của Nhóm 04. Cảm ơn thầy cô và các bạn đã theo dõi.” |

## 3. Điều khiển cần hiển thị trong video

### Chơi đơn

| Hành động | Phím |
|---|---|
| Đi trái/phải | `A` / `D` hoặc `Left` / `Right` |
| Nhảy/bơi | `W`, `Up` hoặc `Space` |
| Cúi/đi xuống pipe | `S` hoặc `Down` |
| Chạy nhanh | `Left Shift` hoặc `Right Shift` |
| Bắn fireball | `X` |
| Pause | `Esc` |

### Co-op

| Người chơi | Di chuyển | Nhảy | Chạy | Bắn |
|---|---|---|---|---|
| P1 | `W/A/S/D` | `W` hoặc `Space` | `Left Shift` hoặc `J` | `X` hoặc `F` |
| P2 | Phím mũi tên hoặc Numpad | `Up`, `Numpad 8`, `Numpad 0` hoặc `Enter` | `Right Shift`, `Right Ctrl`, `M` hoặc `Numpad 1` | `/`, `.` hoặc `Numpad 3` |

> Với bàn phím không có Numpad, nên dùng nhóm phím mũi tên + `Enter`,
> `Right Shift`/`Right Ctrl` và `/` cho P2.

### Versus

| Người chơi | Trái/phải | Nhảy | Chạy | Bắn |
|---|---|---|---|---|
| P1 | `A` / `D` | `W` | `Left Shift` | `X` |
| P2 | `Left` / `Right` | `Up` | `Right Shift` | `/` |

## 4. Clip bổ sung nên quay

Các clip sau không cần nằm trong timeline chính, nhưng hữu ích khi giảng viên
hỏi sâu hoặc khi cần chứng minh tính năng:

- Bowser bị hạ sau **5 lần trúng fireball** — đây là take riêng, không ghép nối
  như thể xảy ra trước cảnh rìu làm sập cầu.
- Mario đứng trong vùng shockwave và bị stun, sau đó một take khác nhảy qua
  shockwave để thể hiện cách khắc chế.
- Game Over khi hết lives, sau đó thử cả `Retry Stage` và quay về Main Menu.
- Mario ăn 1-Up Mushroom; Star hạ quái bằng va chạm; shell hạ nhiều quái liên
  tiếp.
- Dạng Small, Super, Fire Small và Fire Super; cho thấy chỉ dạng lớn mới phá
  brick, còn hai dạng Fire có thể bắn.
- Một lần chạm cột cờ thấp và một lần chạm cao để so sánh điểm thưởng
  (100–5000 điểm theo độ cao).
- Pause/resume nhiều lần để kiểm tra gameplay dừng đúng và nhạc tiếp tục mà
  không bị phát chồng.

## 5. Gợi ý dựng video

- Dùng caption ngắn ở đầu mỗi chương: `OVERWORLD`, `UNDERGROUND`,
  `UNDERWATER`, `CASTLE & BOWSER`, `CO-OP`, `VERSUS`.
- Giữ HUD trong khung hình để người xem thấy score, coin, lives và trạng thái
  sức mạnh thay đổi theo gameplay.
- Không thay nhạc game bằng nhạc nền khác; âm thanh là một phần của sản phẩm.
- Có thể tăng tốc các đoạn chỉ di chuyển, nhưng không tăng tốc cảnh chứng minh
  cơ chế vật lý, AI, power-up, lưu dữ liệu hoặc chuyển state.
- Khi nói về kiến trúc, chỉ dùng một slide 15–20 giây; trọng tâm video vẫn là
  sản phẩm chạy thật.
- Tên file xuất đề xuất: `CS202_Group04_Mario_Remake_Demo.mp4`.

## 6. Kiểm tra sau khi quay

- [ ] Video thể hiện đủ bốn level theo thứ tự `1-1 → 1-2 → 1-3 → 1-4`.
- [ ] Có cả bốn chủ đề hình ảnh/âm nhạc: Overworld, Underground, Underwater,
      Castle.
- [ ] Có ít nhất một ví dụ cho movement, item/power-up, enemy AI, pipe warp,
      tính điểm và boss.
- [ ] Không mô tả việc “bắn hạ Bowser rồi chặt cầu làm Bowser rơi” trong cùng
      một lượt chơi; đó là hai cách thắng độc lập.
- [ ] Có cảnh đóng/mở lại game để chứng minh save/load.
- [ ] Có footage thật của Co-op và Versus, không chỉ dừng ở menu.
- [ ] Kiểm tra lại tiếng thuyết minh, game audio, caption và chính tả trước khi
      nộp.

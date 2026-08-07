# TV5 Audio / HUD / Item integration checklist

Checklist này là bằng chứng manual tái lập cho các phần không phù hợp để
assert trực tiếp trong unit test (audio device, render layout và full game
loop). Chạy từ clean executable directory sau build.

## Setup

1. Build bằng `cmake --build build --parallel 2`.
2. Chạy executable từ `build/` để các path `assets/...` được resolve đúng.
3. Bật âm lượng hệ thống ở mức nghe được và ghi lại build/commit đang test.

## Items và QuestionBlock

- [ ] Đứng dưới `?` khi Mario ở SMALL: block chuyển sang used, Mushroom spawn
      đúng một lần, không collect được trong frame spawn và collect được sau
      emergence delay.
- [ ] Đứng dưới `?` khi Mario ở SUPER hoặc FIRE: content resolve thành
      FireFlower, không đổi content khi hit lại.
- [ ] Hit `U`: chỉ spawn một 1-Up Mushroom; collect tăng đúng một life.
- [ ] Hit `O`: chỉ spawn một Star; collect bật invincibility tạm thời và
      không đổi Mario thành SUPER vĩnh viễn.
- [ ] Collect cùng một item hai lần bằng overlap liên tục: chỉ có một score
      update và một power-up event/SFX.
- [ ] Collect coin đến 100: life tăng một và coin counter giữ phần dư.

## HUD

- [ ] Mỗi level bắt đầu với `TIME 400` và `WORLD 1-1`, `1-2` hoặc `1-3` đúng.
- [ ] Score luôn zero-padding sáu chữ số; score vượt giới hạn hiển thị không
      làm tràn layout.
- [ ] Power indicator lần lượt hiển thị `SMALL`, `SUPER`, `FIRE`, `STAR`.
- [ ] Khi timer qua 100, `hurryup.wav` chỉ phát một lần.
- [ ] Pause/resume 20 lần: timer không giảm trong pause, coin/lives/power không
      đổi ngoài gameplay.
- [ ] Fade transition: timer đứng yên trong thời gian fade/load.

## Audio

- [ ] Level 1/2/3 lần lượt dùng overworld/underground/castle track.
- [ ] Coin hoặc stomp liên tục: các âm thanh chồng bằng voice pool, không bị
      restart/cắt ngay voice đang phát.
- [ ] Pause: music giữ vị trí và resume cùng track sau khi pop PauseState.
- [ ] Volume âm thanh và music thử ở -10, 0, 50, 100, 150; API không trả ra
      giá trị ngoài `[0,100]`.
- [ ] Event `PLAYER_DIED`, `PLAYER_POWER_UP`, `PLAYER_POWER_DOWN`,
      `LEVEL_COMPLETED` phát đúng SFX catalog.

## Ghi nhận kết quả

| Ngày | Build/commit | Người chạy | Kết quả | Ghi chú |
|---|---|---|---|---|
| | | | `PASS` / `FAIL` | |

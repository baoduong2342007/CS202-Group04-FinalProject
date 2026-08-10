# TV5 — Checklist tích hợp Audio/HUD/Item và runtime

Tài liệu này ghi lại bằng chứng cho S6-TV5-44 và các đường chạy runtime liên
quan đến S6-TV5-16/S6-TV5-32. Snapshot kiểm tra là `HEAD
128e047d876adcc06cfa672d66613c050d32059c + working tree`, ngày 2026-08-09.

## 1. Kiểm tra tự động

Chạy từ repository root:

```powershell
cmake --build --preset mingw-tests --parallel 2
ctest --preset mingw-tests --output-on-failure
```

Kết quả snapshot này: `12/12` test pass, bao gồm `tv5_integration_tests`.

| Luồng | Bằng chứng | Kết quả |
|---|---|---|
| Score stomp/shell/fireball/star | Box2D contact thật trong `testDefeatScoreAndShellEventsThroughCollisionRuntime` | PASS |
| Shell kick không lặp khi contact kéo dài | Hai physics step, kiểm tra score và event count | PASS |
| Level 1 → Level 2 → Level 3 | `testStateAudioRuntimeAndLevelTracks` qua `GameManager`/`PlayState` thật | PASS |
| Level 3 → Win | `LEVEL_COMPLETED` và `WinState::onEnter()` | PASS |
| Player death → Death music | `EventBus::PLAYER_DIED` tới `SoundManager` | PASS |
| GameOver music | `GameOverState::onEnter()` | PASS |
| Pause/resume music | 20 vòng `PauseState` push/pop, không đổi track | PASS |
| Star music override/restore | Star track và restore track level sau expiry | PASS |
| Sound/music volume persistence | `SaveManager` save/reload hai volume độc lập | PASS |
| HUD star/timeout/pause/death/level guard | `testStarTimerHudAndExpiryEvent` và `testHudTimeoutAndGameplayFreeze` | PASS |
| Item score và pickup idempotency | Coin 100, power-up 1000, one-up threshold, adaptive block | PASS |
| Runtime asset dimensions/manifest | PNG dimensions và `ASSETS_LIST.md` caller/path assertions | PASS |

Các test runtime chính nằm trong
[`tests/TV5IntegrationTests.cpp`](../../tests/TV5IntegrationTests.cpp).

## 2. Checklist thủ công khi chạy game có audio device

Phần này không thay thế test headless và không được đánh dấu PASS nếu chưa
nghe/chụp bằng chứng trên máy chạy game.

1. Chạy `build-debug/SuperMario.exe` từ thư mục `build-debug`.
2. Ở Level 1, nhặt Coin, Mushroom/FireFlower và Star; xác nhận HUD đổi state,
   score tăng một lần cho mỗi pickup, Star hiện trong 10 giây rồi HUD trở về
   state Mario trước đó.
3. Stomp một Goomba, kick một Koopa rồi để shell chạm một enemy khác; xác nhận
   stomp chỉ có SFX stomp, kick và shell-kill là hai lần phát độc lập, không
   lặp liên tục khi shell còn tiếp xúc.
4. Hoàn thành tuần tự Level 1, 2, 3; xác nhận nhạc lần lượt là overworld,
   underground, castle và nhạc Win sau Level 3.
5. Pause/resume nhiều lần; xác nhận nhạc không bị nhân bản hoặc tự chuyển track.
6. Chết rồi để hết lượt; xác nhận Death rồi GameOver music, không bị Star expiry
   ghi đè.
7. Trong Pause, chỉnh Music và SFX riêng; thoát/chạy lại game và xác nhận hai
   volume được khôi phục độc lập.

Khi thực hiện manual QA, ghi ngày, build directory, người kiểm tra và ảnh/video
hoặc mô tả kết quả vào ticket/release note tương ứng; không dùng checklist này
để suy diễn rằng thiết bị audio đã được kiểm tra nếu chỉ chạy CTest.

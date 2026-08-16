Redesign PvP arena: bỏ 2 cầu thang, mỗi bên 1 block trôi đối xứng làm bậc leo lên bệ hoa lửa
=======================================================================================

Hiện trạng & ràng buộc đã xác minh
----------------------------------
- Map hiện tại `levels/pvp_arena.txt` (20×11 tile) có 2 cầu thang đá 4 bậc chiếm hết cols 3–6 và 13–16, không còn không gian PvP.
- Vật lý: PPM=30, gravity=25 m/s², jumpForce Mario=460 (tầm nhảy tối đa ≈141px ≈ 4.4 tile), Luigi=510 (≈173px). Bệ hoa lửa (row 4, top y=128) cao 160px so với sàn (top y=288) → Mario không nhảy thẳng từ đất lên được; cần bậc trung gian.
- Giữ nguyên: kích thước 20×11, tường 2 bên, sàn `0` ở rows 9–10, W tại (9,3), nền bệ `SSSS` row 4 cols 8–11, spawn M (2,8) / m (17,8). Camera (640×360, khóa) và pit-death tự suy từ map nên không cần sửa code.

Thay đổi
--------
1. **`levels/pvp_arena.txt`** — layout mới (block leo tại (7,6) và (12,6), đối xứng quanh tâm 9.5):

   ```
   S..................S
   S..................S
   S..................S
   S........W.........S
   S.......SSSS.......S
   S..................S
   S......S....S......S
   S..................S
   S.M..............m.S
   00000000000000000000
   00000000000000000000
   ```
   - Đường leo mỗi bên: sàn → block (nhảy 3 tile, dễ cho cả 2 nhân vật) → bệ hoa lửa (nhảy 2 tile + dịch 1 cột, dễ).
   - Cập nhật phần comment header (dòng 4–7) mô tả layout mới: sàn đấu trống, 2 block bước đối xứng, không còn cầu thang.
   - Vùng drop của `PvpFlowTests` (cols 9–10, rows 6–8) vẫn toàn воздуха → test flow không phải sửa.

2. **`tests/PvpArenaTests.cpp`** — trong `testShippedArenaFixtureLoads`, thay khẳng định cầu thang (dòng 95–99) bằng khẳng định layout mới (mạnh hơn, khẳng định đúng yêu cầu "có chỗ PvP"):
   ```cpp
   // Open duel floor: staircases removed. Two symmetric floating step
   // blocks flank the pedestal; the whole fight band is otherwise air.
   assert(tileMap.isSolid(7, 6) && tileMap.isSolid(12, 6));
   for (int column = 1; column <= 18; ++column) {
       for (int row = 5; row <= 8; ++row) {
           if ((column == 7 || column == 12) && row == 6) continue;
           assert(!tileMap.isSolid(column, row));
       }
   }
   ```

3. **`FILE_STRUCTURE.md` dòng 77** — sửa mô tả `pvp_arena.txt`: "symmetric staircases" → mô tả mới "symmetric floating step blocks flank the center pedestal" (sàn đấu trống).

Không phải sửa: `PvpPlayState.cpp`, `Level.cpp`, `TileMap.cpp` (mọi logic spawn/hoa lửa/pit đều dẫn xuất từ map); `PvpFlowTests.cpp` và `PvpStompTests.cpp` (vùng drop giữa map vẫn trống); CMake (tên file giữ nguyên).

Xác minh
--------
- Configure + build ở build dir mới (theo convention của repo, vd. `build-pvp-arena-redesign`, dùng preset/generator sẵn có).
- Chạy 3 test PvP: `pvp_arena_tests`, `pvp_flow_tests`, `pvp_stomp_tests` (qua ctest) — tất cả phải pass.
- Chạy thêm toàn bộ ctest nếu thời gian cho phép để chắc không regression.
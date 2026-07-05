# Review & Action Items — TV4 (Vy - Level & Enemy)

> **Nhánh làm việc:** `feature/level-and-enemy`  
> **Người review:** TV1 (Architect)  
> **Trạng thái:** Đã gộp vào `develop` — Cần bổ sung file khung trong lần push kế tiếp

---

## 1. Đánh giá kết quả Tuần 1
- **Điểm mạnh:**
  - Đã tạo file bản đồ mẫu [`levels/level1.txt`](../levels/level1.txt) theo đúng chuẩn ASCII (lưới 64x10), có đầy đủ chú thích ký hiệu (`M` Mario, `G` Goomba, `K` Koopa, `?` Question block...).
  - Đã gộp (merge) thành công vào `develop` không xảy ra lỗi build hay xung đột.
- **Điểm cần bổ sung (Action Items):**
  - Trong lần commit kế tiếp, TV4 cần bổ sung 2 file header khung (skeleton) còn thiếu của module Level & Enemy.

---

## 2. Checklist nhiệm vụ cần làm trong lần Push kế tiếp

TV4 cần tạo và cập nhật các file sau theo đúng quy tắc kiến trúc của nhóm ([`ROLES.md`](../ROLES.md) và [`CODING_RULES.md`](../CODING_RULES.md)):

- [ ] **Tạo file `include/level/TileMap.h` (Skeleton):**
  - Khai báo lớp `TileMap` với các hàm cơ bản: `loadFromFile(string path)`, `render(sf::RenderWindow& window)`, `getTileAt(int x, int y)`, `isSolid(int x, int y)`.
  - Biến thành viên private đặt tên đúng chuẩn tiền tố `m_`: `m_grid`, `m_tileset`.
- [ ] **Tạo file `include/entities/Enemy.h` (Skeleton):**
  - Khai báo lớp trừu tượng `Enemy` (kế thừa từ `Character`).
  - Định nghĩa 2 hàm ảo thuần túy (pure virtual) bắt buộc: `virtual void patrol() = 0;` và `virtual void onStomp() = 0;`.
  - Ghi chú lý do sử dụng Design Pattern theo Quy tắc 4 (đa hình / Template method cho Goomba và Koopa).
- [ ] **Thêm Header Comments chuẩn Doxygen (Quy tắc 3):**
  ```cpp
  /**
   * @file TileMap.h / Enemy.h
   * @author TV4 (Vy)
   * @brief TileMap management and abstract Enemy base class
   * @note Week 2 skeleton setup
   */
  ```

---

## 3. Hướng dẫn Git Workflow cho TV4

Do nhánh `develop` hiện tại đã tích hợp code mới nhất của TV1, TV5 và TV4, trước khi viết code cho Tuần 2, TV4 cần gộp nhánh `develop` mới nhất về nhánh của mình:

### Các bước thực hiện trên terminal:

**Bước 1: Chuyển sang nhánh làm việc của TV4**
```bash
git checkout feature/level-and-enemy
```

**Bước 2: Cập nhật code mới nhất từ nhánh `develop` về**
```bash
git fetch origin
git merge origin/develop
```

**Bước 3: Tạo 2 file `TileMap.h` và `Enemy.h` theo checklist ở trên**

**Bước 4: Kiểm tra dịch chương trình (Đảm bảo không lỗi)**
```bash
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

**Bước 5: Commit và Push lên remote**
```bash
git add .
git commit -m "[TV4] Add skeleton headers for TileMap and Enemy"
git push origin feature/level-and-enemy
```

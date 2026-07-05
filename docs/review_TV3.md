# Review & Refactor Checklist — TV3 (Bảo - Mario & Physics)

> **Nhánh làm việc hiện tại:** `origin/Baro` *(Vi phạm quy tắc đặt tên nhánh)*  
> **Người review:** TV1 (Architect)  
> **Trạng thái:** ❌ **TẠM HOÃN GỘP (DO NOT MERGE)** — Yêu cầu TV3 refactor và chuyển nhánh theo checklist dưới đây trước khi merge vào `develop`.

---

## 1. Đánh giá chung Tuần 1

### 🟢 Những điểm làm tốt (Cần giữ nguyên & phát huy):
- Cấu trúc 2 file `Entity.h` và `Character.h` viết rất chuẩn, gọn gàng và khoa học (chia rõ từng mục: Constructor, Override, Public methods, Getters/Setters, Protected members).
- **Tuân thủ tuyệt đối Quy tắc 2 (Naming convention):** Đã đặt đúng tiền tố `m_` cho tất cả các biến thành viên protected/private (`m_boundingBox`, `m_position`, `m_sprite`, `m_velocity`, `m_facingDirection`, `m_health`, `m_isGrounded`).
- Kế thừa OOP hợp lý (`Character` kế thừa từ `Entity`, `Entity` kế thừa từ `sf::Drawable`).
- Hàm `update(float dt)` dùng tham số thời gian giây đúng chuẩn quy tắc giao tiếp module.

### 🔴 Các vi phạm nội quy & Lỗi nghiêm trọng cần sửa:
1. **Sai tên nhánh (Quy tắc 1):** Nhánh hiện tại đặt là `Baro` là sai quy định. Bắt buộc phải đặt tên nhánh theo cấu trúc `feature/<module-name>`, cụ thể là **`feature/mario-and-physics`** (hoặc `feature/mario-physics`).
2. **Sai định dạng Commit Message:** Commit hiện tại không có tiền tố `[TV3]`. Các commit bắt buộc phải theo cấu trúc: `[TVx] <Verb> <Short Description>`.
3. **Đặt sai thư mục lưu trữ (Directory Placement Error):** Theo `FILE_STRUCTURE.md` và `ROLES.md`, các lớp nhân vật/thực thể phải nằm trong thư mục **`include/entities/`**. Hiện tại TV3 lại đặt nhầm vào thư mục **`include/items/`** (đây là thư mục dành cho vật phẩm như Coin, Mushroom của TV5).
4. **File `Mario.h` bị rỗng (0 bytes):** File `include/items/Mario.h` hiện đang là file trống hoàn toàn chưa có bất kỳ dòng code nào.
5. **Thiếu Header Comments bắt buộc (Quy tắc 3):** Cả 3 file header đều chưa có Doxygen comment ở đầu file.
6. **Thay đổi lộn xộn các file cấu hình chung:** Nhánh cũ dính nhiều thay đổi về dòng trắng/CRLF trên `README.md`, `CODING_RULES.md`, `src/main.cpp`...

---

## 2. Checklist chỉnh sửa dành cho TV3

TV3 cần thực hiện refactor code và tuân thủ các bước sau:

- [ ] **Tạo nhánh làm việc mới đúng chuẩn:** Chuyển từ nhánh `develop` mới nhất sang nhánh `feature/mario-and-physics`.
- [ ] **Di chuyển đúng thư mục:** Chuyển 3 file header từ `include/items/` sang **`include/entities/`**:
  - `include/entities/Entity.h`
  - `include/entities/Character.h`
  - `include/entities/Mario.h`
- [ ] **Bổ sung Header Comments chuẩn Doxygen (Quy tắc 3) vào đầu cả 3 file:**
  ```cpp
  /**
   * @file Entity.h / Character.h / Mario.h
   * @author TV3 (Bảo)
   * @brief Base Entity class / Character class / Mario player class
   * @note Week 1 skeleton setup
   */
  ```
- [ ] **Viết khung skeleton cho `include/entities/Mario.h` (Không để file rỗng):**
  - Kế thừa từ `Character`.
  - Định nghĩa các trạng thái cơ bản hoặc override hàm `update(float dt) override;` và `void handleInput();` theo thiết kế tuần 1/2.
- [ ] **Tuân thủ Commit Message:** Bắt đầu bằng `[TV3] ...`.

---

## 3. Hướng dẫn Git Workflow từng bước để khắc phục

Để tránh bị xung đột (conflict) với các thay đổi trên `develop` và dọn dẹp các file bị đặt sai chỗ, TV3 hãy làm theo đúng trình tự các lệnh sau trên Terminal:

### Bước 1: Cập nhật nhánh `develop` mới nhất từ GitHub
```bash
git checkout develop
git fetch origin
git pull origin develop
```

### Bước 2: Tạo nhánh mới đúng tên chuẩn từ `develop`
```bash
git checkout -b feature/mario-and-physics
```

### Bước 3: Chỉnh sửa và sắp xếp lại code
1. Tạo thư mục `include/entities/` (nếu chưa có).
2. Viết/di chuyển 3 file `Entity.h`, `Character.h`, `Mario.h` vào đúng thư mục `include/entities/`.
3. Đảm bảo thư mục `include/items/` không chứa code của TV3.
4. Thêm Header Comments vào đầu file và bổ sung code khung cho `Mario.h`.

### Bước 4: Kiểm tra dịch chương trình (Kiểm thử trước khi commit)
```bash
cmake -B build -G "MinGW Makefiles"
cmake --build build
```
*(Đảm bảo terminal báo `[100%] Built target main` không có lỗi)*

### Bước 5: Commit đúng chuẩn và Push nhánh mới lên remote
```bash
git add .
git commit -m "[TV3] Add skeleton headers for Entity, Character, and Mario in entities folder"
git push -u origin feature/mario-and-physics
```
*(Sau đó thông báo cho TV1 để tiến hành review lại và merge vào `develop`)*

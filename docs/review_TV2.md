# Review & Refactor Checklist — TV2 (Nhật - Engine & Render)

> **Nhánh làm việc:** `feature/game-loop`  
> **Người review:** TV1 (Architect)  
> **Trạng thái:** Yêu cầu chỉnh sửa trước khi gộp (Request Refactor before Merge)

---

## 1. Danh sách điểm cần chỉnh sửa (Refactor Checklist)

TV2 cần thực hiện các chỉnh sửa sau trên nhánh `feature/game-loop` để tuân thủ quy tắc nhóm ([`CODING_RULES.md`](../CODING_RULES.md)) và đúng thiết kế kiến trúc ([`ROLES.md`](../ROLES.md)):

### 🔴 Quy tắc đặt tên & Cấu trúc lớp (Naming & Layout)
- [ ] **Thêm Header Comments bắt buộc:** Thêm khối Doxygen comment vào đầu 2 file `include/core/Game.h` và `src/core/Game.cpp` theo đúng chuẩn quy tắc 3:
  ```cpp
  /**
   * @file Game.h / Game.cpp
   * @author TV2
   * @brief Game loop, window management and event handling
   * @note Week 1 — SFML 3.0.0 setup
   */
  ```
- [ ] **Thêm tiền tố `m_` cho biến private:** Sửa tên các thành viên private trong `Game.h` theo đúng chuẩn quy tắc 2 (dòng 89):
  - `window` $\rightarrow$ `m_window`
  - `shape` $\rightarrow$ `m_shape`
- [ ] **Gộp nhãn `private:` trong `Game.h`:** Hiện tại file header đang có 2 nhãn `private:` bị lặp lại, cần gộp chung lại theo đúng layout chuẩn của nhóm.

### 🟡 Kiến trúc & Xử lý cảnh báo (Architecture & Warnings)
- [ ] **Chuẩn hóa hàm `update(float dt)`:** 
  - Khai báo hiện tại `void update(sf::Time deltaTime)` làm phát sinh cảnh báo compiler `unused parameter` và không đúng giao tiếp với các module khác.
  - Cần chuyển tham số thành `void update(float dt)` (giây, tính bằng `deltaTime.asSeconds()`) để tương thích với `Character::update(float dt)`, `Level::update(float dt)`,... theo đúng sơ đồ lớp.
- [ ] **Loại bỏ Magic Numbers:** Thay thế các con số gán cứng trong `Game.cpp` (`1280`, `720`, `60`, `100.f`) bằng hằng số rõ ràng:
  ```cpp
  const unsigned int SCREEN_WIDTH = 1280;
  const unsigned int SCREEN_HEIGHT = 720;
  const unsigned int FRAMERATE_LIMIT = 60;
  ```
- [ ] **Xử lý lệnh in debug `std::cout`:** Bọc các lệnh `std::cout` trong block `#ifdef DEBUG` hoặc xóa bỏ trước khi tạo Pull Request để tránh rác console khi chạy game thật.

---

## 2. Hướng dẫn Git Workflow (Pull / Merge từ branch nào để làm tiếp)

Do nhánh `develop` vừa được TV1 cập nhật (bỏ theo dõi file nhị phân SFML ~30MB, tối ưu CMakeLists.txt và thêm sơ đồ lớp), **TV2 bắt buộc phải gộp (merge) code mới nhất từ `develop` về nhánh của mình trước khi viết tiếp code**, để tránh xung đột (conflict) và lỗi build sau này.

### Các bước thực hiện chi tiết cho TV2:

**Bước 1: Chuyển sang nhánh làm việc của TV2**
```bash
git checkout feature/game-loop
```

**Bước 2: Pull / Merge cập nhật mới nhất từ nhánh `develop` về**
```bash
git fetch origin
git merge origin/develop
```
> *Lưu ý: Việc merge `origin/develop` về sẽ tự động xóa các file nhị phân SFML cũ khỏi git tracking của nhánh bạn, giúp nhẹ máy và đồng bộ CMakeLists.txt mới nhất.*

**Bước 3: Thực hiện sửa code theo Checklist ở mục 1**

**Bước 4: Kiểm thử build (Đảm bảo 0 lỗi, 0 cảnh báo)**
```bash
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

**Bước 5: Commit và Push lên để review lại**
```bash
git add .
git commit -m "[TV2] Refactor Game class to follow coding rules and architecture"
git push origin feature/game-loop
```

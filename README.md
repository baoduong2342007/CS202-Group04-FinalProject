# Super Mario — CS202 Final Project

> Dự án game 2D Mario-style viết bằng C++17 + SFML 3.0.0  
> Nhóm 5 người · 6 tuần

---

## Thành viên & vai trò nhanh

| Thành viên | Vai trò | Module chính |
|---|---|---|
| TV1 | Architect / Team Lead | OOP design, design patterns, tích hợp |
| TV2 | Engine & Render | SFML wrapper, animation, camera |
| TV3 | Mario & Physics | Character controller, collision |
| TV4 | Level & Enemy | Tilemap, enemy AI, level design |
| TV5 | UI, Sound & Items | HUD, menu, âm thanh, power-up |

→ Chi tiết: xem [`ROLES.md`](ROLES.md)  
→ Kế hoạch tuần: xem [`WEEKLY_PLAN.md`](WEEKLY_PLAN.md)

---

## Tech stack

| Thứ | Chi tiết |
|---|---|
| Ngôn ngữ | C++17 |
| Game engine | SFML 3.0.0 (graphics, window, system, audio) |
| Build | CMake 3.15+ |
| Compiler | g++ (MinGW-w64) 14.2.0+ |
| Version control | Git + GitHub |

---

## Cài đặt & build

### 1. Cài SFML & Tools

**Windows (MinGW):**
- Cài đặt **g++ (MinGW-w64) 14.2.0 trở lên** và **CMake 3.15 trở lên**.
- Thư viện **SFML 3.0.0** đã được tích hợp sẵn trong repo tại thư mục `thirdparty/SFML` (không cần tải thêm). Cửa sổ game và DLLs sẽ tự động được copy sau khi build.

**Ubuntu/Debian:**
```bash
sudo apt-get install libsfml-dev
```

**macOS:**
```bash
brew install sfml
```

### 2. Clone repo

```bash
git clone https://github.com/<org>/SuperMario.git
cd SuperMario
```

### 3. Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### 4. Chạy game

```bash
./main              # Linux/macOS
main.exe            # Windows
```

> **Lưu ý:** executable phải được chạy từ thư mục `build/` vì asset path là `assets/` tương đối.

---
---

## .gitignore (copy vào root)

```
build/
saves/highscore.dat
*.o
*.exe
.vscode/
.idea/
CMakeCache.txt
CMakeFiles/
```

---

## Tài liệu liên quan

- [`ROLES.md`](ROLES.md) — Phân vai chi tiết
- [`WEEKLY_PLAN.md`](WEEKLY_PLAN.md) — Kế hoạch 6 tuần theo từng thành viên
- [`FILE_STRUCTURE.md`](FILE_STRUCTURE.md) — Cấu trúc thư mục & file
- [`CODING_RULES.md`](CODING_RULES.md) — Quy tắc code, Git, naming
- [`docs/design_patterns.md`](docs/design_patterns.md) — Mô tả 5 design pattern _(TV1 viết — tuần 5)_
- [`docs/class_diagram.png`](docs/class_diagram.png) — Class diagram _(tuần 6)_

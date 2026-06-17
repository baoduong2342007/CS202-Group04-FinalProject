# Super Mario — CS202 Final Project

> Dự án game 2D Mario-style viết bằng C++17 + SFML 2.6.2  
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
| Game engine | SFML 2.6.2 (graphics, window, system, audio) |
| Build | CMake 3.16+ |
| Compiler | GCC 10+ hoặc MSVC 2019+ |
| Version control | Git + GitHub |

---

## Cài đặt & build

### 1. Cài SFML

**Windows (MinGW):**
```bash
# Tải SFML 2.6.2 từ https://www.sfml-dev.org/download.php
# Giải nén, copy vào C:/SFML hoặc thư mục tùy chọn
```

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
./SuperMario        # Linux/macOS
SuperMario.exe      # Windows
```

> **Lưu ý:** executable phải được chạy từ thư mục `build/` vì asset path là `assets/` tương đối.

---

## CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(SuperMario VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.6.2 COMPONENTS graphics window system audio REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME}
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# Tự copy assets và levels vào build/
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/levels $<TARGET_FILE_DIR:${PROJECT_NAME}>/levels
)
```

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

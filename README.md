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
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

# Ưu tiên tìm SFML 3.0 trong thirdparty/SFML nếu tồn tại
if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/SFML")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/thirdparty/SFML")
endif()

# Tìm SFML 3.0 (MinGW/MSYS2 hoặc từ thirdparty)
find_package(SFML 3.0 COMPONENTS Graphics Window System Audio REQUIRED)

# Nạp tất cả source code
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

# Cấu hình compiler (MinGW/GCC)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)

# Ẩn console khi build Release trên Windows
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE TRUE)
endif()

# Thư mục include
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Liên kết SFML 3 (Target names thay đổi thành SFML::Component)
target_link_libraries(${PROJECT_NAME} PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)

# Tự động copy assets & levels sau khi build
foreach(DIR "assets" "levels")
    if(EXISTS "${CMAKE_SOURCE_DIR}/${DIR}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${DIR}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${DIR}"
            COMMENT "Copying ${DIR} to output directory..."
        )
    endif()
endforeach()

# Tự động copy SFML 3 DLLs (để chạy file .exe độc lập)
if(WIN32)
    # Xác định thư mục chứa DLL
    if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/SFML/bin")
        set(SFML_DLL_DIR "${CMAKE_SOURCE_DIR}/thirdparty/SFML/bin")
    else()
        get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        set(SFML_DLL_DIR "${MINGW_BIN_DIR}")
    endif()

    set(SFML_MINGW_DLLS
        "${SFML_DLL_DIR}/sfml-graphics-3.dll"
        "${SFML_DLL_DIR}/sfml-graphics-d-3.dll"
        "${SFML_DLL_DIR}/sfml-window-3.dll"
        "${SFML_DLL_DIR}/sfml-window-d-3.dll"
        "${SFML_DLL_DIR}/sfml-system-3.dll"
        "${SFML_DLL_DIR}/sfml-system-d-3.dll"
        "${SFML_DLL_DIR}/sfml-audio-3.dll"
        "${SFML_DLL_DIR}/sfml-audio-d-3.dll"
    )
    foreach(DLL_FILE IN LISTS SFML_MINGW_DLLS)
        if(EXISTS "${DLL_FILE}")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLL_FILE}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/"
                COMMENT "Copying ${DLL_FILE}..."
            )
        endif()
    endforeach()
endif()
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

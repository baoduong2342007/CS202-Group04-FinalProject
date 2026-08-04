# Super Mario — CS202 Final Project

> 2D Mario-style game project written in C++17 + SFML 3.0.0  
> 5 members · 6 weeks

---

## Team Members & Quick Roles

| Member | Name | Role | Main Module |
|---|---|---|---|
| TV1 | Dương | Architect / Team Lead | OOP design, design patterns, integration |
| TV2 | Nhật | Engine & Render | SFML wrapper, animation, camera |
| TV3 | Bảo | Mario & Physics | Character controller, collision |
| TV4 | Vy | Level & Enemy | Tilemap, enemy AI, level design |
| TV5 | Truyền | UI, Sound & Items | HUD, menu, sound, power-up |

→ Detailed roles: see [`docs/management/ROLES.md`](docs/management/ROLES.md)  
→ Weekly plan: see [`docs/management/WEEKLY_PLAN.md`](docs/management/WEEKLY_PLAN.md)

---

## Tech Stack

| Component | Details |
|---|---|
| Language | C++17 |
| Game Engine | SFML 3.0.0 (graphics, window, system, audio) |
| Build System | CMake 3.15+ |
| Compiler | g++ (MinGW-w64) 14.2.0+ |
| Version Control | Git + GitHub |

---

## Installation & Build

### 1. Install Build Tools

**Windows (MinGW):**
- Install **g++ (MinGW-w64) 14.2.0 or higher** and **CMake 3.16 or higher**.
- If `thirdparty/SFML` does not exist, **SFML 3.0.0 pre-built binaries** (~30MB) will be automatically downloaded and extracted into `thirdparty/SFML` on the first build.

**macOS (Homebrew):**
```bash
brew install cmake sfml
```

### 2. Clone the repository

```bash
git clone https://github.com/baoduong2342007/CS202-Group04-FinalProject.git
cd CS202-Group04-FinalProject
```

### 3. Build

**Windows (MinGW):**
```bash
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

**macOS / Linux:**
```bash
cmake -B build
cmake --build build
```

### 4. Run the game

**Windows:**
```bash
.\build\main.exe
```

**macOS / Linux:**
```bash
./build/main
```

> **Note:** The executable is built into the `build/` directory, and all required DLLs (on Windows) as well as the `assets/` and `levels/` folders are automatically copied there during the build.

---

## Directory Structure Summary

```
CS202-Group04-FinalProject/
├── assets/          # Sprite sheets, sounds, background music, fonts
├── include/         # Header files (.h) organized by module (core, entities, states...)
├── src/             # Source files (.cpp) corresponding to include/
├── levels/          # Text files containing level designs (.txt)
├── thirdparty/      # External libraries (SFML binaries are auto-downloaded here on Windows)
└── CMakeLists.txt   # CMake build configuration file
```

For detailed file structure and naming rules, see [`FILE_STRUCTURE.md`](FILE_STRUCTURE.md).

---

## Related Documents

- [`ROLES.md`](ROLES.md) — Detailed role assignments
- [`WEEKLY_PLAN.md`](WEEKLY_PLAN.md) — 6-week plan by member
- [`FILE_STRUCTURE.md`](FILE_STRUCTURE.md) — Folder & file structure
- [`CODING_RULES.md`](CODING_RULES.md) — Coding conventions, Git, naming rules
- [`docs/design_patterns.md`](docs/design_patterns.md) — Description of the 5 design patterns _(written by TV1 in Week 5)_
- [`docs/class_diagram.png`](docs/class_diagram.png) — Class diagram _(Week 6)_

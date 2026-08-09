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
| Build System | CMake 3.16+ |
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
.\build\SuperMario.exe
```

**macOS / Linux:**
```bash
./build/SuperMario
```

> **Note:** The executable is built into the `build/` directory, and all required DLLs (on Windows) as well as the `assets/` and `levels/` folders are automatically copied there during the build.

---

### 5. Build & Run the tests (CTest)

Tests are enabled when `BUILD_TESTING=ON` (Debug and Tests presets). Run them with:

**Windows (MinGW):**
```bash
cmake --preset mingw-tests
cmake --build --preset mingw-tests
ctest --preset mingw-tests --output-on-failure
```

**Manually (any generator):**
```bash
cmake -B build-tests -D BUILD_TESTING=ON
cmake --build build-tests
ctest --test-dir build-tests --output-on-failure
```

Test suites: `input_state_tests`, `tile_collision_span_tests`, `mario_physics_tests`,
`play_state_tests`, `game_manager_tests`, `event_bus_tests`,
`tv5_integration_tests`, `level_catalog_tests`, `save_manager_tests`, and
`level_validator_tests`.

---

## Controls

| Action | Key |
|---|---|
| Move left | `A` / `←` (hold) |
| Move right | `D` / `→` (hold) |
| Jump | `W` / `↑` / `Space` |
| Run (hold) | `LShift` / `RShift` |
| Shoot FireBall (Fire Mario) | `X` (press) |
| Pause | `Esc` |
| Menu / Confirm (in menus) | `Enter` or mouse click |

---

## Directory Structure Summary

```
CS202-Group04-FinalProject/
├── assets/          # Sprite sheets, sounds, background music, fonts
├── include/         # Header files (.h) organized by module (core, entities, states...)
├── src/             # Source files (.cpp) corresponding to include/
├── levels/          # Text files containing level designs (.txt)
├── tests/           # Automated unit tests (CTest)
├── thirdparty/      # External libraries (SFML binaries are auto-downloaded here on Windows)
└── CMakeLists.txt   # CMake build configuration file
```

For detailed file structure and naming rules, see [`FILE_STRUCTURE.md`](FILE_STRUCTURE.md).

---

## Related Documents

- [`docs/management/ROLES.md`](docs/management/ROLES.md) — Detailed role assignments
- [`docs/management/WEEKLY_PLAN.md`](docs/management/WEEKLY_PLAN.md) — 6-week plan by member
- [`FILE_STRUCTURE.md`](FILE_STRUCTURE.md) — Folder & file structure
- [`CODING_RULES.md`](CODING_RULES.md) — Coding conventions, Git, naming rules
- [`docs/design_patterns.md`](docs/design_patterns.md) — Design patterns documentation
- [`docs/class_diagram.md`](docs/class_diagram.md) — Class diagram _(Week 6)_
- [`PLAN.md`](PLAN.md) — Sprint 6 plan (current sprint)

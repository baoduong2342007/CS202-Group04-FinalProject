# Super Mario Bros. — CS202 Group 04 Final Project

**Course**: CS202 — Object-Oriented Programming (APCS)  
**Semester**: HK3 2025-2026  
**Language**: C++17 | **Libraries**: SFML 3.0.0 + Box2D 2.4.1  
**GitHub Repository**: [https://github.com/baoduong2342007/CS202-Group04-FinalProject](https://github.com/baoduong2342007/CS202-Group04-FinalProject)  
**Demo Video**: [https://youtu.be/trNv75ICXMg](https://youtu.be/trNv75ICXMg)  

---

## Submission Contents

| Folder | Content |
|---|---|
| `01_AI_Usage_Declaration/` | AI usage report (Markdown + PDF) |
| `02_Demo_Video/` | Demo video link |
| `03_Report/` | Class diagram, Design patterns, OOP principles (Markdown + PDF) |
| `04_Feature_List/` | Complete list of 106 features |
| `05_Source_Code/` | Full C++ source code with assets |
| `06_Member_Contribution/` | Member contribution spreadsheet |

---

## How to Build & Play

### System Requirements

- **CMake**: 3.16 or newer
- **C++ Compiler**: Supporting C++17
  - **Windows**: MinGW-w64 GCC 14.2+ or MSVC (Visual Studio 2022)
  - **macOS**: Apple Clang / LLVM (Xcode Command Line Tools)
  - **Linux**: GCC 9+ or Clang 10+
- **SFML 3.0.0 & Box2D 2.4.1**: Downloaded and linked automatically via CMake on first build! (Internet connection required for initial configure).

---

### Windows Build Instructions

#### Option A: Using MinGW-w64 (Recommended)

Open **PowerShell** or **Command Prompt**:

```powershell
# Navigate to source code directory
cd "05_Source_Code"

# Generate build files
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Compile
cmake --build build --parallel 4

# Run the game
.\build\SuperMario.exe
```

#### Option B: Using Visual Studio / MSVC

```powershell
cd "05_Source_Code"
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 4
.\build\Release\SuperMario.exe
```

---

### macOS Build Instructions

#### Step 1: Install prerequisites via Homebrew

```bash
brew install cmake sfml
```

#### Step 2: Build and run

```bash
# Navigate to source code directory
cd 05_Source_Code

# Generate build files
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Compile
cmake --build build -j$(sysctl -n hw.ncpu)

# Run the game
./build/SuperMario
```

---

### Linux Build Instructions (Ubuntu / Debian / Arch / Fedora)

#### Step 1: Install dependencies

**Ubuntu / Debian**:
```bash
sudo apt update
sudo apt install -y build-essential cmake libxrandr-dev libxcursor-dev libudev-dev \
    libopenal-dev libflac-dev libvorbis-dev libgl1-mesa-dev libegl1-mesa-dev libfreetype-dev
```

**Fedora**:
```bash
sudo dnf install -y gcc-c++ cmake SFML-devel libXrandr-devel libXcursor-devel \
    systemd-devel openal-soft-devel flac-devel libvorbis-devel mesa-libGL-devel freetype-devel
```

**Arch Linux**:
```bash
sudo pacman -S --needed base-devel cmake sfml
```

#### Step 2: Build and run

```bash
# Navigate to source code directory
cd 05_Source_Code

# Generate build files
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Compile
cmake --build build -j$(nproc)

# Run the game
./build/SuperMario
```

> **Note**: Assets (`assets/`) and level maps (`levels/`) are automatically copied into the `build/` directory during compilation on all platforms.

---

## Controls

### Main Menu

Use **arrow keys** to select a mode, press **Enter** to confirm.

### Single Player

| Action | Key |
|---|---|
| Move left / right | `A` / `D` or ← / → |
| Jump | `W`, ↑, or `Space` |
| Run (sprint) | `Shift` (hold) |
| Shoot fireball (Fire Mario) | `X` |
| Enter pipe | `S` or ↓ |
| Pause | `Esc` |
| **Take Screenshot** | **`F12`** or **`P`** (saved to `screenshots/`) |

### 2-Player Co-op

Both players share one keyboard and cooperate through 4 levels:

| Action | Player 1 | Player 2 |
|---|---|---|
| Move | `A` / `D` | ← / → |
| Jump | `W` / `Space` | ↑ / `Numpad 8` |
| Run | `Left Shift` / `J` | `Right Shift` / `Numpad 1` |
| Shoot fireball | `X` / `F` | `/` / `Numpad 3` |
| Enter pipe | `S` | ↓ / `Numpad 2` |
| Screenshot | `F12` / `P` | `F12` / `P` |

### 2-Player Versus

Best-of-3 stomp duel — land on your opponent's head to score!

| Action | Player 1 | Player 2 |
|---|---|---|
| Move | `A` / `D` | ← / → |
| Jump | `W` | ↑ |
| Run | `Left Shift` | `Right Shift` |
| Shoot fireball | `X` | `/` |
| Screenshot | `F12` / `P` | `F12` / `P` |

A Fire Flower spawns on the center pedestal after 6–12 seconds. Fireballs only stun — you must stomp to score!

---

## Gameplay Tips

- **Stomp on enemies** to defeat them (Goomba, Koopa)
- **Hit `?` blocks** from below to receive items (Coin, Mushroom, Fire Flower)
- **Mushroom** → Mario grows bigger (Super Mario)
- **Fire Flower** → Shoot fireballs to defeat enemies (Fire Mario)
- **Star** → Temporary invincibility, run through enemies
- **Level 4** features Boss Bowser — reach the axe at the end of the bridge to defeat him!
- **Hall of Fame & Records**: View stage high scores and match history from the Main Menu.
- **Auto-save system**: Automatically preserves per-stage records, highest unlocked level, and match logs in `saves/save.txt`.
- **Screenshot capture**: Press `P` or `F12` anytime to export timestamped PNG captures to the `screenshots/` directory.

---

## Team Members

| ID | Student ID | Name | Responsibility |
|---|---|---|---|
| TV1 | 25125009 | Truong Quang Bao Duong | Architecture, states, integration, Factory & Observer patterns |
| TV2 | 25125033 | Nguyen Phan Minh Nhat | Engine, rendering, camera, UI states |
| TV3 | 25125079 | Nguyen Quoc Bao | Mario, physics, collision |
| TV4 | 25125049 | Tran Hong Vy | Levels, enemies, save system |
| TV5 | 25125068 | Le Phi Truyen | Input, sound, HUD, items, Command pattern |

---

## Design Patterns (5/5)

1. **Factory Method** — `EntityFactory` dynamically creates entities from level files
2. **Singleton** — `GameManager`, `SoundManager` ensure a single global instance
3. **Observer** — `EventBus` publishes/subscribes to game events (jump, coin, death...)
4. **State** — `IGameState` manages Menu / LevelSelect / CharacterSelect / Play / Pause / GameOver / Win / Records / PvP transitions
5. **Command** — `ICommand` + `InputHandler` maps keyboard input to player actions

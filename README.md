# Super Mario — CS202 Group 04 Final Project

A C++17 platform game built with SFML 3.0.0 and Box2D 2.4.1.

## Game Overview

A faithful Super Mario Bros recreation featuring 4 worlds with distinct themes:

`Menu -> 1-1 (Overworld) -> 1-2 (Underground) -> 1-3 (Underwater) -> 1-4 (Castle) -> Win`

### Game Modes

- **Single Player**: Classic Mario campaign through 4 levels
- **2-Player Co-op**: Two players cooperate through the campaign
- **2-Player Versus**: Best-of-3 stomp duel on a shared arena

## Team

| Member | Name | Primary area |
|---|---|---|
| TV1 | Duong | Architecture, states, integration |
| TV2 | Nhat | Engine, rendering, camera, UI states |
| TV3 | Bao | Mario, physics, collision |
| TV4 | Vy | Levels, enemies, persistence |
| TV5 | Truyen | Input, sound, HUD, items |

## Requirements

- CMake 3.16 or newer
- A C++17 compiler; MinGW-w64 GCC 14.2 or newer is the supported Windows toolchain
- SFML 3.0.0
- Box2D 2.4.1, obtained through CMake FetchContent

On Windows, the repository can bootstrap the pinned SFML archive if `thirdparty/SFML` is absent. CMake verifies TLS and the archive hash.

## Configure, build, and run

### Windows with MinGW

```powershell
cmake --preset mingw-release
cmake --build --preset mingw-release --parallel 2
.\build-release\SuperMario.exe
```

Debug build:

```powershell
cmake --preset mingw-debug
cmake --build --preset mingw-debug --parallel 2
.\build-debug\SuperMario.exe
```

### macOS or Linux

Install SFML with the platform package manager, then run:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/SuperMario
```

The build copies the runtime `assets` and `levels` directories next to the executable. The incremental copy step only reruns when its inputs change.

## Controls

### Single Player

| Action | Input |
|---|---|
| Move left | `A` or Left Arrow, held |
| Move right | `D` or Right Arrow, held |
| Jump | `W`, Up Arrow, or Space |
| Run | Left Shift or Right Shift, held |
| Shoot as Fire Mario | `X`, pressed |
| Pause | `Esc` |
| Navigate menus | Arrow keys |
| Confirm menu selection | Enter or logical mouse click |

### 2-Player Versus

Best-of-3 stomp duel, both players on one keyboard:

| Action | Player 1 | Player 2 |
|---|---|---|
| Move | `A` / `D` | Left / Right Arrow |
| Jump (tap for short hop) | `W` | Up Arrow |
| Run | Left Shift, held | Right Shift, held |
| Shoot fireball (while on fire) | `X` | `/` |
| Pause | `Esc` | `Esc` |

Only landing on the opponent's head wins a round. The fire flower spawns on
the center pedestal after a random 6-12s and grants a 5-second fire state;
fireballs launch the victim airborne with a short stun instead of scoring.

Clicks in letterbox bars are intentionally ignored. The game renders to a logical 640x360 canvas using centered integer scaling.

## Repository map

| Path | Purpose |
|---|---|
| `include/` | Public C++ headers grouped by module |
| `src/` | Production implementations |
| `levels/` | Release level data |
| `assets/` | Runtime textures, font, sounds, and asset manifest |
| `docs/` | Class diagram, design patterns, and project documentation |
| `thirdparty/` | Local third-party binaries when present |

## Project documentation

- [Coding rules](CODING_RULES.md)
- [File structure](FILE_STRUCTURE.md)
- [Class diagram](docs/class_diagram.md)
- [Design patterns](docs/design_patterns.md)
- [OOP principles and design patterns](docs/oop_principles_and_design_patterns.md)
- [Asset manifest](assets/ASSETS_LIST.md)

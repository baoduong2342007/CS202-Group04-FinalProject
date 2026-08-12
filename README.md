# Super Mario — CS202 Group 04 Final Project

A C++17 platform game built with SFML 3.0.0 and Box2D 2.4.1.

## Sprint 6 release contract

The release flow is locked to:

`Menu -> Level 1 (Overworld) -> Level 2 (Underground) -> Level 3 (Castle) -> Win`

`levels/level0.txt` is a test fixture. `levels/level4.txt` and the underwater theme are future/reference content and are not reachable through the release catalog.

## Team

| Member | Name | Primary area |
|---|---|---|
| TV1 | Dương | Architecture, states, integration |
| TV2 | Nhật | Engine, rendering, camera, UI states |
| TV3 | Bảo | Mario, physics, collision |
| TV4 | Vy | Levels, enemies, persistence |
| TV5 | Truyền | Input, sound, HUD, items |

See [roles](docs/management/ROLES.md) and the [Sprint 6 plan](docs/management/s6_plan.md) for detailed ownership.

## Requirements

- CMake 3.16 or newer
- A C++17 compiler; MinGW-w64 GCC 14.2 or newer is the supported Windows toolchain
- SFML 3.0.0
- Box2D 2.4.1, obtained through CMake FetchContent

On Windows, the repository can bootstrap the pinned SFML archive if `thirdparty/SFML` is absent. CMake verifies TLS and the archive hash.

## Configure, build, and run

### Windows with MinGW

```powershell
cmake --preset mingw-debug
cmake --build --preset mingw-debug --parallel 2
.\build-debug\SuperMario.exe
```

Release build:

```powershell
cmake --preset mingw-release
cmake --build --preset mingw-release --parallel 2
.\build-release\SuperMario.exe
```

### macOS or Linux

Install SFML with the platform package manager, then run:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/SuperMario
```

The build copies the runtime `assets` and `levels` directories next to the executable. The incremental copy step only reruns when its inputs change.

## Tests

```powershell
cmake --preset mingw-tests
cmake --build --preset mingw-tests --parallel 2
ctest --preset mingw-tests --output-on-failure
```

The Sprint 6 candidate contains 17 CTest suites:

- `input_state_tests`
- `tile_collision_span_tests`
- `mario_physics_tests`
- `play_state_tests`
- `game_manager_tests`
- `event_bus_tests`
- `tv5_integration_tests`
- `level_catalog_tests`
- `save_manager_tests`
- `level_validator_tests`
- `springboard_tests`
- `gate0_contract_tests`
- `save_session_tests`
- `sprite_frames_theme_tests`
- `collision_matrix_tests`
- `fireball_request_tests`
- `display_camera_ui_tests`

Automated tests cover the three-level graph, deterministic item blocks, save migration, power-state transitions, collision order/idempotence, FireBall queue/limit/SFX behavior, camera clamping, integer letterboxing, mouse remapping, HUD, audio events, and state transitions. Manual acceptance remains separately recorded in [the release playthrough log](docs/testing/TV4_PLAYTHROUGH_LOG.md).

## Controls

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

Clicks in letterbox bars are intentionally ignored. The game renders to a logical 640x360 canvas using centered integer scaling.

## Repository map

| Path | Purpose |
|---|---|
| `include/` | Public C++ headers grouped by module |
| `src/` | Production implementations and a separately excluded demo |
| `levels/` | Release level data plus labeled test/future fixtures |
| `assets/` | Runtime textures, font, sounds, and asset manifest |
| `tests/` | Automated CTest executables |
| `docs/` | Design, management, specification, and acceptance evidence |
| `thirdparty/` | Local third-party binaries when present |

The detailed, current tree is in [FILE_STRUCTURE.md](FILE_STRUCTURE.md).

## Project documentation

- [Coding rules](CODING_RULES.md)
- [Class diagram](docs/class_diagram.md)
- [Design patterns](docs/design_patterns.md)
- [Asset manifest](assets/ASSETS_LIST.md)
- [Sprint 6 audit tracker](docs/management/S6_AUDIT_TRACKER.md)
- [Sprint 6 bug register](docs/management/S6_BUG_REGISTER.md)
- [Sprint 6 integration report](docs/management/TV1_CHANGES_SUMMARY.md)
- [Sprint 6 remediation plan](s6_fix_plan_v4.md)

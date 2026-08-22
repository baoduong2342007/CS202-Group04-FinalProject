# Super Mario — CS202 Group 04 Final Project

A C++17 platform game built with SFML 3.0.0 and Box2D 2.4.1.

## Sprint 6 release contract (historical)

The following graph was the Sprint 6 candidate contract and is retained for
history. It is superseded by the Sprint 7 addendum below; it is not current RC
evidence.

The release flow is locked to:

`Menu -> Level 1 (Overworld) -> Level 2 (Underground) -> Level 3 (Castle) -> Win`

`levels/level0.txt` is a test fixture. In this historical snapshot,
`levels/level4.txt` and the underwater theme were future/reference content.

## Sprint 7 current contract addendum (2026-08-16)

The current catalog metadata contract has four public entries:

`Menu -> 1 (1-1 Overworld) -> 2 (1-2 Underground) -> 3 (1-3 Underwater) -> 4 (1-4 Castle) -> Win`

Exact file, world-label, theme, music, and camera metadata are covered by
[`LevelCatalogTests.cpp`](tests/LevelCatalogTests.cpp) and
[`Gate0ContractTests.cpp`](tests/Gate0ContractTests.cpp). Retry restarts a
failed level (1–4), Level Select enforces `highestUnlockedLevel`, and the v1
save schema keeps level bounds monotonic; see [`S7_TV1_TV5_STATUS.md`](docs/management/S7_TV1_TV5_STATUS.md).

Level 2 and Level 3 start in the Overworld before their warp/area transition;
their dominant themes are Underground and Underwater respectively. Automated
catalog metadata and progression checks are verified. Physical visual play
(GUI, audio, and video) remains a separate manual acceptance activity.

## Team

| Member | Name | Primary area |
|---|---|---|
| TV1 | Duong | Architecture, states, integration |
| TV2 | Nhat | Engine, rendering, camera, UI states |
| TV3 | Bao | Mario, physics, collision |
| TV4 | Vy | Levels, enemies, persistence |
| TV5 | Truyen | Input, sound, HUD, items |

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

The Sprint 6 candidate's test inventory is historical and is not the current
release contract:

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
- `character_flow_tests`
- `stomp_score_tests`
- `pvp_arena_tests`
- `pvp_stomp_tests`
- `pvp_flow_tests`

The current source registers 37 CTest suites. Fresh external Debug and
Release `BUILD_TESTING=ON` runs each passed 37/37. These automated results do
not replace manual GUI, audio, or video acceptance; those activities remain
separate evidence recorded in [the release playthrough log](docs/testing/TV4_PLAYTHROUGH_LOG.md).
The complete registration source is [CMakeLists.txt](CMakeLists.txt).

## Controls

Single player:

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

2 PLAYER VERSUS (best-of-3 stomp duel, both players on one keyboard):

| Action | Player 1 | Player 2 |
|---|---|---|
| Move | `A` / `D` | Left / Right Arrow |
| Jump (tap for short hop) | `W` | Up Arrow |
| Run | Left Shift, held | Right Shift, held |
| Shoot fireball (while on fire) | `X` | `/` |
| Pause | `Esc` | `Esc` |

Only landing on the opponent's head wins a round. The fire flower spawns on
the center pedestal after a random 6-12s (never while a flower is on the
field or someone is on fire) and grants a 5-second fire state; fireballs
launch the victim airborne with a short stun instead of scoring.

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
- [Sprint 6 plan](docs/management/s6_plan.md)
- [Sprint 7 TV1/TV5 status matrix](docs/management/S7_TV1_TV5_STATUS.md)
- [Sprint 7 runtime package manifest](docs/management/S7_TV5_PACKAGE_MANIFEST.md)

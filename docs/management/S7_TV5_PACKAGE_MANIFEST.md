# S7-TV5 Runtime Package Manifest

This manifest covers the automated packaging portion of `S7-TV5-03` and provides
the input inventory for `S7-TV5-05`. CMake copies exactly 43 runtime assets and
7 level/config files (50 package files) from the entries below, keeping the
listed package-relative path next to the `SuperMario` executable. It does not
include `level0.txt`, Future assets, or `docs/assets/reference/` files.

This is not a manual audio, screenshot, hash, or final release sign-off.  Those
checks remain `PENDING` for the TV5-05 evidence owner until they are run against
one frozen candidate.

## Inventory

Source paths are repository-relative.  Package paths are relative to the
executable/package root and intentionally retain the `assets/` or `levels/`
prefix used by runtime code.

| Source path | Package-relative path | Category | Status | Runtime use / notes |
|---|---|---|---|---|
| `assets/ASSETS_LIST.md` | `assets/ASSETS_LIST.md` | Package metadata | Required input | Runtime inventory/usage metadata |
| `assets/fonts/mario.ttf` | `assets/fonts/mario.ttf` | Font | Required input | HUD and state UI font |
| `assets/ui/bg_world.png` | `assets/ui/bg_world.png` | UI background | Required input | Overworld background |
| `assets/textures/ui/hud.png` | `assets/textures/ui/hud.png` | UI texture | Required input | Menu and HUD bitmap |
| `assets/textures/ui/stage_1.png` | `assets/textures/ui/stage_1.png` | UI preview | Required input | Level Select World 1-1 preview |
| `assets/textures/ui/stage_2.png` | `assets/textures/ui/stage_2.png` | UI preview | Required input | Level Select World 1-2 preview |
| `assets/textures/ui/stage_3.png` | `assets/textures/ui/stage_3.png` | UI preview | Required input | Level Select World 1-3 preview |
| `assets/textures/ui/stage_4.png` | `assets/textures/ui/stage_4.png` | UI preview | Required input | Level Select World 1-4 preview |
| `assets/textures/mario/MarioLuigi.png` | `assets/textures/mario/MarioLuigi.png` | Character texture | Required input | Mario/Luigi spritesheet |
| `assets/textures/tiles/tileset.png` | `assets/textures/tiles/tileset.png` | Tile texture | Required input | Theme tile catalog |
| `assets/textures/items/items_blocks.png` | `assets/textures/items/items_blocks.png` | Item texture | Required input | Question/used block frames |
| `assets/textures/items/items_objects.png` | `assets/textures/items/items_objects.png` | Item texture | Required input | Coin, mushroom, flower, star, and object frames |
| `assets/textures/enemies/enemies.png` | `assets/textures/enemies/enemies.png` | Enemy texture | Required input | Goomba/Koopa/Cheep Cheep atlas |
| `assets/textures/enemies/goomba.png` | `assets/textures/enemies/goomba.png` | Enemy fixture texture | Required input | Deterministic Goomba/package coverage |
| `assets/textures/enemies/koopa.png` | `assets/textures/enemies/koopa.png` | Enemy fixture texture | Required input | Deterministic Koopa/package coverage |
| `assets/textures/enemies/hammer.png` | `assets/textures/enemies/hammer.png` | Projectile texture | Required input | Hammer projectile loaded by `Hammer` |
| `assets/sounds/effects/1up.wav` | `assets/sounds/effects/1up.wav` | SFX | Required input | One-up cue |
| `assets/sounds/effects/brick.wav` | `assets/sounds/effects/brick.wav` | SFX | Required input | Brick break cue |
| `assets/sounds/effects/bump.wav` | `assets/sounds/effects/bump.wav` | SFX | Required input | Block bump cue |
| `assets/sounds/effects/coin.wav` | `assets/sounds/effects/coin.wav` | SFX | Required input | Coin collection cue |
| `assets/sounds/effects/death.wav` | `assets/sounds/effects/death.wav` | SFX | Required input | Player death cue |
| `assets/sounds/effects/fire.wav` | `assets/sounds/effects/fire.wav` | SFX | Required input | Bowser fire cue |
| `assets/sounds/effects/fireball.wav` | `assets/sounds/effects/fireball.wav` | SFX | Required input | Fireball cue |
| `assets/sounds/effects/bowserfall.wav` | `assets/sounds/effects/bowserfall.wav` | SFX | Required input | Bowser defeat/fall cue |
| `assets/sounds/effects/flagpole.wav` | `assets/sounds/effects/flagpole.wav` | SFX | Required input | Level completion cue |
| `assets/sounds/effects/gameover.wav` | `assets/sounds/effects/gameover.wav` | SFX | Required input | Game-over cue |
| `assets/sounds/effects/hurryup.wav` | `assets/sounds/effects/hurryup.wav` | SFX | Required input | Low-time warning cue |
| `assets/sounds/effects/item.wav` | `assets/sounds/effects/item.wav` | SFX | Required input | Item emergence cue |
| `assets/sounds/effects/jump.wav` | `assets/sounds/effects/jump.wav` | SFX | Required input | Jump cue |
| `assets/sounds/effects/jumpsmall.wav` | `assets/sounds/effects/jumpsmall.wav` | SFX | Required input | Small-jump catalog variant |
| `assets/sounds/effects/kickkill.wav` | `assets/sounds/effects/kickkill.wav` | SFX | Required input | Shell-kick/defeat source cue |
| `assets/sounds/effects/pause.wav` | `assets/sounds/effects/pause.wav` | SFX | Required input | Pause cue |
| `assets/sounds/effects/pipepowerdown.wav` | `assets/sounds/effects/pipepowerdown.wav` | SFX | Required input | Power-down cue |
| `assets/sounds/effects/powerup.wav` | `assets/sounds/effects/powerup.wav` | SFX | Required input | Power-up cue |
| `assets/sounds/effects/stompswim.wav` | `assets/sounds/effects/stompswim.wav` | SFX | Required input | Enemy stomp/swim cue |
| `assets/sounds/music/overworld.flac` | `assets/sounds/music/overworld.flac` | Music | Required input | Level 1 / Overworld; also initial spawn music for Levels 2 and 3 |
| `assets/sounds/music/underground.flac` | `assets/sounds/music/underground.flac` | Music | Required input | Level 2 dominant/stage track after warp; not the initial spawn track |
| `assets/sounds/music/underwater.flac` | `assets/sounds/music/underwater.flac` | Music | Required input | Level 3 dominant/stage track after warp; playback/sequence remains a manual gate |
| `assets/sounds/music/castle.flac` | `assets/sounds/music/castle.flac` | Music | Required input | Level 4 / Castle |
| `assets/sounds/music/invincible.flac` | `assets/sounds/music/invincible.flac` | Music | Required input | Star override |
| `assets/sounds/music/death.flac` | `assets/sounds/music/death.flac` | Music | Required input | Death interruption |
| `assets/sounds/music/gameover.flac` | `assets/sounds/music/gameover.flac` | Music | Required input | Game-over interruption |
| `assets/sounds/music/level_complete.flac` | `assets/sounds/music/level_complete.flac` | Music | Required input | Win/completion interruption |
| `levels/level1.txt` | `levels/level1.txt` | Level data | Required input | Release Level 1 map |
| `levels/level2.txt` | `levels/level2.txt` | Level data | Required input | Release Level 2 map |
| `levels/level3.txt` | `levels/level3.txt` | Level data | Required input | Release Level 3 map |
| `levels/level4.txt` | `levels/level4.txt` | Level data | Required input | Release Level 4 map |
| `levels/cheep_cheep.txt` | `levels/cheep_cheep.txt` | Runtime config | Optional runtime config (included) | Cheep Cheep registry; runtime tolerates absence |
| `levels/elevators.txt` | `levels/elevators.txt` | Runtime config | Optional runtime config (included) | Elevator registry; runtime tolerates absence |
| `levels/pvp_arena.txt` | `levels/pvp_arena.txt` | Runtime config | Required input | PvP arena loaded directly by `PvpPlayState` |

### Source and license note

The repository has usage/dimension notes in [assets/ASSETS_LIST.md](../../assets/ASSETS_LIST.md),
but no per-file provenance or license record is included in this package
artifact. Source attribution and redistribution permission are therefore
`BLOCKED` for final RC evidence; see [THIRD_PARTY_NOTICES.md](../../THIRD_PARTY_NOTICES.md).
This manifest does not infer or invent a license.

## Evidence template

Record the command/method and exact result when each check is run.  A clean
isolated directory is required so stale files cannot satisfy the inventory.
Rows without recorded evidence remain `PENDING`.

| Check | Status | Evidence / result |
|---|---|---|
| CMake configure validates every allowlisted source path | `VERIFIED (automated)` | Fresh external MinGW Makefiles Debug/Release configure passed; existence checks in `CMakeLists.txt` validate the allowlist, which is the packaging source of truth. |
| SuperMario Debug package contains exactly the allowlist | `VERIFIED (automated)` | Fresh Debug `all`, `SuperMario`, and `CopyRuntimeAssets` builds passed; inventory comparison produced 50 expected / 50 actual (43 assets + 7 level/config), with no Future/Reference extras. |
| SuperMario Release package contains exactly the allowlist | `VERIFIED (automated)` | Fresh Release `all`, `SuperMario`, and `CopyRuntimeAssets` builds passed; inventory comparison produced 50 expected / 50 actual (43 assets + 7 level/config), with no Future/Reference extras. |
| Debug and Release CTest suites | `VERIFIED (automated)` | `ctest --test-dir <root> --output-on-failure` passed 37/37 in each root; the P4 matrix records the tested contracts. |
| Existing test configuration remains valid | `VERIFIED (configure smoke)` | Clean Debug configure with `BUILD_TESTING=ON` completed successfully; rerun with the repository's existing test configuration. |
| Missing-input configure failure is clear and path-specific | `VERIFIED (negative test)` | Controlled negative configure with one allowlisted input absent emitted the expected path-specific `Required runtime ... missing or is not a file` fatal diagnostic; these diagnostics are expected negative tests, not release failures. |
| Audio-device output and interruption/resume behavior | `PENDING` | TV5-05 device evidence reference: ____________________ |
| HUD/item screenshots and manual route evidence | `PENDING` | TV5-05 screenshot/playthrough reference: ____________________ |
| Candidate hash and final package sign-off | `PENDING` | No final RC commit/hash exists. Worktree fingerprint is evidence identity only, not a commit or RC hash. |

The external evidence snapshot is cited for traceability and is not a durable
repository artifact:
`C:\Users\ASUS\AppData\Local\Temp\supermario-p4-final-20260821-025948\evidence\`.
See `p4-criterion-matrix.md`, `package-inventory-audit.txt`,
`debug-ctest-full.log`, `release-ctest-full.log`, `log-scan.txt`, and
`startup-smoke.txt`. The recorded command sequence was
`cmake -S . -B <root> -G "MinGW Makefiles" -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=<Debug|Release>`;
`cmake --build <root> --target all`; `cmake --build <root> --target SuperMario CopyRuntimeAssets`;
and `ctest --test-dir <root> --output-on-failure` in each root. Release startup
smoke ran for 8 seconds and was safely terminated with zero package filesystem
changes.

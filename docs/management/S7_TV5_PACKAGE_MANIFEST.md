# S7-TV5 Runtime Package Manifest

This manifest covers the automated packaging portion of `S7-TV5-03` and provides
the input inventory for `S7-TV5-05`.  CMake copies only the entries below, keeping
the listed package-relative path next to the `SuperMario` executable.  It does not
include `level0.txt`, future assets, or `docs/assets/reference/` files.

This is not a manual audio, screenshot, hash, or final release sign-off.  Those
checks remain `PENDING` for the TV5-05 evidence owner until they are run against
one frozen candidate.

## Inventory

Source paths are repository-relative.  Package paths are relative to the
executable/package root and intentionally retain the `assets/` or `levels/`
prefix used by runtime code.

| Source path | Package-relative path | Category | Status | Runtime use / notes |
|---|---|---|---|---|
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
| `assets/sounds/effects/1up.wav` | `assets/sounds/effects/1up.wav` | SFX | Required input | One-up cue |
| `assets/sounds/effects/brick.wav` | `assets/sounds/effects/brick.wav` | SFX | Required input | Brick break cue |
| `assets/sounds/effects/bump.wav` | `assets/sounds/effects/bump.wav` | SFX | Required input | Block bump cue |
| `assets/sounds/effects/coin.wav` | `assets/sounds/effects/coin.wav` | SFX | Required input | Coin collection cue |
| `assets/sounds/effects/death.wav` | `assets/sounds/effects/death.wav` | SFX | Required input | Player death cue |
| `assets/sounds/effects/fireball.wav` | `assets/sounds/effects/fireball.wav` | SFX | Required input | Fireball cue |
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
| `assets/sounds/music/overworld.flac` | `assets/sounds/music/overworld.flac` | Music | Required input | Level 1 / Overworld |
| `assets/sounds/music/underground.flac` | `assets/sounds/music/underground.flac` | Music | Required input | Level 2 / Underground |
| `assets/sounds/music/underwater.flac` | `assets/sounds/music/underwater.flac` | Music | Required input | Level 3 / Underwater |
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

### Source and license note

The repository has usage/dimension notes in `assets/ASSETS_LIST.md`, but no
per-file provenance or license record is included in this package artifact.
Source attribution and license compatibility are therefore `PENDING` for the
final RC evidence; this manifest does not infer or invent a license.

## Evidence template

Record the command/method and exact result when each check is run.  A clean
isolated directory is required so stale files cannot satisfy the inventory.
Rows without recorded evidence remain `PENDING`.

| Check | Status | Evidence / result |
|---|---|---|
| CMake configure validates every allowlisted source path | `VERIFIED (automated)` | Clean Debug/Release configure using the local dependency cache; method is the existence checks in `CMakeLists.txt` at the runtime packaging section (the allowlist is the source of truth). |
| SuperMario Debug package contains exactly the allowlist | `VERIFIED (automated)` | Clean Debug build plus inventory comparison produced 43 expected / 43 actual; excluded examples absent and no `docs/` output. Compare package files against the allowlist and this manifest table. |
| SuperMario Release package contains exactly the allowlist | `VERIFIED (automated)` | Clean Release build plus the same inventory comparison produced 43 expected / 43 actual; excluded examples absent and no `docs/` output. Compare package files against the allowlist and this manifest table. |
| Existing test configuration remains valid | `VERIFIED (configure smoke)` | Clean Debug configure with `BUILD_TESTING=ON` completed successfully; rerun with the repository's existing test configuration. |
| Missing-input configure failure is clear and path-specific | `VERIFIED (independent tester)` | Criterion-E negative configure on an isolated source copy with one allowlisted input absent emitted the expected path-specific `Required runtime ... missing or is not a file` fatal diagnostic; see the tester's retained criterion-E run log. |
| Audio-device output and interruption/resume behavior | `PENDING` | TV5-05 device evidence reference: ____________________ |
| HUD/item screenshots and manual route evidence | `PENDING` | TV5-05 screenshot/playthrough reference: ____________________ |
| Candidate hash and final package sign-off | `PENDING` | TV5-05/TV1 frozen-candidate reference: ____________________ |

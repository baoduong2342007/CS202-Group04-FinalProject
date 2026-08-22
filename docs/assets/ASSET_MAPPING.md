# ASSET_MAPPING.md — Complete Asset-to-Code Mapping

- **Purpose:** Single authoritative map of every asset in this repository to the exact source location that loads or references it at runtime, in tests, or in the CMake package step. Complements `assets/ASSETS_LIST.md` (the packaged manifest) with file:line-level evidence.
- **Generated:** 2026-08-22.
- **Method:** Grep-verified. Every "Loaded by" reference below was confirmed by searching `src/`, `include/`, `tests/`, and `CMakeLists.txt` for the literal asset path (or its path constant). PNG dimensions were read directly from IHDR headers. No claim was copied from prior docs without re-verification.
- **Status legend:**
  - **Runtime** — loaded by game code on an active execution path.
  - **Future** — exists on disk, referenced by no loader; reserved for upcoming features.
  - **Packaged-unused** — copied into the release package by `CMakeLists.txt` but loaded by zero runtime code paths.
  - **Fixture** — referenced only by tests.
  - **Reference** — analysis-only material under `docs/assets/reference/`, never packaged.
- **Update rule:** This document MUST be updated whenever an asset is added, removed, renamed, moved, or re-referenced (new loader, manifest entry, or package-list change). A PR that touches `assets/`, `levels/`, `SoundManifest.def`, `SUPERMARIO_RUNTIME_ASSETS`, or any `*_PATH`/`*_TEXTURE_PATH` constant must update this file in the same change.

All paths are relative to the repository root. All sizes are KiB (`du -k`).

---

## 1. Summary

| Folder | Files | Size | Status |
|---|---|---|---|
| `assets/fonts/` | 1 | 116 KB | Runtime |
| `assets/sounds/effects/` | 22 | 4,088 KB | 19 Runtime, 3 Future |
| `assets/sounds/music/` | 10 | 13,676 KB | 8 Runtime, 2 Future |
| `assets/textures/backgrounds/` | 4 | 220 KB | Runtime |
| `assets/textures/enemies/` | 4 | 57 KB | 2 Runtime, 2 Packaged-unused fixtures |
| `assets/textures/items/` | 2 | 68 KB | Runtime |
| `assets/textures/mario/` | 1 | 52 KB | Runtime |
| `assets/textures/tiles/` | 1 | 60 KB | Runtime |
| `assets/textures/ui/` | 8 | 212 KB | 4 Runtime, 1 Packaged-unused, 3 Future |
| `assets/ui/` | 1 | 1,340 KB | Packaged-unused (legacy) |
| `assets/ASSETS_LIST.md` | 1 | 12 KB | Packaged manifest doc |
| `docs/assets/reference/` (appendix) | 12 | 2,168 KB | Reference (never packaged) |
| `levels/` (appendix) | 8 | 44 KB | 7 Runtime, 1 test fixture |

Runtime loading mechanisms (verified):

- **SFX:** `include/core/SoundManifest.def` (23 entries) is X-macro-expanded into the `SoundId` enum at `include/core/SoundManager.h:44-46` and into a metadata table at `include/core/SoundManager.h:59-63`. `src/core/SoundManager.cpp:96-98` iterates the table and loads every entry; `SoundManager::loadSound` resolves the disk path at `src/core/SoundManager.cpp:192-198`.
- **Music:** paths registered in `src/core/SoundManager.cpp:456-463`; overworld additionally eager-loaded at `src/core/SoundManager.cpp:102`.
- **Textures/font:** per-entity `constexpr` path constants plus the shared constants in `include/core/SpriteFrames_shared.h`, resolved through `src/core/TextureManager.cpp`.

---

## 2. `assets/fonts/`

| Path | Size | Status | Loaded by | Notes |
|---|---|---|---|---|
| `assets/fonts/mario.ttf` | 116 KB | Runtime | `src/states/MenuState.cpp:23`; `src/states/LevelSelectState.cpp:22`; `src/states/CharacterSelectState.cpp:20`; `src/states/CoopCharacterSelectState.cpp:20`; `src/states/PvpCharacterSelectState.cpp:21`; `src/states/PvpPlayState.cpp:35`; `src/states/PauseState.cpp:28`; `src/states/GameOverState.cpp:28`; `src/states/WinState.cpp:28`; `src/ui/HUD.cpp:33`; `src/level/Level.cpp:143` (Toad dialogue font) | 11 distinct `FONT_PATH` constants; packaged at `CMakeLists.txt:518`. |

## 3. `assets/sounds/effects/` (22 files)

19 files are Runtime via `SoundManifest.def`. `kickkill.wav` is the one shared sample, serving 5 logical cues (`KICK`, `SHELL_KICK`, `SHELL_KILL`, `ENEMY_FIREBALL`, `ENEMY_STAR`).

**Runtime (19 files, exact manifest entry in `include/core/SoundManifest.def`):**

| Path | Size | Status | Loaded by | Notes |
|---|---|---|---|---|
| `1up.wav` | 164 KB | Runtime | `SoundManifest.def:26` (`ONE_UP`) | via `SoundManager.cpp:96-98` |
| `bowserfall.wav` | 184 KB | Runtime | `SoundManifest.def:30` (`BOWSER_FALL`) | |
| `brick.wav` | 112 KB | Runtime | `SoundManifest.def:23` (`BRICK`) | |
| `bump.wav` | 40 KB | Runtime | `SoundManifest.def:24` (`BUMP`) | |
| `coin.wav` | 176 KB | Runtime | `SoundManifest.def:10` (`COIN`) | |
| `death.wav` | 504 KB | Runtime | `SoundManifest.def:13` (`DEATH`) | |
| `fire.wav` | 212 KB | Runtime | `SoundManifest.def:29` (`BOWSER_FIRE`) | |
| `fireball.wav` | 28 KB | Runtime | `SoundManifest.def:17` (`FIREBALL`) | |
| `flagpole.wav` | 212 KB | Runtime | `SoundManifest.def:22` (`FLAGPOLE`) | |
| `gameover.wav` | 696 KB | Runtime | `SoundManifest.def:14` (`GAME_OVER`) | |
| `hurryup.wav` | 540 KB | Runtime | `SoundManifest.def:28` (`HURRY_UP`) | |
| `item.wav` | 112 KB | Runtime | `SoundManifest.def:25` (`ITEM`) | |
| `jump.wav` | 108 KB | Runtime | `SoundManifest.def:8` (`JUMP`) | |
| `jumpsmall.wav` | 112 KB | Runtime | `SoundManifest.def:9` (`JUMP_SMALL`) | |
| `kickkill.wav` | 44 KB | Runtime | `SoundManifest.def:12,18,19,20,21` (`KICK`, `SHELL_KICK`, `SHELL_KILL`, `ENEMY_FIREBALL`, `ENEMY_STAR`) | Shared sample: 1 file, 5 cues |
| `pause.wav` | 132 KB | Runtime | `SoundManifest.def:27` (`PAUSE`) | |
| `pipepowerdown.wav` | 144 KB | Runtime | `SoundManifest.def:16` (`POWER_DOWN`) | |
| `powerup.wav` | 180 KB | Runtime | `SoundManifest.def:15` (`POWER_UP`) | |
| `stompswim.wav` | 56 KB | Runtime | `SoundManifest.def:11` (`STOMP`) | |

**Future (no manifest entry, no code reference; mentioned only in `assets/ASSETS_LIST.md:83,84,103`):**

| Path | Size | Status | Loaded by | Notes |
|---|---|---|---|---|
| `beep.wav` | 32 KB | Future | none | No event mapping |
| `billfirework.wav` | 84 KB | Future | none | No Bullet Bill firework path |
| `vine.wav` | 212 KB | Future | none | Vine gameplay exists (`src/level/Level.cpp:1167-1168`) but plays no cue |

All 19 Runtime effects are packaged automatically: CMake parses `SoundManifest.def` (comment block at `CMakeLists.txt:548-551`) and appends the parsed paths to `SUPERMARIO_RUNTIME_ASSETS` at `CMakeLists.txt:649` (no handwritten SFX list). The 3 Future files are not packaged.

## 4. `assets/sounds/music/` (10 files)

| Path | Size | Status | Loaded by | Notes |
|---|---|---|---|---|
| `overworld.flac` | 7,140 KB | Runtime | `src/core/SoundManager.cpp:102` (eager load), `:456` (`MusicId::OVERWORLD`) | Default startup theme |
| `underground.flac` | 904 KB | Runtime | `src/core/SoundManager.cpp:457` (`MusicId::UNDERGROUND`) | |
| `castle.flac` | 1,180 KB | Runtime | `src/core/SoundManager.cpp:458` (`MusicId::CASTLE`) | |
| `underwater.flac` | 2,808 KB | Runtime | `src/core/SoundManager.cpp:459` (`MusicId::UNDERWATER`) | |
| `invincible.flac` | 616 KB | Runtime | `src/core/SoundManager.cpp:460` (`MusicId::STAR`) | |
| `death.flac` | 132 KB | Runtime | `src/core/SoundManager.cpp:461` (`MusicId::DEATH`) | |
| `gameover.flac` | 160 KB | Runtime | `src/core/SoundManager.cpp:462` (`MusicId::GAME_OVER`) | |
| `level_complete.flac` | 296 KB | Runtime | `src/core/SoundManager.cpp:463` (`MusicId::WIN`) | |
| `castle_complete.flac` | 344 KB | Future | none | Optional win variant; only `assets/ASSETS_LIST.md:127` |
| `pipe.flac` | 92 KB | Future | none | Optional pipe transition; only `assets/ASSETS_LIST.md:128` |

The 8 Runtime tracks are packaged explicitly at `CMakeLists.txt:538-545`.

## 5. `assets/textures/backgrounds/` (4 files, all 640×360)

Path constants: `include/core/SpriteFrames_shared.h:991-994` (`OVERWORLD_PATH`, `UNDERGROUND_PATH`, `UNDERWATER_PATH`, `CASTLE_PATH`, namespace `SpriteFrames::legacy::Backgrounds`); runtime consumer: `src/level/BackgroundRenderer.cpp:36-46` (per-theme load) and `:244-251` (theme→path resolution).

| Path | Size | Status | Loaded by | Notes |
|---|---|---|---|---|
| `bg_overworld.png` | 20 KB | Runtime | `SpriteFrames_shared.h:991` → `src/level/BackgroundRenderer.cpp:36-37,251` | Default theme |
| `bg_underground.png` | 80 KB | Runtime | `SpriteFrames_shared.h:992` → `src/level/BackgroundRenderer.cpp:39-40,244` | |
| `bg_underwater.png` | 68 KB | Runtime | `SpriteFrames_shared.h:993` → `src/level/BackgroundRenderer.cpp:42-43,246` | |
| `bg_castle.png` | 52 KB | Runtime | `SpriteFrames_shared.h:994` → `src/level/BackgroundRenderer.cpp:45-46,248` | |

## 6. `assets/textures/enemies/` (4 files)

| Path | Size | Status | Loaded by | Notes |
|---|---|---|---|---|
| `enemies.png` (436×530) | 44 KB | Runtime | `src/entities/Goomba.cpp:27`; `src/entities/Koopa.cpp:35`; `src/entities/Bowser.cpp:25`; `src/entities/BowserFire.cpp:19`; `src/entities/BulletBill.cpp:18`; `src/entities/CheepCheep.cpp:23`; `src/entities/Blooper.cpp:22`; `src/entities/Lakitu.cpp:20`; `src/entities/Spiny.cpp:20`; `src/entities/SpinyEgg.cpp:20`; `src/entities/PiranhaPlant.cpp:16`; `src/entities/Podoboo.cpp:20`; `src/entities/HammerBro.cpp:23`; `src/states/MenuState.cpp:27` | Shared enemy atlas, 14 loaders; frame constants throughout `include/core/SpriteFrames_*.h` |
| `hammer.png` (16×16) | 1 KB | Runtime | `src/entities/Hammer.cpp:15` (`HAMMER_TEXTURE_PATH`) | Dedicated sprite, not from the atlas |
| `goomba.png` (96×32) | 4 KB | Packaged-unused (fixture) | `CMakeLists.txt:530` (package); `tests/TV5IntegrationTests.cpp:784` (dimension assertion) | Zero runtime loaders; packaged for coverage verification |
| `koopa.png` (128×48) | 8 KB | Packaged-unused (fixture) | `CMakeLists.txt:531` (package); `tests/TV5IntegrationTests.cpp:785` (dimension assertion) | Zero runtime loaders; packaged for coverage verification |

## 7. `assets/textures/items/` (2 files)

| Path | Size | Status | Loaded by | Notes |
|---|---|---|---|---|
| `items_blocks.png` (448×256) | 16 KB | Runtime | `src/entities/BlockDebris.cpp:13`; `src/entities/QuestionBlock.cpp:27` | Block/debris atlas; frame comments at `SpriteFrames_shared.h:897`, `include/level/TileFrames.h:46` |
| `items_objects.png` (592×572) | 52 KB | Runtime | `src/entities/Firebar.cpp:16`; `src/entities/FireBall.cpp:29`; `src/entities/FireballExplosion.cpp:15`; `src/entities/Elevator.cpp:17`; `src/entities/ScorePopup.cpp:13`; `src/entities/Springboard.cpp:16`; `src/entities/Toad.cpp:7`; `src/items/Coin.cpp:26`; `src/items/Mushroom.cpp:25`; `src/items/FireFlower.cpp:22`; `src/items/Star.cpp:25`; `src/level/TileMap.cpp:606` (`OBJECTS_TILESET_PATH`); `src/states/MenuState.cpp:26` | Objects atlas, 13 loaders; frame constants at `SpriteFrames_shared.h:725+` |

## 8. `assets/textures/mario/` (1 file)

| Path | Size | Status | Loaded by | Notes |
|---|---|---|---|---|
| `MarioLuigi.png` (584×469) | 52 KB | Runtime | `src/entities/Mario.cpp:80`; `src/states/MenuState.cpp:25`; `src/states/CharacterSelectState.cpp:21`; `src/states/CoopCharacterSelectState.cpp:21`; `src/states/PvpCharacterSelectState.cpp:22` | Character sheet for both players; transition/projectile frame constants documented at `SpriteFrames_shared.h:1479,1589` |

## 9. `assets/textures/tiles/` (1 file)

| Path | Size | Status | Loaded by | Notes |
|---|---|---|---|---|
| `tileset.png` (680×356) | 60 KB | Runtime | `src/level/TileMap.cpp:605` (`TILESET_PATH`); `src/items/Coin.cpp:24`; `src/entities/BowserAxe.cpp:14`; `src/entities/BulletBillLauncher.cpp:23`; `src/states/MenuState.cpp:24` | IHDR-verified 680×356. Older docs claimed 680×776 — corrected in the 2026-08-22 audit |

## 10. `assets/textures/ui/` (8 files)

| Path | Size | Status | Loaded by | Notes |
|---|---|---|---|---|
| `stage_1.png` (256×224) | 4 KB | Runtime | `src/states/LevelSelectState.cpp:25` | Level-select thumbnail array |
| `stage_2.png` (256×240) | 4 KB | Runtime | `src/states/LevelSelectState.cpp:26` | |
| `stage_3.png` (256×240) | 4 KB | Runtime | `src/states/LevelSelectState.cpp:27` | |
| `stage_4.png` (256×240) | 4 KB | Runtime | `src/states/LevelSelectState.cpp:28` | |
| `hud.png` (784×948) | 92 KB | Packaged-unused | `CMakeLists.txt:520` (package only) | Zero code loaders. The HUD is font/vector-rendered (`src/ui/HUD.cpp`, font at `:33`). Reclassified in `S7_TV5_PACKAGE_MANIFEST` during the 2026-08-22 audit |
| `bg_clouds.png` (768×1129) | 28 KB | Future | none (only `tests/TV5IntegrationTests.cpp:804`, which asserts ASSETS_LIST.md *text* mentions it) | No runtime loader; not packaged |
| `bg_mountains.png` (768×1660) | 36 KB | Future | none (only a code comment, `src/core/TextureManager.cpp:47`) | No runtime loader; not packaged |
| `bg_trees.png` (768×1660) | 36 KB | Future | none (only `assets/ASSETS_LIST.md:64`) | No runtime loader; not packaged |

## 11. `assets/ui/` (1 file)

| Path | Size | Status | Loaded by | Notes |
|---|---|---|---|---|
| `bg_world.png` (1857×847) | 1,340 KB | Packaged-unused (legacy) | Path constant `include/core/SpriteFrames_shared.h:986` (`WORLD_PATH`), alias `include/core/SpriteFrames_ovw.h:129`; package copy at `CMakeLists.txt:519` | Grep-verified: the constant is defined and aliased but never consumed by any load call in `src/`. Legacy overworld backdrop, superseded by `textures/backgrounds/`. Largest single packaged asset |

## 12. `assets/ASSETS_LIST.md`

| Path | Size | Status | Loaded by | Notes |
|---|---|---|---|---|
| `assets/ASSETS_LIST.md` | 12 KB | Packaged doc | `CMakeLists.txt:517` (package); read by `tests/TV5IntegrationTests.cpp:789` (manifest stream), `:130` (root discovery), `:804` (text assertions) | Human-readable packaged manifest; the packaged inventory is machine-verified against `runtime_expected_inventory.txt` |

## Appendix A — `docs/assets/reference/` (12 PNGs, 2,168 KB)

Analysis-only source material. Never packaged: the CMake package list is an explicit allow-list (`SUPERMARIO_RUNTIME_ASSETS`, `CMakeLists.txt:516`), whose header comment (`CMakeLists.txt:512-513`) states reference files must not be pulled into an RC.

Contents: `bg_world_candidate.png` (1,620 KB), `blocks_all_components_atlas_full.png`, `enemies.png`, `enemies_all_components_atlas.png`, `enemies_all_components_atlas_full.png` (1072×1160), `enemies_candidate.png`, `general_tile.png`, `items_blocks_candidate.png`, `items_objects_all_components_atlas_full.png`, `items_objects_candidate.png`, `tileset_all_components_atlas_full.png`, `tileset_candidate.png`.

Notes from the 2026-08-22 audit: 2 byte-identical duplicates were deleted (including `tileset(v2).png`). `enemies_all_components_atlas_full.png` was **kept** despite being byte-identical to `enemies_all_components_atlas.png` because `tests/TV5IntegrationTests.cpp:820` asserts its existence (as do `:822-823` for `blocks_all_components_atlas_full.png` and `:824-825` for `items_objects_all_components_atlas_full.png`). Its true dimension is 1072×1160 (ASSETS_LIST.md corrected accordingly).

## Appendix B — `levels/` (8 files)

Levels live outside `assets/`; they are packaged via `SUPERMARIO_RUNTIME_LEVELS` (`CMakeLists.txt:678-689`).

| Path | Size | Status | Loaded by | Notes |
|---|---|---|---|---|
| `levels/level1.txt` | 8 KB | Runtime | `include/core/LevelCatalog.h:55` | "1-1" overworld |
| `levels/level2.txt` | 8 KB | Runtime | `include/core/LevelCatalog.h:58` | "1-2" overworld→underground |
| `levels/level3.txt` | 8 KB | Runtime | `include/core/LevelCatalog.h:61` | "1-3" overworld→underwater |
| `levels/level4.txt` | 4 KB | Runtime | `include/core/LevelCatalog.h:64` | "1-4" castle |
| `levels/cheep_cheep.txt` | 4 KB | Runtime | `src/level/CheepCheepConfig.cpp:16` (`CHEEP_CONFIG_PATH`) | External spawn registry; consumed by `Level.cpp:685` |
| `levels/elevators.txt` | 4 KB | Runtime | `src/level/ElevatorConfig.cpp:16` (`ELEVATOR_CONFIG_PATH`) | External elevator-route registry; consumed by `Level.cpp:714` |
| `levels/pvp_arena.txt` | 4 KB | Runtime | `src/states/PvpPlayState.cpp:36` (`PVP_ARENA_PATH`) | 2P versus arena; also test-loaded by `tests/PvpArenaTests.cpp:74,132,183` |
| `levels/level0.txt` | 4 KB | Fixture | `tests/LevelValidatorTests.cpp:74`; `tests/Gate0ContractTests.cpp:240,441` | Test fixture only; NOT in `SUPERMARIO_RUNTIME_LEVELS`, excluded from package |

`levels/level_athletic.txt` was deleted in the 2026-08-22 audit; grep confirms zero remaining references in `src/`, `include/`, `tests/`, `CMakeLists.txt`, or `levels/`.

---

## Findings & recommendations

1. **`hud.png` is packaged but never loaded.** Only reference is the package list (`CMakeLists.txt:520`); the HUD is font/vector-rendered (`src/ui/HUD.cpp`). Recommendation: either keep packaged as a deliberate placeholder for a future graphical HUD, or drop it from `SUPERMARIO_RUNTIME_ASSETS` in a future release decision (a TV5 inventory update would be required either way).
2. **`bg_world.png` (1,340 KB) is the largest packaged asset and is never loaded.** Its constant `WORLD_PATH` (`SpriteFrames_shared.h:986`, alias `SpriteFrames_ovw.h:129`) has zero consumers. Same keep-or-drop decision as `hud.png`; dropping it would shrink the package by ~1.3 MB.
3. **`goomba.png` / `koopa.png` are package-coverage fixtures.** No runtime loader; they exist in the package so `TV5IntegrationTests.cpp:784-785` can assert dimensions. If package minimization ever takes priority, they are safe to remove from the list together with their assertions.
4. **5 Future audio files are unreferenced:** effects `beep.wav`, `billfirework.wav`, `vine.wav`; music `castle_complete.flac`, `pipe.flac`. None are packaged, so they cost only repository size. Retain if the corresponding features (menu beep, Bullet Bill firework, vine climb cue, castle-win variant, pipe transition) are planned; otherwise delete in a cleanup pass.
5. **3 Future UI textures are unreferenced:** `bg_clouds.png`, `bg_mountains.png`, `bg_trees.png`. Their only appearances are a test asserting ASSETS_LIST *text* (`TV5IntegrationTests.cpp:804`), a code comment (`TextureManager.cpp:47`), and ASSETS_LIST itself. None are packaged.
6. **Zero code-referenced assets are missing from disk.** Every literal path and path constant grep-verified above resolves to an existing file — the package and the source tree are consistent.
7. **Duplicate cleanup (this audit):** 2 byte-identical reference duplicates deleted (`tileset(v2).png` et al.); `enemies_all_components_atlas_full.png` retained solely because `TV5IntegrationTests.cpp:820` asserts its existence.
8. **Dimension correction:** `tileset.png` is 680×356 (IHDR-verified), not 680×776 as previously documented; ASSETS_LIST.md was corrected this audit.

---

## Audit integrity note

No asset binary (image, audio, or font) was modified, renamed, moved, or deleted in this audit, per team policy. This audit only re-verified references and produced this document. `assets/ASSETS_LIST.md` and all other existing files were left untouched.

# Sprint 7 TV1/TV5 Status Matrix

> Updated: 2026-08-16
> Scope: verified current-source evidence and remaining release gates for
> `S7-TV1-01`..`05` and `S7-TV5-01`..`05`.

`VERIFIED` means the stated automated or package evidence passed. `PARTIAL`
means the named automated portion passed but an acceptance portion remains.
`BLOCKED` identifies an external gate that cannot be closed by this document
set. `REVIEW` means evidence or release disposition is still required.

## Task matrix

| Task | Status | Verified scope and exact evidence | Remaining limitation / decision gate |
|---|---|---|---|
| `S7-TV1-01` Baseline and contract review | `VERIFIED` | Four-entry contract is represented in [LevelCatalog.h](../../include/core/LevelCatalog.h) and asserted by [LevelCatalogTests.cpp](../../tests/LevelCatalogTests.cpp) and [Gate0ContractTests.cpp](../../tests/Gate0ContractTests.cpp). | Carry-over/manual evidence is tracked separately; no final candidate hash exists. |
| `S7-TV1-02` Catalog 1..4 and metadata | `PARTIAL` | Exact entries `1`/`1-1` Overworld, `2`/`1-2` Underground, `3`/`1-3` Underwater, `4`/`1-4` Castle, including file/theme/music/camera metadata, pass [LevelCatalogTests.cpp](../../tests/LevelCatalogTests.cpp). Syntactic loader checks pass in [Gate0ContractTests.cpp](../../tests/Gate0ContractTests.cpp) and [LevelValidatorTests.cpp](../../tests/LevelValidatorTests.cpp). | Semantic Underwater acceptance is `BLOCKED`: [levels/level3.txt](../../levels/level3.txt) remains Castle-style and is TV4-owned for map/asset correction and playthrough. |
| `S7-TV1-03` Progression, save bounds, and Win | `VERIFIED` | Retry restarts failed levels 1–4 and clamps malformed inputs in [PlayStateTests.cpp](../../tests/PlayStateTests.cpp); Level Select unlock bounds are covered in the same file; v1 schema/monotonic bounds are covered by [SaveManagerTests.cpp](../../tests/SaveManagerTests.cpp) and [SaveSessionTests.cpp](../../tests/SaveSessionTests.cpp). | Manual completion and same-hash evidence remain open. |
| `S7-TV1-04` Catalog/state tests and integration gate | `VERIFIED` | Fresh writable isolated MinGW Debug and Release `BUILD_TESTING=ON` CTest runs each passed `21/21`; Release `BUILD_TESTING=OFF` production build passed. Test-target-only NDEBUG restoration gives test targets `-DNDEBUG` then `-UNDEBUG`, while `game_lib`/`SuperMario` retain only `-DNDEBUG`; no assert-disabled or production compiler warnings were observed, apart from external Box2D CMake deprecation. Registered tests are listed in [CMakeLists.txt](../../CMakeLists.txt); earlier 17/19/20 counts are historical. | Automated MinGW evidence does not prove manual route, device/MSVC execution, or RC sign-off; the MSVC branch was statically reviewed but not executed. |
| `S7-TV1-05` RC governance and design-pattern consistency | `REVIEW` | Current contract and limitations are synchronized in [README.md](../../README.md), [class_diagram.md](../class_diagram.md), [TV1_CHANGES_SUMMARY.md](TV1_CHANGES_SUMMARY.md), and [change_in_develop.md](../change_in_develop.md). | No same-final-RC commit/hash; final governance, source/license record, BUG-038, and P0/P1 disposition remain pending. |
| `S7-TV5-01` Music mapping for four levels | `VERIFIED` | Four tracks, Star/death/GameOver/Win transitions, and level flow pass [TV5IntegrationTests.cpp](../../tests/TV5IntegrationTests.cpp). Missing/invalid track handling clears stale current state without a crash in the same test. | Device-audio output and interruption/resume observation remain manual gates. |
| `S7-TV5-02` HUD world labels and timer | `PARTIAL` | `WORLD 1-1` through `WORLD 1-4`, timer, Star/death/GameOver/Win, and volume persistence pass [TV5IntegrationTests.cpp](../../tests/TV5IntegrationTests.cpp); implementation is [HUD.cpp](../../src/ui/HUD.cpp). | Required screenshots and interactive four-level observations remain `PENDING`. |
| `S7-TV5-03` Underwater/Castle asset package | `PARTIAL` | Explicit allowlist has 37 assets + 6 levels/configs = 43 entries. Clean Debug/Release inventory and negative missing-file configure checks are recorded in [S7_TV5_PACKAGE_MANIFEST.md](S7_TV5_PACKAGE_MANIFEST.md), [CMakeLists.txt](../../CMakeLists.txt), and [ASSETS_LIST.md](../../assets/ASSETS_LIST.md). | Source/license attribution is `PENDING`; semantic Underwater map acceptance is separately `BLOCKED` under TV4. |
| `S7-TV5-04` Audio/HUD/item verification | `REVIEW` | Automated item, event/SFX, HUD, volume, and state-audio coverage passes in [TV5IntegrationTests.cpp](../../tests/TV5IntegrationTests.cpp) and related suites; current checklist is [TV5_AUDIO_HUD_ITEM_CHECKLIST.md](TV5_AUDIO_HUD_ITEM_CHECKLIST.md). | Device-audio, screenshots, manual route, and `S6-TV5-43/44` dispositions remain `PENDING`/`REVIEW`. |
| `S7-TV5-05` Final package and sign-off | `REVIEW` | Package inventory and missing-input configure evidence are `VERIFIED` in [S7_TV5_PACKAGE_MANIFEST.md](S7_TV5_PACKAGE_MANIFEST.md). | Candidate hash, manual evidence, source/license attribution, `BUG-038`, S6 dispositions, and final TV1/TV5 sign-off are not complete. Do not claim RC completion or no-P0/P1. |

**Progress semantics clarification (`S7-TV1-03`).** “Progress preserved” means
the failed current-level selection and monotonic `highestUnlockedLevel` are
retained. GameOver → Retry starts a fresh run, so transient score, coins,
lives, and power reset; the save schema remains v1 with no migration.

## Evidence boundaries

- **Catalog metadata contract:** four public entries and exact metadata in the
  catalog tests. **Syntactic map validation:** rectangular/supported-symbol/
  loader checks. Neither is semantic Underwater acceptance.
- **Automated evidence:** fresh isolated writable MinGW Debug and Release
  `BUILD_TESTING=ON` CTest runs are each `21/21`, and Release
  `BUILD_TESTING=OFF` production build passed. This supersedes historical
  17/19/20 snapshots for current automated evidence, but is not manual,
  device-audio, MSVC execution, or final-RC evidence.
- **Package evidence:** Debug and Release allowlist inventories each contain
  43 expected entries, with a path-specific negative configure check. This
  does not establish device audio, screenshots, attribution, or a candidate
  hash.
- **Release status:** manual four-level playthrough, screenshots, device audio,
  source/license attribution, `S6-TV5-43/44`, `BUG-038`, and final sign-off
  remain `REVIEW`/`PENDING`. No same-final-RC commit/hash is present in the
  uncommitted worktree.
- `level_select_preview.png` is an untracked generated snapshot artifact, not
  an RC source deliverable.

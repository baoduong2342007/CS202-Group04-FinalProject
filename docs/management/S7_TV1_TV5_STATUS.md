# Sprint 7 TV1/TV5 Status Matrix

> Updated: 2026-08-21
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
| `S7-TV1-02` Catalog 1..4 and metadata | `PARTIAL` | Exact entries `1`/`1-1` Overworld, `2`/`1-2` Underground, `3`/`1-3` Underwater, `4`/`1-4` Castle, including file/theme/music/camera metadata, pass [LevelCatalogTests.cpp](../../tests/LevelCatalogTests.cpp). For Levels 2 and 3, `initialTheme` and initial music are `OVERWORLD` at spawn; Underground/Underwater are the dominant stage themes after warp. Syntactic loader checks and Level 3 initial/dominant-theme semantic probes pass in [Gate0ContractTests.cpp](../../tests/Gate0ContractTests.cpp), [LevelValidatorTests.cpp](../../tests/LevelValidatorTests.cpp), and the P4 criterion matrix. | Interactive/visual Underwater acceptance remains `PENDING`; automated theme semantics do not substitute for a human visual review. |
| `S7-TV1-03` Progression, save bounds, and Win | `VERIFIED` | Retry restarts failed levels 1–4 and clamps malformed inputs in [PlayStateTests.cpp](../../tests/PlayStateTests.cpp); Level Select unlock bounds are covered in the same file; v1 schema/monotonic bounds are covered by [SaveManagerTests.cpp](../../tests/SaveManagerTests.cpp) and [SaveSessionTests.cpp](../../tests/SaveSessionTests.cpp). | Manual completion and same-hash evidence remain open. |
| `S7-TV1-04` Catalog/state tests and integration gate | `VERIFIED` | Fresh external MinGW Debug and Release roots each configured, built `all`, built `SuperMario` and `CopyRuntimeAssets`, and passed `37/37` CTest. The matrix covers L1→L4 progression/goals, movement/pipes/collision, enemies/Bowser/projectiles, all items, death/save/HUD/transitions, co-op/PvP, and package/manifest checks. | Automated MinGW evidence does not prove physical listening, interactive GUI playthrough, visual review, device/MSVC execution, or RC sign-off. |
| `S7-TV1-05` RC governance and design-pattern consistency | `REVIEW` | Current contract and limitations are synchronized in [README.md](../../README.md), [class_diagram.md](../class_diagram.md), [TV1_CHANGES_SUMMARY.md](TV1_CHANGES_SUMMARY.md), and [change_in_develop.md](../change_in_develop.md). | No same-final-RC commit/hash; final governance, source/license record, BUG-038, and P0/P1 disposition remain pending. |
| `S7-TV5-01` Music mapping for four levels | `PARTIAL` | SoundManifest/package presence and Star/death/GameOver/Win transitions pass automated coverage. Current [TV5IntegrationTests.cpp](../../tests/TV5IntegrationTests.cpp) directly asserts only Overworld, Overworld, and Castle transitions; it does not directly assert Underwater playback or a complete four-level music sequence. | Underwater playback/sequence, device-audio output, and interruption/resume observation remain `PENDING`/manual gates. |
| `S7-TV5-02` HUD world labels and timer | `PARTIAL` | `WORLD 1-1` through `WORLD 1-4`, timer, Star/death/GameOver/Win, and volume persistence pass [TV5IntegrationTests.cpp](../../tests/TV5IntegrationTests.cpp); implementation is [HUD.cpp](../../src/ui/HUD.cpp). | Required screenshots and interactive four-level observations remain `PENDING`. |
| `S7-TV5-03` Underwater/Castle asset package | `PARTIAL` | Explicit allowlist has 43 assets + 7 levels/configs = 50 files. Clean Debug/Release inventory contains exactly 50 each, required fire/Bowser/goomba/koopa/hammer assets are present, and no Future/Reference extras are present; see [S7_TV5_PACKAGE_MANIFEST.md](S7_TV5_PACKAGE_MANIFEST.md), [CMakeLists.txt](../../CMakeLists.txt), and [ASSETS_LIST.md](../../assets/ASSETS_LIST.md). | Per-file source/license attribution and redistribution are `BLOCKED`; see [THIRD_PARTY_NOTICES.md](../../THIRD_PARTY_NOTICES.md). Interactive/visual Underwater acceptance remains `PENDING`. |
| `S7-TV5-04` Audio/HUD/item verification | `REVIEW` | Automated item, event/SFX, HUD, volume, and state-audio coverage passes in [TV5IntegrationTests.cpp](../../tests/TV5IntegrationTests.cpp) and related suites; current checklist is [TV5_AUDIO_HUD_ITEM_CHECKLIST.md](TV5_AUDIO_HUD_ITEM_CHECKLIST.md). | Device-audio, screenshots, manual route, and `S6-TV5-43/44` dispositions remain `PENDING`/`REVIEW`. |
| `S7-TV5-05` Final package and sign-off | `REVIEW` | Package inventory and controlled missing-input configure evidence are `VERIFIED` in [S7_TV5_PACKAGE_MANIFEST.md](S7_TV5_PACKAGE_MANIFEST.md); Release startup smoke ran for 8 seconds and was safely terminated with no package filesystem change. | No final RC commit/hash exists; the validated worktree fingerprint is not a commit or RC hash. Manual audio/GUI/visual evidence, per-file attribution, `BUG-038`, S6 dispositions, and final TV1/TV5 sign-off remain open. Do not claim RC completion or no-P0/P1. |

**Progress semantics clarification (`S7-TV1-03`).** “Progress preserved” means
the failed current-level selection and monotonic `highestUnlockedLevel` are
retained. GameOver → Retry starts a fresh run, so transient score, coins,
lives, and power reset; the save schema remains v1 with no migration.

## Evidence boundaries

- **Catalog metadata contract:** four public entries and exact metadata in the
  catalog tests. **Syntactic map validation:** rectangular/supported-symbol/
  loader checks. The P4 matrix also records passing Level 3
  initial/dominant-theme semantic probes; none of these automated checks is a
  substitute for interactive/visual Underwater acceptance.
- **Automated evidence:** fresh external MinGW Debug and Release roots each
  configured, built `all`, built `SuperMario` and `CopyRuntimeAssets`, and passed
  `37/37` CTest. The P4 matrix covers L1→L4 progression/goals, movement/pipes,
  collision/enemies/Bowser/projectiles, all items, death/save/HUD/transitions,
  co-op/PvP, and package/manifest checks. This is not physical listening,
  interactive GUI playthrough, visual review, MSVC execution, or final-RC evidence.
- **Package evidence:** Debug and Release inventories each contain exactly 50
  files: 43 assets (including package metadata) plus 7 level/config files. The
  required fire/Bowser/goomba/koopa/hammer paths are present; Future/Reference
  extras are absent; controlled missing-input diagnostics are negative tests.
  This does not establish attribution, redistribution permission, or a candidate
  hash.
- **Release status:** manual four-level playthrough, screenshots/video, physical
  audio, visual review, `S6-TV5-43/44`, `BUG-038`, per-file source/license
  attribution, and final sign-off remain `PENDING`/`BLOCKED`. No same-final-RC
  commit/hash is present in the uncommitted worktree. Fingerprint
  `df57eee2bda743329debbfadc95a20f25563bfb4aebe1c6b86178e2b8ae1a331` is a
  validated worktree fingerprint only, not a commit or final-RC hash.
- Generated previews are not RC source deliverables or release evidence unless
  explicitly tied to the immutable candidate record.

## P4 external evidence reference

The dated evidence snapshot is external to the repository and is cited for
traceability, not treated as durable project content:
`C:\Users\ASUS\AppData\Local\Temp\supermario-p4-final-20260821-025948\evidence\`.
Relevant records are `p4-criterion-matrix.md`, `debug-ctest-full.log`,
`release-ctest-full.log`, `package-inventory-audit.txt`, `log-scan.txt`,
`startup-smoke.txt`, and `worktree-fingerprint.txt`. The recorded command
sequence for each fresh root was
`cmake -S . -B <root> -G "MinGW Makefiles" -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=<Debug|Release>`;
`cmake --build <root> --target all`; `cmake --build <root> --target SuperMario CopyRuntimeAssets`;
then `ctest --test-dir <root> --output-on-failure`. The Release smoke was a
bounded 8-second start.

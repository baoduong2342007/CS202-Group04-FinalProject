# Develop Integration Log
Note: entries #1–#73 are historical (Vietnamese); from entry #74 onward entries are written in English.

This file summarizes important integration checkpoints. Git history remains the authoritative record of individual commits and merges. Historical test counts are not current release evidence.

| Date | Checkpoint | Result | Notes |
|---|---|---|---|
| 2026-08-25 | [TV5] Merge `feature/sound-input` into `main` | CTest 38/38 PASS | Merged TV5's video script refinements (`docs/DEMO_VIDEO_SCRIPT.md`) with comprehensive recording walkthrough, authentic SMB1 victory routes, control mappings for P1/P2 in Single, Co-op, and Versus modes, and video production guidelines. All 38/38 CTest suites pass (100%). |
| 2026-08-24 | [TV1] Comprehensive Markdown Audit & Quality Remediation | CTest 38/38 PASS | Audited and verified all 56 markdown files across the repository: (1) Resolved all UTF-8 encoding corruptions and mojibake across historical entries and romanized Vietnamese headers in docs/change_in_develop.md. (2) Fixed unclosed nested code block in docs/management/ROLES.md. (3) Synchronized CTest suite count from 37 to 38 across README.md, FILE_STRUCTURE.md, and agent_docs. (4) URL-encoded path parentheses (%282025-2026%29) across agent_docs to ensure valid Markdown link parsing. (5) Fixed LaTeX math formatting and unescaped delimiters in docs/change_in_develop.md and agent_docs/project_core_tech.md. (6) Cleaned corrupted shell echo residue between Entry 66 and 67 in change_in_develop.md. Full CTest suite passes 38/38 (100%). |
| 2026-08-24 | [TV1] Review remediation: Bowser Co-op shockwave, Cannon state leakage, and Document recovery | Manual Verification PASS | (1) Fixed critical state leakage bug in Level.cpp where `m_hasBowserEncounter` persisted across level reloads causing permanent cannon cease-fire. (2) Resolved dangling `Mario*` pointers in Bowser.cpp to prevent use-after-free crashes on respawn. (3) Upgraded Bowser's shockwave stun to affect both players simultaneously in Co-op mode. (4) Fixed `patrol()` hardcoded frame time to use dynamic `dt` for consistent pacing regardless of refresh rate. (5) Recovered `change_in_develop.md` from mojibake corruption and broken LaTeX escape sequences. (6) Unified magic numbers, include order, and `@author` tags across Bowser and BulletBill entities. |
| 2026-08-24 | [TV1] Documentation remediation: fix em-dash mojibake and anchor links | CTest 38/38 PASS | Fixed corrupted em-dash byte sequences (`—` -> `—`) across `docs/design_patterns.md`, `docs/oop_principles_and_design_patterns.md`, and `docs/class_diagram.md`. Fixed stale anchor link `#L224` -> `#L281` for `PlayState::onEnter` evidence in `docs/class_diagram.md`. All 38/38 CTest suites pass 100%. |
| 2026-08-23 | [TV1] Review remediation: Bowser hammer-residue cleanup, dead branch removal, const-correctness, docs resync | CTest 38/38 PASS | (1) `include/entities/Bowser.h` + `src/entities/Bowser.cpp`: removed the write-only `m_hammerVariant` field/constructor parameter and the orphaned `THROW_ANIMATION` registration plus `throwFrames()` helper; zero callers passed the variant and no code played the animation, so behavior is unchanged. (2) `src/physics/CollisionManager.cpp`: collapsed the always-true `else if (Enemy* enemy = otherParticipant.enemy())` re-check into a plain `else` and deleted the unreachable trailing power-down branch in the Mario lateral-collision path. (3) `src/level/Level.cpp`: `nearestEligiblePlayer()` now returns the non-owning `Mario*` directly, eliminating the `const_cast` formerly needed to feed `Bowser::updateMarioTarget()`. (4) Docs: `docs/tileset_coordinate.md` gains a Themed Cannon Cells table matching `BulletBillLauncher::getCannonParts()`; `docs/DEMO_VIDEO_SCRIPT.md` World 1-4 chapter rewritten for the current boss fight (rotating cannons, directional Bullet Bills, shockwave stun, enrage twin fire, boss-gated Toad ending); re-synced all drifted `file:line` evidence anchors in `design_patterns.md`, `oop_principles_and_design_patterns.md`, and `class_diagram.md`; documented the Bullet Bill direct-damage exception. No gameplay changes; 38/38 CTest suites pass. |
| 2026-08-23 | [TV4/TV3/TV1] Advanced Enemy AI (Aggro Pursuit, Agile Hops, Predictive Drops, Boss Trajectories) & Co-op Boost Jump | CTest 38/38 PASS | (1) Advanced Enemy AI: Implemented proximity/LoS-based aggro chase mode across ground enemies (`Goomba`, `Spiny`) with +30% pursuit speed boost and agile surprise hops within close range; enhanced `HammerBro` with dynamic multi-tier jumping and faster defensive throwing cadence; enhanced `Lakitu` with velocity tracking and predictive lead egg tossing; enhanced `Blooper` with diagonal swoop pulses towards Mario; enhanced `Bowser` with player altitude-adaptive multi-height fire trajectories and airborne interception jumping. (2) Co-op Head-Bounce Boost Jump: Integrated cooperative boost jumping in `Level::updateCoop` and `CollisionManager`: landing on a co-op partner's head grants full upward jump velocity without friendly-fire damage, accompanied by jump audio. Added regression tests in `tests/CoopFlowTests.cpp` (`testCoopHeadBounceBoostJump`). (38/38 CTest suites pass 100%). |
| 2026-08-23 | [TV2/TV1] Revert Camera System to Clean Original Baseline (Pre-6e78353) | CTest 38/38 PASS | Restored clean original Camera baseline (`include/level/Camera.h`, `src/level/Camera.cpp`, `include/core/LevelCatalog.h`, `src/level/Level.cpp`): (1) Restored `CameraVerticalMode` enum to `LOCKED` (bottom-pinned) and `DEAD_ZONE` (`TOP_FOLLOW_DISTANCE = 32.0f`, pans upward when Mario/target rises within 32px of the top edge and returns smoothly to original bottom resting view upon landing). (2) Restored 2-way horizontal follow with standard 5% horizontal deadzone (`CAMPAIGN_HORIZONTAL_DEADZONE = 0.05f`) in single-player and 0.0f in co-op. (3) Removed complex experimental modes (`TRACK`, `setVerticalEdgeMarginRatio`, `setVerticalSettleEnabled`, `setVerticalFrozen`) and monotonic backward scroll locking. (4) Updated test suite assertions across `tests/LevelCatalogTests.cpp`, `tests/CoopFlowTests.cpp`, and `tests/DisplayCameraUITests.cpp`. All 38/38 CTest test suites pass (100%). |
| 2026-08-23 | [TV2/TV3] Camera Feel Pass: Edge-Margin Vertical Follow, Airborne Hold, Idle Settle & Co-op Fit-Both Bias + Invisible Ceiling | CTest 38/38 PASS | (1) Vertical TRACK follow rewritten from stable-center dead-zone to EDGE-MARGIN minimal-shift (`Camera::update`): the view pans only when the target comes within `verticalEdgeMarginRatio` (0.15 → ~54px) of the CURRENT top/bottom edge and shifts just far enough to restore it. A standard jump or a platform well inside the frame never scrolls the ground out of view; standing on a high platform keeps maximum ground visible below. (2) AIRBORNE HOLD (`setVerticalFrozen`, campaign only): while Mario is off the ground and not vine-climbing, the view applies NO vertical motion — jump arcs, springboard launches and fast falls never scroll the screen; the frame re-anchors through the edge-margin rule only after landing. The flagpole sequence keeps following. Base-load clears the latched flag after its pre-render `update(0.f)` so co-op/PvP start clean. (3) Campaign-only idle settle (`setVerticalSettleEnabled`, SETTLE_IDLE_DELAY 0.35s, SETTLE_RATE 4/s, rest bias −40px): standing still eases the view into a rest framing that keeps Mario slightly below center so the ground under his feet stays framed. Co-op explicitly disables settle. (4) Co-op midpoint uses the same edge-margin rule on Y (horizontal stays instant for edge-stuck pull-along). (5) New `Level::computeCoopCameraTarget()` clamps the follow target into the range that keeps BOTH partners framed, favoring the lower partner beyond one viewport of separation. (6) Invisible-ceiling guard in `clampCoopPlayersToCamera()`: gap > view height − 2×`COOP_VIEW_EDGE_MARGIN` cancels the higher climber's upward velocity (no teleport, falling never interrupted). (7) World-space warning arrow via `renderCoopSeparationWarning()` at 75% of the limit. (8) Tests: rewrote `testCoopCameraTracksVerticalMidpointAndSticksAtEdges` on a tall synthetic fixture; added `testCoopInvisibleCeilingCapsVerticalSeparation` and `testCampaignAirborneVerticalHold` (view is pixel-frozen during a full jump arc). All camera constants named in `src/level/Level.cpp` / `src/level/Camera.cpp`. 38/38 CTest suites pass (100%). |
| 2026-08-23 | [TV3/TV1] Jump Buffering (120ms Window) & Synchronized Multi-Player Jump Edge | CTest 38/38 PASS | Implemented 120ms jump buffer (`m_jumpBufferTimer`) in `include/entities/Mario.h` and `src/entities/Mario.cpp` (`preparePhysics`, `applyGroundPhysics`, `jump`). Eliminates dropped jump inputs when pressing jump a few frames before touching ground or right after landings. Synchronized rising-edge `m_pressedThisFrame` in `InputState::syncHardware()` to guarantee simultaneous co-op jumps execute reliably without dropped events. (38/38 CTest suites pass 100%). |
| 2026-08-23 | [TV5/TV1] Real-time Hardware Keyboard Synchronization & Anti-Key-Stuck Protection | CTest 38/38 PASS | Implemented `InputState::syncHardware()` with direct `sf::Keyboard::isKeyPressed()` physical polling during active window focus in `src/core/Game.cpp`. Automatically detects and clears any stuck/ghost held keys if `KeyReleased` events were missed across transitions, state pushes, or focus shifts. Added intent resets in `PlayState::onPause()` and `PlayState::onResume()`. (38/38 CTest suites pass 100%). |
| 2026-08-23 | [TV5/TV1] Co-op Input Responsiveness & Flexible Key Bindings Expansion | CTest 38/38 PASS | Expanded and decoupled 2-Player Co-op key bindings in `src/states/PlayState.cpp`: (1) Player 1 supports `A`/`D` (move), `W`/`Space` (jump), `S` (pipe), `LShift`/`J` (run), `X`/`F` (fireball); (2) Player 2 supports `Left`/`Right`/`Num4`/`Num6` (move), `Up`/`Num8`/`Num0`/`Enter` (jump), `Down`/`Num2` (pipe), `RShift`/`Num1`/`RControl`/`M` (run), `Slash`/`Period`/`Num3` (fireball). Updated `processCoopInput` and `dispatchPlayer` to seamlessly handle multi-key jump releases and run holding without keyboard ghosting conflicts or stuttering. (38/38 CTest suites pass 100%). |
| 2026-08-23 | [TV2/TV3] Camera Deadzone Stability Window for 4-Way Tracking | CTest 38/38 PASS | Implemented comfortable inner deadzone thresholds (`m_horizontalDeadzoneRatio = 0.10f`, `m_verticalDeadzoneRatio = 0.20f`) in single-player campaign mode (`include/level/Camera.h`, `src/level/Level.cpp`). The camera only pans vertically/horizontally when Mario nears the screen margins, preventing vertical camera jitter/bounce during standard jumps and keeping the ground and underlying enemies fully visible at all times. Co-op preserves exact dual-player midpoint tracking (`0.0f` deadzones). (38/38 CTest suites pass 100%). |
| 2026-08-23 | [TV4/TV1] Restore Smart Enemy Ledge Detection & Breakout AI across All Species | CTest 38/38 PASS | Restored smart platform ledge awareness (`isApproachingLedge()` using `isEnemySupport()`) and `turnsAtLedge() = true` across all ground-walking enemies (`Goomba`, `Koopa`, `Paratroopa`, `Spiny`, `BuzzyBeetle`, `HammerBro`, `RedKoopa`). Enemies smoothly reverse when reaching pit/platform boundaries instead of falling blindly, while preserving narrow-range oscillation breakout AI (`isEscapingNarrowRange()`) towards Mario. Updated unit tests in `tests/KoopaVariantTests.cpp` (38/38 CTest suites pass 100%). |
| 2026-08-23 | [TV1/TV3] 4-Way Camera Follow (Single-player & Co-op) + Co-op Dual Pipe Warp & Flagpole Completion | CTest 38/38 PASS | (1) 4-Way Camera Follow: Single-player campaign and 2-Player Co-op freely track Mario and Luigi in all 4 directions (Left, Right, Up, Down) without backward lock. Set `CameraVerticalMode::TRACK` with zero vertical deadzone across release levels (`include/core/LevelCatalog.h`, `include/level/Level.h`, `src/level/Level.cpp`). (2) Co-op Shared Viewport & Dual Transition Safety: Co-op mode continuously tracks both players within a shared viewport (`clampCoopPlayersToCamera`). (3) Co-op Dual Pipe Warp: Entering warp pipes in co-op requires both players to be positioned on the pipe mouth (±4px margin) and both pressing Down (or Right for horizontal entry); both players descend simultaneously, teleport together, and emerge out of the exit pipe together (`Level::startPipeWarpCoop`, `updatePipeWarp`, `warpMarioToReturn`). (4) Co-op Dual Flagpole Completion: Both players must reach the finish trigger; both slide down the flagpole and walk side-by-side into the castle before firing `LEVEL_COMPLETED` (`checkFinishFlag`, `updateFlagSequence`). (5) Automated Test Suite: Added `testCoopDualPipeWarpRequiresBothPlayers` and `testCoopDualFlagpoleCompletion` to `tests/CoopFlowTests.cpp`; all 38/38 test suites pass (100%). |
| 2026-08-22 | [TV3/TV1] Canonical gameplay fixes D2-D7: stomp held-jump bounce, Bowser star immunity, monotonic campaign camera, Piranha radius, flagpole score, dynamic_cast audit | CTest 36/37 PASS (1 pre-existing packaging failure, see notes) | (1) D2 src/physics/CollisionManager.cpp stomp bounce: holding Space/Up/W at bounce applies the character jump velocity (Mario::getJumpForce(), 460/510) instead of the fixed 300/200, mirroring the springboard held-key pattern. (2) D3 src/entities/Bowser.cpp onStarHit() is now a no-op (canonical SMB1: star never harms Bowser; only 5 fireballs or the axe); CollisionManager star path skips multi-hit bosses (getFireballHealth() > 0) so no kill/score/event transaction is created; tests/BowserTests.cpp testBowserStarAndAxeKill split into testBowserStarIsHarmless + testBowserAxeCollapseKill. (3) D4 include/level/Camera.h + src/level/Camera.cpp: new setMonotonicScroll(bool), the stable center x never decreases; src/level/Level.cpp + include/level/Level.h: enabled only for the single-player campaign (!coop && !pvp), coop/PvP keep free follow; new Level::clampCampaignPlayerToCameraLeft() holds Mario at the view left edge (mirrors clampCoopPlayersToCamera); docs/management/S6_LOCKED_INTERFACES.md documents the new camera-mode axis; tests/P2GameplayInterfaceTests.cpp Lakitu/fireball probe phases reordered forward-only. (4) D5 include/entities/PiranhaPlant.h PROXIMITY_RADIUS 20->40 (plant stays hidden while Mario is on/adjacent to the pipe). (5) D6 src/level/Level.cpp checkFinishFlag() awards canonical grab-height tiers 100/400/800/2000/5000 through Mario::queueScoreAward (same popup/score pipeline as stomps); new tests/Gate0ContractTests.cpp testFlagpoleScoreByGrabHeight. (6) D7 dynamic_cast audit: all 14 candidate sites in CollisionManager.cpp/Level.cpp KEPT as dynamic_cast; the O(1) discriminator virtuals (getType/getSubtype/getCapabilities) are spoofable by any Entity subclass and tests/CollisionMatrixTests.cpp (IdentitySpoof) pins the fail-closed contract; the enum checks already perform the type test per AGENTS.md rule 5, the dynamic_cast validates the concrete base. Pre-existing failure unrelated to this change: runtime_package_inventory_tests flags stale files (levels/level0.txt, level_athletic.txt, old asset copies) left in the build-tests package dir by in-flight asset/manifest reorganization. |
| 2026-08-22 | [TV5] Merge `feature/sound-input` into `develop` | CTest 37/37 PASS | Fast-forward merged `origin/feature/sound-input` (including commit `b5d7726`). Includes complete OOP principles & design pattern documentation (`docs/oop_principles_and_design_patterns.md`, updated `docs/class_diagram.md` and `docs/design_patterns.md`), Factory pattern enhancements, audio & collision systems, and 100% passing test suite (37/37 CTest). |
| 2026-08-21 | [TV4/TV1] Fix Paratroopa Stomp Effect & Deploy Hammer Bros to Levels | CTest 31/31 PASS | Removed unintended `FireballExplosion` spawn from `Paratroopa::clipWings()` in `src/entities/Paratroopa.cpp`, restoring authentic NES behavior where stomping a winged Koopa removes its wings and converts it to a walking Koopa with standard stomp sound (`stompswim.wav`) and zero fireball pop; deployed `HammerBro` (`'n'`) with full theme palette support to canonical locations: World 1-4 (pre-Bowser stone platform at row 31, col 98), World 1-1 (upper brick platform at row 35, col 140), and World 1-2 (upper underground catwalk at row 36, col 139). |
| 2026-08-21 | [TV4/TV1] Tileset Transparency Cleanup & Comprehensive Chroma-Key Expansion | CTest 31/31 PASS | Converted 4,555 solid blue and lavender backdrop pixels in `assets/textures/tiles/tileset.png` (around underwater coral reef 'S' tiles, horizontal pipe, and scenery) to transparent `RGBA(0, 0, 0, 0)`; expanded `TileMap.cpp` and `TextureManager.cpp` chroma-key masking palette to 9 standard NES background shades (`32, 56, 236`, `36, 100, 252`, `0, 0, 168`, `12, 69, 176`, `92, 148, 252`, `108, 106, 255`, `146, 144, 255`, `148, 148, 255`, `0, 41, 140`), eliminating all solid blue bounding box artifacts behind coral and scenery across World 1-3. |
| 2026-08-21 | [TV4/TV1] Fix Sub-Area Grid Boundaries across Levels 1, 2, and 3 | CTest 31/31 PASS | Corrected sub-area column boundary ranges in `Level::getThemeForGridPosition`: fixed Level 2 range to `49..281` (eliminating the orange/brown Overworld left boundary brick wall and ground block at col 49 upon underground spawn, and ensuring full underground bonus room coverage up to col 281); fixed Level 1 underground bonus room range to `249..265`; fixed Level 3 underwater reef range to `49..242` (ensuring left stone boundary wall at col 49 renders in Underwater palette). |
| 2026-08-21 | [TV4/TV1] Fix Level 3 BG Visibility & Level 2 Per-Column Pipe Theme Resolution | CTest 31/31 PASS | Added `m_columnThemeResolver` to `TileMap` to dynamically resolve per-column themes across multi-area stages (`resolveColumnTheme`), ensuring Level 2 underground pipes and blocks render in authentic cyan/teal Underground palette while intro/flagpole pipes remain Overworld green; fixed `TileMap::buildWaterVertices()` to skip full-screen opaque `WATER_BODY_UNDERWATER` quads in underwater sections, fully unblocking `BackgroundRenderer` (`bg_underwater.png`) across the entire stage. |
| 2026-08-21 | [TV4/TV1] Themed Pixel-Art Backgrounds & Parallax Renderer across Levels 1–4 | CTest 31/31 PASS | Replaced static/solid-color backgrounds across all 4 environments (Overworld, Underground, Underwater, Castle) with procedural 16-bit retro pixel-art backgrounds (`bg_overworld.png`, `bg_underground.png`, `bg_underwater.png`, `bg_castle.png`); created `BackgroundRenderer` component with 0.35x horizontal parallax factor, seamless viewport-wrapping strip math, vertical camera tracking, dynamic area theme switching during pipe warps (`Level::applyAreaTheme`), and theme-specific ambient particle systems (rising bubbles, castle embers, crystal dust, pollen motes); updated CMake runtime assets and `ASSETS_LIST.md`. |
| 2026-08-21 | [TV2] Merge `feature/engine-core-systems` (S7 Visual Sign-Off & Theme/Camera Audit) | CTest 31/31 PASS | Fast-forward merged the 2 newest commits (`d3bb3a4` and `94c42da`) from `origin/feature/engine-core-systems` authored by TV2 (TheMeoXuExp). Added `docs/management/S7_TV2_VISUAL_SIGN_OFF.md` containing TV2's visual sign-off matrix for Underwater and Castle themes, camera bounds, Z-order, and UI resolution tests. Confirmed `docs/management/s7_plan.md` is 100% intact. |
| 2026-08-20 | PvP Top Ceiling Boundary Clamp & Complete HUD Realignment | CTest 31/31 PASS | Implemented top screen ceiling boundary clamp (`m_ceilingClampEnabled`, `Level::clampPvpFighters`) in PvP mode so fighters never disappear off the top frame when jumping from platforms; redesigned duel HUD into balanced 3-column screen-fixed layout (`P1` top-left, `P2` top-right, `Match Score/Round` top-center, dedicated `FIRE` badges) with clear padding from arena walls and center pedestal; added regression tests in `PvpArenaTests`. |
| 2026-08-20 | PvP Camera Elevation & Enemy Narrow Patrol Breakout AI | CTest 31/31 PASS | Enabled `CameraVerticalMode::DEAD_ZONE` in `PvpPlayState` with dual-player elevation tracking in `Level::updatePvp`; implemented narrow patrol (<= 2-2.5 tile) breakout AI in `Enemy.h/.cpp` (Goomba, Koopa, Spiny, HammerBro) to break out towards player upon rapid turnaround oscillation; added test coverage in `PvpArenaTests` and `KoopaVariantTests`. |
| 2026-08-20 | Area-Accurate Multi-Theme Resolution for Enemies, Blocks & Entities | CTest 31/31 PASS | Added `Level::getThemeForGridPosition(gridX)` to dynamically resolve area themes (Level 1 Underground Bonus, Level 2 Underground Stage, Level 3 Underwater Reef) during initial spawn; updated `EntityFactory::createFromTileCode`, `spawnElevatorsFromTileMap`, `spawnCheepCheepRoutesFromTileMap`, `spawnCheepCheepsFromConfig`, and `updateCheepCheepGenerators`; added `Entity::getSprite()` getter; added regression unit tests in `Gate0ContractTests`. |
| 2026-08-20 | Fix Level 1 Sound Glitch, Level 2 Intro Theme, Finish Pole Texture & Clean Preview Images | CTest 31/31 PASS | Removed duplicate playMusic() in PlayState::onEnter(); changed Level 2 catalog initial theme to OVERWORLD matching overworld intro spawn; fixed TileMap underground finish pole texture rect; reformatted Koopa::kick(); deleted preview pngs; updated unit test assertions. |
| 2026-08-20 | Fix Enemy Ledge Probes, Level 2 Warps/Palettes & Level 3 Theme | CTest 31/31 PASS | Fixed enemy ledge detection on question blocks using isEnemySupport; fixed Level 2 enemy symbols (k->K, q->p) and warp 4 theme transition; fixed Level 3 initial theme in LevelCatalog to OVERWORLD; stripped UTF-8 BOM. |
| 2026-08-20 | Fix Box2D Physics Sync Order for Enemies & Items | CTest 31/31 PASS | Moved syncPhysics() to the beginning of update(dt) to prevent mid-step velocity overwrites and fix physics solver output conflicts. |
| 2026-08-08 | Initial Sprint 6 integration | 7/7 CTest | Superseded by later implementation |
| 2026-08-09 | Evaluation v3 integration | 12/12 CTest | Superseded by later implementation |
| 2026-08-11 | `origin/develop` at `3047252` | 14/14 CTest | Review v4 reopened 40 tasks |
| 2026-08-12 | Sprint 6 remediation based on `3047252` | Clean Debug/Release/Tests PASS; CTest 17/17 | no-op asset build PASS; final commit/manual evidence pending |
| 2026-08-12 | BUG-042 body-tier FireFlower/transform fix | Debug/Release PASS; CTest 17/17 | Small/Super Fire forms retain body size; transformation frames update continuously |
| 2026-08-13 | Merge `feature/sound-input` into `develop` | Fast-forward merge `b9622fb` | Updated local `feature/sound-input` from `origin` and merged into `develop` |
| 2026-08-13 | Underground Castle sprite support for Level 2 | CTest 17/17 PASS | Added `TileFrames::CASTLE_UNDERGROUND` (419,196,80,80) for `LevelTheme::UNDERGROUND` in Level 2 |
| 2026-08-13 | Remove all blue background shades from tileset | Image processed | Converted all 112,463 background pixels matching blue shades `(r<=5, 35<=g<=45, 135<=b<=145)` in `assets/textures/tiles/tileset.png` to transparent (0, 0, 0, 0) |
| 2026-08-13 | Update HARD_BLOCK_UNDERGROUND texture rect | CTest 17/17 PASS | Changed `HARD_BLOCK_UNDERGROUND` sprite rect to `(147, 33)` {16, 16} in `TileFrames.h` and test suite |
| 2026-08-13 | Fix IDE warnings & directly reference facade symbol | Clean compile | Added `SpriteFrames::IS_FACADE_ACTIVE`, annotated headers with `IWYU pragma: export` and suppressed unused diagnostics inside `SpriteFrames.h` so IDE linter recognizes it cleanly |
| 2026-08-13 | Elevator (moving platform) feature | CTest 18/18 PASS | New `Elevator` kinematic entity riding on `^`/`~` tile markers (vertical/horizontal, end-pause) + external `levels/elevators.txt` routes + theme-specific platform sprite frames + `ElevatorTests`. Map markers must use empty cells and should not duplicate a config route; the active level theme selects the platform palette. |
| 2026-08-14 | Cheep Cheep enemy & external spawn feature | CTest 19/19 PASS | Added `CheepCheep` enemy class with swimming and jumping mechanics adhering to SMB1 NES canon; added external spawn registry `levels/cheep_cheep.txt` & camera leap generator without modifying any existing map files (`level*.txt`); added `CheepCheepTests` with 100% pass rate. |
| 2026-08-14 | Merge `feature/sound-input` (Mario/Luigi Character Select) | CTest 20/20 PASS | Integrated character selection flow (`CharacterSelectState`), Mario vs. Luigi physics profiles, Luigi Grow/Shrink/Fire animation atlases, session persistence in `GameProgress`, GameOver retry retention, and `character_flow_tests`. |
| 2026-08-14 | Fix Mario & Luigi Animation & Invincibility Logic | CTest 20/20 PASS | Added transform presentation sequence for same-body-tier Fire transitions (`SMALL<->FIRE_SMALL`, `SUPER<->FIRE_SUPER`), preserved fireball shoot pose (`action`) during movement, resolved Starman rainbow timer underflow bug, and prevented transform completion from overriding active damage grace blinking. |
| 2026-08-14 | Fix Luigi Fire Transformation & Shared Fire Form Sprites | CTest 20/20 PASS | Unified FIRE states (`FIRE_SMALL`, `FIRE_SUPER`) to use canonical Fire Mario sprite sheets for both Mario & Luigi; corrected Luigi FireFlower upgrade animation to use `FireMario::growSequence()` (fire flash palette) instead of mistakenly playing `Luigi::growSequence()` (small-to-big mushroom growth). |
| 2026-08-14 | Pipe Warp Smooth Entry Animation & Transition Delay | CTest 20/20 PASS | Added smooth pipe descent/entry animation (0.5s slide at 48 px/s behind foreground tiles), authentic pipe travel audio cue (`powerdown`), and deliberate 0.35s warp transition delay before Mario emerges at destination. |
| 2026-08-14 | Fix Fireball Shooting Poses & Natural Pipe Crouch/Walk Animations | CTest 20/20 PASS | Replaced fireball projectile frames with authentic NES character throw poses (`FireBigMario::ACTION`, `FireSmallMario::WALK3`); added dynamic `crouch` state for Super/Fire forms; enabled live animation advancing (`updateVisuals`) during pipe entry and smooth horizontal alignment during pipe descent. |
| 2026-08-14 | Pipe Exit / Emerging Animation & Complete Luigi Warp Parity | CTest 20/20 PASS | Added smooth pipe emergence animation (`EXITING_VERTICAL`, rising UP out of destination pipe with `powerdown` audio cue); verified 100% full parity for Luigi (including Luigi crouch/walk/idle and sprite frames during pipe entrance/exit). |
| 2026-08-15 | Sprint 7 TV1 4-Level Release Contract (`S7-TV1-01`..`04`) | CTest 20/20 PASS (historical snapshot; not current RC evidence) | Formalized 4-level release graph: `1-1 Overworld` -> `1-2 Underground` -> `1-3 Underwater` -> `1-4 Castle` -> `Win`. Updated `LevelCatalog.h`, `level4.txt`, `LevelCatalogTests.cpp`, `PlayStateTests.cpp`, `Gate0ContractTests.cpp`, and `TV5IntegrationTests.cpp`. |
| 2026-08-15 | Stage Select UI & Flow Integration (`LevelSelectState`) | CTest 20/20 PASS (historical snapshot; not current RC evidence) | Created `LevelSelectState` presenting 4 themed cards (Overworld, Underground, Underwater, Castle) with keyboard and mouse interaction. Wired Title Menu `START GAME` -> `LevelSelectState` -> `CharacterSelectState(selectedLevel)` -> `PlayState(selectedLevel, characterType)`. |
| 2026-08-15 | Code Review Remediation for `cc80466` (P0/P1/P2 fixes) | CTest 20/20 PASS (historical snapshot; not current RC evidence) | Eliminated hot-loop `dynamic_cast` via virtual `isCoin()`; decomposed `Mario::update()` (~40 lines) and `Level::update()` (~45 lines) into focused helpers; enforced Box2D physics sync at line 1 of update loop; decomposed `onEnter()` in `LevelSelectState` & `CharacterSelectState`; cleaned `level4.txt` trailing lines; added automated validation for all 4 release levels in `LevelValidatorTests.cpp`. |
| 2026-08-16 | Fix Fire State Growth/Shrink Transformation Animation | CTest 21/21 PASS | Fixed `Mario::applyStateTransition` to prioritize `usesFireTransition` over `m_characterType == LUIGI` so Fire Small -> Fire Super uses Fire transformation frames instead of normal green Luigi frames; added regression tests. |
| 2026-08-16 | Multi-kill Combo Scoring & Defeat Score Popup Fixes | CTest 21/21 PASS | Fixed missing score popups for FireBall, sliding Koopa shell, Star, and block bump kills; implemented sliding shell combo kill streak (200..8000, 1UP); verified airborne stomp multi-kill handling. |
| 2026-08-16 | Item/Enemy Theme Support, Pixel Sharpness & Title Screen Redesign | CTest 21/21 PASS | Added `LevelTheme` to `Coin` and `Mushroom`; fixed `CheepCheep` red swim frames across all themes; redesigned `levels/level3.txt` (1-3 Underwater); disabled font smoothing for crystal-clear UI; redesigned `MenuState` Title Screen with full scenery, HUD, and live character animation. |
| 2026-08-16 | Fix Score Popup Texture Rect Mapping in Shared Atlas | CTest 21/21 PASS | Corrected completely scrambled score popup texture rect coordinates in `SpriteFrames_shared.h` (SCORE_200..8000, 1UP) so displayed visual score popups precisely match the points awarded to Mario; added `SCORE_500` support. |
| 2026-08-16 | Multi-Theme Score Popups & Title Screen Layout Fine-tuning | CTest 21/21 PASS | Implemented theme-aware score popups across all 4 environments (Overworld, Underground, Castle, Underwater); removed clunky bounding boxes and overlapping blocks from `MenuState` Title Screen for authentic NES presentation; verified with render snapshot tests. |
| 2026-08-16 | Title Cover Screen Branding & Clutter Cleanup | CTest 21/21 PASS | Removed gameplay HUD banner (`MARIO 000000 | x00 WORLD 1-1 TIME 400`) and arbitrary high-score line from Title cover screen; added project credit `CS202 - OOP FINAL PROJECT` and team name `GROUP 04`; polished vertical layout and spacing. |
| 2026-08-16 | Title Screen Pipe Symmetry & Title Punctuation Cleanup | CTest 21/21 PASS | Symmetrically balanced left pipe (`x=32..96`) and right pipe (`x=544..608`) with exact 32px screen edge margins; centered Goomba (`x=320`) and balanced Mario (`x=200`) and Luigi (`x=440`); removed trailing dot from `SUPER MARIO BROS`. |
| 2026-08-16 | Authentic NES Underwater Cheep Cheep Sprite Configuration | CTest 21/21 PASS | Configured `SpriteFrames_udw.h` and `Level.cpp` to use the authentic NES 1985 grey/white underwater palette (`UW_SWIM_UP {292, 164}`) for Cheep Cheeps swimming in World 1-3. |
| 2026-08-17 | Comprehensive Game UI Redesign & Screenshot Layout Overhaul | CTest 31/31 PASS | Complete redesign of all game state screens (`MenuState`, `LevelSelectState`, `CharacterSelectState`, `CoopCharacterSelectState`, `PvpCharacterSelectState`, `PauseState`, `GameOverState`, `WinState`, `HUD`) with screenshot-verified pixel perfection, crisp outlines, side-by-side hero showcase cards, modal dialogue frames, and balanced HUD. |
| 2026-08-18 | Sprint 7 TV3 Physics & 4-Level Manual Playthrough Audit | 31/31 MSVC, 30/31 macOS PASS | Verified underwater buoyancy/swimming cadence, Box2D contact filtering, Castle hazards (Firebar/Podoboo/Bowser), and 4-level campaign walkthrough (1-1..1-4); documented TV3 status matrix and residual P2 notes in `docs/management/S7_TV3_STATUS.md`. |
| 2026-08-18 | Repository Cleanup: Ignore .zcode and Remove Preview Images | CTest 31/31 PASS | Untracked and removed temporary UI preview images (`level_select_preview.png`, `menu_state_preview.png`), untracked `.zcode/` directory, and added `.zcode/` and `*_preview.png` patterns to `.gitignore`. |
| 2026-08-19 | Dual Mode Screen Clamp, Ceiling Boundary & Pit Hazards | CTest 31/31 PASS | Enforced camera viewport containment for both players in 2P Co-op; implemented underwater/top ceiling clamp in Mario physics; added test coverage in MarioPhysicsTests and CoopFlowTests. |
| 2026-08-19 | World 1-4 Castle Level Refinements & Alternative Bowser Passage | CTest 31/31 PASS | Raised Castle pipe in Section 1; filled pit under Section 2 question blocks; added Springboard and aerial horizontal Elevator above Bowser in Section 4 with cleared ceiling; verified with render snapshots. |
| 2026-08-19 | Koopa Stomp Latch Re-arm & Firebar Damage Normalization | CTest 31/31 PASS | Re-armed stomp latch in Koopa::kick() and wakeUpFromShell() to allow stopping sliding shells and restomping waking Koopas; normalized Firebar damage via queuePowerDown() instead of loseLife(); added regression unit tests in KoopaVariantTests. |
| 2026-08-19 | Fix Co-op Boundary Character Flicker & Level 3 Left Swim Breach | CTest 31/31 PASS | Configured Camera horizontal deadzone to 0 in co-op; overrode Mario::syncPhysics() to synchronize sprite offsets (updateSpriteLayout) after body clamping; placed solid barrier at column 49 in level3.txt; added regression unit tests in CoopFlowTests and LevelValidatorTests. |
| 2026-08-19 | PvP Arena Platform Headroom & Layout Lowering | CTest 31/31 PASS | Lowered center pedestal platform (`SSSS`) and Fire Flower pedestal (`W`) to rows 5 & 4, side step blocks to row 7, and expanded arena height to 12 rows (384px) in `levels/pvp_arena.txt` for flush floor alignment eliminating bottom blue clear gap; updated regression assertions in `PvpArenaTests.cpp`. |

## 2026-08-12 remediation scope

- Restored the locked three-level graph and Level 3 Castle -> Win boundary.
- Restored deterministic adaptive QuestionBlocks and explicit required item routes.
- Preserved Small Fire versus Super Fire body forms and fixed the transition renderer to advance atlas frames while movement is frozen.
- Unified Mario growth/fixture transitions, independent timers, death completion, and respawn reset.
- Consolidated FireBall requests/storage/events and added world-lock coverage.
- Completed collision order/idempotence and one-event/one-SFX coverage.
- Added metadata-driven camera Y policy, full X/Y clamp, integer letterboxing, and physical-to-logical input mapping.
- Synchronized shared UI behavior, final/high-score screens, theme backgrounds, tests, and release documentation.

## Evidence policy

The active status is maintained in:

- [Sprint 6 audit tracker](management/S6_AUDIT_TRACKER.md)
- [Sprint 6 bug register](management/S6_BUG_REGISTER.md)
- [TV1 integration report](management/TV1_CHANGES_SUMMARY.md)
- [TV4 release playthrough log](testing/TV4_PLAYTHROUGH_LOG.md)
- [TV5 audio/HUD/item checklist](management/TV5_AUDIO_HUD_ITEM_CHECKLIST.md)

A working-tree result may support review, but release sign-off requires one immutable commit hash shared by clean builds and every manual evidence record.

## 3. MODIFIED FILES LOG (Chi tiết các file đã sửa)

### 2026-08-25: Merge `feature/sound-input` (Demo Video Script & Controls Reference)
- **`docs/DEMO_VIDEO_SCRIPT.md`**:
  - Refined demo video recording timeline, chapter timings, and rubric alignment for all 4 worlds (Overworld, Underground, Underwater, Castle & Bowser), 2-Player Co-op, and 2-Player Versus modes.
  - Added dedicated controls cheat sheets for Single-player, Co-op, and Versus modes (covering standard arrow/WASD, Numpad, and modifier keys).
  - Outlined standalone supplementary recording takes (5-fireball Bowser defeat vs. Axe collapse, shockwave stun counterplay, GameOver retry flow, power-up tier transformations, flagpole height tiers).
  - Added post-recording validation checklist and production export guidelines (`CS202_Group04_Mario_Remake_Demo.mp4`).

### 2026-08-24: Documentation Remediation -- Em-dash Mojibake Fix & Anchor Link Synchronization
- **`docs/design_patterns.md`**:
  - Replaced corrupted ANSI em-dash character sequences (`—`) with clean UTF-8 em-dash (`—`) across H2 section headers and prose paragraphs (sections 1–6).
- **`docs/oop_principles_and_design_patterns.md`**:
  - Fixed corrupted em-dash sequences across introductory text, SOLID heuristic table rows (S, O, L, I, D), pattern headers, Command limits, State capabilities, and developer extension recipes.
- **`docs/class_diagram.md`**:
  - Replaced corrupted em-dash in section notes.
  - Re-synced the `Source evidence` table anchor link for `PlayState::onEnter` from `#L224` to `#L281` matching the updated line number.

### 2026-08-23: Review Remediation -- Hammer Residue Cleanup, Dead Branch Removal, Const-Correctness, Docs Resync
- **`include/entities/Bowser.h` & `src/entities/Bowser.cpp`**:
  - Removed the write-only `m_hammerVariant` field and its constructor default argument; every call site already used the 3-argument form.
  - Dropped the orphaned `THROW_ANIMATION` registration and `throwFrames()` helper that no code played after the hammer-variant attack was removed.
- **`src/physics/CollisionManager.cpp`**:
  - Simplified the Mario lateral-collision chain: the always-true `else if (Enemy* enemy = otherParticipant.enemy())` guard became a plain `else`, and the unreachable duplicate power-down branch at the end of the chain was deleted. Bullet Bill nose-hit / rear-brush policy untouched.
- **`src/level/Level.cpp`**:
  - `nearestEligiblePlayer()` now hands back the non-owning `Mario*` target directly, removing the `const_cast` previously required to feed `Bowser::updateMarioTarget()`.
- **`docs/tileset_coordinate.md`**:
  - Added a "Themed Cannon Cells" table documenting the per-theme Bullet Bill Cannon atlas triplets used by `BulletBillLauncher::getCannonParts()` (Overworld/Castle share column x=119; Underground/Underwater share column x=266) and marked legacy rows #74-#76 as the Overworld variant.
- **`docs/DEMO_VIDEO_SCRIPT.md`**:
  - Rewrote the World 1-4 chapter for the current boss fight: rotating Turtle Cannons firing angled Bullet Bills (front-nose hit = instant death), enraged twin fire waves, ground-stomp shockwave rings with dizzy stun stars, and the Toad ending locked until Bowser is defeated; removed outdated hammer-throw mentions and added a Part B shockwave close-up shot.
- **`docs/design_patterns.md`, `docs/oop_principles_and_design_patterns.md`, `docs/class_diagram.md`**:
  - Re-synced every drifted `file:line` evidence anchor to the post-89852e9/f6f5a89 sources (e.g. `requestFireBallShot(Mario&)` 1708->2000, `spawnEntitiesFromTileMap()` 625->685, `applyStateTransition()` 937->963, Level ownership members Level.h 192-201 -> 210-218, `PlayState::onEnter` 224 -> 281).
  - Documented the Bullet Bill direct-damage exception in the collision->events flow.

### 2026-08-23: Advanced Enemy AI (Aggro Pursuit, Agile Hops, Predictive Drops, Boss Trajectories) & Co-op Boost Jump
- **`include/entities/Enemy.h` & `src/entities/Enemy.cpp`**:
  - Added shared proximity & relative positioning queries (`isPlayerNearby(distX, distY)`, `isPlayerInFront()`, `isPlayerBelow()`).
  - Added safe ledge drop probing `isSafeDropAhead(tileMap, maxDropTiles)` to check for solid support below platforms.
  - Added aggro status tracking (`m_aggroActive`, `getAggroSpeedMultiplier() = 1.30f`) and surprise hop timer management (`canAggroHop()`, `resetHopCooldown()`, `updateHopCooldown(dt)`).
- **`src/entities/Goomba.cpp` & `src/entities/Spiny.cpp`**:
  - Activated aggressive pursuit mode: moves +30% faster when player is nearby in line-of-sight.
  - Added agile surprise hop (`velocity.y = -170.f`, 3.0s cooldown) when closing in on the player (distance 24px–90px).
- **`src/entities/HammerBro.cpp`**:
  - Enhanced tier jump targeting: executes higher leaps when player is positioned on upper brick platforms.
  - Accelerated defensive projectile barrage: hammer throw cadence speeds up to 0.75s–1.5s when player closes within 160px.
- **`include/entities/Lakitu.h` & `src/entities/Lakitu.cpp`**:
  - Added player velocity tracking (`m_marioVelX`) in `updateMarioPosition` to lead SpinyEgg tosses ahead of player trajectory.
- **`src/entities/Blooper.cpp`**:
  - Enhanced swimming state machine: pulses diagonally towards Mario during RISE state and tracks faster horizontally during DRIFT state when player is nearby.
- **`src/entities/Bowser.cpp`**:
  - Added adaptive multi-height fire breath aiming: checks Mario's elevation (biases towards HIGH/MID fireballs when Mario is airborne, and LOW floor-skimmers when Mario is grounded).
  - Added airborne interception hop: leaps upward if Mario attempts to vault high over Bowser's head.
- **`include/level/Level.h` & `src/level/Level.cpp`**:
  - Implemented `Level::processCoopPlayerHits()` in `Level::updateCoop(float dt)`: processes pending stomps between co-op partners, propelling the top player with a full jump boost (`attacker->getJumpForce()`) and small jump sound, keeping the partner completely safe with 0 damage.
- **`tests/CoopFlowTests.cpp`**:
  - Added `testCoopHeadBounceBoostJump()` verifying that when Player 1 stomps Player 2 in co-op mode, Player 1 gains upward bounce velocity while Player 2 remains undamaged and alive with all lives intact.

### 2026-08-23: Camera Feel Pass -- Edge-Margin Vertical Follow, Airborne Hold, Idle Settle, Co-op Fit-Both Bias and Invisible Ceiling
- **`include/level/Camera.h`**:
  - Replaced the TRACK vertical dead-zone API with `set/getVerticalEdgeMarginRatio()` (default 0.15): panning is measured against the CURRENT view edges instead of a band around the stable center.
  - Added `setVerticalSettleEnabled()` and `setVerticalFrozen()` plus private state (`m_verticalFrozen`, `m_settleTimer`, `m_previousTargetY`, `m_hasPreviousTargetY`).
- **`src/level/Camera.cpp`**:
  - New tuning constants: `SETTLE_IDLE_DELAY = 0.35s`, `SETTLE_RATE = 4.0/s`, `SETTLE_REST_Y_BIAS = −40px`.
  - TRACK branch rewritten to edge-margin minimal-shift: pan only when the target nears the current top/bottom edge; shift exactly enough to restore the margin. Sustained elevations inside the frame never drag the view up.
  - Airborne hold: while `m_verticalFrozen` is set the branch applies no vertical motion at all (and resets the settle timer), so jump arcs, springboard launches and fast falls never scroll the screen.
  - Idle settle glide: when enabled and the target holds still past `SETTLE_IDLE_DELAY`, the center eases toward `target.y + SETTLE_REST_Y_BIAS` (actor rests slightly below center → ground under his feet stays framed). Any vertical movement resets the timer instantly.
- **`src/level/Level.cpp`**:
  - Renamed/retuned constants: `CAMPAIGN_VERTICAL_EDGE_MARGIN` (0.15), `COOP_VERTICAL_EDGE_MARGIN` (0.15), alongside `CAMPAIGN_HORIZONTAL_DEADZONE` (0.10) and `COOP_HORIZONTAL_DEADZONE` (0.0); `COOP_VIEW_EDGE_MARGIN` (16px) and `COOP_CEILING_WARN_RATIO` (0.75) unchanged.
  - Campaign camera block sets `setVerticalFrozen(!flagSequenceActive && !grounded && !climbing)` every frame: only vine climbing and the flagpole sequence keep following while airborne. Campaign load enables `setVerticalSettleEnabled(true)`; co-op load explicitly disables it after the shared base load so the fit-both contract owns co-op framing (a settle drift would fight the invisible-ceiling guard).
  - Base load clears the latched airborne-hold flag after its pre-render `update(0.f)` (the freshly spawned Mario is not grounded yet), so co-op / PvP loads start with a clean slate; the campaign path re-establishes the flag every frame anyway.
  - Co-op overload applies the same edge-margin ratio on Y; horizontal remains instant for the edge-stuck pull-along rule.
  - New `Level::computeCoopCameraTarget()`: horizontal midpoint; vertical midpoint clamped into the camera-center range that keeps BOTH player centers framed with `COOP_VIEW_EDGE_MARGIN` on each side; beyond one viewport of separation the LOWER partner wins the frame.
  - `clampCoopPlayersToCamera()`: added the invisible-ceiling guard — vertical gap beyond `viewHeight − 2 × COOP_VIEW_EDGE_MARGIN` cancels the higher climber's upward velocity (Box2D negative-y = ascending). No teleporting; falling never interrupted.
  - New `renderCoopSeparationWarning()` drawn from `render()` after foreground tiles: white world-space arrow above the higher partner once the gap reaches 75% of the limit; skipped during pipe warps / flagpole sequence or inactive partners.
- **`tests/CoopFlowTests.cpp`**:
  - Rewrote `testCoopCameraTracksVerticalMidpointAndSticksAtEdges` on a generated tall flat fixture (70×30, floor rows 12–13). Rationale: shipped `levels/level0.txt` trims to ~12 rows (~384px), leaving ~24px of clamp travel — geometrically incompatible with any non-zero vertical margin. Assertions: upward pan magnitude from the bottom rest, no-scroll when the reunited pair sits mid-frame, both-partner visibility bounds, and unchanged horizontal edge-stuck pin + exact midpoint tracking.
  - Added `testCoopInvisibleCeilingCapsVerticalSeparation`: forces P1 upward past one viewport while P2 stands grounded; asserts the worst gap stays within limit (+ one-frame overshoot) and P2 never exits through the view bottom.
- **`tests/DisplayCameraUITests.cpp`**:
  - Added `testCampaignAirborneVerticalHold`: launches Mario with jump-strength velocity on a flat fixture and asserts the view center is pixel-frozen for the whole arc while he is airborne.

### 2026-08-23: Restore Smart Enemy Ledge Detection AI & 4-Way Camera / Co-op Dual Transitions
- **`include/entities/Enemy.h`**:
  - Restored `virtual bool turnsAtLedge() const { return true; }` as the default enemy behavior.
- **`src/entities/Goomba.cpp`**, **`src/entities/Koopa.cpp`**, **`src/entities/Paratroopa.cpp`**, **`src/entities/Spiny.cpp`**:
  - Restored `isApproachingLedge()` checks inside `patrol()`, so all ground-walking enemies continuously detect platform edges and reverse direction instead of walking blindly off cliffs/platforms.
  - Preserved Breakout AI (`isEscapingNarrowRange()`) which intelligently bypasses ledge checks when breaking out of 1-2 tile trapped corridors towards Mario.
- **`tests/KoopaVariantTests.cpp`**:
  - Updated `testEnemySupportTileCoverageAndLedgeProbing` assertions to verify that all enemy species (`Goomba`, `Koopa`, `Spiny`, `Paratroopa`, `BuzzyBeetle`, `RedKoopa`, `HammerBro`) detect ledges and reverse direction.
- **`include/core/LevelCatalog.h`**:
  - Updated `LevelData` entries for Levels 1, 2, 3, and 4 to use `CameraVerticalMode::TRACK` (smooth full 4-way vertical tracking) instead of clamped single-pane height.
- **`include/level/Level.h`**:
  - Added co-op transition state members: `m_flagPlayer2`, `m_flagSlideStartMarioY2`, `m_flagWalkTargetX2`, `m_warpPlayer2`.
  - Added helper declarations: `startPipeWarpCoop(const TileMap::WarpEntry& entry)`, `isPipeWarpActive()`.
- **`src/level/Level.cpp`**:
  - **4-Way Camera Follow**: Removed single-player backward screen clamp in `clampCampaignPlayerToCameraLeft()`, allowing free left/right and up/down movement. Removed vertical deadzones so camera immediately follows player verticality across all levels.
  - **Co-op Dual Pipe Warp**: Implemented `startPipeWarpCoop()`; updated `checkPipeWarps()` to verify both players are positioned on the pipe mouth (±4px margin) and pressing Down/Right before triggering; updated `updatePipeWarp()` and `warpMarioToReturn()` so both characters slide in simultaneously, warp together, and slide up out of the exit pipe.
  - **Co-op Dual Flagpole Completion**: Updated `checkFinishFlag()` so co-op completion requires both players to reach the flagpole; updated `updateFlagSequence()` so both players slide down together, face right, and walk side-by-side into the castle before firing `LEVEL_COMPLETED`.
- **`tests/CoopFlowTests.cpp`**:
  - Added `testCoopDualPipeWarpRequiresBothPlayers()` to verify co-op pipe warp requires both players.
  - Added `testCoopDualFlagpoleCompletion()` to verify co-op flagpole completion requires both players.
  - Updated `testCoopCameraTracksVerticalMidpointAndSticksAtEdges()` to verify edge clamping and midpoint tracking across safe ground coordinates.
- **`tests/LevelCatalogTests.cpp`**:
  - Updated expected vertical camera mode assertion to `CameraVerticalMode::TRACK` for all levels.
---

## 4. DETAILED LOGIC CHANGE LOG (For Opus Review)

### Entry #54: [TV2 Visual & Camera Audit] - Sprint 7 Theme Rendering, Camera Tracking & Multi-Resolution UI Sign-Off
- **Status:** Đã hoàn thành 100%, 31/31 CTest passed.
- **Tác giả:** TV2 (Nguyễn Phúc Minh Nhat - `TheMeoXuExp <npmnhat2523@apcs.fitus.edu.vn>`)
- **Affected Files:** `docs/management/S7_TV2_VISUAL_SIGN_OFF.md`, `docs/change_in_develop.md`.
- **Description:**
  1. **Đồng bộ nhánh TV2**: Tích hợp các commit từ `origin/feature/engine-core-systems` (`d3bb3a4` và `94c42da`) vào `develop`.
  2. **Báo cáo Sign-Off Visual Sprint 7 (`S7-TV2-01..05`)**:
     - Hoàn thiện tài liệu [`docs/management/S7_TV2_VISUAL_SIGN_OFF.md`](docs/management/S7_TV2_VISUAL_SIGN_OFF.md) ghi nhận đầy đủ bằng chứng kiểm thử visual cho Underwater (Level 3) và Castle (Level 4).
     - Kiểm chứng giới hạn camera (Camera Bounds) ở điểm đầu, giữa và cột cờ đích không lộ vùng ngoài map.
     - Xác nhận thứ tự hiển thị Z-order (HUD > Mario > Foreground > Background).
     - Kiểm tra ma trận phân giải UI (800x600, 1080p) trên các màn hình Menu, Pause, Game Over, Win State.
  3. **Bao toàn Kế hoạch Sprint 7**: Toàn bộ nội dung [`docs/management/s7_plan.md`](docs/management/s7_plan.md) được giữ nguyên vẹn 100%.

### Entry #53: [Feature & AI] - PvP Arena Vertical Camera Elevation & Enemy Narrow Patrol Breakout AI
- **Status:** Đã hoàn thành 100%, 31/31 CTest passed.
- **Affected Files:** `include/entities/Enemy.h`, `src/entities/Enemy.cpp`, `src/entities/Goomba.cpp`, `src/entities/Koopa.cpp`, `src/entities/Spiny.cpp`, `src/entities/HammerBro.cpp`, `include/level/Level.h`, `src/level/Level.cpp`, `src/states/PvpPlayState.cpp`, `tests/PvpArenaTests.cpp`, `tests/KoopaVariantTests.cpp`, `docs/change_in_develop.md`.
- **Description:**
  1. **Nâng Camera Chế độ PvP (`CameraVerticalMode::DEAD_ZONE`)**:
     - Thiết lập `m_level->setCameraVerticalMode(CameraVerticalMode::DEAD_ZONE)` trong `PvpPlayState::onEnter()`.
     - Trong `Level::updatePvp()`, tính toán trung điểm ngang của cả 2 đấu thủ `(center1.x + center2.x) / 2.0f` và vị trí Y cao nhất `std::min(center1.y, center2.y)`.
     - Khi một trong 2 người chơi nhảy cao, nảy trên springboard hoặc leo lên bục trung tâm, camera tự động nâng lên theo trục Y mượt mà theo đúng deadzone chuẩn.
     - Bổ sung unit test `testPvpCameraVerticalElevation` trong `tests/PvpArenaTests.cpp`.
  2. **AI Quái Thoát Vùng Hẹp (1-2 ô) Về Hướng Nhân Vật (Narrow Patrol Breakout AI)**:
     - Thêm cơ chế nhận diện dao động phạm vi hẹp trong `Enemy.h` và `Enemy.cpp`: theo dõi khoảng cách giữa 2 lần quay đầu (`notifyTurnaround()`, `updateNarrowEscapeStatus()`, `updatePlayerPosition()`).
     - Khi quái đi bộ (Goomba, Koopa, Spiny, HammerBro) liên tục đảo chiều trong phạm vi hẹp ($\le 80\text{px}$, tương đương 1 đến 2.5 ô tile), quái kích hoạt chế độ thoát hiểm `m_isEscapingNarrowRange = true`.
     - Quái tự động quay mặt về hướng của người chơi gần nhất (`escapeDir = (playerPos.x < pos.x) ? LEFT : RIGHT`).
     - Bỏ qua kiểm tra mép rơi `isApproachingLedge()` theo hướng thoát hiểm để bước ra khỏi bục hẹp về phía người chơi.
     - Khi quái đã rơi xuống hoặc vượt qua khỏi phạm vi bục hẹp (`escapedY` hoặc `escapedX`), chế độ thoát hiểm tự động hủy và quái trở lại logic tuần tra / rơi tự do thông thường.
     - Bổ sung broadcast tọa độ nhân vật gần nhất trong `Level::updateEntities()`.
     - Bổ sung unit test toàn diện `testNarrowPatrolBreakoutAI` trong `tests/KoopaVariantTests.cpp`.

### Entry #48: [Bugfix & Physics] - Fix Box2D Physics Sync Order for Enemies and Items
- **Status:** Completed 100%.
- **Affected Files:** src/entities/Goomba.cpp, src/entities/Koopa.cpp, src/entities/Blooper.cpp, src/entities/HammerBro.cpp, src/entities/Lakitu.cpp, src/entities/Spiny.cpp, src/entities/BulletBill.cpp, src/items/Mushroom.cpp, src/items/Star.cpp, include/items/Star.h, src/level/Level.cpp, 	ests/PvpArenaTests.cpp, levels/pvp_arena.txt.
- **Description:**
  1. **Box2D Velocity Synchronization (syncPhysics())**: Moved syncPhysics() call to the very first line of update(dt) for all enemies and items, preventing manual velocity assignments from overwriting solver gravity and bounce impulses from previous frames.
  2. Completely resolved floating, ground-tunneling, and block-stuck physics anomalies for entities and dropped items.

### Entry #47: [Feature & Visual Polish] - Pipe Exit / Emerging Animation & Comprehensive Luigi Sync
- **Status:** Completed 100%, 20/20 CTest passed.
- **Affected Files:** include/level/Level.h, src/level/Level.cpp, docs/change_in_develop.md.
- **Description:**
  1. **Pipe Emerging Animation Phase (PipeWarpPhase::EXITING_VERTICAL)**:
     - When destination (R) is positioned on top of a vertical pipe opening (tile below is [ / ] / p /  / { / }):
       * Player initializes submerged inside the pipe opening (	arget.y + marioSize.y).
       * Triggers pipe exit audio cue (pipepowerdown.wav).
       * Player rises smoothly (ising UP) at \text{ px/s}$ for .45\text{s}$ until feet rest securely on top of the pipe collar.
       * Restores standard physics gravity and returns controls to player upon completion.
     - When destination is open air or ceiling (e.g. bonus underground vault R1): Player spawns directly and falls under normal gravity.
  2. **Full Animation Sync for Luigi**:
     - Luigi inherits pipe traversal animations:
       * Crouching pose (SpriteFrames::shared::BigLuigi::CROUCH / FireBigMario::CROUCH) when entering vertical pipes in Super/Fire form.
       * Walking animation (SmallLuigi / BigLuigi / FireMario) during horizontal pipe entries.
       * Smooth rising emerge sequence upon exiting pipes.

### Entry #46: [Bugfix & Visual Polish] - Fix Fireball Throwing Pose & Refine Pipe Animations
- **Status:** Completed 100%, 20/20 CTest passed.
- **Affected Files:** include/entities/Mario.h, src/entities/Mario.cpp, include/level/Level.h, src/level/Level.cpp, 	ests/MarioPhysicsTests.cpp, docs/change_in_develop.md.
- **Description:**
  1. **Fix Fireball Throwing Poses (setupAnimationsForState)**:
     - Replaced incorrect explosion/fireball coordinate frames with authentic Mario/Luigi throw poses:
       * FIRE_SUPER: Uses SpriteFrames::shared::FireBigMario::ACTION (Frame 14, {264, 139}, {16, 32}) - canonical NES fireball throw arm swing.
       * FIRE_SMALL: Uses SpriteFrames::shared::FireSmallMario::WALK3 (Frame 3, {56, 116}, {16, 16}) - outstretched hand throw pose.
     - Updated test 	estLuigiFireBodyUsesLuigiRows in 	ests/MarioPhysicsTests.cpp.
  2. **Add Crouch Pose for Mario & Luigi**:
     - Loaded "crouch" animation frames BigMario::CROUCH, BigLuigi::CROUCH, and FireBigMario::CROUCH.
     - Automatically triggers crouch frame when sliding into vertical pipes in Super/Fire form.
  3. **Real-Time Walk Animation During Horizontal Pipe Entry**:
     - Maintained active walking frame cycles throughout horizontal pipe transitions instead of static idle frame.

### 43. Sprint 7 TV1 4-Level Release Contract & Test Synchronization (`S7-TV1-01`..`04`)
- **Date:** 2026-08-15
- **Author:** TV1 (Duong)
- **Status:** Historical snapshot — completed then; clean build and 20/20 CTest suites passed. Not current RC evidence.
- **Description:**
  - `include/core/LevelCatalog.h`
  - `levels/level4.txt`
  - `tests/LevelCatalogTests.cpp`
  - `tests/PlayStateTests.cpp`
  - `tests/TV5IntegrationTests.cpp`
  - `tests/Gate0ContractTests.cpp`
- **Logic Changes:**
  1. Formalized Sprint 7 4-level release contract in `LevelCatalog.h`: `1 (1-1 Overworld, OVERWORLD)` -> `2 (1-2 Underground, UNDERGROUND)` -> `3 (1-3 Underwater, UNDERWATER)` -> `4 (1-4 Castle, CASTLE)` -> `Win boundary at 5`.
  2. Synchronized `LevelCatalogTests.cpp` to verify 4 release levels, world labels `1-1` to `1-4`, music IDs, and boundary `isPastFinalLevel(5) == true`.
  3. Synchronized `PlayStateTests.cpp` progression simulation to test `1 -> 2 -> 3 -> 4 -> Win`.
  4. Synchronized `TV5IntegrationTests.cpp` audio progression assertions across all 4 levels (`OVERWORLD`, `UNDERGROUND`, `UNDERWATER`, `CASTLE`) to `WIN`.
  5. Updated `levels/level4.txt` with valid Castle finale layout meeting all block route and validator constraints; updated `Gate0ContractTests.cpp` release level loading to include `level4.txt`.
  6. Verified full 20/20 test suites pass with 0 failures under CTest at that snapshot; this is not the current registered-suite result.

### 44. Level / Stage Select UI & Navigation Integration (`LevelSelectState`)
- **Date:** 2026-08-15
- **Author:** TV1 (Duong)
- **Status:** Historical snapshot — completed then; clean build and 20/20 CTest suites passed. Not current RC evidence.
- **Description:**
  - `include/states/LevelSelectState.h`
  - `src/states/LevelSelectState.cpp`
  - `include/states/CharacterSelectState.h`
  - `src/states/CharacterSelectState.cpp`
### 45. Stage Select UI Polish: Clean Snapshot Previews & Streamlined Layout
- **Date:** 2026-08-15
- **Author:** TV1 (Duong)
- **Status:** Historical snapshot — completed then; clean build and 20/20 CTest suites passed. Not current RC evidence.
- **Description:**
  - `include/states/LevelSelectState.h`
  - `src/states/LevelSelectState.cpp`
  - `tests/DisplayCameraUITests.cpp`
  - `assets/textures/ui/stage_1.png`
  - `assets/textures/ui/stage_2.png`
  - `assets/textures/ui/stage_3.png`
  - `assets/textures/ui/stage_4.png`
- **Logic Changes:**
  1. Streamlined `LevelSelectState` layout according to "Less, but better" design aesthetics, removing redundant metadata lines (`DIFF`, `STYLE`, `ENEMY`, `STATUS`).
  2. Downloaded and integrated authentic retro NES gameplay screenshots for all 4 release stages (1-1 Overworld, 1-2 Underground, 1-3 Underwater, 1-4 Castle).
  3. Added smart top-HUD cropping (`topCrop = 32px`) to eliminate squished score/time numbers, presenting pristine in-game scenery.
  4. Aligned screen header (`SELECT WORLD & STAGE`, `Choose a stage to begin your adventure`) and footer navigation hints (`[A / D] SELECT STAGE    [ENTER / CLICK] START GAME    [ESC] BACK`) with balanced margins and zero clipping.
  5. Added dynamic action tags (`> PLAY <` in gold on active card, `STAGE N` on inactive cards) with animated golden pulsing borders.
  6. Verified via offscreen rendered snapshot inspection and 20/20 passing CTest suites.

### 46. Code Review Remediation for Commit `cc80466` (P0/P1/P2 Quality & Performance Fixes)
- **Date:** 2026-08-15
- **Author:** TV1 (Duong)
- **Status:** Historical snapshot — completed then; clean build with 0 warnings, 20/20 CTest suites passed (100% pass rate). Not current RC evidence.
- **Description:**
  - `include/items/Item.h`
  - `include/items/Coin.h`
  - `include/level/Level.h`
  - `src/level/Level.cpp`
  - `include/entities/Mario.h`
  - `src/entities/Mario.cpp`
  - `include/states/LevelSelectState.h`
  - `src/states/LevelSelectState.cpp`
  - `include/states/CharacterSelectState.h`
  - `src/states/CharacterSelectState.cpp`
  - `levels/level4.txt`
  - `tests/LevelValidatorTests.cpp`
  - `docs/change_in_develop.md`
- **Logic Changes:**
  1. **P0/P1 — Dynamic Cast Elimination in Hot Loop (`Level.cpp`)**:
     - Added `virtual bool isCoin() const { return false; }` in `include/items/Item.h`.
     - Overrode `bool isCoin() const override { return true; }` in `include/items/Coin.h`.
     - Replaced `dynamic_cast<const Coin*>` inside `Level::checkItemCollisions()` with `item->isCoin()` virtual check and safe `static_cast<const Coin*>`, completely removing RTTI overhead in the hot collision loop (adhering to `AGENTS.md` Rule #5).
  2. **P0/P1 — Mario Update Function Length & Physics Sync Order (`Mario.cpp`)**:
     - Moved `syncPhysics()` unconditionally to line 1 of `Mario::update(float dt)`, ensuring Box2D physics solver state is always synchronized before any movement logic runs (adhering to `AGENTS.md` Rule #3).
     - Decomposed the ~176-line `Mario::update()` into 5 clean, focused helper methods: `handleDeathPhase(dt)`, `handleTransformPhase(dt)`, `handleSpawnPhase(dt)`, `updateMovementAnimations(dt)`, and `applyWorldBoundsClamp()`.
     - Reduced `Mario::update()` to ~40 lines, strictly complying with the 40-50 line convention.
  3. **P0/P1 — Level Update Function Length Deconstruction (`Level.cpp`)**:
     - Decomposed the ~235-line `Level::update()` by extracting `updateFlagSequence(dt)`, `updateEntities(dt)`, and `updateExplosions()`.
     - Reduced `Level::update()` to ~45 lines.
  4. **P2 — Decomposed `onEnter()` in Select States (`LevelSelectState.cpp`, `CharacterSelectState.cpp`)**:
     - Split `LevelSelectState::onEnter()` into `initStageTextures()`, `initBackdropPanel()`, `initTextLabels()`, and `initStageCards()`.
     - Split `CharacterSelectState::onEnter()` into `initBackdropPanel()`, `initCards()`, `initAvatars()`, `initTextLabels()`, and `initMenu()`.
     - Silenced nodiscard compiler warning for image loading in fallback preview path.
  5. **P2 — Cleaned Trailing Empty Lines in `levels/level4.txt`**:
     - Removed trailing empty lines 26-27 so `level4.txt` conforms cleanly to rectangular map bounds.
  6. **P2 — Extended `LevelValidatorTests.cpp` Coverage**:
     - Added `testProductionLevelsLoad()` ensuring all 4 release levels (`level1.txt`, `level2.txt`, `level3.txt`, `level4.txt`) are validated for valid non-zero dimensions, single Mario spawn (`M`), single Flagpole (`F`/`T`), and valid parsing.
  7. **Release Gate Verification**:
     - Ran full CTest suite: all 20/20 test suites passed in 8.79s with 0 failures at that historical snapshot; this is not current RC evidence.

### 47. Sprint 7 verification correction and release-evidence status
- **Date:** 2026-08-16
- **Author:** Documentation update from independent tester evidence
- **Status:** Current evidence is mixed; no final RC sign-off.
- **Current facts:** The catalog metadata contract is four entries (`1-1`
  Overworld, `1-2` Underground, `1-3` Underwater, `1-4` Castle). Retry,
  `highestUnlockedLevel` Level Select bounds, and v1 monotonic save bounds are
  covered by current state/save tests. Fresh writable isolated MinGW Debug and
  Release `BUILD_TESTING=ON` CTest runs each passed `21/21`; a Release
  `BUILD_TESTING=OFF` production build also passed. Test-target-only NDEBUG
  restoration gives test targets `-DNDEBUG` then `-UNDEBUG`, while
  `game_lib`/`SuperMario` retain only `-DNDEBUG`; no assert-disabled or
  production compiler warnings were observed apart from external Box2D CMake
  deprecation. MSVC was statically reviewed but not executed.
- **TV5 facts:** Four music tracks and Star/death/GameOver/Win/volume/HUD and
  controlled missing/invalid-track behavior pass automated integration tests.
  The package allowlist is 37 assets plus 6 levels/configs (43 entries), with
  clean Debug/Release inventory and negative missing-file configure evidence.
- **Limitations:** `levels/level3.txt` remains Castle-style and its semantic
  Underwater acceptance/playthrough is a TV4-owned `BLOCKED` gate. Manual
  four-level playthrough, screenshots, device-audio, source/license
  attribution, `S6-TV5-43/44` dispositions, `BUG-038`, a same-final-RC hash,
  and final TV1/TV5 sign-off remain `REVIEW`/`PENDING`.
- **References:** [S7_TV1_TV5_STATUS.md](management/S7_TV1_TV5_STATUS.md),
  [S7_TV5_PACKAGE_MANIFEST.md](management/S7_TV5_PACKAGE_MANIFEST.md),
  [TV1_CHANGES_SUMMARY.md](management/TV1_CHANGES_SUMMARY.md), and
  [TV5_CHANGES_SUMMARY.md](management/TV5_CHANGES_SUMMARY.md).

### 48. Fix Fire State Growth/Shrink Transformation Animation Selection
- **Date:** 2026-08-16
- **Author:** TV1 (Duong)
- **Status:** Completed; clean build and 21/21 CTest suites passed (100% pass rate).
- **Description:**
  - `src/entities/Mario.cpp`
  - `tests/MarioPhysicsTests.cpp`
- **Detailed Logic Changes:**
  1. **Fixed Priority in `isGrowth` / `isShrink` in `Mario::applyStateTransition()`**:
     - Previously, `m_characterType == CharacterType::LUIGI` was evaluated before `usesFireTransition`, causing Luigi in `FIRE_SMALL` upgrading to `FIRE_SUPER` (e.g., collecting a Mushroom) to mistakenly play `GrowShrink::Luigi::growSequence()` (normal green Luigi small->medium->big frames) instead of Fire transformation frames.
     - Changed the sequence selector to prioritize `usesFireTransition` (`GrowShrink::FireMario::growSequence()` / `shrinkSequence()`) for both Mario and Luigi whenever transitioning between Fire body states, and preserved character-specific normal sequences (`Luigi::growSequence()` / `Mario::growSequence()`) for standard non-fire state transitions.
  2. **Character-Aware Fire Upgrade/Downgrade Sequence**:
     - Updated `isFireUpgrade || isFireDowngrade` presentation branch so Luigi properly uses `GrowShrink::Luigi::bigFireSequence()` / `smallFireSequence()`, while Mario uses `GrowShrink::FireMario::bigFireSequence()` / `smallFireSequence()`.
  3. **Regression Test Coverage in `MarioPhysicsTests.cpp`**:
     - Extended `testLuigiTransformationUsesLuigiRows()` to verify that `FIRE_SMALL` + Mushroom -> `FIRE_SUPER` explicitly renders frames from `SpriteFrames::shared::GrowShrink::FireMario::BIG` instead of the green normal Luigi row.
  4. **Release Gate Verification**:
     - Ran full CTest suite: all 21/21 test suites passed in 9.50s with 0 failures.

### 49. Fix Defeat Score Popups (Fireball, Shell, Star, Block Bump) & Implement Shell Kill Streak
- **Date:** 2026-08-16
- **Author:** TV3 (Bao) & TV1 (Duong)
- **Status:** Completed; clean build and 21/21 CTest suites passed (100% pass rate).
- **Description:**
  - `include/entities/Mario.h`
  - `src/entities/Mario.cpp`
  - `include/entities/Koopa.h`
  - `src/entities/Koopa.cpp`
  - `include/physics/CollisionManager.h`
  - `src/physics/CollisionManager.cpp`
  - `src/level/Level.cpp`
  - `tests/StompScoreTests.cpp`
- **Detailed Logic Changes:**
  1. **Generic Score Award Queueing (`Mario::queueScoreAward`)**:
     - Added `Mario::queueScoreAward(position, points, grantsLife)` to handle score point awards, 1UP lives addition + `EventType::ONE_UP_COLLECTED` notification, and queueing of `StompScoreAward` for `Level` to render as `ScorePopup`.
  2. **Fireball Defeat Score Popup (`CollisionManager::defeatEnemy`)**:
     - Fixed missing score popup when enemies are defeated by FireBall (`DefeatCause::FIREBALL`). It now queues a 200-point `ScorePopup` at the defeated enemy's center position.
  3. **Koopa Shell Sliding Streak & Combo Scoring (`Koopa` & `CollisionManager`)**:
     - Added `m_shellKillStreak` to `Koopa` class to track consecutive kills by the sliding shell; reset on kick, stomp, and death.
     - Updated `CollisionManager::defeatEnemy` with `streakIndex` parameter applying canonical SMB1 progression: `200 -> 400 -> 800 -> 1000 -> 2000 -> 4000 -> 5000 -> 8000 -> 1UP` and spawning appropriate `ScorePopup`s at each victim's position.
     - Updated `CollisionManager::tryShellKill` lambda to query and increment shell kill streak on successful defeat transactions.
  4. **Star & Block Bump Score Popups**:
     - Added score popup generation for Starman kills (200 pts) and Block bump kills (100 pts).
  5. **Immediate Score Popup Processing in `Level::update()`**:
     - Added a second drain call to `processPendingStompScorePopups()` after `updateEntities()` and `removeDeadEntities()` so popups generated during entity/block updates appear in the very same frame without 1-frame latency.
  6. **Unit Test Suite Expansion (`StompScoreTests.cpp`)**:
     - Added `testFireballDefeatScoreAndPopup()`, `testSlidingShellDefeatChainAndPopups()` (testing full 8-step score progression + 1UP), and `testStarAndBlockBumpScoreAndPopups()`.
  7. **Release Gate Verification**:
     - Ran full CTest suite: all 21/21 test suites passed in 5.81s with 0 failures.

### 50. Item/Enemy Theme Support, Pixel Sharpness & Title Screen Redesign
- **Date:** 2026-08-16
- **Author:** TV1 (Duong), TV2 (Nhat), TV5 (Truyen)
- **Status:** Completed; clean build and 21/21 CTest suites passed (100% pass rate).
- **Description:**
  - `include/items/Coin.h`
  - `src/items/Coin.cpp`
  - `include/items/Mushroom.h`
  - `src/items/Mushroom.cpp`
  - `src/entities/QuestionBlock.cpp`
  - `include/patterns/EntityFactory.h`
  - `src/patterns/EntityFactory.cpp`
  - `src/entities/CheepCheep.cpp`
  - `levels/level3.txt`
  - `levels/cheep_cheep.txt`
  - `levels/elevators.txt`
  - `include/states/MenuState.h`
  - `src/states/MenuState.cpp`
  - `src/states/LevelSelectState.cpp`
  - `src/states/CharacterSelectState.cpp`
  - `src/states/PauseState.cpp`
  - `src/states/GameOverState.cpp`
  - `src/states/WinState.cpp`
  - `src/ui/HUD.cpp`
  - `tests/SpriteFramesThemeTests.cpp`
- **Detailed Logic Changes:**
  1. **Item Theme Propagation (`Coin` & `Mushroom`)**:
     - Added `LevelTheme` support to `Coin` and `Mushroom`. Map coins (`'C'`) and mushrooms now resolve palette-accurate sprites corresponding to the active `LevelTheme` (`OVERWORLD`, `UNDERGROUND`, `CASTLE`, `UNDERWATER`).
     - `QuestionBlock::onHit()` now maps its `BlockTheme` to `LevelTheme` and passes it to spawned `Coin` and `Mushroom` entities so power-ups emerging from blocks match the level palette.
     - Updated `EntityFactory::createItem` and `createFromTileCode` to forward `LevelTheme`.
  2. **Cheep Cheep Red/Green Swim Animation Fix (`CheepCheep.cpp`)**:
     - Fixed `CheepCheep::initAnimations` so red Cheep Cheeps (`m_color == CheepCheepColor::RED`) use `redSwimFrames()` across all themes (`UNDERGROUND`, `CASTLE`, `UNDERWATER`, `OVERWORLD`) instead of defaulting to green swim frames in non-overworld themes.
  3. **Level 3 (World 1-3 Underwater) Authentic Redesign (`levels/level3.txt`)**:
     - Converted `levels/level3.txt` from a legacy Castle duplicate into an authentic, rich Underwater coral reef stage featuring swimming Cheep Cheeps (`c`, `x`), coral formations (`1`), question blocks (`?`, `U`, `f`, `O`), and floating coin paths (`C`), fully compliant with `Gate0ContractTests` and `LevelValidatorTests`.
     - Updated `levels/cheep_cheep.txt` and `levels/elevators.txt` documentation and route comments.
  4. **Global UI Pixel Sharpness & Font Smoothing Elimination**:
     - Added `m_font.setSmooth(false)` across all UI states (`MenuState`, `LevelSelectState`, `CharacterSelectState`, `PauseState`, `GameOverState`, `WinState`, `HUD`) to eliminate font texture blurriness and achieve razor-sharp retro pixel typography.
     - Disabled texture smoothing on preview cards and character avatars (`tex.setSmooth(false)`).
  5. **Polished Title / Cover Screen Overhaul (`MenuState.cpp`)**:
     - Replaced the squished fractional-scaled 256px NES banner with a full-canvas (640x360) presentation:
       - Double-row ground tiles, classic green pipes, and floating question/brick blocks.
       - Top arcade HUD (`MARIO`, `WORLD 1-1`, `TIME 400`, `x00` spinning coin).
       - Floating 3D Title Plaque (`SUPER MARIO - CS202 REMASTERED`) with gold/crimson styling and subtle bobbing animation.
       - Live animated Mario (left), Luigi (right), and Goomba.
       - Arcade `TOP - 000000` high-score display and blinking `PRESS ENTER OR CLICK TO START`.
       - Interactive menu with animated Super Mushroom cursor.
  6. **Release Gate Verification**:
     - Ran full CTest suite: all 21/21 test suites passed in 5.37s with 0 failures.

### 51. Fix Score Popup Texture Rect Mapping in Shared Atlas
- **Date:** 2026-08-16
- **Author:** TV5 (Truyen) & TV1 (Duong)
- **Status:** Completed; clean build and 21/21 CTest suites passed (100% pass rate).
- **Description:**
  - `include/core/SpriteFrames_shared.h`
  - `src/entities/ScorePopup.cpp`
  - `tests/SpriteFramesThemeTests.cpp`
- **Detailed Logic Changes:**
  1. **Atlas Coordinate Grid Alignment in `SpriteFrames_shared.h`**:
     - Discovered that score frame coordinates in `items_objects.png` were completely misaligned with the texture atlas:
       - `SCORE_200` was previously pointing to `(252, 26)` which is actually `1000`.
       - `SCORE_400` & `SCORE_800` were pointing to `(270, 26)` & `(288, 26)` which are empty/transparent areas.
       - `SCORE_1000` was pointing to `(234, 36)` which is actually `200`.
       - `SCORE_4000` & `SCORE_5000` were pointing to empty areas.
       - `SCORE_8000` was pointing to `(234, 46)` which is `400`.
       - `SCORE_1UP` was pointing to `(252, 46)` which is `4000`.
     - Corrected the entire score mapping to match the authentic 2-column vertical grid:
       - Column X=234: `SCORE_100` (Y=26), `SCORE_200` (Y=36), `SCORE_400` (Y=46), `SCORE_500` (Y=56), `SCORE_800` (Y=66).
       - Column X=252: `SCORE_1000` (Y=26), `SCORE_2000` (Y=36), `SCORE_4000` (Y=46), `SCORE_5000` (Y=56), `SCORE_8000` (Y=66), `SCORE_1UP` (Y=76).
  2. **Added `SCORE_500` Frame Support (`ScorePopup.cpp`)**:
     - Added `case 500: return SCORE_500;` to `ScorePopup::frameFor` to handle 500-point awards cleanly.
  3. **Expanded Regression Tests (`SpriteFramesThemeTests.cpp`)**:
     - Added `testScoreCoordinates()` asserting exact pixel positions for all 11 score text frames (`SCORE_100` .. `SCORE_8000`, `SCORE_1UP`).
  4. **Release Gate Verification**:
     - Ran full CTest suite: all 21/21 test suites passed in 10.09s with 0 failures.

### 52. Multi-Theme Score Popups & Title Screen Layout Fine-tuning
- **Date:** 2026-08-16
- **Author:** TV1 (Duong), TV2 (Nhat), TV5 (Truyen)
- **Status:** Completed; clean build and 21/21 CTest suites passed (100% pass rate).
- **Description:**
  - `include/entities/ScorePopup.h`
  - `src/entities/ScorePopup.cpp`
  - `src/level/Level.cpp`
  - `include/states/MenuState.h`
  - `src/states/MenuState.cpp`
  - `tests/DisplayCameraUITests.cpp`
- **Detailed Logic Changes:**
  1. **Multi-Theme Score Popups Support (`ScorePopup` & `Level`)**:
     - Extended `ScorePopup` to take `LevelTheme` and resolve palette-themed score digits across all 4 environments:
       - Overworld: base Y offsets (Y=26..76)
       - Underground: cyan palette Y offsets (+108 -> Y=134..184)
       - Castle: stone grey palette Y offsets (+216 -> Y=242..292)
       - Underwater: teal palette Y offsets (+324 -> Y=350..400)
     - `Level::spawnScorePopup` forwards `m_theme` so floating defeat score numbers visually match the environment palette.
  2. **Title Screen Polish & Clutter Removal (`MenuState.cpp`)**:
     - Removed arbitrary bounding boxes and floating blocks that overlapped with menu text.
     - Replaced with authentic, clean NES Super Mario Bros title composition:
       - Large gold `SUPER MARIO BROS.` 3D title with crimson shadow.
       - Centered `(C) 1985 NINTENDO` and `TOP - 000000` high score.
       - Perfectly aligned `1 PLAYER GAME` and `STAGE SELECT` with animated Super Mushroom cursor.
       - Blinking `PRESS ENTER OR CLICK TO START` prompt.
       - Clear, unobstructed ground with animated Mario, Luigi, and Goomba.
  3. **Visual Verification (`DisplayCameraUITests.cpp`)**:
     - Added `testMenuStateRenderSnapshot()` to render and export full 640x360 frame snapshots for automated visual validation.
  4. **Release Gate Verification**:
     - Ran full CTest suite: all 21/21 test suites passed in 9.43s with 0 failures.

### 53. Title Cover Screen Branding & Clutter Cleanup
- **Date:** 2026-08-16
- **Author:** TV1 (Duong) & TV2 (Nhat)
- **Status:** Completed; clean build and 21/21 CTest suites passed (100% pass rate).
- **Description:**
  - `include/states/MenuState.h`
  - `src/states/MenuState.cpp`
- **Detailed Logic Changes:**
  1. **Removed In-Game HUD Elements from Title Screen (`MenuState.cpp`)**:
     - Removed the in-game gameplay HUD bar (`MARIO 000000 | x00 WORLD 1-1 TIME 400`) and the `TOP - 000000` text to make the title screen clean, authentic, and uncluttered.
  2. **Added Course & Group Branding**:
     - Added `CS202 - OOP FINAL PROJECT` in crisp white retro pixel font directly under the 3D Title logo (`Y=108`).
     - Added `GROUP 04` in arcade gold retro pixel font (`Y=126`).
  3. **Optimized Spacing & Vertical Hierarchy**:
     - Balanced menu options `1 PLAYER GAME` and `STAGE SELECT` (`Y=182`) with aligned animated mushroom cursor.
     - Positioned copyright `(C) 1985 NINTENDO` cleanly below the menu (`Y=228`).
     - Maintained clear open sky for blinking `PRESS ENTER OR CLICK TO START` (`Y=262`).
  4. **Release Gate Verification**:
     - Ran full CTest suite: all 21/21 test suites passed in 5.70s with 0 failures.

### 54. Title Screen Pipe Symmetry & Title Punctuation Cleanup
- **Date:** 2026-08-16
- **Author:** TV1 (Duong) & TV2 (Nhat)
- **Status:** Completed; clean build and 21/21 CTest suites passed (100% pass rate).
- **Description:**
  - `src/states/MenuState.cpp`
- **Detailed Logic Changes:**
  1. **Centered & Symmetrized Green Pipes (`MenuState.cpp`)**:
     - Adjusted right pipe position from `x=576` (which touched the right edge) to `x=544..608`.
     - Now both the left pipe (`x=32..96`) and right pipe (`x=544..608`) maintain an identical 32px margin from the screen borders for flawless horizontal symmetry.
  2. **Balanced Ground Characters**:
     - Centered Goomba precisely at `x=304` (center at `x=320`).
     - Balanced Mario at `x=184` (center at `x=200`, -120px from center) and Luigi at `x=424` (center at `x=440`, +120px from center).
  3. **Removed Trailing Dot in Title**:
     - Changed `"SUPER MARIO BROS."` to `"SUPER MARIO BROS"` in both shadow and main text.
  4. **Release Gate Verification**:
     - Ran full CTest suite: all 21/21 test suites passed in 9.70s with 0 failures.

### 55. Authentic NES Underwater Cheep Cheep Sprite Configuration
- **Date:** 2026-08-16
- **Author:** TV4 (Vy) & TV1 (Duong)
- **Status:** Completed; clean build and 21/21 CTest suites passed (100% pass rate).
- **Description:**
  - `include/core/SpriteFrames_udw.h`
  - `src/level/Level.cpp`
- **Detailed Logic Changes:**
  1. **Maintained Authentic NES Underwater Palette (`SpriteFrames_udw.h`)**:
     - Preserved `UW_SWIM_UP ({292, 164})` and `UW_SWIM_DOWN ({310, 164})` as canonical underwater swimming fish frames, honoring the authentic grey/white NES 1985 underwater hardware palette for World 1-3 / World 2-2.
     - Kept `RED_SWIM_UP ({0, 370})` available for red jumping fish variants.
  2. **Configured Level Cheep Cheep Routes (`Level.cpp`)**:
     - Set `Level::spawnCheepCheepRoutesFromTileMap` to spawn `CheepCheepColor::GREEN` so underwater levels render the authentic NES grey/white Cheep Cheeps swimming along reef routes.
  3. **Release Gate Verification**:
     - Ran full CTest suite: all 21/21 test suites passed in 9.12s with 0 failures.

### 56. Comprehensive Game UI Redesign & Screenshot Layout Overhaul
- **Date:** 2026-08-17
- **Author:** TV1 (Duong), TV2 (Nhat), TV5 (Truyen)
- **Status:** Completed; 31/31 CTest suites passed (100% pass rate).
- **Description:**
  - `include/states/MenuState.h` & `src/states/MenuState.cpp`
  - `include/states/LevelSelectState.h` & `src/states/LevelSelectState.cpp`
  - `include/states/CharacterSelectState.h` & `src/states/CharacterSelectState.cpp`
  - `include/states/CoopCharacterSelectState.h` & `src/states/CoopCharacterSelectState.cpp`
  - `include/states/PvpCharacterSelectState.h` & `src/states/PvpCharacterSelectState.cpp`
  - `include/states/PauseState.h` & `src/states/PauseState.cpp`
  - `include/states/GameOverState.h` & `src/states/GameOverState.cpp`
  - `include/states/WinState.h` & `src/states/WinState.cpp`
  - `include/ui/HUD.h` & `src/ui/HUD.cpp`
  - `include/ui/UIMenuWidget.h` & `src/ui/UIMenuWidget.cpp`
  - `tests/CaptureScreenshots.cpp` & `CMakeLists.txt`
- **Detailed Logic Changes:**
  1. **Automated Screenshot Generator (`CaptureScreenshots.cpp`)**:
     - Added headless capture utility exporting 14 full-fidelity PNG screenshots (`01` through `14`) across all screens directly to `screenshots_output/`.
  2. **Main Title Screen (`MenuState.cpp`)**:
     - Removed redundant `TOP - ...` score line and redundant `STAGE SELECT` menu item.
     - Cleanly spaced the 3 core game modes (`1 PLAYER GAME`, `2 PLAYER CO-OP`, `2 PLAYER VERSUS`) with exact 24px vertical spacing at `y = 178.f`.
     - Positioned `CS202 OOP PROJECT` at top left (`x=24, y=12`) and `GROUP 04` at top right (`x=616, y=12`).
  3. **Stage Select Screen (`LevelSelectState.cpp`)**:
     - Fixed thumbnail preview boxes to maintain crisp 4:3 aspect ratio (121x90) without distortion.
     - Added stage descriptions (`GRASSLANDS`, `UNDERGROUND`, `DEEP OCEAN`, `CASTLE KEEP`) and status badges (`CLEARED`, `UNLOCKED`, `LOCKED`).
     - Added glowing gold selection outline and clean footer keybindings.
  4. **Character Select Screens (`CharacterSelectState.cpp`, `CoopCharacterSelectState.cpp`, `PvpCharacterSelectState.cpp`)**:
     - Redesigned into **Side-by-Side Dual Hero Cards** (Mario Crimson Card at x=65 vs Luigi Emerald Card at x=335, symmetrical 20px padding and 30px central gap).
     - Centered stat meter blocks horizontally inside each card using dynamic text bounds (`getLocalBounds()`) with 8pt crisp typography.
     - Implemented dynamic origin recalculation for action buttons (`> SELECT MARIO <`, `> SELECT LUIGI <`) so changing active hero retains exact mathematical centering.
     - Fine-tuned top title (`TITLE_Y = 14`, `SUBTITLE_Y = 36`) and player badges to completely eliminate any visual overlap with headers in Co-op and PvP modes.
     - Added animated showcase sprites, hero titles, attribute stat meters, and pulsing gold active selection highlights.
  5. **Pause & Audio Settings Modal (`PauseState.cpp`, `PauseState.h`)**:
     - Converted Pause screen into a unified 4-item interactive arcade modal (`MUSIC`, `SFX`, `RESUME GAME`, `QUIT TO MAIN MENU`).
     - Renamed `SOUND` to `SFX` and aligned both labels to 7-character headers (`"MUSIC  "`, `"SFX    "`) with matching 2-character prefixes (`"> "` vs `"  "`) for symmetrical vertical column alignment.
     - Implemented dynamic 2D centering (origin at text bounding box midpoint) and adaptive highlight capsule with 36px horizontal / 24px vertical padding for balanced framing.
     - Enabled intuitive `Up` / `Down` (or `W` / `S`) navigation across all 4 items with a pulsing gold selection highlight bar.
     - Enabled seamless `Left` / `Right` (or `A` / `D`) volume adjustment on the active volume slider with instant SFX feedback (`coin` preview on SFX volume, `bump` on Music volume) and atomic `SaveManager` persistence.
     - Added mouse hover and click interaction support across all 4 menu rows with left/right volume delta detection.
     - Compacted progress bars and text strings to fit within the modal frame with zero overflow.
  6. **Cross-Screen Visual & Color Hierarchy Polish (`CharacterSelectState`, `Coop`, `PvP`, `LevelSelectState`, `GameOverState`, `WinState`)**:
     - Eliminated monotone yellow overuse by establishing a layered multi-tone color system:
       - Active/Selected: Bright Glowing Gold (`#FFD700`)
       - Inactive/Unselected items: Crisp Pure White (`#F0F5FF` / `#EBF0FF`)
       - Subtitles & Section headers: Soft Cyan (`#82C8FF`), Mint Green (`#82FFB4`), and Coral (`#FFA08C`)
       - Footer keybinding hints: Soft Ice Blue (`#B4D2FA`)
  7. **Game Over & Victory Scorecards (`GameOverState.cpp`, `WinState.cpp`)**:
     - Replaced plain text screens with framed arcade scorecards displaying Final Score, High Score, Stage Reached, and World Status.
     - Added glowing title animations and responsive action menus.
  8. **Gameplay Heads-Up Display (`HUD.cpp`)**:
     - Balanced all 5 columns evenly across the 640px logical canvas (`SCORE`, `COINS`, `WORLD`, `TIME`, `LIVES`).
     - Increased font size to 10pt with crisp black text outlines (1.2px) for maximum contrast against all stage themes.
  9. **Release Gate Verification & Cleanup**:
     - Executed full CTest suite: all 31/31 test suites passed in 21.99s with 0 errors.
     - Removed all temporary screen capture artifacts (`screenshots_output/`, `level_select_preview.png`, `menu_state_preview.png`, `tests/CaptureScreenshots.cpp`) ensuring a pristine repository state.


### 57. Enemy Visual Effects Overhaul & Aesthetic Polish ("Authentic NES Pixel Art Polish")
- **Date:** 2026-08-17
- **Author:** TV1 (Duong), TV4 (Vy), TV5 (Truyen)
- **Status:** Completed; 31/31 CTest suites passed (100% pass rate).
- **Description:**
  - `assets/textures/enemies/hammer.png`
  - `src/core/TextureManager.cpp`
  - `src/entities/Hammer.cpp`
  - `include/entities/Podoboo.h` & `src/entities/Podoboo.cpp`
  - `src/entities/Bowser.cpp`
  - `src/entities/BulletBillLauncher.cpp`
  - `include/entities/Paratroopa.h` & `src/entities/Paratroopa.cpp`
  - `src/entities/Blooper.cpp`
- **Detailed Logic Changes:**
  1. **Fixed Hammer Transparency & Rotation Center (`hammer.png` & `Hammer.cpp`)**:
     - Converted `assets/textures/enemies/hammer.png` from a 24-bit RGB file with an opaque blue box `RGB(146, 144, 255)` into a true 32-bit RGBA PNG with alpha transparency channel.
     - Fixed `Hammer::update` so it does not overwrite `m_sprite->setPosition(m_position)`, allowing `updatePresentation` to control the exact center-of-mass rotation pivot `{8.f, 8.f}` at `m_position + m_size / 2.f`. Hammers now spin seamlessly and smoothly through the air without wobbly or rectangular box glitches.
  2. **Nearest-Neighbor Texture Filtering Across All Assets (`TextureManager.cpp`)**:
     - Explicitly enforced `texture->setSmooth(false)` on all created `sf::Texture` instances in `TextureManager::loadTexture`, ensuring sharp retro 8-bit NES pixel presentation and eliminating bilinear blur/plastic appearance.
  3. **Podoboo Vertical Flip on Descent (`Podoboo.cpp`)**:
     - Implemented dynamic vertical sprite flipping on descent (`setScale({2.f, -2.f})` with origin `{0.f, 16.f}` when `m_velocityY > 0.f`), matching authentic NES lava bubble physics where the flame point aims downward when plunging into lava.
  4. **Bowser Multi-Theme Palettes (`Bowser.cpp`)**:
     - Updated `walkFrames`, `fireFrames`, and `throwFrames` in `Bowser.cpp` to accurately return Overworld green skin frames (`WALK_CLOSED`, `WALK_OPEN`, `FIRE_POSE1`, `FIRE_POSE2`) when `LevelTheme::OVERWORLD` is active, and the classic grey/dark palette for Castle/Underground stages.
  5. **Bullet Bill Cannon Launch Muzzle Effect (`BulletBillLauncher.cpp`)**:
     - Added an animated muzzle smoke flash in `BulletBillLauncher::draw()` during the initial launch frame window (`m_fireTimer < 0.2f`), providing visual impact feedback when firing Bullet Bills.
  6. **Paratroopa Wing-Clip & Blooper Flip Smoothness (`Paratroopa.cpp` & `Blooper.cpp`)**:
     - Added wing-clip particle support and centered rotation pivot for Blooper's flipped death presentation (`setOrigin({8.f, 12.f})`).
  7. **Release Gate Verification**:
     - Clean build succeeded and all 31/31 CTest suites passed with 0 errors.

### 58. Fix Enemy Facing Directions, Horizontal Sprite Flipping, and Ledge Turning
- **Date:** 2026-08-17
- **Author:** TV1 (Duong), TV4 (Vy)
- **Status:** Completed; 31/31 CTest suites passed (100% pass rate).
- **Description:**
  - `src/entities/Bowser.cpp`
  - `src/entities/Lakitu.cpp`
  - `src/entities/Paratroopa.cpp`
  - `src/entities/HammerBro.cpp`
- **Detailed Logic Changes:**
  1. **Bowser Horizontal Facing Flip (`Bowser.cpp`)**:
     - Fixed `Bowser::update` to dynamically flip the sprite horizontally (`setScale({-2.f, 2.f})` with origin at texture width) when moving/facing `Direction::RIGHT`. Bowser no longer walks backwards/moonwalks when pacing to the right.
  2. **Lakitu Dynamic Mario Tracking & Facing Flip (`Lakitu.cpp`)**:
     - Updated `Lakitu::update` to orient its facing direction toward Mario's relative column position (and movement velocity) and flip its sprite horizontally when facing `Direction::RIGHT`.
  3. **Hopping Paratroopa Ledge Reversal (`Paratroopa.cpp`)**:
     - Added `isApproachingLedge()` check inside `Paratroopa::patrol()` for `ParatroopaMode::HOP`. Green hopping Paratroopas now correctly detect platform edges and reverse direction instead of walking blindly off ledges.
  4. **Hammer Bro Player-Facing & Throw Orientation (`HammerBro.cpp`)**:
     - Fixed `HammerBro::throwHammer()` and patrol logic to update `setFacingDirection(direction)` so Hammer Bro properly turns around and faces Mario when throwing hammers.
  5. **Release Gate Verification**:
     - Ran full CTest suite: all 31/31 test suites passed in 22.43s with 0 errors.

### 59. Bowser Boss Mechanics, Fire-Breathing Animations, SFX, and Combat Overhaul
- **Date:** 2026-08-17
- **Author:** TV1 (Duong), TV4 (Vy)
- **Status:** Completed; 31/31 CTest suites passed (100% pass rate).
- **Description:**
  - `include/entities/Bowser.h`
  - `src/entities/Bowser.cpp`
  - `include/entities/BowserFire.h`
  - `src/entities/BowserFire.cpp`
  - `src/core/SoundManager.cpp`
- **Detailed Logic Changes:**
  1. **Two-Stage Fire-Breathing Choreography (`Bowser.cpp` & `Bowser.h`)**:
     - Separated the fire-breathing state into clear authentic stages: Phase 1 (0.0s to 0.3s) Windup (`FIRE_WINDUP_ANIMATION`, pose STT #115/#119/#123) where Bowser rears back with open mouth, and Phase 2 (0.3s to 0.7s) Exhale (`FIRE_EXHALE_ANIMATION`, pose STT #116/#120/#124) where fire bursts from his jaws and sustains the breath pose. Eliminated the rapid 0.18s mouth-snapping/jitter loop.
  2. **Accurate Mouth Spawn Coordinates & Multi-Height Fire Breath (`Bowser.cpp`)**:
     - Recalibrated `breatheFire()` spawn coordinates so `BowserFire` emerges directly from Bowser's mouth (`spawnX = m_position.x - 44.f` when facing left, `spawnX = m_position.x + m_size.x - 4.f` when facing right).
     - Implemented authentic SMB1 **3-tier fire elevation distribution**:
       - **Low (40%, `offsetY = +38px`)**: Ground-skimming flame that covers the floor ($Y=38..54\text{px}$), forcing Small and Big Mario to jump. Eliminates the safe camping spot where Small Mario stood immune.
       - **Mid (35%, `offsetY = +22px`)**: Chest-level flame ($Y=22..38\text{px}$) hitting Big Mario torso and Small Mario head.
       - **High (25%, `offsetY = +8px`)**: Head-level flame ($Y=8..24\text{px}$) that allows Small Mario to sprint underneath towards Bowser.
  3. **Audio Preload & Boss SFX Integration (`SoundManager.cpp` & `Bowser.cpp`)**:
     - Preloaded `"bowser_fire"` (`assets/sounds/effects/fire.wav`) and `"bowser_fall"` (`assets/sounds/effects/bowserfall.wav`).
     - Triggered `bowser_fire` roar SFX on fire release and `bowser_fall` SFX on bridge collapse/lava descent in `enterDie()`.
  4. **Patrol AI Pacing & Movement/Facing Decoupling (`Bowser.cpp` & `Bowser.h`)**:
     - Decoupled movement velocity direction (`m_patrolMoveDir`) from visual tracking (`setFacingDirection`). Bowser now naturally paces back and forth across the arena bridge without getting locked moving left, while maintaining visual focus on Mario.
  5. **Boss Jump Balancing, Airborne Animation Freeze & Hit Flash (`Bowser.cpp` & `Bowser.h`)**:
     - Rebalanced boss rhythm to 75% fire breath / 25% low hop (`HOP_SPEED = 300.f`, ~2 tiles), preventing unnatural high-frequency jumping.
     - Froze foot animation while airborne in `Bowser::update` to stop comical mid-air foot pedaling.
     - Added damage flash timer (`m_damageFlashTimer = 0.15s`) that applies a hurt tint (`sf::Color(255, 120, 120, 240)`) when hit by Mario's fireballs.
  6. **Flame Projectile Frame Flicker & Physics Sync (`BowserFire.cpp` & `BowserFire.h` & `HammerBroTests.cpp`)**:
     - Adjusted subtle natural wave motion (`WAVE_AMPLITUDE = 6.f`, `WAVE_FREQUENCY = 1.0f`) and increased fire flickering animation rate to `0.08s` per frame for authentic retro flame presentation.
     - Fixed Box2D wave transform calculation order in `BowserFire::update` to execute before `EnemyProjectile::update(dt)`, eliminating 1-frame rendering desync and jitter.

### 60. World 1-4 Authentic NES Castle Remake & Rotating Firebar Entity Integration
- **Date:** 2026-08-17
- **Author:** TV1 (Duong), TV4 (Vy)
- **Status:** Completed; 31/31 CTest suites passed (100% pass rate).
- **Description:**
  - `include/entities/Entity.h`
  - `include/entities/Firebar.h`
  - `src/entities/Firebar.cpp`
  - `include/patterns/EntityFactory.h`
  - `src/patterns/EntityFactory.cpp`
  - `src/level/Level.cpp`
  - `levels/level4.txt`
  - `scratch/generate_level4.ps1`
- **Detailed Logic Changes:**
  1. **Rotating Firebar Hazard Entity (`Firebar.h` & `Firebar.cpp`)**:
     - Created `Firebar` entity extending `Enemy` with `canBeStomped() = false`, `isIndestructible() = true`, and `isFirebar() = true`.
     - Implemented authentic rotational mechanics using angular velocity (`m_rotationSpeed`), radial fireball spacing (`m_ballSpacing = 16.f`), and animated rotating fireball sprites sourced from `items_objects.png` Castle fireball coordinates (`{180, 270}, 8x8` scaled 2x to 16x16).
     - Implemented `Firebar::checkMarioCollision(const sf::FloatRect& marioBox)` calculating precise bounding box intersections for each fiery orb in the rotating chain.
     - Implemented `void Firebar::draw(sf::RenderTarget& target, sf::RenderStates states) const override` satisfying `sf::Drawable` interface with lazy texture loading fallback.
  2. **Entity Factory & Level Collision Integration (`EntityFactory.cpp`, `Entity.h`, `Level.cpp`)**:
     - Added `virtual bool isFirebar() const` in base `Entity` class (avoiding `dynamic_cast` in hot loops per project rule #5).
     - Added `FIREBAR` to `EnemyType` enum and mapped symbols `'e'` and `'E'` in `EntityFactory::createFromTileCode`.
     - Integrated `Firebar` damage overlap loops in `Level::updateEntities` for both Player 1 (`m_mario`) and Player 2 (`m_mario2`).
  3. **Authentic NES World 1-4 Map Recreation (`levels/level4.txt`)**:
     - Rebuilt the full 168x16 Castle World 1-4 layout matching Ian Albert's verified NES level map:
       - **Section 1 (Cols 0—17)**: Low ceiling entry chamber, downward stone staircase, lava pit with leaping Podoboo (`P`), and post-pit platform.
       - **Section 2 (Cols 18—36)**: High chamber, Question block (`?`), ceiling Firebar (`e`), double lava pits with Podoboos, and central stone island with Firebar mount.
       - **Section 3 (Cols 37—74)**: Long stone corridor with alternating ceiling and floor Firebars (`e`), patrolling Koopa (`K`), and Fire Flower block (`f`).
       - **Section 4 (Cols 75—110)**: Stepping stone platforms (`B`) over double lava pits with leaping Podoboos, 1-Up Mushroom (`U`), Starman (`O`), and springboard (`J`).
       - **Section 5 (Cols 111—125)**: Low ceiling narrow entryway to the boss arena.
       - **Section 6 (Cols 126—153)**: High-vaulted Bowser Boss Chamber with wide lava pit, authentic 11-tile suspended collapsible bridge (`===========`), Bowser boss (`X`), and Bowser Axe (`A`).
       - **Section 7 (Cols 154—168)**: Royal end chamber with Toad / Princess Peach room and flagpole exit sequence markers (`T`, `F`, `|`).
  4. **Release Gate Verification**:
     - Clean build succeeded and all 31/31 CTest test suites passed with 0 errors (100% pass rate).

### 61. Integrate TV4 Commits (`ab75c2e` & `f3a76f1`) via Cherry-Pick
- **Date:** 2026-08-17
- **Author:** TV4 (Vy), TV1 (Duong)
- **Status:** Completed; 31/31 CTest suites passed (100% pass rate).
- **Description:**
  - `levels/level_athletic.txt`
  - `docs/change_in_develop.md`

### 62. Dual Play Camera Screen Containment, Swimming Ceiling Boundary & Pit Hazards
- **Date:** 2026-08-19
- **Author:** TV1 (Duong) & TV3 (Bao)
- **Status:** Completed; 31/31 CTest suites passed (100% pass rate).
- **Description:**
  - `include/entities/Mario.h`
  - `src/entities/Mario.cpp`
  - `include/level/Level.h`
  - `src/level/Level.cpp`
  - `levels/level4.txt`
  - `tests/MarioPhysicsTests.cpp`
  - `tests/CoopFlowTests.cpp`
  - `docs/change_in_develop.md`
- **Detailed Logic Changes:**
  1. **Dual Play (2P Co-op) Camera Viewport Containment (`Level.h`, `Level.cpp`)**:
     - Added `Level::clampCoopPlayersToCamera()` called immediately after camera centering in `Level::updateCoop`.
     - Dynamically computes horizontal viewport bounds (`[viewLeft + halfWidth, viewRight - halfWidth]`) and clamps both Player 1 and Player 2.
     - When leading player rushes ahead, they are blocked at the right screen edge; when trailing player falls behind, they are carried forward by the screen's left edge.
     - Prevents any player from ever wandering off-screen or disappearing during co-op gameplay.
  2. **Ceiling Boundary Limit for Underwater Swimming (`Mario.h`, `Mario.cpp`)**:
     - Implemented top ceiling clamping in `Mario::applyWorldBoundsClamp()` (`bodyPos.y >= halfHeightMeters` when `m_isUnderwater`).
     - Cancels upward velocity (`velocity.y = 0`) when contacting the ceiling, preventing Mario/Luigi from swimming infinitely above the top of the screen in World 1-3.
     - Checked ceiling proximity in `Mario::applyMovementPhysics()` during jump/swim input to prevent unnecessary upward impulses at the surface.
     - Synchronized physics immediately upon clamping.
  3. **Pit Hazard Support & Castle Level 4 Lava Fill (`levels/level4.txt`, `levels/cheep_cheep.txt`)**:
     - Filled missing lava surface (`WW`) and leaping Podoboos (`P.`) in the entry corridor pits of Castle World 1-4 (`levels/level4.txt` cols 21-22 and 31-32), ensuring all pits have complete lava and hazard coverage.
  4. **Regression & Contract Tests (`MarioPhysicsTests.cpp`, `CoopFlowTests.cpp`)**:
     - Added `testUnderwaterCeilingClamp()` in `MarioPhysicsTests` verifying that repeated swim impulses do not exceed `y = 0` and upward velocity is clamped.
     - Added `testCoopPlayersClampedInsideCameraViewport()` in `CoopFlowTests` verifying that one player running right while the other is idle keeps both players strictly within the camera view for 300+ frames.

### 63. World 1-4 Castle Level Refinements & Alternative Bowser Passage
- **Date:** 2026-08-19
- **Author:** TV1 (Duong) & TV4 (Vy)
- **Status:** Completed; 31/31 CTest suites passed (100% pass rate).
- **Description:**
  - `levels/level4.txt`
  - `include/level/TileFrames.h`
  - `src/level/TileMap.cpp`
  - `tests/DisplayCameraUITests.cpp`
  - `docs/change_in_develop.md`
- **Detailed Logic Changes:**
  1. **Section 1 (Entry Corridor) Pipe & Piranha Plant Upgrade**:
     - Raised Castle pipe at cols 31-32 to 3 blocks tall (`pr` at row 10, `{}` at rows 11-12, `00` ground base at rows 13-14), perfectly level with the stone corridor walls.
     - Spawns a Castle Piranha Plant emerging between the stone walls.
  2. **Section 2 Question Blocks Solid Platform Fill**:
     - Replaced pit at cols 46-48 under the `?f?` question block row with continuous solid stone ground (`000` at rows 13-14), providing smooth running terrain.
  3. **Section 4 Bowser Arena Springboard & Horizontal Elevator**:
     - Placed Springboard (`J`) on the stone ledge at col 125, row 9 directly in front of the collapsible bridge.
     - Cut hanging ceiling blocks (cols 120-143, rows 3-5) to provide clear headroom above Bowser.
     - Installed a horizontal moving Elevator platform (`^` at col 122, `~` at col 139, row 5) gliding back and forth over Bowser towards the Axe (`A`), giving players an alternate aerial bypass route.
  4. **Visual Verification & Test Suite**:
     - Generated high-resolution section snapshots (`level4_sec1_corridor.png`, `level4_sec2_powerup.png`, `level4_sec3_stepping.png`, `level4_sec4_bowser.png`, `level4_sec4_bowser_moving.png`) verifying layout alignment.
     - Full clean build and 31/31 CTest test suites pass 100%.

### 64. Koopa Stomp Latch Re-arm & Firebar Damage Normalization
- **Date:** 2026-08-19
- **Author:** TV1 (Duong) & TV3 (Bao)
- **Status:** Completed; 31/31 CTest suites passed (100% pass rate).
- **Description:**
  - `src/entities/Koopa.cpp`
  - `src/level/Level.cpp`
  - `tests/KoopaVariantTests.cpp`
  - `docs/change_in_develop.md`
- **Detailed Logic Changes:**
  1. **Koopa Stomp Latch Re-arm (`Koopa.cpp`)**:
     - Re-armed `m_stompLatch = false` in `Koopa::kick()` and `Koopa::wakeUpFromShell()`.
     - Allows stopping a sliding shell with a downward stomp and allows re-stomping a Koopa that has awakened from its shell into walking state.
  2. **Firebar Damage Normalization (`Level.cpp`)**:
     - Changed Firebar player collision resolution from instant fatal `loseLife()` to standard damage pipeline `queuePowerDown()`.
     - Allows Super and Fire Mario/Luigi to take damage, enter invincibility grace period, and shrink instead of immediately losing a life.
  3. **Regression Tests (`KoopaVariantTests.cpp`)**:
     - Added `testSlidingShellStompReArm()` and `testWakingKoopaStompReArm()` validating multiple stomp cycles.

### 65. Fix Co-op Boundary Character Flicker & Level 3 Left Swim Breach
- **Date:** 2026-08-19
- **Author:** TV1 (Duong) & TV3 (Bao)
- **Status:** Completed; 31/31 CTest suites passed (100% pass rate).
- **Description:**
  - `include/level/Camera.h`
  - `src/level/Camera.cpp`
  - `include/entities/Mario.h`
  - `src/entities/Mario.cpp`
  - `include/level/Level.h`
  - `src/level/Level.cpp`
  - `levels/level3.txt`
  - `tests/CoopFlowTests.cpp`
  - `tests/LevelValidatorTests.cpp`
  - `docs/change_in_develop.md`
- **Detailed Logic Changes:**
  1. **Camera Horizontal Deadzone Configuration (`Camera.h`, `Camera.cpp`, `Level.cpp`)**:
     - Added `setHorizontalDeadzoneRatio(float ratio)` and `getHorizontalDeadzoneRatio()` to `Camera`.
     - In 2P Co-Op mode (`Level::loadFromFile`), sets horizontal deadzone ratio to `0.0f` so leading players advancing at screen edges immediately advance the camera view without deadzone stalling.
  2. **Sprite Layout Synchronization on Physics Sync (`Mario.h`, `Mario.cpp`)**:
     - Overrode `Mario::syncPhysics()` to call `Entity::syncPhysics()` followed immediately by `updateSpriteLayout()`.
     - Ensures bottom-anchored sprite offsets are always synchronized whenever player physics bodies are clamped to the camera viewport boundaries in `Level::clampCoopPlayersToCamera()`, completely eliminating single-frame visual flicker.
  3. **Level 3 Sub-area Boundary Tile Partition (`levels/level3.txt`)**:
     - Placed solid barrier tiles ('S' on rows 1..12 and '0' on rows 13..14) at column 49 in `levels/level3.txt` to partition the overworld pipe entry from the underwater sub-area, preventing players from swimming left into the void.
  4. **Regression Verification Tests (`CoopFlowTests.cpp`, `LevelValidatorTests.cpp`)**:
     - Added regression assertions in `tests/CoopFlowTests.cpp` (`testCoopPlayersClampedInsideCameraViewport()`) verifying 0.0f deadzone ratio and exact position sync.
     - Added `testLevel3BarrierColumn49()` in `tests/LevelValidatorTests.cpp` asserting column 49 is solid across rows 1..14 with 'S' and '0' tiles.


### 66. PvP Arena Platform Headroom & Layout Lowering
- **Date:** 2026-08-19
- **Author:** TV1 (Duong) & TV5 (Truyen)
- **Status:** Completed; 31/31 CTest suites passed (100% pass rate).
- **Description:**
  - `levels/pvp_arena.txt`
  - `tests/PvpArenaTests.cpp`
  - `docs/change_in_develop.md`
- **Detailed Logic Changes:**
  1. **PvP Arena Central Platform & Item Spawner Lowering (`levels/pvp_arena.txt`)**:
     - Shifted contested center pedestal platform (`SSSS`) down by 2 rows from row 3 (Y = 96px) to row 5 (Y = 160px).
     - Shifted Fire Flower item spawn point (`W`) down by 2 rows from row 2 (Y = 64px) to row 4 (Y = 128px).
     - Increases vertical headroom above the center platform to 5 tiles (160px), giving ample clearance for high jumps, aerial stomps, and fireball knockbacks without colliding with the top HUD header (`P1 MARIO 0 - 0 LUIGI P2`).
  2. **Side Step Blocks Lowering & Uniform Stepping (`levels/pvp_arena.txt`)**:
     - Shifted symmetrical flank step blocks at columns 7 and 12 down by 1 row from row 6 (Y = 192px) to row 7 (Y = 224px).
     - Creates a perfectly balanced, uniform 2-tile vertical stepping rhythm: Ground (row 9, Y = 288px) -> Step blocks (row 7, Y = 224px, +2 tiles) -> Center Pedestal (row 5, Y = 160px, +2 tiles).
  3. **12-Row Map Height & Bottom Edge Floor Flush Alignment (`levels/pvp_arena.txt`)**:
     - Added 12th row of solid ground tiles ('0') on row 11 (Y = 352..384px) to expand total arena height from 11 tiles (352px) to 12 tiles (384px).
     - With `CameraVerticalMode::LOCKED` resting against the map floor (`center.y = 384 - 180 = 204px`), the camera view (640x360) renders exactly from Y = 24px to Y = 384px, placing the solid ground floor flush against the bottom window edge and completely eliminating the exposed blue background strip under the floor tiles.
  4. **Regression Tests (`PvpArenaTests.cpp`)**:
     - Updated `testShippedArenaFixtureLoads()` assertions to validate `tileMap.getHeight() == 12`, ground solidity on rows 9..11, and solid step blocks at `(7, 7)` and `(12, 7)` with clear air band across rows 6..8.
     - 31/31 CTest test suites pass 100%.


 
### 67. Fix Box2D IsLocked Assertion Crash & Shell Velocity
 
 - **Date:** 2026-08-20
 
 - **Author:** Agent
 
 - **Status:** Completed; 31/31 CTest suites passed.
 
 - **Modified Files:**
 
     - `src/entities/Goomba.cpp`
     - `src/entities/Koopa.cpp`
 
 - **Detailed Logic Changes:**
 
     1. **Fix IsLocked Box2D Assertion**: Changed `Goomba::onStomp()` to update `m_position` directly without calling `setPosition()` inside the Box2D collision callback. This prevents modifying Box2D transforms while the world is locked during `Step()`, fixing the 100% crash rate in tests like `v5_integration_tests`.
 
     2. **Fix Koopa/Buzzy Beetle Shell Velocity**: Restored `setVelocity()` logic inside `Koopa::kick()` and `reverseDirection()`. The earlier physics refactor had accidentally stripped out shell slide speeds, causing kicked shells to stand completely still instead of sliding across the screen.
 
 
 
 
### 68. Map Geometries and Theme Sync (Level 3 & 4)
- **Date:** 2026-08-20
- **Author:** TV4
- **Status:** Completed
- **Description:**
  - include/core/LevelCatalog.h
  - levels/level3.txt
  - levels/level4.txt
- **Detailed Logic Changes:**
  1. **Level 3 Theme Sync (LevelCatalog.h)**:
     - Changed LevelTheme::OVERWORLD and MusicId::OVERWORLD to UNDERWATER so the deep blue sea background and music correctly load for Level 3.
  2. **Level 3 Map Geometry Fix (level3.txt)**:
     - Blocked an infinite leftward swimming escape route above spawn R1 by extending the S wall tiles upward at column 49 to close the gap at the top of the map.
  3. **Level 4 Castle Authenticity (level4.txt)**:
     - Removed out-of-theme floating coins and replaced bright Overworld-style ? question blocks with castle-appropriate Brick blocks B.
  4. **Level 4 Missing Floor and Springboard Fix (level4.txt)**:
     - Added Piranha Plant Pipes (pr, {}) in the non-lethal bottomless pits between the initial platforms to correctly punish missed jumps.
     - Removed the ceiling blocks directly above the springboard J so Mario can gain enough height to reach the upper elevator path without hitting his head.

### 69. Add Piranha Pipe to Level 4 Late Gap
- **Date:** 2026-08-20
- **Author:** TV4
- **Status:** Completed
- **Description:**
  - levels/level4.txt
- **Detailed Logic Changes:**
  1. **Add Pipe to Final Gap**: Added a Piranha plant pipe (pr, {}) to the 2-tile wide gap (cols 116-117) immediately preceding the long 10-tile solid ground stretch. This fulfills the player's request to place a pipe exactly at the rightmost bottomless pit shown in their screenshot.

### 70. Make Runtime Asset Sync Automatic on Every Build
- **Date:** 2026-08-20
- **Author:** TV1
- **Status:** Completed
- **Description:**
  - CMakeLists.txt
- **Detailed Logic Changes:**
  1. **Decoupled Copy Commands**: Moved the copy_if_different POST_BUILD steps for assets and levels away from the SuperMario target and into a standalone `add_custom_target(CopyRuntimeAssets ALL)`.
  2. **Hot-reload Support**: Now, when users modify levelX.txt (or image files) and hit Build, CMake will execute the copy commands and update the `build/` directory immediately without needing to pointlessly re-link the C++ executable.

### 71. Fix Enemy Ledge Detection, Level 2 Palette/Warps & Level 3 Theme
- **Date:** 2026-08-20
- **Author:** TV4
- **Status:** Completed
- **Description:**
  - src/entities/Spiny.cpp
  - src/entities/Goomba.cpp
  - src/entities/HammerBro.cpp
  - levels/level2.txt
  - src/level/Level.cpp
  - include/core/LevelCatalog.h
  - levels/level3.txt
- **Detailed Logic Changes:**
  1. **Enemy Ledge Probe Fix (Spiny.cpp, Goomba.cpp, HammerBro.cpp)**:
     - Replaced `m_tileMap->isSolid(...)` with `m_tileMap->isEnemySupport(...)` in `isApproachingLedge()` across Spiny, Goomba, and Hammer Bro so that Question Blocks, pipes, and other enemy-supporting surfaces are recognized as valid ground, preventing enemies from getting stuck or prematurely reversing.
  2. **Level 2 Palette & Theme Transitions (level2.txt, Level.cpp)**:
     - Replaced non-themed enemy symbols `'q]'` with `'p]'` and `'k'` with `'K'` in `levels/level2.txt` so theme-responsive underground enemy palettes are properly loaded.
     - Added area theme transition handling in `Level::warpMarioToReturn()` for `level2.txt` (warp 1 -> Underground, warp 4 -> Overworld).
  3. **Level 3 Initial Theme (LevelCatalog.h) & BOM Fix (level3.txt)**:
     - Updated Level 3 initial theme in `LevelCatalog.h` to `LevelTheme::OVERWORLD` and `MusicId::OVERWORLD` to match the starting intro overworld area before pipe warp to underwater.
     - Stripped UTF-8 BOM from `levels/level3.txt` ensuring valid map loading across all environments.

### 73. PvP Top Ceiling Boundary Clamp & Complete HUD Realignment
- **Date:** 2026-08-20
- **Author:** TV1
- **Status:** Completed
- **Description:**
  - `include/entities/Mario.h`
  - `src/entities/Mario.cpp`
  - `include/level/Level.h`
  - `src/level/Level.cpp`
  - `include/states/PvpPlayState.h`
  - `src/states/PvpPlayState.cpp`
  - `tests/PvpArenaTests.cpp`
- **Detailed Logic Changes:**
  1. **Top Screen Ceiling Boundary Containment (`Mario.h/.cpp`, `Level.h/.cpp`)**:
     - Added `m_ceilingClampEnabled` and `setCeilingClampEnabled(bool)` to `Mario` so any entity can enable a top world ceiling clamp.
     - Updated `Mario::applyWorldBoundsClamp()` to clamp body position to the top ceiling (`bodyPos.y >= halfHeightMeters`) and cancel upward velocity (`vel.y = 0`) when `m_ceilingClampEnabled` is active.
     - Enabled ceiling clamp on both PvP fighters during `Level::loadPvpArena()`.
     - Implemented `Level::clampPvpFighters()` called on every update in `Level::updatePvp()` to enforce 4-way viewport containment (left, right, and top ceiling), ensuring fighters can never jump or get bounced off the top of the frame or leave the arena viewport.
  2. **Duel HUD Realignment & Screen-Fixed Multi-Column Layout (`PvpPlayState.h/.cpp`)**:
     - Replaced the single-line jammed HUD with a balanced 3-column layout anchored in fixed logical screen space (`target.getDefaultView()`):
       - **Left Column ($X = 40\text{px}$)**: `P1 <NAME>` header, `WINS: X` counter, and dedicated `FIRE X.Xs` badge below P1 status when active.
       - **Right Column ($X = 600\text{px}$)**: `<NAME> P2` header, `WINS: Y` counter, and dedicated `FIRE Y.Ys` badge below P2 status when active.
       - **Top Center ($X = 320\text{px}$)**: Match score badge (`X  -  Y`) and round indicator (`ROUND N` / `FINAL ROUND`).
       - **Center Screen Banners ($Y = 85\text{px}, 112\text{px}$)**: Repositioned intro/round/match banners to float above the center pedestal with clean clearance.
       - **Overhead Tags (`P1`/`P2`)**: Dynamically clamped above fighter heads so they never clip beyond the top of the camera viewport.
  3. **Automated Unit & Render Tests (`PvpArenaTests.cpp`)**:
     - Added `testPvpCeilingClampAndContainment` verifying that extreme upward impulses from the center pedestal cannot breach $Y < 0$.
     - Added `testPvpPlayStateHudRenderSnapshot` rendering and verifying pixel-perfect HUD presentation and dedicated Fire Flower timer badge snapshots.
     - Verified all 31/31 CTest suites pass with 100% success rate.

### 74. Canonical SMB1 Ledge Behavior - Walk-Off Patrollers vs Ledge-Aware Guards
- **Date:** 2026-08-22
- **Author:** D1 (Executor)
- **Status:** Completed
- **Description:**
  - `include/entities/Enemy.h`
  - `include/entities/RedKoopa.h`
  - `include/entities/HammerBro.h`
  - `src/entities/Goomba.cpp`
  - `src/entities/Koopa.cpp`
  - `src/entities/Spiny.cpp`
  - `src/entities/Paratroopa.cpp`
  - `tests/KoopaVariantTests.cpp`
  - `levels/level2.txt`
- **Detailed Logic Changes:**
  1. **New Ledge Policy Hook (`Enemy.h`)**:
     - Added `virtual bool turnsAtLedge() const { return false; }` to the `Enemy` base class. Canonical SMB1 ground walkers (Goomba, green Koopa, Spiny, wingless Paratroopa, Buzzy Beetle) do NOT reverse at ledges - they walk off and fall into pits.
  2. **Ledge Reversal Guards (Goomba.cpp, Koopa.cpp, Spiny.cpp, Paratroopa.cpp)**:
     - Guarded every patrol ledge-reversal site with `if (turnsAtLedge() && isApproachingLedge())`: Goomba patrol, Koopa patrol (shared by Buzzy Beetle and the wingless Paratroopa walk mode), Spiny patrol, and the Paratroopa HOP drift. Wall-reversal (`onWallCollision`), `isEscapingNarrowRange()` interplay, and the `isEnemySupport()` probing machinery are untouched.
  3. **Ledge-Aware Overrides (`RedKoopa.h`, `HammerBro.h`)**:
     - `RedKoopa::turnsAtLedge()` and `HammerBro::turnsAtLedge()` override to `true` - the canonical SMB1 ledge guards that stay on their platforms. HammerBro.cpp needed no change (its patrol already reverses unconditionally).
  4. **Test Contract Rewrite (`KoopaVariantTests.cpp`)**:
     - `testEnemySupportTileCoverageAndLedgeProbing` now asserts: Goomba / green Koopa / Spiny / wingless Paratroopa / Buzzy Beetle keep facing a ledge (walk off), Red Koopa and HammerBro reverse at the ledge, and `onWallCollision()` still turns every walker.
     - `testNarrowPatrolBreakoutAI` re-based on Red Koopa (the ledge-aware species) for the 2-tile pocket oscillation and wide-platform cases, plus a new canonical-walker section proving a Goomba on the same pocket walks straight off without triggering the breakout AI.
  5. **Level Data Audit (`levels/level1-4.txt`)**:
     - Audited every Goomba/Koopa/Spiny/Paratroopa/BuzzyBeetle spawn against pit columns. Single adjustment: `levels/level2.txt` line 42 col 195 changed `K` -> `k` (green Koopa -> Red Koopa): the Koopa guarding the 8-tile island between the two pits near the finish would otherwise walk into the left pit ~1.3s after activation. A Red Koopa is the canonical SMB1 island guard. All other spawns either drop onto solid ground below (canonical SMB1 behavior) or are far enough from pits to stay meaningful.

### 75. Repo-Wide Docs Phase - Junk Cleanup, Vietnamese-to-English Translation, Doc Fact Repairs, Asset Mapping
- **Date:** 2026-08-22
- **Author:** End-of-Session Worker (deployment codebase_audit_2026_08_22, Heavy route; consolidating the docs-phase work of the audit executors)
- **Status:** Completed
- **Description:**
  - Deleted (9 junk files): `Evaluate.md`, `docs/assets/reference/tileset(v2).png`, `docs/tiles_coordinate.md`, `docs/management/S6_DEPENDENCY_BOARD.md`, `docs/management/WEEKLY_PLAN.md`, `include/demo/TV3Demo.h`, `src/demo/TV3Demo.cpp`, `levels/level_athletic.txt`, `tests/TestSpawnDeath.cpp`
  - Created: `docs/assets/ASSET_MAPPING.md`
  - Doc fact repairs: `FILE_STRUCTURE.md`, `CODING_RULES.md` (line 287), `assets/ASSETS_LIST.md` (lines 144/154), `README.md`, `docs/management/ROLES.md`, `docs/management/S6_LOCKED_INTERFACES.md`, `docs/management/S6_AUDIT_TRACKER.md`, `docs/management/S6_BUG_REGISTER.md`, `docs/management/s6_plan.md`, `docs/management/S7_TV5_PACKAGE_MANIFEST.md`, `tests/Gate0ContractTests.cpp` (header comment only; the D6 test addition is logged in the D2-D7 checkpoint row above)
  - Translations: `docs/tileset_coordinate.md` (plus coordinate corrections), `docs/blocks_coordinate.md`, `docs/enemies_coordinate.md`, `docs/items_objects_coordinate.md`, `docs/design_patterns.md`, `docs/oop_principles_and_design_patterns.md`, `docs/class_diagram.md` (plus 2 malformed anchor fixes), `docs/management/S7_TV3_STATUS.md`, `docs/management/S7_TV2_VISUAL_SIGN_OFF.md`, `include/core/SpriteFrames_shared.h`, `include/core/LevelCatalog.h`, `docs/change_in_develop.md` (English-entry note)
  - Romanized `@author` headers: 83 source files across `include/` and `src/`
- **Detailed Logic Changes:**
  1. **Junk Cleanup (9 deletions, with deliberate keeps)**:
     - Removed dead/duplicate/stale artifacts: the superseded tileset reference image, the duplicate `docs/tiles_coordinate.md`, the orphaned athletic level, the never-registered `TestSpawnDeath.cpp`, the dead TV3 demo pair, the stale S6 dependency board and weekly plan, and the dead-root `Evaluate.md` (stale Sprint-6 grading snapshot; its `Evaluate_v4.md` successor never existed — the S6 tracker/register references to it were reworded).
     - Deliberately kept: `assets/textures/enemies/enemies_all_components_atlas_full.png` (test contract), `levels/level0.txt` (registered test fixture), `docs/management/s6_plan.md`/`s7_plan.md` and the S6/S7 status docs (audit trail).
  2. **Documentation Fact Repairs**:
     - `FILE_STRUCTURE.md`: phantom entries removed, missing entries added, every path re-verified against the tree. `S6_LOCKED_INTERFACES.md`: corrected to the 4-level catalog, co-op `GameProgress` fields, `defeatEnemy` parameter, and the camera-mode axis line. `ROLES.md` rewritten around real APIs. Dead `Evaluate_v4` references reworded in the S6 tracker/bug register. `S7_TV5_PACKAGE_MANIFEST.md`: `hud.png` reclassified as packaged-unused. `CODING_RULES.md`, `ASSETS_LIST.md`, `README.md`, `s6_plan.md`, and the `Gate0ContractTests.cpp` header comment corrected.
  3. **Vietnamese-to-English Translation (~2,238 lines)**:
     - Full translation of the four coordinate documents, both design-pattern documents, `class_diagram.md`, S7_TV3_STATUS, S7_TV2 sign-off, `SpriteFrames_shared.h` comments (10), `LevelCatalog.h`, plus 83 romanized `@author` headers. `tileset_coordinate.md` values corrected against the real sheet (680x356; Stone/Hard/Ground per `TileFrames.h`). Vietnamese intentionally retained only in: `change_in_develop.md` history (entries #1-#73), `s6_plan.md`, `s7_plan.md`.
  4. **New `docs/assets/ASSET_MAPPING.md`**:
     - Grep-verified file:line map of every asset to its loader (runtime / fixture / packaged-unused / future / reference). Key findings: `bg_world.png` and `hud.png` are packaged-unused legacy assets (candidates for future package-list removal).
  5. **Verification**:
     - Independent tester clean-rebuilt both build directories: build/ 37/37 and build-tests/ 37/37 CTest pass (logs `/tmp/ctest_main_final.log`, `/tmp/ctest_tests_final.log`); no-weakening audit of all changed test suites passed; regression sweep clean (sole binary diff = intentional junk deletion); Vietnamese grep: code directories empty, docs limited to the 3 protected files.

### 76. Demo Video Recording Script

- **Date:** 2026-08-22
- **Author:** TV1 (glm-workflow leaf task, main agent)
- **Status:** Completed
- **Description:**
  - Created: `docs/DEMO_VIDEO_SCRIPT.md`
  - Updated: `FILE_STRUCTURE.md` (docs tree now lists `DEMO_VIDEO_SCRIPT.md` and `docs/assets/ASSET_MAPPING.md`)
  - Updated: `docs/change_in_develop.md` (this entry; plus factual correction to entry #75: `Evaluate_v4.md` never existed)
- **Detailed Logic Changes:**
  1. Documentation only; no code or assets changed. Added a 5-minute demo video recording script aligned with the 20-scenario matrix in `docs/testing/TV4_PLAYTHROUGH_LOG.md`, so one recording session on one immutable RC commit yields both the required spec deliverable (demo video) and the human playthrough evidence.
  2. The script contains: pre-flight checklist (release build, RC commit citation, capture/audio setup, two-pass save plan), a chaptered main timeline with exact key actions and per-chapter rubric mapping (menu/character select, 1-1 overworld mechanics showcase including the new canonical ledge walk-off and flagpole tiers, 1-2 underground, 1-3 underwater, 1-4 castle with both Bowser kill routes, save/load persistence proof, 2-player co-op, 2-player versus, pause), an off-camera Part B list covering the remaining matrix rows (no-damage, death-respawn, low-flag, GameOver paths, pause stress, continuous run), a controls cheat sheet sourced from `src/states/PlayState.cpp` bindings, and editing notes (1280x720/60fps, keep game audio, cut only between chapters).
  3. Entry #75 correction: replaced the false claim that `Evaluate.md`'s live content is `docs/management/Evaluate_v4.md` — that file never existed (verified during the audit); `Evaluate.md` was a stale Sprint-6 grading snapshot with dead links.

### 77. Level 4 Castle Finale Boss AI 2.0 & Turtle Cannon Platforming Integration

- **Date:** 2026-08-23
- **Author:** TV1 (Duong), TV4 (Vy)
- **Status:** Completed; 38/38 CTest suites passed (100% pass rate).
- **Description:**
  - include/entities/Bowser.h
  - src/entities/Bowser.cpp
  - src/entities/BulletBillLauncher.cpp
  - levels/level4.txt
  - docs/change_in_develop.md
- **Detailed Logic Changes:**
  1. **Bowser Boss AI 2.0 - Adaptive Multi-Phase & Tactical Combat (Bowser.h, Bowser.cpp)**:
     - Added isEnraged() phase transition when Bowser's HP drops to <= 2 or when Mario moves behind Bowser (X_mario > X_bowser + 32px).
     - **Phase 1 (Tactical Pacing)**: Maintains distance, performs anti-air intercept hops (-HOP_SPEED * 1.2f) when Mario tries to vault over via elevator/springboard, and fires multi-height flames (BowserFire).
     - **Phase 2 (Enraged Berserk)**: Attack timer accelerates from 1.8s-3.0s down to 0.9s-1.6s. Bowser gains the ability to throw arcing hammer barrages (throwHammer()), turns rapidly to confront Mario, leaps with increased velocity (-HOP_SPEED * 1.35f), and displays an enraged fiery red pulsating visual aura.
  2. **Bullet Bill Launcher Theme Support & Accurate Muzzle Spawn (BulletBillLauncher.cpp)**:
     - Replaced hardcoded CANNON_PARTS with dynamic getCannonParts(LevelTheme theme) supporting all 4 authentic NES tileset palettes (Overworld, Underground, Castle, Underwater).
     - Fixed bullet spawn point to exit from the top muzzle barrel (Y = m_position.y) rather than the lower base.
  3. **Level 4 Castle Layout Enhancement (levels/level4.txt)**:
     - Strategically integrated Turtle Cannons ('D') into the Castle boss gauntlet: a ground-level cannon before the springboard (J) and an upper ceiling turret overlooking the elevator traverse (^ ~ ~).
     - Validated all map rows to maintain the strict 160-tile length contract.
  4. **Verification**:
     - Rebuilt full game binary and ran test suite: all 38/38 CTest suites passed 100% with 0 regressions.
### 78. Swivel Turret Cannon AI & Dynamic Angled Bullet Bill Integration

- **Date:** 2026-08-23
- **Author:** TV1 (Duong), TV4 (Vy)
- **Status:** Completed; 38/38 CTest suites passed (100% pass rate).
- **Description:**
  - include/entities/BulletBill.h
  - src/entities/BulletBill.cpp
  - include/entities/BulletBillLauncher.h
  - src/entities/BulletBillLauncher.cpp
  - docs/change_in_develop.md
- **Detailed Logic Changes:**
  1. **Swivel Turret Cannon AI (BulletBillLauncher.h, BulletBillLauncher.cpp)**:
     - Upgraded the Turtle Cannon into a dynamic 2D aiming turret that tracks Mario's live position relative to the cannon's top barrel center:
       $$\theta = \text{atan2}(y_{\text{mario}} - y_{\text{top}}, |x_{\text{mario}} - x_{\text{top}}|)$$
     - Clamped the dynamic vertical turret elevation to $\pm 25^\circ$, ensuring realistic retro turret rotation without clipping or escaping level boundaries.
     - Separated visual rendering: the base and skull body remain firmly planted upright on the ground while the top muzzle barrel rotates smoothly around its center pivot $\{8\text{px}, 8\text{px}\}$ oriented towards Mario.
     - Positioned the launch muzzle flash smoke puff at the rotated barrel opening.
  2. **Angled Vector Bullet Bill Projectile (BulletBill.h, BulletBill.cpp)**:
     - Extended BulletBill to accept an `angleDegrees` launch parameter, computing linear velocity along the launch vector: $v_x = \text{SPEED} \cdot \cos\theta$, $v_y = \text{SPEED} \cdot \sin\theta$.
     - Sprite rotation dynamically matches the flight angle ($\theta$) with its nose pointing towards the trajectory, maintaining full compatibility with Mario stomp interactions.
  3. **Verification**:
     - Full CTest suite executed: all 38/38 test suites passed 100% with 0 regressions.
### 79. Turtle Cannon Mechanical Swivel Collar Joint & Pivot Refinement

- **Date:** 2026-08-23
- **Author:** TV1 (Duong), TV4 (Vy)
- **Status:** Completed; 38/38 CTest suites passed (100% pass rate).
- **Description:**
  - src/entities/BulletBillLauncher.cpp
  - docs/change_in_develop.md
- **Detailed Logic Changes:**
  1. **Mechanical Swivel Collar Cushion Plate (BulletBillLauncher.cpp)**:
     - Added a dual-tone mechanical swivel collar (dark base plate + theme-matching metallic highlight ring) rendered directly at the joint intersection between the skull body and the top muzzle barrel.
     - Adjusted the top turret rotation pivot to the authentic hinge eyelet circle {8px, 12px} at the base of the top tile.
     - Eliminates visual gaps and awkward sharp-edge floating during elevation changes, resulting in a cohesive, solid, and seamless arcade turret look.
  2. **Verification**:
     - Verified with 38/38 CTest suites passing 100% with 0 regressions.
### 80. Bowser Combat Refinement: Pure Twin Fire Wave & Hammer Throw Removal

- **Date:** 2026-08-23
- **Author:** TV1 (Duong), TV4 (Vy)
- **Status:** Completed; 38/38 CTest suites passed (100% pass rate).
- **Description:**
  - include/entities/Bowser.h
  - src/entities/Bowser.cpp
  - docs/change_in_develop.md
- **Detailed Logic Changes:**
  1. **Pure Flame Combat & Twin Fire Wave (Bowser.h, Bowser.cpp)**:
     - Removed hammer throwing attacks (throwHammer()) and hammer-variant references from Bowser to prevent graphical asset clipping.
     - In Phase 1: Bowser executes calculated multi-tier flame breath (ground-skimming low flames, chest-level mid flames, head-level high flames) and anti-air hops.
     - In Phase 2 (Enraged): Bowser unleashes a devastating **Twin Fire Wave** (two concurrent fire waves at distinct elevation tiers, e.g. low + high) at accelerated intervals ($0.9\text{s} - 1.6\text{s}$).
  2. **Verification**:
     - Verified with full CTest suite: all 38/38 test suites passed 100% in 17.25s.
### 81. Bowser Boss AI: Seismic Ground Stomp Shockwave & Evade Jump Mechanics

- **Date:** 2026-08-23
- **Author:** TV1 (Duong), TV4 (Vy)
- **Status:** Completed; 38/38 CTest suites passed (100% pass rate).
- **Description:**
  - include/entities/Bowser.h
  - src/entities/Bowser.cpp
  - src/level/Level.cpp
  - docs/change_in_develop.md
- **Detailed Logic Changes:**
  1. **Seismic Ground Stomp Shockwave Attack (Bowser.h, Bowser.cpp)**:
     - Added airborne-to-ground landing detection (m_wasAirborne to grounded transition).
     - When Bowser lands from a jump, triggerGroundStomp() fires:
       - Plays ground impact rumble sound (SoundId::BUMP).
       - Triggers a $0.35\text{s}$ dynamic shockwave effect expanding across a $220\text{px}$ radius.
       - **Jump-To-Evade Platformer Mechanic**: Checks if Mario is within the shockwave radius ($\le 220\text{px}$) AND grounded (mario->isGrounded()). If Mario is standing on the ground, he takes shockwave damage (powerDown()). If Mario jumps in the air when Bowser lands, he successfully evades the attack unharmed.
  2. **Shockwave Visual Effects (Bowser::draw)**:
     - Renders dual expanding energy crests (bright yellow/white) and multi-layered dust smoke puffs radiating outward along the floor from Bowser's landing point.
  3. **Verification**:
     - Verified with full CTest suite: all 38/38 test suites passed 100% with 0 regressions.
### 82. Bowser Ground Stomp Shockwave Rebalancing & Mario Stun Status Integration

- **Date:** 2026-08-23
- **Author:** TV1 (Duong), TV3 (Bao), TV4 (Vy)
- **Status:** Completed; 38/38 CTest suites passed (100% pass rate).
- **Description:**
  - include/entities/Mario.h
  - src/entities/Mario.cpp
  - include/entities/Bowser.h
  - src/entities/Bowser.cpp
  - docs/change_in_develop.md
- **Detailed Logic Changes:**
  1. **Shockwave Nerf & Concentric Ripple Waves (Bowser.h, Bowser.cpp)**:
     - Reduced effective shockwave radius from $240\text{px}$ down to $140\text{px}$ ($\approx 4.5$ tiles).
     - Renders 3 concentric ripple shockwave crests expanding outward in successive phases with fading alpha and dust smoke particles.
     - Changed combat impact from lethal damage/powerdown to **Stun Status Effect**: Bowser's shockwave now inflicts a $1.2\text{s}$ stun (mario->stun(1.2f)) rather than stripping lives/power-ups.
  2. **Mario Stun Mechanism & Orbiting Dizzy Stars (Mario.h, Mario.cpp)**:
     - Added stun(float duration) and isStunned() to Mario: freezes horizontal player movement intent and disables jump triggers for the stun duration while maintaining full physics gravity/grounding.
     - Implemented Mario::draw override to render 3 rotating golden dizzy stars (★) orbiting in an elliptical trajectory around Mario's head while stunned.
  3. **Verification**:
     - Verified with full CTest suite: all 38/38 test suites passed 100% in 17.79s.
### 83. Bowser Boss: 2D Horizontal Ground Shockwave Wave Crests & Energy Beam VFX

- **Date:** 2026-08-23
- **Author:** TV1 (Duong), TV4 (Vy)
- **Status:** Completed; 38/38 CTest suites passed (100% pass rate).
- **Description:**
  - src/entities/Bowser.cpp
  - docs/change_in_develop.md
- **Detailed Logic Changes:**
  1. **2D Horizontal Ground Shockwave Presentation (Bowser::draw)**:
     - Upgraded the ground stomp VFX to an authentic 2D platformer horizontal wave rush:
       - Renders a ground-level glowing golden energy beam connecting from the impact epicenter outwards along the floor.
       - Generates 3 concentric triangular energy spikes/wave crests rushing flat along the ground line in both directions ($\pm$ spread).
       - Features an inner hot-white core energy spike for the leading wave and low-profile ground dust puffs billowing along the floor.
  2. **Verification**:
     - Verified with full CTest suite: all 38/38 test suites passed 100% in 18.46s.
### 84. Bowser Boss: Pure Procedural Concentric Propagating Ground Wave Shockwave VFX

- **Date:** 2026-08-23
- **Author:** TV1 (Duong), TV4 (Vy)
- **Status:** Completed; 38/38 CTest suites passed (100% pass rate).
- **Description:**
  - include/entities/Bowser.h
  - src/entities/Bowser.cpp
  - CMakeLists.txt
  - docs/change_in_develop.md
- **Detailed Logic Changes:**
  1. **Algorithmic Concentric Wave Propagation (Bowser::draw)**:
     - Developed a 100% procedural vector rendering algorithm for Bowser's ground stomp shockwave:
       - **Phase 1 (Impact Burst)**: Renders a 8-pointed golden star flash at the stomp contact point for the first 35% of the animation.
       - **Phase 2 (Ground Energy Rail)**: Renders a glowing golden ground-level energy beam stretching outward as the wave expands.
       - **Phase 3 (3 Successive Propagating Waves)**: 3 concentric wave crests propagate outward with staggered time delays ($0.00\text{s}$, $0.12\text{s}$, $0.16\text{s}$):
         - Triangular golden energy spikes oriented outward along the floor.
         - Hot-white interior core spikes for the leading wave.
         - Billowing low-ground dust puff particles trailing each wave crest with alpha fade.
  2. **Verification**:
     - Verified with full CTest suite: all 38/38 test suites passed 100% in 17.80s.
### 85. Bowser Boss: Exact Touch-down Landing Ground Stomp Trigger & Enhanced Floor Wave VFX

- **Date:** 2026-08-23
- **Author:** TV1 (Duong), TV4 (Vy)
- **Status:** Completed; 38/38 CTest suites passed (100% pass rate).
- **Description:**
  - include/entities/Bowser.h
  - src/entities/Bowser.cpp
  - docs/change_in_develop.md
- **Detailed Logic Changes:**
  1. **Strict Touch-down Landing Detection (Bowser.h, Bowser.cpp)**:
     - Added m_previousVy tracking to guarantee the ground stomp shockwave **only** triggers when Bowser actively launched into the air (vy < -80.f) and makes physical impact with the bridge floor (m_previousVy > 30.f && std::abs(vy) < 2.f).
     - Prevents accidental triggers during normal walking/patrolling on flat terrain.
  2. **Enhanced 2D Ground Wave Propagation Presentation (Bowser::draw)**:
     - Upgraded the procedural ground wave graphics:
       - Renders a multi-layer glowing energy rail on the bridge surface (outer golden rail + bright core).
       - Features sharp 2-tone energy wave crests (outer orange/gold spike + inner hot-white core blade).
       - Realistic smoke puffs billow upward and backward ($-4\text{px}$ to $-15\text{px}$) behind each surge wave with smooth alpha fading.
  3. **Verification**:
     - Verified with full CTest suite: all 38/38 test suites passed 100% in 17.44s.
### 86. Bullet Bill Artillery: Long-range Elevator Tracking & Multi-Tier Cannon Battery

- **Date:** 2026-08-23
- **Author:** TV1 (Duong), TV4 (Vy)
- **Status:** Completed; 38/38 CTest suites passed (100% pass rate).
- **Description:**
  - include/entities/BulletBillLauncher.h
  - src/entities/BulletBillLauncher.cpp
  - levels/level4.txt
  - docs/change_in_develop.md
- **Detailed Logic Changes:**
  1. **Long-Range Elevator AI Tracking (BulletBillLauncher.h, BulletBillLauncher.cpp)**:
     - Expanded horizontal activation reach (RANGE) from $500\text{px} \to 850\text{px}$ ($\sim 26.5$ tiles), allowing the cannon to detect and engage Mario across the entire elevator corridor and boss approach.
     - Tuned dynamic pitch elevation tracking (rawPitch clamped to $\pm 20^\circ$) to aim at Mario riding the elevator or vaulting across platforms while keeping bullet trajectories fully bounded within the level.
  2. **Multi-Tier Cannon Defense System in Level 4 (levels/level4.txt)**:
     - Added a second Turtle Cannon battery D at column 99 overlooking the elevator entrance, paired with the rampart cannon at column 143 to create a 2-tier crossfire fortress covering both the elevator route and the Bowser bridge.
  3. **Verification**:
     - Full CTest suite executed: all 38/38 test suites passed 100% in 17.74s.
### 87. Bullet Bill Lethality: Instant Death on Direct Hit

- **Date:** 2026-08-23
- **Author:** TV1 (Duong), TV3 (Bao)
- **Status:** Completed; 38/38 CTest suites passed (100% pass rate).
- **Description:**
  - src/physics/CollisionManager.cpp
  - docs/change_in_develop.md
- **Detailed Logic Changes:**
  1. **Instant Death on Bullet Bill Impact (CollisionManager::handleMarioCollision)**:
     - Updated Mario vs. Enemy lateral/direct collision handling:
       - If the colliding enemy is BULLET_BILL and Mario is not invincible (!mario->isInvincible() && !mario->isStarInvincible()), it directly invokes mario->loseLife() (instant death), bypassing normal tiered power-down degradation.
       - Stomping from above remains functional to defeat the Bullet Bill and bounce safely.
  2. **Verification**:
     - Full CTest suite executed: all 38/38 test suites passed 100% in 17.99s.
### 88. Bullet Bill: Absolute Omnidirectional Lethality

- **Date:** 2026-08-23
- **Author:** TV1 (Duong), TV3 (Bao)
- **Status:** Completed; 38/38 CTest suites passed (100% pass rate).
- **Description:**
  - src/physics/CollisionManager.cpp
  - docs/change_in_develop.md
- **Detailed Logic Changes:**
  1. **Omnidirectional Lethality on Bullet Bill Contact (CollisionManager::handleMarioCollision)**:
     - Updated Mario vs. Enemy contact resolution:
       - If the collided entity is BULLET_BILL and Mario does not have Starman invincibility (!mario->isStarInvincible()), ANY contact angle (stomping from above, lateral collision, running into it, or being hit from behind/below) immediately triggers mario->loseLife() (instant fatal death).
       - Only active Starman invincibility can destroy the Bullet Bill safely.
  2. **Verification**:
     - Full CTest suite executed: all 38/38 test suites passed 100% in 17.70s.
### 89. Bullet Bill: Realistic Front-Nose / Warhead Lethality vs. Stomp & Rear Contact

- **Date:** 2026-08-23
- **Author:** TV1 (Duong), TV3 (Bao)
- **Status:** Completed; 38/38 CTest suites passed (100% pass rate).
- **Description:**
  - src/physics/CollisionManager.cpp
  - docs/change_in_develop.md
- **Detailed Logic Changes:**
  1. **Directional Front-Nose Lethality (CollisionManager::handleMarioCollision)**:
     - Modeled realistic collision physics for Bullet Bill:
       - **Front Nose / Warhead Hit**: When colliding head-on into the front face of the flying bullet along its forward trajectory vector, it inflicts instant death via mario->loseLife().
       - **Top Stomp**: Landing on top of the bullet (isStomp) allows Mario to defeat the bullet, receive 200 points, and bounce off safely.
       - **Rear / Exhaust Brush**: Brushing against the rear end or tail of the bullet applies normal power-down degradation via mario->queuePowerDown().
       - **Starman Invincibility**: Touching the bullet while in Starman state instantly crushes the bullet safely.
  2. **Verification**:
     - Full CTest suite executed: all 38/38 test suites passed 100% in 17.27s.
### 90. Bowser Stomp Shockwave: Expanded Arena Stun Radius & Visual Range

- **Date:** 2026-08-23
- **Author:** TV1 (Duong), TV4 (Vy)
- **Status:** Completed; 38/38 CTest suites passed (100% pass rate).
- **Description:**
  - include/entities/Bowser.h
  - src/entities/Bowser.cpp
  - docs/change_in_develop.md
- **Detailed Logic Changes:**
  1. **Expanded Shockwave Radius (Bowser.h, Bowser.cpp)**:
     - Increased SHOCKWAVE_RADIUS from $140\text{px} \to 220\text{px}$ ($\sim 7$ full tiles), enabling Bowser's ground shockwave to cover nearly the entire bridge arena when landing in the central zone.
     - Adjusted shockwave ripple duration from $0.45\text{s} \to 0.50\text{s}$ to allow the visual horizontal concentric waves and trailing smoke dust to propagate smoothly across the wider radius.
  2. **Verification**:
     - Full CTest suite executed: all 38/38 test suites passed 100% in 16.22s.
### 91. Bullet Bill & Elevator Collision: Strict Airborne Descent Check for Stomps

- **Date:** 2026-08-23
- **Author:** TV1 (Duong), TV3 (Bao)
- **Status:** Completed; 38/38 CTest suites passed (100% pass rate).
- **Description:**
  - src/physics/CollisionManager.cpp
  - docs/change_in_develop.md
- **Detailed Logic Changes:**
  1. **Strict Stomp Guard for Bullet Bill (CollisionManager::handleMarioCollision)**:
     - Fixed an issue where Mario standing on an elevated surface (such as moving elevators in Level 4) was falsely recognized by the geometric bounding box check as performing a top stomp on oncoming Bullet Bills because Mario's feet were higher than the bullet's centerline.
     - Added strict guard: to stomp a Bullet Bill, Mario **must be airborne (!mario->isGrounded()) and descending downwards (marioVel.y >= -0.1f)**.
     - If Mario is grounded on an elevator platform or moving horizontally without falling, collisions with oncoming Bullet Bills are properly recognized as dangerous direct strikes (mario->loseLife() on front nose).
  2. **Verification**:
     - Full CTest suite executed: all 38/38 test suites passed 100% in 17.09s.
### 92. Level 4 Finale: Castle Toad Ending & Victory Trigger Robustness

- **Date:** 2026-08-23
- **Author:** TV1 (Duong), TV2 (Nhat)
- **Status:** Completed; 38/38 CTest suites passed (100% pass rate).
- **Description:**
  - src/level/Level.cpp
  - docs/change_in_develop.md
- **Detailed Logic Changes:**
  1. **Direct Toad Chamber Victory (Level::checkToadEnding)**:
     - Fixed an issue where reaching Toad (N) at the end of Level 4 did not trigger game completion if Mario bypassed Bowser without touching the Axe or defeated Bowser via fireballs.
     - Removed the rigid prerequisite lock m_castleExitReady from checkToadEnding(), enabling Toad's ending cutscene and dialogue (*"THANK YOU MARIO! BUT OUR PRINCESS IS IN ANOTHER CASTLE!"*) to trigger whenever Mario or Luigi reaches Toad's chamber.
  2. **Co-op Mode Castle Finale Support (Level::updateCoop)**:
     - Added updateToadDialogue(dt), updateBridgeCollapse(dt), and checkToadEnding() into updateCoop(dt) so that 2-player mode properly executes the Castle finale sequence.
     - Supported Bowser Axe bridge collapse triggering for both Player 1 and Player 2.
  3. **Verification**:
     - Full CTest suite executed: all 38/38 test suites passed 100% in 17.70s.
### 93. Castle Finale: Mandatory Bowser Defeat Gate & Artillery Ceasefire

- **Date:** 2026-08-23
- **Author:** TV1 (Duong), TV4 (Vy)
- **Status:** Completed; 38/38 CTest suites passed (100% pass rate).
- **Description:**
  - include/entities/BulletBillLauncher.h
  - src/entities/BulletBillLauncher.cpp
  - include/level/Level.h
  - src/level/Level.cpp
  - docs/change_in_develop.md
- **Detailed Logic Changes:**
  1. **Mandatory Bowser Defeat Before Toad Rescue (Level::checkToadEnding, Level::isBowserDefeated)**:
     - Added isBowserDefeated() and hasBowserInLevel() to query Bowser's status across all elimination methods (Axe bridge collapse, 5 Fireball impacts, or falling into lava).
     - In levels containing Bowser encounters (e.g. Level 4), reaching Toad will only trigger the victory dialogue and complete the game **after** Bowser has been defeated.
  2. **Artillery Ceasefire on Bowser Defeat (BulletBillLauncher, Level::updateEntities)**:
     - Added setCeaseFire(bool) state to BulletBillLauncher.
     - Whenever Bowser is defeated by any method, Level::updateEntities() automatically commands all Turtle Cannons / Bullet Bill Launchers to cease fire immediately (setCeaseFire(true)), silencing all artillery and ensuring a peaceful victory path to Toad.
  3. **Verification**:
     - Full CTest suite executed: all 38/38 test suites passed 100% in 16.70s.

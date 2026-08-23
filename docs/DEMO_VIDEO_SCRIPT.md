# Demo Video Recording Script

> Purpose: the course spec lists a **demo video** as a required deliverable.
> This script is aligned with the 20-scenario matrix in
> `docs/testing/TV4_PLAYTHROUGH_LOG.md` so one recording session produces both
> the video and most of the playthrough evidence. Record everything on ONE
> immutable RC commit and fill the per-scenario records with it.
> Suggested total length: 5:00–5:30.

## Pre-flight checklist (do this before recording)

- [ ] Build the Release preset and run the packaged exe once (smoke).
- [ ] Record the RC commit hash (e.g. `git log --oneline -1`) — the playthrough
      log requires every scenario row to cite it.
- [ ] Capture tool: OBS/ShadowPlay at 60 fps, capture the game window (logical
      resolution 640x360 — capture at 2x window size or upscale to 1280x720).
- [ ] Audio: capture BOTH game audio tracks; check SFX and music balance in a
      30-second test clip (jump, coin, stomp, music all audible).
- [ ] Saves plan: do TWO passes —
      1. **Fresh pass** (rename/delete `saves/save.txt`): full progression
         Level 1 -> 4 for the main timeline.
      2. **Existing-save pass** (after the fresh pass has unlocked levels):
         only Chapter 6 (save/load proof) needs this.
- [ ] Keep deaths in the edit for the death-respawn scenarios (Part B) — do not
      cut them.

## Main timeline (Part A — the video)

| Time | Chapter | Actions (exact keys) | What the viewer sees | Log IDs |
|---|---|---|---|---|
| 0:00–0:25 | Boot & Menu | Launch exe; idle on menu 3s (music); open Options, wiggle SFX + music volume sliders; character select — highlight Mario vs Luigi stat card (Luigi: higher jump, slower run); open Level Select | Menu state machine, options persistence, character system | — |
| 0:25–1:45 | World 1-1 Overworld | Walk (A/D), then run (hold Shift) with a skid stop; full jump (hold W) vs short hop (tap); stomp 2 Goombas in one chain (100 -> 200); pause 2s on a Goomba near a ledge — it walks off and falls (canonical ledge policy); bump ?-block (coin), then the mushroom block, collect Mushroom -> Super Mario; break a brick (Super); take FireFlower -> Fire Mario; shoot fireballs (X) at a Koopa — keep 2 fireballs on screen (max-2 rule); stomp Koopa -> kick shell (walk into it) -> shell clears another enemy; stand ON a Piranha pipe 2s (plant stays hidden), then step off and let it emerge; enter the bonus pipe (hold S on pipe); return via warp pipe; finish with a HIGH flagpole grab (top tier 5000) | Movement model, stomp chains, power-up state machine, fireball cap, shell mechanics, Piranha proximity AI, pipe warps, flagpole height scoring, level-complete music | L1-01, L1-04, L1-05(high) |
| 1:45–2:25 | World 1-2 Underground | Enter and let the theme/music switch show; ride an elevator; find the Red Koopa on the island — pause 2s to show it TURNING at the ledge (contrast with 1-1); fight the Hammer Bro (dodge 1-2 hammer bursts, aimed throws); take the Star -> rainbow tint -> run through 2-3 enemies; exit via flagpole | Theme switching, moving platforms, ledge-guard AI, boss-style aiming AI, star invincibility | L2-01, L2-04, L2-05(high) |
| 2:25–2:55 | World 1-3 Underwater | Enter water — show swim strokes (tap W) vs sinking; bump the ceiling once (clamped); let a Blooper home in and try (and fail) to stomp it; dodge swimming Cheep Cheeps; show bubbles + parallax briefly; exit via pipe/flag | Swim physics, distinct aquatic enemy AI, underwater theme | L3-01, L3-04 |
| 2:55–3:45 | World 1-4 Castle | Dodge 1-2 Firebar rotations; jump over a Podoboo leap; pass the Turtle Cannon — show its turret rotating to track Mario and firing angled Bullet Bills (front-nose hit = instant death; safe stomp only from the air); Bowser arena — survive fire breath (varied heights; twin fire waves once enraged) and JUMP over the ground-stomp shockwave rings (being caught shows the dizzy stun stars), then kill him with 5 fireballs (count them); note the Toad castle ending stays locked until Bowser is defeated; then grab the AXE -> bridge collapses -> Bowser falls -> Toad ending -> Win screen; let the Win music + credits play 5s | Boss fight, directional cannon ballistics, shockwave-stun mechanic, boss-gated ending, two canonical kill routes, Win state | L4-01, L4-04, L4-05 |
| 3:45–4:05 | Save/Load proof | From the Win/menu screen, quit the game completely; relaunch the exe; open Level Select | Previously unlocked levels still unlocked, high score and volumes persisted (file-based save/load requirement) | cross-level: "Restarting the executable preserves..." |
| 4:05–4:35 | 2-Player Co-op | Menu -> 2 PLAYER CO-OP; pick Mario + Luigi; play 30s — show both on screen, shared lives pool, camera containing both | Multiplayer requirement (co-op), character select | — |
| 4:35–5:00 | 2-Player Versus | Menu -> 2 PLAYER VERSUS; play one best-of-3 set: one stomp KO, one springboard super-bounce (hold jump), round transition, match winner screen | Multiplayer requirement (PvP), round FSM | — |
| 5:00–5:10 | Outro | Esc -> pause menu -> resume once; end on a title card (project name + team) | Pause state management | cross-level: pause/resume |

## Off-camera evidence (Part B — record after the main pass)

These fill the remaining playthrough-log rows. Record as short clips or simply
run them and fill the log (footage optional except where noted):

- **No-damage completions**: L1-02, L2-02, L3-02, L4-02 (deathless runs).
- **Death -> respawn -> complete**: L1-03, L2-03, L3-03, L4-03 (keep deaths in).
- **Low flagpole grabs**: L1-05/L2-05/L4-05 low variant (grab near base; show the
  lower tier score, e.g. 100/400).
- **Bowser shockwave close-up (L4)**: stand grounded inside the shockwave radius
  when Bowser lands to capture the stun stars, then a second take jumping over
  the expanding rings — one clip proves both the hazard and the counterplay.
- **GameOver path**: die until 0 lives -> GameOver screen -> Retry AND Menu
  paths (keyboard and mouse).
- **Pause stress**: pause/resume 20x in one level — no gameplay advance, no
  duplicated music.
- **Continuous run**: Level 1 -> 2 -> 3 -> 4 -> Win without quitting (score,
  coins, lives, power carry over; five music themes in order; death-with-lives
  respawns; one simultaneous death/flag condition if reproducible).

## Controls cheat sheet (from `src/states/PlayState.cpp`)

| Action | P1 (single/co-op) | P2 (co-op) |
|---|---|---|
| Move left / right | A / D (or arrows) | Left / Right |
| Jump | W, Up, or Space (tap = short hop, hold = full) | Up |
| Down / enter pipe | S / Down (hold) | Down |
| Run | LShift / RShift | (per on-screen hint) |
| Fireball (Fire state) | X | (per on-screen hint) |
| Pause | Esc | Esc |

## Editing notes

- Cut only BETWEEN chapters; keep each level continuous inside a chapter so the
  playthrough evidence stays valid.
- Add a 1-line caption at each chapter start naming the feature being shown
  (matches the "What the viewer sees" column) — graders map video to rubric lines.
- Do not mute or replace game audio (sounds are worth 10 rubric points).
- Export 1280x720 / 60 fps / H.264, name it `CS202_Group04_DemoVideo.mp4`,
  and reference it from `README.md` when published.

## After recording

1. Fill every per-scenario record in `docs/testing/TV4_PLAYTHROUGH_LOG.md`
   (commit, build preset, timestamp, duration, deaths, path, expected/actual,
   result) and update the matrix statuses.
2. Update `Overall result` and the cross-level run status.
3. Add the video link (or file path) to `README.md` and mark the demo-video
   deliverable done in the deliverables checklist.

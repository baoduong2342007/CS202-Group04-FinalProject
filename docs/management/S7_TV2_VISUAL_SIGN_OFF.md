# S7 TV2 - Visual Sign-off & Regression Matrix

This document serves as the visual evidence and regression matrix for TV2 tasks in Sprint 7.

**Reviewer:** Nhat (TV2)
**Date:** `20/08/2026`

---

## S7-TV2-01: Theme Render Audit
*Ensure Underwater and Castle themes render correctly with appropriate backgrounds, tiles, and entities.*

### Underwater Theme (Level 3)
* **Checklist:** Blue background, underwater tiles, Cheep Cheeps/Bloopers render correctly.
* **Evidence:**
  ![Pasted image](https://markdownviewer.pages.dev/api/image/PxAagY_xKgDChuoBbs5Kk1R-)
### Castle Theme (Level 4)
* **Checklist:** Dark background, castle brick tiles, Firebars/Bowser render correctly.
* **Evidence:**
  ![Pasted image](https://markdownviewer.pages.dev/api/image/BoA-dt0IF5Ll7yCVlEU1pd3l)
  ![Pasted image](https://markdownviewer.pages.dev/api/image/nVpo9gH793R9J1ezfZJ5Tow9)
---

## S7-TV2-02 & S7-TV2-03: Camera Viewport & Visual Regression Matrix
*Screenshots taken at the Start, Middle, and End of each level to verify camera bounds, Z-order (Mario vs. Background vs. HUD), and visual stability.*

### Level 1 (Overworld)
| Location | Screenshot | Notes (Z-order, Camera bounds, etc.) |
| :--- | :--- | :--- |
| **Start** | ![Pasted image](https://markdownviewer.pages.dev/api/image/Vimm0RZlW6Zww9z65qQmnLU5) | Camera stops at left bound correctly. |
| **Middle** | ![Pasted image](https://markdownviewer.pages.dev/api/image/vn6JBO0XMeJM3KGHG0BFOULa) | Mario renders in front of background. HUD is top-most. |
| **End (Flag)** | ![Pasted image](https://markdownviewer.pages.dev/api/image/Qbyxh2Mzcf6jSabTq9tRDJkJ) | Camera frames flag correctly without revealing out-of-bounds. |

### Level 2 (Underground)
| Location | Screenshot | Notes |
| :--- | :--- | :--- |
| **Start** | ![Pasted image](https://markdownviewer.pages.dev/api/image/Wu44W3exzPzAqMN25jNLamHm) | Wrong theme, should be overworld background and theme. |
| **Start (Post-Pipe)** | ![Pasted image](https://markdownviewer.pages.dev/api/image/0PXRXRZqjzb662W_v7TPKdSe) | Camera is out of bound on the left side. |
| **Middle** | ![Pasted image](https://markdownviewer.pages.dev/api/image/bmRT9sY0cYYgPvsHBurPB7TG) | |
| **End (Pipe/Exit)**| ![Pasted image](https://markdownviewer.pages.dev/api/image/vKFV8-mN7ef54JUJWg29gtpt) | Wrong theme, should be overworld background and theme.|

### Level 3 (Underwater)
| Location | Screenshot | Notes |
| :--- | :--- | :--- |
| **Start** | ![Pasted image](https://markdownviewer.pages.dev/api/image/jKx83mmPOOgBi-bf3yC17fSk) | |
| **Start (Post-Pipe)** | ![Pasted image](https://markdownviewer.pages.dev/api/image/1SEfAmnKXPGVBh6yK0JmIe9u) | Camera is out of bound on the left side and Mario can swim into out of bound zone. |
| **Middle** | ![Pasted image](https://markdownviewer.pages.dev/api/image/Od7SsOEwIw-gVsFdy-2NBg-G) | |
| **End (Flag)** | ![Pasted image](https://markdownviewer.pages.dev/api/image/1TsQOmdAx_ASxpi6keloDaR6) | |

### Level 4 (Castle)
| Location | Screenshot | Notes |
| :--- | :--- | :--- |
| **Start** | ![Pasted image](https://markdownviewer.pages.dev/api/image/2psQTwi9if57PKwCa-rcQ4jk) | |
| **Middle (Bridge)**| ![Pasted image](https://markdownviewer.pages.dev/api/image/nirE08dEr_QC3orHWXMGDH0A) | |
| **End (Toad/Win)**| ![Pasted image](https://markdownviewer.pages.dev/api/image/2eS2XQ20hFUHxuw7bsKkrZTp) | Toad's asset still has background|

---

## S7-TV2-04: UI States & Resolution Matrix
*Verifying Menu, Pause, Game Over, and Win screens render correctly and input targets focus properly across target resolutions.*

| UI State | Screenshot (Resolution 1 - e.g., 800x600) | Screenshot (Resolution 2 - e.g., 1080p fullscreen) | Notes (Input Target Focus) |
| :--- | :--- | :--- | :--- |
| **Main Menu** | ![Pasted image](https://markdownviewer.pages.dev/api/image/tO9fgtUX5glej01lXq7OvS8t) | ![Pasted image](https://markdownviewer.pages.dev/api/image/HplrZOiwuJxeWJ8g486W-n4Q) | Buttons highlight on hover correctly. |
| **Pause Screen** | ![Pasted image](https://markdownviewer.pages.dev/api/image/o37vJCBQ5RFcMXTOOd6c8KBH) | ![Pasted image](https://markdownviewer.pages.dev/api/image/-r4lN2rVzMEPyeuouQlz7r5j) | Overlay applies correctly. |
| **Game Over** | ![Pasted image](https://markdownviewer.pages.dev/api/image/dPSBPhh_S1e6-yfPZuOnpdFg) | ![Pasted image](https://markdownviewer.pages.dev/api/image/fzASjG-31AqjPmqIn9F9yqOy) | Buttons highlight on hover correctly.|
| **Win Screen** | ![Pasted image](https://markdownviewer.pages.dev/api/image/dntbVzCL-8p2YVczZzqzBpDo) | ![Pasted image](https://markdownviewer.pages.dev/api/image/HplrZOiwuJxeWJ8g486W-n4Q) | Buttons highlight on hover correctly.|

---

## S7-TV2-05: Final Sign-off Route
*Verification of level transitions, world labels ("World 1-1", etc.), and completion frames.*

* **Level 1 to 2 Transition (World Label):** Did not appear.
* **Level 2 to 3 Transition (World Label):** Did not appear.
* **Level 3 to 4 Transition (World Label):** Did not appear.
* **Level 4 Win Frame:**
  ![Pasted image](https://markdownviewer.pages.dev/api/image/HplrZOiwuJxeWJ8g486W-n4Q)

### Sign-off Checklist
- [ ] Theme rendering is correct for Underwater and Castle.
- [ ] Camera bounds strictly adhered to; no out-of-bounds rendering.
- [X] Z-order is correct (HUD > Mario > Foreground > Background).
- [X] UI input hitboxes align with visual buttons at all tested resolutions.
- [ ] All World Labels correctly display during transitions.


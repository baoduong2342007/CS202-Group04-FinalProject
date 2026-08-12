# Develop Integration Log

This file summarizes important integration checkpoints. Git history remains the authoritative record of individual commits and merges. Historical test counts are not current release evidence.

| Date | Checkpoint | Result | Notes |
|---|---|---|---|
| 2026-08-08 | Initial Sprint 6 integration | 7/7 CTest | Superseded by later implementation |
| 2026-08-09 | Evaluation v3 integration | 12/12 CTest | Superseded by later implementation |
| 2026-08-11 | `origin/develop` at `3047252` | 14/14 CTest | Review v4 reopened 40 tasks |
| 2026-08-12 | Sprint 6 remediation based on `3047252` | Clean Debug/Release/Tests PASS; CTest 17/17 | no-op asset build PASS; final commit/manual evidence pending |
| 2026-08-12 | BUG-042 body-tier FireFlower/transform fix | Debug/Release PASS; CTest 17/17 | Small/Super Fire forms retain body size; transformation frames update continuously |

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

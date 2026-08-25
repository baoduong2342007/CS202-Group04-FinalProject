# Third-Party Notices and Asset Attribution Gate

> Updated: 2026-08-21

This notice records the third-party license facts verified for the current
project dependencies and the unresolved provenance gate for packaged game
assets. It is not a grant of rights for any asset or redistribution bundle.

## Direct dependencies

| Dependency | Version / role | Verified notice |
|---|---|---|
| SFML | 3.0.0; graphics, window, system, and audio | SFML's official [license page](https://www.sfml-dev.org/license/) identifies the zlib license and the applicable notices for included components such as libpng. |
| Box2D | 2.4.1; physics | Official [Box2D v2.4.1 MIT license](https://github.com/erincatto/box2d/blob/v2.4.1/LICENSE). |

The redistributed SFML binaries and their transitive dependencies require a
separate binary/license audit before external distribution. This document does
not treat a dependency version, a build pass, or a link to a license as proof
that every redistributed binary has been audited.

## Packaged project assets: unresolved provenance

No repository evidence currently establishes the owner, source, license, or
redistribution permission for the packaged `mario.ttf`, WAV/FLAC recordings,
sprites, backgrounds, or other Nintendo-inspired textures. The usage and
dimensions in [assets/ASSETS_LIST.md](assets/ASSETS_LIST.md) are inventory facts,
not rights evidence. The tileset sprites were compared against the
[SMB1 Tileset on The Spriters Resource](https://www.spriters-resource.com/nes/supermariobros/asset/52571/?source=genre)
as an analysis reference; this does not grant permission to use or redistribute
the compared or derived assets.

Status: **BLOCKED**. Before external redistribution or final release sign-off,
record per-file provenance, source URL or owner, applicable license/permission,
and any required notices for every packaged asset. Do not infer rights from
educational use. Until that record exists, the runtime package may be verified
as a build artifact, but its redistribution/sign-off gate remains blocked.

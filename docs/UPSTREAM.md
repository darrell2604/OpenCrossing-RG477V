# Upstream references

This project is developed in `darrell2604/OpenCrossing-RG477V` only.

## Animal Crossing decompilation

Reference repository: https://github.com/ACreTeam/ac-decomp

Starting reference revision recorded for this project:

- Commit: `09ca8e8b5b24e6ab44047ee980cf0088ad7ecb4c`
- Date: 2026-07-18
- Version target: `GAFE01_00` (USA Rev 0)

The upstream project states that it contains no game assets and requires an existing game copy. Our repository will likewise contain no Nintendo game assets or disc data.

## Existing Anbernic native port

Reference repository: https://github.com/GabeConway/OpenCrossing-Anbernic

This is reference material only. It is not a dependency that we modify. Its current implementation demonstrates an ARM native port, OpenGL ES 3.2 rendering, controller handling, saves, settings and Dolphin-format HD texture packs. Its target hardware is the Allwinner H700 family, so the RG477V work here should be treated as a new ARM64/Android target rather than assuming binary compatibility.

## Attribution

When source or ideas are adapted from upstream projects, preserve their applicable copyright notices and licences in the relevant files and documentation.

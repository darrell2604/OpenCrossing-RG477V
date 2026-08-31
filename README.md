# OpenCrossing-RG477V

Native Animal Crossing (GameCube) ARM64/Android port targeting the Anbernic RG477V.

## Project rules

- This repository is the only repository we modify for this project.
- Upstream repositories are reference sources only; never push changes to them.
- Nintendo game assets are not included. The runtime will require the user's legally obtained game data.
- The goal is a native recompilation route, not an emulator-based Android port.

## Initial goals

1. Establish a clean project structure and build documentation.
2. Port the native runtime from the existing 32-bit assumptions to ARM64 where required.
3. Add an Android NDK platform layer.
4. Bring up OpenGL ES rendering on Android.
5. Map the RG477V physical controls to the GameCube controller layout.
6. Preserve/add Dolphin-compatible HD texture replacement support.
7. Add RG477V-specific performance and frame-pacing options.

## Development approach

Work incrementally. Each milestone should build independently where practical, with changes kept isolated and documented. No upstream repository will be modified.

## Current status

Repository scaffolding created. Native source integration and ARM64 audit are the next development steps.

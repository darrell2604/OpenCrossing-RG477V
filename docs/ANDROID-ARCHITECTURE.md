# Android / ARM64 Architecture

## Goal

Build a native ARM64 Android port of the Animal Crossing GameCube decompilation for the Anbernic RG477V. This repository is the only writable project repository; upstream repositories are reference-only.

## Current direction

1. Keep the decompiled game code and platform code clearly separated.
2. Introduce a 64-bit-safe platform boundary rather than attempting a blind whole-tree pointer conversion.
3. Use the Android NDK and OpenGL ES 3.x for the first renderer target.
4. Keep controller input behind an abstraction so the RG477V mapping can be remapped without changing game code.
5. Preserve the existing Dolphin-format HD texture replacement concept and make it a first-class platform service.

## 64-bit migration strategy

The current PC/native port contains explicit 32-bit assumptions, including pointer-to-u32 casts. We will identify these uses first and isolate them behind platform types or address helpers where required. GameCube emulated-address values must not be casually converted to native host pointers.

The intended model is:

- Game/engine addresses: fixed-width emulated-address types.
- Host pointers: `uintptr_t` / native pointer types.
- Android ABI: `arm64-v8a`.
- Graphics handles: host API types, never GameCube address types.

## Rendering boundary

Android owns EGL/GL context creation and the presentation surface. The native GX translation layer owns GameCube-style render state, texture upload, vertex submission and draw dispatch. The first milestone is a deterministic clear/present loop; the second is importing the existing GX translation code.

## Input boundary

Android event/input code feeds a neutral controller state:

- digital buttons
- D-pad
- left stick X/Y
- right stick X/Y
- left trigger
- right trigger

The RG477V profile will map those controls to the GameCube layout without requiring changes to the decompiled game logic.

## Texture boundary

Texture replacement remains external to the binary. The runtime will eventually resolve a texture identity to either an original disc asset or a user-provided replacement. The initial implementation should remain compatible with the existing Dolphin-format replacement workflow where practical.

## Build principle

Every meaningful porting step should compile independently. Avoid creating a large speculative patch. Each phase should leave the Android project in a buildable state and record known limitations in `docs/ROADMAP.md`.

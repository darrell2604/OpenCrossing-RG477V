# Upstream runtime entry-point audit

This report describes the first integration boundary for the Android/ARM64 port.

## Reference target

The CI workflow fetches `ACreTeam/ac-decomp` into a temporary build workspace and currently records revision `09ca8e8b5b24e6ab44047ee980cf0088ad7ecb4c`.

The target metadata lives under `config/GAFE01_00/`. The upstream repository provides `splits.txt`, `symbols.txt`, `build.sha1`, and `configure.py`; these remain reference inputs and are not copied into this repository.

## Initial runtime boundary

The current Android native layer owns:

- JNI lifecycle (`nativeSurfaceCreated`, `nativeSurfaceChanged`, `nativeDrawFrame`)
- OpenGL ES presentation
- Android/ARM64 build configuration
- RG477V controller state

The decomp integration boundary should therefore begin at the game/application loop rather than by importing the complete GameCube platform layer.

## First source family to investigate

Start with these upstream areas when building the dependency graph:

1. `src/main.c` — executable entry / top-level startup boundary.
2. `src/game/` — game systems and frame/update logic.
3. `src/actor/` — world actors and NPC systems once the core loop exists.
4. `src/static/dolphin/` — platform/runtime support that must be replaced or wrapped rather than compiled unchanged for Android.

## Porting rule

Do not make 32-bit pointer fixes globally. Distinguish between:

- genuine pointer/address arithmetic that must become pointer-width safe on ARM64;
- GameCube data structures whose `u32` fields are intentionally 32-bit;
- hardware/GX/OS interfaces that require an Android replacement;
- code that can be compiled unchanged.

## Next implementation milestone

Build an explicit `oc::PlatformServices` boundary for timing, input, memory allocation, and graphics submission. Then connect one small game-loop subsystem from the decompilation through that boundary.

This keeps the native runtime testable while the larger GameCube platform dependencies are progressively isolated.

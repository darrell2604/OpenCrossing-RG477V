# OpenCrossing-RG477V Roadmap

## Phase 0 — Project foundation
- [x] Create dedicated project repository
- [x] Document repository safety boundary
- [ ] Add build/dependency documentation

## Phase 1 — Native source integration
- [ ] Identify the exact upstream revision used as the starting point
- [ ] Import only the source needed by this project, preserving upstream attribution/licensing
- [ ] Audit 32-bit pointer assumptions and `u32` pointer casts
- [ ] Identify platform-specific filesystem, timing, audio and graphics dependencies
- [ ] Establish an ARM64-native build target

## Phase 2 — Android platform layer
- [ ] Android Gradle/NDK project
- [ ] ARM64 (`arm64-v8a`) native library
- [ ] Application lifecycle integration
- [ ] Storage/configuration handling
- [ ] Logging and crash diagnostics

## Phase 3 — Graphics
- [ ] Android OpenGL ES context
- [ ] Renderer bring-up
- [ ] Shader/program loading
- [ ] EFB/render-target behaviour
- [ ] Texture upload and caching
- [ ] Render-scale options

## Phase 4 — RG477V input
- [ ] Detect physical gamepad
- [ ] Map GameCube controls
- [ ] Map analogue sticks and triggers
- [ ] D-pad handling
- [ ] Configurable bindings

## Phase 5 — Texture replacement
- [ ] Preserve Dolphin-compatible texture-pack conventions where practical
- [ ] Texture lookup/replacement layer
- [ ] Cache management
- [ ] User-selectable texture packs

## Phase 6 — Performance
- [ ] Frame pacing
- [ ] 30/60 FPS modes where stable
- [ ] CPU/GPU profiling
- [ ] Memory optimisation
- [ ] Battery/thermal considerations

## Phase 7 — Packaging
- [ ] User-friendly APK build
- [ ] First-run game-data setup
- [ ] Configuration UI
- [ ] Release documentation

## Safety boundary

Only this repository is modified for the project. Upstream repositories are read-only reference material. No proprietary Nintendo game assets are committed to this repository.

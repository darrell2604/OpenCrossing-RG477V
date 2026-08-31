# RG477V Input

The RG477V is expected to present its integrated controls to Android using a conventional gamepad/Xbox-style mapping. The port therefore treats Android's standard gamepad key/axis events as the hardware-facing layer and translates them into the neutral controller state used by the native runtime.

This is intentionally a profile rather than hard-coded game logic. If a particular RG477V firmware reports a different axis or key code, only the profile/event adapter should need adjustment.

## Default intent

- A/B/X/Y -> GameCube face buttons
- Start -> GameCube Start
- Select -> auxiliary/debug/menu action
- L1/R1 -> shoulder buttons
- L2/R2 -> analogue triggers
- Left stick -> primary movement
- Right stick -> camera/secondary input
- D-pad -> D-pad

The runtime must retain a remapping path because Android device descriptors and firmware can vary.

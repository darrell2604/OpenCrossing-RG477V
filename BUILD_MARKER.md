Fresh build marker for debugging stale Actions runs.

Current known-good contract:
- RenderFrame contains player/camera fields and WorldGeometry pointer.
- GameRuntime does not reference world_vertices.
- native_bridge delegates frame presentation to GameRuntime/RendererPipeline.

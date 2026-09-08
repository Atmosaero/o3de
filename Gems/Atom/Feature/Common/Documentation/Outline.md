# Outline

Outline draws stylized screen-space lines at depth discontinuities. Add a **PostFX Layer**
and an **Outline** component to an entity, then select **Enable Outline**. The component
is in **Graphics / PostFX**. It is disabled by default.

| Setting | Default | Meaning |
| --- | --- | --- |
| Thickness | 1 | Sampling radius in output pixels, from 1 to 8. Fractional values blend line coverage between adjacent radii. |
| Depth Threshold | 0.02 | Relative depth curvature needed to draw a line. Lower values reveal smaller discontinuities. |
| Opacity | 1 | Line opacity, from 0 to 1. Zero bypasses the pass. |
| Color | Black | Linear RGB line color, before subsequent color grading and display mapping. |

Each setting supports the usual PostFX layer override. `OutlineRequestBus` exposes
getters and setters for settings and overrides, including `SetEnabled`, `SetThickness`,
`SetDepthThreshold`, `SetOpacity`, and `SetColor` (an `AZ::Vector3`).

The pass reads resolved reverse-Z depth and scene color. Opposite samples in four
directions detect depth curvature while suppressing planar surfaces. Lines are drawn
on the nearer side of depth breaks, keeping silhouettes from expanding into the
background. Diagonal sampling distances are normalized, and a soft threshold controls
line coverage. Thickness stays in screen pixels; very small objects can still be
covered by their outlines. The pass runs before Panini projection so depth and color use
the same screen coordinates; film grain, white balance, and vignette follow it.
When disabled, its input color is forwarded through a fallback connection.

This implementation is connected to the standard `PostProcessParentTemplate`.
The separate mobile post-process template and custom pipelines must connect the pass
explicitly to use it. It does not detect texture edges, normal-map details, or objects
that do not write depth. Only the positive reverse-depth curvature contributes, so
concave creases on continuous surfaces may be omitted. The pass has no temporal
history; camera motion can cause flickering, and depth of field can soften scene
color independently of the outlines.

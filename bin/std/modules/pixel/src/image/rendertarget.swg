#global public

struct RenderTarget
{
    handle:         RenderTargetHandle
    colorBuffer:    Texture
    depthStencil:   RenderTargetSurfaceHandle
    paintAlpha:     bool
    tris:           Core.Array'{s32, s32}
}
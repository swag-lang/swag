#global public

struct RenderTarget
{
    handle:            RenderTargetHandle
    colorBuffer:       Texture
    depthStencil:      RenderTargetSurfaceHandle

    paintAlpha:        bool
    paintAlphaIdx:     u64
}

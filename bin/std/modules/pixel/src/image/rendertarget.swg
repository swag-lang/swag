#global public

struct RenderTarget
{
    handle:         RenderTargetHandle
    colorBuffer:    Texture
    depthStencil:   RenderTargetSurfaceHandle
    paintAlpha:     bool

    alphaPassTris:  Core.Array'{s32, s32}
    alphaPassModel: Core.Array'Core.Math.Matrix4x4
}
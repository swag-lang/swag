using Core, Pixel

public struct PaintContext
{
    painter:    *Painter
    renderer:   *RenderOgl
    clipRect:   Math.Rectangle
    isDisabled: bool
}
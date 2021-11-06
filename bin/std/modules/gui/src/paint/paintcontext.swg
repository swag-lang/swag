using Core, Pixel

public struct PaintContext
{
    theme:      *Theme
    painter:    *Painter
    renderer:   *RenderOgl
    clipRect:   Math.Rectangle
    isDisabled: bool
}
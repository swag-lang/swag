using Core, Pixel

public struct PaintContext
{
    theme:      *Theme
    tweak:      *Tweak
    painter:    *Painter
    renderer:   *RenderOgl
    clipRect:   Math.Rectangle
    isDisabled: bool
}
using Core, Pixel

public struct PaintContext
{
    theme:      *Theme
    palette:    *Palette
    painter:    *Painter
    renderer:   *RenderOgl
    clipRect:   Math.Rectangle
    isDisabled: bool
}
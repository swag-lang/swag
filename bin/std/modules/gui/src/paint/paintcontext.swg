using Pixel

public struct PaintContext
{
    theme:      *Theme
    painter:    *Painter
    renderer:   *RenderOgl
    isDisabled: bool
}
using Core, Pixel

public struct PaintContext
{
    theme:      *Theme
    colors:     *ThemeColors
    metrics:    *ThemeMetrics
    painter:    *Painter
    renderer:   *RenderOgl
    clipRect:   Math.Rectangle
    isDisabled: bool
}
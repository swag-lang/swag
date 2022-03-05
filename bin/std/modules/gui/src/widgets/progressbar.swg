#global public
using Core, Pixel

#[Swag.EnumFlags]
enum ProgressBarFlags
{
    None
    Infinite
}

struct ProgressBar
{
    using wnd:      Wnd

    prgBarFlags     = ProgressBarFlags.None
    progression:    f32
}

impl IWnd for ProgressBar
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc     := evt.bc
        theme   := getTheme()
        metrics := getThemeMetrics()
        colors  := getThemeColors()
        rects   := getThemeRects()

        var subRectBk:      *ThemeImageRect
        var subRectBorder:  *ThemeImageRect
        var subRectMark:    *ThemeImageRect
        var colorBk:        Color = ?
        var colorBorder:    Color = ?
        var colorMark:      Color = ?

        switch
        {
        case bc.isDisabled:
            colorBk       = colors.progressBar_DisabledBk
            colorBorder   = colors.progressBar_DisabledBorder
            colorMark     = colors.progressBar_DisabledMark
        default:
            colorBk       = colors.progressBar_Bk
            colorBorder   = colors.progressBar_Border
            colorMark     = colors.progressBar_Mark
        }

        subRectBk     = &rects.progressBar_Bk
        subRectBorder = &rects.progressBar_Border
        subRectMark   = &rects.progressBar_Mark

        // Bk
        clientRect := getClientRect()
        theme.drawSubRect(bc.painter, clientRect, subRectBk, colorBk)

        bc.painter.startClippingRegion()
        clip := clientRect
        clip.inflate(-metrics.progressBar_ClipOffset)
        theme.drawSubRect(bc.painter, clip, subRectBk, Argb.White)
        bc.painter.endClippingRegion()

        // Bar
        posBox := clientRect
        posBox.y = clientRect.vertCenter() - (posBox.height * 0.5)
        if prgBarFlags & .Infinite
        {
            w := Math.max(clientRect.width / 5, 1)
            posBox.width = w

            wt := clientRect.width + 2 * w
            posBox.x = -w + (progression * wt)

            // Automatic update of progresssion
            progression += surface.app.timing.dt * metrics.progressBar_SpeedInf
            if progression > 1 progression = 0
            invalidate()
        }
        else
        {
            posBox.width = clientRect.width * progression
        }

        theme.drawSubRect(bc.painter, posBox, subRectMark, colorMark)

        // Text
        bc.painter.resetClippingRegion()
        if !(prgBarFlags & .Infinite)
        {
        }

        // Border
        theme.drawSubRect(bc.painter, clientRect, subRectBorder, colorBorder)
    }
}

impl ProgressBar
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle, id: string = null)->*ProgressBar
    {
        Debug.assert(parent != null)
        pos := position
        if pos.height == 0
            pos.height = parent.getThemeMetrics().progressBar_Height
        return Wnd.create'ProgressBar(parent, null, pos, id)
    }

    // Set the progression in [0..1] range
    mtd setProgression(prg: f32)
    {
        newPrg := Math.saturate(prg)
        if newPrg == progression
            return
        progression = newPrg
        invalidate()
    }
}
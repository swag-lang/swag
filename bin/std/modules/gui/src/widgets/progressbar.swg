#global public
using Core, Pixel

#[Swag.EnumFlags]
enum ProgressBarFlags
{
    Zero     = 0
    Infinite
}

struct ProgressBar
{
    using wnd:       Wnd

    prgBarFlags      = ProgressBarFlags.Zero
    progression:     f32
}

impl IWnd for ProgressBar
{
    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc      = evt.bc
        let painter = bc.painter
        let theme   = .getTheme()
        let metrics = .getThemeMetrics()
        let colors  = .getThemeColors()
        let rects   = .getThemeRects()

        var colorBk:     Color = undefined
        var colorBorder: Color = undefined
        var colorMark:   Color = undefined

        switch
        {
        case bc.isDisabled:
            colorBk     = colors.progressBar_DisabledBk
            colorBorder = colors.progressBar_DisabledBorder
            colorMark   = colors.progressBar_DisabledMark
        default:
            colorBk     = colors.progressBar_Bk
            colorBorder = colors.progressBar_Border
            colorMark   = colors.progressBar_Mark
        }

        let subRectBk     = &rects.progressBar_Bk
        let subRectBorder = &rects.progressBar_Border
        let subRectMark   = &rects.progressBar_Mark

        // Bk
        let clientRect = .getClientRect()
        theme.drawSubRect(painter, clientRect, subRectBk, colorBk)

        painter.startClippingRegion()
        var clip = clientRect
        clip.inflate(-metrics.progressBar_ClipOffset)
        theme.drawSubRect(painter, clip, subRectBk, Argb.White)
        painter.endClippingRegion()

        // Bar
        var posBox = clientRect
        posBox.y = clientRect.vertCenter() - (posBox.height * 0.5)
        if .prgBarFlags.has(.Infinite)
        {
            let w = Math.max(clientRect.width / 5, 1)
            posBox.width = w

            let wt = clientRect.width + 2 * w
            posBox.x = -w + (.progression * wt)

            // Automatic update of progresssion
            .progression += .getTiming().dt * metrics.progressBar_SpeedInf
            if .progression > 1 do
                .progression = 0
            .invalidate()
        }
        else
        {
            posBox.width = clientRect.width * .progression
        }

        theme.drawSubRect(painter, posBox, subRectMark, colorMark)
        painter.resetClippingRegion()

        // Border
        theme.drawSubRect(painter, clientRect, subRectBorder, colorBorder)
    }
}

impl ProgressBar
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null)->*ProgressBar
    {
        Debug.assert(parent != null)
        var pos = position
        if pos.height == 0 do
            pos.height = parent.getThemeMetrics().progressBar_Height
        return Wnd.create'ProgressBar(parent, null, pos, id)
    }

    // Set the progression in [0..1] range
    mtd setProgression(prg: f32)
    {
        let newPrg = Math.saturate(prg)
        if newPrg == .progression do
            return
        .progression = newPrg
        .invalidate()
    }
}

#global public
using Core, Pixel

enum FrameWndForm
{
    Square
    Round
    Flat
    Transparent
}

struct FrameWnd
{
    using wnd:      Wnd
    view:           *Wnd
    frameForm       = FrameWndForm.Transparent
    focusBorder     = true
}

impl IWnd for FrameWnd
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        if frameForm == .Transparent
            return

        bc      := evt.bc
        painter := bc.painter
        theme   := getTheme()
        colors  := getThemeColors()
        rects   := getThemeRects()

        var subRectBk:      *ThemeImageRect
        var subRectBorder:  *ThemeImageRect
        var colorBk:        Color = ?
        var colorBorder:    Color = ?

        switch frameForm
        {
        case Square:
            subRectBk = &rects.frame_SquareBk
            subRectBorder = &rects.frame_SquareBorder
        case Round:
            subRectBk = &rects.frame_RoundBk
            subRectBorder = &rects.frame_RoundBorder
        case Flat:
            subRectBk = &rects.frame_FlatBk
        }

        // Display focus borders when the focus wnd is a child of me
        showFocus := focusBorder
        if showFocus
        {
            focus := getFocus()
            if !focus or !isParentOf(focus)
                showFocus = false
        }

        switch
        {
        case bc.isDisabled:
            colorBorder = colors.frameWnd_DisabledBorder
            colorBk = colors.frameWnd_DisabledBk
        case showFocus:
            colorBorder = colors.frameWnd_FocusBorder
            colorBk = colors.frameWnd_FocusBk
        default:
            colorBorder = colors.frameWnd_Border
            colorBk = colors.frameWnd_Bk
        }

        clientRect := getClientRect()
        if evt.kind == .BeforeChilds
            theme.drawSubRect(painter, clientRect, subRectBk, colorBk)
        if frameForm != .Flat and evt.kind == .PostChilds
            theme.drawSubRect(painter, clientRect, subRectBorder, colorBorder)
    }

    mtd onResizeEvent(evt: *ResizeEvent)
    {
        if !view
            return
        view.move(padding.x, padding.y)
        view.resize(position.width - padding.x - padding.z, position.height - padding.y - padding.w)
    }
}

impl FrameWnd
{
    #[Swag.Discardable]
    mtd(T) createView()->*T
    {
        view = Wnd.create'T(self, "", position)
        view.backgroundStyle = .Window
        sendResizeEvent()
        return cast(*T) view
    }

    // Set the editbox form
    mtd setFrameForm(form: FrameWndForm)
    {
        if self.frameForm == #self.form
            return

        self.frameForm = #self.form
        hasPostChildsPaint = false

        #[Swag.Complete]
        switch self.frameForm
        {
        case Square:
            hasPostChildsPaint = true
            padding = getThemeMetrics().frameWnd_SquarePadding
        case Round:
            hasPostChildsPaint = true
            padding = getThemeMetrics().frameWnd_RoundPadding
        case Flat:
            padding = getThemeMetrics().frameWnd_FlatPadding
        case Transparent:
            padding = @{0, 0, 0, 0}
        }

        sendResizeEvent()
    }
}
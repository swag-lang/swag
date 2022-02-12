#global public
using Core, Pixel

enum FrameWndForm
{
    Square
    Round
    Transparent
}

#[Swag.EnumFlags]
enum FrameWndFlags
{
    None
    Borders
    FocusBorder
}

struct FrameWnd
{
    using wnd: Wnd

    view:       *Wnd
    frameForm   = FrameWndForm.Transparent
    frameFlags  = FrameWndFlags.Borders
}

impl IWnd for FrameWnd
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        if frameForm == .Transparent
            return
        if !(frameFlags & .Borders) and evt.kind == .PostChilds
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
        }

        // Display focus borders when the focus wnd is a child of me
        showFocus := cast(bool) (frameFlags & .FocusBorder)
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
        elif evt.kind == .PostChilds
            theme.drawSubRect(painter, clientRect, subRectBorder, colorBorder, false)
    }

    mtd onResizeEvent(evt: *ResizeEvent)
    {
        if !view
            return
        rect := getClientRectPadding()
        rect.applyPadding(view.margin)
        view.setPosition(rect)
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
        if frameForm == #self.form
            return
        frameForm = #self.form

        #[Swag.Complete]
        switch self.frameForm
        {
        case Square:
            wndFlags |= .PostChildsPaint
            setAllPaddings(getThemeMetrics().frameWnd_SquarePadding)
        case Round:
            wndFlags |= .PostChildsPaint
            setAllPaddings(getThemeMetrics().frameWnd_RoundPadding)
        case Transparent:
            wndFlags &= ~.PostChildsPaint
            setAllPaddings(0)
        }

        sendResizeEvent()
    }
}
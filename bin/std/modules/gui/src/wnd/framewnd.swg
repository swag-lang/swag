#global public
using Core, Pixel

enum FrameWndForm
{
    Transparent
    Square
    Round
}

#[Swag.EnumFlags]
enum FrameWndFlags
{
    None
    Borders
    FocusBorder
    SmallShadow
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
    mtd onPrePaintEvent(evt: *PaintEvent)
    {
        if frameFlags & .SmallShadow
        {
            bc      := evt.bc
            painter := bc.painter
            theme   := getTheme()
            colors  := getThemeColors()
            rects   := getThemeRects()
            metrics := getThemeMetrics()

            clientRect := getClientRect()
            clientRect.applyPadding(metrics.frameWnd_SmallShadowSize)
            subRect := &theme.rects.frameWnd_SmallShadow
            theme.drawSubRect(painter, clientRect, subRect, colors.frameWnd_SmallShadow)
        }
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        if frameForm == .Transparent
            return

        bc      := evt.bc
        painter := bc.painter
        theme   := getTheme()
        colors  := getThemeColors()
        rects   := getThemeRects()
        metrics := getThemeMetrics()

        var subRectBk:  *ThemeImageRect
        var colorBk:    Color = ?

        switch frameForm
        {
        case Square:
            subRectBk = &rects.frameWnd_SquareBk
        case Round:
            subRectBk = &rects.frameWnd_RoundBk
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
            colorBk = colors.frameWnd_DisabledBk
        case showFocus:
            colorBk = colors.frameWnd_FocusBk
        default:
            colorBk = colors.frameWnd_Bk
        }

        clientRect := getClientRect()
        theme.drawSubRect(painter, clientRect, subRectBk, colorBk)
    }

    mtd onPostPaintEvent(evt: *PaintEvent)
    {
        if frameForm == .Transparent
            return
        if !(frameFlags & .Borders)
            return

        bc      := evt.bc
        painter := bc.painter
        theme   := getTheme()
        colors  := getThemeColors()
        rects   := getThemeRects()
        metrics := getThemeMetrics()

        var subRectBorder:  *ThemeImageRect
        var colorBorder:    Color = ?

        switch frameForm
        {
        case Square:
            subRectBorder = &rects.frameWnd_SquareBorder
        case Round:
            subRectBorder = &rects.frameWnd_RoundBorder
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
        case showFocus:
            colorBorder = colors.frameWnd_FocusBorder
        default:
            colorBorder = colors.frameWnd_Border
        }

        clientRect := getClientRect()
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
    mtd(T) createView(hook: HookEvent = null)->*T
    {
        view = Wnd.create'T(self, "", position, hook: hook)
        view.backgroundStyle = .Window
        sendResizeEvent()
        return cast(*T) view
    }

    // Set the frame wnd flags
    mtd setFrameFlags(add, remove = FrameWndFlags.None)
    {
        frameFlags |= add
        frameFlags &= ~remove

        if frameFlags & .SmallShadow
            wndFlags |= .PreChildsPaint
        else
            wndFlags &= ~.PreChildsPaint

        if frameFlags & .SmallShadow
            invalidatePadding = getThemeMetrics().frameWnd_SmallShadowSize
        else
            invalidatePadding.clear()
    }

    // Set the frame wnd form
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
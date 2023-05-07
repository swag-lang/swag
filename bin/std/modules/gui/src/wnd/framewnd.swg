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
    Zero = 0
    Borders
    FocusBorder
    SmallShadow
}

enum FrameWndAnchor
{
    None
    Left
    Top
    Right
    Bottom
}

struct FrameWnd
{
    using wnd: Wnd

    view:           *Wnd
    frameForm       = FrameWndForm.Transparent
    frameFlags      = FrameWndFlags.Borders
    anchor          = FrameWndAnchor.None
    anchorPos:      f32
    usedColorBk:    Color
}

impl IWnd for FrameWnd
{
    mtd onPrePaintEvent(evt: *PaintEvent)
    {
        if frameFlags.has(.SmallShadow)
        {
            bc      := evt.bc
            painter := bc.painter
            theme   := getTheme()
            colors  := getThemeColors()
            metrics := getThemeMetrics()

            clientRect := getClientRect()
            clientRect.applyPadding(-metrics.frameWnd_SmallShadowSize)
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

        var subRectBk:  *ThemeImageRect
        var colorBk:    Color

        switch frameForm
        {
        case Square:
            subRectBk = &rects.frameWnd_SquareBk
        case Round:
            subRectBk = &rects.frameWnd_RoundBk
        }

        // Display focus borders when the focus wnd is a child of me
        showFocus := frameFlags.has(.FocusBorder)
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
        usedColorBk = colorBk
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
        var colorBorder:    Color = undefined

        switch frameForm
        {
        case Square:
            subRectBorder = &rects.frameWnd_SquareBorder
        case Round:
            subRectBorder = &rects.frameWnd_RoundBorder
        }

        // Display focus borders when the focus wnd is a child of me
        showFocus := frameFlags.has(.FocusBorder)
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

        // Anchor
        if anchor != .None
        {
            posA := anchorPos
            var marginA: f32
            switch frameForm
            {
            case Square:
                marginA = metrics.frameWnd_AnchorSquareBorderMargin
            case Round:
                marginA = metrics.frameWnd_AnchorRoundBorderMargin
            }

            switch anchor
            {
            case Top:
                posA = Math.clamp(posA, marginA, clientRect.width - marginA)
                clientRect.x = clientRect.x + posA - metrics.frameWnd_AnchorWidth * 0.5
                clientRect.y -= metrics.frameWnd_AnchorHeight
                clientRect.width = metrics.frameWnd_AnchorWidth
                clientRect.height = metrics.frameWnd_AnchorHeight + metrics.frameWnd_AnchorOverlap
                theme.drawSubRect(painter, clientRect, &rects.frameWnd_AnchorTopBk, usedColorBk)
                theme.drawSubRect(painter, clientRect, &rects.frameWnd_AnchorTop, colorBorder, false)

            case Bottom:
                posA = Math.clamp(posA, marginA, clientRect.width - marginA)
                clientRect.x = clientRect.x + posA - metrics.frameWnd_AnchorWidth * 0.5
                clientRect.y = clientRect.bottom() - metrics.frameWnd_AnchorOverlap
                clientRect.width = metrics.frameWnd_AnchorWidth
                clientRect.height = metrics.frameWnd_AnchorHeight + metrics.frameWnd_AnchorOverlap
                theme.drawSubRect(painter, clientRect, &rects.frameWnd_AnchorBottomBk, usedColorBk)
                theme.drawSubRect(painter, clientRect, &rects.frameWnd_AnchorBottom, colorBorder, false)

            case Left:
                posA = Math.clamp(posA, marginA, clientRect.height - marginA)
                clientRect.x -= metrics.frameWnd_AnchorHeight
                clientRect.y = clientRect.y + posA - metrics.frameWnd_AnchorHeight * 0.5
                clientRect.width = metrics.frameWnd_AnchorHeight + metrics.frameWnd_AnchorOverlap
                clientRect.height = metrics.frameWnd_AnchorWidth
                theme.drawSubRect(painter, clientRect, &rects.frameWnd_AnchorLeftBk, usedColorBk)
                theme.drawSubRect(painter, clientRect, &rects.frameWnd_AnchorLeft, colorBorder, false)

            case Right:
                posA = Math.clamp(posA, marginA, clientRect.height - marginA)
                clientRect.x = clientRect.right() - metrics.frameWnd_AnchorOverlap
                clientRect.y = clientRect.y + posA - metrics.frameWnd_AnchorHeight * 0.5
                clientRect.width = metrics.frameWnd_AnchorHeight + metrics.frameWnd_AnchorOverlap
                clientRect.height = metrics.frameWnd_AnchorWidth
                theme.drawSubRect(painter, clientRect, &rects.frameWnd_AnchorRightBk, usedColorBk)
                theme.drawSubRect(painter, clientRect, &rects.frameWnd_AnchorRight, colorBorder, false)
            }
        }
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
        view.backgroundStyle = .Transparent
        sendResizeEvent()
        return cast(*T) view
    }

    mtd setView(what: *Wnd)
    {
        if view
            view.destroy()
        what.setParent(self)
        view = what
        sendResizeEvent()
    }

    // Set the frame wnd flags
    mtd setFrameFlags(add, remove = FrameWndFlags.Zero)
    {
        frameFlags.add(add)
        frameFlags.remove(remove)

        if frameFlags.has(.SmallShadow)
            wndFlags.add(.PreChildsPaint)
        else
            wndFlags.remove(.PreChildsPaint)

        if frameFlags.has(.SmallShadow)
            invalidatePadding = -getThemeMetrics().frameWnd_SmallShadowSize
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
            wndFlags.add(.PostChildsPaint)
            setAllPaddings(getThemeMetrics().frameWnd_SquarePadding)
        case Round:
            wndFlags.add(.PostChildsPaint)
            setAllPaddings(getThemeMetrics().frameWnd_RoundPadding)
        case Transparent:
            wndFlags.remove(.PostChildsPaint)
            setAllPaddings(0)
        }

        sendResizeEvent()
    }
}
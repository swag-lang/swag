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
    Zero        = 0
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
    using wnd:       Wnd

    view:            *Wnd
    frameForm        = FrameWndForm.Transparent
    frameFlags       = FrameWndFlags.Borders
    anchor           = FrameWndAnchor.None
    anchorPos:       f32
    usedColorBk:     Color
}

impl IWnd for FrameWnd
{
    mtd impl onPrePaintEvent(evt: *PaintEvent)
    {
        if me.frameFlags.has(.SmallShadow)
        {
            let bc      = evt.bc
            let painter = bc.painter
            let theme   = me.getTheme()
            let colors  = me.getThemeColors()
            let metrics = me.getThemeMetrics()

            var clientRect = me.getClientRect()
            clientRect.applyPadding(-metrics.frameWnd_SmallShadowSize)
            let subRect = &theme.rects.frameWnd_SmallShadow
            theme.drawSubRect(painter, clientRect, subRect, colors.frameWnd_SmallShadow)
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        if me.frameForm == .Transparent:
            return

        let bc      = evt.bc
        let painter = bc.painter
        let theme   = me.getTheme()
        let colors  = me.getThemeColors()
        let rects   = me.getThemeRects()

        var subRectBk: *ThemeImageRect
        var colorBk:   Color

        switch me.frameForm
        {
        case Square:
            subRectBk = &rects.frameWnd_SquareBk
        case Round:
            subRectBk = &rects.frameWnd_RoundBk
        }

        // Display focus borders when the focus wnd is a child of me
        var showFocus = me.frameFlags.has(.FocusBorder)
        if showFocus
        {
            let focus = me.getFocus()
            if !focus or !me.isParentOf(focus):
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

        var clientRect = me.getClientRect()
        theme.drawSubRect(painter, clientRect, subRectBk, colorBk)
        me.usedColorBk = colorBk
    }

    mtd impl onPostPaintEvent(evt: *PaintEvent)
    {
        if me.frameForm == .Transparent:
            return
        if !(me.frameFlags & .Borders):
            return

        let bc      = evt.bc
        let painter = bc.painter
        let theme   = me.getTheme()
        let colors  = me.getThemeColors()
        let rects   = me.getThemeRects()
        let metrics = me.getThemeMetrics()

        var subRectBorder: *ThemeImageRect
        var colorBorder:   Color = undefined

        switch me.frameForm
        {
        case Square:
            subRectBorder = &rects.frameWnd_SquareBorder
        case Round:
            subRectBorder = &rects.frameWnd_RoundBorder
        }

        // Display focus borders when the focus wnd is a child of me
        var showFocus = me.frameFlags.has(.FocusBorder)
        if showFocus
        {
            let focus = me.getFocus()
            if !focus or !me.isParentOf(focus):
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

        var clientRect = me.getClientRect()
        theme.drawSubRect(painter, clientRect, subRectBorder, colorBorder, false)

        // Anchor
        if me.anchor != .None
        {
            var posA     = me.anchorPos
            var marginA: f32
            switch me.frameForm
            {
            case Square:
                marginA = metrics.frameWnd_AnchorSquareBorderMargin
            case Round:
                marginA = metrics.frameWnd_AnchorRoundBorderMargin
            }

            switch me.anchor
            {
            case Top:
                posA         = Math.clamp(posA, marginA, clientRect.width - marginA)
                clientRect.x = clientRect.x + posA - metrics.frameWnd_AnchorWidth * 0.5
                clientRect.y -= metrics.frameWnd_AnchorHeight
                clientRect.width  = metrics.frameWnd_AnchorWidth
                clientRect.height = metrics.frameWnd_AnchorHeight + metrics.frameWnd_AnchorOverlap
                theme.drawSubRect(painter, clientRect, &rects.frameWnd_AnchorTopBk, me.usedColorBk)
                theme.drawSubRect(painter, clientRect, &rects.frameWnd_AnchorTop, colorBorder, false)

            case Bottom:
                posA              = Math.clamp(posA, marginA, clientRect.width - marginA)
                clientRect.x      = clientRect.x + posA - metrics.frameWnd_AnchorWidth * 0.5
                clientRect.y      = clientRect.bottom() - metrics.frameWnd_AnchorOverlap
                clientRect.width  = metrics.frameWnd_AnchorWidth
                clientRect.height = metrics.frameWnd_AnchorHeight + metrics.frameWnd_AnchorOverlap
                theme.drawSubRect(painter, clientRect, &rects.frameWnd_AnchorBottomBk, me.usedColorBk)
                theme.drawSubRect(painter, clientRect, &rects.frameWnd_AnchorBottom, colorBorder, false)

            case Left:
                posA = Math.clamp(posA, marginA, clientRect.height - marginA)
                clientRect.x -= metrics.frameWnd_AnchorHeight
                clientRect.y      = clientRect.y + posA - metrics.frameWnd_AnchorHeight * 0.5
                clientRect.width  = metrics.frameWnd_AnchorHeight + metrics.frameWnd_AnchorOverlap
                clientRect.height = metrics.frameWnd_AnchorWidth
                theme.drawSubRect(painter, clientRect, &rects.frameWnd_AnchorLeftBk, me.usedColorBk)
                theme.drawSubRect(painter, clientRect, &rects.frameWnd_AnchorLeft, colorBorder, false)

            case Right:
                posA              = Math.clamp(posA, marginA, clientRect.height - marginA)
                clientRect.x      = clientRect.right() - metrics.frameWnd_AnchorOverlap
                clientRect.y      = clientRect.y + posA - metrics.frameWnd_AnchorHeight * 0.5
                clientRect.width  = metrics.frameWnd_AnchorHeight + metrics.frameWnd_AnchorOverlap
                clientRect.height = metrics.frameWnd_AnchorWidth
                theme.drawSubRect(painter, clientRect, &rects.frameWnd_AnchorRightBk, me.usedColorBk)
                theme.drawSubRect(painter, clientRect, &rects.frameWnd_AnchorRight, colorBorder, false)
            }
        }
    }

    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        if !me.view:
            return
        var rect = me.getClientRectPadding()
        rect.applyPadding(me.view.margin)
        me.view.setPosition(rect)
    }
}

impl FrameWnd
{
    #[Swag.Discardable]
    mtd(T) createView(hook: HookEvent = null)->*T
    {
        view                 = Wnd.create'T(me, "", position, hook: hook)
        view.backgroundStyle = .Transparent
        sendResizeEvent()
        return cast(*T) view
    }

    mtd setView(what: *Wnd)
    {
        if me.view:
            me.view.destroy()
        what.setParent(me)
        me.view = what
        me.sendResizeEvent()
    }

    // Set the frame wnd flags
    mtd setFrameFlags(add, remove = FrameWndFlags.Zero)
    {
        me.frameFlags.add(add)
        me.frameFlags.remove(remove)

        if me.frameFlags.has(.SmallShadow):
            me.wndFlags.add(.PreChildsPaint)
        else:
            me.wndFlags.remove(.PreChildsPaint)

        if me.frameFlags.has(.SmallShadow):
            me.invalidatePadding = -me.getThemeMetrics().frameWnd_SmallShadowSize
        else:
            me.invalidatePadding.setZero()
    }

    // Set the frame wnd form
    mtd setFrameForm(form: FrameWndForm)
    {
        if me.frameForm == #me.form:
            return
        me.frameForm = #me.form

        #[Swag.Complete]
        switch me.frameForm
        {
        case Square:
            me.wndFlags.add(.PostChildsPaint)
            me.setAllPaddings(me.getThemeMetrics().frameWnd_SquarePadding)
        case Round:
            me.wndFlags.add(.PostChildsPaint)
            me.setAllPaddings(me.getThemeMetrics().frameWnd_RoundPadding)
        case Transparent:
            me.wndFlags.remove(.PostChildsPaint)
            me.setAllPaddings(0)
        }

        me.sendResizeEvent()
    }
}

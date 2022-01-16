using Core, Pixel

public struct ScrollWnd
{
    using frameWnd: FrameWnd

    scrollIncrementV: f32 = 50
    scrollIncrementH: f32 = 50

    scrollSize:     Math.Point
    smallBarSize:   bool

    showBarV:       bool
    posBoxV:        f32
    sizeBoxV:       f32
    isHotV:         bool
    isPressedV:     bool

    showBarH:       bool
    posBoxH:        f32
    sizeBoxH:       f32
    isHotH:         bool
    isPressedH:     bool

    isMoving:       bool
    correcMoving:   f32
}

impl IWnd for ScrollWnd
{
    mtd onResizeEvent(evt: *ResizeEvent)
    {
        computeLayout()
        FrameWnd.IWnd.onResizeEvent(self, evt)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        clientRect := getClientRect()
        pos := surfaceToLocal(evt.surfacePos)

        switch evt.kind
        {
        case MouseWheel:
            if evt.modifiers & .Control
                setScrollPos(view.scrollPos.x - (evt.move.y * scrollIncrementH), view.scrollPos.y)
            else
                setScrollPos(view.scrollPos.x, view.scrollPos.y - (evt.move.y * scrollIncrementV))

        case MouseLeave:
            isHotV, isHotH = false
            invalidate()

        case MouseMove:
            if !isMoving
            {
                hitTest(evt.surfacePos)
                break
            }

            if isPressedV
            {
                f := (pos.y - correcMoving) / clientRect.height
                f *= scrollSize.y
                setScrollPos(view.scrollPos.x, f)
            }
            elif isPressedH
            {
                f := (pos.x - correcMoving) / clientRect.width
                f *= scrollSize.x
                setScrollPos(f, view.scrollPos.y)
            }

            invalidate()

        case MousePressed:
            if evt.button == .Left
            {
                isMoving = true
                if isHotV
                {
                    isPressedV = true
                    correcMoving = pos.y - posBoxV
                }
                elif isHotH
                {
                    isPressedH = true
                    correcMoving = pos.x - posBoxH
                }
                elif pos.x > clientRect.right() - padding.z
                {
                    correcMoving = 0
                    isPressedV = true
                    f := pos.y / clientRect.height
                    f *= scrollSize.y
                    setScrollPos(view.scrollPos.x, f)
                }
                elif pos.y > clientRect.bottom() - padding.w
                {
                    correcMoving = 0
                    isPressedH = true
                    f := pos.x / clientRect.width
                    f *= scrollSize.x
                    setScrollPos(f, view.scrollPos.y)
                }

                captureMouse()
                invalidate()
                hitTest(evt.surfacePos)
            }

        case MouseReleased:
            if isMoving and evt.button == .Left
            {
                releaseMouse()
                isPressedV, isPressedH, isMoving = false
                hitTest(evt.surfacePos)
                invalidate()
            }
        }
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc      := evt.bc
        theme   := getTheme()
        metrics := getThemeMetrics()
        colors  := getThemeColors()
        rects   := getThemeRects()

        // Vertical bar
        if showBarV
        {
            clientRect := getClientRect()
            clientRect.x = clientRect.right() - padding.z
            clientRect.width = padding.z
            if showBarH
                clientRect.height -= padding.w
            theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_BkV, colors.scrollBar_Bk)

            clientRect.y = posBoxV
            clientRect.height = sizeBoxV

            var colorBox: Color = ?
            if isHotV and isPressedV
                colorBox = colors.scrollBar_PressedBox
            elif isHotV
                colorBox = colors.scrollBar_HotBox
            else
                colorBox = colors.scrollBar_Box
            clientRect.inflate(-metrics.scrollBar_BoxPadding)
            theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_BoxV, colorBox)
        }

        // Horizontal bar
        if showBarH
        {
            clientRect := getClientRect()
            clientRect.y = clientRect.bottom() - padding.w
            clientRect.height = padding.w
            if showBarV
                clientRect.width -= padding.w
            theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_BkH, colors.scrollBar_Bk)

            clientRect.x = posBoxH
            clientRect.width = sizeBoxH

            var colorBox: Color = ?
            if isHotH and isPressedH
                colorBox = colors.scrollBar_PressedBox
            elif isHotH
                colorBox = colors.scrollBar_HotBox
            else
                colorBox = colors.scrollBar_Box
            clientRect.inflate(-metrics.scrollBar_BoxPadding)
            theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_BoxH, colorBox)
        }

        // Corner
        if showBarV and showBarH
        {
            clientRect := getClientRect()
            clientRect.x = clientRect.right() - padding.z
            clientRect.y = clientRect.bottom() - padding.w
            clientRect.width = padding.z
            clientRect.height = padding.w
            theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_Corner, colors.scrollBar_Bk)
        }
    }
}

impl ScrollWnd
{
    mtd hitTest(surfacePos: Math.Point)
    {
        pos := surfaceToLocal(surfacePos)
        if isHotV or isHotH
        {
            invalidate()
            isHotV, isHotH = false
        }

        rect := getPosBoxV()
        if rect.contains(pos)
        {
            isHotV = true
            invalidate()
            return
        }

        rect = getPosBoxH()
        if rect.contains(pos)
        {
            isHotH = true
            invalidate()
        }
    }

    mtd getPosBoxV()->Math.Rectangle
    {
        var result: retval = getClientRect()
        result.x = result.right() - padding.z
        result.width = padding.z
        result.y = posBoxV
        result.height = sizeBoxV
        return result
    }

    mtd getPosBoxH()->Math.Rectangle
    {
        var result: retval = getClientRect()
        result.y = result.bottom() - padding.w
        result.height = padding.w
        result.x = posBoxH
        result.width = sizeBoxH
        return result
    }

    mtd computeLayout()
    {
        sx := Math.max(scrollSize.x, position.width)
        sy := Math.max(scrollSize.y, position.height)

        metrics := getThemeMetrics()
        showBarV, showBarH = false
        padding.z, padding.w = 0

        if sy > position.height
            showBarV = true
        if sx > position.width
            showBarH = true

        barSize := smallBarSize ? metrics.scrollBar_SmallSize : metrics.scrollBar_NormalSize

        if showBarV
        {
            padding.z = barSize
            vh := position.height
            if showBarH vh -= barSize

            sizeBoxV = (vh / sy) * position.height
            sizeBoxV = Math.max(sizeBoxV, 10)
            posBoxV = view.scrollPos.y / sy
            posBoxV *= vh
        }

        if showBarH
        {
            padding.w = barSize
            vh := position.width
            if showBarV vh -= barSize

            sizeBoxH = (vh / sx) * position.width
            sizeBoxH = Math.max(sizeBoxH, 10)
            posBoxH = view.scrollPos.x / sx
            posBoxH *= vh
        }

        invalidate()
    }
}

public impl ScrollWnd
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle, id: string = null)->*ScrollWnd
    {
        res := Wnd.create'ScrollWnd(parent, "", position, id)
        res.createView'Wnd()
        res.sendCreateEvent()
        return res
    }

    #[Swag.Discardable]
    func(T) create(parent: *Wnd, position: Math.Rectangle, id: string = null)->*ScrollWnd
    {
        res := Wnd.create'ScrollWnd(parent, "", position, id)
        res.createView'T()
        res.sendCreateEvent()
        return res
    }

    // Get the current scroll position
    mtd getScrollPos()->Math.Point
    {
        return view.scrollPos
    }

    // Set the current scroll position
    mtd setScrollPos(x, y: f32)
    {
        var newSP: Math.Point = ?
        newSP.x = Math.max(x, 0)
        newSP.y = Math.max(y, 0)
        newSP.x = Math.min(newSP.x, scrollSize.x - position.width)
        newSP.y = Math.min(newSP.y, scrollSize.y - position.height)
        if newSP == view.scrollPos
            return

        view.scrollPos = newSP
        computeLayout()
    }

    // Set the scroll size (window virtual size)
    mtd setScrollSize(x, y: f32)
    {
        if scrollSize.x == x and scrollSize.y == y
            return

        scrollSize = @{x, y}
        computeLayout()
        sendResizeEvent()
    }
}
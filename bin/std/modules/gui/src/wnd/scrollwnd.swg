using Core, Pixel

public struct ScrollWnd
{
    using frameWnd: FrameWnd

    scrollIncrementV: f32 = 50
    scrollIncrementH: f32 = 50

    scrollSize:     Math.Point
    smallBarSize:   bool

    sigScrollPosChanged:    func(*Self, Math.Point)

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
    offsetClientV:  f32
    offsetClientH:  f32
    paddingZ:       f32
    paddingW:       f32
}

public impl IWnd for ScrollWnd
{
    mtd onResizeEvent(evt: *ResizeEvent)
    {
        computeLayout()
        if !view
            return
        rect := getClientRectPadding()
        rect.width -= paddingZ
        rect.height -= paddingW
        rect.applyPadding(view.margin)
        view.setPosition(rect)
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
                vh := position.height - paddingW
                f := (pos.y - correcMoving) / (vh - offsetClientV)
                f *= scrollSize.y
                setScrollPos(view.scrollPos.x, f)
            }
            elif isPressedH
            {
                vh := position.width - paddingZ
                f := (pos.x - correcMoving) / (vh - offsetClientH)
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
                elif pos.x > clientRect.right() - paddingZ
                {
                    correcMoving = 0
                    isPressedV = true
                    f := pos.y / clientRect.height
                    f *= scrollSize.y
                    setScrollPos(view.scrollPos.x, f)
                }
                elif pos.y > clientRect.bottom() - paddingW
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

        var colorBk: Color = ?
        switch
        {
            case bc.isDisabled:
                colorBk = colors.scrollBar_DisabledBk
            default:
                colorBk = colors.scrollBar_Bk
        }

        // Vertical bar
        if showBarV
        {
            clientRect := getClientRect()
            clientRect.x = clientRect.right() - paddingZ
            clientRect.width = paddingZ
            if showBarH
                clientRect.height -= paddingW
            theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_BkV, colorBk)

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
            clientRect.y = clientRect.bottom() - paddingW
            clientRect.height = paddingW
            if showBarV
                clientRect.width -= paddingW
            theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_BkH, colorBk)

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
            clientRect.x = clientRect.right() - paddingZ
            clientRect.y = clientRect.bottom() - paddingW
            clientRect.width = paddingZ
            clientRect.height = paddingW
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
        result.x = result.right() - paddingZ
        result.width = paddingZ
        result.y = posBoxV
        result.height = sizeBoxV
        return result
    }

    mtd getPosBoxH()->Math.Rectangle
    {
        var result: retval = getClientRect()
        result.y = result.bottom() - paddingW
        result.height = paddingW
        result.x = posBoxH
        result.width = sizeBoxH
        return result
    }

    mtd computeLayout()
    {
        if position.width == 0 or position.height == 0
            return
        clientRect := getClientRectPadding()
        sx := Math.max(scrollSize.x, clientRect.width)
        sy := Math.max(scrollSize.y, clientRect.height)

        metrics := getThemeMetrics()

        showBarV, showBarH = false
        if sy > clientRect.height
            showBarV = true
        if sx > clientRect.width
            showBarH = true

        barSize := smallBarSize ? metrics.scrollBar_SmallSize : metrics.scrollBar_NormalSize

        paddingZ, paddingW = 0
        if showBarV
            paddingZ = barSize
        if showBarH
            paddingW = barSize

        if showBarV
        {
            vh0 := clientRect.height - paddingW
            vh1 := position.height - paddingW
            sizeBoxV = (vh0 / sy) * vh1
            if sizeBoxV < metrics.scrollBar_BoxMinSize
            {
                offsetClientV = metrics.scrollBar_BoxMinSize - sizeBoxV
                vh1 -= offsetClientV
                sizeBoxV = metrics.scrollBar_BoxMinSize
            }

            posBoxV = view.scrollPos.y / sy
            posBoxV *= vh1
        }

        if showBarH
        {
            vh0 := clientRect.width - paddingZ
            vh1 := position.width - paddingZ
            sizeBoxH = (vh0 / sx) * vh1
            if sizeBoxH < metrics.scrollBar_BoxMinSize
            {
                offsetClientH = metrics.scrollBar_BoxMinSize - sizeBoxH
                vh1 -= offsetClientH
                sizeBoxH = metrics.scrollBar_BoxMinSize
            }

            posBoxH = view.scrollPos.x / sx
            posBoxH *= vh1
        }

        invalidate()
    }
}

public impl ScrollWnd
{
    // Get the current scroll position
    mtd getScrollPos()->Math.Point
    {
        return view.scrollPos
    }

    // Set the current scroll position
    mtd setScrollPos(x, y: f32)
    {
        var newSP: Math.Point = ?

        clientRect := view.getClientRect()
        newSP.x = Math.max(x, 0)
        newSP.y = Math.max(y, 0)
        newSP.x = Math.min(newSP.x, scrollSize.x - clientRect.width)
        newSP.y = Math.min(newSP.y, scrollSize.y - clientRect.height)
        newSP.x = Math.max(newSP.x, 0)
        newSP.y = Math.max(newSP.y, 0)
        if newSP == view.scrollPos
            return

        view.scrollPos = newSP
        if sigScrollPosChanged
            sigScrollPosChanged(self, newSP)
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
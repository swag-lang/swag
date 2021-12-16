using Core, Pixel

public struct ScrollWnd
{
    using frameWnd: FrameWnd
    scrollSize:     Math.Point

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

        switch evt.kind
        {
        case MouseLeave:
            isHotV, isHotH = false
            invalidate()

        case MouseMove:
            if !isMoving
            {
                hitTest(evt.pos)
                break
            }

            if isPressedV
            {
                f := (evt.pos.y - correcMoving) / clientRect.height
                f *= scrollSize.y
                setScrollPos(scrollPos.x, f)
            }
            else if isPressedH
            {
                f := (evt.pos.x - correcMoving) / clientRect.width
                f *= scrollSize.x
                setScrollPos(f, scrollPos.y)
            }

            invalidate()

        case MousePressed:
            if evt.button == .Left
            {
                isMoving = true
                if isHotV
                {
                    isPressedV = true
                    correcMoving = evt.pos.y - posBoxV
                }
                else if isHotH
                {
                    isPressedH = true
                    correcMoving = evt.pos.x - posBoxH
                }
                else if evt.pos.x > clientRect.right() - padding.z
                {
                    correcMoving = 0
                    isPressedV = true
                    f := evt.pos.y / clientRect.height
                    f *= scrollSize.y
                    setScrollPos(scrollPos.x, f)
                }
                else if evt.pos.y > clientRect.bottom() - padding.w
                {
                    correcMoving = 0
                    isPressedH = true
                    f := evt.pos.x / clientRect.width
                    f *= scrollSize.x
                    setScrollPos(f, scrollPos.y)
                }

                captureMouse()
                invalidate()
                hitTest(evt.pos)
            }

        case MouseReleased:
            if isMoving and evt.button == .Left
            {
                releaseMouse()
                isPressedV, isPressedH, isMoving = false
                hitTest(evt.pos)
                invalidate()
            }
        }
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc      := evt.bc
        metrics := getThemeMetrics()

        if showBarV
        {
            clientRect := getClientRect()
            clientRect.x = clientRect.right() - padding.z
            clientRect.width = padding.z
            if showBarH
                clientRect.height -= padding.w
            bc.painter.fillRect(clientRect, Argb.Red)

            clientRect.y = posBoxV
            clientRect.height = sizeBoxV
            bc.painter.fillRect(clientRect, isHotV ? Argb.Green : Argb.Blue)
        }

        if showBarH
        {
            clientRect := getClientRect()
            clientRect.y = clientRect.bottom() - padding.w
            clientRect.height = padding.w
            if showBarV
                clientRect.width -= padding.w
            bc.painter.fillRect(clientRect, Argb.Red)

            clientRect.x = posBoxH
            clientRect.width = sizeBoxH
            bc.painter.fillRect(clientRect, isHotH ? Argb.Green : Argb.Blue)
        }
    }
}

impl ScrollWnd
{
    mtd hitTest(pos: Math.Point)
    {
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
        scrollSize.x = Math.max(scrollSize.x, position.width)
        scrollSize.y = Math.max(scrollSize.y, position.height)

        metrics := getThemeMetrics()
        showBarV, showBarH = false
        padding.z, padding.w = 0

        if scrollSize.y > position.height
            showBarV = true
        if scrollSize.x > position.width
            showBarH = true

        if showBarV
        {
            padding.z = metrics.scrollBar_size
            vh := position.height
            if showBarH vh -= metrics.scrollBar_size

            sizeBoxV = (vh / scrollSize.y) * position.height
            sizeBoxV = Math.max(sizeBoxV, 10)
            posBoxV = view.scrollPos.y / scrollSize.y
            posBoxV *= vh
        }

        if showBarH
        {
            padding.w = metrics.scrollBar_size
            vh := position.width
            if showBarV vh -= metrics.scrollBar_size

            sizeBoxH = (vh / scrollSize.x) * position.width
            sizeBoxH = Math.max(sizeBoxH, 10)
            posBoxH = view.scrollPos.x / scrollSize.x
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
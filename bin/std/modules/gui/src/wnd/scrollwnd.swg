using Core, Pixel

#[Swag.EnumFlags]
public enum ScrollWndFlags
{
    Zero            = 0
    SmallBar
    ForceHorizontal
    ForceVertical
    TransparentBar
}

public struct ScrollWnd
{
    using frameWnd:           FrameWnd

    scrollIncrementV:         f32 = 50
    scrollIncrementH:         f32 = 50

    scrollSize:               Math.Point
    scrollWndFlags            = ScrollWndFlags.Zero

    sigScrollPosChanged:      SigArray'func||(*ScrollWnd, Math.Point, Math.Point)
    sigScrollSizeChanged:     SigArray'func||(*ScrollWnd, Math.Point, Math.Point)

    oldMouseCapture:          *Wnd

    scrollSmoothPos:          Math.Point
    scrollRequestPos:         Math.Point
    smoothScroll:             bool

    showBarV:                 bool
    needV:                    bool
    posBoxV:                  f32
    sizeBoxV:                 f32
    isHotV:                   bool
    isHotBarV:                bool
    isPressedV:               bool

    showBarH:                 bool
    needH:                    bool
    posBoxH:                  f32
    sizeBoxH:                 f32
    isHotH:                   bool
    isHotBarH:                bool
    isPressedH:               bool

    isMoving:                 bool
    isGrabbing:               bool
    correcMoving:             f32
    offsetClientV:            f32
    offsetClientH:            f32
    paddingZ:                 f32
    paddingW:                 f32

    aniBkBarV:                BlendColor
    aniBkBarH:                BlendColor
    aniBoxV:                  BlendColor
    aniBoxH:                  BlendColor

    grabbingPos:              Math.Point
}

public impl IWnd for ScrollWnd
{
    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        .computeLayout()
        if !.view do
            return
        var rect = .getClientRectPadding()
        rect.width -= .paddingZ
        rect.height -= .paddingW
        rect.applyPadding(.view.margin)
        .view.setPosition(rect)
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        let clientRect = .getClientRect()
        let pos        = .surfaceToLocal(evt.surfacePos)

        switch evt.kind
        {
        case MouseWheel:
            if evt.modifiers & .Shift or !.showBarV do
                .setScrollPos(.view.scrollPos.x - (evt.move.y * .scrollIncrementH), .view.scrollPos.y)
            else do
                .setScrollPos(.view.scrollPos.x, .view.scrollPos.y - (evt.move.y * .scrollIncrementV))

        case MouseLeave:
            .isHotV, .isHotH, .isHotBarV, .isHotBarH = false
            .invalidate()

        case MouseMove:
            if .isGrabbing
            {
                let move   = evt.surfacePos - .grabbingPos
                let scroll = .scrollRequestPos - move
                .scrollRequestPos = .getScrollPosPrivate(scroll.x, scroll.y)
                .smoothScroll     = true
                .grabbingPos      = evt.surfacePos
                .invalidate()
                break
            }

            if !.isMoving
            {
                .hitTest(evt.surfacePos)
                break
            }

            if .isPressedV
            {
                let vh = .position.height - .paddingW
                var f  = (pos.y - .correcMoving) / (vh - .offsetClientV)
                f *= .scrollSize.y
                .setScrollPos(.view.scrollPos.x, f)
            }
            elif .isPressedH
            {
                let vh = .position.width - .paddingZ
                var f  = (pos.x - .correcMoving) / (vh - .offsetClientH)
                f *= .scrollSize.x
                .setScrollPos(f, .view.scrollPos.y)
            }

            .invalidate()

        case MousePressed:
            if evt.button == .Middle
            {
                .scrollSmoothPos = .view.scrollPos
                .isGrabbing      = true
                .grabbingPos     = evt.surfacePos
                .oldMouseCapture = .getMouseCapture()
                .captureMouse()
                .invalidate()
            }
            elif evt.button == .Left
            {
                .isMoving = true
                if .isHotV
                {
                    .isPressedV   = true
                    .correcMoving = pos.y - .posBoxV
                }
                elif .isHotH
                {
                    .isPressedH   = true
                    .correcMoving = pos.x - .posBoxH
                }
                elif .isHotBarV
                {
                    .correcMoving = 0
                    .isPressedV   = true
                    var f = pos.y / clientRect.height
                    f *= .scrollSize.y
                    .setScrollPos(.view.scrollPos.x, f)
                }
                elif .isHotBarH
                {
                    .correcMoving = 0
                    .isPressedH   = true
                    var f = pos.x / clientRect.width
                    f *= .scrollSize.x
                    .setScrollPos(f, .view.scrollPos.y)
                }

                .oldMouseCapture = .getMouseCapture()
                .captureMouse()
                .invalidate()
            }

        case MouseReleased:
            if .isGrabbing and evt.button == .Middle
            {
                .releaseMouse()
                if .oldMouseCapture do
                    .oldMouseCapture.captureMouse()
                .hitTest(evt.surfacePos)
                .invalidate()
                .isGrabbing = false
            }
            elif .isMoving and evt.button == .Left
            {
                .releaseMouse()
                if .oldMouseCapture do
                    .oldMouseCapture.captureMouse()
                .isPressedV, .isPressedH, .isMoving = false
                .hitTest(evt.surfacePos)
                .invalidate()
            }
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        if .smoothScroll
        {
            let dt = .getApp().getDt()
            if !Math.isEqualEpsilon(.scrollRequestPos.x, .view.scrollPos.x) or
               !Math.isEqualEpsilon(.scrollRequestPos.y, .view.scrollPos.y)
            {
                .scrollSmoothPos.x = Math.lerp(.scrollSmoothPos.x, .scrollRequestPos.x, 10 * dt)
                .scrollSmoothPos.y = Math.lerp(.scrollSmoothPos.y, .scrollRequestPos.y, 10 * dt)
                discard .setScrollPosPrivate(.scrollSmoothPos.x, .scrollSmoothPos.y)
                .invalidate()
            }
            else do
                .smoothScroll = false
        }

        let bc      = evt.bc
        let theme   = .getTheme()
        let metrics = .getThemeMetrics()
        let colors  = .getThemeColors()
        let rects   = .getThemeRects()

        var colorBk: Color
        switch
        {
        case .scrollWndFlags.has(.TransparentBar):
            break
        case bc.isDisabled:
            colorBk = colors.scrollBar_DisabledBk
        default:
            colorBk = colors.scrollBar_Bk
        }

        // Vertical bar
        if .showBarV
        {
            var clientRect = .getClientRect()
            clientRect.x     = clientRect.right() - .paddingZ
            clientRect.width = .paddingZ
            if .showBarH do
                clientRect.height -= .paddingW
            theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_BkV, colorBk)

            var colBk = colors.scrollBar_HotBk
            if !.isHotBarV do
                colBk.a = 0
            .aniBkBarV.apply(me, colBk)
            theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_BkV, .aniBkBarV.cur())

            if .needV
            {
                clientRect.y      = .posBoxV
                clientRect.height = .sizeBoxV

                var colorBox: Color = undefined
                if .isHotV and .isPressedV do
                    colorBox = colors.scrollBar_PressedBox
                elif .isHotV do
                    colorBox = colors.scrollBar_HotBox
                else do
                    colorBox = colors.scrollBar_Box
                clientRect.inflate(-metrics.scrollBar_BoxPadding)
                .aniBoxV.apply(me, colorBox)
                theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_BoxV, .aniBoxV.cur())
            }
        }

        // Horizontal bar
        if .showBarH
        {
            var clientRect = .getClientRect()
            clientRect.y      = clientRect.bottom() - .paddingW
            clientRect.height = .paddingW
            if .showBarV do
                clientRect.width -= .paddingW
            theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_BkH, colorBk)

            var colBk = colors.scrollBar_HotBk
            if !.isHotBarH do
                colBk.a = 0
            .aniBkBarH.apply(me, colBk)
            theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_BkV, .aniBkBarH.cur())

            if .needH
            {
                clientRect.x     = .posBoxH
                clientRect.width = .sizeBoxH

                var colorBox: Color = undefined
                if .isHotH and .isPressedH do
                    colorBox = colors.scrollBar_PressedBox
                elif .isHotH do
                    colorBox = colors.scrollBar_HotBox
                else do
                    colorBox = colors.scrollBar_Box
                clientRect.inflate(-metrics.scrollBar_BoxPadding)
                .aniBoxH.apply(me, colorBox)
                theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_BoxH, .aniBoxH.cur())
            }
        }

        // Corner
        if .showBarV and .showBarH
        {
            var clientRect = .getClientRect()
            clientRect.x      = clientRect.right() - .paddingZ
            clientRect.y      = clientRect.bottom() - .paddingW
            clientRect.width  = .paddingZ
            clientRect.height = .paddingW
            theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_Corner, colors.scrollBar_Bk)
        }
    }
}

impl ScrollWnd
{
    mtd hitTest(surfacePos: Math.Point)
    {
        let pos = .surfaceToLocal(surfacePos)
        if .isHotV or .isHotH or .isHotBarV or .isHotBarH
        {
            .invalidate()
            .isHotV, .isHotH, .isHotBarV, .isHotBarH = false
        }

        var rect = .getPosBoxV()
        if rect.contains(pos)
        {
            .isHotV, .isHotBarV = true
            .invalidate()
            return
        }

        rect = .getPosBoxH()
        if rect.contains(pos)
        {
            .isHotH, .isHotBarH = true
            .invalidate()
            return
        }

        rect = .getPosBoxV(true)
        if rect.contains(pos)
        {
            .isHotBarV = true
            .invalidate()
            return
        }

        rect = .getPosBoxH(true)
        if rect.contains(pos)
        {
            .isHotBarH = true
            .invalidate()
            return
        }
    }

    mtd getPosBoxV(full = false)->Math.Rectangle
    {
        var result: retval = .getClientRect()
        result.x     = result.right() - .paddingZ
        result.width = .paddingZ
        if !full
        {
            result.y      = .posBoxV
            result.height = .sizeBoxV
        }

        return result
    }

    mtd getPosBoxH(full = false)->Math.Rectangle
    {
        var result: retval = .getClientRect()
        result.y      = result.bottom() - .paddingW
        result.height = .paddingW
        if !full
        {
            result.x     = .posBoxH
            result.width = .sizeBoxH
        }

        return result
    }

    mtd computeLayout()
    {
        if .position.width == 0 or .position.height == 0 do
            return
        let clientRect = .getClientRectPadding()
        var sx         = Math.max(.scrollSize.x, clientRect.width)
        var sy         = Math.max(.scrollSize.y, clientRect.height)
        sx = Math.max(sx, 1)
        sy = Math.max(sy, 1)

        let metrics = .getThemeMetrics()

        .showBarV, .showBarH = false
        .needH, .needV = false

        if sy > clientRect.height do
            .needV = true
        if .needV or .scrollWndFlags.has(.ForceVertical) do
            .showBarV = true

        if sx > clientRect.width do
            .needH = true
        if .needH or .scrollWndFlags.has(.ForceHorizontal) do
            .showBarH = true

        let barSize = .scrollWndFlags.has(.SmallBar) ? metrics.scrollBar_SmallSize : metrics.scrollBar_NormalSize

        .paddingZ, .paddingW = 0
        if .showBarV do
            .paddingZ = barSize
        if .showBarH do
            .paddingW = barSize

        if .showBarV
        {
            let vh0 = clientRect.height - .paddingW
            var vh1 = .position.height - .paddingW
            .sizeBoxV = (vh0 / sy) * vh1
            if .sizeBoxV < metrics.scrollBar_BoxMinSize
            {
                .offsetClientV = metrics.scrollBar_BoxMinSize - .sizeBoxV
                vh1 -= .offsetClientV
                .sizeBoxV = metrics.scrollBar_BoxMinSize
            }

            .posBoxV = .view.scrollPos.y / sy
            .posBoxV *= vh1

            if .view.scrollPos.y > .scrollSize.y - clientRect.height do
                .view.scrollPos.y = Math.max(.scrollSize.y - clientRect.height, 0)
        }

        if .showBarH
        {
            let vh0 = clientRect.width - .paddingZ
            var vh1 = .position.width - .paddingZ
            .sizeBoxH = (vh0 / sx) * vh1
            if .sizeBoxH < metrics.scrollBar_BoxMinSize
            {
                .offsetClientH = metrics.scrollBar_BoxMinSize - .sizeBoxH
                vh1 -= .offsetClientH
                .sizeBoxH = metrics.scrollBar_BoxMinSize
            }

            .posBoxH = .view.scrollPos.x / sx
            .posBoxH *= vh1

            if .view.scrollPos.x > .scrollSize.x - clientRect.width do
                .view.scrollPos.x = Math.max(.scrollSize.x - clientRect.width, 0)
        }

        .invalidate()
    }
}

public impl ScrollWnd
{
    // Get the current scroll position
    mtd getScrollPos()->Math.Point
    {
        return .view.scrollPos
    }

    internal mtd getScrollPosPrivate(x, y: f32)->Math.Point
    {
        var newSP: Math.Point = undefined

        let clientRect = .view.getClientRect()
        newSP.x = Math.max(x, 0)
        newSP.y = Math.max(y, 0)
        newSP.x = Math.min(newSP.x, .scrollSize.x - clientRect.width)
        newSP.y = Math.min(newSP.y, .scrollSize.y - clientRect.height)
        newSP.x = Math.max(newSP.x, 0)
        newSP.y = Math.max(newSP.y, 0)
        newSP.x = Math.round(newSP.x)
        newSP.y = Math.round(newSP.y)

        return newSP
    }

    internal mtd setScrollPosPrivate(x, y: f32)->Math.Point
    {
        let newSP = .getScrollPosPrivate(x, y)
        if newSP == .view.scrollPos do
            return newSP
        let oldPos = .view.scrollPos
        .view.scrollPos = newSP
        .sigScrollPosChanged.call(me, oldPos, newSP)
        .computeLayout()
        return newSP
    }

    // Set the current scroll position
    #[Swag.Overload]
    mtd setScrollPos(pt: Math.Point)
    {
        .setScrollPos(pt.x, pt.y)
    }

    #[Swag.Overload]
    mtd setScrollPos(x, y: f32)
    {
        let newSP = .setScrollPosPrivate(x, y)
        .scrollRequestPos = newSP
        .smoothScroll     = false
    }

    // Set the scroll size (window virtual size)
    mtd setScrollSize(x, y: f32)
    {
        if .scrollSize.x == x and .scrollSize.y == y do
            return

        let oldScrollSize = .scrollSize
        .scrollSize = {x, y}
        .setScrollPos(.view.scrollPos.x, .view.scrollPos.y)
        .sigScrollSizeChanged.call(me, oldScrollSize, .scrollSize)
        .sendResizeEvent()
    }

    mtd getClientScrollRect()->Math.Rectangle
    {
        var result: retval
        result.width  = Math.max(.scrollSize.x, .view.position.width)
        result.height = Math.max(.scrollSize.y, .view.position.height)
        return result
    }
}

#global marked
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
        me.computeLayout()
        if !me.view:
            return
        var rect = me.getClientRectPadding()
        rect.width -= me.paddingZ
        rect.height -= me.paddingW
        rect.applyPadding(me.view.margin)
        me.view.setPosition(rect)
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        var clientRect = me.getClientRect()
        var pos        = me.surfaceToLocal(evt.surfacePos)

        switch evt.kind
        {
        case MouseWheel:
            if evt.modifiers & .Shift or !me.showBarV:
                me.setScrollPos(me.view.scrollPos.x - (evt.move.y * me.scrollIncrementH), me.view.scrollPos.y)
            else:
                me.setScrollPos(me.view.scrollPos.x, me.view.scrollPos.y - (evt.move.y * me.scrollIncrementV))

        case MouseLeave:
            me.isHotV, me.isHotH, me.isHotBarV, me.isHotBarH = false
            me.invalidate()

        case MouseMove:
            if me.isGrabbing
            {
                var move   = evt.surfacePos - me.grabbingPos
                var scroll = me.scrollRequestPos - move
                me.scrollRequestPos = me.getScrollPosPrivate(scroll.x, scroll.y)
                me.smoothScroll     = true
                me.grabbingPos      = evt.surfacePos
                me.invalidate()
                break
            }

            if !me.isMoving
            {
                me.hitTest(evt.surfacePos)
                break
            }

            if me.isPressedV
            {
                let vh = me.position.height - me.paddingW
                var f  = (pos.y - me.correcMoving) / (vh - me.offsetClientV)
                f *= me.scrollSize.y
                me.setScrollPos(me.view.scrollPos.x, f)
            }
            elif me.isPressedH
            {
                let vh = me.position.width - me.paddingZ
                var f  = (pos.x - me.correcMoving) / (vh - me.offsetClientH)
                f *= me.scrollSize.x
                me.setScrollPos(f, me.view.scrollPos.y)
            }

            me.invalidate()

        case MousePressed:
            if evt.button == .Middle
            {
                me.scrollSmoothPos = me.view.scrollPos
                me.isGrabbing      = true
                me.grabbingPos     = evt.surfacePos
                me.oldMouseCapture = me.getMouseCapture()
                me.captureMouse()
                me.invalidate()
            }
            elif evt.button == .Left
            {
                me.isMoving = true
                if me.isHotV
                {
                    me.isPressedV   = true
                    me.correcMoving = pos.y - me.posBoxV
                }
                elif me.isHotH
                {
                    me.isPressedH   = true
                    me.correcMoving = pos.x - me.posBoxH
                }
                elif me.isHotBarV
                {
                    me.correcMoving = 0
                    me.isPressedV   = true
                    var f = pos.y / clientRect.height
                    f *= me.scrollSize.y
                    me.setScrollPos(me.view.scrollPos.x, f)
                }
                elif me.isHotBarH
                {
                    me.correcMoving = 0
                    me.isPressedH   = true
                    var f = pos.x / clientRect.width
                    f *= me.scrollSize.x
                    me.setScrollPos(f, me.view.scrollPos.y)
                }

                me.oldMouseCapture = me.getMouseCapture()
                me.captureMouse()
                me.invalidate()
            }

        case MouseReleased:
            if me.isGrabbing and evt.button == .Middle
            {
                me.releaseMouse()
                if me.oldMouseCapture:
                    me.oldMouseCapture.captureMouse()
                me.hitTest(evt.surfacePos)
                me.invalidate()
                me.isGrabbing = false
            }
            elif me.isMoving and evt.button == .Left
            {
                me.releaseMouse()
                if me.oldMouseCapture:
                    me.oldMouseCapture.captureMouse()
                me.isPressedV, me.isPressedH, me.isMoving = false
                me.hitTest(evt.surfacePos)
                me.invalidate()
            }
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        if me.smoothScroll
        {
            let dt = me.getApp().getDt()
            if !Math.isEqualEpsilon(me.scrollRequestPos.x, me.view.scrollPos.x) or
               !Math.isEqualEpsilon(me.scrollRequestPos.y, me.view.scrollPos.y)
            {
                me.scrollSmoothPos.x = Math.lerp(me.scrollSmoothPos.x, me.scrollRequestPos.x, 10 * dt)
                me.scrollSmoothPos.y = Math.lerp(me.scrollSmoothPos.y, me.scrollRequestPos.y, 10 * dt)
                discard me.setScrollPosPrivate(me.scrollSmoothPos.x, me.scrollSmoothPos.y)
                me.invalidate()
            }
            else:
                me.smoothScroll = false
        }

        let bc      = evt.bc
        let theme   = me.getTheme()
        let metrics = me.getThemeMetrics()
        let colors  = me.getThemeColors()
        let rects   = me.getThemeRects()

        var colorBk: Color
        switch
        {
        case me.scrollWndFlags.has(.TransparentBar):
            break
        case bc.isDisabled:
            colorBk = colors.scrollBar_DisabledBk
        default:
            colorBk = colors.scrollBar_Bk
        }

        // Vertical bar
        if me.showBarV
        {
            var clientRect = me.getClientRect()
            clientRect.x     = clientRect.right() - me.paddingZ
            clientRect.width = me.paddingZ
            if me.showBarH:
                clientRect.height -= me.paddingW
            theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_BkV, colorBk)

            var colBk = colors.scrollBar_HotBk
            if !me.isHotBarV:
                colBk.a = 0
            me.aniBkBarV.apply(me, colBk)
            theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_BkV, me.aniBkBarV.cur())

            if me.needV
            {
                clientRect.y      = me.posBoxV
                clientRect.height = me.sizeBoxV

                var colorBox: Color = undefined
                if me.isHotV and me.isPressedV:
                    colorBox = colors.scrollBar_PressedBox
                elif me.isHotV:
                    colorBox = colors.scrollBar_HotBox
                else:
                    colorBox = colors.scrollBar_Box
                clientRect.inflate(-metrics.scrollBar_BoxPadding)
                me.aniBoxV.apply(me, colorBox)
                theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_BoxV, me.aniBoxV.cur())
            }
        }

        // Horizontal bar
        if me.showBarH
        {
            var clientRect = me.getClientRect()
            clientRect.y      = clientRect.bottom() - me.paddingW
            clientRect.height = me.paddingW
            if me.showBarV:
                clientRect.width -= me.paddingW
            theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_BkH, colorBk)

            var colBk = colors.scrollBar_HotBk
            if !me.isHotBarH:
                colBk.a = 0
            me.aniBkBarH.apply(me, colBk)
            theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_BkV, me.aniBkBarH.cur())

            if me.needH
            {
                clientRect.x     = me.posBoxH
                clientRect.width = me.sizeBoxH

                var colorBox: Color = undefined
                if me.isHotH and me.isPressedH:
                    colorBox = colors.scrollBar_PressedBox
                elif me.isHotH:
                    colorBox = colors.scrollBar_HotBox
                else:
                    colorBox = colors.scrollBar_Box
                clientRect.inflate(-metrics.scrollBar_BoxPadding)
                me.aniBoxH.apply(me, colorBox)
                theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_BoxH, me.aniBoxH.cur())
            }
        }

        // Corner
        if me.showBarV and me.showBarH
        {
            var clientRect = me.getClientRect()
            clientRect.x      = clientRect.right() - me.paddingZ
            clientRect.y      = clientRect.bottom() - me.paddingW
            clientRect.width  = me.paddingZ
            clientRect.height = me.paddingW
            theme.drawSubRect(bc.painter, clientRect, &rects.scrollBar_Corner, colors.scrollBar_Bk)
        }
    }
}

impl ScrollWnd
{
    mtd hitTest(surfacePos: Math.Point)
    {
        var pos = me.surfaceToLocal(surfacePos)
        if me.isHotV or me.isHotH or me.isHotBarV or me.isHotBarH
        {
            me.invalidate()
            me.isHotV, me.isHotH, me.isHotBarV, me.isHotBarH = false
        }

        var rect = me.getPosBoxV()
        if rect.contains(pos)
        {
            me.isHotV, me.isHotBarV = true
            me.invalidate()
            return
        }

        rect = me.getPosBoxH()
        if rect.contains(pos)
        {
            me.isHotH, me.isHotBarH = true
            me.invalidate()
            return
        }

        rect = me.getPosBoxV(true)
        if rect.contains(pos)
        {
            me.isHotBarV = true
            me.invalidate()
            return
        }

        rect = me.getPosBoxH(true)
        if rect.contains(pos)
        {
            me.isHotBarH = true
            me.invalidate()
            return
        }
    }

    mtd getPosBoxV(full = false)->Math.Rectangle
    {
        var result: retval = me.getClientRect()
        result.x     = result.right() - me.paddingZ
        result.width = me.paddingZ
        if !full
        {
            result.y      = me.posBoxV
            result.height = me.sizeBoxV
        }

        return result
    }

    mtd getPosBoxH(full = false)->Math.Rectangle
    {
        var result: retval = me.getClientRect()
        result.y      = result.bottom() - me.paddingW
        result.height = me.paddingW
        if !full
        {
            result.x     = me.posBoxH
            result.width = me.sizeBoxH
        }

        return result
    }

    mtd computeLayout()
    {
        if me.position.width == 0 or me.position.height == 0:
            return
        var clientRect = me.getClientRectPadding()
        var sx         = Math.max(me.scrollSize.x, clientRect.width)
        var sy         = Math.max(me.scrollSize.y, clientRect.height)
        sx = Math.max(sx, 1)
        sy = Math.max(sy, 1)

        let metrics = me.getThemeMetrics()

        me.showBarV, me.showBarH = false
        me.needH, me.needV = false

        if sy > clientRect.height:
            me.needV = true
        if me.needV or me.scrollWndFlags.has(.ForceVertical):
            me.showBarV = true

        if sx > clientRect.width:
            me.needH = true
        if me.needH or me.scrollWndFlags.has(.ForceHorizontal):
            me.showBarH = true

        let barSize = me.scrollWndFlags.has(.SmallBar) ? metrics.scrollBar_SmallSize : metrics.scrollBar_NormalSize

        me.paddingZ, me.paddingW = 0
        if me.showBarV:
            me.paddingZ = barSize
        if me.showBarH:
            me.paddingW = barSize

        if me.showBarV
        {
            let vh0 = clientRect.height - me.paddingW
            var vh1 = me.position.height - me.paddingW
            me.sizeBoxV = (vh0 / sy) * vh1
            if me.sizeBoxV < metrics.scrollBar_BoxMinSize
            {
                me.offsetClientV = metrics.scrollBar_BoxMinSize - me.sizeBoxV
                vh1 -= me.offsetClientV
                me.sizeBoxV = metrics.scrollBar_BoxMinSize
            }

            me.posBoxV = me.view.scrollPos.y / sy
            me.posBoxV *= vh1

            if me.view.scrollPos.y > me.scrollSize.y - clientRect.height:
                me.view.scrollPos.y = Math.max(me.scrollSize.y - clientRect.height, 0)
        }

        if me.showBarH
        {
            let vh0 = clientRect.width - me.paddingZ
            var vh1 = me.position.width - me.paddingZ
            me.sizeBoxH = (vh0 / sx) * vh1
            if me.sizeBoxH < metrics.scrollBar_BoxMinSize
            {
                me.offsetClientH = metrics.scrollBar_BoxMinSize - me.sizeBoxH
                vh1 -= me.offsetClientH
                me.sizeBoxH = metrics.scrollBar_BoxMinSize
            }

            me.posBoxH = me.view.scrollPos.x / sx
            me.posBoxH *= vh1

            if me.view.scrollPos.x > me.scrollSize.x - clientRect.width:
                me.view.scrollPos.x = Math.max(me.scrollSize.x - clientRect.width, 0)
        }

        me.invalidate()
    }
}

public impl ScrollWnd
{
    // Get the current scroll position
    mtd getScrollPos()->Math.Point
    {
        return me.view.scrollPos
    }

    internal mtd getScrollPosPrivate(x, y: f32)->Math.Point
    {
        var newSP: Math.Point = undefined

        var clientRect = me.view.getClientRect()
        newSP.x = Math.max(x, 0)
        newSP.y = Math.max(y, 0)
        newSP.x = Math.min(newSP.x, me.scrollSize.x - clientRect.width)
        newSP.y = Math.min(newSP.y, me.scrollSize.y - clientRect.height)
        newSP.x = Math.max(newSP.x, 0)
        newSP.y = Math.max(newSP.y, 0)
        newSP.x = Math.round(newSP.x)
        newSP.y = Math.round(newSP.y)

        return newSP
    }

    internal mtd setScrollPosPrivate(x, y: f32)->Math.Point
    {
        var newSP = me.getScrollPosPrivate(x, y)
        if newSP == me.view.scrollPos:
            return newSP
        var oldPos = me.view.scrollPos
        me.view.scrollPos = newSP
        me.sigScrollPosChanged.call(me, oldPos, newSP)
        me.computeLayout()
        return newSP
    }

    // Set the current scroll position
    #[Swag.Overload]
    mtd setScrollPos(pt: Math.Point)
    {
        me.setScrollPos(pt.x, pt.y)
    }

    #[Swag.Overload]
    mtd setScrollPos(x, y: f32)
    {
        var newSP = me.setScrollPosPrivate(x, y)
        me.scrollRequestPos = newSP
        me.smoothScroll     = false
    }

    // Set the scroll size (window virtual size)
    mtd setScrollSize(x, y: f32)
    {
        if me.scrollSize.x == x and me.scrollSize.y == y:
            return

        var oldScrollSize = me.scrollSize
        me.scrollSize = {x, y}
        me.setScrollPos(me.view.scrollPos.x, me.view.scrollPos.y)
        me.sigScrollSizeChanged.call(me, oldScrollSize, me.scrollSize)
        me.sendResizeEvent()
    }

    mtd getClientScrollRect()->Math.Rectangle
    {
        var result: retval
        result.width  = Math.max(me.scrollSize.x, me.view.position.width)
        result.height = Math.max(me.scrollSize.y, me.view.position.height)
        return result
    }
}

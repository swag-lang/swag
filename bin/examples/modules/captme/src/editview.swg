struct EditView
{
    using wnd:       Wnd
    main:            *MainWnd
    scroll:          *ScrollWnd
    capture:         *Capture
    zoom:            f32 = 1
    viewMargin:      f32 = 32
    firstPaint:      bool = true
    inPlaceEdit:     bool
}

impl IWnd for EditView
{
    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        evt.accepted = false
        if !me.capture:
            return

        if me.capture.mouse(me, evt)
        {
            me.invalidate()
            evt.accepted = true
            me.setFocus()
            return
        }

        switch evt.kind
        {
        case MouseWheel:
            if evt.modifiers & .Control
            {
                evt.accepted = true
                let z = me.zoom + (evt.move.y > 0 ? 0.1 : -0.1)
                me.setZoom(z, true)
            }
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        if me.firstPaint
        {
            me.zoomToFit(false)
            me.firstPaint = false
        }

        let painter = evt.bc.painter

        // Background
        var rect = me.getClientRect()
        rect.offset(me.scrollPos)
        painter.fillRect(rect, Argb.Black)
        if !me.capture:
            return

        // Capture content
        let captureRect = me.getCaptureRect()
        var clip        = rect
        clip.inflate(100, 100)
        me.capture.paint(painter, clip, captureRect)

        // Around capture
        var col = me.main.editorOptions.editBackColor
        col.a = me.main.editorOptions.clipToSelection ? 255 : 200
        painter.fillRect({rect.x, rect.y, captureRect.x, rect.height}, col)
        painter.fillRect({captureRect.x, rect.y, captureRect.width, captureRect.y}, col)
        painter.fillRect({captureRect.right(), rect.y, rect.right() - captureRect.right(), rect.height}, col)
        painter.fillRect({captureRect.x, captureRect.bottom(), captureRect.width, rect.bottom() - captureRect.bottom()}, col)

        // Capture gizmo
        painter.pushTransform()
        painter.translateTransform(captureRect.x, captureRect.y)
        me.capture.gizmo.rect = Math.Rectangle{0, 0, captureRect.width, captureRect.height}
        me.capture.gizmo.paint(painter, false)
        painter.popTransform()
    }
}

impl EditView
{
    mtd opDrop()
    {
        Memory.delete(me.capture)
    }

    mtd getCaptureRect()->Math.Rectangle
    {
        var captureRect: retval
        var rect         = me.scroll.getClientScrollRect()
        captureRect.width  = me.capture.width * me.zoom
        captureRect.height = me.capture.height * me.zoom
        if me.inPlaceEdit
        {
            captureRect.x = -me.capture.backImagePos.x
            captureRect.y = -me.capture.backImagePos.y
        }
        else
        {
            captureRect.x = rect.horzCenter() - captureRect.width * 0.5
            captureRect.y = rect.vertCenter() - captureRect.height * 0.5
        }

        captureRect.round()
        return captureRect
    }

    const MinZoom = 0.1
    const MaxZoom = 10.0

    mtd setZoom(z: f32, mouseCenter = false)
    {
        var mousePos, mousePos1: Math.Point
        if mouseCenter
        {
            mousePos  = me.getApp().mouse.getPosition()
            mousePos  = me.screenToSurface(mousePos)
            mousePos  = me.surfaceToLocal(mousePos)
            mousePos1 = mousePos + me.scrollPos
            mousePos1 /= me.zoom
        }

        me.zoom = Math.clamp(z, MinZoom, MaxZoom)
        var captureRect = me.getCaptureRect()
        me.scroll.setScrollSize(captureRect.width + (2 * me.viewMargin), captureRect.height + (2 * me.viewMargin))

        if mouseCenter
        {
            mousePos1 *= me.zoom
            mousePos1 -= mousePos
            me.scroll.setScrollPos(mousePos1.x, mousePos1.y)
        }

        me.invalidate()
    }

    mtd canZoomIn()  => me.zoom != MaxZoom
    mtd canZoomOut() => me.zoom != MinZoom

    mtd zoomIn()
    {
        me.setZoom(me.zoom + 0.1)
    }

    mtd zoomOut()
    {
        me.setZoom(me.zoom - 0.1)
    }

    mtd zoomToFit(canZoom = true)
    {
        if !me.capture:
            return
        var rect = me.getClientRect()
        rect.inflate(-me.viewMargin * 2)

        var zx = rect.width / me.capture.width
        var zy = rect.height / me.capture.height

        if !canZoom
        {
            zx = Math.min(zx, 1)
            zy = Math.min(zy, 1)
        }

        me.setZoom(Math.min(zx, zy))
    }

    mtd setCapture(c: *Capture)
    {
        if me.capture:
            Memory.delete(me.capture)
        me.capture = c
        if me.capture:
            me.capture.setup(me)
        if me.main:
            me.main.captureHasChanged()
        me.invalidate()
    }
}

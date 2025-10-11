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
        if !.capture do
            return

        if .capture.mouse(me, evt)
        {
            .invalidate()
            evt.accepted = true
            .setFocus()
            return
        }

        switch evt.kind
        {
        case MouseWheel:
            if evt.modifiers & .Control
            {
                evt.accepted = true
                let z = .zoom + (evt.move.y > 0 ? 0.1 : -0.1)
                .setZoom(z, true)
            }
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        if .firstPaint
        {
            .zoomToFit(false)
            .firstPaint = false
        }

        let painter = evt.bc.painter

        // Background
        var rect = .getClientRect()
        rect.offset(.scrollPos)
        painter.fillRect(rect, Argb.Black)
        if !.capture do
            return

        // Capture content
        let captureRect = .getCaptureRect()
        var clip        = rect
        clip.inflate(100, 100)
        .capture.paint(painter, clip, captureRect)

        // Around capture
        var col = .main.editorOptions.editBackColor
        col.a = .main.editorOptions.clipToSelection ? 255 : 200
        painter.fillRect({rect.x, rect.y, captureRect.x, rect.height}, col)
        painter.fillRect({captureRect.x, rect.y, captureRect.width, captureRect.y}, col)
        painter.fillRect({captureRect.right(), rect.y, rect.right() - captureRect.right(), rect.height}, col)
        painter.fillRect({captureRect.x, captureRect.bottom(), captureRect.width, rect.bottom() - captureRect.bottom()}, col)

        // Capture gizmo
        painter.pushTransform()
        painter.translateTransform(captureRect.x, captureRect.y)
        .capture.gizmo.rect = Math.Rectangle{0, 0, captureRect.width, captureRect.height}
        .capture.gizmo.paint(painter, false, .getThemeColors().hilight)
        painter.popTransform()
    }
}

impl EditView
{
    mtd opDrop()
    {
        Memory.delete(.capture)
    }

    mtd getCaptureRect()->Math.Rectangle
    {
        var captureRect: retval
        let rect         = .scroll.getClientScrollRect()
        captureRect.width  = .capture.width * .zoom
        captureRect.height = .capture.height * .zoom
        if .inPlaceEdit
        {
            captureRect.x = -.capture.backImagePos.x
            captureRect.y = -.capture.backImagePos.y
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
            mousePos  = .getApp().mouse.getPosition()
            mousePos  = .screenToSurface(mousePos)
            mousePos  = .surfaceToLocal(mousePos)
            mousePos1 = mousePos + .scrollPos
            mousePos1 /= .zoom
        }

        .zoom = Math.clamp(z, MinZoom, MaxZoom)
        let captureRect = .getCaptureRect()
        .scroll.setScrollSize(captureRect.width + (2 * .viewMargin), captureRect.height + (2 * .viewMargin))

        if mouseCenter
        {
            mousePos1 *= .zoom
            mousePos1 -= mousePos
            .scroll.setScrollPos(mousePos1.x, mousePos1.y)
        }

        .invalidate()
    }

    mtd canZoomIn()  => .zoom != MaxZoom
    mtd canZoomOut() => .zoom != MinZoom

    mtd zoomIn()
    {
        .setZoom(.zoom + 0.1)
    }

    mtd zoomOut()
    {
        .setZoom(.zoom - 0.1)
    }

    mtd zoomToFit(canZoom = true)
    {
        if !.capture do
            return
        var rect = .getClientRect()
        rect.inflate(-.viewMargin * 2)

        var zx = rect.width / .capture.width
        var zy = rect.height / .capture.height

        if !canZoom
        {
            zx = Math.min(zx, 1)
            zy = Math.min(zy, 1)
        }

        .setZoom(Math.min(zx, zy))
    }

    mtd setCapture(c: *Capture)
    {
        if .capture do
            Memory.delete(.capture)
        .capture = c
        if .capture do
            .capture.setup(me)
        if .main do
            .main.captureHasChanged()
        .invalidate()
    }
}

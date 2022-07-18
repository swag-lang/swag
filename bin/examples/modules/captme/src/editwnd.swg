struct EditWnd
{
    using scrollWnd: ScrollWnd
    editView:        *EditView
}

struct EditView
{
    using wnd:  Wnd
    main:       *MainWnd
    scroll:     *ScrollWnd
    capture:    *Capture
    zoom:       f32 = 1
    viewMargin: f32 = 32
    firstPaint: bool = true
}

impl EditWnd
{
    func create(main: *MainWnd, parent: *Wnd)->*EditWnd
    {
        scroll := Wnd.create'EditWnd(parent)
        view   := Wnd.create'EditView(scroll)
        view.scroll = scroll
        view.main = main
        scroll.editView = view
        scroll.view = view
        return scroll
    }
}

impl IWnd for EditView
{
    mtd onMouseEvent(evt: *MouseEvent)
    {
        evt.accepted = false
        if !capture return

        if capture.mouse(self, evt)
        {
            invalidate()
            evt.accepted = true
            return
        }

        switch evt.kind
        {
        case MouseWheel:
            if evt.modifiers & .Control
            {
                z := zoom + (evt.move.y > 0 ? 0.1 : -0.1)
                setZoom(z, true)
                evt.accepted = true
            }
        }
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        if firstPaint
        {
            zoomToFit(false)
            firstPaint = false
        }

        painter := evt.bc.painter

        // Background
        rect := getClientRect()
        rect.offset(scrollPos)
        painter.fillRect(rect, getThemeColors().view_Bk)
        if !capture
            return

        // Capture content
        rect = getCaptureRect()
        capture.paint(painter, rect)

        // Capture gizmo
        capture.gizmo.rect = rect
        capture.gizmo.paint(painter, false)
    }
}

impl EditView
{
    mtd getCaptureRect()->Math.Rectangle
    {
        var captureRect: retval
        rect := scroll.getClientScrollRect()
        captureRect.width  = capture.width * zoom
        captureRect.height = capture.height * zoom
        captureRect.x = rect.horzCenter() - captureRect.width * 0.5
        captureRect.y = rect.vertCenter() - captureRect.height * 0.5
        return captureRect
    }

    mtd setZoom(z: f32, mouseCenter = false)
    {
        var mousePos, mousePos1: Math.Point
        if mouseCenter
        {
            mousePos = getApp().mouse.getPosition()
            mousePos = screenToSurface(mousePos)
            mousePos = surfaceToLocal(mousePos)
            mousePos1 = mousePos + scrollPos
            mousePos1 /= zoom
        }

        zoom = Math.clamp(z, 0.1, 10.0)
        captureRect := getCaptureRect()
        scroll.setScrollSize(captureRect.width + (2*viewMargin), captureRect.height + (2*viewMargin))

        if mouseCenter
        {
            mousePos1 *= zoom
            mousePos1 -= mousePos
            scroll.setScrollPos(mousePos1.x, mousePos1.y)
        }

        invalidate()
    }

    mtd zoomToFit(canZoom = true)
    {
        rect := getClientRect()
        rect.inflate(-viewMargin*2)

        zx := rect.width / capture.width
        zy := rect.height / capture.height

        if !canZoom
        {
            zx = Math.min(zx, 1)
            zy = Math.min(zy, 1)
        }

        setZoom(Math.min(zx, zy))
    }

    mtd setCapture(c: *Capture)
    {
        if capture
            Memory.delete(capture)
        capture = c
        if capture
            capture.setup(self)
        invalidate()
    }
}
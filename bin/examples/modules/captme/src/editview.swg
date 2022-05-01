using Core, Gui, Pixel

struct EditWnd
{
    using scrollWnd: ScrollWnd
    editView:        *EditView
}

struct EditView
{
    using wnd:  Wnd
    scroll:     *ScrollWnd
    capture:    *Capture
    zoom:       f32 = 1
}

impl EditWnd
{
    func create(parent: *Wnd)->*EditWnd
    {
        scroll := Wnd.create'EditWnd(parent)
        view   := Wnd.create'EditView(scroll)
        view.scroll = scroll
        scroll.editView = view
        scroll.view = view

        // TEMP
        capture := Memory.new'Capture()
        capture.width = 400
        capture.height = 400
        form := Memory.new'ShapeForm()
        form.x, form.y = 50
        capture.forms.add(cast(IForm) form)
        view.setCapture(capture)
        // END TEMP

        return scroll
    }
}

impl IWnd for EditView
{
    mtd onMouseEvent(evt: *MouseEvent)
    {
        evt.accepted = false
        if !capture return

        if capture.gizmo.mouse(self, evt)
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
                zoom += evt.move.y > 0 ? 0.1 : -0.1
                setZoom(zoom)
                evt.accepted = true
            }
        }
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        painter := evt.bc.painter

        // Background
        rect := getClientRect()
        rect.offset(scrollPos)
        painter.fillRect(rect, getThemeColors().scrollBar_Bk)
        if !capture
            return

        // Capture
        rect = getCaptureRect()
        capture.paint(painter, rect)

        capture.gizmo.rect = rect
        capture.gizmo.paint(painter)
    }
}

impl EditView
{
    mtd setCapture(c: *Capture)
    {
        capture = c
        setZoom(1)

        capture.gizmo.sigChanged = @|self|(g) {
            /*if old.width != new.width
                self.capture.width = cast(s32) new.width
                self.invalidate()*/
        }
    }

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

    mtd setZoom(z: f32)
    {
        zoom = Math.clamp(z, 0.1, 2.0)
        captureRect := getCaptureRect()
        scroll.setScrollSize(captureRect.width + 64, captureRect.height + 64)
        invalidate()
    }
}
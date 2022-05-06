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

        form0 := Memory.new'RectShapeForm()
        form0.x, form0.y = 50
        form0.backColor = Argb.Green
        form0.setup(capture)
        capture.forms.add(cast(IForm) form0)

        form1 := Memory.new'RectShapeForm()
        form1.x, form1.y = 150
        form1.w = 100
        form1.backColor = Argb.Zero
        form1.setup(capture)
        capture.forms.add(cast(IForm) form1)

        form2 := Memory.new'RectShapeForm()
        form2.kind = .Rectangle
        form2.x, form2.y = 200
        form2.w, form2.h = 100
        form2.alpha = 50
        form2.backColor = Argb.Blue
        form2.borderSize = 10
        form2.setup(capture)
        capture.forms.add(cast(IForm) form2)

        form3 := Memory.new'RectShapeForm()
        form3.kind = .Rectangle
        form3.x = 100
        form3.y = 200
        form3.w, form3.h = 100
        form3.alpha = 50
        form3.backColor = Argb.Zero
        form3.borderSize = 10
        form3.setup(capture)
        capture.forms.add(cast(IForm) form3)

        view.setCapture(capture)
        // END TEMP

        return scroll
    }
}

impl IWnd for EditView
{
    mtd onKeyEvent(evt: *KeyEvent)
    {
        if evt.kind != .KeyPressed
            return
        ctrl := evt.modifiers & .Control

        switch evt.key
        {
        case Z:
            if ctrl and capture.canUndo()
            {
                capture.undo()
                invalidate()
            }
        case Y:
            if ctrl and capture.canRedo()
            {
                capture.redo()
                invalidate()
            }
        }
    }

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
    mtd setCapture(c: *Capture)
    {
        capture = c
        capture.setup(self)
        setZoom(2)
        invalidate()
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
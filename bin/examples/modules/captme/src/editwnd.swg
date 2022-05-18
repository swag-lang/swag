using Core, Gui, Pixel

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

        // TEMP
        var itf: IForm
        capture := Memory.new'Capture()
        capture.width = 400
        capture.height = 400

        forma := Memory.new'FormLine()
        forma.xa = 100
        forma.ya = 40
        forma.xb = 180
        forma.yb = 140
        forma.borderColor = Argb.Red
        forma.borderSize = 20
        itf = cast(IForm) forma
        itf.setup(capture)
        capture.forms.add(itf)

        formb := Memory.new'FormMultiLine()
        formb.points.add(@{25, 25})
        formb.points.add(@{125, 25})
        formb.points.add(@{75, 50})
        formb.borderColor = Argb.Purple
        itf = cast(IForm) formb
        itf.setup(capture)
        capture.forms.add(itf)

        form0 := Memory.new'FormRect()
        form0.x, form0.y = 50
        form0.backColor = Argb.Green
        itf = cast(IForm) form0
        itf.setup(capture)
        capture.forms.add(itf)

        form1 := Memory.new'FormRect()
        form1.x, form1.y = 150
        form1.w = 100
        form1.backColor = Argb.Zero
        itf = cast(IForm) form1
        itf.setup(capture)
        capture.forms.add(itf)

        form2 := Memory.new'FormRect()
        form2.kind = .Rectangle
        form2.x, form2.y = 200
        form2.w, form2.h = 100
        form2.alpha = 50
        form2.backColor = Argb.Blue
        form2.borderSize = 10
        itf = cast(IForm) form2
        itf.setup(capture)
        capture.forms.add(itf)

        form3 := Memory.new'FormRect()
        form3.kind = .RoundRectangle
        form3.x = 100
        form3.y = 200
        form3.w, form3.h = 100
        form3.alpha = 75
        form3.borderColor = Argb.Red
        form3.backColor = Argb.Zero
        form3.borderSize = 5
        itf = cast(IForm) form3
        itf.setup(capture)
        capture.forms.add(itf)

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
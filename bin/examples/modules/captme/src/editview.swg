using Core, Gui, Pixel

const CmdIdUndo         = "CmdIdUndo"
const CmdIdRedo         = "CmdIdRedo"
const CmdIdBringToFront = "CmdIdBringToFront"
const CmdIdSendToBack   = "CmdIdSendToBack"
const CmdIdBringForward = "CmdIdBringForward"
const CmdIdSendBackward = "CmdIdSendBackward"

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

        view.registerKeyShortcut(Input.KeyModifiers.Control, Input.Key.Z, CmdIdUndo)
        view.registerKeyShortcut(Input.KeyModifiers.Control, Input.Key.Y, CmdIdRedo)
        view.registerKeyShortcut(Input.KeyModifiers.CtrlShift, Input.Key.F, CmdIdBringForward)
        view.registerKeyShortcut(Input.KeyModifiers.CtrlShift, Input.Key.B, CmdIdSendBackward)

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
        form3.kind = .RoundRectangle
        form3.x = 100
        form3.y = 200
        form3.w, form3.h = 100
        form3.alpha = 75
        form3.borderColor = Argb.Red
        form3.backColor = Argb.Zero
        form3.borderSize = 5
        form3.setup(capture)
        capture.forms.add(cast(IForm) form3)

        view.setCapture(capture)
        // END TEMP

        return scroll
    }
}

impl IWnd for EditView
{
    mtd onCommandUIEvent(evt: *CommandUIEvent)
    {
        hasSel := capture and capture.selection.count

        switch evt.id
        {
        case CmdIdBringToFront:
            evt.setFlags |= .Disabled | .Icon
            evt.disabled = !hasSel or capture.selection.contains(capture.forms.count - 1)
            evt.icon = evt.target.getTheme().getIcon24(.BringToFront)
            evt.accepted = true

        case CmdIdSendToBack:
            evt.setFlags |= .Disabled | .Icon
            evt.disabled = !hasSel or capture.selection.contains(0)
            evt.icon = evt.target.getTheme().getIcon24(.SendToBack)
            evt.accepted = true

        case CmdIdBringForward:
            evt.setFlags |= .Disabled | .Icon | .Shortcut
            evt.disabled = !hasSel or capture.selection.contains(capture.forms.count - 1)
            evt.icon = evt.target.getTheme().getIcon24(.BringForward)
            evt.accepted = true

        case CmdIdSendBackward:
            evt.setFlags |= .Disabled | .Icon
            evt.disabled = !hasSel or capture.selection.contains(0)
            evt.icon = evt.target.getTheme().getIcon24(.SendBackward)
            evt.accepted = true

        case CmdIdUndo:
            evt.setFlags |= .Disabled
            evt.disabled = !capture or !capture.canUndo()
            evt.accepted = true

        case CmdIdRedo:
            evt.setFlags |= .Disabled
            evt.disabled = !capture or !capture.canRedo()
            evt.accepted = true
        }
    }

    mtd onCommandEvent(evt: *CommandEvent)
    {
        var undo: *Undo
        switch evt.id
        {
        case CmdIdBringToFront:
            undo = capture.newUndo(.MoveToFront)
            evt.accepted = true
        case CmdIdSendToBack:
            undo = capture.newUndo(.MoveToBack)
            evt.accepted = true
        case CmdIdBringForward:
            undo = capture.newUndo(.MoveForward)
            evt.accepted = true
        case CmdIdSendBackward:
            undo = capture.newUndo(.MoveBackward)
            evt.accepted = true
        case CmdIdUndo:
            capture.undo()
            evt.accepted = true
        case CmdIdRedo:
            capture.redo()
            evt.accepted = true
        }

        if undo
            capture.pushUndo(undo)
        if evt.accepted
            invalidate()
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
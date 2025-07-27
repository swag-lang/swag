using Core, Win32, Gdi32, Pixel

enum InPlaceEditMode
{
    Draw
    Line
    Circle
    Rect
    Arrow
    Text
    Marker
}

struct InPlaceEditWnd
{
    using wnd:            Wnd
    editWnd:              *EditWnd
    actionBar:            *IconBar
    editBar:              *IconBar
    capture:              *Capture
    newMain:              *MainWnd
    editMode:             InPlaceEditMode = .Arrow
    editColor:            Color = Argb.Red
    editColorHilight:     Color = Argb.Yellow
}

impl InPlaceEditWnd
{
    func create(capture: *Capture, main: *MainWnd, parent: *Wnd)->*InPlaceEditWnd
    {
        // Generate a main window
        let theme   = main.getTheme()
        let newMain = Wnd.create'MainWnd(main.parent, {})
        newMain.registerActions()
        newMain.registerShortcuts()
        newMain.inPlaceEdit = true

        with let frame = Wnd.create'InPlaceEditWnd(parent)
        {
            .owner             = newMain
            .capture           = capture
            .editWnd           = EditWnd.create(newMain, frame)
            .editWnd.dockStyle = .Center
            .newMain           = newMain
            newMain.editWnd    = .editWnd

            with let view = frame.editWnd.editView
            {
                .dockStyle  = .Center
                .viewMargin = 0
                .setCapture(capture)
                .setZoom(1)
                .setFocus()
                .inPlaceEdit = true
            }

            .actionBar = IconBar.create(.editWnd.editView, 24, {})
            .actionBar.addItem(theme.getIcon24(ThemeIcons24.Copy), "copy")
            .actionBar.addItem(theme.getIcon24(ThemeIcons24.Edit), "edit")
            .actionBar.addItem(theme.getIcon24(ThemeIcons24.WndClose), "close")
            .actionBar.sigPressed += func|frame|(btn)
            {
                switch btn.id
                {
                case "copy":
                    InPlaceEditWnd.cropCapture(frame.capture)
                    frame.capture.copyAll()
                    g_Exit     = true
                    g_GrabDone = false
                case "close":
                    g_Exit     = true
                    g_GrabDone = false
                case "edit":
                    InPlaceEditWnd.cropCapture(frame.capture)
                    g_Exit     = true
                    g_GrabDone = true
                }
            }

            .editBar = IconBar.create(.editWnd.editView, 24, {})
            .editBar.addCheckableItem(Icon.from(&main.icons48, 7), "draw")
            .editBar.addCheckableItem(Icon.from(&main.icons48, 1), "line")
            .editBar.addCheckableItem(Icon.from(&main.icons48, 19), "circle")
            .editBar.addCheckableItem(Icon.from(&main.icons48, 20), "rect")
            .editBar.addCheckableItem(Icon.from(&main.icons48, 3), "arrow")
            .editBar.addCheckableItem(Icon.from(&main.icons48, 4), "text")
            .editBar.addCheckableItem(Icon.from(&main.icons48, 9), "marker")
            .editBar.setIconBarFlags(.Vertical)

            .editBar.sigCheckChanged += func|frame|(btn)
            {
                switch btn.id
                {
                case "draw":
                    frame.editMode = .Draw
                case "line":
                    frame.editMode = .Line
                case "circle":
                    frame.editMode = .Circle
                case "rect":
                    frame.editMode = .Rect
                case "arrow":
                    frame.editMode = .Arrow
                case "text":
                    frame.editMode = .Text
                case "marker":
                    frame.editMode = .Marker
                }

                frame.updateEditMode()
            }

            let btnColor = .editBar.addItem({}, "color")
            btnColor.iconBtnFlags.add(.Popup)
            btnColor.sigPaintIcon = func|frame|(btn, bc, pos)
            {
                bc.painter.setParams(.Antialiased)
                var rect = pos
                rect.inflate(-2)
                var color = frame.editMode == .Marker ? frame.editColorHilight : frame.editColor
                bc.painter.fillRoundRect(rect, 10, 10, color)
            }

            btnColor.sigShowPopup += func|frame|(btn)
            {
                var color = frame.editMode == .Marker ? frame.editColorHilight : frame.editColor
                var picker = PropWnd.createColorPicker(cast(*IconButton) btn, color)
                picker.sigChanged += func|picker, frame, btn|(pk)
                {
                    if frame.editMode == .Marker:
                        frame.editColorHilight = picker.getColor()
                    else:
                        frame.editColor = picker.getColor()
                    frame.updateEditMode()
                    btn.invalidate()
                }
            }

           .actionBar.padding = 5
        }

        capture.gizmo.sigHasChanged = func|frame|()
        {
            frame.moveButtons()
        }

        frame.moveButtons()
        frame.updateEditMode()

        return frame
    }

    func cropCapture(capture: *Capture)
    {
        var grabRect: Math.Rectangle
        with capture
        {
            grabRect.x      = -.backImagePos.x
            grabRect.y      = -.backImagePos.y
            grabRect.width  = .width
            grabRect.height = .height

            let scale = Env.getDPIScale()
            grabRect.x *= scale
            grabRect.y *= scale
            grabRect.width *= scale
            grabRect.height *= scale

            .backImg.crop(grabRect)
            .backImgOrg = .backImg
            .width, .widthOrg = cast(s32) .backImg.width
            .height, .heightOrg = cast(s32) .backImg.height
            .backImagePos = {}

            .gizmo.sigHasChanged = null
            .inPlaceEdit         = false
            .dirtyBackImg        = true

            foreach f in capture.forms:
                f.scaleBaseForm(scale)

            .selection.clear()
        }
    }

    mtd updateEditMode()
    {
        var btn: *IconButton
        btn = assume editBar.getButton("draw")
        btn.setCheck(editMode == .Draw)
        btn = assume editBar.getButton("line")
        btn.setCheck(editMode == .Line)
        btn = assume editBar.getButton("circle")
        btn.setCheck(editMode == .Circle)
        btn = assume editBar.getButton("rect")
        btn.setCheck(editMode == .Rect)
        btn = assume editBar.getButton("arrow")
        btn.setCheck(editMode == .Arrow)
        btn = assume editBar.getButton("text")
        btn.setCheck(editMode == .Text)
        btn = assume editBar.getButton("marker")
        btn.setCheck(editMode == .Marker)

        with newMain
        {
            switch editMode
            {
            case Draw:
                .editMode                = .Draw
                .mdlFormDraw.borderSize  = 8
                .mdlFormDraw.borderColor = editColor
            case Line:
                .editMode                = .Line
                .mdlFormLine.borderSize  = 8
                .mdlFormLine.borderColor = editColor
            case Circle:
                .editMode                 = .Shape
                .mdlFormShape.kind        = .Ellipse
                .mdlFormShape.borderSize  = 8
                .mdlFormShape.borderColor = editColor
                .mdlFormShape.backColor   = 0
            case Rect:
                .editMode                 = .Shape
                .mdlFormShape.kind        = .RoundRectangle
                .mdlFormShape.borderSize  = 8
                .mdlFormShape.borderColor = editColor
                .mdlFormShape.backColor   = 0
            case Arrow:
                .editMode                 = .Arrow
                .mdlFormArrow.borderSize  = 8
                .mdlFormArrow.endSize     = 5
                .mdlFormArrow.endCap      = .ArrowAnchor
                .mdlFormArrow.borderColor = editColor
            case Text:
                .editMode             = .Text
                .mdlFormText.fontSize = 48
                .mdlFormText.text     = "A"
            case Marker:
                .editMode                  = .Draw
                .mdlFormDraw.borderSize    = 30
                .mdlFormDraw.borderColor   = editColorHilight
                .mdlFormDraw.borderColor.a = 160
            }
        }
    }

    mtd moveButtons()
    {
        var rect: Math.Rectangle
        rect.x      = -capture.backImagePos.x
        rect.y      = -capture.backImagePos.y
        rect.width  = capture.width
        rect.height = capture.height

        {
            const H = 40.0
            let w   = actionBar.position.width

            let x = Math.max(10, rect.right() - w)
            if rect.bottom() < g_ScreenHeight - H:
                actionBar.setPosition(x, rect.bottom(), w, H)
            elif rect.y > H:
                actionBar.setPosition(x, rect.y - H, w, H)
            else:
                actionBar.setPosition(x, rect.bottom() - H, w, H)
        }

        {
            let h   = editBar.position.height
            const W = 40.0

            if rect.right() < g_ScreenWidth - W:
                editBar.setPosition(rect.right(), rect.bottom() - h, W, h)
            elif rect.x > W:
                editBar.setPosition(rect.x - W, rect.bottom() - h, W, h)
            else:
                editBar.setPosition(rect.right() - W, rect.bottom() - h, W, h)
        }
    }
}

impl IWnd for InPlaceEditWnd
{
    mtd impl onKeyEvent(evt: *KeyEvent)
    {
        evt.accepted = false
        if evt.kind == .KeyPressed and evt.key == .Escape:
            g_Exit = true
    }
}

#global marked
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

            .actionBar.addItem(theme.getIcon24(ThemeIcons24.Save), "save").toolTip      = "Save capture as image file"
            .actionBar.addItem(theme.getIcon24(ThemeIcons24.Copy), "copy").toolTip      = "Copy image to clipboard"
            .actionBar.addItem(theme.getIcon24(ThemeIcons24.Edit), "edit").toolTip      = "Open in Captme editor"
            .actionBar.addItem(theme.getIcon24(ThemeIcons24.WndClose), "close").toolTip = "Cancel and discard capture"

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
                case "save":
                    let fileName = frame.newMain.geteSaveAsImageName()
                    if fileName.isEmpty():
                        return
                    InPlaceEditWnd.cropCapture(frame.capture)
                    frame.newMain.doSaveAsImage(fileName)
                    g_Exit     = true
                    g_GrabDone = false
                }
            }

            .editBar = IconBar.create(.editWnd.editView, 24, {})

            .editBar.addCheckableItem(Icon.from(&main.icons48, 7), "draw").toolTip    = "Free draw tool"
            .editBar.addCheckableItem(Icon.from(&main.icons48, 1), "line").toolTip    = "Draw straight line"
            .editBar.addCheckableItem(Icon.from(&main.icons48, 19), "circle").toolTip = "Draw circle or ellipse"
            .editBar.addCheckableItem(Icon.from(&main.icons48, 20), "rect").toolTip   = "Draw rectangle or square"
            .editBar.addCheckableItem(Icon.from(&main.icons48, 3), "arrow").toolTip   = "Draw arrow"
            .editBar.addCheckableItem(Icon.from(&main.icons48, 4), "text").toolTip    = "Add text annotation"
            .editBar.addCheckableItem(Icon.from(&main.icons48, 9), "marker").toolTip  = "Highlight with marker"
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
                var color  = frame.editMode == .Marker ? frame.editColorHilight : frame.editColor
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
            if .backImgOriginal.isValid():
                .backImgOriginal = .backImg
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
        with InPlaceEditMode
        {
            foreach m in [{"draw", .Draw}, {"line", .Line}, {"circle", .Circle}, {"rect", .Rect}, {"arrow", .Arrow}, {"text", .Text}, {"marker", .Marker}]
            {
                let btn = assume me.editBar.getButton(m.item0)
                btn.setCheck(me.editMode == m.item1)
            }
        }

        with me.newMain
        {
            switch me.editMode
            {
            case Draw:
                .editMode                = .Draw
                .mdlFormDraw.borderSize  = 8
                .mdlFormDraw.borderColor = me.editColor
            case Line:
                .editMode                = .Line
                .mdlFormLine.borderSize  = 8
                .mdlFormLine.borderColor = me.editColor
            case Circle:
                .editMode                 = .Shape
                .mdlFormShape.kind        = .Ellipse
                .mdlFormShape.borderSize  = 8
                .mdlFormShape.borderColor = me.editColor
                .mdlFormShape.backColor   = 0
            case Rect:
                .editMode                 = .Shape
                .mdlFormShape.kind        = .RoundRectangle
                .mdlFormShape.borderSize  = 8
                .mdlFormShape.borderColor = me.editColor
                .mdlFormShape.backColor   = 0
            case Arrow:
                .editMode                 = .Arrow
                .mdlFormArrow.borderSize  = 8
                .mdlFormArrow.endSize     = 5
                .mdlFormArrow.endCap      = .ArrowAnchor
                .mdlFormArrow.borderColor = me.editColor
            case Text:
                .editMode               = .Text
                .mdlFormText.fontSize   = 40
                .mdlFormText.borderSize = 3
                .mdlFormText.text       = "A"
                .mdlFormText.boldSize   = 2
                .mdlFormText.backColor  = me.editColor
            case Marker:
                .editMode                = .Draw
                .mdlFormDraw.borderSize  = 30
                .mdlFormDraw.borderColor = me.editColorHilight.withAlpha(160)
            }
        }
    }

    mtd moveButtons()
    {
        var rect: Math.Rectangle{-me.capture.backImagePos.x, -me.capture.backImagePos.y, me.capture.width, me.capture.height}

        {
            const H = 40.0
            let w   = me.actionBar.position.width

            let x = Math.max(10, rect.right() - w)
            if rect.bottom() < g_ScreenHeight - H:
                me.actionBar.setPosition(x, rect.bottom(), w, H)
            elif rect.y > H:
                me.actionBar.setPosition(x, rect.y - H, w, H)
            else:
                me.actionBar.setPosition(x, rect.bottom() - H, w, H)
        }

        {
            let h   = me.editBar.position.height
            const W = 40.0

            if rect.right() < g_ScreenWidth - W:
                me.editBar.setPosition(rect.right(), rect.bottom() - h, W, h)
            elif rect.x > W:
                me.editBar.setPosition(rect.x - W, rect.bottom() - h, W, h)
            else:
                me.editBar.setPosition(rect.right() - W, rect.bottom() - h, W, h)
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

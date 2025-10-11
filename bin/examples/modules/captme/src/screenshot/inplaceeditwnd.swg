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
        with let frame = createMainFrame(main, parent, capture)
        {
            .createActionBar()
            .createEditBar(main)
            .setupCapture(capture)
        }

        return frame
    }

    // Private helper methods
    func createMainFrame(main: *MainWnd, parent: *Wnd, capture: *Capture)->*InPlaceEditWnd
    {
        let newMain = Wnd.create'MainWnd(main.parent, {})
        newMain.registerActions()
        newMain.registerShortcuts()
        newMain.inPlaceEdit = true

        with let frame = Wnd.create'InPlaceEditWnd(parent)
        {
            .owner   = newMain
            .capture = capture
            .newMain = newMain

            .editWnd           = EditWnd.create(newMain, frame)
            .editWnd.dockStyle = .Center
            newMain.editWnd    = .editWnd

            with let view = frame.editWnd.editView
            {
                .dockStyle   = .Center
                .viewMargin  = 0
                .inPlaceEdit = true
                .setCapture(capture)
                .setZoom(1)
                .setFocus()
            }
        }

        return frame
    }

    mtd createActionBar()
    {
        .actionBar                 = IconBar.create(.editWnd.editView, 24, {})
        .actionBar.backgroundStyle = .Window

        let actions = [
            {"save", ThemeIcons24.Save, "Save capture as image file"},
            {"copy", ThemeIcons24.Copy, "Copy image to clipboard"},
            {"edit", ThemeIcons24.Edit, "Open in Captme editor"},
            {"close", ThemeIcons24.WndClose, "Cancel and discard capture"}]

        let theme = .getTheme()
        foreach action in actions do
            .actionBar.addItem(theme.getIcon24(action.item1), action.item0).toolTip = action.item2

        .actionBar.sigPressed += mtd||(btn) { .handleActionBarPress(btn.id); }
    }

    mtd createEditBar(main: *MainWnd)
    {
        .editBar                 = IconBar.create(.editWnd.editView, 24, {})
        .editBar.backgroundStyle = .Window
        .editBar.setIconBarFlags(.Vertical)

        let editTools = [
            {"draw", 7, "Free draw tool"},
            {"line", 1, "Draw straight line"},
            {"circle", 19, "Draw circle or ellipse"},
            {"rect", 20, "Draw rectangle or square"},
            {"arrow", 3, "Draw arrow"},
            {"text", 4, "Add text annotation"},
            {"marker", 9, "Highlight with marker"}]

        foreach tool in editTools do
            .editBar.addCheckableItem(Icon.from(&main.icons48, tool.item1), tool.item0).toolTip = tool.item2

        .editBar.sigCheckChanged += mtd||(btn) { .handleEditModeChange(btn.id); }
        .createColorPicker()
    }

    mtd createColorPicker()
    {
        let btnColor = .editBar.addItem({}, "color")
        btnColor.iconBtnFlags.add(.Popup)

        btnColor.sigPaintIcon = mtd||(btn, bc, pos, flags)
        {
            bc.painter.setParams(.Antialiased)
            var rect = pos
            rect.inflate(-2)
            let color = .editMode == .Marker ? .editColorHilight : .editColor
            bc.painter.fillRoundRect(rect, 10, 10, color)
        }

        btnColor.sigShowPopup += mtd||(btn)
        {
            let color  = .editMode == .Marker ? .editColorHilight : .editColor
            let picker = PropWnd.createColorPicker(cast(*IconButton) btn, color)
            picker.sigChanged += mtd|picker, btn|(pk)
            {
                if .editMode == .Marker do
                    .editColorHilight = picker.getColor()
                else do
                    .editColor = picker.getColor()
                .updateEditMode()
                btn.invalidate()
            }
        }
    }

    mtd setupCapture(capture: *Capture)
    {
        capture.gizmo.sigHasChanged = mtd||() { .moveButtons(); }
        .moveButtons()
        .updateEditMode()
    }

    mtd handleActionBarPress(actionId: string)
    {
        switch actionId
        {
        case "copy":
            .capture.crop()
            .capture.copyAll()
            exitWithResult(false)
        case "close":
            exitWithResult(false)
        case "edit":
            .capture.crop()
            exitWithResult(true)
        case "save":
            let fileName = .newMain.getSaveAsImageName()
            if !fileName.isEmpty() do
                .capture.crop()
            .newMain.doSaveAsImage(fileName)
            exitWithResult(false)
        }
    }

    mtd handleEditModeChange(toolId: string)
    {
        switch toolId
        {
        case "draw":
            .editMode = .Draw
        case "line":
            .editMode = .Line
        case "circle":
            .editMode = .Circle
        case "rect":
            .editMode = .Rect
        case "arrow":
            .editMode = .Arrow
        case "text":
            .editMode = .Text
        case "marker":
            .editMode = .Marker
        }

        .updateEditMode()
    }

    func exitWithResult(grabDone: bool)
    {
        g_Exit     = true
        g_GrabDone = grabDone
    }

    func crop(capture: *Capture)
    {
        let grabRect = calculateGrabRect(capture)
        capture.applyScaling(grabRect)
        capture.updateCaptureState()
    }

    func calculateGrabRect(capture: *Capture)->Math.Rectangle
    {
        return Math.Rectangle{-capture.backImagePos.x, -capture.backImagePos.y, capture.width, capture.height}
    }

    func applyScaling(capture: *Capture, rect: Math.Rectangle)
    {
        var grabRect = rect
        let scale    = Env.getDPIScale()
        grabRect.x *= scale
        grabRect.y *= scale
        grabRect.width *= scale
        grabRect.height *= scale

        capture.backImg.crop(grabRect)
        if capture.backImgOriginal.isValid() do
            capture.backImgOriginal = capture.backImg

        foreach f in capture.forms do
            f.scaleBaseForm(scale)
    }

    func updateCaptureState(capture: *Capture)
    {
        with capture
        {
            .width, .widthOrg = cast(s32) .backImg.width
            .height, .heightOrg = cast(s32) .backImg.height
            .backImagePos        = {}
            .gizmo.sigHasChanged = null
            .inPlaceEdit         = false
            .dirtyBackImg        = true
            .selection.clear()
        }
    }

    mtd updateEditMode()
    {
        .updateEditBarButtons()
        .configureMainWindowEditMode()
    }

    mtd updateEditBarButtons()
    {
        with InPlaceEditMode
        {
            let modeMap = [
                {"draw", .Draw}, {"line", .Line}, {"circle", .Circle}, {"rect", .Rect},
                {"arrow", .Arrow}, {"text", .Text}, {"marker", .Marker}]
        }

        foreach m in modeMap
        {
            let btn = assume .editBar.getButton(m.item0)
            btn.setCheck(.editMode == m.item1)
        }
    }

    mtd configureMainWindowEditMode()
    {
        const BORDER_SIZE    = 8
        const MARKER_SIZE    = 30
        const TEXT_SIZE      = 40
        const TEXT_BOLD      = 2
        const ARROW_END_SIZE = 5

        with me.newMain
        {
            switch me.editMode
            {
            case Draw:
                .editMode                = .Draw
                .mdlFormDraw.borderSize  = BORDER_SIZE
                .mdlFormDraw.borderColor = me.editColor

            case Line:
                .editMode                = .Line
                .mdlFormLine.borderSize  = BORDER_SIZE
                .mdlFormLine.borderColor = me.editColor

            case Circle:
                .editMode                 = .Shape
                .mdlFormShape.kind        = .Ellipse
                .mdlFormShape.borderSize  = BORDER_SIZE
                .mdlFormShape.borderColor = me.editColor
                .mdlFormShape.backColor   = 0

            case Rect:
                .editMode                 = .Shape
                .mdlFormShape.kind        = .RoundRectangle
                .mdlFormShape.borderSize  = BORDER_SIZE
                .mdlFormShape.borderColor = me.editColor
                .mdlFormShape.backColor   = 0

            case Arrow:
                .editMode                 = .Arrow
                .mdlFormArrow.borderSize  = BORDER_SIZE
                .mdlFormArrow.endSize     = ARROW_END_SIZE
                .mdlFormArrow.endCap      = .ArrowAnchor
                .mdlFormArrow.borderColor = me.editColor

            case Text:
                .editMode               = .Text
                .mdlFormText.fontSize   = TEXT_SIZE
                .mdlFormText.borderSize = 3
                .mdlFormText.text       = "A"
                .mdlFormText.boldSize   = TEXT_BOLD
                .mdlFormText.backColor  = me.editColor

            case Marker:
                .editMode                = .Draw
                .mdlFormDraw.borderSize  = MARKER_SIZE
                .mdlFormDraw.borderColor = me.editColorHilight.withAlpha(160)
            }
        }
    }

    mtd moveButtons()
    {
        let rect = Math.Rectangle{-.capture.backImagePos.x, -.capture.backImagePos.y, .capture.width, .capture.height}
        .positionActionBar(rect)
        .positionEditBar(rect)
    }

    mtd positionActionBar(rect: Math.Rectangle)
    {
        const MARGIN = 4
        const HEIGHT = 40.0
        let width    = .actionBar.position.width
        let x        = Math.max(10, rect.right() - width)

        if rect.bottom() < g_ScreenHeight - HEIGHT + MARGIN do
            .actionBar.setPosition(x, rect.bottom() + MARGIN, width, HEIGHT)
        elif rect.y > HEIGHT + MARGIN do
            .actionBar.setPosition(x, rect.y - HEIGHT - MARGIN, width, HEIGHT)
        else do
            .actionBar.setPosition(x, rect.bottom() - HEIGHT - MARGIN, width, HEIGHT)
    }

    mtd positionEditBar(rect: Math.Rectangle)
    {
        const MARGIN = 4
        const WIDTH  = 40.0
        let height   = .editBar.position.height

        if rect.right() < g_ScreenWidth - WIDTH + MARGIN do
            .editBar.setPosition(rect.right() + MARGIN, rect.bottom() - height, WIDTH, height)
        elif rect.x > WIDTH + MARGIN do
            .editBar.setPosition(rect.x - WIDTH - MARGIN, rect.bottom() - height, WIDTH, height)
        else do
            .editBar.setPosition(rect.right() - WIDTH - MARGIN, rect.bottom() - height - MARGIN, WIDTH, height)
    }
}

impl IWnd for InPlaceEditWnd
{
    mtd impl onKeyEvent(evt: *KeyEvent)
    {
        evt.accepted = false
        if evt.kind == .KeyPressed and evt.key == .Escape do
            g_Exit = true
    }
}

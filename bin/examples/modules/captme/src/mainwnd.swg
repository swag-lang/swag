enum EditMode
{
    Shape
    Arrow
    Line
    Polygon
    Text
    Favorites
    Draw
}

enum BottomSize
{
    Small
    Medium
    Big
}

struct MainWnd
{
    using wnd: Wnd

    icons24:    ImageList
    icons48:    ImageList
    toolbar:    *Wnd
    editWnd:    *EditWnd
    quickWnd:   *QuickWnd
    propWnd:    *PropWnd
    clipFmt:    Clipboard.Format
    captureBtn: *IconButton
    zoomBtn:    *IconButton
    recentWnd:  *RecentWnd
    bottomBar:  *Wnd
    bottomSizeBar: *IconBar

    editMode:   EditMode = .Shape
    bottomSize: BottomSize = Medium
    snap:       bool

    mdlFormShape:   FormShape
    mdlFormArrow:   FormArrow
    mdlFormLine:    FormLine
    mdlFormPolygon: FormPolygon
    mdlFormText:    FormText
    mdlFormDraw:    FormDraw
    favoriteMdl:    IForm
}

const Icons24 = #include("datas/icons24.png")
const Icons48 = #include("datas/icons48.png")

impl IWnd for MainWnd
{
    mtd onSerializeStateEvent(evt: *SerializeStateEvent)
    {
        catch evt.ser.serialize("MainWnd.editMode", &editMode)
        catch evt.ser.serialize("MainWnd.snap", &snap)
        catch evt.ser.serialize("MainWnd.bottomSize", &bottomSize)
        if evt.ser.isReading()
        {
            setBottomSize(bottomSize)
            updateState()
        }
    }

    mtd onCommandUIEvent(evt: *CommandUIEvent)
    {
        editView := editWnd  ? cast(*EditView) editWnd.view : null
        capture  := editView ? editView.capture : null

        hasSel       := capture and capture.selection.count
        hasSingleSel := capture and capture.selection.count == 1

        switch evt.id
        {
        case CmdIdResetQuickStyle:
            evt.setFlags |= .Name
            evt.name = "Restore default styles"
            evt.accepted = true

        case CmdIdRecentSmall:
            evt.setFlags |= .Name | .ToolTip | .Checked
            //evt.name = "Small Recent View"
            evt.toolTip = "Small recent view"
            evt.checked = bottomSize == .Small
            evt.accepted = true

        case CmdIdRecentMedium:
            evt.setFlags |= .Name | .ToolTip | .Checked
            //evt.name = "Medium Recent View"
            evt.toolTip = "Medium recent view"
            evt.checked = bottomSize == .Medium
            evt.accepted = true

        case CmdIdRecentBig:
            evt.setFlags |= .Name | .ToolTip | .Checked
            //evt.name = "Big Recent View"
            evt.toolTip = "Big recent view"
            evt.checked = bottomSize == .Big
            evt.accepted = true

        case CmdIdDeleteCapture:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Delete"
            evt.disabled = !capture
            evt.accepted = true

        case CmdIdFileLocation:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Open File Location"
            evt.disabled = !capture
            evt.accepted = true

        case CmdIdZoom1:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Actual Size"
            evt.disabled = !capture
            evt.accepted = true

        case CmdIdZoomFit:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Fit To View"
            evt.disabled = !capture
            evt.accepted = true

        case CmdIdZoomIn:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Zoom In"
            evt.disabled = !capture or !editWnd.editView.canZoomIn()
            evt.accepted = true

        case CmdIdZoomOut:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Zoom Out"
            evt.disabled = !capture or !editWnd.editView.canZoomOut()
            evt.accepted = true

        case CmdIdDupCapture:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Duplicate"
            evt.disabled = !capture
            evt.accepted = true

        case CmdIdSnap:
            evt.setFlags |= .Name | .Checked
            evt.name = "Snap To Forms"
            evt.checked = snap
            evt.accepted = true

        case CmdIdCapture:
            evt.setFlags |= .Name | .ToolTip
            evt.name = "Capture"
            evt.toolTip = "Start a new capture"
            evt.accepted = true

        case CmdIdSelectAll:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture
            evt.name = "Select All Objects"
            evt.accepted = true

        case CmdIdDeSelectAll:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture or !hasSel
            evt.name = "Deselect All Objects"
            evt.accepted = true

        case CmdIdInvertSelection:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture
            evt.name = "Invert Selection"
            evt.accepted = true

        case CmdIdCopy:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !hasSel
            evt.name = "Copy"
            evt.accepted = true

        case CmdIdPaste:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture or !Clipboard.hasFormat(clipFmt)
            evt.name = "Paste"
            evt.accepted = true

        case CmdIdCopyAsImage:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = capture == null
            evt.name = "Copy As Image"
            evt.accepted = true

        case CmdIdDeleteSelection:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !hasSel
            evt.name = "Delete"
            evt.accepted = true

        case CmdIdModeShape:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon = Icon.from(&icons48, 0)
            evt.name = "Shape"
            evt.toolTip = "Add circles and rectangle to the capture"
            evt.checked = editMode == .Shape
            evt.accepted = true

        case CmdIdModeArrow:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon = Icon.from(&icons48, 3)
            evt.name = "Arrow"
            evt.toolTip = "Add arrows to the capture"
            evt.checked = editMode == .Arrow
            evt.accepted = true

        case CmdIdModeLine:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon = Icon.from(&icons48, 1)
            evt.name = "Line"
            evt.toolTip = "Add lines to the capture"
            evt.checked = editMode == .Line
            evt.accepted = true

        case CmdIdModePolygon:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon = Icon.from(&icons48, 2)
            evt.name = "Polygon"
            evt.toolTip = "Add polygons to the capture"
            evt.checked = editMode == .Polygon
            evt.accepted = true

        case CmdIdModeText:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon = Icon.from(&icons48, 4)
            evt.name = "Text"
            evt.toolTip = "Add some text to the capture"
            evt.checked = editMode == .Text
            evt.accepted = true

        case CmdIdModeDraw:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon = Icon.from(&icons48, 7)
            evt.name = "Draw"
            evt.toolTip = "Add some free hand paint to the capture"
            evt.checked = editMode == .Draw
            evt.accepted = true

        case CmdIdModeFavorites:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon = Icon.from(&icons48, 5)
            evt.name = "Favorites"
            evt.toolTip = "Save multiple quick styles in one single location"
            evt.checked = editMode == .Favorites
            evt.accepted = true

        case CmdIdAlignLeft:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Left"
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignCenter:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Center"
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignRight:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Right"
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignTop:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Top"
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignMiddle:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Middle"
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignBottom:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Bottom"
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdBringToFront:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name = "Bring To Front"
            evt.disabled = !hasSel or capture.selection.contains(capture.forms.count - 1)
            evt.icon = evt.target.getTheme().getIcon24(.BringToFront)
            evt.accepted = true

        case CmdIdSendToBack:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name = "Send To Back"
            evt.disabled = !hasSel or capture.selection.contains(0)
            evt.icon = evt.target.getTheme().getIcon24(.SendToBack)
            evt.accepted = true

        case CmdIdBringForward:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name = "Bring Forward"
            evt.disabled = !hasSel or capture.selection.contains(capture.forms.count - 1)
            evt.icon = evt.target.getTheme().getIcon24(.BringForward)
            evt.accepted = true

        case CmdIdSendBackward:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name = "Send Backward"
            evt.disabled = !hasSel or capture.selection.contains(0)
            evt.icon = evt.target.getTheme().getIcon24(.SendBackward)
            evt.accepted = true

        case CmdIdUndo:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Undo"
            evt.disabled = !capture or !capture.canUndo()
            evt.accepted = true

        case CmdIdRedo:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Redo"
            evt.disabled = !capture or !capture.canRedo()
            evt.accepted = true
        }
    }

    mtd onCommandEvent(evt: *CommandEvent)
    {
        editView := cast(*EditView) editWnd.view
        capture  := editView.capture

        var undo: *Undo
        switch evt.id
        {
        case CmdIdResetQuickStyle:
            quickWnd.resetQuickStyles()

        case CmdIdRecentSmall:
            setBottomSize(.Small)

        case CmdIdRecentMedium:
            setBottomSize(.Medium)

        case CmdIdRecentBig:
            setBottomSize(.Big)

        case CmdIdDeleteCapture:
            res := MessageDlg.yesNo(surface, "This will delete the capture file.\nAre you sure ?")
            if res == MessageDlg.BtnYes
                recentWnd.removeRecent()

        case CmdIdFileLocation:
            File.locateInExplorer(getCapture().file)

        case CmdIdZoom1:
            editWnd.editView.setZoom(1)

        case CmdIdZoomFit:
            editWnd.editView.zoomToFit()

        case CmdIdZoomIn:
            editWnd.editView.zoomIn()
        case CmdIdZoomOut:
            editWnd.editView.zoomOut()

        case CmdIdDupCapture:
            file := Library.newFileName()
            catch File.duplicate(file, capture.file)
            if @err break
            dupCapture := catch recentWnd.addRecent(file)
            if @err break
            Memory.delete(dupCapture)
            recentWnd.recentView.select(0)

        case CmdIdSnap:
            snap = !snap

        case CmdIdCapture:
            captureScreen()

        case CmdIdSelectAll:
            capture.selection.clear()
            loop capture.forms
                capture.selection.add(@index)
            capture.selectionHasChanged()

        case CmdIdDeSelectAll:
            capture.selection.clear()
            capture.selectionHasChanged()

        case CmdIdInvertSelection:
            var newSel: Array'uint
            loop i: capture.forms
            {
                if !capture.selection.contains(i)
                    newSel.add(i)
            }
            capture.selection = newSel
            capture.selectionHasChanged()

        case CmdIdCopy:
            capture.copy()
        case CmdIdPaste:
            capture.paste()

        case CmdIdCopyAsImage:
            capture.copyAll()

        case CmdIdDeleteSelection:
            undo = capture.newUndo(.DeleteSelection)
            evt.accepted = true

        case CmdIdModeShape:
            editMode = .Shape
            evt.accepted = true
            updateState()

        case CmdIdModeArrow:
            editMode = .Arrow
            evt.accepted = true
            updateState()

        case CmdIdModeLine:
            editMode = .Line
            evt.accepted = true
            updateState()

        case CmdIdModePolygon:
            editMode = .Polygon
            evt.accepted = true
            updateState()

        case CmdIdModeText:
            editMode = .Text
            evt.accepted = true
            updateState()

        case CmdIdModeDraw:
            editMode = .Draw
            evt.accepted = true
            updateState()

        case CmdIdModeFavorites:
            editMode = .Favorites
            evt.accepted = true
            updateState()

        case CmdIdAlignLeft:
            undo = capture.newUndo(.AlignLeft)
            evt.accepted = true

        case CmdIdAlignCenter:
            undo = capture.newUndo(.AlignCenter)
            evt.accepted = true

        case CmdIdAlignRight:
            undo = capture.newUndo(.AlignRight)
            evt.accepted = true

        case CmdIdAlignTop:
            undo = capture.newUndo(.AlignTop)
            evt.accepted = true

        case CmdIdAlignMiddle:
            undo = capture.newUndo(.AlignMiddle)
            evt.accepted = true

        case CmdIdAlignBottom:
            undo = capture.newUndo(.AlignBottom)
            evt.accepted = true

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
            capture.selectionHasChanged()

        case CmdIdRedo:
            capture.redo()
            evt.accepted = true
            capture.selectionHasChanged()
        }

        if undo
            capture.pushUndo(undo)
        if evt.accepted
            invalidate()
    }
}

impl MainWnd
{
    mtd getEditMode()->EditMode
    {
        if editMode == .Favorites
        {
            #[Swag.Complete]
            switch @kindof(favoriteMdl)
            {
            case FormShape:     return EditMode.Shape
            case FormArrow:     return EditMode.Arrow
            case FormLine:      return EditMode.Line
            case FormPolygon:   return EditMode.Polygon
            case FormText:      return EditMode.Text
            case FormDraw:      return EditMode.Draw
            }
        }

        return editMode
    }

    mtd createTopMenu()
    {
        topMenu := MenuCtrl.createBar(parent)
        topMenu.owner = self
        topMenu.bringToFront()
        topMenu.dockStyle = .Top
        topMenu.margin.y = -parent.padding.y
        topMenu.backgroundStyle = .Transparent

        editMenu := MenuCtrl.createPopup(parent)
        editMenu.owner = self
        editMenu.addItem(CmdIdUndo)
        editMenu.addItem(CmdIdRedo)
        editMenu.addSeparator()
        editMenu.addItem(CmdIdCopy)
        editMenu.addItem(CmdIdPaste)
        editMenu.addItem(CmdIdCopyAsImage)
        editMenu.addSeparator()
        editMenu.addItem(CmdIdSelectAll)
        editMenu.addItem(CmdIdDeSelectAll)
        editMenu.addItem(CmdIdInvertSelection)
        topMenu.addPopup("Edit", editMenu)

        displayMenu := MenuCtrl.createPopup(parent)
        displayMenu.owner = self
        displayMenu.addItem(CmdIdZoomIn)
        displayMenu.addItem(CmdIdZoomOut)
        displayMenu.addItem(CmdIdZoom1)
        displayMenu.addItem(CmdIdZoomFit)
        topMenu.addPopup("View", displayMenu)
    }

    mtd createToolbar(topBar: *Wnd)
    {
        mtd createBtn(id: WndId)
        {
            b := IconButton.create(toolbar, position: @{0, 0, 60, 64})
            b.id = #self.id
            b.style.addStyleSheetColors("btnIcon_CheckedBk    $btnIcon_HotBk")
            b.style.addStyleSheetColors("btnIcon_CheckedHotBk $btnIcon_HotBk")
            b.iconBtnFlags |= .Checkable
            b.checkedForm = .Bottom
        }

        toolbar = StackLayoutCtrl.create(topBar, .HorzCenter, @{0, 0, 64, 64})
        toolbar.dockStyle = .Top
        toolbar.backgroundStyle = .Window
        toolbar.style.addStyleSheetColors("wnd_Bk $wnd_Caption")
        createBtn(CmdIdModeFavorites)
        createBtn(CmdIdModeShape)
        createBtn(CmdIdModeArrow)
        createBtn(CmdIdModeLine)
        createBtn(CmdIdModePolygon)
        createBtn(CmdIdModeText)
        createBtn(CmdIdModeDraw)
        toolbar.updateCommandState()
    }

    mtd setBottomSize(bottomSize: BottomSize)
    {
        self.bottomSize = #self.bottomSize
        switch self.bottomSize
        {
        case Small:
            bottomBar.resize(0, 80)
        case Medium:
            bottomBar.resize(0, 128)
        case Big:
            bottomBar.resize(0, 224)
        }

        applyLayout(@{position.width, position.height})
    }

    mtd registerShortcuts()
    {
        registerKeyShortcut(Input.KeyModifiers.Control,   Input.Key.PrintScreen,    CmdIdCapture)
        registerKeyShortcut(Input.KeyModifiers.Control,   Input.Key.C,              CmdIdCopy)
        registerKeyShortcut(Input.KeyModifiers.Control,   Input.Key.V,              CmdIdPaste)
        registerKeyShortcut(Input.KeyModifiers.Control,   Input.Key.Insert,         CmdIdCopy)
        registerKeyShortcut(Input.KeyModifiers.Shift,     Input.Key.Insert,         CmdIdPaste)
        registerKeyShortcut(Input.KeyModifiers.Control,   Input.Key.Z,              CmdIdUndo)
        registerKeyShortcut(Input.KeyModifiers.Control,   Input.Key.Y,              CmdIdRedo)
        registerKeyShortcut(Input.KeyModifiers.CtrlShift, Input.Key.F,              CmdIdBringForward)
        registerKeyShortcut(Input.KeyModifiers.CtrlShift, Input.Key.B,              CmdIdSendBackward)
        registerKeyShortcut(Input.KeyModifiers.None,      Input.Key.Delete,         CmdIdDeleteSelection)
        registerKeyShortcut(Input.KeyModifiers.CtrlShift, Input.Key.A,              CmdIdCopyAsImage)
        registerKeyShortcut(Input.KeyModifiers.Control,   Input.Key.A,              CmdIdSelectAll)
        registerKeyShortcut(Input.KeyModifiers.Control,   Input.Key.D,              CmdIdDeSelectAll)
        registerKeyShortcut(Input.KeyModifiers.Control,   Input.Key.Add,            CmdIdZoomIn)
        registerKeyShortcut(Input.KeyModifiers.Control,   Input.Key.Subtract,       CmdIdZoomOut)
        registerKeyShortcut(Input.KeyModifiers.Control,   Input.Key.NumPad0,        CmdIdZoom1)
        registerKeyShortcut(Input.KeyModifiers.CtrlShift, Input.Key.D,              CmdIdDupCapture)
        registerKeyShortcut(Input.KeyModifiers.CtrlShift, Input.Key.I,              CmdIdInvertSelection)
    }

    mtd updateState()
    {
        captureBtn.updateCommandState()
        toolbar.updateCommandState()
        bottomSizeBar.updateCommandState()
        propWnd.selectionHasChanged()
        quickWnd.updateQuickStyles()
    }

    mtd getCapture()->*Capture
    {
        return editWnd.editView.capture
    }

    mtd captureScreen()
    {
        surface.hide()
        img := catch screenShot()
        surface.show()
        if @err or img.width == 0 or img.height == 0
            return

        capture := Memory.new'Capture()
        capture.changed = true
        capture.width = img.width
        capture.height = img.height
        capture.backImg = img
        capture.file = Library.newFileName()
        editWnd.editView.setCapture(capture)
        editWnd.editView.zoomToFit(false)

        capture.computePreview()
        recentWnd.addRecent(capture.file, capture.preview)
        recentWnd.recentView.selected = 0
        capture.save()
    }

    func create(parent: *Wnd)->*MainWnd
    {
        res := Wnd.create'MainWnd(parent, "captme.mainwnd")
        renderer := parent.getApp().getRenderer()

        img24 := assume Image.decode(".png", Icons24)
        res.icons24.set(renderer.addImage(img24), 24, 24)

        img48 := assume Image.decode(".png", Icons48)
        res.icons48.set(renderer.addImage(img48), 48, 48)

        res.clipFmt = catch Clipboard.registerFormat(ClipboardFormat)

        topBar := Wnd.create'Wnd(res, @{0, 0, 64, 64})
        topBar.dockStyle = .Top
        topBar.backgroundStyle = .Window
        topBar.style.addStyleSheetColors("wnd_Bk $wnd_Caption")

        res.createToolbar(topBar)
        res.registerShortcuts()
        res.favoriteMdl = &res.mdlFormShape

        res.captureBtn = IconButton.create(topBar, Icon.from(&res.icons48, 6), @{0, 0, 64, 64}, id: CmdIdCapture)
        with res.captureBtn
        {
            style.addStyleSheetColors("btnIcon_Icon    0xFFFF0000")
            style.addStyleSheetColors("btnIcon_HotIcon 0xFFFF4444")
        }

        // Main edit area
        res.editWnd = EditWnd.create(res, res)
        res.editWnd.dockStyle = .Center
        res.backgroundStyle = .Window

        res.bottomBar = Wnd.create'Wnd(res, @{0, 0, 0, 128})
        res.bottomBar.dockStyle = .Bottom

        // Recent left toolbar
        res.bottomSizeBar = IconBar.create(res.bottomBar, 20)
        with res.bottomSizeBar
        {
            setIconBarFlags(.Vertical)
            dockStyle = .Left
            iconPadding = 0
            addItem(Icon.from(&res.icons24, 0), CmdIdRecentSmall, .Checkable)
            addItem(Icon.from(&res.icons24, 1), CmdIdRecentMedium, .Checkable)
            addItem(Icon.from(&res.icons24, 2), CmdIdRecentBig, .Checkable)
        }

        // Recent
        res.recentWnd = RecentWnd.create(res.bottomBar, res)
        res.recentWnd.dockStyle = .Center

        // Right bar
        rightWnd := StackLayoutCtrl.create(res, .Top, @{0, 0, 300, 0})
        with rightWnd
        {
            dockStyle = .Right
            backgroundStyle = .Window
            spacing = 0
        }

        label0 := Label.create(rightWnd, "Quick Styles", @{0, 0, 0, 30})
        label0.backgroundStyle = .Window
        label0.style.addStyleSheetColors("wnd_Bk $dlg_BtnBarBk")

        res.quickWnd = QuickWnd.create(rightWnd)
        res.quickWnd.main = res

        label1 := Label.create(rightWnd, "Tool Properties", @{0, 0, 0, 30})
        label1.backgroundStyle = .Window
        label1.style.addStyleSheetColors("wnd_Bk $dlg_BtnBarBk")

        res.propWnd = Wnd.create'PropWnd(rightWnd, @{0, 0, 0, 300})
        with res.propWnd
        {
            main = res
            backgroundStyle = .Window
            layoutKind = .TopAutoSize
        }

        // Bottom bar
        barWnd := StackLayoutCtrl.create(res, .HorzCenter, @{0, 0, 300, 32})
        barWnd.dockStyle = .Bottom
        barWnd.backgroundStyle = .Window

        func updateZoomDisplay(res: *MainWnd)
        {
            z := cast(s32) (res.editWnd.editView.zoom * 100)
            res.zoomBtn.name = Format.toString("% %%", z)
            res.zoomBtn.invalidate()
        }

        res.zoomBtn = IconButton.create(barWnd, res.getTheme().getIcon24(ThemeIcons24.Search), @{0, 0, 110, 32})
        res.zoomBtn.textPos = .Right
        res.zoomBtn.arrowPos = .RightUp
        res.editWnd.sigScrollSizeChanged += @|res|(scroll, old, new) {
            updateZoomDisplay(res);
        }
        res.zoomBtn.sigPressed += @|res|(btn) {
            pos  := btn.getSurfaceRect()
            menu := MenuCtrl.createPopup(res)
            menu.menuCtrlFlags |= .NoKeyShortcuts
            menu.addItem(CmdIdZoom1)
            menu.addItem(CmdIdZoomFit)
            discard menu.doModal(@{pos.horzCenter(), pos.y}, .AnchorBottomCenter, autoDestroy: true)
        }

        updateZoomDisplay(res)
        res.setBottomSize(.Medium)
        res.updateState()

        return res
    }
}
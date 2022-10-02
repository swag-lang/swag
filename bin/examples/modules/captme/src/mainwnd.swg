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

    icons24:        ImageList
    icons48:        ImageList
    editToolBar:    *Wnd
    editWnd:        *EditWnd
    quickWnd:       *QuickWnd
    propWnd:        *PropWnd
    libWnd:         *LibraryWnd
    clipFmt:        Clipboard.Format
    captureBtn:     *IconButton
    libraryBtn:     *IconButton
    zoomBtn:        *IconButton
    recentWnd:      *RecentWnd
    recentBar:      *Wnd
    recentSizeBar:  *IconBar
    quickStyleBar:  *StackLayoutCtrl
    editZoomBar:    *StackLayoutCtrl

    editMode:       EditMode = .Shape
    bottomSize:     BottomSize = Medium
    snap:           bool
    libraryMode:    bool

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
    mtd onSysCommandEvent(evt: *SysCommandEvent)
    {
        evt.accepted = false
        if evt.sysKind == .Close
        {
            libWnd.requestClose()
        }
    }

    mtd onSerializeStateEvent(evt: *SerializeStateEvent)
    {
        catch
        {
            with evt.ser
            {
                .serialize("MainWnd.editMode", &editMode)
                .serialize("MainWnd.snap", &snap)
                .serialize("MainWnd.bottomSize", &bottomSize)
            }
        }

        if evt.ser.isReading()
        {
            setBottomSize(bottomSize)
            updateState()
        }
    }

    mtd onCommandUIEvent(evt: *CommandUIEvent)
    {
        editView := editWnd  ? cast(*EditView) editWnd.view : null
        capture  := editView and !libraryMode ? editView.capture : null

        hasSel       := capture and capture.selection.count
        hasSingleSel := capture and capture.selection.count == 1

        switch evt.id
        {
        case CmdIdExit:
            evt.setFlags |= .Name
            evt.name = "Exit"
            evt.accepted = true

        case CmdIdRestoreOrg:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Restore to Original"
            evt.disabled = !capture or !capture.backImgOrg.isValid()
            evt.accepted = true

        case CmdIdResetQuickStyle:
            evt.setFlags |= .Name
            evt.name = "Restore Default Styles"
            evt.accepted = true

        case CmdIdRecentSmall:
            evt.setFlags |= .Name | .ToolTip | .Checked
            evt.toolTip = "Small recent view"
            evt.checked = bottomSize == .Small
            evt.accepted = true

        case CmdIdRecentMedium:
            evt.setFlags |= .Name | .ToolTip | .Checked
            evt.toolTip = "Medium recent view"
            evt.checked = bottomSize == .Medium
            evt.accepted = true

        case CmdIdRecentBig:
            evt.setFlags |= .Name | .ToolTip | .Checked
            evt.toolTip = "Big recent view"
            evt.checked = bottomSize == .Big
            evt.accepted = true

        case CmdIdDeleteCapture:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Delete"
            evt.disabled = !capture
            evt.accepted = true

        case CmdIdFlatten:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Flatten"
            evt.disabled = !capture or !hasSel
            evt.accepted = true

        case CmdIdFlattenAll:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Flatten All"
            evt.disabled = !capture or !capture.forms.count
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

        case CmdIdLibrary:
            evt.setFlags |= .Name | .ToolTip | .Checked
            evt.name = "Library"
            evt.toolTip = "Display captures library"
            evt.accepted = true
            evt.checked = libraryMode

        case CmdIdSelectAll:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture or !capture.forms.count
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
        case CmdIdExit:
            postQuitEvent()

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
            g_Library.addFile(file)

        case CmdIdSnap:
            snap = !snap

        case CmdIdCapture:
            captureScreen()

        case CmdIdLibrary:
            swapLibraryMode()

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

        case CmdIdRestoreOrg:
            undo = capture.newUndo(.RestoreOrg)
            evt.accepted = true

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

        case CmdIdFlatten:
            undo = capture.newUndo(.Flatten)
            evt.accepted = true

        case CmdIdFlattenAll:
            undo = capture.newUndo(.FlattenAll)
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
        with topMenu := MenuCtrl.createBar(parent, owner: self)
        {
            .bringToFront()
            .dockStyle = .Top
            .margin.y = -parent.padding.y
            .backgroundStyle = .Transparent
        }

        with fileMenu := MenuCtrl.createPopup(parent, owner: self)
        {
            .addItem(CmdIdExit)
            topMenu.addPopup("File", fileMenu)
        }

        with editMenu := MenuCtrl.createPopup(parent, owner: self)
        {
            .addItem(CmdIdUndo)
            .addItem(CmdIdRedo)
            .addItem(CmdIdRestoreOrg)
            .addSeparator()
            .addItem(CmdIdCopy)
            .addItem(CmdIdPaste)
            .addItem(CmdIdCopyAsImage)
            .addSeparator()
            .addItem(CmdIdSelectAll)
            .addItem(CmdIdDeSelectAll)
            .addItem(CmdIdInvertSelection)
            topMenu.addPopup("Edit", editMenu)
        }

        with displayMenu := MenuCtrl.createPopup(parent, owner: self)
        {
            .addItem(CmdIdZoomIn)
            .addItem(CmdIdZoomOut)
            .addItem(CmdIdZoom1)
            .addItem(CmdIdZoomFit)
            topMenu.addPopup("View", displayMenu)
        }
    }

    mtd createToolbar(topBar: *Wnd)
    {
        mtd createBtn(id: WndId)
        {
            b := IconButton.create(editToolBar, position: @{0, 0, 60, 64})
            b.id = #self.id
            b.style.addStyleSheetColors("btnIcon_CheckedBk    $btnIcon_HotBk")
            b.style.addStyleSheetColors("btnIcon_CheckedHotBk $btnIcon_HotBk")
            b.iconBtnFlags |= .Checkable
            b.checkedForm = .Bottom
        }

        editToolBar = StackLayoutCtrl.create(topBar, .HorzCenter, @{0, 0, 64, 64})
        editToolBar.dockStyle = .Top
        editToolBar.backgroundStyle = .Window
        editToolBar.style.addStyleSheetColors("wnd_Bk $wnd_Caption")
        createBtn(CmdIdModeFavorites)
        createBtn(CmdIdModeShape)
        createBtn(CmdIdModeArrow)
        createBtn(CmdIdModeLine)
        createBtn(CmdIdModePolygon)
        createBtn(CmdIdModeText)
        createBtn(CmdIdModeDraw)
        editToolBar.updateCommandState()
    }

    mtd setBottomSize(bottomSize: BottomSize)
    {
        self.bottomSize = #self.bottomSize
        switch self.bottomSize
        {
        case Small:
            recentBar.resize(0, 80)
        case Medium:
            recentBar.resize(0, 128)
        case Big:
            recentBar.resize(0, 224)
        }

        applyLayout(@{position.width, position.height})
    }

    mtd swapLibraryMode()
    {
        libraryMode = !libraryMode

        editWnd.show(!libraryMode)
        quickStyleBar.show(!libraryMode)
        recentBar.show(!libraryMode)
        editZoomBar.show(!libraryMode)
        editToolBar.show(!libraryMode)

        libWnd.show(libraryMode)

        if libraryMode
            libWnd.fill()

        applyLayout()
    }

    mtd registerShortcuts()
    {
        registerKeyShortcut(.Control,          .PrintScreen,    CmdIdCapture)
        registerKeyShortcut(.Control,          .C,              CmdIdCopy)
        registerKeyShortcut(.Control,          .V,              CmdIdPaste)
        registerKeyShortcut(.Control,          .Insert,         CmdIdCopy)
        registerKeyShortcut(.Shift,            .Insert,         CmdIdPaste)
        registerKeyShortcut(.Control,          .Z,              CmdIdUndo)
        registerKeyShortcut(.Control,          .Y,              CmdIdRedo)
        registerKeyShortcut(.CtrlShift,        .F,              CmdIdBringForward)
        registerKeyShortcut(.CtrlShift,        .B,              CmdIdSendBackward)
        registerKeyShortcut(.None,             .Delete,         CmdIdDeleteSelection)
        registerKeyShortcut(.CtrlShift,        .A,              CmdIdCopyAsImage)
        registerKeyShortcut(.Control,          .A,              CmdIdSelectAll)
        registerKeyShortcut(.Control,          .D,              CmdIdDeSelectAll)
        registerKeyShortcut(.Control,          .Add,            CmdIdZoomIn)
        registerKeyShortcut(.Control,          .Subtract,       CmdIdZoomOut)
        registerKeyShortcut(.Control,          .NumPad0,        CmdIdZoom1)
        registerKeyShortcut(.CtrlShift,        .D,              CmdIdDupCapture)
        registerKeyShortcut(.CtrlShift,        .I,              CmdIdInvertSelection)
        registerKeyShortcut(.CtrlShift,        .T,              CmdIdFlattenAll)
        registerKeyShortcut(.Alt,              .F4,             CmdIdExit)
    }

    mtd updateState()
    {
        captureBtn.updateCommandState()
        libraryBtn.updateCommandState()
        editToolBar.updateCommandState()
        recentSizeBar.updateCommandState()
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

        with capture := Memory.new'Capture()
        {
            .changed = true
            .width, .widthOrg = img.width
            .height, .heightOrg = img.height
            .backImg, .backImgOrg = img
            .file = Library.newFileName()
        }

        editWnd.editView.setCapture(capture)
        editWnd.editView.zoomToFit(false)

        capture.computePreview()
        recentWnd.addRecent(capture.file, capture.preview)
        recentWnd.recentView.selected = 0
        capture.save()
        g_Library.addFile(capture.file)

        // Force edition mode
        if libraryMode
        {
            swapLibraryMode()
            updateState()
        }
    }

    func updateZoomDisplay(res: *MainWnd)
    {
        z := cast(s32) (res.editWnd.editView.zoom * 100)
        res.zoomBtn.name = Format.toString("% %%", z)
        res.zoomBtn.invalidate()
    }

    func create(parent: *Wnd)->*MainWnd
    {
        img24 := assume Image.decode(".png", Icons24)
        img48 := assume Image.decode(".png", Icons48)
        renderer := parent.getApp().getRenderer()

        with res := Wnd.create'MainWnd(parent, "captme.mainwnd")
        {
            .icons24.set(renderer.addImage(img24), 24, 24)
            .icons48.set(renderer.addImage(img48), 48, 48)
            .clipFmt = catch Clipboard.registerFormat(ClipboardFormat)
            .backgroundStyle = .Window
            .favoriteMdl = &res.mdlFormShape
            .registerShortcuts()
        }

        with topBar := Wnd.create'Wnd(res, @{0, 0, 64, 68})
        {
            .dockStyle = .Top
            .backgroundStyle = .Window
            .style.addStyleSheetColors("wnd_Bk $wnd_Caption")
        }

        res.createToolbar(topBar)

        with res.captureBtn = IconButton.create(topBar, Icon.from(&res.icons48, 6), @{0, 0, 64, 64}, id: CmdIdCapture)
        {
            .style.addStyleSheetColors("btnIcon_Icon    0xFFFF0000")
            .style.addStyleSheetColors("btnIcon_HotIcon 0xFFFF4444")
        }

        res.libraryBtn = IconButton.create(topBar, Icon.from(&res.icons48, 8), @{64, 0, 64, 64}, id: CmdIdLibrary)

        // Main edit area
        with res.editWnd = EditWnd.create(res, res)
        {
            .dockStyle = .Center
            .sigScrollSizeChanged += @|res|(scroll, old, new) {
                updateZoomDisplay(res)
            }
        }

        with res.recentBar = Wnd.create'Wnd(res, @{0, 0, 0, 128})
        {
            .dockStyle = .Bottom
        }

        // Recent left editToolBar
        with res.recentSizeBar = IconBar.create(res.recentBar, 20)
        {
            .setIconBarFlags(.Vertical)
            .dockStyle = .Left
            .iconPadding = 0
            .addItem(Icon.from(&res.icons24, 0), CmdIdRecentSmall, .Checkable)
            .addItem(Icon.from(&res.icons24, 1), CmdIdRecentMedium, .Checkable)
            .addItem(Icon.from(&res.icons24, 2), CmdIdRecentBig, .Checkable)
        }

        // Recent
        with res.recentWnd = RecentWnd.create(res.recentBar, res)
        {
            .dockStyle = .Center
        }

        // Right bar
        with res.quickStyleBar = StackLayoutCtrl.create(res, .Top, @{0, 0, 300, 0})
        {
            .dockStyle = .Right
            .backgroundStyle = .Window
            .spacing = 0
        }

        with label0 := Label.create(res.quickStyleBar, "Quick Styles", @{0, 0, 0, 30})
        {
            .backgroundStyle = .Window
            .style.addStyleSheetColors("wnd_Bk $dlg_BtnBarBk")
        }

        with res.quickWnd = QuickWnd.create(res.quickStyleBar)
        {
            .main = res
        }

        with label1 := Label.create(res.quickStyleBar, "Tool Properties", @{0, 0, 0, 30})
        {
            .backgroundStyle = .Window
            .style.addStyleSheetColors("wnd_Bk $dlg_BtnBarBk")
        }

        with res.propWnd = Wnd.create'PropWnd(res.quickStyleBar, @{0, 0, 0, 300})
        {
            .main = res
            .backgroundStyle = .Window
            .layoutKind = .TopAutoSize
        }

        // Bottom bar
        with res.editZoomBar = StackLayoutCtrl.create(res, .HorzCenter, @{0, 0, 300, 32})
        {
            .dockStyle = .Bottom
            .backgroundStyle = .Window
        }

        with res.zoomBtn = IconButton.create(res.editZoomBar, res.getTheme().getIcon24(ThemeIcons24.Search), @{0, 0, 110, 32})
        {
            .textPos = .Right
            .arrowPos = .RightUp
            .sigPressed += @|res|(btn) {
                pos  := btn.getSurfaceRect()
                menu := MenuCtrl.createPopup(res)
                menu.menuCtrlFlags |= .NoKeyShortcuts
                menu.addItem(CmdIdZoom1)
                menu.addItem(CmdIdZoomFit)
                discard menu.doModal(@{pos.horzCenter(), pos.y}, .AnchorBottomCenter, autoDestroy: true)
            }
        }

        updateZoomDisplay(res)

        with res
        {
            .setBottomSize(.Medium)
            .updateState()
            .libWnd = LibraryWnd.create(res)
            .libWnd.hide()
        }

        return res
    }
}
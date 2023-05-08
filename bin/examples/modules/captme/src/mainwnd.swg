var g_Main: *MainWnd

enum EditMode
{
    Shape
    Arrow
    Line
    Polygon
    Text
    Favorites
    Draw
    Highlight
    Selection
    Step
    Image
    PixSel
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
    sizeBtn:        *IconButton
    recentWnd:      *RecentWnd
    recentBar:      *Wnd
    recentSizeBar:  *IconBar
    quickStyleBar:  *SplitterCtrl
    editZoomBar:    *StackLayoutCtrl
    monitors:       Array'Env.Monitor
    pixSelFillCol:  Color = Argb.White

    editMode:       EditMode = .Shape
    lastEditMode:   EditMode = .Shape
    bottomSize:     BottomSize = Medium
    snap:           bool
    libraryMode:    bool
    fileDlgState:   FileDlgState

    captureHideEditor   = true
    captureShowEditor   = true
    captureToEditor     = true
    captureToClipboard  = false
    captureDelay        = 0

    mdlFormShape:   FormShape
    mdlFormImage:   FormImage
    mdlFormStep:    FormStep
    mdlFormArrow:   FormArrow
    mdlFormLine:    FormLine
    mdlFormPolygon: FormPolygon
    mdlFormText:    FormText
    mdlFormDraw:    FormDraw
    mdlHighlight:   IForm
    mdlPixSel:      IForm
    mdlFavorite:    IForm
}

const Icons24 = #include("datas/icons24.png")
const Icons48 = #include("datas/icons48.png")

impl IWnd for MainWnd
{
    mtd popupBar()
    {
        sf := catch surface.app.createSurface(500, 400, 300, 300, .Secondary | .TopMost)
        with menu := Wnd.create'MenuCtrl(sf.wnd.view, cast(WndId) null)
        {
            .addItem(CmdIdShowEditor)
            .addSeparator()
            .addItem(CmdIdCaptureArea)
            .addItem(CmdIdCaptureLastArea)
            .addItem(CmdIdCaptureFullScreen)
            .addItem(CmdIdCaptureActiveWnd)
            .addSeparator()
            .addItem(CmdIdNewFromClipboard)
            .addSeparator()
            .addItem(CmdIdExit)

            .owner = self
            .paintByFrame = true
            .dockStyle = .Center
            .padding = 10
            .updateState()
            .computeLayoutPopup()
        }

        pt := Input.getMousePos()
        menu.position.width += 2 * getThemeMetrics().surfaceWnd_ShadowSize
        menu.position.height += 2 * getThemeMetrics().surfaceWnd_ShadowSize
        sf.setPosition({pt.x, pt.y, menu.position.width, menu.position.height})
        sf.constaintToScreen()
        sf.wnd.view.backgroundStyle = .Window

        sf.show()
        sf.bringToTop()
        while !menu.endModal
            surface.app.runFrame()
        sf.hide()
        if menu.endModalResult
            postCommandEvent(menu.endModalResult)
        sf.destroy()
    }

    mtd onSysUserEvent(evt: *SysUserEvent)
    {
        #if #os == Swag.TargetOs.Windows
        {
            if evt.param1 == Win32.WM_LBUTTONDBLCLK
            {
                surface.show()
                surface.bringToTop()
            }
            elif evt.param1 == Win32.WM_RBUTTONDOWN
            {
                popupBar()
            }
        }
    }

    mtd onSysCommandEvent(evt: *SysCommandEvent)
    {
        evt.accepted = false
        switch evt.sysKind
        {
        case Close:
            evt.accepted = true
            surface.hide()
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
                .serialize("MainWnd.fileDlgState", &fileDlgState)
                .serialize("MainWnd.captureHideEditor", &captureHideEditor)
                .serialize("MainWnd.captureShowEditor", &captureShowEditor)
                .serialize("MainWnd.captureToEditor", &captureToEditor)
                .serialize("MainWnd.captureToClipbpard", &captureToClipboard)
                .serialize("MainWnd.captureDelay", &captureDelay)
                .serialize("MainWnd.RightBar.Pane0", &quickStyleBar.items[0].size)
                .serialize("MainWnd.RightBar.Pane1", &quickStyleBar.items[1].size)
            }
        }

        if evt.ser.isReading()
        {
            quickStyleBar.sendResizeEvent()
            setBottomSize(bottomSize)
            updateState()
        }
    }

    mtd onFrameEvent(evt: *FrameEvent)
    {
        if evt.firstFrame
        {
            quickWnd.setDefault()
        }
    }

    mtd onCommandUIEvent(evt: *CommandUIEvent)
    {
        editView := editWnd  ? cast(*EditView) editWnd.view : null
        capture  := editView and !libraryMode ? editView.capture : null

        hasSel := capture and capture.selection.count

        var lastSelType: typeinfo
        if capture
        {
            if capture.selection.count
            {
                f := capture.forms[capture.selection.back()]
                lastSelType = @kindof(f)
            }
        }

        switch evt.id
        {
        case CmdIdShowEditor:
            evt.setFlags |= .Name
            evt.name = "Show Editor"
            evt.accepted = true

        case CmdIdResizeImage:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Resize Image..."
            evt.disabled = !capture
            evt.accepted = true

        case CmdIdResizeCanevas:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Resize Capture..."
            evt.disabled = !capture
            evt.accepted = true

        case CmdIdTrimCanevas:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Trim"
            evt.disabled = !capture
            evt.accepted = true

        case CmdIdSaveAsImage:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Save As Image..."
            evt.disabled = !capture
            evt.accepted = true

        case CmdIdSaveAsLinkedImage:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Save As Linked Image..."
            evt.disabled = !capture
            evt.accepted = true

        case CmdIdOpenImage:
            evt.setFlags |= .Name
            evt.name = "Open Image..."
            evt.accepted = true

        case CmdIdUpdateLinkedImage:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Update Linked Image"
            evt.disabled = !capture or capture.linkedImage.length == 0
            evt.accepted = true

        case CmdIdNewImage:
            evt.setFlags |= .Name
            evt.name = "New Image"
            evt.accepted = true

        case CmdIdNewFromClipboard:
            evt.setFlags |= .Name | .Disabled
            evt.name = "New From Clipboard"
            evt.disabled = !Clipboard.hasImage()
            evt.accepted = true

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

        case CmdEditText:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !hasSel or !lastSelType or lastSelType != FormText
            evt.name = "Edit"
            evt.accepted = true

        case CmdIdClearImageInside:
            evt.setFlags |= .Disabled | .Name | .Hidden
            evt.disabled, evt.hidden = true
            evt.name = "Clear Inside Image"
            evt.accepted = true

            if capture and hasSel
            {
                visit it: capture.selection
                {
                    itfForm := capture.forms[it]
                    if @kindof(itfForm) != FormImage
                        continue
                    b := cast(*FormImage) @dataof(itfForm)
                    if b.kind != .Image
                        continue
                    evt.hidden = false
                    if b.image.isValid()
                    {
                        evt.disabled = false
                        break
                    }
                }
            }

        case CmdIdDeleteSelection:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !hasSel
            evt.name = "Delete"
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
        case CmdIdShowEditor:
            surface.show()
            surface.bringToTop()

        case CmdIdResizeImage:
            resizeImage()
        case CmdIdResizeCanevas:
            resizeCanevas()
        case CmdIdTrimCanevas:
            trimCanevas()

        case CmdIdSaveAsImage:
            saveAsImage()
        case CmdIdSaveAsLinkedImage:
            saveAsImage(linked: true)
        case CmdIdOpenImage:
            openImage()
        case CmdIdUpdateLinkedImage:
            txt := Format.toString("This will overwrite the original image.\n")
            txt += Format.toString("File is '%'", capture.linkedImage)
            if MessageDlg.confirm(surface, txt) != Dialog.BtnYes
                break
            Cursor.wait()
            img := capture.toImage()
            catch img.save(capture.linkedImage)
            if @err
                MessageDlg.error(surface, @err, "Failed to update file !")

        case CmdIdExit:
            libWnd.requestClose()
            postQuitEvent()

        case CmdIdResetQuickStyle:
            quickWnd.resetQuickStyles()

        case CmdIdRecentSmall:
            setBottomSize(.Small)

        case CmdIdRecentMedium:
            setBottomSize(.Medium)

        case CmdIdRecentBig:
            setBottomSize(.Big)

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

        case CmdIdDeleteCapture:
            res := MessageDlg.confirm(surface, "This will delete the capture file.")
            if res == MessageDlg.BtnYes
                recentWnd.removeRecent()

        case CmdIdDupCapture:
            file := Library.newFileName()
            trycatch File.duplicate(file, capture.file)
            dupCapture := trycatch recentWnd.addRecent(file)
            Memory.delete(dupCapture)
            recentWnd.recentView.select(0)
            g_Library.addFile(file)

        case CmdIdSnap:
            snap = !snap

        case CmdIdNewImage:
            newImage()

        case CmdIdNewFromClipboard:
            newCapture(Clipboard.getImage())

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
            var newSel: Array'u64
            loop i: capture.forms
            {
                if !capture.selection.contains(i)
                    newSel.add(i)
            }
            capture.selection = newSel
            capture.selectionHasChanged()

        case CmdEditText:
            lastSel := cast(*FormText) @dataof(capture.forms[capture.selection.back()])
            discard propWnd.editText(lastSel.text)
            capture.selectionHasChanged()

        case CmdIdClearImageInside:
            capture.clearImageInside()

        case CmdIdDeleteSelection:
            undo = capture.newUndo(.DeleteSelection)
            evt.accepted = true

        case CmdIdRestoreOrg:
            undo = capture.newUndo(.RestoreOrg)
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
        if editMode == .Selection
        {
            editMode = .Shape
            getCapture().updateEditModeFromSelection()
            edMode := editMode
            editMode = .Selection
            return edMode
        }
        elif editMode == .Favorites
        {
            #[Swag.Complete]
            switch @kindof(mdlFavorite)
            {
            case FormShape:     return EditMode.Shape
            case FormArrow:     return EditMode.Arrow
            case FormLine:      return EditMode.Line
            case FormPolygon:   return EditMode.Polygon
            case FormText:      return EditMode.Text
            case FormDraw:      return EditMode.Draw
            case FormStep:      return EditMode.Step
            }
        }
        elif editMode == .Highlight
        {
            #[Swag.Complete]
            switch @kindof(mdlHighlight)
            {
            case FormShape:     return EditMode.Shape
            case FormArrow:     return EditMode.Arrow
            case FormLine:      return EditMode.Line
            case FormPolygon:   return EditMode.Polygon
            case FormText:      return EditMode.Text
            case FormDraw:      return EditMode.Draw
            case FormStep:      return EditMode.Step
            }
        }
        elif editMode == .PixSel
        {
            #[Swag.Complete]
            switch @kindof(mdlPixSel)
            {
            case FormShape:     return EditMode.Shape
            case FormArrow:     return EditMode.Arrow
            case FormLine:      return EditMode.Line
            case FormPolygon:   return EditMode.Polygon
            case FormText:      return EditMode.Text
            case FormDraw:      return EditMode.Draw
            case FormStep:      return EditMode.Step
            }
        }

        return editMode
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

        applyLayout({position.width, position.height})
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

    mtd updateState()
    {
        captureBtn.updateCommandState()
        libraryBtn.updateCommandState()
        editToolBar.updateCommandState()
        recentSizeBar.updateCommandState()
        propWnd.selectionHasChanged()
        quickWnd.updateQuickStyles()
    }

    mtd captureHasChanged()
    {
        updateSizeDisplay()
    }

    mtd getCapture()->*Capture
    {
        if !editWnd return null
        return editWnd.editView.capture
    }

    mtd updateZoomDisplay()
    {
        z := cast(s32) (editWnd.editView.zoom * 100)
        zoomBtn.name = Format.toString("% %%", z)
        zoomBtn.invalidate()
    }

    mtd updateSizeDisplay()
    {
        capture := getCapture()
        if !capture or !sizeBtn
            return
        sizeBtn.name = Format.toString("% x %", capture.width, capture.height)
        sizeBtn.invalidate()
    }

    mtd registerActions()
    {
        registerAction'ActionDelay()
        registerAction'ActionPixSel()
        registerAction'ActionCapture()
        registerAction'ActionMoveSel()
        registerAction'ActionToolMode()
        registerAction'ActionCopyPaste()
    }

    mtd registerShortcuts()
    {
        registerKeyShortcut(.Control,          .C,              CmdIdCopy)
        registerKeyShortcut(.Control,          .V,              CmdIdPaste)
        registerKeyShortcut(.Control,          .Insert,         CmdIdCopy)
        registerKeyShortcut(.Shift,            .Insert,         CmdIdPaste)
        registerKeyShortcut(.Control,          .Z,              CmdIdUndo)
        registerKeyShortcut(.Control,          .Y,              CmdIdRedo)
        registerKeyShortcut(.CtrlShift,        .F,              CmdIdBringForward)
        registerKeyShortcut(.CtrlShift,        .B,              CmdIdSendBackward)
        registerKeyShortcut(.Zero,             .Delete,         CmdIdDeleteSelection)
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
        registerKeyShortcut(.Control,          .N,              CmdIdNewImage)
        registerKeyShortcut(.CtrlShift,        .N,              CmdIdNewFromClipboard)
        registerKeyShortcut(.Control,          .S,              CmdIdSaveAsImage)
        registerKeyShortcut(.CtrlShift,        .S,              CmdIdUpdateLinkedImage)
        registerKeyShortcut(.Control,          .O,              CmdIdOpenImage)
        registerKeyShortcut(.Shift,            .Left,           CmdIdMoveSelLeft1)
        registerKeyShortcut(.Shift,            .Up,             CmdIdMoveSelTop1)
        registerKeyShortcut(.Shift,            .Right,          CmdIdMoveSelRight1)
        registerKeyShortcut(.Shift,            .Down,           CmdIdMoveSelBottom1)
        registerKeyShortcut(.Zero,             .Left,           CmdIdMoveSelLeft)
        registerKeyShortcut(.Zero,             .Up,             CmdIdMoveSelTop)
        registerKeyShortcut(.Zero,             .Right,          CmdIdMoveSelRight)
        registerKeyShortcut(.Zero,             .Down,           CmdIdMoveSelBottom)

        errorReg := false
        #[Swag.Mixin]
        func registerHotKey(mdf: Input.KeyModifiers, key: Input.Key, wndId: WndId)
        {
            catch getApp().registerHotKey(mdf, key, wndId, self)
            if @err
            {
                if !errorReg
                {
                    msg := Format.toString("Cannot register global shortcut '%'. ", Input.getKeyName(mdf, key))
                    msg += "Another running application is probably already using it !"
                    discard EmbInfoCtrl.create(self.editWnd, msg, .Critical)
                }

                errorReg = true
            }
        }

        registerHotKey(.Zero,      .PrintScreen, CmdIdCaptureArea)
        registerHotKey(.CtrlShift, .PrintScreen, CmdIdCaptureLastArea)
        registerHotKey(.Control,   .PrintScreen, CmdIdCaptureFullScreen)
        registerHotKey(.Alt,       .PrintScreen, CmdIdCaptureActiveWnd)
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
            .addItem(CmdIdNewImage)
            .addItem(CmdIdNewFromClipboard)
            .addSeparator()
            .addItem(CmdIdOpenImage)
            .addItem(CmdIdSaveAsImage)
            .addItem(CmdIdSaveAsLinkedImage)
            .addSeparator()
            .addItem(CmdIdUpdateLinkedImage)
            .addSeparator()
            .addItem(CmdIdExit)
            topMenu.addPopup("File", fileMenu)
        }

        with captureMenu := MenuCtrl.createPopup(parent, owner: self)
        {
            .addItem(CmdIdCaptureArea)
            .addItem(CmdIdCaptureLastArea)
            .addItem(CmdIdCaptureFullScreen)
            .addItem(CmdIdCaptureActiveWnd)

            visit m, idx: monitors
            {
                switch idx
                {
                case 0: .addItem(CmdIdCaptureScreen0)
                case 1: .addItem(CmdIdCaptureScreen1)
                case 2: .addItem(CmdIdCaptureScreen2)
                case 3: .addItem(CmdIdCaptureScreen3)
                }
            }

            .addSeparator()
            .addItem(CmdIdCaptureHideEditor)
            .addItem(CmdIdCaptureShowEditor)

            with delayMenu := MenuCtrl.createPopup(captureMenu, owner: self)
            {
                .addItem(CmdIdCaptureDelay0)
                .addItem(CmdIdCaptureDelay1)
                .addItem(CmdIdCaptureDelay2)
                .addItem(CmdIdCaptureDelay5)
                .addItem(CmdIdCaptureDelay9)
                captureMenu.addPopup(delayMenu, CmdIdCaptureDelay)
            }

            topMenu.addPopup("Capture", captureMenu)

            .addSeparator()
            .addItem(CmdIdCaptureToEditor)
            .addItem(CmdIdCaptureToClipboard)
        }

        with editMenu := MenuCtrl.createPopup(parent, owner: self)
        {
            .addItem(CmdIdUndo)
            .addItem(CmdIdRedo)
            .addSeparator()
            .addItem(CmdIdCopy)
            .addItem(CmdIdPaste)
            .addItem(CmdIdCopyAsImage)
            .addSeparator()
            .addItem(CmdIdSelectAll)
            .addItem(CmdIdDeSelectAll)
            .addItem(CmdIdInvertSelection)
            .addSeparator()
            .addItem(CmdIdSnap)
            topMenu.addPopup("Edit", editMenu)
        }

        with imageMenu := MenuCtrl.createPopup(parent, owner: self)
        {
            .addItem(CmdIdRestoreOrg)
            .addSeparator()
            .addItem(CmdIdResizeImage)
            .addItem(CmdIdResizeCanevas)
            .addItem(CmdIdTrimCanevas)
            .addSeparator()
            .addItem(CmdIdFlattenAll)
            topMenu.addPopup("Image", imageMenu)
        }

        with viewMenu := MenuCtrl.createPopup(parent, owner: self)
        {
            .addItem(CmdIdZoomIn)
            .addItem(CmdIdZoomOut)
            .addSeparator()
            .addItem(CmdIdZoom1)
            .addItem(CmdIdZoomFit)
            topMenu.addPopup("View", viewMenu)
        }
    }

    mtd createToolbar(topBar: *Wnd)
    {
        mtd createBtn(id: WndId)
        {
            with b := IconButton.create(editToolBar, {}, position: {0, 0, 60, 64})
            {
                .id = #self.id
                .style.addStyleSheetColors("btnIcon_CheckedBk    $btnIcon_HotBk")
                .style.addStyleSheetColors("btnIcon_CheckedHotBk $btnIcon_HotBk")
                .iconBtnFlags |= .Checkable
                .checkedForm = .Bottom
            }
        }

        with editToolBar = StackLayoutCtrl.create(topBar, .HorzCenter, {0, 0, 64, 64})
        {
            .dockStyle = .Top
            .backgroundStyle = .Window
            .style.addStyleSheetColors("wnd_Bk $wnd_Caption")
        }

        createBtn(CmdIdModeFavorites)
        createBtn(CmdIdModeSelection)
        createBtn(CmdIdModeShape)
        createBtn(CmdIdModeArrow)
        createBtn(CmdIdModeLine)
        createBtn(CmdIdModePolygon)
        createBtn(CmdIdModeText)
        createBtn(CmdIdModeDraw)
        createBtn(CmdIdModeHighlight)
        createBtn(CmdIdModeStep)
        createBtn(CmdIdModeImage)
        createBtn(CmdIdModePixSel)
        editToolBar.updateCommandState()
    }

    func create(parent: *Wnd)->*MainWnd
    {
        parent.surface.minSize = {1024, 512}

        img24 := assume Image.decode(".png", Icons24)
        img48 := assume Image.decode(".png", Icons48)
        renderer := parent.getApp().getRenderer()

        with res := Wnd.create'MainWnd(parent, "captme.mainwnd")
        {
            .icons24.set(renderer.addImage(img24), 24, 24)
            .icons48.set(renderer.addImage(img48), 48, 48)
            .clipFmt = catch Clipboard.registerFormat(ClipboardFormat)
            .backgroundStyle = .Window
            .mdlFavorite  = &res.mdlFormShape
            .mdlHighlight = &res.mdlFormShape
            .mdlPixSel    = &res.mdlFormShape
            .monitors = Env.getMonitors()
        }

        with topBar := Wnd.create'Wnd(res, {0, 0, 64, 68})
        {
            .dockStyle = .Top
            .backgroundStyle = .Window
            .style.addStyleSheetColors("wnd_Bk $wnd_Caption")
        }

        res.createToolbar(topBar)

        res.captureBtn = IconButton.create(topBar, Icon.from(&res.icons48, 6), {0, 0, 64, 64}, id: CmdIdCapture)
        res.libraryBtn = IconButton.create(topBar, Icon.from(&res.icons48, 8), {64, 0, 64, 64}, id: CmdIdLibrary)

        // Main edit area
        with res.editWnd = EditWnd.create(res, res)
        {
            .dockStyle = .Center
            .sigScrollSizeChanged += closure|res|(scroll, old, new) {
                res.updateZoomDisplay()
            }
        }

        with res.recentBar = Wnd.create'Wnd(res, {0, 0, 0, 128})
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
        with res.quickStyleBar = SplitterCtrl.create(res, {0, 0, 300, 600}, vertical: true)
        {
            .dockStyle = .Right
            .splitterFlags.add(.HideBars)
            .backgroundStyle = .Window
        }

        with res.quickWnd = QuickWnd.create(res.quickStyleBar)
        {
            .main = res
        }

        with res.propWnd = PropWnd.create(res.quickStyleBar)
        {
            .main = res
        }

        item := res.quickStyleBar.addPane(res.quickWnd, title: true)
        item.tryPreserveSize = true
        item.label.setText("Quick Styles")

        item = res.quickStyleBar.addPane(res.propWnd, title: true)
        item.label.setText("Tool Properties")

        res.quickStyleBar.setPaneSize(0, 300)

        // Bottom bar
        with res.editZoomBar = StackLayoutCtrl.create(res, .HorzCenter, {0, 0, 300, 32})
        {
            .dockStyle = .Bottom
            .backgroundStyle = .Window
        }

        ic := res.getTheme().getIcon24(ThemeIcons24.Search, 20)
        with res.zoomBtn = IconButton.create(res.editZoomBar, ic, {0, 0, 120, 32})
        {
            .iconBtnFlags.add(.Center)
            .textPos = .Right
            .arrowPos = .RightUp
            .sigPressed += closure|res|(btn) {
                pos  := btn.getSurfaceRect()
                menu := MenuCtrl.createPopup(res)
                menu.menuCtrlFlags |= .NoKeyShortcuts
                menu.addItem(CmdIdZoom1)
                menu.addItem(CmdIdZoomFit)
                discard menu.doModal({pos.horzCenter(), pos.y}, .AnchorBottomCenter, autoDestroy: true)
            }
        }

        with res.sizeBtn = IconButton.create(res.editZoomBar, {}, {0, 0, 120, 32})
        {
            .iconBtnFlags.add(.Center)
            .textPos = .Right
            .arrowPos = .RightUp
            .sigPressed += closure|res|(btn) {
                pos  := btn.getSurfaceRect()
                menu := MenuCtrl.createPopup(res)
                menu.menuCtrlFlags.add(.NoKeyShortcuts)
                menu.addItem(CmdIdResizeImage)
                menu.addItem(CmdIdResizeCanevas)
                discard menu.doModal({pos.horzCenter(), pos.y}, .AnchorBottomCenter, autoDestroy: true)
            }
        }

        with res
        {
            .registerActions()
            .registerShortcuts()
            .updateZoomDisplay()
            .updateSizeDisplay()
            .setBottomSize(.Medium)
            .updateState()
            .libWnd = LibraryWnd.create(res)
            .libWnd.hide()
        }

        g_Main = res
        return res
    }
}
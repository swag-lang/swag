var g_Main: *MainWnd

const ClipboardFormat = "Swag.CaptMe"

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

const Icons24 = @include("datas/icons24.png")
const Icons48 = @include("datas/icons48.png")

impl IWnd for MainWnd
{
    mtd popupBar()
    {
        let sf = catch surface.app.createSurface(500, 400, 300, 300, .Secondary | .TopMost)
        with let menu = Wnd.create'MenuCtrl(sf.wnd.view, cast(WndId) null)
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

        var pt = Input.getMousePos()
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

    mtd impl onSysUserEvent(evt: *SysUserEvent)
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

    mtd impl onSysCommandEvent(evt: *SysCommandEvent)
    {
        evt.accepted = false
        switch evt.sysKind
        {
        case Close:
            evt.accepted = true
            surface.hide()
        }
    }

    mtd impl onSerializeStateEvent(evt: *SerializeStateEvent)
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

    mtd impl onFrameEvent(evt: *FrameEvent)
    {
        if evt.firstFrame
        {
            quickWnd.setDefault()
        }
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
            let edMode = editMode
            editMode = .Selection
            return edMode
        }

        var kindOf: typeinfo
        if editMode == .Favorites
            kindOf = @kindof(mdlFavorite)
        elif editMode == .Highlight
            kindOf = @kindof(mdlHighlight)
        elif editMode == .PixSel
            kindOf = @kindof(mdlPixSel)

        if kindOf
        {
            #[Swag.Complete]
            switch kindOf
            {
            case FormShape:     return EditMode.Shape
            case FormArrow:     return EditMode.Arrow
            case FormLine:      return EditMode.Line
            case FormPolygon:   return EditMode.Polygon
            case FormText:      return EditMode.Text
            case FormDraw:      return EditMode.Draw
            case FormStep:      return EditMode.Step
            case FormImage:     return EditMode.Image
            }
        }

        return editMode
    }

    mtd updateState()
    {
        updateCommandState()
        propWnd.selectionHasChanged()
        quickWnd.updateQuickStyles()
    }

    mtd captureHasChanged()
    {
        updateSizeDisplay()
        if recentWnd
        {
            editToolBar.updateCommandState()
            recentWnd.updateRecent()
        }
    }

    mtd getCapture()->*Capture
    {
        if !editWnd return null
        return editWnd.editView.capture
    }

    mtd updateZoomDisplay()
    {
        let z = cast(s32) (editWnd.editView.zoom * 100)
        zoomBtn.name = Format.toString("% %%", z)
        zoomBtn.invalidate()
    }

    mtd updateSizeDisplay()
    {
        let capture = getCapture()
        if !capture or !sizeBtn
            return
        sizeBtn.name = Format.toString("% x %", capture.width, capture.height)
        sizeBtn.invalidate()
    }

    mtd registerActions()
    {
        registerAction'ActionUndo()
        registerAction'ActionDelay()
        registerAction'ActionPixSel()
        registerAction'ActionCapture()
        registerAction'ActionMoveSel()
        registerAction'ActionToolMode()
        registerAction'ActionCopyPaste()
        registerAction'ActionFile()
        registerAction'ActionZoom()
        registerAction'ActionImage()
        registerAction'ActionSelection()
        registerAction'ActionFlatten()
        registerAction'ActionRecentSize()
        registerAction'ActionQuickStyle()
        registerAction'ActionEdit()
        registerAction'ActionGlobal()
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

        var errorReg = false

        #[Swag.Mixin]
        func registerHotKey(mdf: Input.KeyModifiers, key: Input.Key, wndId: WndId)
        {
            catch getApp().registerHotKey(mdf, key, wndId, self)
            if @errmsg
            {
                if !errorReg
                {
                    var msg = Format.toString("Cannot register global shortcut '%'. ", Input.getKeyName(mdf, key))
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
        with let topMenu = MenuCtrl.createBar(parent, owner: self)
        {
            .bringToFront()
            .dockStyle = .Top
            .margin.y = -parent.padding.y
            .backgroundStyle = .Transparent
        }

        with let fileMenu = MenuCtrl.createPopup(parent, owner: self)
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

        with let captureMenu = MenuCtrl.createPopup(parent, owner: self)
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

            with let delayMenu = MenuCtrl.createPopup(captureMenu, owner: self)
            {
                .addItem(CmdIdCaptureDelay0)
                .addSeparator()
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

        with let editMenu = MenuCtrl.createPopup(parent, owner: self)
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

        with let imageMenu = MenuCtrl.createPopup(parent, owner: self)
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

        with let viewMenu = MenuCtrl.createPopup(parent, owner: self)
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
        mtd createBtn(id: WndId, canCheck = true)
        {
            with let b = IconButton.create(editToolBar, {}, position: {0, 0, 60, 64})
            {
                .id = #self.id
                .style.addStyleSheetColors("btnIcon_CheckedBk    $btnIcon_HotBk")
                .style.addStyleSheetColors("btnIcon_CheckedHotBk $btnIcon_HotBk")
                if canCheck
                    .iconBtnFlags.add(.Checkable)
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

        //createBtn(CmdIdUndo, false)
        //createBtn(CmdIdRedo, false)
    }

    func create(parent: *Wnd)->*MainWnd
    {
        parent.surface.minSize = {1024, 512}

        var img24 = assume Image.decode(".png", Icons24)
        var img48 = assume Image.decode(".png", Icons48)
        let renderer = parent.getApp().getRenderer()

        with let res = Wnd.create'MainWnd(parent, "captme.mainwnd")
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

        with let topBar = Wnd.create'Wnd(res, {0, 0, 64, 68})
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
            .addItem({}, CmdIdRecentSmall, .Checkable)
            .addItem({}, CmdIdRecentMedium, .Checkable)
            .addItem({}, CmdIdRecentBig, .Checkable)
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

        var item = res.quickStyleBar.addPane(res.quickWnd, title: true)
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

        var ic = res.getTheme().getIcon24(ThemeIcons24.Search, 20)
        with res.zoomBtn = IconButton.create(res.editZoomBar, ic, {0, 0, 120, 32})
        {
            .iconBtnFlags.add(.Center)
            .textPos = .Right
            .arrowPos = .RightUp
            .sigPressed += closure|res|(btn) {
                var pos = btn.getSurfaceRect()
                var menu = MenuCtrl.createPopup(res)
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
                var pos = btn.getSurfaceRect()
                var menu = MenuCtrl.createPopup(res)
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
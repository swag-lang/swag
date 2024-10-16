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
    using wnd:          Wnd

    icons24:            ImageList
    icons48:            ImageList
    appIcon:            ImageList
    topBar:             *Wnd
    editToolBar:        *Wnd
    editWnd:            *EditWnd
    quickWnd:           *QuickWnd
    propWnd:            *PropWnd
    libWnd:             *LibraryWnd
    captureBtn:         *IconButton
    libraryBtn:         *IconButton
    zoomBtn:            *IconButton
    sizeBtn:            *IconButton
    recentWnd:          *RecentWnd
    recentBar:          *Wnd
    recentSizeBar:      *IconBar
    quickStyleBar:      *SplitterCtrl
    editZoomBar:        *StackLayoutCtrl
    monitors:           Array'Env.Monitor
    pixSelFillCol:      Color = Argb.White
    firstShow           = true

    editMode:           EditMode = .Shape
    lastEditMode:       EditMode = .Shape
    bottomSize:         BottomSize = Medium
    snap:               bool
    libraryMode:        bool
    fileDlgState:       FileDlgState

    mdlFormShape:       FormShape
    mdlFormImage:       FormImage
    mdlFormStep:        FormStep
    mdlFormArrow:       FormArrow
    mdlFormLine:        FormLine
    mdlFormPolygon:     FormPolygon
    mdlFormText:        FormText
    mdlFormDraw:        FormDraw
    mdlHighlight:       IForm
    mdlPixSel:          IForm
    mdlFavorite:        IForm
}

const Icons24 = #include("datas/icons24.png")
const Icons48 = #include("datas/icons48.png")
const AppIcon = #include("datas/appicon.png")

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
            .addItem(CmdIdExit)

            .owner        = self
            .paintByFrame = true
            .dockStyle    = .Center
            .padding      = 10
            .updateState()
            .computeLayoutPopup()
        }

        var pt = Input.getMousePos()
        menu.position.width += 2 * getThemeMetrics().surfaceWnd_ShadowSize
        menu.position.height += 2 * getThemeMetrics().surfaceWnd_ShadowSize
        sf.setPosition({pt.x, pt.y, menu.position.width, menu.position.height})
        sf.constaintPositionToScreen()
        sf.wnd.view.backgroundStyle = .Window

        sf.show()
        sf.bringToTop()
        while !menu.endModal:
            surface.app.runFrame()
        sf.hide()
        if menu.endModalResult:
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
        switch evt.kind
        {
        case Close:
            evt.accepted = true
            if g_EditorOptions.hideOnClose:
                surface.hide()
            else:
                postCommandEvent(CmdIdExit)
        }
    }

    mtd impl onDestroyEvent(evt: *DestroyEvent)
    {
        if !firstShow:
            catch g_App.saveState()
    }

    mtd impl onStateEvent(evt: *StateEvent)
    {
        if evt.kind == .Show
        {
            if firstShow
            {
                firstShow = false
                catch g_App.loadState()
                surface.constaintPositionToScreen(topLeft: true, margin: 10)
                recentWnd.loadRecent()
            }

            evt.accepted = true
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
                .serialize("MainWnd.RightBar.Pane0", &quickStyleBar.items[0].size)
                .serialize("MainWnd.RightBar.Pane1", &quickStyleBar.items[1].size)
                .serialize("MainWnd.captureOptions", &g_CaptureOptions)
                .serialize("MainWnd.editorOptions", &g_EditorOptions)
            }
        }

        if evt.ser.isReading()
        {
            quickStyleBar.sendResizeEvent()
            setBottomSize(bottomSize)
            setTopBarHeight()
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
    mtd updateModel(f: IForm)
    { 
        let t    = @kindof(f)
        let form = cast(*BaseForm) @dataof(f)
        if form.subKind == .Highlight:
            mdlHighlight = f
        elif form.subKind == .PixSel:
            mdlPixSel = f

        #[Swag.Complete]
        switch t
        {
        case FormShape:
            let b = cast(*FormShape) @dataof(f)
            mdlFormShape = dref b

        case FormImage:
            let b = cast(*FormImage) @dataof(f)
            mdlFormImage = dref b

        case FormStep:
            let b = cast(*FormStep) @dataof(f)
            mdlFormStep    = dref b
            mdlFormStep.ws = b.w
            mdlFormStep.hs = b.h

        case FormArrow:
            let b = cast(*FormArrow) @dataof(f)
            mdlFormArrow = dref b

        case FormLine:
            let b = cast(*FormLine) @dataof(f)
            mdlFormLine = dref b

        case FormPolygon:
            let b = cast(*FormPolygon) @dataof(f)
            mdlFormPolygon = dref b

        case FormText:
            let b = cast(*FormText) @dataof(f)
            mdlFormText = dref b

        case FormDraw:
            let b = cast(*FormDraw) @dataof(f)
            mdlFormDraw = dref b
        }               
    }

    mtd updateEditMode(f: IForm)
    {
        let t    = @kindof(f)
        let form = cast(*BaseForm) @dataof(f)

        if editMode == .Favorites
        {
            #[Swag.Complete]
            switch t
            {
            case FormShape:
                mdlFavorite = mdlFormShape
            case FormImage:
                mdlFavorite = mdlFormImage
            case FormStep:
                mdlFavorite = mdlFormStep
            case FormArrow:
                mdlFavorite = mdlFormArrow
            case FormLine:
                mdlFavorite = mdlFormLine
            case FormPolygon:
                mdlFavorite = mdlFormPolygon
            case FormText:
                mdlFavorite = mdlFormText
            case FormDraw:
                mdlFavorite = mdlFormDraw
            }
        }
        elif form.subKind == .Highlight
        {
            editMode = .Highlight
        }
        elif form.subKind == .PixSel
        {
            editMode = .PixSel
        }
        else
        {
            #[Swag.Complete]
            switch t
            {
            case FormShape:
                editMode = .Shape
            case FormImage:
                editMode = .Image
            case FormStep:
                editMode = .Step
            case FormArrow:
                editMode = .Arrow
            case FormLine:
                editMode = .Line
            case FormPolygon:
                editMode = .Polygon
            case FormText:
                editMode = .Text
            case FormDraw:
                editMode = .Draw
            }
        }
    }

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
        if editMode == .Favorites:
            kindOf = @kindof(mdlFavorite)
        elif editMode == .Highlight:
            kindOf = @kindof(mdlHighlight)
        elif editMode == .PixSel:
            kindOf = @kindof(mdlPixSel)

        if kindOf
        {
            #[Swag.Complete]
            switch kindOf
            {
            case FormShape:
                return EditMode.Shape
            case FormArrow:
                return EditMode.Arrow
            case FormLine:
                return EditMode.Line
            case FormPolygon:
                return EditMode.Polygon
            case FormText:
                return EditMode.Text
            case FormDraw:
                return EditMode.Draw
            case FormStep:
                return EditMode.Step
            case FormImage:
                return EditMode.Image
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
        if !capture or !sizeBtn:
            return
        sizeBtn.name = Format.toString("% x %", capture.width, capture.height)
        sizeBtn.invalidate()
    }

    mtd registerActions()
    {
        registerAction'ActionAlign()
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
        registerAction'ActionAbout()
        registerAction'ActionOptions()
        registerAction'ActionView()
    }

    mtd registerShortcuts()
    {
        registerKeyShortcut(.Control, .C, CmdIdCopy)
        registerKeyShortcut(.Control, .V, CmdIdPaste)
        registerKeyShortcut(.Control, .Insert, CmdIdCopy)
        registerKeyShortcut(.Shift, .Insert, CmdIdPaste)
        registerKeyShortcut(.Control, .Z, CmdIdUndo)
        registerKeyShortcut(.Control, .Y, CmdIdRedo)
        registerKeyShortcut(.CtrlShift, .F, CmdIdBringForward)
        registerKeyShortcut(.CtrlShift, .B, CmdIdSendBackward)
        registerKeyShortcut(.Zero, .Delete, CmdIdDeleteSelection)
        registerKeyShortcut(.CtrlShift, .A, CmdIdCopyAsImage)
        registerKeyShortcut(.Control, .A, CmdIdSelectAll)
        registerKeyShortcut(.Control, .D, CmdIdDeSelectAll)
        registerKeyShortcut(.Control, .Add, CmdIdZoomIn)
        registerKeyShortcut(.Control, .Subtract, CmdIdZoomOut)
        registerKeyShortcut(.Control, .NumPad0, CmdIdZoom1)
        registerKeyShortcut(.CtrlShift, .D, CmdIdDupCapture)
        registerKeyShortcut(.CtrlShift, .I, CmdIdInvertSelection)
        registerKeyShortcut(.CtrlShift, .T, CmdIdFlattenAll)
        registerKeyShortcut(.Alt, .F4, CmdIdExit)
        registerKeyShortcut(.Control, .N, CmdIdNewImage)
        registerKeyShortcut(.CtrlShift, .N, CmdIdNewFromClipboard)
        registerKeyShortcut(.Control, .S, CmdIdSaveAsImage)
        registerKeyShortcut(.CtrlShift, .S, CmdIdUpdateLinkedImage)
        registerKeyShortcut(.Control, .O, CmdIdOpenImage)
        registerKeyShortcut(.Shift, .Left, CmdIdMoveSelLeft1)
        registerKeyShortcut(.Shift, .Up, CmdIdMoveSelTop1)
        registerKeyShortcut(.Shift, .Right, CmdIdMoveSelRight1)
        registerKeyShortcut(.Shift, .Down, CmdIdMoveSelBottom1)
        registerKeyShortcut(.Zero, .Left, CmdIdMoveSelLeft)
        registerKeyShortcut(.Zero, .Up, CmdIdMoveSelTop)
        registerKeyShortcut(.Zero, .Right, CmdIdMoveSelRight)
        registerKeyShortcut(.Zero, .Down, CmdIdMoveSelBottom)
        registerKeyShortcut(.CtrlAlt, .O, CmdIdOptions)

        var errorReg = false

        #[Swag.Mixin]
        func registerHotKey(mdf: Input.KeyModifiers, key: Input.Key, wndId: WndId)
        {
            catch getApp().registerHotKey(mdf, key, wndId, self)
            if @err() != null
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

        registerHotKey(.Zero, .PrintScreen, CmdIdCaptureArea)
        registerHotKey(.CtrlShift, .PrintScreen, CmdIdCaptureLastArea)
        registerHotKey(.Control, .PrintScreen, CmdIdCaptureFullScreen)
        registerHotKey(.Alt, .PrintScreen, CmdIdCaptureActiveWnd)
    }

    mtd createTopMenu()
    {
        with let topMenu = MenuCtrl.createBar(parent, owner: self)
        {
            .bringToFront()
            .dockStyle       = .Top
            .margin.y        = -parent.padding.y
            .backgroundStyle = .Transparent
        }

        with let fileMenu = MenuCtrl.createPopup(parent, owner: self)
        {
            .addItem(CmdIdNewImage)
            .addItem(CmdIdNewFromClipboard)
            .addSeparator()
            .addItem(CmdIdOpenImage)
            .addItem(CmdIdSaveAsImage)
            .addSeparator()
            .addItem(CmdIdSaveAsLinkedImage)
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

            foreach m, idx in monitors
            {
                switch idx
                {
                case 0:
                    .addItem(CmdIdCaptureScreen0)
                case 1:
                    .addItem(CmdIdCaptureScreen1)
                case 2:
                    .addItem(CmdIdCaptureScreen2)
                case 3:
                    .addItem(CmdIdCaptureScreen3)
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
            .addSeparator()
            .addItem(CmdIdFlipImage)
            .addItem(CmdIdMirrorImage)
            .addItem(CmdIdRotateImageRight)
            .addItem(CmdIdRotateImageLeft)
            topMenu.addPopup("Image", imageMenu)
        }

        with let viewMenu = MenuCtrl.createPopup(parent, owner: self)
        {
            .addItem(CmdIdZoomIn)
            .addItem(CmdIdZoomOut)
            .addSeparator()
            .addItem(CmdIdZoom1)
            .addItem(CmdIdZoomFit)
            .addSeparator()
            .addItem(CmdIdViewClip)
            topMenu.addPopup("View", viewMenu)
        }

        with let helpMenu = MenuCtrl.createPopup(parent, owner: self)
        {
            .addItem(CmdIdOptions)
            .addItem(CmdIdAbout)
            topMenu.addPopup("Help", helpMenu)
        }
    }

    mtd createToolbar()
    {
        mtd createBtn(id: WndId, canCheck = true)
        {
            with let b = IconButton.create(editToolBar, {}, position: {0, 0, 60, 64})
            {
                .id = #self.id
                .style.addStyleSheetColors("btnIcon_CheckedBk    $btnIcon_HotBk")
                .style.addStyleSheetColors("btnIcon_CheckedHotBk $btnIcon_HotBk")
                if canCheck:
                    .iconBtnFlags.add(.Checkable)
                .checkedForm = .Bottom
                .iconSizeX, .iconSizeY = 48
            }
        }

        with editToolBar = StackLayoutCtrl.create(topBar, .HorzCenter, {0, 0, 64, 64})
        {
            .dockStyle       = .Top
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

        captureBtn = IconButton.create(topBar, Icon.from(&icons48, 6), {0, 0, 64, 64}, id: CmdIdCapture)
        libraryBtn = IconButton.create(topBar, Icon.from(&icons48, 8), {64, 0, 64, 64}, id: CmdIdLibrary)
    }

    mtd setTopBarHeight()
    {
        var icons: Array'(*IconButton)

        foreach c in topBar.childs
        {
            if c.type == IconButton:
                icons.add(cast(*IconButton) c)
        }

        foreach c in editToolBar.childs
        {
            if c.type == IconButton:
                icons.add(cast(*IconButton) c)
        }

        foreach c in icons
        {
            if g_EditorOptions.showToolBarLabels
            {
                c.iconBtnFlags.remove(.HideName)
                c.position.width, c.position.height = 64
                c.iconSizeX, c.iconSizeY = 48
                libraryBtn.position.x = 64
            }
            else
            {
                c.iconBtnFlags.add(.HideName)
                c.position.width, c.position.height = 40
                c.iconSizeX, c.iconSizeY = 32
                libraryBtn.position.x = 48
            }
        }

        topBar.position.height = g_EditorOptions.showToolBarLabels ? 68 : 50
        editWnd.sendResizeEvent()
        topBar.sendResizeEvent()
        editToolBar.sendResizeEvent()
        applyLayout()
    }

    func create(parent: *Wnd)->*MainWnd
    {
        parent.surface.minSize = {1024, 512}

        var img24  = assume Image.decode(".png", Icons24)
        var img48  = assume Image.decode(".png", Icons48)
        var imgApp = assume Image.decode(".png", AppIcon)

        let renderer = parent.getApp().getRenderer()

        with let res = Wnd.create'MainWnd(parent, "captme.mainwnd")
        {
            .icons24.set(renderer.addImage(img24), 24, 24)
            .icons48.set(renderer.addImage(img48), 48, 48)
            .appIcon.set(renderer.addImage(imgApp))
            .backgroundStyle = .Window
            .mdlFavorite     = &res.mdlFormShape
            .mdlHighlight    = &res.mdlFormShape
            .mdlPixSel       = &res.mdlFormShape
            .monitors        = Env.getMonitors()
        }

        with res.topBar = Wnd.create'Wnd(res, {0, 0, 64, 68})
        {
            .dockStyle       = .Top
            .backgroundStyle = .Window
            .style.addStyleSheetColors("wnd_Bk $wnd_Caption")
        }

        res.createToolbar()

        // Main edit area
        with res.editWnd = EditWnd.create(res, res)
        {
            .dockStyle = .Center
            .sigScrollSizeChanged += func|res|(scroll, old, new)
            {
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
            .dockStyle   = .Left
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
            .dockStyle       = .Bottom
            .backgroundStyle = .Window
        }

        var ic = res.getTheme().getIcon24(.Search, 20)
        with res.zoomBtn = IconButton.create(res.editZoomBar, ic, {0, 0, 120, 32})
        {
            .iconBtnFlags.add(.Center)
            .textPos  = .Right
            .arrowPos = .RightUp
            .sigPressed += func|res|(btn)
            {
                var pos  = btn.getSurfaceRect()
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
            .textPos  = .Right
            .arrowPos = .RightUp
            .sigPressed += func|res|(btn)
            {
                var pos  = btn.getSurfaceRect()
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
            .setTopBarHeight()
            .updateState()
            .libWnd = LibraryWnd.create(res)
            .libWnd.hide()
        }

        g_Main = res
        return res
    }
}

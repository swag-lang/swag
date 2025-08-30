#global marked
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
    using wnd:               Wnd

    inPlaceEdit:             bool
    icons24:                 ImageList
    icons48:                 ImageList
    appIcon:                 ImageList
    topBar:                  *Wnd
    editToolBar:             *Wnd
    editWnd:                 *EditWnd
    quickWnd:                *QuickWnd
    propWnd:                 *PropWnd
    libWnd:                  *LibraryWnd
    captureBtn:              *IconButton
    libraryBtn:              *IconButton
    zoomBtn:                 *IconButton
    sizeBtn:                 *IconButton
    recentWnd:               *RecentWnd
    recentBar:               *Wnd
    recentSizeBar:           *IconBar
    quickStyleBar:           *SplitterCtrl
    editZoomBar:             *StackLayoutCtrl
    monitors:                Array'Env.Monitor
    pixSelFillCol:           Color = Argb.White
    firstShow                = true
    embInfoFreeze:           *EmbInfoCtrl

    editMode:                EditMode = .Shape
    bottomSize:              BottomSize = Medium
    snap:                    bool
    freezeSel:               bool
    libraryMode:             bool
    fileDlgState:            FileDlgState
    fileDlgStateInPlace:     FileDlgState
    captureOptions:          CaptureOptions
    editorOptions:           EditorOptions

    mdlFormShape:            FormShape
    mdlFormImage:            FormImage
    mdlFormStep:             FormStep
    mdlFormArrow:            FormArrow
    mdlFormLine:             FormLine
    mdlFormPolygon:          FormPolygon
    mdlFormText:             FormText
    mdlFormDraw:             FormDraw
    mdlHighlight:            IForm
    mdlPixSel:               IForm
    mdlFavorite:             IForm
}

const Icons24 = #include("datas/icons24.png")
const Icons48 = #include("datas/icons48.png")
const AppIcon = #include("datas/appicon.png")

impl IWnd for MainWnd
{
    mtd popupBar()
    {
        let sf = catch me.surface.app.createSurface(500, 400, 300, 300, .Secondary | .TopMost)
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

            .owner        = me
            .paintByFrame = true
            .dockStyle    = .Center
            .padding      = 10
            .updateState()
            .computeLayoutPopup()
        }

        var pt = Input.getMousePos()
        menu.position.width += 2 * me.getThemeMetrics().surfaceWnd_ShadowSize
        menu.position.height += 2 * me.getThemeMetrics().surfaceWnd_ShadowSize
        sf.setPosition({pt.x, pt.y, menu.position.width, menu.position.height})
        sf.constaintPositionToScreen()
        sf.wnd.view.backgroundStyle = .Window

        sf.show()
        sf.bringToTop()
        while !menu.endModal:
            me.surface.app.runFrame()
        sf.hide()
        if menu.endModalResult:
            me.postCommandEvent(menu.endModalResult)
        sf.destroy()
    }

    mtd impl onSysUserEvent(evt: *SysUserEvent)
    {
        #if #os == Swag.TargetOs.Windows
        {
            switch evt.userMsg
            {
            case Win32.WM_USER:
                if evt.param1 == Win32.WM_LBUTTONDBLCLK
                {
                    me.surface.show()
                    me.surface.bringToTop()
                }
                elif evt.param1 == Win32.WM_RBUTTONDOWN
                {
                    me.popupBar()
                }

            case Win32.WM_USER + 1:
                if !Clipboard.hasString():
                    break
                me.loadExternalImage(Clipboard.getString())
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
            if me.editorOptions.hideOnClose:
                me.surface.hide()
            else:
                me.postCommandEvent(CmdIdExit)
        }
    }

    mtd impl onDestroyEvent(evt: *DestroyEvent)
    {
        if !me.firstShow:
            catch g_App.saveState()
    }

    mtd impl onStateEvent(evt: *StateEvent)
    {
        if evt.kind == .Show
        {
            if me.firstShow
            {
                me.firstShow = false
                catch g_App.loadState()
                me.surface.constaintPositionToScreen(topLeft: true, margin: 10)
                me.recentWnd.loadRecent()
            }

            evt.accepted = true
        }
    }

    mtd impl onSerializeStateEvent(evt: *SerializeStateEvent)
    {
        if me.inPlaceEdit:
            return
        catch
        {
            with evt.ser
            {
                .serialize("MainWnd.editMode", &me.editMode)
                .serialize("MainWnd.snap", &me.snap)
                .serialize("MainWnd.bottomSize", &me.bottomSize)
                .serialize("MainWnd.fileDlgState", &me.fileDlgState)
                .serialize("MainWnd.fileDlgStateInPlace", &me.fileDlgStateInPlace)
                .serialize("MainWnd.RightBar.Pane0", &me.quickStyleBar.items[0].size)
                .serialize("MainWnd.RightBar.Pane1", &me.quickStyleBar.items[1].size)
                .serialize("MainWnd.captureOptions", &me.captureOptions)
                .serialize("MainWnd.editorOptions", &me.editorOptions)
            }
        }

        if evt.ser.isReading()
        {
            me.quickStyleBar.sendResizeEvent()
            me.setBottomSize(me.bottomSize)
            me.setTopBarHeight()
            me.updateState()
        }
    }

    mtd impl onFrameEvent(evt: *FrameEvent)
    {
        if evt.firstFrame
        {
            me.quickWnd.setDefault()
        }
    }
}

impl MainWnd
{
    mtd loadExternalImage(arg: string)
    {
        let imgToLoad = catch Image.load(arg)
        if @err != null
        {
            let str = Format.toString("Failed to load file: %", arg)
            MessageDlg.error(me.surface, str)
            return
        }

        var done = false
        foreach md in imgToLoad.metaDatas
        {
            if md.isTag("Captme.File")
            {
                let dupCapture = catch me.recentWnd.addRecent(cast(string) md.toSlice(), true, true)
                Memory.delete(dupCapture)
                done = true
                break
            }
        }

        if !done
        {
            me.newCapture(imgToLoad, arg)
        }

        if me.libraryMode
        {
            me.swapLibraryMode()
            me.updateState()
        }

        me.surface.bringToTop()
    }

    mtd getEditMode()->EditMode
    {
        switch me.editMode
        {
        case Selection:
            me.editMode = .Shape
            let capture = me.getCapture()
            if capture:
                capture.updateEditModeFromSelection()
            let edMode = me.editMode
            me.editMode = .Selection
            return edMode

        case Favorites:
            return formToEditMode(me.mdlFavorite)
        case Highlight:
            return formToEditMode(me.mdlHighlight)
        case PixSel:
            return formToEditMode(me.mdlPixSel)
        }

        return me.editMode
    }

    mtd freezeSelection(freeze: bool)
    {
        if !freeze and me.embInfoFreeze
        {
            me.embInfoFreeze.destroy()
            me.embInfoFreeze = null
            me.freezeSel     = false
        }
        elif freeze and !me.embInfoFreeze
        {
            me.embInfoFreeze = EmbInfoCtrl.create(me.editWnd.view, "Selection is freezed", .Message, flags: .Bottom)
            me.embInfoFreeze.sigOnClose += mtd||(ctrl)
            {
                me.freezeSel     = false
                me.embInfoFreeze = null
            }

            let capture = assume me.getCapture()
            capture.selection.clear()
            capture.selectionHasChanged()
            me.freezeSel = true

            foreach it in capture.forms
            {
                let p = cast(*BaseForm) @dataof(it)
                p.freeze = true
            }
        }
    }

    mtd updateState()
    {
        me.updateCommandState()
        if me.propWnd:
            me.propWnd.selectionHasChanged()
        if me.quickWnd:
            me.quickWnd.updateQuickStyles()
    }

    mtd captureHasChanged()
    {
        me.updateSizeDisplay()
        if me.recentWnd
        {
            me.editToolBar.updateCommandState()
            me.updateRecent()
        }
    }

    mtd updateRecent()
    {
        if !me.recentWnd:
            return
        me.recentWnd.updateRecent()
    }

    mtd getCapture()->#null *Capture
    {
        if me.libraryMode:
            return me.libWnd.getCapture()
        return me.editWnd.getCapture()
    }

    mtd updateZoomDisplay()
    {
        let z = cast(s32) (me.editWnd.editView.zoom * 100)
        me.zoomBtn.name = Format.toString("% %%", z)
        me.zoomBtn.invalidate()
    }

    mtd updateSizeDisplay()
    {
        let capture = me.getCapture()
        if !capture or !me.sizeBtn:
            return
        me.sizeBtn.name = Format.toString("% x %", capture.width, capture.height)
        me.sizeBtn.invalidate()
    }

    mtd registerActions()
    {
        with me
        {
            .registerAction'ActionAlign()
            .registerAction'ActionUndo()
            .registerAction'ActionDelay()
            .registerAction'ActionPixSel()
            .registerAction'ActionCapture()
            .registerAction'ActionMoveSel()
            .registerAction'ActionToolMode()
            .registerAction'ActionCopyPaste()
            .registerAction'ActionFile()
            .registerAction'ActionZoom()
            .registerAction'ActionImage()
            .registerAction'ActionSelection()
            .registerAction'ActionFlatten()
            .registerAction'ActionRecentSize()
            .registerAction'ActionQuickStyle()
            .registerAction'ActionEdit()
            .registerAction'ActionGlobal()
            .registerAction'ActionAbout()
            .registerAction'ActionOptions()
            .registerAction'ActionView()
        }
    }

    mtd registerShortcuts()
    {
        with me
        {
            .registerKeyShortcut(.Control, .C, CmdIdCopy)
            .registerKeyShortcut(.Control, .X, CmdIdCut)
            .registerKeyShortcut(.Control, .V, CmdIdPaste)
            .registerKeyShortcut(.Control, .Insert, CmdIdCopy)
            .registerKeyShortcut(.Shift, .Insert, CmdIdPaste)
            .registerKeyShortcut(.Control, .Z, CmdIdUndo)
            .registerKeyShortcut(.Control, .Y, CmdIdRedo)
            .registerKeyShortcut(.Zero, .Home, CmdIdSendToFront)
            .registerKeyShortcut(.Zero, .End, CmdIdSendToBack)
            .registerKeyShortcut(.Zero, .PageUp, CmdIdMoveForward)
            .registerKeyShortcut(.Zero, .PageDown, CmdIdMoveBackward)
            .registerKeyShortcut(.Zero, .Delete, CmdIdDeleteSelection)
            .registerKeyShortcut(.CtrlShift, .A, CmdIdCopyAsImage)
            .registerKeyShortcut(.Control, .Space, CmdIdFreeze)
            .registerKeyShortcut(.Control, .A, CmdIdSelectAll)
            .registerKeyShortcut(.Control, .D, CmdIdDeSelectAll)
            .registerKeyShortcut(.Control, .Add, CmdIdZoomIn)
            .registerKeyShortcut(.Control, .Subtract, CmdIdZoomOut)
            .registerKeyShortcut(.Control, .NumPad0, CmdIdZoom1)
            .registerKeyShortcut(.CtrlShift, .D, CmdIdDupCapture)
            .registerKeyShortcut(.CtrlShift, .I, CmdIdInvertSelection)
            .registerKeyShortcut(.CtrlShift, .T, CmdIdFlattenAll)
            .registerKeyShortcut(.Alt, .F4, CmdIdExit)
            .registerKeyShortcut(.Control, .N, CmdIdNewImage)
            .registerKeyShortcut(.CtrlShift, .N, CmdIdNewFromClipboard)
            .registerKeyShortcut(.CtrlShift, .S, CmdIdSaveAsImage)
            .registerKeyShortcut(.Control, .S, CmdIdUpdateLinkedImage)
            .registerKeyShortcut(.Control, .O, CmdIdOpenImage)
            .registerKeyShortcut(.Shift, .Left, CmdIdMoveSelLeft1)
            .registerKeyShortcut(.Shift, .Up, CmdIdMoveSelTop1)
            .registerKeyShortcut(.Shift, .Right, CmdIdMoveSelRight1)
            .registerKeyShortcut(.Shift, .Down, CmdIdMoveSelBottom1)
            .registerKeyShortcut(.Zero, .Left, CmdIdMoveSelLeft)
            .registerKeyShortcut(.Zero, .Up, CmdIdMoveSelTop)
            .registerKeyShortcut(.Zero, .Right, CmdIdMoveSelRight)
            .registerKeyShortcut(.Zero, .Down, CmdIdMoveSelBottom)
            .registerKeyShortcut(.CtrlAlt, .O, CmdIdOptions)
            .registerKeyShortcut(.Zero, .F2, CmdIdEditText)
        }

        var errorReg = false

        #[Swag.Mixin]
        func registerHotKey(mdf: Input.KeyModifiers, key: Input.Key, wndId: WndId)
        {
            catch me.getApp().registerHotKey(mdf, key, wndId, me)
            if @err != null
            {
                if !errorReg
                {
                    var msg = Format.toString("Cannot register global shortcut '%'. ", Input.getKeyName(mdf, key))
                    msg += "Another running application is probably already using it !"
                    discard EmbInfoCtrl.create(me.editWnd, msg, .Critical)
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
        with let topMenu = MenuCtrl.createBar(me.parent, owner: me)
        {
            .bringToFront()
            .dockStyle       = .Top
            .margin.y        = -me.parent.padding.y
            .backgroundStyle = .Transparent
        }

        with let fileMenu = MenuCtrl.createPopup(me.parent, owner: me)
        {
            .addItem(CmdIdNewImage)
            .addItem(CmdIdNewFromClipboard)
            .addSeparator()
            .addItem(CmdIdOpenImage)
            .addItem(CmdIdSaveAsImage)
            .addSeparator()
            .addItem(CmdIdSaveAsLinkedImage)
            .addItem(CmdIdUpdateLinkedImage)
            .addItem(CmdIdUnlinkImage)
            .addSeparator()
            .addItem(CmdIdFileDetails)
            .addSeparator()
            .addItem(CmdIdExit)
            topMenu.addPopup("File", fileMenu)
        }

        with let captureMenu = MenuCtrl.createPopup(me.parent, owner: me)
        {
            .addItem(CmdIdCaptureArea)
            .addItem(CmdIdCaptureLastArea)
            .addItem(CmdIdCaptureFullScreen)
            .addItem(CmdIdCaptureActiveWnd)

            foreach m, idx in me.monitors
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

            with let delayMenu = MenuCtrl.createPopup(captureMenu, owner: me)
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
            .addItem(CmdIdCaptureToInPlace)
            .addItem(CmdIdCaptureToEditor)
            .addItem(CmdIdCaptureToClipboard)
        }

        with let editMenu = MenuCtrl.createPopup(me.parent, owner: me)
        {
            .addItem(CmdIdUndo)
            .addItem(CmdIdRedo)
            .addSeparator()
            .addItem(CmdIdCopy)
            .addItem(CmdIdCut)
            .addItem(CmdIdPaste)
            .addSeparator()
            .addItem(CmdIdSelectAll)
            .addItem(CmdIdSelectSameAll)
            .addItem(CmdIdDeSelectAll)
            .addItem(CmdIdInvertSelection)
            .addSeparator()
            .addItem(CmdIdFreeze)
            .addSeparator()
            .addItem(CmdIdSnap)
            topMenu.addPopup("Edit", editMenu)
        }

        with let imageMenu = MenuCtrl.createPopup(me.parent, owner: me)
        {
            .addItem(CmdIdCopyAsImage)
            .addSeparator()
            .addItem(CmdIdRestoreOrg)
            .addSeparator()
            .addItem(CmdIdResizeImage)
            .addItem(CmdIdExpandImage)
            .addItem(CmdIdExpandCapture)
            .addItem(CmdIdTrimCapture)
            .addSeparator()
            .addItem(CmdIdFlattenAll)
            .addSeparator()
            .addItem(CmdIdFlipImage)
            .addItem(CmdIdMirrorImage)
            .addItem(CmdIdRotateImageRight)
            .addItem(CmdIdRotateImageLeft)
            topMenu.addPopup("Image", imageMenu)
        }

        with let viewMenu = MenuCtrl.createPopup(me.parent, owner: me)
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

        with let helpMenu = MenuCtrl.createPopup(me.parent, owner: me)
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
            with let b = IconButton.create(me.editToolBar, {}, position: {0, 0, 60, 64})
            {
                .id = #me.id
                .style.addStyleSheetColors("btnIcon_CheckedBk    $btnIcon_HotBk")
                .style.addStyleSheetColors("btnIcon_CheckedHotBk $btnIcon_HotBk")
                if canCheck:
                    .iconBtnFlags.add(.Checkable)
                .checkedForm = .Bottom
                .iconSizeX, .iconSizeY = 48
            }
        }

        with me.editToolBar = StackLayoutCtrl.create(me.topBar, .HorzCenter, {0, 0, 64, 64})
        {
            .dockStyle       = .Top
            .backgroundStyle = .Window
            .style.addStyleSheetColors("wnd_Bk $wnd_Caption")
        }

        me.createBtn(CmdIdModeFavorites)
        me.createBtn(CmdIdModeSelection)
        me.createBtn(CmdIdModeShape)
        me.createBtn(CmdIdModeArrow)
        me.createBtn(CmdIdModeLine)
        me.createBtn(CmdIdModePolygon)
        me.createBtn(CmdIdModeText)
        me.createBtn(CmdIdModeDraw)
        me.createBtn(CmdIdModeHighlight)
        me.createBtn(CmdIdModeStep)
        me.createBtn(CmdIdModeImage)
        me.createBtn(CmdIdModePixSel)

        me.captureBtn = IconButton.create(me.topBar, Icon.from(&me.icons48, 6), {0, 0, 64, 64}, id: CmdIdCapture)
        me.libraryBtn = IconButton.create(me.topBar, Icon.from(&me.icons48, 8), {64, 0, 64, 64}, id: CmdIdLibrary)
    }

    mtd setTopBarHeight()
    {
        var icons: Array'(*IconButton)

        foreach c in me.topBar.childs
        {
            if c.type == IconButton:
                icons.add(cast(*IconButton) c)
        }

        foreach c in me.editToolBar.childs
        {
            if c.type == IconButton:
                icons.add(cast(*IconButton) c)
        }

        foreach c in icons
        {
            if me.editorOptions.showToolBarLabels
            {
                c.iconBtnFlags.remove(.HideName)
                c.position.width, c.position.height = 64
                c.iconSizeX, c.iconSizeY = 48
                me.libraryBtn.position.x = 64
            }
            else
            {
                c.iconBtnFlags.add(.HideName)
                c.position.width, c.position.height = 40
                c.iconSizeX, c.iconSizeY = 32
                me.libraryBtn.position.x = 48
            }
        }

        me.topBar.position.height = me.editorOptions.showToolBarLabels ? 68 : 50
        me.editWnd.sendResizeEvent()
        me.topBar.sendResizeEvent()
        me.editToolBar.sendResizeEvent()
        me.applyLayout()
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
                menu.addItem(CmdIdExpandImage)
                menu.addItem(CmdIdExpandCapture)
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

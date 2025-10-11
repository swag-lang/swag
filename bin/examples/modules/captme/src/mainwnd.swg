// Global variable
var g_Main: *MainWnd

// Constants
const ClipboardFormat = "Swag.CaptMe"
const Icons24         = #include("datas/icons24.png")
const Icons48         = #include("datas/icons48.png")
const AppIcon         = #include("datas/appicon.png")

// Enums
enum EditMode { Shape, Arrow, Line, Polygon, Text, Favorites, Draw, Highlight, Selection, Step, Image, PixSel }

enum BottomSize
{
    Small
    Medium
    Big
}

// Main structure
struct MainWnd
{
    using wnd:                                    Wnd
    topBar, editToolBar, recentBar:               *Wnd
    editWnd:                                      *EditWnd
    quickWnd:                                     *QuickWnd
    propWnd:                                      *PropWnd
    libWnd:                                       *LibraryWnd
    recentWnd:                                    *RecentWnd
    recentSizeBar:                                *IconBar
    quickStyleBar:                                *SplitterCtrl
    editZoomBar:                                  *StackLayoutCtrl
    captureBtn, libraryBtn, zoomBtn, sizeBtn:     *IconButton
    embInfoFreeze:                                *EmbInfoCtrl
    icons24, icons48, appIcon:                    ImageList
    monitors:                                     Array'Env.Monitor

    // State
    editMode:                                      EditMode = .Shape
    bottomSize:                                    BottomSize = Medium
    pixSelFillCol:                                 Color = Argb.White
    inPlaceEdit, snap, freezeSel, libraryMode:     bool
    firstShow                                      = true
    fileDlgState, fileDlgStateInPlace:             FileDlgState
    captureOptions:                                CaptureOptions
    editorOptions:                                 EditorOptions

    // Form models
    mdlFormShape:                             FormShape
    mdlFormImage:                             FormImage
    mdlFormStep:                              FormStep
    mdlFormArrow:                             FormArrow
    mdlFormLine:                              FormLine
    mdlFormPolygon:                           FormPolygon
    mdlFormText:                              FormText
    mdlFormDraw:                              FormDraw
    mdlHighlight, mdlPixSel, mdlFavorite:     IForm
}

// Interface implementation
impl IWnd for MainWnd
{
    mtd popupBar()
    {
        let sf = catch .surface.app.createSurface(500, 400, 300, 300, .Secondary | .TopMost)
        with let menu = Wnd.create'MenuCtrl(sf.wnd.view, cast(WndId) null)
        {
            .addItem(ActionGlobal.ShowEditor)
            .addSeparator()
            .addItem(ActionCapture.Area)
            .addItem(ActionCapture.LastArea)
            .addItem(ActionCapture.FullScreen)
            .addItem(ActionCapture.ActiveWnd)

            for idx in me.monitors
            {
                switch idx
                {
                case 0:
                    .addItem(ActionCapture.Screen0)
                case 1:
                    .addItem(ActionCapture.Screen1)
                case 2:
                    .addItem(ActionCapture.Screen2)
                case 3:
                    .addItem(ActionCapture.Screen3)
                }
            }

            .addSeparator()
            .addItem(ActionGlobal.Exit)
            .owner        = me
            .paintByFrame = true
            .dockStyle    = .Center
            .padding      = 10
            .updateState()
            .computeLayoutPopup()
        }

        let pt         = Input.getMousePos()
        let shadowSize = .getThemeMetrics().surfaceWnd_ShadowSize
        menu.position.width += 2 * shadowSize
        menu.position.height += 2 * shadowSize
        sf.setPosition({pt.x, pt.y, menu.position.width, menu.position.height})
        sf.constaintPositionToScreen()
        sf.wnd.view.backgroundStyle = .Window

        sf.show()
        sf.bringToTop()
        while !menu.endModal do
            .surface.app.runFrame()
        sf.hide()
        if menu.endModalResult do
            .postCommandEvent(menu.endModalResult)
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
                    .surface.show()
                    .surface.bringToTop()
                }
                elif evt.param1 == Win32.WM_RBUTTONDOWN do
                    .popupBar()

            case Win32.WM_USER + 1:
                if Clipboard.hasString() do
                    .loadExternalImage(Clipboard.getString())
            }
        }
    }

    mtd impl onSysCommandEvent(evt: *SysCommandEvent)
    {
        evt.accepted = false
        if evt.kind == .Close
        {
            evt.accepted = true
            if .editorOptions.hideOnClose do
                .surface.hide()
            else do
                .postCommandEvent(ActionGlobal.Exit)
        }
    }

    mtd impl onDestroyEvent(evt: *DestroyEvent)
    {
        if !.firstShow do
            catch g_App.saveState()
    }

    mtd impl onStateEvent(evt: *StateEvent)
    {
        if evt.kind == .Show
        {
            if .firstShow
            {
                .firstShow = false
                catch g_App.loadState()
                .surface.constaintPositionToScreen(topLeft: true, margin: 10)
                .recentWnd.loadRecent()
            }
            evt.accepted = true
        }
    }

    mtd impl onSerializeStateEvent(evt: *SerializeStateEvent)
    {
        if .inPlaceEdit do
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
            .setTheme()
            .quickStyleBar.sendResizeEvent()
            .setBottomSize(.bottomSize)
            .setTopBarHeight()
            .updateState()
        }
    }

    mtd impl onFrameEvent(evt: *FrameEvent)
    {
        if evt.firstFrame do
            .quickWnd.setDefault()
    }
}

// Main implementation
impl MainWnd
{
    mtd loadExternalImage(arg: string)
    {
        let imgToLoad = catch Image.load(arg)
        if @err != null
        {
            MessageDlg.error(.surface, Format.toString("Failed to load file: %", arg))
            return
        }

        var done = false
        foreach md in imgToLoad.metaDatas
        {
            if md.isTag("Captme.File")
            {
                let dupCapture = catch .recentWnd.addRecent(cast(string) md.toSlice(), true, true)
                Memory.delete(dupCapture)
                done = true
                break
            }
        }

        if !done do
            .newCapture(imgToLoad, arg)

        if .libraryMode
        {
            .swapLibraryMode()
            .updateState()
        }

        .surface.bringToTop()
    }

    mtd getEditMode()->EditMode
    {
        switch .editMode
        {
        case Selection:
            .editMode = .Shape
            let capture = .getCapture()
            if capture do
                capture.updateEditModeFromSelection()
            let edMode = .editMode
            .editMode = .Selection
            return edMode
        case Favorites:
            return formToEditMode(.mdlFavorite)
        case Highlight:
            return formToEditMode(.mdlHighlight)
        case PixSel:
            return formToEditMode(.mdlPixSel)
        }
        return .editMode
    }

    mtd freezeSelection(freeze: bool)
    {
        if !freeze and .embInfoFreeze
        {
            .embInfoFreeze.destroy()
            .embInfoFreeze = null
            .freezeSel     = false
        }
        elif freeze and !.embInfoFreeze
        {
            .embInfoFreeze = EmbInfoCtrl.create(.editWnd.view, "Selection is freezed", .Message, flags: .Bottom)
            .embInfoFreeze.sigOnClose += mtd||(ctrl)
            {
                .freezeSel     = false
                .embInfoFreeze = null
            }

            let capture = assume .getCapture()
            capture.selection.clear()
            capture.selectionHasChanged()
            .freezeSel = true

            foreach it in capture.forms
            {
                let p = cast(*BaseForm) @dataof(it)
                p.freeze = true
            }
        }
    }

    mtd updateState()
    {
        .updateCommandState()
        if .propWnd do
            .propWnd.selectionHasChanged()
        if .quickWnd do
            .quickWnd.updateQuickStyles()
    }

    mtd captureHasChanged()
    {
        .updateSizeDisplay()
        if .recentWnd
        {
            .editToolBar.updateCommandState()
            .updateRecent()
        }
    }

    mtd getCapture()->#null *Capture
    {
        if .libraryMode do
            return .libWnd.getCapture()
        return .editWnd.getCapture()
    }

    mtd updateRecent()
    {
        if .recentWnd do
            .recentWnd.updateRecent()
    }

    mtd updateZoomDisplay()
    {
        let z = cast(s32) (.editWnd.editView.zoom * 100)
        .zoomBtn.name = Format.toString("% %%", z)
        .zoomBtn.invalidate()
    }

    mtd updateSizeDisplay()
    {
        let capture = .getCapture()
        if !capture or !.sizeBtn do
            return
        .sizeBtn.name = Format.toString("% x %", capture.width, capture.height)
        .sizeBtn.invalidate()
    }

    mtd registerActions()
    {
        .registerAction'ActionAlign()
        .registerAction'ActionUndo()
        .registerAction'ActionCaptureDelay()
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

    mtd registerShortcuts()
    {
        // Standard shortcuts
        .registerKeyShortcut(.Control, .C, ActionCopyPaste.Copy)
        .registerKeyShortcut(.Control, .X, ActionCopyPaste.Cut)
        .registerKeyShortcut(.Control, .V, ActionCopyPaste.Paste)
        .registerKeyShortcut(.Control, .Insert, ActionCopyPaste.Copy)
        .registerKeyShortcut(.Shift, .Insert, ActionCopyPaste.Paste)
        .registerKeyShortcut(.Control, .Z, ActionUndo.DoUndo)
        .registerKeyShortcut(.Control, .Y, ActionUndo.DoRedo)
        .registerKeyShortcut(.Zero, .Delete, ActionSelection.DeleteSelection)
        .registerKeyShortcut(.CtrlShift, .A, ActionCopyPaste.CopyAsImage)
        .registerKeyShortcut(.Control, .Space, ActionSelection.Freeze)
        .registerKeyShortcut(.Control, .A, ActionSelection.SelectAll)
        .registerKeyShortcut(.Control, .D, ActionSelection.DeSelectAll)
        .registerKeyShortcut(.CtrlShift, .D, ActionFile.DupCapture)
        .registerKeyShortcut(.CtrlShift, .I, ActionSelection.InvertSelection)
        .registerKeyShortcut(.CtrlShift, .T, ActionFlatten.FlattenAll)
        .registerKeyShortcut(.Alt, .F4, ActionGlobal.Exit)
        .registerKeyShortcut(.Control, .N, ActionFile.NewImage)
        .registerKeyShortcut(.CtrlShift, .N, ActionFile.NewFromClipboard)
        .registerKeyShortcut(.CtrlShift, .S, ActionFile.SaveAsImage)
        .registerKeyShortcut(.Control, .S, ActionFile.UpdateLinkedImage)
        .registerKeyShortcut(.Control, .O, ActionFile.OpenImage)
        .registerKeyShortcut(.Zero, .F2, ActionEdit.EditText)
        .registerKeyShortcut(.CtrlAlt, .O, ActionOptions.Options)
        .registerKeyShortcut(.Zero, .Home, ActionMoveSel.SendToFront)
        .registerKeyShortcut(.Zero, .End, ActionMoveSel.SendToBack)
        .registerKeyShortcut(.Zero, .PageUp, ActionMoveSel.MoveForward)
        .registerKeyShortcut(.Zero, .PageDown, ActionMoveSel.MoveBackward)
        .registerKeyShortcut(.Shift, .Left, ActionAlign.Left1)
        .registerKeyShortcut(.Shift, .Up, ActionAlign.Top1)
        .registerKeyShortcut(.Shift, .Right, ActionAlign.Right1)
        .registerKeyShortcut(.Shift, .Down, ActionAlign.Bottom1)
        .registerKeyShortcut(.Zero, .Left, ActionAlign.Left)
        .registerKeyShortcut(.Zero, .Up, ActionAlign.Top)
        .registerKeyShortcut(.Zero, .Right, ActionAlign.Right)
        .registerKeyShortcut(.Zero, .Down, ActionAlign.Bottom)
        .registerKeyShortcut(.Control, .Add, ActionZoom.ZoomIn)
        .registerKeyShortcut(.Control, .Subtract, ActionZoom.ZoomOut)
        .registerKeyShortcut(.Control, .NumPad0, ActionZoom.Zoom1)

        var errorReg = false

        #[Swag.Mixin]
        mtd registerHotKey(mdf: Input.KeyModifiers, key: Input.Key, wndId: WndId)
        {
            catch .getApp().registerHotKey(mdf, key, wndId, me)
            if @err != null
            {
                if !errorReg
                {
                    var msg = Format.toString("Cannot register global shortcut '%'. ", Input.getKeyName(mdf, key))
                    msg += "Another running application is probably already using it!"
                    discard EmbInfoCtrl.create(.editWnd, msg, .Critical)
                }
                errorReg = true
            }
        }

        .registerHotKey(.Zero, .PrintScreen, ActionCapture.Area)
        .registerHotKey(.CtrlShift, .PrintScreen, ActionCapture.LastArea)
        .registerHotKey(.Control, .PrintScreen, ActionCapture.FullScreen)
        .registerHotKey(.Alt, .PrintScreen, ActionCapture.ActiveWnd)
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

        // File menu
        with let fileMenu = MenuCtrl.createPopup(me.parent, owner: me)
        {
            .addGroup([ActionFile.NewImage, ActionFile.NewFromClipboard])
            .addGroup([ActionFile.OpenImage, ActionFile.SaveAsImage])
            .addGroup([ActionFile.SaveAsLinkedImage, ActionFile.UpdateLinkedImage, ActionFile.UnlinkImage])
            .addGroup([ActionFile.FileDetails])
            .addItems([ActionGlobal.Minimize, ActionGlobal.Exit])
            topMenu.addPopup("File", fileMenu)
        }

        // Capture menu
        with let captureMenu = MenuCtrl.createPopup(me.parent, owner: me)
        {
            .addItems([ActionCapture.Area, ActionCapture.LastArea, ActionCapture.FullScreen, ActionCapture.ActiveWnd])

            let screenActions = [ActionCapture.Screen0, ActionCapture.Screen1, ActionCapture.Screen2, ActionCapture.Screen3]
            foreach m, idx in me.monitors do
                .addItem(screenActions[idx])

            .addSeparator()
            .addItem(ActionCapture.HideEditor)

            with let delayMenu = MenuCtrl.createPopup(captureMenu, owner: me)
            {
                .addGroup([ActionCaptureDelay.Delay0])
                .addItems([ActionCaptureDelay.Delay1, ActionCaptureDelay.Delay2, ActionCaptureDelay.Delay5, ActionCaptureDelay.Delay9])
                captureMenu.addPopup(delayMenu, ActionCaptureDelay.Delay)
            }

            topMenu.addPopup("Capture", captureMenu)

            .addSeparator()
            .addItems([ActionCapture.ToInPlace, ActionCapture.ToEditor, ActionCapture.ToClipboard])
        }

        // Edit menu
        with let editMenu = MenuCtrl.createPopup(me.parent, owner: me)
        {
            .addGroup([ActionUndo.DoUndo, ActionUndo.DoRedo])
            .addGroup([ActionCopyPaste.Copy, ActionCopyPaste.Cut, ActionCopyPaste.Paste])
            .addGroup([ActionSelection.SelectAll, ActionSelection.SelectSameAll, ActionSelection.DeSelectAll, ActionSelection.InvertSelection])
            .addGroup([ActionSelection.Freeze])
            .addItems([ActionSelection.Snap])
            topMenu.addPopup("Edit", editMenu)
        }

        // Image menu
        with let imageMenu = MenuCtrl.createPopup(me.parent, owner: me)
        {
            .addGroup([ActionCopyPaste.CopyAsImage])
            .addGroup([ActionImage.RestoreOrg])
            .addGroup([ActionImage.ResizeImage, ActionImage.ExpandImage, ActionImage.ExpandCapture, ActionImage.TrimCapture])
            .addGroup([ActionFlatten.FlattenAll])
            .addItems([ActionImage.FlipImage, ActionImage.MirrorImage, ActionImage.RotateImageRight, ActionImage.RotateImageLeft])
            topMenu.addPopup("Image", imageMenu)
        }

        // View menu
        with let viewMenu = MenuCtrl.createPopup(me.parent, owner: me)
        {
            .addGroup([ActionZoom.ZoomIn, ActionZoom.ZoomOut])
            .addGroup([ActionZoom.Zoom1, ActionZoom.ZoomFit])
            .addItems([ActionView.Clip])
            topMenu.addPopup("View", viewMenu)
        }

        // Help menu
        with let helpMenu = MenuCtrl.createPopup(me.parent, owner: me)
        {
            .addItem(ActionOptions.Options)
            .addSeparator()
            .addItem(ActionAbout.About)
            topMenu.addPopup("Help", helpMenu)
        }
    }

    mtd createToolbar()
    {
        with me.editToolBar = StackLayoutCtrl.create(me.topBar, .HorzCenter, {0, 0, 64, 64})
        {
            .dockStyle       = .Top
            .backgroundStyle = .Window
            .style.addStyleSheetColors("wnd_Bk $wnd_CaptionBk")
        }

        let toolButtons = [
            ActionToolMode.Favorites, ActionToolMode.Selection, ActionToolMode.Shape,
            ActionToolMode.Arrow, ActionToolMode.Line, ActionToolMode.Poly,
            ActionToolMode.Text, ActionToolMode.Draw, ActionToolMode.Highlight,
            ActionToolMode.Step, ActionToolMode.Img, ActionToolMode.PixSel]

        foreach id in toolButtons
        {
            with let b = IconButton.create(me.editToolBar, {}, position: {0, 0, 60, 64})
            {
                .id = id
                .style.addStyleSheetColors("btnIcon_CheckedBk           $btnIcon_HotBk")
                .style.addStyleSheetColors("btnIcon_CheckedIcon         $btnIcon_CheckedMark")
                .style.addStyleSheetColors("btnIcon_CheckedText         $btnIcon_CheckedMark")
                .style.addStyleSheetColors("btnIcon_CheckedHotBk        $btnIcon_HotBk")
                .style.addStyleSheetColors("btnIcon_CheckedHotIcon      $btnIcon_CheckedHotMark")
                .style.addStyleSheetColors("btnIcon_CheckedHotText      $btnIcon_CheckedHotMark")
                .style.addStyleSheetColors("btnIcon_CheckedPressedIcon  $btnIcon_CheckedMark")
                .style.addStyleSheetColors("btnIcon_CheckedPressedText  $btnIcon_CheckedMark")
                .iconBtnFlags.add(.Checkable)
                .checkedForm = .Bottom
                .iconSizeX, .iconSizeY = 48
            }
        }

        .captureBtn           = IconButton.create(.topBar, Icon.from(&.icons48, 6), {0, 0, 64, 64}, id: ActionCapture.DoIt)
        .captureBtn.iconColor = Argb.White
        .libraryBtn           = IconButton.create(.topBar, Icon.from(&.icons48, 8), {64, 0, 64, 64}, id: ActionGlobal.ShowLibrary)
    }

    mtd setTheme()
    {
        if .editorOptions.theme == .Dark do
            .getApp().theme.colors.setDark()
        else do
            .getApp().theme.colors.setLight()
        .getApp().themeHasChanged()
    }

    mtd setTopBarHeight()
    {
        var icons: Array'(*IconButton)
        foreach c in .topBar.childs
        {
            if c.type == IconButton do
                icons.add(cast(*IconButton) c)
        }
        foreach c in .editToolBar.childs
        {
            if c.type == IconButton do
                icons.add(cast(*IconButton) c)
        }

        foreach c in icons
        {
            if .editorOptions.showToolBarLabels
            {
                c.iconBtnFlags.remove(.HideName)
                c.position.width, c.position.height = 64
                c.iconSizeX, c.iconSizeY = 48
                .libraryBtn.position.x = 64
            }
            else
            {
                c.iconBtnFlags.add(.HideName)
                c.position.width, c.position.height = 40
                c.iconSizeX, c.iconSizeY = 32
                .libraryBtn.position.x = 48
            }
        }

        .topBar.position.height = .editorOptions.showToolBarLabels ? 68 : 50
        .editWnd.sendResizeEvent()
        .topBar.sendResizeEvent()
        .editToolBar.sendResizeEvent()
        .applyLayout()
    }

    func create(parent: *Wnd)->*MainWnd
    {
        parent.surface.minSize = {1024, 512}

        let img24    = assume Image.decode(".png", Icons24)
        let img48    = assume Image.decode(".png", Icons48)
        let imgApp   = assume Image.decode(".png", AppIcon)
        let renderer = parent.getApp().getRenderer()

        with let res = Wnd.create'MainWnd(parent, "captme.mainwnd")
        {
            .surface.setView(res)
            .icons24.set(renderer.addImage(img24), 24, 24)
            .icons48.set(renderer.addImage(img48), 48, 48)
            .appIcon.set(renderer.addImage(imgApp))
            .backgroundStyle = .Window
            .mdlFavorite     = &res.mdlFormShape
            .mdlHighlight    = &res.mdlFormShape
            .mdlPixSel       = &res.mdlFormShape
            .monitors        = Env.getMonitors()
        }

        // Create top bar
        with res.topBar = Wnd.create'Wnd(res, {0, 0, 64, 68})
        {
            .dockStyle       = .Top
            .backgroundStyle = .Window
            .style.addStyleSheetColors("wnd_Bk $wnd_CaptionBk")
        }

        res.createToolbar()

        // Main edit area
        with res.editWnd = EditWnd.create(res, res)
        {
            .dockStyle = .Center
            .sigScrollSizeChanged += func|res|(scroll, old, new) { res.updateZoomDisplay(); }
        }

        // Recent bar
        with res.recentBar = Wnd.create'Wnd(res, {0, 0, 0, 128})
        {
            .dockStyle = .Bottom
        }

        with res.recentSizeBar = IconBar.create(res.recentBar, 20)
        {
            .setIconBarFlags(.Vertical)
            .dockStyle   = .Left
            .iconPadding = 0
            .addItem({}, ActionRecentSize.RecentSmall, .Checkable)
            .addItem({}, ActionRecentSize.RecentMedium, .Checkable)
            .addItem({}, ActionRecentSize.RecentBig, .Checkable)
        }

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

        res.quickWnd = QuickWnd.create(res.quickStyleBar, res)
        res.propWnd  = PropWnd.create(res.quickStyleBar, res)

        with let quickPane = res.quickStyleBar.addPane(res.quickWnd, title: true)
        {
            .tryPreserveSize = true
            .label.setText("Quick Styles")
        }

        with let quickStyle = res.quickStyleBar.addPane(res.propWnd, title: true)
        {
            .label.setText("Tool Properties")
        }

        res.quickStyleBar.setPaneSize(0, 300)

        // Bottom bar
        with res.editZoomBar = StackLayoutCtrl.create(res, .HorzCenter, {0, 0, 300, 32})
        {
            .dockStyle       = .Bottom
            .backgroundStyle = .Window
        }

        let ic = res.getTheme().getIcon24(.Search, 20)
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
                menu.addItem(ActionZoom.Zoom1)
                menu.addItem(ActionZoom.ZoomFit)
                discard menu.doModal({pos.horzCenter(), pos.y}, .AnchorBottomCenter, autoDestroy: true)
            }
        }

        // Initialize everything
        with res
        {
            .registerActions()
            .registerShortcuts()
            .updateZoomDisplay()
            .updateSizeDisplay()
            .setBottomSize(.Medium)
            .setTopBarHeight()
            .updateState()
            .createTopMenu()
            .libWnd = LibraryWnd.create(res)
            .libWnd.hide()
        }

        g_Main = res
        return res
    }
}

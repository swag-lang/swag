using Core, Gui

var g_Prg: *ProgressBar

func onEvent(wnd: *Wnd, evt: IEvent)->bool
{
    if let pevt = evt as MouseEvent where pevt.kind == .MouseAccept
    {
        pevt.accepted = true
        return true
    }

    if let pevt = evt as CommandEvent where pevt.id == "exitBtn"
    {
        wnd.surface.destroy()
    }

    if evt is FrameEvent
    {
        g_Prg.setProgression(g_Prg.progression + wnd.getTiming().dt)
    }

    if let pevt = evt as MouseEvent where pevt.button == .Right
    {
        let menu = MenuCtrl.createPopup(wnd)
        menu.addItem("Coucou", "0", rightName: "Ctrl+A")
        menu.addItem("Salut", "1", app.theme.getIcon24(ThemeIcons24.Search, 18), disabled: true)
        menu.addItem("La", "2", app.theme.getIcon24(ThemeIcons24.WndClose, 18))
        menu.addSeparator()

        var popup = MenuCtrl.createPopup(wnd)
        popup.addItem("Coucou0", "0")
        popup.addItem("Coucou1", "1")
        menu.addPopup("Popup1", popup)

        let popup1 = MenuCtrl.createPopup(wnd)
        popup1.addItem("Coucou0", "0")
        popup1.addItem("Coucou1", "1")

        popup = MenuCtrl.createPopup(wnd)
        popup.addItem("Coucou0", "0")
        popup.addPopup("Coucou1", popup1)
        menu.addPopup("Popup2", popup)

        menu.addSeparator()
        menu.addItem("Foule", "3")
        discard menu.doModal(pevt.surfacePos)
    }

    return false
}

func createView0(tabView: *Tab, view: *Wnd)
{
    let theme = app.theme
    view.hookOnEvent = &onEvent
    view.getApp().registerFrameEvent(view)

    // Push buttons
    {
        var button = PushButton.create(view, "Normal", {5, 10})
        button.icon = app.theme.getIcon24(ThemeIcons24.WndClose)
        button.sigPressed += func(btn)
        {
            btn.name = "Pressed"
        }

        button = PushButton.create(view, "Default", {5, 50})
        button.sigPressed += func(btn)
        {
            btn.icon = btn.getTheme().getIcon24(ThemeIcons24.WndClose, 18)
        }
        button.form = .Default

        button = PushButton.create(view, "Disabled", {5, 90})
        button.disable()

        button        = PushButton.create(view, "ExitExitExitExitExit", {5, 130}, "exitBtn")
        button.cursor = Cursor.from(CursorShape.Help)
    }

    {
        var button = PushButton.create(view, "YesNo", {5, 190})
        button.form = .Strong
        button.sigPressed += func(btn)
        {
            discard MessageDlg.yesNo(btn.surface, "<u>Are</u> you very very <b>sure</b> ?")
        }

        button      = PushButton.create(view, "YesNo", {5, 230})
        button.form = .Strong
        button.sigPressed += func(btn)
        {
            var opt: MessageDlgOptions
            opt.horzAlignmentText = .Justify
            discard MessageDlg.yesNo(btn.surface, "Lorem ipsum <p1>dolor sit amet</p>, consectetur <b>adipiscing</b> elit, sed: eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.", options: opt)
        }

        button      = PushButton.create(view, "OK", {5, 270})
        button.form = .Flat
        button.style.addStyleSheetColors("btnPushFlat_HotBk 0xFF00AA00")
        button.style.addStyleSheetColors("btnPushFlat_PressedBk 0xFF00BB00")
        button.sigPressed += func(btn)
        {
            MessageDlg.ok(btn.surface, "Lorem ipsum <p1>dolor sit amet</p>, consectetur <b>adipiscing</b> elit, sed: eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.")
        }

        button      = PushButton.create(view, "YesNoCancel", {5, 310})
        button.form = .Flat
        button.sigPressed += func(btn)
        {
            discard MessageDlg.yesNoCancel(btn.surface, "Avant de désinstaller cette application, veuillez nous indiquer si vous souhaitez conserver ou supprimer les notifications, les alertes, les paramètres d'interface, les modules et d'autres préférences. I_a suppression des préférences peut empêcher leur migration vers de nouvelles versions de l'application.")
        }
    }

    // Check buttons
    {
        var button = CheckButton.create(view, "Normal", {125, 10})

        button = CheckButton.create(view, "Disabled", {125, 40})
        button.disable()

        button = CheckButton.create(view, "Disabled", {125, 70})
        button.disable()
        button.checked = .Checked

        button = CheckButton.create(view, "3 State", {125, 100})
        button.checkButtonFlags |= .ThreeState
    }

    // Radio buttons
    {
        var button = RadioButton.create(view, "Normal", {240, 10})

        button = RadioButton.create(view, "Disabled", {240, 40})
        button.disable()

        button = RadioButton.create(view, "Disabled", {240, 70})
        button.disable()
        button.checked = true
    }

    // EditBox
    {
        var edit = EditBox.create(view, "TOTO LA foule this is a very long string", {360, 10, 100})
        edit.setNote("<i>Find<i>")

        edit = EditBox.create(view, "TOTO LA foule this is a very long string", {360, 50, 100})
        edit.setForm(EditBoxForm.Round)
        edit.icon = theme.getIcon24(ThemeIcons24.Search, 18)
        edit.setNote("<i>Find<i>")

        edit = EditBox.create(view, "", {360, 90, 200})
        edit.setForm(EditBoxForm.Flat)
        edit.icon = theme.getIcon24(ThemeIcons24.Search, 18)
        edit.setNote("<i>Whatever<i>")

        edit = EditBox.create(view, "", {360, 140, 100})
        edit.setForm(EditBoxForm.Square)
        edit.setNote("MaxLen=3")
        edit.maxLength = 3

        edit = EditBox.create(view, "toto toto toto", {470, 140, 100})
        let btn = IconButton.create(edit, app.theme.getIcon24(ThemeIcons24.WndClose, 18), {0, 0, 24, 24})
        btn.dockStyle = .Right
        btn.setForm(IconButtonForm.Round)
        btn.sigPressed += func(btn)
        {
            var edt = cast(*EditBox) btn.parent
            edt.setText(null)
        }
        edit.setForm(EditBoxForm.Round)
        edit.setNote("")
        edit.setPadding(right: 4.0)
        edit.rightMargin = 24

        edit = EditBox.create(view, "", {360, 180, 100})
        edit.setForm(EditBoxForm.Square)
        edit.setNote("-127/128")
        edit.setInputS64(-127, 128)

        edit = EditBox.create(view, "disabled", {470, 180, 100})
        edit.disable()
        edit.setForm(EditBoxForm.Square)

        edit = EditBox.create(view, "", {360, 220, 100})
        edit.setForm(EditBoxForm.Square)
        edit.setNote("Float")
        edit.setInputF64()

        edit = EditBox.create(view, "ReadOnly", {470, 220, 100})
        edit.editBoxFlags |= .ReadOnly
        edit.setForm(EditBoxForm.Square)
        edit.setNote("ReadOnly")
    }

    // Toggle
    {
        var toggle = ToggleButton.create(view, {360, 280})

        toggle           = ToggleButton.create(view, {360, 310})
        toggle.isToggled = true

        toggle = ToggleButton.create(view, {360, 340})
        toggle.disable()
    }

    // Slider
    {
        var slider = Slider.create(view, {10, 350})
        slider.setMinMax(-127, 128)
        slider.setValue(0)

        slider = Slider.create(view, {10, 380}, range: true)
        slider.setMinMax(-2.5, 2.5)
        slider.setRangeValues(-2.5, 2.5)

        slider = Slider.create(view, {10, 410})
        slider.setMinMax(0, 1, 0.1)
        slider.sliderFlags.add(.ShowSteps)
        slider.setValue(0.55)

        slider = Slider.create(view, {10, 440})
        slider.disable()

        slider = Slider.create(view, {340, 600}, vertical: true)
        slider.setMinMax(0, 1, 0.1)
        slider.setValue(0.55)

        let slider0 = Slider.create(view, {10, 470}, id: "slider0")
        slider0.sliderFlags &= ~.HilightBar
        let edit0 = EditBox.create(view, "0", {210, 464, 45}, id: "edit0")
        edit0.setForm(EditBoxForm.Transparent)

        slider0.setMinMax(0, 255, 5)
        slider0.sigChanged += func(wnd)
        {
            let val  = Format.toString("%", cast(s32) wnd.getValue())
            let edit = cast(*EditBox) assume wnd.getTopView().getChildById("edit0")
            edit.setText(val)
        }

        edit0.setInputU64(0, 255)
        edit0.maxLength = 3
        edit0.sigLoseFocus += func(wnd)
        {
            let (val, ?, ?) = catch StrConv.parseS32(wnd.text)
            let slider = cast(*Slider) assume wnd.getTopView().getChildById("slider0")
            slider.setValue(cast(f32) val)
        }

        slider0.setValue(100)
    }

    // Circular
    {
        var circular = Circular.create(view, {1050, 50})
        circular.markerForm = .Needle
        circular.stepForm   = .Dot
        circular.setAngleRange(0'deg, 90'deg)
        circular.setStepCount(5)

        circular = Circular.create(view, {1050, 100})
        circular.setMinMax(0, 360)
        circular.setValue(90)

        circular = Circular.create(view, {1050, 200, 80})
        circular.circularFlags.add(.Range)
        circular.circleGap = 4

        circular = Circular.create(view, {1050, 300, 80})
        circular.circularFlags.add(.Integer)
        circular.setMinMax(0, 360, 10)
        circular.sigChanged += func(val) { @print(val.getValue(), " "); }
    }

    // SliderCtrl
    {
        let sliderctrl = SliderCtrl.create(view, "Center", {370, 590, 250, 50})
        sliderctrl.slider.setMinMax(0, 255)
        sliderctrl.edit.setInputU64(0, 255)
        sliderctrl.edit.setForm(.Square)
        sliderctrl.slider.setValue(0)
    }

    // Labels
    {
        var label = Label.create(view, "Left", {125, 140, 140, 30})
        label.icon = theme.getIcon24(ThemeIcons24.WndClose)
        label.style.addStyleSheetColors("label_Icon 0xFFFF0000")
        label.backgroundStyle   = .Dialog
        label.horzAlignmentText = .Left
        label.horzAlignmentIcon = .Left
        label.paddingText       = {24, -3}

        label                   = Label.create(view, "Align Right", {125, 180, 140, 30})
        label.icon              = theme.getIcon24(ThemeIcons24.WndClose)
        label.backgroundStyle   = .Dialog
        label.horzAlignmentText = .Right
        label.vertAlignmentText = .Center
        label.horzAlignmentIcon = .Right
        label.paddingText       = {0, 0, 24, 3}

        label                   = Label.create(view, "Center", {125, 220, 140, 30})
        label.icon              = theme.getIcon24(ThemeIcons24.WndClose)
        label.backgroundStyle   = .Dialog
        label.horzAlignmentText = .Center
        label.horzAlignmentIcon = .Left

        label                   = Label.create(view, "TopLeft", {125, 260, 140, 30})
        label.backgroundStyle   = .Dialog
        label.horzAlignmentText = .Left
        label.vertAlignmentText = .Top

        label                   = Label.create(view, "BottomRight", {125, 300, 140, 30})
        label.backgroundStyle   = .Dialog
        label.horzAlignmentText = .Right
        label.vertAlignmentText = .Bottom
    }

    // Tab
    {
        let frame = Wnd.create'FrameWnd(view, {600, 20, 400, 100})
        frame.style.addStyleSheetColors("frameWnd_Border Red")
        let tab = Tab.create(frame, {})
        tab.barForm  = .FlatRound
        tab.viewForm = .Round
        frame.view   = tab
        frame.setFrameForm(FrameWndForm.Round)
        frame.padding.setZero()
        tab.dockStyle = .Center

        let view0 = Wnd.create'Wnd(tab)
        let view1 = Wnd.create'Wnd(tab)
        Label.create(view0, "view0", {10, 10, 50, 30})
        Label.create(view1, "view1", {10, 10, 50, 30})

        tab.addItem("Tab0", app.theme.getIcon24(ThemeIcons24.Search, 16), view0)
        tab.addItem("Tab1", {}, view1)

        frame.sendResizeEvent()
    }

    {
        let tab = Tab.create(view, {600, 150, 400, 100}, id: "tab0")
        tab.setForm(TabForm.Round)
        tab.barLayout = .Fixed

        for 5
        {
            let view0 = Wnd.create'Wnd(tab)
            let n     = Format.toString("Tab%", @index == 0 ? 1568 : @index)
            tab.addItem(n, app.theme.getIcon24(ThemeIcons24.Search, 16), view0)
            Label.create(view0, n, {10, 10, 50, 30})
        }
    }

    {
        let tab = Tab.create(view, {600, 560, 400, 100})
        tab.setForm(.Button)
        tab.style.setFont(view.getTheme().createDefaultFont(40), .Regular)
        tab.addItem("toto", app.theme.getIcon24(ThemeIcons24.Search, 16))
        tab.addItem("tata", app.theme.getIcon24(ThemeIcons24.Search, 16))
        tab.addItem("titilafoule", app.theme.getIcon24(ThemeIcons24.Search, 16))
    }

    {
        let tab = Tab.create(view, {600, 800, 400, 100})
        tab.setForm(.PushButton)
        tab.addItem("toto", app.theme.getIcon24(ThemeIcons24.Search, 16))
        tab.addItem("tata", app.theme.getIcon24(ThemeIcons24.Search, 16))
        tab.addItem("titilafoule", app.theme.getIcon24(ThemeIcons24.Search, 16))
    }

    // MenuCtrl
    {
        let menu = MenuCtrl.createBar(view, {600, 280, 400})
        menu.addItem("Coucou", "0")
        menu.addItem("Salut", "0")
        menu.addItem("La", "0")

        let popup1 = MenuCtrl.createPopup(view)
        popup1.addItem("Coucou0", "0")
        popup1.addItem("Coucou1", "1")

        let popup = MenuCtrl.createPopup(view)
        popup.addItem("Coucou0", "0")
        popup.addItem("Coucou1", "1")
        popup.addPopup("Popup1", popup1)

        menu.addPopup("Popup1", popup)
        menu.addPopup("Popup2", popup)
    }

    // Scroll
    {
        let swnd = Wnd.create'ScrollWnd(view, {600, 350, 400, 200}, id: "scroll")
        swnd.createView'Wnd()
        swnd.view.backgroundStyle = .Dialog
        discard PushButton.create(swnd.view, "Normal", {5, 15})
        discard PushButton.create(swnd.view, "Other", {5, 55})
        let edit = EditBox.create(swnd.view, "NoScroll", {5, 100})
        edit.setNote("NoScroll")
        edit.wndFlags |= .NoScroll
        edit.dockStyle = .Bottom
        swnd.setScrollSize(600, 600)
    }

    // Combo
    {
        var combo = ComboBox.create(view, {360, 400})
        combo.maxHeightPopup = 150
        combo.addItem("Coucou", "0")
        combo.addItem("Salut", "1", app.theme.getIcon24(ThemeIcons24.Search, 18))
        combo.addItem("La Foule", "2")
        combo.addItem("Coucou", "0")
        combo.addItem("Salut", "1", app.theme.getIcon24(ThemeIcons24.Search, 18))
        combo.addItem("La Foule", "2")

        combo = ComboBox.create(view, {360, 440})
        combo.addItem("<i>Coucou", "0")
        combo.addItem("<b>La", "1")
        combo.addItem("<p1>La Foule", "2")

        combo = ComboBox.create(view, {360, 480})
        combo.addItem("Coucou", "0")
        combo.selectItem(0)
        combo.disable()

        combo = ComboBox.create(view, {360, 520}, kind: .Edit)
        combo.addItem("Coucou", "0")
        combo.addItem("Coucou1", "1")
        combo.addItem("Coucou2", "2")
        combo.selectItem(0)
    }

    // Progress Bar
    {
        let prg = ProgressBar.create(view, {20, 520, 200})
        prg.prgBarFlags |= .Infinite
    }

    {
        let prg = ProgressBar.create(view, {20, 550, 200})
        g_Prg = prg
        tabView.sigSelChanged += func|prg|(tab)
        {
            prg.setProgression(0)
        }
    }

    {
        let prg = ProgressBar.create(view, {20, 580, 200})
        prg.setProgression(0.5)
        prg.disable()
    }

    let frm = Wnd.create'FrameWnd(view, {10, 610, 300, 170})
    frm.setFrameForm(.Round)

    // ImageRect
    {
        let img = ImageRect.create(view, {20, 620, 48, 48})
        img.setMovie(theme.res.spin)
        img.style.addStyleSheetColors("imageRect_Fg $hilight")

        let img1 = ImageRect.create(view, {20, 680, 48, 48})
        img1.setMovie(theme.res.spin)

        let img2  = ImageRect.create(view, {400, 640, 128, 128})
        let path2 = Path.combine(Path.getDirectoryName(#file), "/datas/test.gif")
        assume img2.setFile(path2)

        let btn    = IconButton.create(view, app.theme.getIcon24(ThemeIcons24.Pause), {400, 780, 24, 24})
        let slider = Slider.create(view, {425, 780, 120})
        slider.disable()

        btn.sigPressed += func|img2, slider|(btn)
        {
            img2.movie.pause(!img2.movie.inPause)
            slider.enable(img2.movie.inPause)
            btn.icon = img2.movie.inPause ? btn.getTheme().getIcon24(ThemeIcons24.Play) : btn.getTheme().getIcon24(ThemeIcons24.Pause)
        }
        img2.movie.sigFrameChanged += func|slider|(movie)
        {
            slider.setMinMax(0, cast() movie.numFrames)
            slider.setValue(cast() movie.frameIndex)
        }
        slider.sigChanged += func|img2|(slider)
        {
            if !img2.movie.inPause do
                return
            img2.movie.sigFrameChanged.disable()
            img2.movie.setFrameIndex(cast(s32) slider.getValue())
            img2.movie.sigFrameChanged.enable()
            img2.invalidate()
        }

        let img3  = ImageRect.create(view, {540, 640, 128, 128})
        let path3 = Path.combine(Path.getDirectoryName(#file), "/datas/testa.gif")
        assume img3.setFile(path3)

        let img4 = ImageRect.create(view, {680, 640, 128, 128})
        img4.setImage(theme.res.icons64)
    }

    // IconButton
    {
        let btn0 = IconButton.create(view, theme.getIcon24(ThemeIcons24.Search), {80, 620, 32, 32})
        btn0.setForm(.RoundSquare)
        let btn1 = IconButton.create(view, theme.getIcon24(ThemeIcons24.Picker), {112, 620, 32, 32})
        btn1.setForm(.RoundSquare)
        let btn2 = IconButton.create(view, theme.getIcon24(ThemeIcons24.Picker), {144, 620, 32, 32})
        btn2.disable()
        btn2.setForm(.RoundSquare)
    }

    {
        func setStyle(btn: *IconButton)
        {
            btn.setForm(.RoundSquare)
            btn.style.addStyleSheetColors("btnIcon_Icon             $disabled")
            btn.style.addStyleSheetColors("btnIcon_HotBk            $transparent")
            btn.style.addStyleSheetColors("btnIcon_PressedBk        $transparent")
            btn.style.addStyleSheetColors("btnIcon_CheckedPressedBk $transparent")
            btn.iconBtnFlags.set(.CheckableToggle)
        }

        let btn0 = IconButton.create(view, theme.getIcon24(ThemeIcons24.Search), {80, 660, 48, 48})
        setStyle(btn0)
        btn0.checkedForm = .Bottom

        let btn1 = IconButton.create(view, theme.getIcon24(ThemeIcons24.Picker), {130, 660, 48, 48})
        setStyle(btn1)
        btn1.checkedForm = .Top

        let btn2 = IconButton.create(view, theme.getIcon24(ThemeIcons24.Search), {180, 660, 48, 48})
        setStyle(btn2)
        btn2.checkedForm = .None
        btn2.style.addStyleSheetColors("btnIcon_CheckedIcon      $hilight")
        btn2.style.addStyleSheetColors("btnIcon_CheckedHotIcon   $hilight")

        let btn3 = IconButton.create(view, theme.getIcon24(ThemeIcons24.Picker), {230, 660, 48, 48})
        setStyle(btn3)
        btn3.checkedForm = .None
        btn3.style.addStyleSheetColors("btnIcon_CheckedIcon      $hilight")
        btn3.style.addStyleSheetColors("btnIcon_CheckedHotIcon   $hilight")
    }

    {
        let btn0 = IconButton.create(view, theme.getIcon24(ThemeIcons24.Search), {80, 720, 48, 48})
        btn0.setForm(.RoundSquare)
        btn0.checkedForm = .Full
        btn0.iconBtnFlags.set(.CheckableToggle)
    }

    {
        let ib = IconBar.create(view, 24, {880, 630, 124, 48})
        ib.addItem(theme.getIcon24(ThemeIcons24.Play), "0")
        ib.addItem(theme.getIcon24(ThemeIcons24.Pause), "1")
        ib.addItem(theme.getIcon24(ThemeIcons24.Stop), "2")
    }

    {
        let ib = IconBar.create(view, 18, {880, 680, 124, 48})
        ib.setIconBarFlags(.Vertical)
        ib.addItem(theme.getIcon24(ThemeIcons24.Play), "0")
        ib.addItem(theme.getIcon24(ThemeIcons24.Pause), "1")
        ib.addItem(theme.getIcon24(ThemeIcons24.Stop), "2")
    }
}

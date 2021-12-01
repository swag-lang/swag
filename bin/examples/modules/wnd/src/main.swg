#global if #os == Swag.TargetOs.Windows
using Core, Pixel, Gui

const W = 1024
const H = 1024

#main
{
    var app: Gui.Application

    surface0 := try app.createSurface(100, 100, W, H, SurfaceFlags.OverlappedWindow)
    surface0.setTitle("Example 0")
    surface0.setIcon(app.theme.getIcon24(ThemeIcons24.WndMinimize, 20))
    view := surface0.wnd.view
    theme := app.theme

    // Push buttons
    {
        button := PushButton.create(view, "Normal", @{5, 10})
        button.icon = app.theme.getIcon24(ThemeIcons24.WndClose)
        button.sigPressed = @(btn: *Button) { btn.name = "Pressed"; }

        button = PushButton.create(view, "Default", @{5, 50})
        button.sigPressed = @(btn: *Button) { btn.icon = btn.getTheme().getIcon24(ThemeIcons24.WndClose, 18); }
        button.form = .Default

        button = PushButton.create(view, "Disabled", @{5, 90})
        button.disable()

        button = PushButton.create(view, "ExitExitExitExitExit", @{5, 130})
        button.cursor = Cursor.from(CursorShape.Help)
    }

    {
        button := PushButton.create(view, "YesNo", @{5, 190})
        button.sigPressed = @(btn: *Button) { discard MessageDlg.yesNo(btn.surface, "<u>Are<u> you very very <b>sure<b> ?"); }

        button = PushButton.create(view, "YesNo", @{5, 230})
        button.sigPressed = @(btn: *Button) { discard MessageDlg.yesNo(btn.surface, "Lorem ipsum <p1>dolor sit amet<p1>, consectetur <b>adipiscing<b> elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."); }

        button = PushButton.create(view, "OK", @{5, 270})
        button.form = .Flat
        button.style.addStyleSheetColors("btnPushFlat_HotBk 0xFF00AA00")
        button.style.addStyleSheetColors("btnPushFlat_PressedBk 0xFF00BB00")
        button.sigPressed = @(btn: *Button) { MessageDlg.ok(btn.surface, "Lorem ipsum <p1>dolor sit amet<p1>, consectetur <b>adipiscing<b> elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur."); }

        button = PushButton.create(view, "YesNoCancel", @{5, 310})
        button.form = .Flat
        button.sigPressed = @(btn: *Button) { discard MessageDlg.yesNoCancel(btn.surface, "Avant de désinstaller cette application, veuillez nous indiquer si vous souhaitez conserver ou supprimer les notifications, les alertes, les paramètres d'interface, les modules et d'autres préférences. I_a suppression des préférences peut empêcher leur migration vers de nouvelles versions de l'application."); }
    }

    // Check buttons
    {
        button := CheckButton.create(view, "Normal", @{125, 10})

        button = CheckButton.create(view, "Disabled", @{125, 40})
        button.disable()

        button = CheckButton.create(view, "Disabled", @{125, 70})
        button.disable()
        button.isChecked = .Checked

        button = CheckButton.create(view, "3 State", @{125, 100})
        button.isThreeState = true
    }

    // Radio buttons
    {
        button := RadioButton.create(view, "Normal", @{240, 10})

        button = RadioButton.create(view, "Disabled", @{240, 40})
        button.disable()

        button = RadioButton.create(view, "Disabled", @{240, 70})
        button.disable()
        button.isChecked = true
    }

    // EditBox
    {
        edit := EditBox.create(view, "TOTO LA foule this is a very long string", @{360, 10, 100})
        edit.setNote("<i>Find<i>")

        edit = EditBox.create(view, "TOTO LA foule this is a very long string", @{360, 50, 100})
        edit.setForm(EditBoxForm.Round)
        edit.icon = theme.getIcon24(ThemeIcons24.Search, 18)
        edit.setNote("<i>Find<i>")

        edit = EditBox.create(view, "", @{360, 90, 200})
        edit.setForm(EditBoxForm.Flat)
        edit.icon = theme.getIcon24(ThemeIcons24.Search, 18)
        edit.setNote("<i>Whatever<i>")

        edit = EditBox.create(view, "", @{360, 140, 100})
        edit.setForm(EditBoxForm.Square)
        edit.setNote("MaxLen=3")
        edit.maxLength = 3

        edit = EditBox.create(view, "", @{360, 180, 100})
        edit.setForm(EditBoxForm.Square)
        edit.setNote("-127/128")
        edit.setInputS64(-127, 128)

        edit = EditBox.create(view, "", @{360, 220, 100})
        edit.setForm(EditBoxForm.Square)
        edit.setNote("Float")
        edit.setInputF64()
    }

    // Toggle
    {
        toggle := ToggleButton.create(view, @{360, 300})

        toggle = ToggleButton.create(view, @{360, 330})
        toggle.isToggled = true

        toggle = ToggleButton.create(view, @{360, 360})
        toggle.isDisabled = true
    }

    // Slider
    {
        slider := Slider.create(view, @{450, 300})
        slider.setMinMax(-127, 128)
        slider.setValue(0)

        slider = Slider.create(view, @{450, 330}, range: true)
        slider.setRangeValues(0.25, 0.75)

        slider = Slider.create(view, @{450, 360})
        slider.setMinMax(0, 1, 0.1)
        slider.setValue(0.55)

        slider = Slider.create(view, @{450, 390})
        slider.isDisabled = true


        slider0 := Slider.create(view, @{450, 420}, id: "slider0")
        slider0.hilightBar = false
        edit0   := EditBox.create(view, "0", @{645, 414, 45}, id: "edit0")
        edit0.setForm(EditBoxForm.Transparent)

        slider0.setMinMax(0, 255)
        slider0.sigChanged = @(wnd: *Slider)
        {
            val  := Format.toString("%", cast(s32) wnd.getValue())
            edit := cast(*EditBox) wnd.getTopView().getChildById("edit0")
            edit.setText(val)
        }

        edit0.setInputU64(0, 255)
        edit0.sigLoseFocus = @(wnd: *EditBox)
        {
            (val, ?, ?) := catch StrConv.parseS32(wnd.text)
            slider := cast(*Slider) wnd.getTopView().getChildById("slider0")
            slider.setValue(cast(f32) val)
        }

        slider0.setValue(100)
    }

    // Labels
    {
        label := Label.create(view, "Left", @{125, 140, 140, 30})
        label.icon = theme.getIcon24(ThemeIcons24.WndClose)
        label.style.addStyleSheetColors("label_Icon 0xFFFF0000")
        label.backgroundStyle = .Dialog
        label.paddingText = @{24, -3}

        label = Label.create(view, "Align Right", @{125, 180, 140, 30})
        label.icon = theme.getIcon24(ThemeIcons24.WndClose)
        label.backgroundStyle = .Dialog
        label.horzAlignmentText = .Right
        label.horzAlignmentIcon = .Right
        label.paddingText = @{0, 0, 24, 3}

        label = Label.create(view, "Center", @{125, 220, 140, 30})
        label.icon = theme.getIcon24(ThemeIcons24.WndClose)
        label.backgroundStyle = .Dialog
        label.horzAlignmentText = .Center

        label = Label.create(view, "TopLeft", @{125, 260, 140, 30})
        label.backgroundStyle = .Dialog
        label.horzAlignmentText = .Left
        label.vertAlignmentText = .Top

        label = Label.create(view, "BottomRight", @{125, 300, 140, 30})
        label.backgroundStyle = .Dialog
        label.horzAlignmentText = .Right
        label.vertAlignmentText = .Bottom
    }

    // Tab
    {
        tab := Tab.create(view, @{600, 20, 400, 200}, id: "tab0")
        view0 := Wnd.create'Wnd()
        view0.backgroundStyle = .Dialog
        tab.addView("Tab0", view0, app.theme.getIcon24(ThemeIcons24.Search, 16))
        view1 := Wnd.create'Wnd()
        view1.backgroundStyle = .Dialog
        tab.addView("Tab1", view1)

        Label.create(view0, "view0", @{10, 10, 50, 30})
        Label.create(view1, "view1", @{10, 10, 50, 30})
    }

    //surface1 := try app.createSurface(100+W+10, 100, W, H, SurfaceFlags.Secondary | SurfaceFlags.SmallCaption | SurfaceFlags.CloseBtn)

    surface0.show()
    //surface1.show()

    app.run()
}
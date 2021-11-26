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
        button := ButtonPush.create(view, "Normal", @{5, 10})
        button.icon = app.theme.getIcon24(ThemeIcons24.WndClose)
        button.sigPressed = @(btn: *Button) { btn.name = "Pressed"; }
        button = ButtonPush.create(view, "Default", @{5, 50})
        button.sigPressed = @(btn: *Button) { btn.icon = btn.getTheme().getIcon24(ThemeIcons24.WndClose, 18); }
        button.isDefault = true
        button = ButtonPush.create(view, "Disabled", @{5, 90})
        button.disable()
        button = ButtonPush.create(view, "ExitExitExitExitExit", @{5, 130})
        button.cursor = Cursor.from(CursorShape.Help)
    }

    {
        button := ButtonPush.create(view, "YesNo", @{5, 190})
        button.sigPressed = @(btn: *Button) { discard MessageDlg.yesNo(btn.surface, "<u>Are<u> you very very <b>sure<b> ?"); }
        button = ButtonPush.create(view, "YesNo", @{5, 230})
        button.sigPressed = @(btn: *Button) { discard MessageDlg.yesNo(btn.surface, "Lorem ipsum <p1>dolor sit amet<p1>, consectetur <b>adipiscing<b> elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."); }
        button = ButtonPush.create(view, "OK", @{5, 270})
        button.sigPressed = @(btn: *Button) { MessageDlg.ok(btn.surface, "Lorem ipsum <p1>dolor sit amet<p1>, consectetur <b>adipiscing<b> elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur."); }
        button = ButtonPush.create(view, "YesNoCancel", @{5, 310})
        button.sigPressed = @(btn: *Button) { discard MessageDlg.yesNoCancel(btn.surface, "Avant de désinstaller cette application, veuillez nous indiquer si vous souhaitez conserver ou supprimer les notifications, les alertes, les paramètres d'interface, les modules et d'autres préférences. I_a suppression des préférences peut empêcher leur migration vers de nouvelles versions de l'application."); }
    }

    // Check buttons
    {
        button := ButtonCheck.create(view, "Normal", @{125, 10})
        button = ButtonCheck.create(view, "Disabled", @{125, 40})
        button.disable()
        button = ButtonCheck.create(view, "Disabled", @{125, 70})
        button.disable()
        button.isChecked = .Checked
        button = ButtonCheck.create(view, "3 State", @{125, 100})
        button.isThreeState = true
    }

    // Radio buttons
    {
        button := ButtonRadio.create(view, "Normal", @{240, 10})
        button = ButtonRadio.create(view, "Disabled", @{240, 40})
        button.disable()
        button = ButtonRadio.create(view, "Disabled", @{240, 70})
        button.disable()
        button.isChecked = true
    }

    // EditBox
    {
        edit := EditBox.create(view, "TOTO LA foule this is a very long string", @{360, 10, 100})
        edit.note = "<i>Find<i>"

        edit = EditBox.create(view, "TOTO LA foule this is a very long string", @{360, 50, 100})
        edit.setForm(EditBoxForm.Round)
        edit.icon = theme.getIcon24(ThemeIcons24.Search, 18)
        edit.note = "<i>Find<i>"

        edit = EditBox.create(view, "木木木木木", @{360, 90, 200})
        edit.setForm(EditBoxForm.Flat)
        edit.icon = theme.getIcon24(ThemeIcons24.Search, 18)
        edit.note = "<i>Find<i>"
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

    //surface1 := try app.createSurface(100+W+10, 100, W, H, SurfaceFlags.Secondary | SurfaceFlags.SmallCaption | SurfaceFlags.CloseBtn)

    surface0.show()
    //surface1.show()

    app.run()
}
#global if #os == Swag.TargetOs.Windows
using Core, Pixel, Gui

const W = 1024
const H = 1024

func onPaintEvent(wnd: *Wnd, evt: *PaintEvent)->bool
{
    evt.bc.painter.drawTexture(wnd.position, wnd.surface.app.theme.imgWidgets)
    return true
}

#main
{
    var app: Gui.Application
    surface0 := try app.createSurface(100, 100, W, H, SurfaceFlags.OverlappedWindow)
    surface0.setTitle("Example 0")
    surface0.setIcon(Icon.from(&app.theme.icons24, 0, 20))
    view := surface0.wnd.view

    button := ButtonPush.create(view, "Normal", @{5, 5, 100, 30})
    button.icon.set(&app.theme.icons24, 0, 18)
    button.sigPressed = @(btn: *ButtonPush) { btn.name = "Pressed"; }

    button = ButtonPush.create(view, "ExitExitExitExitExit", @{110, 5, 100, 30})
    button.cursor = Cursor.from(CursorShape.Help)
    button.sigPressed = @(btn: *ButtonPush) { MessageDlg.doModal(btn.surface, 200, 200); }

    button = ButtonPush.create(view, "Default", @{5, 40, 100, 30})
    button.sigPressed = @(btn: *ButtonPush) { btn.icon.set(&btn.surface.app.theme.icons24, 1, 18); }
    button.isDefault = true

    button = ButtonPush.create(view, "Disabled", @{5, 75, 100, 30})
    button.isDisabled = true

    surface1 := try app.createSurface(100+W+10, 100, W, H, SurfaceFlags.Secondary | SurfaceFlags.SmallCaption | SurfaceFlags.CloseBtn)
    surface1.wnd.view.hookOnPaintEvent = &onPaintEvent

    surface0.show()
    surface1.show()

    app.run()
}
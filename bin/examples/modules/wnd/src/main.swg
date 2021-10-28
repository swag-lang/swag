#global if #os == Swag.TargetOs.Windows
using Core, Pixel, Gui

const W = 1024
const H = 1024

func onPaintEvent(wnd: *Wnd, evt: *PaintEvent)
{
    evt.bc.painter.paintParams  = .Antialiased
    evt.bc.painter.paintQuality = .Normal
    evt.bc.painter.drawTexture(wnd.position, wnd.surface.app.theme.imgWidgets)
}

func onCloseEvent(wnd: *Wnd, event: *CloseEvent)
{
    wnd.postQuitEvent()
}

#main
{
    var app: Gui.Application
    surface0 := try app.createSurface(100, 100, W, H, SurfaceFlags.BigCaption)
    surface0.wnd.hookOnCloseEvent = &onCloseEvent

    view := surface0.wnd.view
    button := Button.create(view, "Normal", @{5, 5, 100, 30})
    button.icon.set(&app.theme.icons24, 0, 18)
    button.sigPressed = @(btn: *Button) { btn.name = "Pressed"; }
    button = Button.create(view, "ExitExitExitExitExit", @{110, 5, 100, 30})
    button.sigPressed = @(btn: *Button) { btn.postQuitEvent(); }
    button = Button.create(view, "Default", @{5, 40, 100, 30})
    button.sigPressed = @(btn: *Button) { btn.icon.set(&btn.surface.app.theme.icons24, 1, 18); }
    button.isDefault = true
    button = Button.create(view, "Disabled", @{5, 75, 100, 30})
    button.isDisabled = true
    surface0.show()

    surface1 := try app.createSurface(100+W+10, 100, W, H, SurfaceFlags.SecondaryWindow)
    surface1.wnd.view.hookOnPaintEvent = &onPaintEvent
    surface1.wnd.view.hookOnCloseEvent = &onCloseEvent
    surface1.show()

    app.run()
}
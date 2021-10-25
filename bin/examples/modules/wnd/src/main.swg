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
    surface0 := try app.createSurface("Example0", 100, 100, W, H)
    surface0.wnd.hookOnCloseEvent = &onCloseEvent

    button := Button.create(&surface0.wnd, "Normal", @{5, 5, 100, 30})
    button.icon.set(&app.theme.icons24, 0, 18)
    button.sigPressed = @(btn: *Button) { btn.name = "Pressed"; }
    button = Button.create(&surface0.wnd, "Long long long", @{110, 5, 100, 30})
    button = Button.create(&surface0.wnd, "Default", @{5, 40, 100, 30})
    button.sigPressed = @(btn: *Button) { btn.icon.set(&btn.surface.app.theme.icons24, 1, 18); }
    button.isDefault = true
    button = Button.create(&surface0.wnd, "Disabled", @{5, 75, 100, 30})
    button.isDisabled = true

    surface1 := try app.createSurface("Example1", 100+W, 100, W, H, SurfaceFlags.OverlappedWindow | SurfaceFlags.ToolWindow)
    surface1.wnd.hookOnPaintEvent = &onPaintEvent
    surface1.wnd.hookOnCloseEvent = &onCloseEvent

    surface0.show()
    surface1.show()

    app.run()
}
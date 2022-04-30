using Core, Pixel, Gui

var app: Application

#main
{
    surface := try app.createSurface(100, 100, 1200, 1000, SurfaceFlags.OverlappedWindow)
    surface.setTitle("CaptMe")
    surface.setView(EditWnd.create(surface.wnd))
    surface.show()

    app.run()
}
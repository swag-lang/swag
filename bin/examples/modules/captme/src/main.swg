using Core, Pixel, Gui

var g_App: Application

#main
{
    surface := try g_App.createSurface(100, 100, 1200, 1000, SurfaceFlags.OverlappedWindow)
    surface.setTitle("CaptMe")
    surface.setView(EditWnd.create(surface.wnd))
    surface.show()

    g_App.run()
}
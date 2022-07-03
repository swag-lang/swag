var g_App: Application

#main
{
    x := @typeof(Math.Vector2)
    visit m: @modules()
    {
        visit t: m.types
        {
            @print(m.name, ".", t.name, "\n")
        }
    }

    surface := try g_App.createSurface(200, 100, 1200, 1000, SurfaceFlags.OverlappedWindow)
    surface.setTitle("CaptMe")
    surface.setView(MainWnd.create(surface.wnd))
    surface.show()

    g_App.run()
}
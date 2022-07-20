var g_App: Application

#main
{
    Memory.DebugAllocator.install(traceLeaks: false)

    try Jobs.setNumWorkers()
    try Library.init()

    surface := try g_App.createSurface(200, 100, 1200, 1000, SurfaceFlags.OverlappedWindow)
    surface.setTitle("CaptMe")
    surface.setView(MainWnd.create(surface.wnd))
    surface.show()

    g_App.run()
}
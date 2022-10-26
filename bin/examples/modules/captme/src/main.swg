var g_App: Application

#main
{
    Memory.DebugAllocator.install(traceLeaks: false)
    try Jobs.setNumWorkers()
    try Library.init()

    path := Library.getFolder()
    path = Path.combine(path, "captme.cfg")
    g_App.setConfigPath(path)

    img := assume Image.decode(".png", Icons48)
    img.crop(48, 48, 48, 48)
    img.setPixelFormat(.BGRA8)
    g_App.setAppIcon(img)

    surface := try g_App.createSurface(200, 100, 1200, 1000, SurfaceFlags.OverlappedWindow)
    mainWnd := MainWnd.create(surface.wnd)
    surface.setView(mainWnd)
    mainWnd.createTopMenu()
    surface.show()

    if Env.hasArg("swag.test")
        g_App.maxRunFrame = 100
    g_App.run()
}
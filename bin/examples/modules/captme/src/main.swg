var g_App: Application

#main
{
    try Jobs.setNumWorkers()
    try Library.init()

    var path = Library.getFolder()
    path = Path.combine(path, "captme.cfg")
    g_App.setConfigPath(path)

    var img = assume Image.decode(".png", Icons48)
    img.crop(48, 48, 48, 48)
    img.setPixelFormat(.BGRA8)
    g_App.setAppIcon(img)

    var surface = try g_App.createSurface(200, 100, 1200, 1000, SurfaceFlags.OverlappedWindow)
    var mainWnd = MainWnd.create(surface.wnd)
    surface.setView(mainWnd)
    mainWnd.createTopMenu()
    if Env.hasArg("swag.test")
        g_App.maxRunFrame = 100

    catch g_App.loadState()
    surface.constaintToScreen()
    surface.show()

    using Win32
    with var tnid: NOTIFYICONDATAA
    {
        .cbSize = @sizeof(NOTIFYICONDATAA)
        .hWnd = surface.hWnd
        .uID = 1
        .uFlags = NIF_ICON | NIF_MESSAGE
        .uCallbackMessage = WM_USER
        .hIcon = catch img.toHICON()
    }

    catch Shell_NotifyIconA(NIM_ADD, &tnid)

    g_App.run()
    catch g_App.saveState()

    catch Shell_NotifyIconA(NIM_DELETE, &tnid)
}
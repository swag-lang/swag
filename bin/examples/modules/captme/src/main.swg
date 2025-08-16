var g_App: Application

#main
{
    let args = Env.getArgs()
    foreach arg in args
    {
        if !Path.isValidPathName(arg):
            continue
        if !Image.canLoad(arg):
            continue
        Clipboard.addString(arg)
        if Surface.sendCustomMessage("Captme", 1, 0):
            return
    }

    try Jobs.setNumWorkers()
    try Library.init()

    var path = Library.getFolder()
    path = Path.combine(path, "captme.cfg")
    g_App.setConfigPath(path)

    var img = assume Image.decode(".png", Icons48)
    img.crop(48, 48, 48, 48)
    img.setPixelFormat(.BGRA8)
    g_App.setAppIcon(img)

    let surface = try g_App.createSurface(0, 0, 10, 10, SurfaceFlags.OverlappedWindow, surfaceID: "Captme")
    let mainWnd = MainWnd.create(surface.wnd)
    surface.setView(mainWnd)
    mainWnd.createTopMenu()

    if Env.hasArg("swag.test"):
        g_App.maxRunFrame = 100
    if !Env.hasArg("--hide"):
        surface.show()

    // Icon task bar
    with Win32
    {
        with var tnid: NOTIFYICONDATAA
        {
            .cbSize           = #sizeof(NOTIFYICONDATAA)
            .hWnd             = surface.hWnd
            .uID              = 1
            .uFlags           = NIF_ICON | NIF_MESSAGE
            .uCallbackMessage = WM_USER
            .hIcon            = catch img.toHICON()
        }

        catch Shell_NotifyIconA(NIM_ADD, &tnid)
        defer catch Shell_NotifyIconA(NIM_DELETE, &tnid)
    }

    let name = Format.toString("Captme %.%.%", #swagversion, #swagrevision, #swagbuildnum)
    with Env
    {
        .registerApplication("Captme.exe", name)
        .associateFileExtension(".png", "Captme.exe", Env.getExe())
        .associateFileExtension(".jpg", "Captme.exe", Env.getExe())
    }

    foreach arg in args
    {
        if !Path.isValidPathName(arg):
            continue
        if !Image.canLoad(arg):
            continue
        Console.print(arg)
        mainWnd.loadExternalImage(arg)
        break
    }

    g_App.run()
}

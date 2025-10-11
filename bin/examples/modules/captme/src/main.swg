var g_App: Application

#main
{
    let args = Env.getArgs()
    foreach arg in args
    {
        if !Path.isValidPathName(arg) do
            continue
        if !Image.canLoad(arg) do
            continue
        Clipboard.addString(arg)
        if Surface.sendCustomMessage("Captme", 1, 0) do
            return
    }

    try Jobs.setNumWorkers()
    try Library.init()

    let path = Path.combine(Library.getFolder(), "captme.cfg")
    g_App.setConfigPath(path)

    var img = assume Image.decode(".png", Icons48)
    img.crop(48, 48, 48, 48)
    img.setPixelFormat(.BGRA8)
    g_App.setAppIcon(img)

    let surface = try g_App.createSurface(0, 0, 10, 10, SurfaceFlags.OverlappedWindow, surfaceID: "Captme")
    let mainWnd = MainWnd.create(surface.wnd)

    if Env.hasArg("swag.test") do
        g_App.maxRunFrame = 100
    if !Env.hasArg("--hide") do
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

    with Env
    {
        .registerApplication("Captme", "Captme")
        .associateFileExtension(".png", "Captme", Env.getExe())
        .associateFileExtension(".jpg", "Captme", Env.getExe())
    }

    foreach arg in args
    {
        if !Path.isValidPathName(arg) do
            continue
        if !Image.canLoad(arg) do
            continue
        Console.print(arg)
        mainWnd.loadExternalImage(arg)
        break
    }

    g_App.run()
}

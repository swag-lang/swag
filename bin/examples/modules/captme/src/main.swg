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

    let surface = try g_App.createSurface(0, 0, 10, 10, SurfaceFlags.OverlappedWindow)
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

    Env.associateFileExtension(".png", "captme.exe", Env.getExe())

    let args = Env.getArgs()
    foreach arg in args
    {
        if !Image.canLoad(arg):
            continue

        let imgToLoad = catch Image.load(arg)            
        if !imgToLoad.isValid():
            break

        var done = false
        foreach md in imgToLoad.metaDatas
        {
            if md.tag == "Captme.File"
            {
                let dupCapture = catch mainWnd.recentWnd.addRecent(md.value, true, true)
                Memory.delete(dupCapture)
                done = true
                break
            }
        }

        if !done
        {
            mainWnd.newCapture(imgToLoad, arg)
        }
    }

    g_App.run()
}



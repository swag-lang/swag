#global #if #os == Swag.TargetOs.Windows
using Win32, Core, Pixel, Gdi32

public struct ApplicationNative
{
    fileSmallIconsImage:         Image
    fileSmallIconsImageList:     ImageList
    mapFileSmallIcons:           HashTable'(String, s32)

    fileBigIconsImage:           Image
    fileBigIconsImageList:       ImageList
    mapFileBigIcons:             HashTable'(String, s32)

    hotKeysDisabled:             bool
}

impl Application
{
    func pickColorAtMouse()->Color
    {
        let awareness = Win32.SetThreadDpiAwarenessContext(Win32.DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE)
        defer discard Win32.SetThreadDpiAwarenessContext(awareness)

        let hdc = GetDC(null)

        var pt: POINT
        GetCursorPos(&pt)
        let col = GetPixel(hdc, pt.x, pt.y) | 0xFF000000

        ReleaseDC(null, hdc)
        return Color.fromAbgr(col)
    }

    mtd captureMouse(wnd: *Wnd)
    {
        .mouseCaptureWnd = wnd
        SetCapture(wnd.surface.native.hWnd)
    }

    mtd releaseMouse()
    {
        .mouseCaptureWnd = null
        ReleaseCapture()
    }

    #[Swag.Overload]
    func registerHotKey(mdf: Input.KeyModifiers, key: Input.Key) throw
    {
        var mod: u32
        if mdf & .Alt do
            mod |= MOD_ALT
        if mdf & .Control do
            mod |= MOD_CONTROL
        if mdf & .Shift do
            mod |= MOD_SHIFT
        var winkey = Input.Keyboard.keyToVirtualKey(key)

        // We need to have a unique id per shortcut
        var crc32: Hash.Crc32
        crc32.update(@mkslice(cast(const [*] u8) &mod, 4))
        crc32.update(@mkslice(cast(const [*] u8) &winkey, 4))
        crc32.crc &= 0x7FFFFFFF

        catch UnregisterHotKey(null, crc32.crc)
        RegisterHotKey(null, crc32.crc, mod, winkey)
    }

    mtd sendHotKey(msg: MSG)
    {
        if .hotKeysDisabled do
            return
        using Input
        var mdf: KeyModifiers
        if msg.lParam & MOD_ALT do
            mdf |= .Alt
        if msg.lParam & MOD_CONTROL do
            mdf |= .Control
        if msg.lParam & MOD_SHIFT do
            mdf |= .Shift
        let key = Keyboard.virtualKeyToKey(cast() (msg.lParam >> 16))

        let (id, target) = .getHotKeyShortcut(mdf, key)
        if id and target do
            target.sendCommandEvent(id)
    }

    mtd sendSystemEvents()
    {
        var msg: MSG
        while PeekMessageW(&msg, null, 0, 0, PM_REMOVE) != 0
        {
            if msg.message == WM_HOTKEY do
                .sendHotKey(msg)
            else
            {
                TranslateMessage(&msg)
                DispatchMessageW(&msg)
            }
        }
    }

    mtd getSurfaceAt(x, y: f32)->#null *Surface
    {
        let pt:  POINT{cast(s32) x, cast(s32) y}
        let hWnd = WindowFromPoint(pt)
        if hWnd == null do
            return null

        foreach s in .surfaces
        {
            if s.native.hWnd == hWnd
            {
                if .modalSurfaces.count and s != .modalSurfaces.back() do
                    return null
                return s
            }
        }

        return null
    }

    mtd addIconToImageList(name: string, hIcon: HICON, small: bool)->Icon
    {
        var result: retval

        var index:      s32
        var curImg:     *Image
        var curImgList: *ImageList

        if small
        {
            index = cast(s32) .mapFileSmallIcons.count
            .mapFileSmallIcons.add(name, index)
            curImg     = &.fileSmallIconsImage
            curImgList = &.fileSmallIconsImageList
        }
        else
        {
            index = cast(s32) .mapFileBigIcons.count
            .mapFileBigIcons.add(name, index)
            curImg     = &.fileBigIconsImage
            curImgList = &.fileBigIconsImageList
        }

        let sizeIcon = small ? 16 : 32

        // Convert icon to image
        var img = catch Image.from(hIcon)
        defer catch DestroyIcon(hIcon)
        if @err != null do
            return result
        img.resize(sizeIcon, sizeIcon)

        // Append to the image list
        var nimage = Image.create(sizeIcon * (index + 1), sizeIcon, .BGRA8)
        if curImg.isValid() do
            nimage.mix(curImg, 0, 0)
        nimage.mix(img, sizeIcon * index, 0)
        dref curImg = nimage

        if curImgList.texture.isValid() do
            .renderer.removeTexture(&curImgList.texture)
        curImgList.set(.renderer.addImage(curImg), sizeIcon, sizeIcon)

        return Icon.from(curImgList, index)
    }

    mtd getSpecialDirectoryIcon(what: Env.SpecialDirectory, small = true)->Icon
    {
        var val  = what
        let name = Reflection.getEnumName(Env.SpecialDirectory, &val).toString()

        if small
        {
            let here = .mapFileSmallIcons.tryFind(name)
            if here do
                return Icon.from(&.fileSmallIconsImageList, here.value)
        }
        else
        {
            let here = .mapFileBigIcons.tryFind(name)
            if here do
                return Icon.from(&.fileBigIconsImageList, here.value)
        }

        var sgi: SHSTOCKICONINFO
        sgi.cbSize = #sizeof(SHSTOCKICONINFO)

        var whatWin = SHSTOCKICONID.SIID_FOLDER
        switch what
        {
        case Desktop:
            whatWin = .SIID_DESKTOPPC
        case UserVideos:
            whatWin = .SIID_VIDEOFILES
        case UserPictures:
            whatWin = .SIID_IMAGEFILES
        case UserMusic:
            whatWin = .SIID_AUDIOFILES
        case CommonVideos:
            whatWin = .SIID_VIDEOFILES
        case CommonPictures:
            whatWin = .SIID_IMAGEFILES
        case CommonMusic:
            whatWin = .SIID_AUDIOFILES
        }

        catch SHGetStockIconInfo(whatWin, SHGSI_ICON | (small ? SHGSI_SMALLICON : SHGSI_LARGEICON), &sgi)
        if !sgi.hIcon do
            return {}
        return .addIconToImageList(name, sgi.hIcon, small)
    }

    mtd getFileDirectoryIcon(name: string, isDirectory: bool, small = true)->Icon
    {
        var result: retval
        if !@countof(name) do
            return result

        var ext = Path.getExtension(name)

        var extractFromFile = false
        if isDirectory and name[@countof(name) - 1] == Path.VolumeSeparatorChar do
            ext = "c:"
        elif isDirectory do
            ext = "c:/temp"
        elif ext == ".exe" or ext == ".dll"
        {
            extractFromFile = true
            ext             = name
        }
        elif ext == null do
            ext = ".txt"

        if small
        {
            let here = .mapFileSmallIcons.tryFind(ext)
            if here do
                return Icon.from(&.fileSmallIconsImageList, here.value)
        }
        else
        {
            let here = .mapFileBigIcons.tryFind(ext)
            if here do
                return Icon.from(&.fileBigIconsImageList, here.value)
        }

        var sfi: SHFILEINFOA
        if extractFromFile
        {
            catch ExtractIconExA(@dataof(ext), 0, null, &sfi.hIcon, 1)
            if !sfi.hIcon
            {
                ext = Path.getExtension(name)
                if ext == null do
                    ext = ".txt"
                if small
                {
                    let here = .mapFileSmallIcons.tryFind(ext)
                    if here do
                        return Icon.from(&.fileSmallIconsImageList, here.value)
                }
                else
                {
                    let here = .mapFileBigIcons.tryFind(ext)
                    if here do
                        return Icon.from(&.fileBigIconsImageList, here.value)
                }
            }
        }

        if !sfi.hIcon do
            discard SHGetFileInfoA(@dataof(ext), isDirectory ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL, &sfi, #sizeof(SHFILEINFOA), SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SMALLICON)
        if !sfi.hIcon do
            return result

        return .addIconToImageList(ext, sfi.hIcon, small)
    }
}

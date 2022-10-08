#global if #os == Swag.TargetOs.Windows
using Win32, Core, Pixel, Gdi32

public struct ApplicationNative
{
    fileIconsImage:     Image
    fileIconsImageList: ImageList
    mapFileIcons:       HashTable'(string, s32)
}

impl Application
{
    mtd pickColorAtMouse()->Color
    {
        hdc := GetDC(null)

        var pt: POINT
        GetCursorPos(&pt)
        col := GetPixel(hdc, pt.x, pt.y) | 0xFF000000

        ReleaseDC(null, hdc)
        return Color.fromAbgr(col)
    }

    // Capture mouse, on the window and surface
    mtd captureMouse(wnd: *Wnd)
    {
        mouseCaptureWnd = wnd
        SetCapture(wnd.surface.native.hWnd)
    }

    // Release captured mouse
    mtd releaseMouse()
    {
        mouseCaptureWnd = null
        ReleaseCapture()
    }

    mtd sendSystemEvents()
    {
        var msg: MSG
        while PeekMessageW(&msg, null, 0, 0, PM_REMOVE) != 0
        {
            TranslateMessage(&msg)
            DispatchMessageW(&msg)
        }
    }

    // Returns the surface under the given point
    mtd getSurfaceAt(x, y: f32)->*Surface
    {
        var pt: POINT
        pt.x = cast(s32) x
        pt.y = cast(s32) y
        hWnd := WindowFromPoint(pt)
        if hWnd == null
            return null

        visit s: surfaces
        {
            if s.native.hWnd == hWnd
            {
                if modalSurfaces.count and s != modalSurfaces.back()
                    return null
                return s
            }
        }

        return null
    }

    // Returns the OS icon associated with the given filename
    mtd getFileFolderIcon(name: string, isFolder: bool)->Icon throw
    {
        ext := Path.getExtension(name)

        extractFromFile := false
        if isFolder
            ext = "c:/temp"
        elif ext == ".exe" or ext == ".dll"
        {
            extractFromFile = true
            ext = name
        }
        elif ext == null
            ext = ".txt"

        here := mapFileIcons.find(ext)
        if here
            return Icon.from(&fileIconsImageList, here.value)

        var sfi: SHFILEINFOA
        if extractFromFile
        {
            discard ExtractIconExA(@dataof(ext), 0, null, &sfi.hIcon, 1)
            if !sfi.hIcon
            {
                ext = Path.getExtension(name)
                here = mapFileIcons.find(ext)
                if here
                    return Icon.from(&fileIconsImageList, here.value)
            }
        }

        if !sfi.hIcon
            discard SHGetFileInfoA(@dataof(ext), isFolder ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL, &sfi, @sizeof(SHFILEINFOA), SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SMALLICON)
        if !sfi.hIcon
            throw "invalid"

        index := cast(s32) mapFileIcons.count
        mapFileIcons.add(ext, index)

        img := Image.from(sfi.hIcon)
        img.resize(16, 16)

        nimage := Image.create(16 * (index + 1), 16, .BGRA8)
        if fileIconsImage.isValid()
            nimage.mix(fileIconsImage, 0, 0)
        nimage.mix(img, 16 * index, 0)
        fileIconsImage = nimage

        if fileIconsImageList.texture.isValid()
            renderer.removeTexture(&fileIconsImageList.texture)
        fileIconsImageList.set(renderer.addImage(fileIconsImage), 16, 16)

        return Icon.from(&fileIconsImageList, index)
    }
}
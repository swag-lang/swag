#global if #os == Swag.TargetOs.Windows
#global public
using Swag, Core, Win32, Gdi32

impl Image
{
    // Creates an image from a windows HICON
    func from(hicon: HICON)->Image throw
    {
        var result: retval
        var ii: ICONINFO

        GetIconInfo(hicon, &ii)
        result = from(ii.hbmColor)

        if ii.hbmMask != null
            DeleteObject(acast ii.hbmMask)
        if ii.hbmColor != null
            DeleteObject(acast ii.hbmColor)

        return result
    }

    // Creates an image from a windows HBITMAP
    func from(hbitmap: HBITMAP)->Image throw
    {
        var result: retval

        var bitmap: BITMAP
        GetObjectA(cast(HANDLE) hbitmap, @sizeof(BITMAP), &bitmap)

        switch bitmap.bmBitsPixel
        {
        case 24:
            result = create(bitmap.bmWidth, bitmap.bmHeight, PixelFormat.BGR8)
        case 32:
            result = create(bitmap.bmWidth, bitmap.bmHeight, PixelFormat.BGRA8)
        default:
            throw "invalid bitmap pixelformat"
        }

        dc := GetDC(null)
        defer ReleaseDC(null, dc)

        var infos: BITMAPINFO
        infos.bmiHeader.biSize        = @sizeof(BITMAPINFOHEADER)
        infos.bmiHeader.biWidth       = bitmap.bmWidth
        infos.bmiHeader.biHeight      = bitmap.bmHeight
        infos.bmiHeader.biPlanes      = bitmap.bmPlanes
        infos.bmiHeader.biBitCount    = bitmap.bmBitsPixel
        infos.bmiHeader.biCompression = BI_RGB
        GetDIBits(dc, hbitmap, 0, acast bitmap.bmHeight, result.pixels.buffer, &infos, acast DIB_RGB_COLORS)

        return result
    }
}
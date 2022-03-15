#global if #os == Swag.TargetOs.Windows
using Win32, Core, Pixel, Gdi32

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

    mtd changeCursor(wnd: *Wnd)
    {
        if cursorStack.count
            SetCursor(cursorStack.back().native.cursor)
        elif wnd and wnd.cursor.native.cursor != null
            SetCursor(wnd.cursor.native.cursor)
        else
            SetCursor(Cursor.from(CursorShape.Arrow).native.cursor)
    }
}
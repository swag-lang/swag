#global if #os == Swag.TargetOs.Windows
using Win32

impl Application
{
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

    func sendSystemEvents()
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

    mtd pushCursor(cursor: Cursor)
    {
        cursorStack.add(cursor)
    }

    mtd popCursor()
    {
        cursorStack.removeBack()
    }

    mtd changeCursor(wnd: *Wnd)
    {
        if cursorStack.count
            SetCursor(cursorStack.back().native.cursor)
        else if wnd and wnd.cursor.native.cursor != null
            SetCursor(wnd.cursor.native.cursor)
        else
            SetCursor(Cursor.from(CursorShape.Arrow).native.cursor)
    }
}
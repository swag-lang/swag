#global if WINDOWS
#global public
#global namespace mouse
using win32, math

// Returns the current mouse position
func getPos()->Point
{
    var pt: POINT = ?
    GetCursorPos(&pt)
    return Point{cast(f32) pt.x, cast(f32) pt.y}
}

// Set the current mouse position
func setPos(x, y: s32)
{
    SetCursorPos(x, y)
}

// Shoud or hide the mouse cursor
func showCursor(show: bool)
{
    ShowCursor(cast(BOOL) show)
}

#global if WINDOWS
#global public
#global namespace Input
using win32, math

// Returns the current mouse position
func getMousePos()->Point
{
    var pt: POINT = ?
    GetCursorPos(&pt)
    return Point{cast(f32) pt.x, cast(f32) pt.y}
}

// Set the current mouse position
func setMousePos(x, y: s32)
{
    SetCursorPos(x, y)
}

// Shoud or hide the mouse cursor
func showMouseCursor(show: bool)
{
    ShowCursor(cast(BOOL) show)
}

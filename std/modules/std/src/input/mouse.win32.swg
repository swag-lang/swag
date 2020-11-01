#if std.WINDOWS;
public namespace std.mouse;
using win32, math

func getPos()->Point
{
    var pt: POINT = ?
    GetCursorPos(&pt)
    return Point{cast(f32) pt.x, cast(f32) pt.y}
}

func setPos(x, y: s32)
{
    SetCursorPos(x, y)
}

func showCursor(show: bool)
{
    ShowCursor(cast(BOOL) show)
}

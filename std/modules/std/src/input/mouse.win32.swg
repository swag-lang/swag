public namespace std.mouse;
using win32, math

#if std.WINDOWS
{
    func getPos()->Point
    {
        var pt: POINT = ?
        GetCursorPos(&pt)
        return Point{pt.x, pt.y}
    }

    func setPos(x, y: s32)
    {
        SetCursorPos(x, y)
    }

    func showCursor(show: bool)
    {
        ShowCursor(cast(BOOL) show)
    }
}

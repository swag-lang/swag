#global namespace console

public enum Color
{
    Black
    White
    Gray
    Red
    Blue
    Green
    Cyan
    Yellow
    Magenta
    DarkRed
    DarkBlue
    DarkGreen
    DarkCyan
    DarkYellow
    DarkMagenta
}

var g_silentMode: s32

public func silent(value: bool)
{
    debug.assert(value || g_silentMode > 0)
    g_silentMode += value ? 1 : -1
}

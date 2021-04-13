#global namespace console

#[swag.enumindex]
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

// Write parameters to the console
public func print(params: ...)
{
    var buf: ConcatBuffer
    visit v: params
        strconv.convertAny(&buf, v, null)
    printf(buf.toString())
}

// Write a line  to the console
public func println(params: ...)
{
    print(params)
    print("\n")
}

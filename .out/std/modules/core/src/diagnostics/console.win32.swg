#global if WINDOWS
#global namespace console
using win32

var g_colorToWin: [@countof(Color)] u16 = ?

#[swag.compiler]
func colorToWin()
{
    using Color
    g_colorToWin[Black] 		= 0
    g_colorToWin[White] 		= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
    g_colorToWin[Gray]  		= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
    g_colorToWin[Red]  		    = FOREGROUND_RED | FOREGROUND_INTENSITY
    g_colorToWin[Green] 		= FOREGROUND_GREEN | FOREGROUND_INTENSITY
    g_colorToWin[Blue]  		= FOREGROUND_BLUE | FOREGROUND_INTENSITY
    g_colorToWin[Cyan]  		= FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY
    g_colorToWin[Magenta] 	    = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY
    g_colorToWin[Yellow] 		= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
    g_colorToWin[DarkRed] 	    = FOREGROUND_RED
    g_colorToWin[DarkGreen] 	= FOREGROUND_GREEN
    g_colorToWin[DarkBlue] 	    = FOREGROUND_BLUE
    g_colorToWin[DarkCyan] 	    = FOREGROUND_BLUE | FOREGROUND_GREEN
    g_colorToWin[DarkMagenta]   = FOREGROUND_BLUE | FOREGROUND_RED
    g_colorToWin[DarkYellow] 	= FOREGROUND_RED | FOREGROUND_GREEN
}

var g_stdOutputHandle: HANDLE
var g_stdOutputInitAttr: u16
var g_stdOutputCurAttr: u16

#run
{
    colorToWin()
}

#init
{
    g_stdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE)

    var csbi: CONSOLE_SCREEN_BUFFER_INFO
    discard GetConsoleScreenBufferInfo(g_stdOutputHandle, &csbi)
    g_stdOutputInitAttr = csbi.wAttributes
    g_stdOutputCurAttr = g_stdOutputInitAttr
}

#drop
{
    resetColor()
}

// Set the console foreground color
public func setTextColor(color: Color)
{
    attributes := g_colorToWin[color]
    g_stdOutputCurAttr &= BACKGROUND_MASK
    g_stdOutputCurAttr |= attributes
    discard SetConsoleTextAttribute(g_stdOutputHandle, g_stdOutputCurAttr)
}

// Set the console background color
public func setBackColor(color: Color)
{
    attributes := cast(u16) (g_colorToWin[color] << 4)
    g_stdOutputCurAttr &= FOREGROUND_MASK
    g_stdOutputCurAttr |= attributes
    discard SetConsoleTextAttribute(g_stdOutputHandle, g_stdOutputCurAttr)
}

// Restore the console colors to their default values
public func resetColor()
{
    discard SetConsoleTextAttribute(g_stdOutputHandle, g_stdOutputInitAttr)
}

// Write a message to the console
public func print(message: string, params: ...)
{
    if g_silentMode
        return

    if @countof(params)
    {
        str := format.toString(message, params)
        len := cast(u32) @countof(str)
        discard WriteFile(g_stdOutputHandle, @dataof(str), len, null, null)
    }
    else
    {
        len := cast(u32) @countof(message)
        discard WriteFile(g_stdOutputHandle, @dataof(message), len, null, null)
    }
}

// Write a line  to the console
public func println(message: string, params: ...)
{
    print(message, params)
    if @countof(message) && message[@countof(message) - 1] != "\n"'u8
        print("\n", params)
}
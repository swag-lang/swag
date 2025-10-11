#global #if WINDOWS
#global namespace Console
using Win32

var g_ColorToWin: [@countof(Color)] u16 = undefined

#[Swag.Compiler]
func colorToWin()
{
    using Color
    g_ColorToWin[Black]       = 0
    g_ColorToWin[White]       = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
    g_ColorToWin[Gray]        = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
    g_ColorToWin[Red]         = FOREGROUND_RED | FOREGROUND_INTENSITY
    g_ColorToWin[Green]       = FOREGROUND_GREEN | FOREGROUND_INTENSITY
    g_ColorToWin[Blue]        = FOREGROUND_BLUE | FOREGROUND_INTENSITY
    g_ColorToWin[Cyan]        = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY
    g_ColorToWin[Magenta]     = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY
    g_ColorToWin[Yellow]      = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
    g_ColorToWin[DarkRed]     = FOREGROUND_RED
    g_ColorToWin[DarkGreen]   = FOREGROUND_GREEN
    g_ColorToWin[DarkBlue]    = FOREGROUND_BLUE
    g_ColorToWin[DarkCyan]    = FOREGROUND_BLUE | FOREGROUND_GREEN
    g_ColorToWin[DarkMagenta] = FOREGROUND_BLUE | FOREGROUND_RED
    g_ColorToWin[DarkYellow]  = FOREGROUND_RED | FOREGROUND_GREEN
}

var g_StdOutputHandle:   HANDLE
var g_StdOutputInitAttr: u16
var g_StdOutputCurAttr:  u16

#run
{
    colorToWin()
}

#init
{
    catch AttachConsole(ATTACH_PARENT_PROCESS)
    g_StdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE)

    var csbi: CONSOLE_SCREEN_BUFFER_INFO
    catch GetConsoleScreenBufferInfo(g_StdOutputHandle, &csbi)
    g_StdOutputInitAttr = csbi.wAttributes
    g_StdOutputCurAttr  = g_StdOutputInitAttr
}

#drop
{
    resetColor()
    catch FreeConsole()
}

// Set the console foreground color
public func setTextColor(color: Color)
{
    let attributes = g_ColorToWin[color]
    g_StdOutputCurAttr &= BACKGROUND_MASK
    g_StdOutputCurAttr |= attributes
    catch SetConsoleTextAttribute(g_StdOutputHandle, g_StdOutputCurAttr)
}

// Set the console background color
public func setBackColor(color: Color)
{
    let attributes = cast(u16) (g_ColorToWin[color] << 4)
    g_StdOutputCurAttr &= FOREGROUND_MASK
    g_StdOutputCurAttr |= attributes
    catch SetConsoleTextAttribute(g_StdOutputHandle, g_StdOutputCurAttr)
}

// Restore the console colors to their default values
public func resetColor()
{
    catch SetConsoleTextAttribute(g_StdOutputHandle, g_StdOutputInitAttr)
}

// Write a formatted message to the console
public func printf(message: string, params: ...)
    verify
    {
        if !#isconstexpr(message) or !#isconstexpr(params) do
            return true
        Format.checkFormat(#curlocation.funcName, message, params)
        return true
    }
{
    if g_SilentMode do
        return
    if message == null do
        return

    if @countof(params)
    {
        let str = Format.toString(message, params)
        let len = cast(u32) @countof(str)
        catch WriteFile(g_StdOutputHandle, @dataof(str), len, null, null)
    }
    else
    {
        let len = cast(u32) @countof(message)
        catch WriteFile(g_StdOutputHandle, @dataof(message), len, null, null)
    }
}

// Wait for using input, and returns the corresponding string
public func prompt()->String
{
    const BufSize = 1024'u32

    var result: retval
    var buffer: [BufSize] u8
    var dwRead: DWORD

    let h = GetStdHandle(STD_INPUT_HANDLE)
    for
    {
        if !ReadFile(h, &buffer[0], BufSize, &dwRead, null) do
            break
        result.append(@mkstring(&buffer[0], dwRead))
        if dwRead < BufSize do
            break
        if buffer[BufSize - 1] == '\n' do
            break
    }

    return result
}

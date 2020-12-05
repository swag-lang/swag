#if std.WINDOWS;
public namespace std.debugger;
using win32

// Returns true if a debugger is attached to the current process
func isAttached()       => cast(bool) IsDebuggerPresent()

// Trigger a breakpoint
func debugBreak()       => DebugBreak()

// Debug log
func log(msg: string)   => OutputDebugStringA(cast(const *u8) msg)

// Try to attach to a debugger by calling "vsjitdebugger.exe"
// If it works, it will open a dialog box to pickup a visual studio instance
func attach()->ErrorId
{
    var si: STARTUPINFOA
    var pi: PROCESS_INFORMATION
    si.cb = @sizeof(si)

    var cmdLine: strconv.StringBuilder
    cmdLine.appendString("vsjitdebugger.exe ")
    cmdLine.appendString("-p ")
    cmdLine.appendAny(GetCurrentProcessId())
    cmdLine.zeroTerminated()
    str := cmdLine.moveToString()

    if !CreateProcessA(null, @dataof(str), null, null, FALSE, 0, null, null, &si, &pi)
    {
        error.setLastErrorId()
        return error.last()
    }

    while !IsDebuggerPresent() thread.sleep(100)
    return null
}

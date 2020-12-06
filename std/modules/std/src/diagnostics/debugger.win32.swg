#if WINDOWS;
public namespace debugger;
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
    var builder: strconv.StringBuilder
    builder.appendString("-p ")
    builder.appendAny(GetCurrentProcessId())
    builder.zeroTerminated()
    arguments := builder.moveToString()

    (proc, err) := process.start("vsjitdebugger.exe", arguments.toString())
    if err return err

    CloseHandle(proc.handle)

    while !IsDebuggerPresent() thread.sleep(100)
    return null
}

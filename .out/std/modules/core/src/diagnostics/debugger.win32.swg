#global if WINDOWS
#global public
#global namespace Debugger
using win32

// Returns true if a debugger is attached to the current process
func isAttached()       => cast(bool) IsDebuggerPresent()

// Trigger a breakpoint
func debugBreak()       => DebugBreak()

// Debug log
func log(msg: string)   => OutputDebugStringA(cast(const *u8) msg)

// Try to attach to a debugger by calling "vsjitdebugger.exe"
// If it works, it will open a dialog box to pickup a visual studio instance
func attach() throw
{
    var builder: strconv.StringBuilder
    builder.appendString("-p ")
    builder.appendAny(GetCurrentProcessId())
    builder.zeroTerminated()
    arguments := builder.moveToString()

    proc := try process.start("vsjitdebugger.exe", arguments.toString())
    CloseHandle(proc)

    while !IsDebuggerPresent() thread.sleep(100)
}

#global #if WINDOWS
#global public
#global namespace Debugger
using Win32

// Returns true if a debugger is attached to the current process
func isAttached()->bool => cast(bool) IsDebuggerPresent()

// Trigger a breakpoint
func debugBreak() = DebugBreak()

// Debug log
func log(msg: string) = OutputDebugStringA(cast(const ^u8) msg)

// Try to attach to a debugger by calling "vsjitdebugger.exe"
// If it works, it will open a dialog box to pickup a visual studio instance
func attach() throw
{
    var builder: StrConv.StringBuilder
    builder.appendString("-p ")
    builder.appendAny(GetCurrentProcessId())
    builder.zeroTerminated()
    var arguments = builder.toString()

    var proc = try Env.startProcess("vsjitdebugger.exe", arguments.toString())
    CloseHandle(proc)

    while !IsDebuggerPresent():
        Threading.Thread.sleep(100'ms)
}

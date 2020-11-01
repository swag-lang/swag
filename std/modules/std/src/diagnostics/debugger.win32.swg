#if std.WINDOWS;
public namespace std.debugger;
using win32

func isAttached()       => cast(bool) IsDebuggerPresent()
func debugBreak()       => DebugBreak()
func log(msg: string)   => OutputDebugStringA(cast(const *u8) msg)

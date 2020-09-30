public namespace std.debugger;
using win32

#if std.WINDOWS
{
    func isAttached()       => cast(bool) IsDebuggerPresent()
    func break()            => DebugBreak()
    func log(msg: string)   => OutputDebugStringA(cast(const *u8) msg)
}
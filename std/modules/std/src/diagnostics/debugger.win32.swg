#if std.WINDOWS
{
    using win32

    public namespace std.debugger
    {
        func isAttached() => cast(bool) IsDebuggerPresent()
        func break() => DebugBreak()
        func log(msg: string) => OutputDebugStringA(cast(const *u8) msg)
    }
}
#if std.WINDOWS
{
    using win32

    namespace std.diagnostics.debugger
    {
        public
        {
            func isAttached() => cast(bool) IsDebuggerPresent()
            func break() => DebugBreak()
            func log(msg: string) => OutputDebugStringA(cast(const *u8) msg)
        }
    }
}
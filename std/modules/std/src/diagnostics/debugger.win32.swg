#if swag.WINDOWS
{
    using win32

    namespace std.diagnostics.debugger
    {
        public
        {
            func isAttached() => IsDebuggerPresent()
            func break() => DebugBreak()
            func log(msg: string) => OutputDebugStringA(cast(const *u8) msg)
        }
    }
}
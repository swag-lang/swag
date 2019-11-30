namespace std.diagnostics.debugger
{
	public
	{
		func isAttached() => os.IsDebuggerPresent()
		func break() => os.DebugBreak()
		func log(msg: string) => os.OutputDebugStringA(msg)
	}
}
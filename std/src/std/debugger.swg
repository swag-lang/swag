namespace std.diagnostics.debugger
{
	#[swag.public]
	{
		func isAttached() => os.IsDebuggerPresent()
		func break() => os.DebugBreak()
		func log(msg: string) => os.OutputDebugString(msg)
	}
}
namespace std.diagnostics.debug
{
	#[swag.inline]
	public func assert(expr: bool, file: string = #callerfile, line: u32 = #callerline)
	{
		#if ASSERT
		{
			@assert(expr)
		}
	}
}
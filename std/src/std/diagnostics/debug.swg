namespace std.diagnostics.debug
{
	#[swag.public, swag.inline]
	func assert(expr: bool, file: string = #callerfile, line: u32 = #callerline)
	{
		@assert(expr)
	}
}
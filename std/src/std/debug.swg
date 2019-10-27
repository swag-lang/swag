namespace std
{
	#[swag.public]
	func assert(expr: bool, file: string = #callerfile, line: u32 = #callerline)
	{
		@assert(expr)
	}
}
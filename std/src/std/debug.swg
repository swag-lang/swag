namespace std.debug
{
	#[swag.public]
	func assert(expr: bool, file: string = #callerfile, line: u32 = #callerline)
	{
		#version(DEBUG)
		{
			@assert(expr)
			@print("assert\n")
		}
	}
}
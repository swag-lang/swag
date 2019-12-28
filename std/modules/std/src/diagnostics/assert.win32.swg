#if swag.WINDOWS && swag.ASSERT
{
	namespace std.os
	{
		public func assertMessage(file: string = #callerfile, line: u32 = #callerline)
		{
			@assert(false)
		}
	}
}
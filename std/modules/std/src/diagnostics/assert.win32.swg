#if swag.WINDOWS && swag.ASSERT
{
    namespace std.os
    {
    	public func assertMessage(file: string, line: u32)
		{
			MessageBoxA(null, "", "", 0)
			@assert(false)
		}
	}
}
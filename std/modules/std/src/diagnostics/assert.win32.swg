#if swag.WINDOWS && swag.ASSERT
{
    namespace std.os
    {
    	public func assertMessage(message: string)
		{
			var buffer: [1024] u8 = ?

			const msg = "toto la fri"
			memory.copy(&buffer[0], @dataof(msg), @countof(msg) + 1)

			result := MessageBoxA(null, &buffer[0], "Swag Assertion Failed!", MB_ICONERROR | MB_CANCELTRYCONTINUE)
			switch result
			{
				case IDCANCEL:
					ExitProcess(0xFFFFFFFF)
				case IDTRYAGAIN:
					DebugBreak();
				case IDCONTINUE:
			}
		}
	}
}
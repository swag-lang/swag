#if swag.WINDOWS && swag.ASSERT
{
    namespace std.os
    {
    	public func assertMessage(file: string, line: u32)
		{
			result := MessageBoxA(null, file, "Swag Assertion Failed!", MB_ICONERROR | MB_CANCELTRYCONTINUE)
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
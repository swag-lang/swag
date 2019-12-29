#if swag.WINDOWS && swag.ASSERT
{
    namespace std.os
    {
    	public func assertMessage(message: string)
		{
			var title: [128] u16 = ?
			count := text.EncodingUni16.fromUtf8(title, "Swag Assertion Failed!")
			title[count] = 0

			result := MessageBoxW(null, title, title, MB_ICONERROR | MB_CANCELTRYCONTINUE)
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
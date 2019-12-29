#if swag.WINDOWS && swag.ASSERT
{
    namespace std.os
    {
    	public func assertMessage(message: string)
		{
			// User message
			var messageW: [512] u16 = ?
			count := text.unicode16.fromUtf8(messageW, message)
			messageW[count] = 0

			// Message box title
			var titleW: [128] u16 = ?
			count = text.unicode16.fromUtf8(titleW, "Swag Assertion Failed!")
			titleW[count] = 0

			result := MessageBoxW(null, messageW, titleW, MB_ICONERROR | MB_CANCELTRYCONTINUE)
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
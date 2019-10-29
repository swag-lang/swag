namespace std.os
{
	typealias HANDLE: 		*void
	typealias WORD: 		u16
	typealias DWORD: 		u32
	typealias BOOL: 		bool
	typealias LPCVOID:		const *void
	typealias LPCSTR:		string
	typealias LPDWORD:		*DWORD
	typealias LPOVERLAPPED:	*DWORD
	
	const STD_INPUT_HANDLE 	= cast(DWORD) -10
	const STD_OUTPUT_HANDLE = cast(DWORD) -11
	
	const FOREGROUND_BLUE      = 0x0001
	const FOREGROUND_GREEN     = 0x0002 
	const FOREGROUND_RED       = 0x0004
	const FOREGROUND_INTENSITY = 0x0008
	const FOREGROUND_MASK	   = 0x000F
	const BACKGROUND_BLUE      = 0x0010
	const BACKGROUND_GREEN     = 0x0020
	const BACKGROUND_RED       = 0x0040
	const BACKGROUND_INTENSITY = 0x0080
	const BACKGROUND_MASK	   = 0x00F0
		
	struct COORD
	{
		x: WORD
		y: WORD
	}
	
	struct SMALL_RECT
	{
		left: 	WORD
		top:  	WORD
		right: 	WORD
		bottom: WORD
	}	
	
	struct CONSOLE_SCREEN_BUFFER_INFO 
	{
		dwSize: 				COORD 
		dwCursorPosition: 		COORD
		wAttributes: 			WORD
		srWindow: 				SMALL_RECT
		dwMaximumWindowSize: 	COORD
	}	
	
	#[swag.foreign("kernel32")]
	{
		func GetStdHandle(nStdHandle: DWORD)->HANDLE;
		func GetConsoleScreenBufferInfo(hConsoleOutput: HANDLE, lpConsoleScreenBufferInfo: *CONSOLE_SCREEN_BUFFER_INFO)->bool;
		func WriteFile(hFile: HANDLE, lpBuffer: LPCVOID, nNumberOfBytesToWrite: DWORD, lpNumberOfBytesWritten: LPDWORD, lpOverlapped: LPOVERLAPPED);
		func SetConsoleTextAttribute(hConsoleOutput: HANDLE, wAttributes: WORD)->BOOL;
		func IsDebuggerPresent()->BOOL;
		func DebugBreak();
		func OutputDebugString(lpOutputString: LPCSTR);
	}
	
	var g_stdOutputHandle: HANDLE
	
	#init
	{
		g_stdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE)
		console.init()
	}
	
	#drop
	{
		console.drop()
	}	
}
namespace std.os.win32
{
	#[swag.foreign("kernel32")]
	{
		typealias HANDLE: 		*void
		typealias WORD: 		u16
		typealias DWORD: 		u32
		typealias BOOL: 		bool
		typealias LPCVOID:		const *void
		typealias LPDWORD:		*DWORD
		typealias LPOVERLAPPED:	*DWORD
		
		const STD_INPUT_HANDLE 	= cast(DWORD) -10
		const STD_OUTPUT_HANDLE = cast(DWORD) -11
		
		const FOREGROUND_BLUE      = 0x0001
		const FOREGROUND_GREEN     = 0x0002 
		const FOREGROUND_RED       = 0x0004
		const FOREGROUND_INTENSITY = 0x0008
		const BACKGROUND_BLUE      = 0x0010
		const BACKGROUND_GREEN     = 0x0020
		const BACKGROUND_RED       = 0x0040
		const BACKGROUND_INTENSITY = 0x0080
		
		func GetStdHandle(nStdHandle: DWORD)->HANDLE;
		func WriteFile(hFile: HANDLE, lpBuffer: LPCVOID, nNumberOfBytesToWrite: DWORD, lpNumberOfBytesWritten: LPDWORD, lpOverlapped: LPOVERLAPPED);
		func SetConsoleTextAttribute(hConsoleOutput: HANDLE, wAttributes: WORD)->BOOL;
	}
	
	var stdOutputHandle: HANDLE
	#init
	{
		stdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE)
	}
}
#[swag.foreign("kernel32")]
{
	typealias HANDLE: 		u32
	typealias WORD: 		u16
	typealias DWORD: 		u32
	typealias BOOL: 		bool
	typealias LPCVOID:		const *void
	typealias LPDWORD:		*DWORD
	typealias LPOVERLAPPED:	*DWORD
	
	const STD_INPUT_HANDLE 	= cast(HANDLE) -10
	const STD_OUTPUT_HANDLE = cast(HANDLE) -11
	
	func GetStdHandle(nStdHandle: DWORD)->HANDLE;
	func WriteFile(hFile: HANDLE, lpBuffer: LPCVOID, nNumberOfBytesToWrite: DWORD, lpNumberOfBytesWritten: LPDWORD, lpOverlapped: LPOVERLAPPED);
	func SetConsoleTextAttribute(hConsoleOutput: HANDLE, wAttributes: WORD)->BOOL;
}

var stdOutputHandle: HANDLE
#init
{
	stdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE)
}

#test
{
	WriteFile(stdOutputHandle, @dataof("toto\n"), 5, null, null)
}

alias HANDLE= 		*void
alias WORD= 		u16
alias DWORD= 		u32
alias BOOL= 		bool
alias LPCVOID=		const *void
alias LPDWORD=		*DWORD
alias LPOVERLAPPED= *DWORD

const STD_INPUT_HANDLE 	= bitcast(DWORD) -10
const STD_OUTPUT_HANDLE = bitcast(DWORD) -11

#[Swag.foreign("kernel32")]
{
	func GetStdHandle(nStdHandle: DWORD)->HANDLE;
	func WriteFile(hFile: HANDLE, lpBuffer: LPCVOID, nNumberOfBytesToWrite: DWORD, lpNumberOfBytesWritten: LPDWORD, lpOverlapped: LPOVERLAPPED)->s32;
	func SetConsoleTextAttribute(hConsoleOutput: HANDLE, wAttributes: WORD)->BOOL;
}

var stdOutputHandle: HANDLE
#init
{
	stdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE)
}

#test
{
	//WriteFile(stdOutputHandle, @dataof("toto\n"), 5, null, null)
}

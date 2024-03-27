typealias HANDLE       = *void
typealias WORD         = u16
typealias DWORD        = u32
typealias BOOL         = bool
typealias LPCVOID      = const *void
typealias LPDWORD      = *DWORD
typealias LPOVERLAPPED = *DWORD

const STD_INPUT_HANDLE  = cast,bit(DWORD) -10
const STD_OUTPUT_HANDLE = cast,bit(DWORD) -11

#[Swag.Foreign("kernel32")]
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
}

#global public
#foreignlib "kernel32"

alias HANDLE = 			const *void
alias HLOCAL = 			HANDLE
alias HINSTANCE =		const *void
alias HMODULE =		    const *void
alias ATOM =		    const *void
alias LRESULT =		    s64
alias WORD = 			u16
alias DWORD =           u32
alias BOOL = 	        s32
alias BYTE = 			u8
alias LPBYTE = 			*BYTE
alias LONG = 			s32
alias ULONG =           u32
alias LPVOID =			*void
alias LPCVOID =			const *void
alias LPCSTR =			const *u8
alias LPCWSTR =			const *u16
alias LPCTSTR =			const *u16
alias LPDWORD =			*DWORD
alias LPOVERLAPPED =	*DWORD
alias DWORD_PTR =		uint
alias UINT =			u32
alias SHORT =			s16
alias LARGE_INTEGER =	s64
alias PLONG =           *LONG
alias CHAR =            u8
alias LPSTR =           *CHAR
alias SIZE_T =          uint
alias va_list =         *u8

const STD_INPUT_HANDLE 	= cast(DWORD) -10
const STD_OUTPUT_HANDLE = cast(DWORD) -11

const ERROR_SUCCESS = 0
const FALSE = 0
const TRUE  = 1

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

const FILE_SHARE_READ   = 0x00000001
const FILE_SHARE_WRITE  = 0x00000002
const FILE_SHARE_DELETE = 0x00000004

const FILE_ATTRIBUTE_READONLY              = 0x00000001
const FILE_ATTRIBUTE_HIDDEN                = 0x00000002
const FILE_ATTRIBUTE_SYSTEM                = 0x00000004
const FILE_ATTRIBUTE_DIRECTORY             = 0x00000010
const FILE_ATTRIBUTE_ARCHIVE               = 0x00000020
const FILE_ATTRIBUTE_DEVICE                = 0x00000040
const FILE_ATTRIBUTE_NORMAL                = 0x00000080
const FILE_ATTRIBUTE_TEMPORARY             = 0x00000100
const FILE_ATTRIBUTE_SPARSE_FILE           = 0x00000200
const FILE_ATTRIBUTE_REPARSE_POINT         = 0x00000400
const FILE_ATTRIBUTE_COMPRESSED            = 0x00000800
const FILE_ATTRIBUTE_OFFLINE               = 0x00001000
const FILE_ATTRIBUTE_NOT_CONTENT_INDEXED   = 0x00002000
const FILE_ATTRIBUTE_ENCRYPTED             = 0x00004000
const FILE_ATTRIBUTE_INTEGRITY_STREAM      = 0x00008000
const FILE_ATTRIBUTE_VIRTUAL               = 0x00010000
const FILE_ATTRIBUTE_NO_SCRUB_DATA         = 0x00020000
const FILE_ATTRIBUTE_EA                    = 0x00040000
const FILE_ATTRIBUTE_PINNED                = 0x00080000
const FILE_ATTRIBUTE_UNPINNED              = 0x00100000
const FILE_ATTRIBUTE_RECALL_ON_OPEN        = 0x00040000
const FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS = 0x00400000

const GENERIC_READ    = 0x80000000
const GENERIC_WRITE   = 0x40000000
const GENERIC_EXECUTE = 0x20000000
const GENERIC_ALL     = 0x10000000

const FILE_BEGIN   = 0'u32
const FILE_CURRENT = 1'u32
const FILE_END     = 2'u32
const INVALID_FILE_SIZE         = cast(DWORD) -1
const INVALID_SET_FILE_POINTER  = cast(DWORD) -1
const INVALID_FILE_ATTRIBUTES   = cast(DWORD) -1

const CREATE_NEW          = 1'u32
const CREATE_ALWAYS       = 2'u32
const OPEN_EXISTING       = 3'u32
const OPEN_ALWAYS         = 4'u32
const TRUNCATE_EXISTING   = 5'u32

const INVALID_HANDLE_VALUE = cast(HANDLE) cast(uint) -1
const CONSOLE_TEXTMODE_BUFFER = 1'u32
const MAX_PATH = 260
const INFINITE = 0xFFFFFFFF
const CREATE_SUSPENDED = 0x00000004
const MAXIMUM_WAIT_OBJECTS = 64'u32

const FORMAT_MESSAGE_ALLOCATE_BUFFER = 0x00000100
const FORMAT_MESSAGE_IGNORE_INSERTS  = 0x00000200
const FORMAT_MESSAGE_FROM_STRING     = 0x00000400
const FORMAT_MESSAGE_FROM_HMODULE    = 0x00000800
const FORMAT_MESSAGE_FROM_SYSTEM     = 0x00001000
const FORMAT_MESSAGE_ARGUMENT_ARRAY  = 0x00002000
const FORMAT_MESSAGE_MAX_WIDTH_MASK  = 0x000000FF

const LANG_ENGLISH      = 0x09
const SUBLANG_DEFAULT   = 0x01
func MAKELANGID(p, s: WORD) => (s << 10) | p

const WAIT_TIMEOUT = 258

const THREAD_PRIORITY_LOWEST        = -2
const THREAD_PRIORITY_BELOW_NORMAL  = -1
const THREAD_PRIORITY_NORMAL        = 0
const THREAD_PRIORITY_ABOVE_NORMAL  = 1
const THREAD_PRIORITY_HIGHEST       = 2

func LOWORD(l: u64) => cast(s32) (l & 0xffff)
func HIWORD(l: u64) => cast(s32) ((l >> 16) & 0xffff)

struct COORD
{
    x: WORD
    y: WORD
}

struct POINT
{
    x: LONG
    y: LONG
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

struct SYSTEM_INFO
{
    dwOemId:	DWORD // Must be updated to an union
    dwPageSize:	DWORD
    lpMinimumApplicationAddress:	LPVOID
    lpMaximumApplicationAddress:	LPVOID
    dwActiveProcessorMask:			DWORD_PTR
    dwNumberOfProcessors:		    DWORD
    dwProcessorType:			    DWORD
    dwAllocationGranularity:	    DWORD
    wProcessorLevel:			    WORD
    wProcessorRevision:			    WORD
}

struct SYSTEMTIME
{
    wYear:			WORD
    wMonth:			WORD
    wDayOfWeek:		WORD
    wDay:			WORD
    wHour:			WORD
    wMinute:		WORD
    wSecond:		WORD
    wMilliseconds:	WORD
}

struct SECURITY_ATTRIBUTES
{
    nLength:                DWORD
    lpSecurityDescriptor:   LPVOID
    bInheritHandle:         BOOL
}

struct FILETIME
{
    dwLowDateTime:    DWORD
    dwHighDateTime:   DWORD
}

struct WIN32_FILE_ATTRIBUTE_DATA
{
    dwFileAttributes:  DWORD
    ftCreationTime:    FILETIME
    ftLastAccessTime:  FILETIME
    ftLastWriteTime:   FILETIME
    nFileSizeHigh:     DWORD
    nFileSizeLow:      DWORD
}

struct WIN32_FIND_DATAA
{
    dwFileAttributes:   DWORD
    ftCreationTime:     FILETIME
    ftLastAccessTime:   FILETIME
    ftLastWriteTime:    FILETIME
    nFileSizeHigh:      DWORD
    nFileSizeLow:       DWORD
    dwReserved0:        DWORD
    dwReserved1:        DWORD
    cFileName:          [MAX_PATH] CHAR
    cAlternateFileName: [14] CHAR
    dwFileType:         DWORD
    dwCreatorType:      DWORD
    wFinderFlags:       WORD
}

enum GET_FILEEX_INFO_LEVELS
{
    GetFileExInfoStandard
    GetFileExMaxInfoLevel
}

struct STARTUPINFOA
{
    cb:                 DWORD
    lpReserved:         LPSTR
    lpDesktop:          LPSTR
    lpTitle:            LPSTR
    dwX:                DWORD
    dwY:                DWORD
    dwXSize:            DWORD
    dwYSize:            DWORD
    dwXCountChars:      DWORD
    dwYCountChars:      DWORD
    dwFillAttribute:    DWORD
    dwFlags:            DWORD
    wShowWindow:        WORD
    cbReserved2:        WORD
    lpReserved2:        LPBYTE
    hStdInput:          HANDLE
    hStdOutput:         HANDLE
    hStdError:          HANDLE
}

struct PROCESS_INFORMATION
{
    hProcess:       HANDLE
    hThread:        HANDLE
    dwProcessId:    DWORD
    dwThreadId:     DWORD
}

#[swag.foreign("kernel32")]
{
    func GetLastError()->DWORD;
    func FormatMessageA(dwFlags : DWORD, lpSource : LPCVOID, dwMessageId : DWORD, dwLanguageId : DWORD, lpBuffer: LPCSTR, nSize: DWORD, Arguments: *va_list)->DWORD;
    func IsDebuggerPresent()->BOOL;
    func DebugBreak();
    func OutputDebugStringA(lpOutputString: LPCSTR);
    func OutputDebugStringW(lpOutputString: LPCWSTR);
    func DebugActiveProcess(procId: DWORD);

    func CreateProcessA(lpApplicationName: LPCSTR, lpCommandLine: LPCSTR, lpProcessAttributes: *SECURITY_ATTRIBUTES, lpThreadAttributes: *SECURITY_ATTRIBUTES, bInheritHandles: BOOL, dwCreationFlags: DWORD, lpEnvironment: LPVOID, lpCurrentDirectory: LPCSTR, lpStartupInfo: *STARTUPINFOA, lpProcessInformation: *PROCESS_INFORMATION)->BOOL;
    func GetProcAddress(hModule: HMODULE, lpProcName: LPCSTR)->const *void;
    func GetCurrentProcessId()->DWORD;
    func GetModuleHandleA(lpModuleName: LPCSTR)->HMODULE;
    func GetStdHandle(nStdHandle: DWORD)->HANDLE;
    func GetConsoleScreenBufferInfo(hConsoleOutput: HANDLE, lpConsoleScreenBufferInfo: *CONSOLE_SCREEN_BUFFER_INFO)->BOOL;
    func SetConsoleTextAttribute(hConsoleOutput: HANDLE, wAttributes: WORD)->BOOL;
    func CreateConsoleScreenBuffer(dwDesiredAccess: DWORD, dwShareMode: DWORD, lpSecurityAttributes: const *SECURITY_ATTRIBUTES, dwFlags: DWORD, lpScreenBufferData: LPVOID)->HANDLE;
    func SetConsoleActiveScreenBuffer(hConsoleOutput: HANDLE);
    func WriteConsoleOutputCharacterA(hConsoleOutput: HANDLE, lpCharacter: LPCSTR, nLength: DWORD, dwWriteCoord: DWORD, lpNumberOfCharsWritten: LPDWORD)->BOOL;
    func WriteConsoleOutputCharacterW(hConsoleOutput: HANDLE, lpCharacter: LPCTSTR, nLength: DWORD, dwWriteCoord: DWORD, lpNumberOfCharsWritten: LPDWORD)->BOOL;
    func SetConsoleOutputCP(codePage: UINT);

    func GetSystemInfo(lpSystemInfo: *SYSTEM_INFO);
    func LocalFree(hLocal: HLOCAL)->HLOCAL;

    func QueryPerformanceCounter(lpPerformanceCount: *LARGE_INTEGER);
    func QueryPerformanceFrequency(lpFrequency: *LARGE_INTEGER);
    func GetLocalTime(lpSystemTime: *SYSTEMTIME);

    func Sleep(dwMilliseconds: DWORD);
    func SleepEx(dwMilliseconds: DWORD, bAltertable: BOOL);
    func CreateThread(lpThreadAttributes: *SECURITY_ATTRIBUTES, dwStackSize: SIZE_T, lpStartAddress: const *void, lpParameter: *void, dwCreationFlags: DWORD, lpThreadId : *DWORD)->HANDLE;
    func SetThreadPriority(hThread: HANDLE, nPriority: s32)->BOOL;
    func WaitForSingleObject(hHandle: HANDLE, dwMilliseconds: DWORD)->DWORD;
    func WaitForMultipleObjects(nCount: DWORD, lpHandles: const *HANDLE, bWaitAll: BOOL, dwMilliseconds: DWORD)->DWORD;
    func ExitProcess(uExitCode: UINT);
    func ResumeThread(hThread: HANDLE)->DWORD;
    func SwitchToThread()->BOOL;
    func GetCurrentThreadId()->DWORD;
    func GetCurrentThread()->HANDLE;
    func CreateMutexA(lpMutexAttributes: *SECURITY_ATTRIBUTES, bInitialOwner: BOOL, lpName: LPCSTR)->HANDLE;
    func ReleaseMutex(hMutex: HANDLE)->BOOL;

    func CreateWaitableTimerA(lpTimerAttributes: *SECURITY_ATTRIBUTES, bManualReset: BOOL, lpTimerName: LPCSTR)->HANDLE;
    func CancelWaitableTimer(hTimer: HANDLE)->BOOL;
    func SetWaitableTimer(hTimer: HANDLE, lpDueTime: const *LARGE_INTEGER, lPeriod: LONG, pfnCompletionRoutine: *void, lpArgToCompletionRoutine: LPVOID, fResume: BOOL)->BOOL;
    func CreateTimerQueueTimer(phNewTimer: *HANDLE, timerQueue: HANDLE, callback: const *void, parameter: *void, dueTime: DWORD, period: DWORD, flags: ULONG)->BOOL;
    func DeleteTimerQueueTimer(timerQueue: HANDLE, timer: HANDLE, completionEvent: HANDLE)->BOOL;

    func GetFileAttributesA(lpFileName: LPCSTR)->DWORD;
    func GetFileAttributesExA(lpFileName: LPCSTR, fInfoLevelId: GET_FILEEX_INFO_LEVELS, lpFileInformation: LPVOID)->BOOL;
    func CreateFileA(lpFileName: LPCSTR, dwDesiredAccess: DWORD, dwShareMode: DWORD, lpSecurityAttributes: *SECURITY_ATTRIBUTES, dwCreationDisposition: DWORD, dwFlagsAndAttributes: DWORD, hTemplateFile: HANDLE)->HANDLE;
    func ReadFile(hFile: HANDLE, lpBuffer: LPVOID, nNumberOfBytesToRead: DWORD, lpNumberOfBytesRead: LPDWORD, lpOverlapped: LPOVERLAPPED)->BOOL;
    func WriteFile(hFile: HANDLE, lpBuffer: LPCVOID, nNumberOfBytesToWrite: DWORD, lpNumberOfBytesWritten: LPDWORD, lpOverlapped: LPOVERLAPPED)->BOOL;
    func CloseHandle(handle: HANDLE);
    func SetFilePointer(hFile: HANDLE, lDistanceToMove: LONG, lpDistanceToMoveHigh: PLONG, dwMoveMethod: DWORD)->DWORD;
    func GetFileSize(hFile: HANDLE, lpFileSizeHigh: LPDWORD)->DWORD;
    func FileTimeToLocalFileTime(lpFileTime: const *FILETIME, lpLocalFileTime: *FILETIME)->BOOL;
    func FileTimeToSystemTime(lpFileTime: const *FILETIME, lpSystemTime: *SYSTEMTIME)->BOOL;
    func FindFirstFileA(lpFileName: LPCSTR, lpFindFileData: *WIN32_FIND_DATAA)->HANDLE;
    func FindNextFileA(hFindFile: HANDLE, lpFindFileData: *WIN32_FIND_DATAA)->BOOL;
    func FindClose(hFindFile: HANDLE)->BOOL;
    func GetCurrentDirectoryA(nBufferLength: DWORD, lpBuffer: LPCSTR)->DWORD;
    func SetCurrentDirectoryA(lpPathName: LPCSTR)->BOOL;
}

#foreignlib "user32"

alias HANDLE = 			const *void
alias HLOCAL = 			HANDLE
alias HWND = 			const *void
alias WORD = 			u16
alias DWORD =           u32
alias BOOL = 	        s32
alias BYTE = 			u8
alias LPBYTE = 			*BYTE
alias LONG = 			s32
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

const VK_LBUTTON        = 0x01
const VK_RBUTTON        = 0x02
const VK_CANCEL         = 0x03
const VK_MBUTTON        = 0x04
const VK_XBUTTON1       = 0x05
const VK_XBUTTON2       = 0x06
const VK_BACK           = 0x08
const VK_TAB            = 0x09
const VK_CLEAR          = 0x0C
const VK_RETURN         = 0x0D
const VK_SHIFT          = 0x10
const VK_CONTROL        = 0x11
const VK_MENU           = 0x12
const VK_PAUSE          = 0x13
const VK_CAPITAL        = 0x14
const VK_KANA           = 0x15
const VK_HANGEUL        = 0x15
const VK_HANGUL         = 0x15
const VK_JUNJA          = 0x17
const VK_FINAL          = 0x18
const VK_HANJA          = 0x19
const VK_KANJI          = 0x19
const VK_ESCAPE         = 0x1B
const VK_CONVERT        = 0x1C
const VK_NONCONVERT     = 0x1D
const VK_ACCEPT         = 0x1E
const VK_MODECHANGE     = 0x1F
const VK_SPACE          = 0x20
const VK_PRIOR          = 0x21
const VK_NEXT           = 0x22
const VK_END            = 0x23
const VK_HOME           = 0x24
const VK_LEFT           = 0x25
const VK_UP             = 0x26
const VK_RIGHT          = 0x27
const VK_DOWN           = 0x28
const VK_SELECT         = 0x29
const VK_PRINT          = 0x2A
const VK_EXECUTE        = 0x2B
const VK_SNAPSHOT       = 0x2C
const VK_INSERT         = 0x2D
const VK_DELETE         = 0x2E
const VK_HELP           = 0x2F
const VK_LWIN           = 0x5B
const VK_RWIN           = 0x5C
const VK_APPS           = 0x5D
const VK_SLEEP          = 0x5F
const VK_NUMPAD0        = 0x60
const VK_NUMPAD1        = 0x61
const VK_NUMPAD2        = 0x62
const VK_NUMPAD3        = 0x63
const VK_NUMPAD4        = 0x64
const VK_NUMPAD5        = 0x65
const VK_NUMPAD6        = 0x66
const VK_NUMPAD7        = 0x67
const VK_NUMPAD8        = 0x68
const VK_NUMPAD9        = 0x69
const VK_MULTIPLY       = 0x6A
const VK_ADD            = 0x6B
const VK_SEPARATOR      = 0x6C
const VK_SUBTRACT       = 0x6D
const VK_DECIMAL        = 0x6E
const VK_DIVIDE         = 0x6F
const VK_F1             = 0x70
const VK_F2             = 0x71
const VK_F3             = 0x72
const VK_F4             = 0x73
const VK_F5             = 0x74
const VK_F6             = 0x75
const VK_F7             = 0x76
const VK_F8             = 0x77
const VK_F9             = 0x78
const VK_F10            = 0x79
const VK_F11            = 0x7A
const VK_F12            = 0x7B
const VK_F13            = 0x7C
const VK_F14            = 0x7D
const VK_F15            = 0x7E
const VK_F16            = 0x7F
const VK_F17            = 0x80
const VK_F18            = 0x81
const VK_F19            = 0x82
const VK_F20            = 0x83
const VK_F21            = 0x84
const VK_F22            = 0x85
const VK_F23            = 0x86
const VK_F24            = 0x87
const VK_NAVIGATION_VIEW     = 0x88
const VK_NAVIGATION_MENU     = 0x89
const VK_NAVIGATION_UP       = 0x8A
const VK_NAVIGATION_DOWN     = 0x8B
const VK_NAVIGATION_LEFT     = 0x8C
const VK_NAVIGATION_RIGHT    = 0x8D
const VK_NAVIGATION_ACCEPT   = 0x8E
const VK_NAVIGATION_CANCEL   = 0x8F
const VK_NUMLOCK        = 0x90
const VK_SCROLL         = 0x91
const VK_OEM_NEC_EQUAL  = 0x92
const VK_OEM_FJ_JISHO   = 0x92
const VK_OEM_FJ_MASSHOU = 0x93
const VK_OEM_FJ_TOUROKU = 0x94
const VK_OEM_FJ_LOYA    = 0x95
const VK_OEM_FJ_ROYA    = 0x96
const VK_LSHIFT         = 0xA0
const VK_RSHIFT         = 0xA1
const VK_LCONTROL       = 0xA2
const VK_RCONTROL       = 0xA3
const VK_LMENU          = 0xA4
const VK_RMENU          = 0xA5
const VK_BROWSER_BACK        = 0xA6
const VK_BROWSER_FORWARD     = 0xA7
const VK_BROWSER_REFRESH     = 0xA8
const VK_BROWSER_STOP        = 0xA9
const VK_BROWSER_SEARCH      = 0xAA
const VK_BROWSER_FAVORITES   = 0xAB
const VK_BROWSER_HOME        = 0xAC
const VK_VOLUME_MUTE         = 0xAD
const VK_VOLUME_DOWN         = 0xAE
const VK_VOLUME_UP           = 0xAF
const VK_MEDIA_NEXT_TRACK    = 0xB0
const VK_MEDIA_PREV_TRACK    = 0xB1
const VK_MEDIA_STOP          = 0xB2
const VK_MEDIA_PLAY_PAUSE    = 0xB3
const VK_LAUNCH_MAIL         = 0xB4
const VK_LAUNCH_MEDIA_SELECT = 0xB5
const VK_LAUNCH_APP1         = 0xB6
const VK_LAUNCH_APP2         = 0xB7
const VK_OEM_1				 = 0xBA
const VK_OEM_PLUS			 = 0xBB
const VK_OEM_COMMA			 = 0xBC
const VK_OEM_MINUS			 = 0xBD
const VK_OEM_PERIOD			 = 0xBE
const VK_OEM_2				 = 0xBF
const VK_OEM_3				 = 0xC0
const VK_GAMEPAD_A                         = 0xC3
const VK_GAMEPAD_B                         = 0xC4
const VK_GAMEPAD_X                         = 0xC5
const VK_GAMEPAD_Y                         = 0xC6
const VK_GAMEPAD_RIGHT_SHOULDER            = 0xC7
const VK_GAMEPAD_LEFT_SHOULDER             = 0xC8
const VK_GAMEPAD_LEFT_TRIGGER              = 0xC9
const VK_GAMEPAD_RIGHT_TRIGGER             = 0xCA
const VK_GAMEPAD_DPAD_UP                   = 0xCB
const VK_GAMEPAD_DPAD_DOWN                 = 0xCC
const VK_GAMEPAD_DPAD_LEFT                 = 0xCD
const VK_GAMEPAD_DPAD_RIGHT                = 0xCE
const VK_GAMEPAD_MENU                      = 0xCF
const VK_GAMEPAD_VIEW                      = 0xD0
const VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON    = 0xD1
const VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON   = 0xD2
const VK_GAMEPAD_LEFT_THUMBSTICK_UP        = 0xD3
const VK_GAMEPAD_LEFT_THUMBSTICK_DOWN      = 0xD4
const VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT     = 0xD5
const VK_GAMEPAD_LEFT_THUMBSTICK_LEFT      = 0xD6
const VK_GAMEPAD_RIGHT_THUMBSTICK_UP       = 0xD7
const VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN     = 0xD8
const VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT    = 0xD9
const VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT     = 0xDA
const VK_OEM_4          = 0xDB
const VK_OEM_5          = 0xDC
const VK_OEM_6          = 0xDD
const VK_OEM_7          = 0xDE
const VK_OEM_8          = 0xDF
const VK_OEM_AX         = 0xE1
const VK_OEM_102        = 0xE2
const VK_ICO_HELP       = 0xE3
const VK_ICO_00         = 0xE4
const VK_PROCESSKEY     = 0xE5
const VK_ICO_CLEAR      = 0xE6
const VK_PACKET         = 0xE7
const VK_OEM_RESET      = 0xE9
const VK_OEM_JUMP       = 0xEA
const VK_OEM_PA1        = 0xEB
const VK_OEM_PA2        = 0xEC
const VK_OEM_PA3        = 0xED
const VK_OEM_WSCTRL     = 0xEE
const VK_OEM_CUSEL      = 0xEF
const VK_OEM_ATTN       = 0xF0
const VK_OEM_FINISH     = 0xF1
const VK_OEM_COPY       = 0xF2
const VK_OEM_AUTO       = 0xF3
const VK_OEM_ENLW       = 0xF4
const VK_OEM_BACKTAB    = 0xF5
const VK_ATTN           = 0xF6
const VK_CRSEL          = 0xF7
const VK_EXSEL          = 0xF8
const VK_EREOF          = 0xF9
const VK_PLAY           = 0xFA
const VK_ZOOM           = 0xFB
const VK_NONAME         = 0xFC
const VK_PA1            = 0xFD
const VK_OEM_CLEAR      = 0xFE

const MB_OK               		= 0x00000000
const MB_OKCANCEL               = 0x00000001
const MB_ABORTRETRYIGNORE       = 0x00000002
const MB_YESNOCANCEL            = 0x00000003
const MB_YESNO                  = 0x00000004
const MB_RETRYCANCEL            = 0x00000005
const MB_CANCELTRYCONTINUE      = 0x00000006
const MB_ICONHAND				= 0x00000010
const MB_ICONQUESTION           = 0x00000020
const MB_ICONEXCLAMATION        = 0x00000030
const MB_ICONASTERISK           = 0x00000040
const MB_USERICON               = 0x00000080
const MB_ICONWARNING            = MB_ICONEXCLAMATION
const MB_ICONERROR              = MB_ICONHAND
const MB_ICONINFORMATION        = MB_ICONASTERISK
const MB_ICONSTOP               = MB_ICONHAND
const MB_DEFBUTTON1             = 0x00000000
const MB_DEFBUTTON2             = 0x00000100
const MB_DEFBUTTON3             = 0x00000200
const MB_DEFBUTTON4             = 0x00000300
const MB_APPLMODAL              = 0x00000000
const MB_SYSTEMMODAL            = 0x00001000
const MB_TASKMODAL              = 0x00002000
const MB_HELP                   = 0x00004000
const MB_NOFOCUS                = 0x00008000
const MB_SETFOREGROUND          = 0x00010000
const MB_DEFAULT_DESKTOP_ONLY   = 0x00020000
const MB_TOPMOST                = 0x00040000
const MB_RIGHT                  = 0x00080000
const MB_RTLREADING             = 0x00100000

const IDOK				= 1
const IDCANCEL          = 2
const IDABORT           = 3
const IDRETRY           = 4
const IDIGNORE        	= 5
const IDYES           	= 6
const IDNO            	= 7
const IDCLOSE         	= 8
const IDHELP          	= 9
const IDTRYAGAIN      	= 10
const IDCONTINUE      	= 11
const IDTIMEOUT 		= 32000

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

    func GetCurrentProcessId()->DWORD;
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
    func CreateThread(lpThreadAttributes: *SECURITY_ATTRIBUTES, dwStackSize: SIZE_T, lpStartAddress: *void, lpParameter: *void, dwCreationFlags: DWORD, lpThreadId : *DWORD)->HANDLE;
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

#[swag.foreign("user32")]
{
    func GetAsyncKeyState(vKey: s32)->SHORT;
    func GetKeyboardState(lpKeystate: *u8)->BOOL;
    func GetCursorPos(lpPoint: *POINT);
    func SetCursorPos(x, y: s32);
    func ShowCursor(show: BOOL);
    func MessageBoxA(hWnd: HWND, lpText: LPCSTR, lpCaption: LPCSTR, uType: u32)->s32;
    func MessageBoxW(hWnd: HWND, lpText: LPCWSTR, lpCaption: LPCWSTR, uType: u32)->s32;
}
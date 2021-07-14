#global export
#global foreignlib "kernel32"
using Swag

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
alias WCHAR =           u16
alias LPSTR =           *CHAR
alias LPWSTR =          *WCHAR
alias SIZE_T =          uint
alias va_list =         *u8

const STD_INPUT_HANDLE 	= bitcast(DWORD) -10
const STD_OUTPUT_HANDLE = bitcast(DWORD) -11

const ERROR_FILE_NOT_FOUND          = 2
const ERROR_NO_MORE_FILES           = 18
const ERROR_IO_PENDING              = 997
const ERROR_DEVICE_NOT_CONNECTED    = 1167

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
const INVALID_FILE_SIZE         = bitcast(DWORD) -1
const INVALID_SET_FILE_POINTER  = bitcast(DWORD) -1
const INVALID_FILE_ATTRIBUTES   = bitcast(DWORD) -1

const CREATE_NEW          = 1'u32
const CREATE_ALWAYS       = 2'u32
const OPEN_EXISTING       = 3'u32
const OPEN_ALWAYS         = 4'u32
const TRUNCATE_EXISTING   = 5'u32

const INVALID_HANDLE_VALUE = cast(HANDLE) bitcast(uint) -1'int
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

const WAIT_OBJECT_0     = 0x00000000
const WAIT_ABANDONED    = 0x00000080
const WAIT_TIMEOUT      = 0x00000102
const WAIT_FAILED       = 0xFFFFFFFF

const THREAD_PRIORITY_LOWEST        = -2
const THREAD_PRIORITY_BELOW_NORMAL  = -1
const THREAD_PRIORITY_NORMAL        = 0
const THREAD_PRIORITY_ABOVE_NORMAL  = 1
const THREAD_PRIORITY_HIGHEST       = 2

func LOWORD(l: u64) => cast(s32) (l & 0xffff)
func HIWORD(l: u64) => cast(s32) ((l >>,safe 16) & 0xffff)

alias SRWLOCK = *void
const SRWLOCK_INIT = cast(SRWLOCK) null

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

struct WIN32_FIND_DATAW
{
    dwFileAttributes:   DWORD
    ftCreationTime:     FILETIME
    ftLastAccessTime:   FILETIME
    ftLastWriteTime:    FILETIME
    nFileSizeHigh:      DWORD
    nFileSizeLow:       DWORD
    dwReserved0:        DWORD
    dwReserved1:        DWORD
    cFileName:          [MAX_PATH] WCHAR
    cAlternateFileName: [14] WCHAR
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

struct STARTUPINFOW
{
    cb:                 DWORD
    lpReserved:         LPWSTR
    lpDesktop:          LPWSTR
    lpTitle:            LPWSTR
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

#[foreign("kernel32"), Discardable]
{
    func SwitchToThread()->BOOL;
}

#[foreign("kernel32")]
{
    func GetLastError()->DWORD;
    func FormatMessageA(dwFlags : DWORD, lpSource : LPCVOID, dwMessageId : DWORD, dwLanguageId : DWORD, lpBuffer: LPCSTR, nSize: DWORD, Arguments: *va_list)->DWORD;
    func IsDebuggerPresent()->BOOL;
    func DebugBreak();
    func OutputDebugStringA(lpOutputString: LPCSTR);
    func OutputDebugStringW(lpOutputString: LPCWSTR);
    func DebugActiveProcess(procId: DWORD);

    func GetProcAddress(hModule: HMODULE, lpProcName: LPCSTR)->const *void;
    func GetCurrentProcessId()->DWORD;
    func GetModuleHandleA(lpModuleName: LPCSTR)->HMODULE;
    func GetModuleHandleW(lpModuleName: LPCWSTR)->HMODULE;
    func GetStdHandle(nStdHandle: DWORD)->HANDLE;
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
    func ExitProcess(uExitCode: UINT);
    func GetCurrentThreadId()->DWORD;
    func GetCurrentThread()->HANDLE;

    func AcquireSRWLockExclusive(SRWLock: *SRWLOCK);
    func AcquireSRWLockShared(SRWLock: *SRWLOCK);
    func InitializeSRWLock(SRWLock: *SRWLOCK);
    func ReleaseSRWLockExclusive(SRWLock: *SRWLOCK);
    func ReleaseSRWLockShared(SRWLock: *SRWLOCK);
    func TryAcquireSRWLockExclusive(SRWLock: *SRWLOCK)->BOOL;
    func TryAcquireSRWLockShared(SRWLock: *SRWLOCK)->BOOL;

    func CreateWaitableTimerA(lpTimerAttributes: *SECURITY_ATTRIBUTES, bManualReset: BOOL, lpTimerName: LPCSTR)->HANDLE;
    func CreateWaitableTimerW(lpTimerAttributes: *SECURITY_ATTRIBUTES, bManualReset: BOOL, lpTimerName: LPCWSTR)->HANDLE;
    func CancelWaitableTimer(hTimer: HANDLE)->BOOL;
    func SetWaitableTimer(hTimer: HANDLE, lpDueTime: const *LARGE_INTEGER, lPeriod: LONG, pfnCompletionRoutine: *void, lpArgToCompletionRoutine: LPVOID, fResume: BOOL)->BOOL;

    func ReadFile(hFile: HANDLE, lpBuffer: LPVOID, nNumberOfBytesToRead: DWORD, lpNumberOfBytesRead: LPDWORD, lpOverlapped: LPOVERLAPPED)->BOOL;
    func CloseHandle(handle: HANDLE);
    func FileTimeToLocalFileTime(lpFileTime: const *FILETIME, lpLocalFileTime: *FILETIME)->BOOL;
}

namespace Win32Priv
{
    #[foreign("kernel32")]
    {
        func SetConsoleTextAttribute(hConsoleOutput: HANDLE, wAttributes: WORD)->BOOL;
        func GetConsoleScreenBufferInfo(hConsoleOutput: HANDLE, lpConsoleScreenBufferInfo: *CONSOLE_SCREEN_BUFFER_INFO)->BOOL;

        func CreateThread(lpThreadAttributes: *SECURITY_ATTRIBUTES, dwStackSize: SIZE_T, lpStartAddress: const *void, lpParameter: *void, dwCreationFlags: DWORD, lpThreadId : *DWORD)->HANDLE;
        func ResumeThread(hThread: HANDLE)->DWORD;
        func SetThreadPriority(hThread: HANDLE, nPriority: s32)->BOOL;
        func WaitForSingleObject(hHandle: HANDLE, dwMilliseconds: DWORD)->DWORD;
        func WaitForMultipleObjects(nCount: DWORD, lpHandles: const *HANDLE, bWaitAll: BOOL, dwMilliseconds: DWORD)->DWORD;
        func ReleaseMutex(hMutex: HANDLE)->BOOL;
        func ResetEvent(hEvent: HANDLE)->BOOL;
        func SetEvent(hEvent: HANDLE)->BOOL;

        func CreateProcessA(lpApplicationName: LPCSTR, lpCommandLine: LPCSTR, lpProcessAttributes: *SECURITY_ATTRIBUTES, lpThreadAttributes: *SECURITY_ATTRIBUTES, bInheritHandles: BOOL, dwCreationFlags: DWORD, lpEnvironment: LPVOID, lpCurrentDirectory: LPCSTR, lpStartupInfo: *STARTUPINFOA, lpProcessInformation: *PROCESS_INFORMATION)->BOOL;
        func CreateProcessW(lpApplicationName: LPCWSTR, lpCommandLine: LPCWSTR, lpProcessAttributes: *SECURITY_ATTRIBUTES, lpThreadAttributes: *SECURITY_ATTRIBUTES, bInheritHandles: BOOL, dwCreationFlags: DWORD, lpEnvironment: LPVOID, lpCurrentDirectory: LPCWSTR, lpStartupInfo: *STARTUPINFOW, lpProcessInformation: *PROCESS_INFORMATION)->BOOL;
        func CreateEventA(lpEventAttributes: *SECURITY_ATTRIBUTES, bManualReset: BOOL, bInitialState: BOOL, lpName: LPCSTR)->HANDLE;
        func CreateEventW(lpEventAttributes: *SECURITY_ATTRIBUTES, bManualReset: BOOL, bInitialState: BOOL, lpName: LPCWSTR)->HANDLE;
        func CreateMutexA(lpMutexAttributes: *SECURITY_ATTRIBUTES, bInitialOwner: BOOL, lpName: LPCSTR)->HANDLE;
        func CreateMutexW(lpMutexAttributes: *SECURITY_ATTRIBUTES, bInitialOwner: BOOL, lpName: LPCWSTR)->HANDLE;
        func CreateTimerQueueTimer(phNewTimer: *HANDLE, timerQueue: HANDLE, callback: const *void, parameter: *void, dueTime: DWORD, period: DWORD, flags: ULONG)->BOOL;
        func DeleteTimerQueueTimer(timerQueue: HANDLE, timer: HANDLE, completionEvent: HANDLE)->BOOL;

        func WriteFile(hFile: HANDLE, lpBuffer: LPCVOID, nNumberOfBytesToWrite: DWORD, lpNumberOfBytesWritten: LPDWORD, lpOverlapped: LPOVERLAPPED)->BOOL;
        func GetFileAttributesA(lpFileName: LPCSTR)->DWORD;
        func GetFileAttributesW(lpFileName: LPCWSTR)->DWORD;
        func GetFileAttributesExA(lpFileName: LPCSTR, fInfoLevelId: GET_FILEEX_INFO_LEVELS, lpFileInformation: LPVOID)->BOOL;
        func GetFileAttributesExW(lpFileName: LPCWSTR, fInfoLevelId: GET_FILEEX_INFO_LEVELS, lpFileInformation: LPVOID)->BOOL;
        func CreateFileA(lpFileName: LPCSTR, dwDesiredAccess: DWORD, dwShareMode: DWORD, lpSecurityAttributes: *SECURITY_ATTRIBUTES, dwCreationDisposition: DWORD, dwFlagsAndAttributes: DWORD, hTemplateFile: HANDLE)->HANDLE;
        func CreateFileW(lpFileName: LPCWSTR, dwDesiredAccess: DWORD, dwShareMode: DWORD, lpSecurityAttributes: *SECURITY_ATTRIBUTES, dwCreationDisposition: DWORD, dwFlagsAndAttributes: DWORD, hTemplateFile: HANDLE)->HANDLE;
        func SetFilePointer(hFile: HANDLE, lDistanceToMove: LONG, lpDistanceToMoveHigh: PLONG, dwMoveMethod: DWORD)->DWORD;
        func GetFileSize(hFile: HANDLE, lpFileSizeHigh: LPDWORD)->DWORD;
        func FileTimeToSystemTime(lpFileTime: const *FILETIME, lpSystemTime: *SYSTEMTIME)->BOOL;
        func FindFirstFileA(lpFileName: LPCSTR, lpFindFileData: *WIN32_FIND_DATAA)->HANDLE;
        func FindFirstFileW(lpFileName: LPCWSTR, lpFindFileData: *WIN32_FIND_DATAW)->HANDLE;
        func FindNextFileA(hFindFile: HANDLE, lpFindFileData: *WIN32_FIND_DATAA)->BOOL;
        func FindNextFileW(hFindFile: HANDLE, lpFindFileData: *WIN32_FIND_DATAW)->BOOL;
        func FindClose(hFindFile: HANDLE)->BOOL;

        func GetCurrentDirectoryA(nBufferLength: DWORD, lpBuffer: LPCSTR)->DWORD;
        func GetCurrentDirectoryW(nBufferLength: DWORD, lpBuffer: LPCWSTR)->DWORD;
        func SetCurrentDirectoryA(lpPathName: LPCSTR)->BOOL;
        func SetCurrentDirectoryW(lpPathName: LPCWSTR)->BOOL;
        func GetFullPathNameA(lpFileName: LPCSTR, nBufferLength: DWORD, lpBuffer: LPSTR, lpFilePart: *LPSTR)->DWORD;
        func GetFullPathNameW(lpFileName: LPCTSTR, nBufferLength: DWORD, lpBuffer: LPWSTR, lpFilePart: *LPWSTR)->DWORD;
    }
}

#[inline]
{
    func SetConsoleTextAttribute(hConsoleOutput: HANDLE, wAttributes: WORD) throw
    {
        if !Win32Priv.SetConsoleTextAttribute(hConsoleOutput, wAttributes)
            try setError(GetLastError())
    }

    func GetConsoleScreenBufferInfo(hConsoleOutput: HANDLE, lpConsoleScreenBufferInfo: *CONSOLE_SCREEN_BUFFER_INFO) throw
    {
        if !Win32Priv.GetConsoleScreenBufferInfo(hConsoleOutput, lpConsoleScreenBufferInfo)
            try setError(GetLastError())
    }

    func CreateThread(lpThreadAttributes: *SECURITY_ATTRIBUTES, dwStackSize: SIZE_T, lpStartAddress: const *void, lpParameter: *void, dwCreationFlags: DWORD, lpThreadId : *DWORD)->HANDLE throw
    {
        h := Win32Priv.CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId)
        if h == null
            try Win32.setError(GetLastError())
        return h
    }

    #[Discardable]
    func ResumeThread(hThread: HANDLE)->DWORD throw
    {
        res := Win32Priv.ResumeThread(hThread)
        if res == bitcast(DWORD) -1
            try setError(GetLastError())
        return res
    }

    func SetThreadPriority(hThread: HANDLE, nPriority: s32) throw
    {
        if !Win32Priv.SetThreadPriority(hThread, nPriority)
            try setError(GetLastError())
    }

    #[Discardable]
    func WaitForSingleObject(hHandle: HANDLE, dwMilliseconds: DWORD)->DWORD throw
    {
        res := Win32Priv.WaitForSingleObject(hHandle, dwMilliseconds)
        if res == WAIT_FAILED
            try setError(GetLastError())
        return res
    }

    #[Discardable]
    func WaitForMultipleObjects(nCount: DWORD, lpHandles: const *HANDLE, bWaitAll: BOOL, dwMilliseconds: DWORD)->DWORD throw
    {
        res := Win32Priv.WaitForMultipleObjects(nCount, lpHandles, bWaitAll, dwMilliseconds)
        if res == WAIT_FAILED
            try setError(GetLastError())
        return res
    }

    func ReleaseMutex(hMutex: HANDLE) throw
    {
        if !Win32Priv.ReleaseMutex(hMutex)
            try setError(GetLastError())
    }

    func ResetEvent(hEvent: HANDLE) throw
    {
        if !Win32Priv.ResetEvent(hEvent)
            try setError(GetLastError())
    }

    func SetEvent(hEvent: HANDLE) throw
    {
        if !Win32Priv.SetEvent(hEvent)
            try setError(GetLastError())
    }

    func CreateProcessA(lpApplicationName: LPCSTR, lpCommandLine: LPCSTR, lpProcessAttributes: *SECURITY_ATTRIBUTES, lpThreadAttributes: *SECURITY_ATTRIBUTES, bInheritHandles: BOOL, dwCreationFlags: DWORD, lpEnvironment: LPVOID, lpCurrentDirectory: LPCSTR, lpStartupInfo: *STARTUPINFOA, lpProcessInformation: *PROCESS_INFORMATION) throw
    {
        if !Win32Priv.CreateProcessA(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation)
            try setError(GetLastError())
    }

    func CreateProcessW(lpApplicationName: LPCWSTR, lpCommandLine: LPCWSTR, lpProcessAttributes: *SECURITY_ATTRIBUTES, lpThreadAttributes: *SECURITY_ATTRIBUTES, bInheritHandles: BOOL, dwCreationFlags: DWORD, lpEnvironment: LPVOID, lpCurrentDirectory: LPCWSTR, lpStartupInfo: *STARTUPINFOW, lpProcessInformation: *PROCESS_INFORMATION) throw
    {
        if !Win32Priv.CreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation)
            try setError(GetLastError())
    }

    func CreateEventA(lpEventAttributes: *SECURITY_ATTRIBUTES, bManualReset: BOOL, bInitialState: BOOL, lpName: LPCSTR)->HANDLE throw
    {
        h := Win32Priv.CreateEventA(lpEventAttributes, bManualReset, bInitialState, lpName)
        if !h
            try setError(GetLastError())
        return h
    }

    func CreateEventW(lpEventAttributes: *SECURITY_ATTRIBUTES, bManualReset: BOOL, bInitialState: BOOL, lpName: LPCWSTR)->HANDLE throw
    {
        h := Win32Priv.CreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName)
        if !h
            try setError(GetLastError())
        return h
    }

    func CreateMutexA(lpMutexAttributes: *SECURITY_ATTRIBUTES, bInitialOwner: BOOL, lpName: LPCSTR)->HANDLE throw
    {
        h := Win32Priv.CreateMutexA(lpMutexAttributes, bInitialOwner, lpName)
        if !h
            try setError(GetLastError())
        return h
    }

    func CreateMutexW(lpMutexAttributes: *SECURITY_ATTRIBUTES, bInitialOwner: BOOL, lpName: LPCWSTR)->HANDLE throw
    {
        h := Win32Priv.CreateMutexW(lpMutexAttributes, bInitialOwner, lpName)
        if !h
            try setError(GetLastError())
        return h
    }

    func CreateTimerQueueTimer(phNewTimer: *HANDLE, timerQueue: HANDLE, callback: const *void, parameter: *void, dueTime: DWORD, period: DWORD, flags: ULONG) throw
    {
        if !Win32Priv.CreateTimerQueueTimer(phNewTimer, timerQueue, callback, parameter, dueTime, period, flags)
            try setError(GetLastError())
    }

    func DeleteTimerQueueTimer(timerQueue: HANDLE, timer: HANDLE, completionEvent: HANDLE)
    {
        loop
        {
            res := Win32Priv.DeleteTimerQueueTimer(timerQueue, timer, completionEvent)
            if res or GetLastError() == ERROR_IO_PENDING return
        }
    }

    #[Discardable]
    func WriteFile(hFile: HANDLE, lpBuffer: LPCVOID, nNumberOfBytesToWrite: DWORD, lpNumberOfBytesWritten: LPDWORD, lpOverlapped: LPOVERLAPPED)->BOOL throw
    {
        res := Win32Priv.WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped)
        if !res and GetLastError() != ERROR_IO_PENDING
            try setError(GetLastError())
        return res
    }

    func FileTimeToSystemTime(lpFileTime: const *FILETIME, lpSystemTime: *SYSTEMTIME) throw
    {
        if !Win32Priv.FileTimeToSystemTime(lpFileTime, lpSystemTime)
            try setError(GetLastError())
    }

    func GetFileAttributesA(lpFileName: LPCSTR)->DWORD throw
    {
        attributes := Win32Priv.GetFileAttributesA(lpFileName)
        if attributes == INVALID_FILE_ATTRIBUTES
            try setError(GetLastError())
        return attributes
    }

    func GetFileAttributesW(lpFileName: LPCWSTR)->DWORD throw
    {
        attributes := Win32Priv.GetFileAttributesW(lpFileName)
        if attributes == INVALID_FILE_ATTRIBUTES
            try setError(GetLastError())
        return attributes
    }

    func GetFileAttributesExA(lpFileName: LPCSTR, fInfoLevelId: GET_FILEEX_INFO_LEVELS, lpFileInformation: LPVOID) throw
    {
        if !Win32Priv.GetFileAttributesExA(lpFileName, fInfoLevelId, lpFileInformation)
            try setError(GetLastError())
    }

    func GetFileAttributesExW(lpFileName: LPCWSTR, fInfoLevelId: GET_FILEEX_INFO_LEVELS, lpFileInformation: LPVOID) throw
    {
        if !Win32Priv.GetFileAttributesExW(lpFileName, fInfoLevelId, lpFileInformation)
            try setError(GetLastError())
    }

    func GetCurrentDirectoryA(nBufferLength: DWORD, lpBuffer: LPCSTR)->DWORD throw
    {
        length := Win32Priv.GetCurrentDirectoryA(nBufferLength, lpBuffer)
        if !length
            try setError(GetLastError())
        return length
    }

    func GetCurrentDirectoryW(nBufferLength: DWORD, lpBuffer: LPCWSTR)->DWORD throw
    {
        length := Win32Priv.GetCurrentDirectoryW(nBufferLength, lpBuffer)
        if !length
            try setError(GetLastError())
        return length
    }

    func SetCurrentDirectoryA(lpPathName: LPCSTR) throw
    {
        if !Win32Priv.SetCurrentDirectoryA(lpPathName)
            try setError(GetLastError())
    }

    func SetCurrentDirectoryW(lpPathName: LPCWSTR) throw
    {
        if !Win32Priv.SetCurrentDirectoryW(lpPathName)
            try setError(GetLastError())
    }

    func CreateFileA(lpFileName: LPCSTR, dwDesiredAccess: DWORD, dwShareMode: DWORD, lpSecurityAttributes: *SECURITY_ATTRIBUTES, dwCreationDisposition: DWORD, dwFlagsAndAttributes: DWORD, hTemplateFile: HANDLE)->HANDLE throw
    {
        h := Win32Priv.CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile)
        if h == INVALID_HANDLE_VALUE
            try setError(GetLastError())
        return h
    }

    func CreateFileW(lpFileName: LPCWSTR, dwDesiredAccess: DWORD, dwShareMode: DWORD, lpSecurityAttributes: *SECURITY_ATTRIBUTES, dwCreationDisposition: DWORD, dwFlagsAndAttributes: DWORD, hTemplateFile: HANDLE)->HANDLE throw
    {
        h := Win32Priv.CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile)
        if h == INVALID_HANDLE_VALUE
            try setError(GetLastError())
        return h
    }

    func FindFirstFileA(lpFileName: LPCSTR, lpFindFileData: *WIN32_FIND_DATAA)->HANDLE throw
    {
        res := Win32Priv.FindFirstFileA(lpFileName, lpFindFileData)
        if res == INVALID_HANDLE_VALUE and GetLastError() != ERROR_FILE_NOT_FOUND
            try setError(GetLastError())
        return res
    }

    func FindFirstFileW(lpFileName: LPCWSTR, lpFindFileData: *WIN32_FIND_DATAW)->HANDLE throw
    {
        res := Win32Priv.FindFirstFileW(lpFileName, lpFindFileData)
        if res == INVALID_HANDLE_VALUE and GetLastError() != ERROR_FILE_NOT_FOUND
            try setError(GetLastError())
        return res
    }

    func FindNextFileA(hFindFile: HANDLE, lpFindFileData: *WIN32_FIND_DATAA)->BOOL throw
    {
        res := Win32Priv.FindNextFileA(hFindFile, lpFindFileData)
        if !res and GetLastError() != ERROR_NO_MORE_FILES
            try setError(GetLastError())
        return res
    }

    func FindNextFileW(hFindFile: HANDLE, lpFindFileData: *WIN32_FIND_DATAW)->BOOL throw
    {
        res := Win32Priv.FindNextFileW(hFindFile, lpFindFileData)
        if !res and GetLastError() != ERROR_NO_MORE_FILES
            try setError(GetLastError())
        return res
    }

    func FindClose(hFindFile: HANDLE) throw
    {
        if !Win32Priv.FindClose(hFindFile)
            try setError(GetLastError())
    }

    #[Discardable]
    func SetFilePointer(hFile: HANDLE, lDistanceToMove: LONG, lpDistanceToMoveHigh: PLONG, dwMoveMethod: DWORD)->DWORD throw
    {
        low := Win32Priv.SetFilePointer(hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod)
        if low == INVALID_SET_FILE_POINTER
            try setError(GetLastError())
        return low
    }

    func GetFileSize(hFile: HANDLE, lpFileSizeHigh: LPDWORD)->DWORD throw
    {
        low := Win32Priv.GetFileSize(hFile, lpFileSizeHigh)
        if low == INVALID_FILE_SIZE
            try setError(GetLastError())
        return low
    }

    #[Discardable]
    func GetFullPathNameA(lpFileName: LPCSTR, nBufferLength: DWORD, lpBuffer: LPSTR, lpFilePart: *LPSTR)->DWORD throw
    {
        res := Win32Priv.GetFullPathNameA(lpFileName, nBufferLength, lpBuffer, lpFilePart)
        if !res
            try setError(GetLastError())
        return res
    }

    #[Discardable]
    func GetFullPathNameW(lpFileName: LPCTSTR, nBufferLength: DWORD, lpBuffer: LPWSTR, lpFilePart: *LPWSTR)->DWORD throw
    {
        res := Win32Priv.GetFullPathNameW(lpFileName, nBufferLength, lpBuffer, lpFilePart)
        if !res
            try setError(GetLastError())
        return res
    }
}
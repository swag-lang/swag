#global export
#foreignlib("kernel32")
using Swag

alias ATOM          = const [*] void
alias HANDLE        = const [*] void
alias HGLOBAL       = HANDLE
alias HLOCAL        = HANDLE
alias HINSTANCE     = HANDLE
alias HMODULE       = HANDLE
alias LRESULT       = s64
alias WORD          = u16
alias DWORD         = u32
alias BOOL          = s32
alias BYTE          = u8
alias LPBYTE        = *BYTE
alias LONG          = s32
alias ULONG         = u32
alias LPVOID        = [*] void
alias LPCVOID       = const [*] void
alias LPCSTR        = const [*] u8
alias PCSTR         = const [*] u8
alias LPCWSTR       = const [*] u16
alias PCWSTR        = const [*] u16
alias LPCTSTR       = const [*] u16
alias LPDWORD       = *DWORD
alias LPOVERLAPPED  = *DWORD
alias DWORD_PTR     = u64
alias UINT          = u32
alias INT           = s32
alias SHORT         = s16
alias USHORT        = u16
alias LARGE_INTEGER = s64
alias DWORD64       = u64
alias ULONG64       = u64
alias PLONG         = *LONG
alias LONG_PTR      = *LONG
alias CHAR          = u8
alias WCHAR         = u16
alias LPSTR         = [*] CHAR
alias LPWSTR        = [*] WCHAR
alias LPCH          = [*] CHAR
alias LPWCH         = [*] WCHAR
alias SIZE_T        = u64
alias va_list       = [*] u8

alias LPTHREAD_START_ROUTINE = func(*void)->DWORD
alias WAITORTIMERCALLBACK    = func(*void, BOOL)

const STD_INPUT_HANDLE      = cast #bit (DWORD) -10
const STD_OUTPUT_HANDLE     = cast #bit (DWORD) -11
const ATTACH_PARENT_PROCESS = cast #bit (DWORD) -1

const FALSE = 0
const TRUE  = 1

const FOREGROUND_BLUE      = 0x0001
const FOREGROUND_GREEN     = 0x0002
const FOREGROUND_RED       = 0x0004
const FOREGROUND_INTENSITY = 0x0008
const FOREGROUND_MASK      = 0x000F
const BACKGROUND_BLUE      = 0x0010
const BACKGROUND_GREEN     = 0x0020
const BACKGROUND_RED       = 0x0040
const BACKGROUND_INTENSITY = 0x0080
const BACKGROUND_MASK      = 0x00F0

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

const GENERIC_READ          = 0x80000000
const GENERIC_WRITE         = 0x40000000
const GENERIC_EXECUTE       = 0x20000000
const GENERIC_ALL           = 0x10000000
const FILE_READ_ATTRIBUTES  = 0x0080
const FILE_WRITE_ATTRIBUTES = 0x0100

const FILE_BEGIN               = 0'u32
const FILE_CURRENT             = 1'u32
const FILE_END                 = 2'u32
const INVALID_FILE_SIZE        = cast #bit (DWORD) -1
const INVALID_SET_FILE_POINTER = cast #bit (DWORD) -1
const INVALID_FILE_ATTRIBUTES  = cast #bit (DWORD) -1

const CREATE_NEW        = 1'u32
const CREATE_ALWAYS     = 2'u32
const OPEN_EXISTING     = 3'u32
const OPEN_ALWAYS       = 4'u32
const TRUNCATE_EXISTING = 5'u32

const INVALID_HANDLE_VALUE    = cast(HANDLE) cast #bit (u64) -1's64
const CONSOLE_TEXTMODE_BUFFER = 1'u32
const MAX_PATH                = 260
const INFINITE                = 0xFFFFFFFF
const CREATE_SUSPENDED        = 0x00000004
const MAXIMUM_WAIT_OBJECTS    = 64'u32

const FORMAT_MESSAGE_ALLOCATE_BUFFER = 0x00000100
const FORMAT_MESSAGE_IGNORE_INSERTS  = 0x00000200
const FORMAT_MESSAGE_FROM_STRING     = 0x00000400
const FORMAT_MESSAGE_FROM_HMODULE    = 0x00000800
const FORMAT_MESSAGE_FROM_SYSTEM     = 0x00001000
const FORMAT_MESSAGE_ARGUMENT_ARRAY  = 0x00002000
const FORMAT_MESSAGE_MAX_WIDTH_MASK  = 0x000000FF

const LANG_ENGLISH    = 0x09
const SUBLANG_DEFAULT = 0x01
func MAKELANGID(p, s: WORD) => (s << 10) | p

const WAIT_OBJECT_0  = 0x00000000
const WAIT_ABANDONED = 0x00000080
const WAIT_TIMEOUT   = 0x00000102
const WAIT_FAILED    = 0xFFFFFFFF

const THREAD_PRIORITY_LOWEST       = -2
const THREAD_PRIORITY_BELOW_NORMAL = -1
const THREAD_PRIORITY_NORMAL       = 0
const THREAD_PRIORITY_ABOVE_NORMAL = 1
const THREAD_PRIORITY_HIGHEST      = 2

func LOWORD(l: u64) => cast(s32) cast #bit (s16) (l & 0xffff)
func HIWORD(l: u64) => cast(s32) cast #bit (s16) ((l >> 16) & 0xffff)

alias SRWLOCK = *void
const SRWLOCK_INIT = cast(SRWLOCK) null

const GMEM_FIXED          = 0x0000
const GMEM_MOVEABLE       = 0x0002
const GMEM_NOCOMPACT      = 0x0010
const GMEM_NODISCARD      = 0x0020
const GMEM_ZEROINIT       = 0x0040
const GMEM_MODIFY         = 0x0080
const GMEM_DISCARDABLE    = 0x0100
const GMEM_NOT_BANKED     = 0x1000
const GMEM_SHARE          = 0x2000
const GMEM_DDESHARE       = 0x2000
const GMEM_NOTIFY         = 0x4000
const GMEM_LOWER          = GMEM_NOT_BANKED
const GMEM_VALID_FLAGS    = 0x7F72
const GMEM_INVALID_HANDLE = 0x8000

struct COORD
{
    x:     WORD
    y:     WORD
}

struct POINT
{
    x:     LONG
    y:     LONG
}

struct SMALL_RECT
{
    left:       WORD
    top:        WORD
    right:      WORD
    bottom:     WORD
}

struct CONSOLE_SCREEN_BUFFER_INFO
{
    dwSize:                  COORD
    dwCursorPosition:        COORD
    wAttributes:             WORD
    srWindow:                SMALL_RECT
    dwMaximumWindowSize:     COORD
}

struct SYSTEM_INFO
{
    dwOemId:                         DWORD         // Must be updated to an union
    dwPageSize:                      DWORD
    lpMinimumApplicationAddress:     LPVOID
    lpMaximumApplicationAddress:     LPVOID
    dwActiveProcessorMask:           DWORD_PTR
    dwNumberOfProcessors:            DWORD
    dwProcessorType:                 DWORD
    dwAllocationGranularity:         DWORD
    wProcessorLevel:                 WORD
    wProcessorRevision:              WORD
}

struct SYSTEMTIME
{
    wYear:             WORD
    wMonth:            WORD
    wDayOfWeek:        WORD
    wDay:              WORD
    wHour:             WORD
    wMinute:           WORD
    wSecond:           WORD
    wMilliseconds:     WORD
}

struct SECURITY_ATTRIBUTES
{
    nLength:                  DWORD
    lpSecurityDescriptor:     LPVOID
    bInheritHandle:           BOOL
}

struct FILETIME
{
    dwLowDateTime:      DWORD
    dwHighDateTime:     DWORD
}

struct WIN32_FILE_ATTRIBUTE_DATA
{
    dwFileAttributes:     DWORD
    ftCreationTime:       FILETIME
    ftLastAccessTime:     FILETIME
    ftLastWriteTime:      FILETIME
    nFileSizeHigh:        DWORD
    nFileSizeLow:         DWORD
}

struct WIN32_FIND_DATAA
{
    dwFileAttributes:       DWORD
    ftCreationTime:         FILETIME
    ftLastAccessTime:       FILETIME
    ftLastWriteTime:        FILETIME
    nFileSizeHigh:          DWORD
    nFileSizeLow:           DWORD
    dwReserved0:            DWORD
    dwReserved1:            DWORD
    cFileName:              [MAX_PATH] CHAR
    cAlternateFileName:     [14] CHAR
    dwFileType:             DWORD
    dwCreatorType:          DWORD
    wFinderFlags:           WORD
}

struct WIN32_FIND_DATAW
{
    dwFileAttributes:       DWORD
    ftCreationTime:         FILETIME
    ftLastAccessTime:       FILETIME
    ftLastWriteTime:        FILETIME
    nFileSizeHigh:          DWORD
    nFileSizeLow:           DWORD
    dwReserved0:            DWORD
    dwReserved1:            DWORD
    cFileName:              [MAX_PATH] WCHAR
    cAlternateFileName:     [14] WCHAR
    dwFileType:             DWORD
    dwCreatorType:          DWORD
    wFinderFlags:           WORD
}

enum GET_FILEEX_INFO_LEVELS
{
    GetFileExInfoStandard
    GetFileExMaxInfoLevel
}

struct STARTUPINFOA
{
    cb:                  DWORD
    lpReserved:          LPSTR
    lpDesktop:           LPSTR
    lpTitle:             LPSTR
    dwX:                 DWORD
    dwY:                 DWORD
    dwXSize:             DWORD
    dwYSize:             DWORD
    dwXCountChars:       DWORD
    dwYCountChars:       DWORD
    dwFillAttribute:     DWORD
    dwFlags:             DWORD
    wShowWindow:         WORD
    cbReserved2:         WORD
    lpReserved2:         LPBYTE
    hStdInput:           HANDLE
    hStdOutput:          HANDLE
    hStdError:           HANDLE
}

struct STARTUPINFOW
{
    cb:                  DWORD
    lpReserved:          LPWSTR
    lpDesktop:           LPWSTR
    lpTitle:             LPWSTR
    dwX:                 DWORD
    dwY:                 DWORD
    dwXSize:             DWORD
    dwYSize:             DWORD
    dwXCountChars:       DWORD
    dwYCountChars:       DWORD
    dwFillAttribute:     DWORD
    dwFlags:             DWORD
    wShowWindow:         WORD
    cbReserved2:         WORD
    lpReserved2:         LPBYTE
    hStdInput:           HANDLE
    hStdOutput:          HANDLE
    hStdError:           HANDLE
}

struct PROCESS_INFORMATION
{
    hProcess:        HANDLE
    hThread:         HANDLE
    dwProcessId:     DWORD
    dwThreadId:      DWORD
}

enum FINDEX_INFO_LEVELS
{
    FindExInfoStandard
    FindExInfoBasic
    FindExInfoMaxInfoLevel
}

enum FINDEX_SEARCH_OPS
{
    FindExSearchNameMatch
    FindExSearchLimitToDirectories
    FindExSearchLimitToDevices
    FindExSearchMaxSearchOp
}

const FIND_FIRST_EX_CASE_SENSITIVE       = 1
const FIND_FIRST_EX_LARGE_FETCH          = 2
const FIND_FIRST_EX_ON_DISK_ENTRIES_ONLY = 4

#[Foreign("kernel32"), Discardable]
{
    func SwitchToThread()->BOOL;
}

#[Foreign("kernel32")]
{
    func GetLastError()->DWORD;
    func FormatMessageA(dwFlags: DWORD, lpSource: LPCVOID, dwMessageId: DWORD, dwLanguageId: DWORD, lpBuffer: LPCSTR, nSize: DWORD, Arguments: #null *va_list)->DWORD;
    func IsDebuggerPresent()->BOOL;
    func DebugBreak();
    func OutputDebugStringA(lpOutputString: LPCSTR);
    func OutputDebugStringW(lpOutputString: LPCWSTR);
    func DebugActiveProcess(procId: DWORD);
    func GetCommandLineA()->const [*] u8;
    func GetCommandLineW()->const [*] u16;

    func GetProcAddress(hModule: HMODULE, lpProcName: LPCSTR)->const *void;
    func GetCurrentProcess()->HANDLE;
    func GetCurrentProcessId()->DWORD;
    func GetModuleHandleA(lpModuleName: LPCSTR)->HMODULE;
    func GetModuleHandleW(lpModuleName: LPCWSTR)->HMODULE;
    func GetStdHandle(nStdHandle: DWORD)->HANDLE;
    func CreateConsoleScreenBuffer(dwDesiredAccess: DWORD, dwShareMode: DWORD, lpSecurityAttributes: const *SECURITY_ATTRIBUTES, dwFlags: DWORD, lpScreenBufferData: LPVOID)->HANDLE;
    func SetConsoleActiveScreenBuffer(hConsoleOutput: HANDLE);
    func WriteConsoleOutputCharacterA(hConsoleOutput: HANDLE, lpCharacter: LPCSTR, nLength: DWORD, dwWriteCoord: DWORD, lpNumberOfCharsWritten: LPDWORD)->BOOL;
    func WriteConsoleOutputCharacterW(hConsoleOutput: HANDLE, lpCharacter: LPCTSTR, nLength: DWORD, dwWriteCoord: DWORD, lpNumberOfCharsWritten: LPDWORD)->BOOL;
    func SetConsoleOutputCP(codePage: UINT);
    func GetLogicalDriveStringsA(nBufferLength: DWORD, lpBuffer: LPCSTR)->DWORD;
    func GetLogicalDriveStringsW(nBufferLength: DWORD, lpBuffer: LPWSTR)->DWORD;

    func GetSystemInfo(lpSystemInfo: *SYSTEM_INFO);
    func GlobalAlloc(uFlags: UINT, dwBytes: SIZE_T)->HGLOBAL;
    func LocalFree(hLocal: HLOCAL)->HLOCAL;
    func GlobalLock(hMem: HGLOBAL)->LPVOID;
    func GlobalUnlock(hMem: HGLOBAL)->BOOL;
    func GlobalSize(hMem: HGLOBAL)->SIZE_T;

    func QueryPerformanceCounter(lpPerformanceCount: *LARGE_INTEGER);
    func QueryPerformanceFrequency(lpFrequency: *LARGE_INTEGER);
    func GetLocalTime(lpSystemTime: *SYSTEMTIME);
    func GetSystemTime(lpSystemTime: *SYSTEMTIME);

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

private namespace Win32Priv
{
    #[Foreign("kernel32")]
    {
        func AllocConsole()->BOOL;
        func FreeConsole()->BOOL;
        func AttachConsole(dwProcessId: DWORD)->BOOL;
        func SetConsoleTextAttribute(hConsoleOutput: HANDLE, wAttributes: WORD)->BOOL;
        func GetConsoleScreenBufferInfo(hConsoleOutput: HANDLE, lpConsoleScreenBufferInfo: *CONSOLE_SCREEN_BUFFER_INFO)->BOOL;

        func CreateThread(lpThreadAttributes: #null *SECURITY_ATTRIBUTES, dwStackSize: SIZE_T, lpStartAddress: LPTHREAD_START_ROUTINE, lpParameter: *void, dwCreationFlags: DWORD, lpThreadId: *DWORD)->HANDLE;
        func ResumeThread(hThread: HANDLE)->DWORD;
        func SetThreadPriority(hThread: HANDLE, nPriority: s32)->BOOL;
        func WaitForSingleObject(hHandle: HANDLE, dwMilliseconds: DWORD)->DWORD;
        func WaitForMultipleObjects(nCount: DWORD, lpHandles: const *HANDLE, bWaitAll: BOOL, dwMilliseconds: DWORD)->DWORD;
        func ReleaseMutex(hMutex: HANDLE)->BOOL;
        func ResetEvent(hEvent: HANDLE)->BOOL;
        func SetEvent(hEvent: HANDLE)->BOOL;

        func CreateProcessA(lpApplicationName: LPCSTR, lpCommandLine: LPCSTR, lpProcessAttributes: #null *SECURITY_ATTRIBUTES, lpThreadAttributes: #null *SECURITY_ATTRIBUTES, bInheritHandles: BOOL, dwCreationFlags: DWORD, lpEnvironment: LPVOID, lpCurrentDirectory: LPCSTR, lpStartupInfo: *STARTUPINFOA, lpProcessInformation: *PROCESS_INFORMATION)->BOOL;
        func CreateProcessW(lpApplicationName: LPCWSTR, lpCommandLine: LPCWSTR, lpProcessAttributes: #null *SECURITY_ATTRIBUTES, lpThreadAttributes: #null *SECURITY_ATTRIBUTES, bInheritHandles: BOOL, dwCreationFlags: DWORD, lpEnvironment: LPVOID, lpCurrentDirectory: LPCWSTR, lpStartupInfo: *STARTUPINFOW, lpProcessInformation: *PROCESS_INFORMATION)->BOOL;
        func CreateEventA(lpEventAttributes: #null *SECURITY_ATTRIBUTES, bManualReset: BOOL, bInitialState: BOOL, lpName: LPCSTR)->HANDLE;
        func CreateEventW(lpEventAttributes: #null *SECURITY_ATTRIBUTES, bManualReset: BOOL, bInitialState: BOOL, lpName: LPCWSTR)->HANDLE;
        func CreateMutexA(lpMutexAttributes: #null *SECURITY_ATTRIBUTES, bInitialOwner: BOOL, lpName: LPCSTR)->HANDLE;
        func CreateMutexW(lpMutexAttributes: #null *SECURITY_ATTRIBUTES, bInitialOwner: BOOL, lpName: LPCWSTR)->HANDLE;
        func CreateTimerQueueTimer(phNewTimer: *HANDLE, timerQueue: HANDLE, callback: WAITORTIMERCALLBACK, parameter: *void, dueTime: DWORD, period: DWORD, flags: ULONG)->BOOL;
        func DeleteTimerQueueTimer(timerQueue: HANDLE, timer: HANDLE, completionEvent: HANDLE)->BOOL;

        func WriteFile(hFile: HANDLE, lpBuffer: LPCVOID, nNumberOfBytesToWrite: DWORD, lpNumberOfBytesWritten: LPDWORD, lpOverlapped: LPOVERLAPPED)->BOOL;
        func DeleteFileA(lpFileName: LPCSTR)->BOOL;
        func DeleteFileW(lpFileName: LPCWSTR)->BOOL;
        func GetFileAttributesA(lpFileName: LPCSTR)->DWORD;
        func GetFileAttributesW(lpFileName: LPCWSTR)->DWORD;
        func GetFileAttributesExA(lpFileName: LPCSTR, fInfoLevelId: GET_FILEEX_INFO_LEVELS, lpFileInformation: LPVOID)->BOOL;
        func GetFileAttributesExW(lpFileName: LPCWSTR, fInfoLevelId: GET_FILEEX_INFO_LEVELS, lpFileInformation: LPVOID)->BOOL;
        func CreateFileA(lpFileName: LPCSTR, dwDesiredAccess: DWORD, dwShareMode: DWORD, lpSecurityAttributes: #null *SECURITY_ATTRIBUTES, dwCreationDisposition: DWORD, dwFlagsAndAttributes: DWORD, hTemplateFile: HANDLE)->HANDLE;
        func CreateFileW(lpFileName: LPCWSTR, dwDesiredAccess: DWORD, dwShareMode: DWORD, lpSecurityAttributes: #null *SECURITY_ATTRIBUTES, dwCreationDisposition: DWORD, dwFlagsAndAttributes: DWORD, hTemplateFile: HANDLE)->HANDLE;
        func SetFilePointer(hFile: HANDLE, lDistanceToMove: LONG, lpDistanceToMoveHigh: PLONG, dwMoveMethod: DWORD)->DWORD;
        func GetFileSize(hFile: HANDLE, lpFileSizeHigh: LPDWORD)->DWORD;

        func FileTimeToSystemTime(lpFileTime: const *FILETIME, lpSystemTime: *SYSTEMTIME)->BOOL;
        func SystemTimeToFileTime(lpSystemTime: const *SYSTEMTIME, lpFileTime: *FILETIME)->BOOL;
        func SetFileTime(hFile: HANDLE, lpCreationTime: #null const *FILETIME, lpLastAccessTime: #null const *FILETIME, lpLastWriteTime: #null const *FILETIME)->BOOL;

        func FindFirstFileA(lpFileName: LPCSTR, lpFindFileData: *WIN32_FIND_DATAA)->HANDLE;
        func FindFirstFileW(lpFileName: LPCWSTR, lpFindFileData: *WIN32_FIND_DATAW)->HANDLE;
        func FindFirstFileExA(lpFileName: LPCSTR, fInfoLevelId: FINDEX_INFO_LEVELS, lpFindFileData: LPVOID, fSearchOp: FINDEX_SEARCH_OPS, lpSearchFilter: LPVOID, dwAdditionalFlags: DWORD)->HANDLE;
        func FindFirstFileExW(lpFileName: LPCWSTR, fInfoLevelId: FINDEX_INFO_LEVELS, lpFindFileData: LPVOID, fSearchOp: FINDEX_SEARCH_OPS, lpSearchFilter: LPVOID, dwAdditionalFlags: DWORD)->HANDLE;
        func FindNextFileA(hFindFile: HANDLE, lpFindFileData: *WIN32_FIND_DATAA)->BOOL;
        func FindNextFileW(hFindFile: HANDLE, lpFindFileData: *WIN32_FIND_DATAW)->BOOL;
        func FindClose(hFindFile: HANDLE)->BOOL;
        func FindFirstVolumeA(lpszVolumeName: LPSTR, cchBufferLength: DWORD)->HANDLE;
        func FindFirstVolumeW(lpszVolumeName: LPWSTR, cchBufferLength: DWORD)->HANDLE;
        func FindNextVolumeA(hFindVolume: HANDLE, lpszVolumeName: LPSTR, cchBufferLength: DWORD)->BOOL;
        func FindNextVolumeW(hFindVolume: HANDLE, lpszVolumeName: LPWSTR, cchBufferLength: DWORD)->BOOL;
        func FindVolumeClose(hFindVolume: HANDLE)->BOOL;
        func GetVolumePathNamesForVolumeNameA(lpszVolumeName: LPCSTR, lpszVolumePathNames: LPCH, cchBufferLength: DWORD, lpcchReturnLength: *DWORD)->BOOL;
        func GetVolumePathNamesForVolumeNameW(lpszVolumeName: LPCWSTR, lpszVolumePathNames: LPWCH, cchBufferLength: DWORD, lpcchReturnLength: *DWORD)->BOOL;

        func CreateDirectoryA(lpPathName: LPCSTR, lpSecurityAttributes: #null *SECURITY_ATTRIBUTES)->BOOL;
        func CreateDirectoryW(lpPathName: LPCWSTR, lpSecurityAttributes: #null *SECURITY_ATTRIBUTES)->BOOL;
        func GetCurrentDirectoryA(nBufferLength: DWORD, lpBuffer: LPCSTR)->DWORD;
        func GetCurrentDirectoryW(nBufferLength: DWORD, lpBuffer: LPCWSTR)->DWORD;
        func SetCurrentDirectoryA(lpPathName: LPCSTR)->BOOL;
        func SetCurrentDirectoryW(lpPathName: LPCWSTR)->BOOL;
        func GetFullPathNameA(lpFileName: LPCSTR, nBufferLength: DWORD, lpBuffer: LPSTR, lpFilePart: #null *LPSTR)->DWORD;
        func GetFullPathNameW(lpFileName: LPCTSTR, nBufferLength: DWORD, lpBuffer: LPWSTR, lpFilePart: #null *LPWSTR)->DWORD;
    }
}

#[Inline]
{
    func AllocConsole() throw
    {
        if !Win32Priv.AllocConsole() do
            throwWinError(GetLastError())
    }

    func FreeConsole() throw
    {
        if !Win32Priv.FreeConsole() do
            throwWinError(GetLastError())
    }

    func AttachConsole(dwProcessId: DWORD) throw
    {
        if !Win32Priv.AttachConsole(dwProcessId) do
            throwWinError(GetLastError())
    }

    func SetConsoleTextAttribute(hConsoleOutput: HANDLE, wAttributes: WORD) throw
    {
        if !Win32Priv.SetConsoleTextAttribute(hConsoleOutput, wAttributes) do
            throwWinError(GetLastError())
    }

    func GetConsoleScreenBufferInfo(hConsoleOutput: HANDLE, lpConsoleScreenBufferInfo: *CONSOLE_SCREEN_BUFFER_INFO) throw
    {
        if !Win32Priv.GetConsoleScreenBufferInfo(hConsoleOutput, lpConsoleScreenBufferInfo) do
            throwWinError(GetLastError())
    }

    func CreateThread(lpThreadAttributes: #null *SECURITY_ATTRIBUTES, dwStackSize: SIZE_T, lpStartAddress: LPTHREAD_START_ROUTINE, lpParameter: *void, dwCreationFlags: DWORD, lpThreadId: *DWORD)->HANDLE throw
    {
        let h = Win32Priv.CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId)
        if h == null do
            try Win32.throwWinError(GetLastError())
        return h
    }

    #[Discardable]
    func ResumeThread(hThread: HANDLE)->DWORD throw
    {
        let res = Win32Priv.ResumeThread(hThread)
        if res == cast #bit (DWORD) -1 do
            throwWinError(GetLastError())
        return res
    }

    func SetThreadPriority(hThread: HANDLE, nPriority: s32) throw
    {
        if !Win32Priv.SetThreadPriority(hThread, nPriority) do
            throwWinError(GetLastError())
    }

    #[Discardable]
    func WaitForSingleObject(hHandle: HANDLE, dwMilliseconds: DWORD)->DWORD throw
    {
        let res = Win32Priv.WaitForSingleObject(hHandle, dwMilliseconds)
        if res == WAIT_FAILED do
            throwWinError(GetLastError())
        return res
    }

    #[Discardable]
    func WaitForMultipleObjects(nCount: DWORD, lpHandles: const *HANDLE, bWaitAll: BOOL, dwMilliseconds: DWORD)->DWORD throw
    {
        let res = Win32Priv.WaitForMultipleObjects(nCount, lpHandles, bWaitAll, dwMilliseconds)
        if res == WAIT_FAILED do
            throwWinError(GetLastError())
        return res
    }

    func ReleaseMutex(hMutex: HANDLE) throw
    {
        if !Win32Priv.ReleaseMutex(hMutex) do
            throwWinError(GetLastError())
    }

    func ResetEvent(hEvent: HANDLE) throw
    {
        if !Win32Priv.ResetEvent(hEvent) do
            throwWinError(GetLastError())
    }

    func SetEvent(hEvent: HANDLE) throw
    {
        if !Win32Priv.SetEvent(hEvent) do
            throwWinError(GetLastError())
    }

    func CreateProcessA(lpApplicationName: LPCSTR, lpCommandLine: LPCSTR, lpProcessAttributes: #null *SECURITY_ATTRIBUTES, lpThreadAttributes: #null *SECURITY_ATTRIBUTES, bInheritHandles: BOOL, dwCreationFlags: DWORD, lpEnvironment: LPVOID, lpCurrentDirectory: LPCSTR, lpStartupInfo: *STARTUPINFOA, lpProcessInformation: *PROCESS_INFORMATION) throw
    {
        if !Win32Priv.CreateProcessA(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation) do
            throwWinError(GetLastError())
    }

    func CreateProcessW(lpApplicationName: LPCWSTR, lpCommandLine: LPCWSTR, lpProcessAttributes: #null *SECURITY_ATTRIBUTES, lpThreadAttributes: #null *SECURITY_ATTRIBUTES, bInheritHandles: BOOL, dwCreationFlags: DWORD, lpEnvironment: LPVOID, lpCurrentDirectory: LPCWSTR, lpStartupInfo: *STARTUPINFOW, lpProcessInformation: *PROCESS_INFORMATION) throw
    {
        if !Win32Priv.CreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation) do
            throwWinError(GetLastError())
    }

    func CreateEventA(lpEventAttributes: #null *SECURITY_ATTRIBUTES, bManualReset: BOOL, bInitialState: BOOL, lpName: LPCSTR)->HANDLE throw
    {
        let h = Win32Priv.CreateEventA(lpEventAttributes, bManualReset, bInitialState, lpName)
        if !h do
            throwWinError(GetLastError())
        return h
    }

    func CreateEventW(lpEventAttributes: #null *SECURITY_ATTRIBUTES, bManualReset: BOOL, bInitialState: BOOL, lpName: LPCWSTR)->HANDLE throw
    {
        let h = Win32Priv.CreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName)
        if !h do
            throwWinError(GetLastError())
        return h
    }

    func CreateMutexA(lpMutexAttributes: #null *SECURITY_ATTRIBUTES, bInitialOwner: BOOL, lpName: LPCSTR)->HANDLE throw
    {
        let h = Win32Priv.CreateMutexA(lpMutexAttributes, bInitialOwner, lpName)
        if !h do
            throwWinError(GetLastError())
        return h
    }

    func CreateMutexW(lpMutexAttributes: #null *SECURITY_ATTRIBUTES, bInitialOwner: BOOL, lpName: LPCWSTR)->HANDLE throw
    {
        let h = Win32Priv.CreateMutexW(lpMutexAttributes, bInitialOwner, lpName)
        if !h do
            throwWinError(GetLastError())
        return h
    }

    func CreateTimerQueueTimer(phNewTimer: *HANDLE, timerQueue: HANDLE, callback: WAITORTIMERCALLBACK, parameter: *void, dueTime: DWORD, period: DWORD, flags: ULONG) throw
    {
        if !Win32Priv.CreateTimerQueueTimer(phNewTimer, timerQueue, callback, parameter, dueTime, period, flags) do
            throwWinError(GetLastError())
    }

    func DeleteTimerQueueTimer(timerQueue: HANDLE, timer: HANDLE, completionEvent: HANDLE)
    {
        for
        {
            let res = Win32Priv.DeleteTimerQueueTimer(timerQueue, timer, completionEvent)
            if res or GetLastError() == ERROR_IO_PENDING do
                return
        }
    }

    #[Discardable]
    func WriteFile(hFile: HANDLE, lpBuffer: LPCVOID, nNumberOfBytesToWrite: DWORD, lpNumberOfBytesWritten: LPDWORD, lpOverlapped: LPOVERLAPPED)->BOOL throw
    {
        let res = Win32Priv.WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped)
        if !res and GetLastError() != ERROR_IO_PENDING do
            throwWinError(GetLastError())
        return res
    }

    func FileTimeToSystemTime(lpFileTime: const *FILETIME, lpSystemTime: *SYSTEMTIME) throw
    {
        if !Win32Priv.FileTimeToSystemTime(lpFileTime, lpSystemTime) do
            throwWinError(GetLastError())
    }

    func SystemTimeToFileTime(lpSystemTime: const *SYSTEMTIME, lpFileTime: *FILETIME) throw
    {
        if !Win32Priv.SystemTimeToFileTime(lpSystemTime, lpFileTime) do
            throwWinError(GetLastError())
    }

    func SetFileTime(hFile: HANDLE, lpCreationTime: #null const *FILETIME, lpLastAccessTime: #null const *FILETIME, lpLastWriteTime: #null const *FILETIME) throw
    {
        if !Win32Priv.SetFileTime(hFile, lpCreationTime, lpLastAccessTime, lpLastWriteTime) do
            throwWinError(GetLastError())
    }

    func DeleteFileA(lpFileName: LPCSTR) throw
    {
        if !Win32Priv.DeleteFileA(lpFileName) do
            throwWinError(GetLastError())
    }

    func DeleteFileW(lpFileName: LPCWSTR) throw
    {
        if !Win32Priv.DeleteFileW(lpFileName) do
            throwWinError(GetLastError())
    }

    func GetFileAttributesA(lpFileName: LPCSTR)->DWORD throw
    {
        let attributes = Win32Priv.GetFileAttributesA(lpFileName)
        if attributes == INVALID_FILE_ATTRIBUTES do
            throwWinError(GetLastError())
        return attributes
    }

    func GetFileAttributesW(lpFileName: LPCWSTR)->DWORD throw
    {
        let attributes = Win32Priv.GetFileAttributesW(lpFileName)
        if attributes == INVALID_FILE_ATTRIBUTES do
            throwWinError(GetLastError())
        return attributes
    }

    func GetFileAttributesExA(lpFileName: LPCSTR, fInfoLevelId: GET_FILEEX_INFO_LEVELS, lpFileInformation: LPVOID) throw
    {
        if !Win32Priv.GetFileAttributesExA(lpFileName, fInfoLevelId, lpFileInformation) do
            throwWinError(GetLastError())
    }

    func GetFileAttributesExW(lpFileName: LPCWSTR, fInfoLevelId: GET_FILEEX_INFO_LEVELS, lpFileInformation: LPVOID) throw
    {
        if !Win32Priv.GetFileAttributesExW(lpFileName, fInfoLevelId, lpFileInformation) do
            throwWinError(GetLastError())
    }

    func GetCurrentDirectoryA(nBufferLength: DWORD, lpBuffer: LPCSTR)->DWORD throw
    {
        let length = Win32Priv.GetCurrentDirectoryA(nBufferLength, lpBuffer)
        if !length do
            throwWinError(GetLastError())
        return length
    }

    func GetCurrentDirectoryW(nBufferLength: DWORD, lpBuffer: LPCWSTR)->DWORD throw
    {
        let length = Win32Priv.GetCurrentDirectoryW(nBufferLength, lpBuffer)
        if !length do
            throwWinError(GetLastError())
        return length
    }

    func SetCurrentDirectoryA(lpPathName: LPCSTR) throw
    {
        if !Win32Priv.SetCurrentDirectoryA(lpPathName) do
            throwWinError(GetLastError())
    }

    func SetCurrentDirectoryW(lpPathName: LPCWSTR) throw
    {
        if !Win32Priv.SetCurrentDirectoryW(lpPathName) do
            throwWinError(GetLastError())
    }

    func CreateFileA(lpFileName: LPCSTR, dwDesiredAccess: DWORD, dwShareMode: DWORD, lpSecurityAttributes: #null *SECURITY_ATTRIBUTES, dwCreationDisposition: DWORD, dwFlagsAndAttributes: DWORD, hTemplateFile: HANDLE)->HANDLE throw
    {
        let h = Win32Priv.CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile)
        if h == INVALID_HANDLE_VALUE do
            throwWinError(GetLastError())
        return h
    }

    func CreateFileW(lpFileName: LPCWSTR, dwDesiredAccess: DWORD, dwShareMode: DWORD, lpSecurityAttributes: #null *SECURITY_ATTRIBUTES, dwCreationDisposition: DWORD, dwFlagsAndAttributes: DWORD, hTemplateFile: HANDLE)->HANDLE throw
    {
        let h = Win32Priv.CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile)
        if h == INVALID_HANDLE_VALUE do
            throwWinError(GetLastError())
        return h
    }

    func FindFirstFileA(lpFileName: LPCSTR, lpFindFileData: *WIN32_FIND_DATAA)->HANDLE throw
    {
        let res = Win32Priv.FindFirstFileA(lpFileName, lpFindFileData)
        if res == INVALID_HANDLE_VALUE and GetLastError() != ERROR_FILE_NOT_FOUND do
            throwWinError(GetLastError())
        return res
    }

    func FindFirstFileW(lpFileName: LPCWSTR, lpFindFileData: *WIN32_FIND_DATAW)->HANDLE throw
    {
        let res = Win32Priv.FindFirstFileW(lpFileName, lpFindFileData)
        if res == INVALID_HANDLE_VALUE and GetLastError() != ERROR_FILE_NOT_FOUND do
            throwWinError(GetLastError())
        return res
    }

    func FindFirstFileExA(lpFileName: LPCSTR, fInfoLevelId: FINDEX_INFO_LEVELS, lpFindFileData: LPVOID, fSearchOp: FINDEX_SEARCH_OPS, lpSearchFilter: LPVOID, dwAdditionalFlags: DWORD)->HANDLE throw
    {
        let res = Win32Priv.FindFirstFileExA(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags)
        if res == INVALID_HANDLE_VALUE and GetLastError() != ERROR_FILE_NOT_FOUND do
            throwWinError(GetLastError())
        return res
    }

    func FindFirstFileExW(lpFileName: LPCWSTR, fInfoLevelId: FINDEX_INFO_LEVELS, lpFindFileData: LPVOID, fSearchOp: FINDEX_SEARCH_OPS, lpSearchFilter: LPVOID, dwAdditionalFlags: DWORD)->HANDLE throw
    {
        let res = Win32Priv.FindFirstFileExW(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags)
        if res == INVALID_HANDLE_VALUE and GetLastError() != ERROR_FILE_NOT_FOUND do
            throwWinError(GetLastError())
        return res
    }

    func FindNextFileA(hFindFile: HANDLE, lpFindFileData: *WIN32_FIND_DATAA)->BOOL throw
    {
        let res = Win32Priv.FindNextFileA(hFindFile, lpFindFileData)
        if !res and GetLastError() != ERROR_NO_MORE_FILES do
            throwWinError(GetLastError())
        return res
    }

    func FindNextFileW(hFindFile: HANDLE, lpFindFileData: *WIN32_FIND_DATAW)->BOOL throw
    {
        let res = Win32Priv.FindNextFileW(hFindFile, lpFindFileData)
        if !res and GetLastError() != ERROR_NO_MORE_FILES do
            throwWinError(GetLastError())
        return res
    }

    func FindClose(hFindFile: HANDLE) throw
    {
        if !Win32Priv.FindClose(hFindFile) do
            throwWinError(GetLastError())
    }

    func FindFirstVolumeA(lpszVolumeName: LPSTR, cchBufferLength: DWORD)->HANDLE throw
    {
        let res = Win32Priv.FindFirstVolumeA(lpszVolumeName, cchBufferLength)
        if res == INVALID_HANDLE_VALUE do
            throwWinError(GetLastError())
        return res
    }

    func FindFirstVolumeW(lpszVolumeName: LPWSTR, cchBufferLength: DWORD)->HANDLE throw
    {
        let res = Win32Priv.FindFirstVolumeW(lpszVolumeName, cchBufferLength)
        if res == INVALID_HANDLE_VALUE do
            throwWinError(GetLastError())
        return res
    }

    func FindNextVolumeA(hFindVolume: HANDLE, lpszVolumeName: LPSTR, cchBufferLength: DWORD)->BOOL throw
    {
        let res = Win32Priv.FindNextVolumeA(hFindVolume, lpszVolumeName, cchBufferLength)
        if !res and GetLastError() != ERROR_NO_MORE_FILES do
            throwWinError(GetLastError())
        return res
    }

    func FindNextVolumeW(hFindVolume: HANDLE, lpszVolumeName: LPWSTR, cchBufferLength: DWORD)->BOOL throw
    {
        let res = Win32Priv.FindNextVolumeW(hFindVolume, lpszVolumeName, cchBufferLength)
        if !res and GetLastError() != ERROR_NO_MORE_FILES do
            throwWinError(GetLastError())
        return res
    }

    func FindVolumeClose(hFindVolume: HANDLE) throw
    {
        if !Win32Priv.FindVolumeClose(hFindVolume) do
            throwWinError(GetLastError())
    }

    func GetVolumePathNamesForVolumeNameA(lpszVolumeName: LPCSTR, lpszVolumePathNames: LPCH, cchBufferLength: DWORD, lpcchReturnLength: *DWORD) throw
    {
        if !Win32Priv.GetVolumePathNamesForVolumeNameA(lpszVolumeName, lpszVolumePathNames, cchBufferLength, lpcchReturnLength) do
            throwWinError(GetLastError())
    }

    func GetVolumePathNamesForVolumeNameW(lpszVolumeName: LPCWSTR, lpszVolumePathNames: LPWCH, cchBufferLength: DWORD, lpcchReturnLength: *DWORD) throw
    {
        if !Win32Priv.GetVolumePathNamesForVolumeNameW(lpszVolumeName, lpszVolumePathNames, cchBufferLength, lpcchReturnLength) do
            throwWinError(GetLastError())
    }

    #[Discardable]
    func SetFilePointer(hFile: HANDLE, lDistanceToMove: LONG, lpDistanceToMoveHigh: PLONG, dwMoveMethod: DWORD)->DWORD throw
    {
        let low = Win32Priv.SetFilePointer(hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod)
        if low == INVALID_SET_FILE_POINTER do
            throwWinError(GetLastError())
        return low
    }

    func GetFileSize(hFile: HANDLE, lpFileSizeHigh: LPDWORD)->DWORD throw
    {
        let low = Win32Priv.GetFileSize(hFile, lpFileSizeHigh)
        if low == INVALID_FILE_SIZE do
            throwWinError(GetLastError())
        return low
    }

    #[Discardable]
    func GetFullPathNameA(lpFileName: LPCSTR, nBufferLength: DWORD, lpBuffer: LPSTR, lpFilePart: #null *LPSTR)->DWORD throw
    {
        let res = Win32Priv.GetFullPathNameA(lpFileName, nBufferLength, lpBuffer, lpFilePart)
        if !res do
            throwWinError(GetLastError())
        return res
    }

    #[Discardable]
    func GetFullPathNameW(lpFileName: LPCTSTR, nBufferLength: DWORD, lpBuffer: LPWSTR, lpFilePart: #null *LPWSTR)->DWORD throw
    {
        let res = Win32Priv.GetFullPathNameW(lpFileName, nBufferLength, lpBuffer, lpFilePart)
        if !res do
            throwWinError(GetLastError())
        return res
    }

    func CreateDirectoryA(lpPathName: LPCSTR, lpSecurityAttributes: #null *SECURITY_ATTRIBUTES) throw
    {
        let res = Win32Priv.CreateDirectoryA(lpPathName, lpSecurityAttributes)
        if !res do
            throwWinError(GetLastError())
    }

    func CreateDirectoryW(lpPathName: LPCWSTR, lpSecurityAttributes: #null *SECURITY_ATTRIBUTES) throw
    {
        let res = Win32Priv.CreateDirectoryW(lpPathName, lpSecurityAttributes)
        if !res do
            throwWinError(GetLastError())
    }
}

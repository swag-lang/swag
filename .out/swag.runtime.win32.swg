#if #os == "windows";

////////////////////////////////////////////////////////////
private
{
    alias HANDLE = *void
    alias DWORD  = u32
    const STD_OUTPUT_HANDLE     = cast(u32) -11
    const MB_CANCELTRYCONTINUE  = 0x00000006
    const MB_ICONERROR          = 0x00000010
    const IDCANCEL    = 2
    const IDTRYAGAIN  = 10
    const IDCONTINUE  = 11

    #[swag.foreign("kernel32")]
    {
        func GetStdHandle(nStdHandle: u32)->HANDLE;
        func WriteFile(hFile: HANDLE, lpBuffer: const *void, nNumberOfBytesToWrite: u32, lpNumberOfBytesWritten: *u32, lpOverlapped: *void)->u32;
        func ExitProcess(uExitCode: u32);
        func RaiseException(dwExceptionCode: u32, dwExceptionFlags: u32, nNumberOfArguments: u32, lpArguments: const **void);
        func MessageBoxA(hWnd: const *void, lpText: const *void, lpCaption: const *void, uType: u32)->s32;
        func DebugBreak();
        func LoadLibraryA(name: const *u8);
        func GetCommandLineA()->const *u8;
        func TlsAlloc()->u32;
        func TlsSetValue(dwTlsIndex: u32, lpTlsValue: *void);
        func TlsGetValue(dwTlsIndex: u32)->*void;
        func RtlCopyMemory(destination: *void, source: const *void, length: u64);
        func RtlFillMemory(destination: *void, length: u64, fill: u32);
        func HeapAlloc(hHeap: HANDLE, dwFlags: DWORD, dwBytes: u64)->*void;
        func HeapReAlloc(hHeap: HANDLE, dwFlags: DWORD, mem: *void, dwBytes: u64)->*void;
        func HeapFree(hHeap: HANDLE, dwFlags: DWORD, mem: *void);
        func GetProcessHeap()->HANDLE;
    }
}

////////////////////////////////////////////////////////////
func @memcpy(dst: *void, src: const *void, size: u32)
{
    RtlCopyMemory(dst, src, cast(u64) size)
}

////////////////////////////////////////////////////////////
func @memset(dst: *void, value: u8, size: u32)
{
    RtlFillMemory(dst, cast(u64) size, cast(u32) value)
}

////////////////////////////////////////////////////////////
func @print(message: string)
{
    if !@dataof(message) || !@countof(message)
        return
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), @dataof(message), @countof(message), null, null);
}

////////////////////////////////////////////////////////////
private var g_exceptionLocation: swag.CompilerSourceLocation
private var g_exceptionParams: [3] const *void
func @error(message: string, loc: swag.CompilerSourceLocation)
{
    contextFlags := @getcontext().flags
    if(contextFlags & swag.ContextFlags.ByteCode)
    {
        g_exceptionLocation = loc
        g_exceptionParams[0] = cast(const *void) &g_exceptionLocation
        g_exceptionParams[1] = cast(const *void) @dataof(message)
        g_exceptionParams[2] = cast(const *void) cast(u64) @countof(message)
        RaiseException(666, 0, 3, &g_exceptionParams[0])
    }

    @print("error: ")
    @print(loc.fileName)
    @print(":")
    @print(cast(s64) (loc.lineStart + 1))
    @print(": ")
    @print(message)
    @print("\n")

    ExitProcess(cast(u32) -666)
}

////////////////////////////////////////////////////////////
func __alloc(size: u32)->*void
{
    if !size return null
    return HeapAlloc(GetProcessHeap(), 0, cast(u64) size)
}

func __realloc(ptr: *void, size: u32)->*void
{
    if !ptr return __alloc(size)
    return HeapReAlloc(GetProcessHeap(), 0, ptr, cast(u64) size)
}

func __free(ptr: *void)
{
    if !ptr return
    HeapFree(GetProcessHeap(), 0, ptr)
}

////////////////////////////////////////////////////////////
func __getCommandLine()                     => GetCommandLineA()
func __loaddll(name: string)                => LoadLibraryA(@dataof(name))
func __exit()                               => ExitProcess(0)
func __tlsAlloc()                           => cast(u64) TlsAlloc()
func __tlsSetValue(id: u64, value: *void)   => TlsSetValue(cast(u32) id, value)
func __tlsGetValue(id: u64)->*void          => TlsGetValue(cast(u32) id)

////////////////////////////////////////////////////////////
func __assertBox(msg, title: string)
{
    result := MessageBoxA(null, @dataof(msg), @dataof(title), MB_ICONERROR | MB_CANCELTRYCONTINUE)
    switch result
    {
        case IDCANCEL:
            ExitProcess(cast(u32) -666)
        case IDTRYAGAIN:
            DebugBreak();
        case IDCONTINUE:
            break
    }
}

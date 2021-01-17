#global if #os == "windows"
#global foreignlib "kernel32"
#global foreignlib "user32"

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
        func RaiseException(dwExceptionCode: u32, dwExceptionFlags: u32, nNumberOfArguments: u32, lpArguments: *const *void);
        func MessageBoxA(hWnd: const *void, lpText: const *void, lpCaption: const *void, uType: u32)->s32;
        func DebugBreak();
        func LoadLibraryA(name: const *u8);
        func GetCommandLineA()->const *u8;
        func TlsAlloc()->u32;
        func TlsSetValue(dwTlsIndex: u32, lpTlsValue: *void);
        func TlsGetValue(dwTlsIndex: u32)->*void;
        func RtlCopyMemory(destination: *void, source: const *void, length: uint);
        func RtlMoveMemory(destination: *void, source: const *void, length: uint);
        func RtlCompareMemory(destination, source: const *void, length: uint)->uint;
        func HeapAlloc(hHeap: HANDLE, dwFlags: DWORD, dwBytes: uint)->*void;
        func HeapReAlloc(hHeap: HANDLE, dwFlags: DWORD, mem: *void, dwBytes: uint)->*void;
        func HeapFree(hHeap: HANDLE, dwFlags: DWORD, mem: *void);
        func GetProcessHeap()->HANDLE;
    }
}

////////////////////////////////////////////////////////////
func @memcmp(dst, src: const *void, size: uint)->s32
{
    len := RtlCompareMemory(dst, src, size)
    if len == size return 0
    p1 := cast(const *s8) dst
    p2 := cast(const *s8) src
    return p1[len] - p2[len]
}

////////////////////////////////////////////////////////////
func @memcpy(dst: *void, src: const *void, size: uint)
{
    RtlCopyMemory(dst, src, size)
}

////////////////////////////////////////////////////////////
func @memmove(dst: *void, src: const *void, size: uint)
{
    RtlMoveMemory(dst, src, size)
}

////////////////////////////////////////////////////////////
func @print(message: string)
{
    if !@dataof(message) || !@countof(message)
        return
    len := cast(u32) @countof(message)
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), @dataof(message), len, null, null);
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
        g_exceptionParams[2] = cast(const *void) cast(uint) @countof(message)
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
func __alloc(size: uint)->*void
{
    if !size return null
    return HeapAlloc(GetProcessHeap(), 0, size)
}

func __realloc(ptr: *void, size: uint)->*void
{
    if !ptr return __alloc(size)
    return HeapReAlloc(GetProcessHeap(), 0, ptr, size)
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

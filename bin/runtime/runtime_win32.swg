#global if #os == "windows"
#global foreignlib "kernel32"
#global foreignlib "user32"

////////////////////////////////////////////////////////////
private
{
    alias HANDLE = *void
    alias DWORD  = u32
    const STD_OUTPUT_HANDLE     = bitcast(u32) -11
    const MB_CANCELTRYCONTINUE  = 0x00000006
    const MB_ICONERROR          = 0x00000010
    const IDCANCEL    = 2
    const IDTRYAGAIN  = 10
    const IDCONTINUE  = 11

    #[Swag.Foreign("kernel32")]
    {
        func GetStdHandle(nStdHandle: u32)->HANDLE;
        func WriteFile(hFile: HANDLE, lpBuffer: const *void, nNumberOfBytesToWrite: u32, lpNumberOfBytesWritten: *u32, lpOverlapped: *void)->u32;
        func ExitProcess(uExitCode: u32);
        func RaiseException(dwExceptionCode: u32, dwExceptionFlags: u32, nNumberOfArguments: u32, lpArguments: *const *void);
        func DebugBreak();
        func LoadLibraryA(name: const *u8);
        func GetCommandLineA()->const *u8;
        func TlsAlloc()->u32;
        func TlsSetValue(dwTlsIndex: u32, lpTlsValue: *void);
        func TlsGetValue(dwTlsIndex: u32)->*void;
    }

    #[Swag.Foreign("user32")]
    {
        func MessageBoxA(hWnd: const *void, lpText: const *void, lpCaption: const *void, uType: u32)->s32;
    }
}

////////////////////////////////////////////////////////////
func __printString(message: string)
{
    if !@dataof(message) or !@countof(message)
        return
    len := cast(u32) @countof(message)
    wlen := WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), @dataof(message), len, null, null);
}

////////////////////////////////////////////////////////////
func @errormsg(message: string, loc: Swag.CompilerSourceLocation)
{
    cxt := @getcontext()
    contextFlags := cxt.flags
    if contextFlags & Swag.ContextFlags.ByteCode
    {
        cxt.exceptionLoc = loc
        cxt.exceptionParams[0] = cast(const *void) &cxt.exceptionLoc
        cxt.exceptionParams[1] = cast(const *void) @dataof(message)
        cxt.exceptionParams[2] = cast(const *void) cast(uint) @countof(message)
        RaiseException(666, 0, 3, &cxt.exceptionParams[0])
        return
    }

    // Construct error message, to print it in one instruction
    var msg: [1024] u8
    ptr := &msg[0]

    @memcpy(ptr, @dataof("error: "), 7)
    ptr += 7

    @memcpy(ptr, @dataof(loc.fileName), @countof(loc.fileName))
    ptr += @countof(loc.fileName)

    ptr[0] = ":"'u8
    ptr += 1
    ptr = __itoa(ptr, cast(s64) loc.lineStart + 1)
    ptr[0] = ":"'u8
    ptr += 1
    ptr = __itoa(ptr, cast(s64) loc.colStart + 1)
    ptr[0] = ":"'u8
    ptr[1] = " "'u8
    ptr += 2

    lenMsg := @countof(message)
    if lenMsg > 512 lenMsg = 512
    if lenMsg
    {
        @memcpy(ptr, @dataof(message), lenMsg)
        ptr += lenMsg
    }

    ptr[0] = "\n"'u8
    ptr[1] = 0
    ptr += 1

    lenTotal := ptr - @dataof(msg)
    @assert(lenTotal < @countof(msg))
    @print(@mkstring(&msg[0], lenTotal))
}

////////////////////////////////////////////////////////////
func __getCommandLine()                     => GetCommandLineA()
func __loaddll(name: string)                => LoadLibraryA(@dataof(name))
func __exit()                               => ExitProcess(0)
func __tlsAlloc()                           => cast(u64) TlsAlloc()
func __tlsSetValue(id: u64, value: *void)   => TlsSetValue(cast(u32) id, value)
func __tlsGetValue(id: u64)->*void          => TlsGetValue(cast(u32) id)

////////////////////////////////////////////////////////////
func __tlsGetPtr(id: u64, size: u64, init: const *void)->*void
{
    ptr := TlsGetValue(cast(u32) id)
    if ptr return ptr
    ptr = @alloc(size)
    @memcpy(ptr, init, size)
    TlsSetValue(cast(u32) id, ptr)
    return ptr
}

////////////////////////////////////////////////////////////
func __panicBox(msg, title: string)
{
    result := MessageBoxA(null, @dataof(msg), @dataof(title), MB_ICONERROR | MB_CANCELTRYCONTINUE)
    switch result
    {
        case IDCANCEL:
            ExitProcess(bitcast(u32) -666)
        case IDTRYAGAIN:
            DebugBreak();
        case IDCONTINUE:
            break
    }
}

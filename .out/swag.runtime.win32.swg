#if #os == "windows";

////////////////////////////////////////////////////////////
private
{
    const STD_OUTPUT_HANDLE: u32 = cast(u32) -11
    const MB_CANCELTRYCONTINUE  = 0x00000006
    const MB_ICONERROR          = 0x00000010
    const IDCANCEL    = 2
    const IDTRYAGAIN  = 10
    const IDCONTINUE  = 11

    #[swag.foreign("kernel32")]
    {
        func GetStdHandle(nStdHandle: u32)->*void;
        func WriteFile(hFile: *void, lpBuffer: const *void, nNumberOfBytesToWrite: u32, lpNumberOfBytesWritten: *u32, lpOverlapped: *void)->u32;
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
func @assertmsg(message: string, loc: swag.CompilerSourceLocation)
{
    contextFlags := @getcontext().flags
    // If we are running bytecode, then just let the runner deal with the assertion,
    // we will have more usefull information
    if(contextFlags & swag.ContextFlags.ByteCode)
        @error(message, loc)
    // During tests, and if not in devmode, then just raise an error without
    // a dialog box
    else if((contextFlags & swag.ContextFlags.Test) && !(contextFlags & swag.ContextFlags.DevMode))
        @error(message, loc)

    // Build message
    var str: [1024] u8
    var len: u32

    if message != null
    {
        len = @countof(message)
        if(len > 512) len = 512
        @memcpy(&str[0], @dataof(message), len)
        str[len]     = "\n"'u8
        str[len + 1] = "\n"'u8
        len += 2;
    }

    // Source location
    @memcpy(&str[len], @dataof(loc.fileName), @countof(loc.fileName))
    len += @countof(loc.fileName)
    @memcpy(&str[len], @dataof(", line "), 7);
    len += 7;
    dstLine := &str[len]
    cptLine := cast(u32) (__itoa(dstLine, cast(s64) loc.lineStart + 1) - dstLine)
    len += cptLine
    @memcpy(&str[len], @dataof("\n\n"), 2)
    len += 2

    // Message to the user
    const title = "Swag Assertion Failed !"
    const info =
    @"- Press Cancel to exit
      - Press Retry to debug the application
      - Press Continue to ignore the assert
      "@

    @memcpy(&str[len], @dataof(info), @countof(info))
    len += @countof(info)

    str[len] = 0

    result := MessageBoxA(null, @dataof(str), @dataof(title), MB_ICONERROR | MB_CANCELTRYCONTINUE)
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

////////////////////////////////////////////////////////////
private const MAX_ARGUMENTS = 512
private var g_Argv: [MAX_ARGUMENTS] string
private var g_Argc = 0'u32

private func convertArgcArgv()
{
    cmdLine := GetCommandLineA()

    g_Argc = 0
    while :cmdLine
    {
        while :cmdLine == " "'u8 cmdLine += 1
        if :cmdLine == 0 break

        if :cmdLine == "\""'u8
        {
            cmdLine += 1
            start := cmdLine
            while :cmdLine && :cmdLine != "\""'u8 cmdLine += 1
            g_Argv[g_Argc] = cast(string) @mkslice(start, cast(u32) cast(u64) (cmdLine - start))
            if :cmdLine cmdLine += 1
        }
        else
        {
            start := cmdLine
            while :cmdLine && :cmdLine != " "'u8 cmdLine += 1
            g_Argv[g_Argc] = cast(string) @mkslice(start, cast(u32) cast(u64) (cmdLine - start))
        }

        g_Argc += 1
        if g_Argc == MAX_ARGUMENTS
            return
        while :cmdLine && :cmdLine == " "'u8 cmdLine += 1
    }
}

func @args()->[..] string
{
    if g_Argc == 0
        convertArgcArgv()
    return @mkslice(&g_Argv[0], g_Argc)
}

////////////////////////////////////////////////////////////
// Privates functions called by the runtime/compiler
////////////////////////////////////////////////////////////
func __loaddll(name: string)                => LoadLibraryA(@dataof(name))
func __exit()                               => ExitProcess(0)
func __tlsAlloc()                           => cast(u64) TlsAlloc()
func __tlsSetValue(id: u64, value: *void)   => TlsSetValue(cast(u32) id, value)
func __tlsGetValue(id: u64)->*void          => TlsGetValue(cast(u32) id)

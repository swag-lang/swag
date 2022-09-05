#global if #os == Swag.TargetOs.Windows
#foreignlib "kernel32"
#foreignlib "user32"
#foreignlib "ucrt"

////////////////////////////////////////////////////////////
namespace __Win32RT
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
        func RtlCopyMemory(destination: *void, source: const *void, length: uint);
        func RtlMoveMemory(destination: *void, source: const *void, length: uint);
        func RtlCompareMemory(destination, source: const *void, length: uint)->uint;
        func RtlFillMemory(destination: *void, length: uint, fill: s32);
        func GetStdHandle(nStdHandle: u32)->HANDLE;
        func WriteFile(hFile: HANDLE, lpBuffer: const *void, nNumberOfBytesToWrite: u32, lpNumberOfBytesWritten: *u32, lpOverlapped: *void)->u32;
        func ExitProcess(uExitCode: u32);
        func RaiseException(dwExceptionCode: u32, dwExceptionFlags: u32, nNumberOfArguments: u32, lpArguments: *const *void);
        func DebugBreak();
        func LoadLibraryA(name: const *u8);
        func GetCommandLineA()->const ^u8;
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
// LIBC
////////////////////////////////////////////////////////////

scopefile
{
    func sinf(val: f32)->f32;
    func cosf(val: f32)->f32;
    func tanf(val: f32)->f32;
    func sinhf(val: f32)->f32;
    func coshf(val: f32)->f32;
    func tanhf(val: f32)->f32;
    func asinf(val: f32)->f32;
    func acosf(val: f32)->f32;
    func atanf(val: f32)->f32;
    func logf(val: f32)->f32;
    func log2f(val: f32)->f32;
    func log10f(val: f32)->f32;
    func floorf(val: f32)->f32;
    func ceilf(val: f32)->f32;
    func truncf(val: f32)->f32;
    func roundf(val: f32)->f32;
    func expf(val: f32)->f32;
    func exp2f(val: f32)->f32;

    func sin(val: f64)->f64;
    func cos(val: f64)->f64;
    func tan(val: f64)->f64;
    func sinh(val: f64)->f64;
    func cosh(val: f64)->f64;
    func tanh(val: f64)->f64;
    func asin(val: f64)->f64;
    func acos(val: f64)->f64;
    func atan(val: f64)->f64;
    func log(val: f64)->f64;
    func log2(val: f64)->f64;
    func log10(val: f64)->f64;
    func floor(val: f64)->f64;
    func ceil(val: f64)->f64;
    func trunc(val: f64)->f64;
    func round(val: f64)->f64;
    func exp(val: f64)->f64;
    func exp2(val: f64)->f64;

    func powf(val1, val2: f32)->f32;
    func atan2f(val1, val2: f32)->f32;
    func pow(val1, val2: f64)->f64;
    func atan2(val1, val2: f64)->f64;

    func malloc(s: int)->^void;
    func realloc(ptr: ^void, s: int)->^void;
    func free(ptr: ^void);

    func strlen(a: const ^u8)->int;
    func strcmp(a, b: const ^u8)->s32;
}

func memset(dst: *void, value: s32, size: uint)
{
    __Win32RT.RtlFillMemory(dst, size, value)
}

func memcpy(dst: *void, src: const *void, size: uint)
{
    __Win32RT.RtlCopyMemory(dst, src, size)
}

func memmove(dst: *void, src: const *void, size: uint)
{
    __Win32RT.RtlMoveMemory(dst, src, size)
}

func memcmp(dst, src: const ^void, size: uint)->s32
{
    len := __Win32RT.RtlCompareMemory(dst, src, size)
    if len == size return 0
    p1 := cast(const ^s8) dst
    p2 := cast(const ^s8) src
    return p1[len] - p2[len]
}

////////////////////////////////////////////////////////////
func __printString(message: string)
{
    if !@dataof(message) or !@countof(message)
        return
    using __Win32RT
    len := cast(u32) @countof(message)
    wlen := WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), @dataof(message), len, null, null);
}

////////////////////////////////////////////////////////////
func __raiseException666(message: string, loc: Swag.CompilerSourceLocation)
{
    cxt := @getcontext()
    cxt.exceptionLoc = loc
    cxt.exceptionParams[0] = cast(const *void) &cxt.exceptionLoc
    cxt.exceptionParams[1] = cast(const *void) @dataof(message)
    cxt.exceptionParams[2] = cast(const *void) cast(uint) @countof(message)
    __Win32RT.RaiseException(666, 0, 3, &cxt.exceptionParams[0])
}

////////////////////////////////////////////////////////////
func __getCommandLine()                     => __Win32RT.GetCommandLineA()
func __loaddll(name: string)                => __Win32RT.LoadLibraryA(@dataof(name))
func __exit()                               => __Win32RT.ExitProcess(0)
func __tlsAlloc()                           => cast(u64) __Win32RT.TlsAlloc()
func __tlsSetValue(id: u64, value: *void)   => __Win32RT.TlsSetValue(cast(u32) id, value)
func __tlsGetValue(id: u64)->*void          => __Win32RT.TlsGetValue(cast(u32) id)

////////////////////////////////////////////////////////////
func __tlsGetPtr(id: u64, size: u64, init: const *void)->*void
{
    using __Win32RT
    ptr := TlsGetValue(cast(u32) id)
    if ptr return ptr
    ptr = @alloc(size)
    @memcpy(ptr, init, size)
    TlsSetValue(cast(u32) id, ptr)
    return ptr
}

////////////////////////////////////////////////////////////
func __exitError()
{
    __Win32RT.ExitProcess(bitcast(u32) -666)
}

func __panicBox(msg, title: string)
{
    using __Win32RT
    result := MessageBoxA(null, @dataof(msg), @dataof(title), MB_ICONERROR | MB_CANCELTRYCONTINUE)
    switch result
    {
    case IDCANCEL:
        __exitError()
    case IDTRYAGAIN:
        DebugBreak();
    case IDCONTINUE:
        break
    }
}

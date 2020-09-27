////////////////////////////////////////////////////////////
#if #os == "windows"
{
    private const STD_OUTPUT_HANDLE: u32 = cast(u32) -11
    private const MB_CANCELTRYCONTINUE  = 0x00000006
    private const MB_ICONERROR          = 0x00000010
    private const IDCANCEL    = 2
    private const IDTRYAGAIN  = 10
    private const IDCONTINUE  = 11

    #[swag.foreign("kernel32")]
    {
    private func GetStdHandle(nStdHandle: u32)->*void;
    private func WriteFile(hFile: *void, lpBuffer: const *void, nNumberOfBytesToWrite: u32, lpNumberOfBytesWritten: *u32, lpOverlapped: *void)->u32;
    private func ExitProcess(uExitCode: u32);
    private func RaiseException(dwExceptionCode: u32, dwExceptionFlags: u32, nNumberOfArguments: u32, lpArguments: const **void);
    private func MessageBoxA(hWnd: const *void, lpText: const *void, lpCaption: const *void, uType: u32)->s32;
    private func DebugBreak();
    }
}

////////////////////////////////////////////////////////////
private func itoa(result: *u8, value: s64)->*u8
{
    const conv = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"

    ptr := result
    orgValue := value
    while true
    {
        tmpValue := orgValue
        orgValue /= 10
        :ptr = conv[35 + (tmpValue - (orgValue * 10))]
        ptr += 1
        if !orgValue
            break
    }

    if value < 0
    {
        :ptr = "-"'u8
        ptr += 1
    }

    retVal := ptr
    :ptr = 0; ptr -= 1

    ptr1 := result
    while ptr1 < ptr
    {
        tmpChar := :ptr
        :ptr    = :ptr1; ptr -= 1
        :ptr1   = tmpChar; ptr1 += 1
    }

    return retVal
}

private func ftoa(result: *u8, value: f64)
{
    ipart := cast(s64) value
    fpart := value - cast(f64) ipart

    n := result
    if ipart == 0
    {
        if value < 0
        {
            :n = "-"'u8
            n += 1
        }

        :n = "0"'u8
        n += 1
    }
    else
        n = itoa(result, ipart);

    :n = "."'u8; n += 1

    afterPoint := 5
    if fpart < 0
        fpart = -fpart
    while afterPoint
    {
        fpart *= 10
        afterPoint -= 1
    }

    ipart = cast(s64) fpart
    if fpart - ipart > 0.5 ipart += 1
    itoa(n, ipart)
}

private func strlen(value: const *u8)->u32
{
    idx := 0'u32
    while value[idx] idx += 1
    return idx
}

////////////////////////////////////////////////////////////
func @memcmp(dst, src: const *void, size: u32)->s32
{
    if !size
        return 0

    p1 := cast(const *s8) dst
    p2 := cast(const *s8) src
    loop i: size
    {
        if p1[i] != p2[i]
            return p1[i] - p2[i]
    }

    return 0
}

////////////////////////////////////////////////////////////
func @strcmp(str1, str2: const *u8, num1, num2: u32)->bool
{
    if num1 != num2
        return false
    if !str1 || !str2
        return str1 == str2
    return !@memcmp(str1, str2, num1)
}

////////////////////////////////////////////////////////////
func @abs(value: f32)->f32
{
    return value < 0 ? -value : value
}

////////////////////////////////////////////////////////////
func @print(message: string)
{
    @print(@dataof(message), @countof(message))
}

func @print(value: s64)
{
    var buf: [100] u8
    itoa(&buf[0], value)
    @print(&buf[0], strlen(&buf[0]))
}

func @print(value: f64)
{
    var buf: [100] u8
    ftoa(&buf[0], value)
    @print(&buf[0], strlen(&buf[0]))
}

func @print(message: const *void, len: u32)
{
    if !message || !len
        return
    #if #os == "windows"
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), message, len, null, null);
}

////////////////////////////////////////////////////////////
func @typecmp(type1, type2: typeinfo, cmpFlags: swag.TypeCmpFlags)->bool
{
    if cast(const*u8) type1 == cast(const*u8) type2
        return true
    if !type1 && !type2
        return true
    if !type1 || !type2
        return false

    // Fine to convert from concrete to ref, or the other way
    if cmpFlags & swag.TypeCmpFlags.CastAny
    {
        if type1.kind == swag.TypeInfoKind.Reference && type2.kind != swag.TypeInfoKind.Reference
        {
            ref := cast(const *swag.TypeInfoReference) type1
            return @typecmp(ref.pointedType, type2, cmpFlags)
        }

        if type1.kind != swag.TypeInfoKind.Reference && type2.kind == swag.TypeInfoKind.Reference
        {
            ref := cast(const *swag.TypeInfoReference) type2
            return @typecmp(type1, ref.pointedType, cmpFlags)
        }
    }

    if (type1.kind != type2.kind) || (type1.sizeof != type2.sizeof) || (type1.flags != type2.flags)
        return false
    return @strcmp(@dataof(type1.name), @dataof(type2.name), @countof(type1.name), @countof(type2.name))
}

////////////////////////////////////////////////////////////
func @interfaceof(structType, itfType: const *swag.TypeInfoStruct)->const *void
{
    loop i: structType.interfaces
    {
        if @strcmp(@dataof(structType.interfaces[i].name),
                   @dataof(itfType.name),
                   @countof(structType.interfaces[i].name),
                   @countof(itfType.name))
        {
            return structType.interfaces[i].value
        }
    }

    return null
}

////////////////////////////////////////////////////////////
func @error(message: string, loc: swag.CompilerSourceLocation)
{
    contextFlags := @getcontext().flags
    if(contextFlags & swag.ContextFlags.ByteCode)
    {
        #if #os == "windows"
        {
            l := loc
            var params: [3] const *void
            params[0] = cast(const *void) &l
            params[1] = cast(const *void) @dataof(message)
            params[2] = cast(const *void) cast(u64) @countof(message)
            RaiseException(666, 0, 3, &params[0])
        }
    }

    @print("error: ")
    @print(loc.fileName)
    @print(":")
    @print(cast(s64) (loc.lineStart + 1))
    @print(": ")
    @print(message)
    @print("\n")

    #if #os == "windows"
        ExitProcess(cast(u32) -666)
}

////////////////////////////////////////////////////////////
func @assert(expr: bool, file: const *u8, line: u32, col: u32, message: const *u8)
{
    if expr return

    var loc: swag.CompilerSourceLocation
    loc.fileName = cast(string) @mkslice(file, strlen(file))
    loc.lineStart, loc.lineEnd = line
    loc.colStart, loc.colEnd = col

    var msg: string = message ? cast(string) @mkslice(message, strlen(message)) : "assertion failed"
    @assertmsg(msg, loc)
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
    cptLine := cast(u32) (itoa(dstLine, cast(s64) loc.lineStart + 1) - dstLine)
    len += cptLine
    @memcpy(&str[len], @dataof("\n\n"), 2)
    len += 2

    // Message to the user
    const title = "Swag Assertion Failed !"
    const info =
    #"- Press Cancel to exit
      - Press Retry to debug the application
      - Press Continue to ignore the assert
      "#

    @memcpy(&str[len], @dataof(info), @countof(info))
    len += @countof(info)

    str[len] = 0

    #if #os == "windows"
    {
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
}
////////////////////////////////////////////////////////////
#[swag.discardable]
#[swag.safety("", false)]
func __itoa(result: *u8, value: s64)->*u8
{
    const conv = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"

    ptr := result
    orgValue := value
    loop
    {
        tmpValue := orgValue
        orgValue /= 10
        dref ptr = conv[35 + (tmpValue - (orgValue * 10))]
        ptr += 1
        if !orgValue
            break
    }

    if value < 0
    {
        dref ptr = "-"'u8
        ptr += 1
    }

    retVal := ptr
    dref ptr = 0; ptr -= 1

    ptr1 := result
    while ptr1 < ptr
    {
        tmpChar := dref ptr
        dref ptr  = dref ptr1; ptr -= 1
        dref ptr1 = tmpChar; ptr1 += 1
    }

    return retVal
}

#[swag.discardable]
#[swag.safety("", false)]
func __utoa(result: *u8, value: u64)->*u8
{
    const conv = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"

    ptr := result
    orgValue := value
    loop
    {
        tmpValue := orgValue
        orgValue /= 10
        dref ptr = conv[35 + (tmpValue - (orgValue * 10))]
        ptr += 1
        if !orgValue
            break
    }

    retVal := ptr
    dref ptr = 0; ptr -= 1

    ptr1 := result
    while ptr1 < ptr
    {
        tmpChar := dref ptr
        dref ptr  = dref ptr1; ptr -= 1
        dref ptr1 = tmpChar; ptr1 += 1
    }

    return retVal
}

#[swag.safety("", false)]
func __ftoa(result: *u8, value: f64)
{
    ipart := cast(s64) value
    fpart := value - cast(f64) ipart

    n := result
    if ipart == 0
    {
        if value < 0
        {
            dref n = "-"'u8
            n += 1
        }

        n[0] = "0"'u8
        n += 1
    }
    else
        n = __itoa(result, ipart);

    n[0] = "."'u8
    n += 1

    const AfterFracN = 5
    afterPoint := AfterFracN
    if fpart < 0
        fpart = -fpart

    leadingZeros := 0
    while afterPoint
    {
        fpart *= 10
        if fpart < 1
            leadingZeros += 1
        afterPoint -= 1
    }

    // Zeros before the value part
    if leadingZeros == AfterFracN
        leadingZeros = 0
    while leadingZeros
    {
        n[0] = "0"'u8
        n += 1
        leadingZeros -= 1
    }

    ipart = cast(s64) fpart
    if fpart - ipart > 0.5 ipart += 1
    __itoa(n, ipart)
}

func __printU64(value: u64)
{
    var buf: [100] u8
    __utoa(&buf[0], value)
    str := cast(string) @mkslice(&buf[0], @cstrlen(&buf[0]))
    __printString(str)
}

func __printS64(value: s64)
{
    var buf: [100] u8
    __itoa(&buf[0], value)
    str := cast(string) @mkslice(&buf[0], @cstrlen(&buf[0]))
    __printString(str)
}

func __printF64(value: f64)
{
    var buf: [100] u8
    __ftoa(&buf[0], value)
    str := cast(string) @mkslice(&buf[0], @cstrlen(&buf[0]))
    __printString(str)
}

func @print(params: ...)
{
    visit v: params
    {
        ko := @kindof(v)
        switch ko
        {
        case s8:        __printS64(cast(s64) cast(s8) v)
        case s16:       __printS64(cast(s64) cast(s16) v)
        case s32:       __printS64(cast(s64) cast(s32) v)
        case s64, int:  __printS64(cast(s64) v)
        case u8:        __printU64(cast(u64) cast(u8) v)
        case u16:       __printU64(cast(u64) cast(u16) v)
        case u32:       __printU64(cast(u64) cast(u32) v)
        case rune:      __printU64(cast(u64) cast(rune) v)
        case u64, uint: __printU64(cast(u64) cast(u64) v)
        case f32:       __printF64(cast(f64) cast(f32) v)
        case f64:       __printF64(cast(f64) v)
        case string:    __printString(cast(string) v)
        case bool:      __printString(cast(bool) v ? "true" : "false")
        default:        __printString("?")
        }
    }
}

////////////////////////////////////////////////////////////
func @strcmp(str1: string, str2: string)->bool
{
    num1 := @countof(str1)
    num2 := @countof(str2)
    if num1 != num2
        return false
    if num1 == 0 and num2 == 0
        return true
    data1 := @dataof(str1)
    data2 := @dataof(str2)
    if data1 == null or data2 == null
        return data1 == data2
    return !@memcmp(data1, data2, num1)
}

////////////////////////////////////////////////////////////
private func concreteAlias(type1: typeinfo)->typeinfo
{
    if type1.kind != swag.TypeInfoKind.Alias or type1.flags & swag.TypeInfoFlags.Strict
        return type1
    typeAlias := cast(const *swag.TypeInfoAlias) type1
    return concreteAlias(typeAlias.rawType)
}

func @typecmp(typeA, typeB: typeinfo, cmpFlags: swag.TypeCmpFlags)->bool
{
    if cast(const*u8) typeA == cast(const*u8) typeB
        return true
    if !typeA and !typeB
        return true
    if !typeA or !typeB
        return false

    type1 := concreteAlias(typeA)
    type2 := concreteAlias(typeB)

    // Fine to convert from concrete to ref, or the other way
    if cmpFlags & swag.TypeCmpFlags.CastAny
    {
        if type1.kind == swag.TypeInfoKind.Reference and type2.kind != swag.TypeInfoKind.Reference
        {
            ref := cast(const *swag.TypeInfoReference) type1
            return @typecmp(ref.pointedType, type2, cmpFlags)
        }

        if type1.kind != swag.TypeInfoKind.Reference and type2.kind == swag.TypeInfoKind.Reference
        {
            ref := cast(const *swag.TypeInfoReference) type2
            return @typecmp(type1, ref.pointedType, cmpFlags)
        }
    }

    if (type1.kind != type2.kind) or (type1.sizeof != type2.sizeof) or (type1.flags != type2.flags)
        return false
    return @strcmp(type1.name, type2.name)
}

////////////////////////////////////////////////////////////
#[swag.safety("", false)]
func @interfaceof(structType, itfType: const *swag.TypeInfoStruct)->const *void
{
    loop i: structType.interfaces
    {
        if @strcmp(structType.interfaces[i].name, itfType.name)
            return structType.interfaces[i].value
    }

    return null
}


////////////////////////////////////////////////////////////
private const MAX_ARGUMENTS = 512
private var g_Argv: [MAX_ARGUMENTS] string
private var g_Argc = 0'u32

private func convertArgcArgv()
{
    cmdLine := __getCommandLine()

    g_Argc = 0
    while dref cmdLine
    {
        while dref cmdLine == " "'u8 cmdLine += 1
        if dref cmdLine == 0 break

        if dref cmdLine == "\""'u8
        {
            cmdLine += 1
            start := cmdLine
            while dref cmdLine and dref cmdLine != "\""'u8 cmdLine += 1
            g_Argv[g_Argc] = cast(string) @mkslice(start, cast(uint) (cmdLine - start))
            if dref cmdLine cmdLine += 1
        }
        else
        {
            start := cmdLine
            while dref cmdLine and dref cmdLine != " "'u8 cmdLine += 1
            g_Argv[g_Argc] = cast(string) @mkslice(start, cast(uint) (cmdLine - start))
        }

        g_Argc += 1
        if g_Argc == MAX_ARGUMENTS
            return
        while dref cmdLine and dref cmdLine == " "'u8 cmdLine += 1
    }
}

func @args()->[..] string
{
    if g_Argc == 0
        convertArgcArgv()
    return @mkslice(&g_Argv[0], cast(uint) g_Argc)
}

////////////////////////////////////////////////////////////
func @seterr(value: string)
{
    cxt := @getcontext()
    if value == null
        cxt.errorMsgLen = 0
    else
    {
        len := @countof(value)
        if len == 0
            cxt.errorMsgLen = 0
        else
        {
            if len > swag.MaxLenErrorMsg - cxt.errorMsgStart len = cast(uint) (swag.MaxLenErrorMsg - cxt.errorMsgStart)
            @assert(len > 0)
            @memcpy(&cxt.errorMsg[cxt.errorMsgStart], @dataof(value), len)
            cxt.errorMsgLen = cast(u32) len
        }
    }
}

func __geterr()->string
{
    cxt := @getcontext()
    if cxt.errorMsgLen == 0 return null
    return @mkstring(&cxt.errorMsg[cxt.errorMsgStart], cxt.errorMsgLen)
}

// Store the current error length after the string, and set
// the new cxt.errorMsgStart after that
func __pusherr()
{
    cxt := @getcontext()
    @assert(cxt.errorMsgLen != 0)
    @assert(swag.MaxLenErrorMsg - cxt.errorMsgStart >= 8)
    ptr := cast(*u32) (&cxt.errorMsg[cxt.errorMsgStart] + cxt.errorMsgLen)
    ptr[0] = cxt.errorMsgLen
    ptr[1] = cxt.traceIndex
    cxt.errorMsgStart += cxt.errorMsgLen + 8
    cxt.errorMsgLen = 0
}

// Get the previous error length from the buffer (the u32 just before
// cxt.errorMsgStart), and restore the previous error length and start
func __poperr()
{
    cxt := @getcontext()
    @assert(cxt.errorMsgStart > 8)
    ptr := cast(*u32) (&cxt.errorMsg[cxt.errorMsgStart] - 8)
    cxt.errorMsgLen = ptr[0]
    cxt.traceIndex = ptr[1]
    cxt.errorMsgStart -= (cxt.errorMsgLen + 8)
}

////////////////////////////////////////////////////////////
func __initStackTrace()
{
    cxt := @getcontext()
    cxt.traceIndex = 0
}

func __stackTrace(loc: swag.CompilerSourceLocation)
{
    cxt := @getcontext()
    if cxt.traceIndex >= swag.MaxTrace
        return
    cxt.trace[cxt.traceIndex] = loc
    cxt.traceIndex = cxt.traceIndex + 1
}


////////////////////////////////////////////////////////////
func @panic(message: string, loc: swag.CompilerSourceLocation)
{
    cxt := @getcontext()

    // User function
    if cxt.panic
    {
        cxt.panic(message, loc)
        return
    }

    contextFlags := cxt.flags

    @errormsg(message, loc)

    // If we are running bytecode, then just let the runner deal with the assertion,
    // we will have more usefull information
    if contextFlags & swag.ContextFlags.ByteCode
        return

    // During tests just raise an error without a dialog box
    if contextFlags & swag.ContextFlags.Test
        return

    // Stack trace if available
    for i := cast(s32) cxt.traceIndex - 1; i >= 0; i -= 1
    {
        @print("trace error: ");
        @print(cxt.trace[i].fileName)
        @print(":");
        @print(cast(s64) cxt.trace[i].lineStart + 1);
        @print(":");
        @print(cast(s64) cxt.trace[i].colStart + 1);
        @print(":\n");
    }

    // Build message
    var str: [1024] u8
    var len: uint

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
    const title = "Swag Panic !"
    const info =
    @"- Press Cancel to exit
      - Press Retry to debug
      - Press Continue to ignore
      "@

    @memcpy(&str[len], @dataof(info), @countof(info))
    len += @countof(info)

    str[len] = 0

    __panicBox(cast(string) str, title)
}

////////////////////////////////////////////////////////////
func __panic(file: const *u8, line: u32, col: u32, message: const *u8)
{
    var loc: swag.CompilerSourceLocation
    loc.fileName = cast(string) @mkslice(file, @cstrlen(file))
    loc.lineStart, loc.lineEnd = line
    loc.colStart, loc.colEnd = col

    var msg: string = message ? cast(string) @mkslice(message, @cstrlen(message)) : "panic"
    @panic(msg, loc)
}
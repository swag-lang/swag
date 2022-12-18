using Swag

// Special attribute that cannot be defined in the boostrap, because
// we need typeinfo to be defined
namespace Swag
{
    #[AttrUsage(AttributeUsage.All)]
    attr Using(what: typeinfo...)
}

////////////////////////////////////////////////////////////
func __strcat(start: ^u8, max: s32, src: string)
{
    dstLen := @strlen(start)
    srcLen := @countof(src)
    if dstLen + srcLen + 1 > max
        srcLen = max - (dstLen + 1)
    if !srcLen
        return
    @memcpy(start + dstLen, @dataof(src), srcLen)
    start[dstLen + srcLen] = 0
}

#[Discardable]
#[Safety("", false)]
func __itoa(result: ^u8, value: s64)->^u8
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

#[Discardable]
#[Safety("", false)]
func __utoa(result: ^u8, value: u64)->^u8
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

#[Safety("", false)]
func __ftoa(result: ^u8, value: f64)
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
    str := cast(string) @mkslice(&buf[0], @strlen(&buf[0]))
    __printString(str)
}

func __printS64(value: s64)
{
    var buf: [100] u8
    __itoa(&buf[0], value)
    str := cast(string) @mkslice(&buf[0], @strlen(&buf[0]))
    __printString(str)
}

func __printF64(value: f64)
{
    var buf: [100] u8
    __ftoa(&buf[0], value)
    str := cast(string) @mkslice(&buf[0], @strlen(&buf[0]))
    __printString(str)
}

func @print(params: ...)
{
    visit v: params
    {
        ko := @kindof(v)

        if ko.flags & TypeInfoFlags.PointerRef
        {
            ptr := cast(const *TypeInfoPointer) ko
            ko = acast ptr.pointedType
            val := cast(**uint) @dataof(v)
            v = @mkany(dref val, ko)
        }

        if ko.flags & TypeInfoFlags.PointerTypeInfo
        {
            #[Safety("cast", false)]
            ptr := cast(const *TypeInfo) v
            __printString(ptr.name)
            continue
        }

        if ko.flags & TypeInfoFlags.CString
        {
            #[Safety("cast", false)]
            ptr := cast(const *u8) v
            len := @strlen(ptr)
            __printString(@mkstring(ptr, len))
            continue
        }

        if ko.kind == .Pointer
        {
            #[Safety("cast", false)]
            __printU64(cast(u64) cast(uint) v)
            continue
        }

        switch ko
        {
        case s8:        __printS64(cast(s64) cast(s8) v)
        case s16:       __printS64(cast(s64) cast(s16) v)
        case s32:       __printS64(cast(s64) cast(s32) v)
        case s64:       __printS64(cast(s64) v)
        case int:       __printS64(cast(s64) cast(int) v)
        case u8:        __printU64(cast(u64) cast(u8) v)
        case u16:       __printU64(cast(u64) cast(u16) v)
        case u32:       __printU64(cast(u64) cast(u32) v)
        case rune:      __printU64(cast(u64) cast(rune) v)
        case u64:       __printU64(cast(u64) v)
        case uint:      __printU64(cast(u64) cast(uint) v)
        case f32:       __printF64(cast(f64) cast(f32) v)
        case f64:       __printF64(cast(f64) v)
        case string:    __printString(cast(string) v)
        case bool:      __printString(cast(bool) v ? "true" : "false")
        default:        __printString("?")
        }
    }
}

////////////////////////////////////////////////////////////
func @stringcmp(str1: string, str2: string)->bool
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
func __concreteAlias(type1: typeinfo)->typeinfo
{
    if type1.kind != TypeInfoKind.Alias or type1.flags & TypeInfoFlags.Strict
        return type1
    typeAlias := cast(const *TypeInfoAlias) type1
    return __concreteAlias(typeAlias.rawType)
}

func @typecmp(typeA, typeB: typeinfo, cmpFlags: TypeCmpFlags)->bool
{
    if cast(const*u8) typeA == cast(const*u8) typeB
        return true
    if !typeA and !typeB
        return true
    if !typeA or !typeB
        return false

    type1 := __concreteAlias(typeA)
    type2 := __concreteAlias(typeB)

    if (type1.kind != type2.kind) or (type1.sizeof != type2.sizeof) or (type1.flags != type2.flags)
        return false

    // For tuples, we compare field by field
    if type1.flags & .Tuple
    {
        type1S := cast(const *TypeInfoStruct) type1
        type2S := cast(const *TypeInfoStruct) type2
        loop i: type1S.fields
        {
            field1 := type1S.fields[i]
            field2 := type2S.fields[i]
            if field1.name != field2.name
                return false
            if !@typecmp(field1.pointedType, field2.pointedType, cmpFlags)
                return false
        }

        return true
    }

    return @stringcmp(type1.fullname, type2.fullname)
}

////////////////////////////////////////////////////////////
#[Safety("", false)]
func @itftableof(structType, itfType: const *TypeInfoStruct)->const *void
{
    loop i: structType.interfaces
    {
        if @stringcmp(structType.interfaces[i].name, itfType.fullname)
            return structType.interfaces[i].value
    }

    return null
}


////////////////////////////////////////////////////////////
const __MAX_ARGUMENTS = 512
var __argv: [__MAX_ARGUMENTS] string
var __argc = 0'u32

func __convertArgcArgv(args: const ^u8)
{
    cmdLine := args
    __argc = 0
    while dref cmdLine
    {
        while dref cmdLine == " "'u8 cmdLine += 1
        if dref cmdLine == 0 break

        if dref cmdLine == "\""'u8
        {
            cmdLine += 1
            start := cmdLine
            while dref cmdLine and dref cmdLine != "\""'u8 cmdLine += 1
            __argv[__argc] = cast(string) @mkslice(start, cast(uint) (cmdLine - start))
            if dref cmdLine cmdLine += 1
        }
        else
        {
            start := cmdLine
            while dref cmdLine and dref cmdLine != " "'u8 cmdLine += 1
            __argv[__argc] = cast(string) @mkslice(start, cast(uint) (cmdLine - start))
        }

        __argc += 1
        if __argc == __MAX_ARGUMENTS
            return
        while dref cmdLine and dref cmdLine == " "'u8 cmdLine += 1
    }
}

func @args()->const [..] string
{
    if __argc == 0
    {
        ptr := @dataof(@getpinfos().args) orelse __getCommandLine()
        __convertArgcArgv(ptr)
    }

    return @mkslice(&__argv[0], cast(uint) __argc)
}

////////////////////////////////////////////////////////////
var __debugAlloc:   DebugAllocator
var __runtimeFlags  = RuntimeFlags.Zero

func __setupRuntime(flags: RuntimeFlags)
{
    @getcontext().tempAllocator.setCapacity(@getcontext().allocator)

    __runtimeFlags = flags
    if !(__runtimeFlags & .FromCompiler) and (__runtimeFlags & .DebugAllocator)
    {
        __debugAlloc.setup(@getcontext().allocator)
        @getcontext().allocator = __debugAlloc
    }
}

func __closeRuntime()
{
    @getcontext().tempAllocator.release()

    //if !(__runtimeFlags & .FromCompiler) and (__runtimeFlags & .DebugAllocator)
    //    __debugAlloc.printLeaks()

    __exit()
}

////////////////////////////////////////////////////////////
func __seterr(value: string)
{
    cxt := @getcontext()
    len := @countof(value)
    if len > MaxLenErrorMsg - cxt.errorMsgStart
    {
        len = cast(uint) (MaxLenErrorMsg - cxt.errorMsgStart)
        @assert(len > 0)
    }
    @memcpy(&cxt.errorMsg[cxt.errorMsgStart], @dataof(value), len)
    cxt.errorMsgLen = cast(u32) len
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
    @assert(MaxLenErrorMsg - cxt.errorMsgStart >= 8)
    ptr := cast(^u32) (&cxt.errorMsg[cxt.errorMsgStart] + cxt.errorMsgLen)
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
    ptr := cast(^u32) (&cxt.errorMsg[cxt.errorMsgStart] - 8)
    cxt.errorMsgLen = ptr[0]
    cxt.traceIndex = ptr[1]
    cxt.errorMsgStart -= (cxt.errorMsgLen + 8)
}

func __checkAny(typeA, typeB: typeinfo, loc: CompilerSourceLocation)
{
    if !@typecmp(typeA, typeB, TypeCmpFlags.CastAny)
    {
        cxt := @getcontext()
        str := &cxt.errorMsg[0]
        cxt.errorMsg[0] = 0

        if !typeB
        {
            __strcat(str, Swag.MaxLenErrorMsg, "[safety] `any` value is null and cannot be casted")
            if typeA
            {
                __strcat(str, Swag.MaxLenErrorMsg, " (`")
                __strcat(str, Swag.MaxLenErrorMsg, typeA.name)
                __strcat(str, Swag.MaxLenErrorMsg, "` expected)")
            }
        }
        else
        {
            __strcat(str, Swag.MaxLenErrorMsg, "[safety] invalid dynamic cast from type `any`")
            if typeA and typeB
            {
                __strcat(str, Swag.MaxLenErrorMsg, " (`")
                __strcat(str, Swag.MaxLenErrorMsg, typeA.name)
                __strcat(str, Swag.MaxLenErrorMsg, "` expected, `")
                __strcat(str, Swag.MaxLenErrorMsg, typeB.name)
                __strcat(str, Swag.MaxLenErrorMsg, "` provided)")
            }
        }

        cxt.errorMsgLen = 0
        @panic(@mkstring(str, @strlen(str)), loc)
    }
}

////////////////////////////////////////////////////////////
func __initStackTrace()
{
    cxt := @getcontext()
    cxt.traceIndex = 0
}

func __stackTrace(loc: CompilerSourceLocation)
{
    cxt := @getcontext()
    if cxt.traceIndex >= MaxTrace
        return
    cxt.trace[cxt.traceIndex] = loc
    cxt.traceIndex = cxt.traceIndex + 1
}


////////////////////////////////////////////////////////////
func @errormsg(message: string, loc: Swag.CompilerSourceLocation)
{
    // If the compiler is around, just let it deal with the message
    cxt := @getcontext()
    contextFlags := cxt.flags
    if contextFlags & Swag.ContextFlags.ByteCode
    {
        __raiseException666(message, loc)
        return
    }

    @print("\x1b[91m")
    @print("error: ", message, "\n")
    @print("\x1b[97m")
    @print(" |  --> ")
    @print(loc.fileName, ":", loc.lineStart + 1, "\n")
}

#[Swag.Safety("", false)]
func @panic(message: string, loc: CompilerSourceLocation)
{
    cxt := @getcontext()

    // User function
    if cxt.panic
    {
        cxt.panic(message, loc)
        return
    }

    // Print the message first
    @errormsg(message, loc)

    // If we are running bytecode, then just let the runner deal with the assertion,
    // we will have more usefull information
    contextFlags := cxt.flags
    if contextFlags & ContextFlags.ByteCode
        return

    // Print the calling stack trace
    __logStackTrace()

    // Print the error stack trace if available
    for i := cast(s32) cxt.traceIndex - 1; i >= 0; i -= 1
    {
        @print("\x1b[93m")
        @print("trace error:\n");
        @print("\x1b[97m")
        @print(" |  --> ")
        @print(cxt.trace[i].fileName)
        @print(":");
        @print(cast(s64) cxt.trace[i].lineStart + 1);
        @print(":");
        @print(cast(s64) cxt.trace[i].colStart + 1);
        @print(":\n");
    }

    // During tests just raise an error without a dialog box
    if contextFlags & ContextFlags.Test
        __exitError()

    // Build message
    var str: [1024] u8
    var len: uint

    if message != null
    {
        len = @countof(message)
        if len > 512 len = 512
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
    const info = """\
      - Press Cancel to exit
      - Press Retry to debug
      - Press Continue to ignore
      """

    @memcpy(&str[len], @dataof(info), @countof(info))
    len += @countof(info)

    str[len] = 0

    __panicBox(cast(string) str, title)
}

func __panic(file: const *u8, line: u32, col: u32, message: const *u8)
{
    var loc: CompilerSourceLocation
    loc.fileName = cast(string) @mkslice(file, @strlen(file))
    loc.lineStart, loc.lineEnd = line
    loc.colStart, loc.colEnd = col

    var msg: string = message ? cast(string) @mkslice(message, @strlen(message)) : "panic"
    @panic(msg, loc)
}
using Swag

////////////////////////////////////////////////////////////

func __seterr(value: string)
{
    cxt := @getcontext()
    len := @countof(value)
    if len > MaxLenErrorMsg - cxt.errorMsgStart
    {
        len = cast(u64) (MaxLenErrorMsg - cxt.errorMsgStart)
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
    @assert(cxt.errorMsgStart >= 8)
    ptr := cast(^u32) (&cxt.errorMsg[cxt.errorMsgStart] - 8)
    cxt.errorMsgLen = ptr[0]
    cxt.traceIndex = ptr[1]
    @assert(cxt.errorMsgStart >= cxt.errorMsgLen + 8)
    cxt.errorMsgStart -= (cxt.errorMsgLen + 8)
}

func __checkAny(typeA, typeB: typeinfo, loc: SourceCodeLocation)
{
    if !@typecmp(typeA, typeB, TypeCmpFlags.CastAny)
    {
        cxt := @getcontext()
        str := &cxt.errorMsg[0]
        cxt.errorMsg[0] = 0

        if !typeB
        {
            __strcat(str, Swag.MaxLenErrorMsg, "[safety] 'any' value is null and cannot be casted")
            if typeA
            {
                __strcat(str, Swag.MaxLenErrorMsg, " ('")
                __strcat(str, Swag.MaxLenErrorMsg, typeA.name)
                __strcat(str, Swag.MaxLenErrorMsg, "' expected)")
            }
        }
        else
        {
            __strcat(str, Swag.MaxLenErrorMsg, "[safety] invalid dynamic cast from type 'any'")
            if typeA and typeB
            {
                __strcat(str, Swag.MaxLenErrorMsg, " ('")
                __strcat(str, Swag.MaxLenErrorMsg, typeA.name)
                __strcat(str, Swag.MaxLenErrorMsg, "' expected, '")
                __strcat(str, Swag.MaxLenErrorMsg, typeB.name)
                __strcat(str, Swag.MaxLenErrorMsg, "' provided)")
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

func __stackTrace(loc: SourceCodeLocation)
{
    cxt := @getcontext()
    if cxt.traceIndex >= MaxTrace
        return
    cxt.trace[cxt.traceIndex] = loc
    cxt.traceIndex = cxt.traceIndex + 1
}

////////////////////////////////////////////////////////////

func @compilererror(message: string, loc: Swag.SourceCodeLocation)
{
    // If the compiler is around, just let it deal with the message
    cxt := @getcontext()
    if cxt.flags & Swag.ContextFlags.ByteCode
    {
        __raiseException666(message, loc, Swag.ExceptionKind.Error)
        return
    }
}

func @compilerwarning(message: string, loc: Swag.SourceCodeLocation)
{
    // If the compiler is around, just let it deal with the message
    cxt := @getcontext()
    if cxt.flags & Swag.ContextFlags.ByteCode
    {
        __raiseException666(message, loc, Swag.ExceptionKind.Warning)
        return
    }
}

#[Swag.Safety("", false)]
func @panic(message: string, loc: SourceCodeLocation)
{
    cxt := @getcontext()

    // User function
    if cxt.panic
    {
        cxt.panic(message, loc)
        return
    }

    // If we are running bytecode, then just let the runner deal with the message,
    // we will have more usefull information
    contextFlags := cxt.flags
    if contextFlags & Swag.ContextFlags.ByteCode
    {
        __raiseException666(message, loc, Swag.ExceptionKind.Panic)
        return
    }

    // Print the message first
    @print("\x1b[91m")
    @print("error: ", message, "\n")
    @print("\x1b[97m")
    @print("--> ")
    @print(loc.fileName, ":", loc.lineStart + 1, "\n")

    // Print the calling stack trace
    var arr: [64] *void
    nb := __captureStack(1, arr)
    __logStackTrace(@mkslice(&arr[0], nb))

    // Print the error stack trace if available
    for i := cast(s32) cxt.traceIndex - 1; i >= 0; i -= 1
    {
        @print("\x1b[93m")
        @print("trace error:\n");
        @print("\x1b[97m")
        @print("--> ")
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
    var len: u64

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

func __panic(file: const ^u8, line: u32, col: u32, message: const ^u8)
{
    var loc: SourceCodeLocation
    loc.fileName = cast(string) @mkslice(file, @strlen(file))
    loc.lineStart, loc.lineEnd = line
    loc.colStart, loc.colEnd = col

    var msg: string = message ? cast(string) @mkslice(message, @strlen(message)) : "panic"
    @panic(msg, loc)
}

func __compilererror(file: const ^u8, line: u32, col: u32, message: const ^u8)
{
    var loc: SourceCodeLocation
    loc.fileName = cast(string) @mkslice(file, @strlen(file))
    loc.lineStart, loc.lineEnd = line
    loc.colStart, loc.colEnd = col

    var msg: string = message ? cast(string) @mkslice(message, @strlen(message)) : "compiler error"
    @compilererror(msg, loc)
}
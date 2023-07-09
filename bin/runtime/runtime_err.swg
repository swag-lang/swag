using Swag

////////////////////////////////////////////////////////////

func __seterr(value: string)
{
    var cxt = @getcontext()
    var len = @countof(value)
    if len > MaxLenErrorMsg
        len = MaxLenErrorMsg

    @assert(cxt.errorIndex < MaxErrors)
    @memcpy(&cxt.errors[cxt.errorIndex].msgBuf, @dataof(value), len)
    cxt.errors[cxt.errorIndex].msg = @mkstring(&cxt.errors[cxt.errorIndex].msgBuf, len)
    cxt.hasError = 1
}

func __geterrmsg()->string
{
    var cxt = @getcontext()
    if !cxt.hasError
        return null
    return cxt.errors[cxt.errorIndex].msg
}

// Returns the current raised [Error] or null if none
func @err()->const *Error
{
    var cxt = @getcontext()
    if !cxt.hasError
        return null
    return &cxt.errors[cxt.errorIndex]
}

func __pusherr()
{
    var cxt = @getcontext()
    @assert(cxt.errorIndex < MaxErrors)
    cxt.errors[cxt.errorIndex].pushHasError = cxt.hasError
    cxt.errors[cxt.errorIndex].pushTraceIndex = cxt.traceIndex
    cxt.errorIndex += 1
}

func __poperr()
{
    var cxt = @getcontext()
    @assert(cxt.errorIndex != 0)
    cxt.errorIndex -= 1
    @assert(cxt.errorIndex < MaxErrors)
    cxt.hasError = cxt.errors[cxt.errorIndex].pushHasError
    cxt.traceIndex = cxt.errors[cxt.errorIndex].pushTraceIndex
}

func __checkAny(typeA, typeB: typeinfo, loc: SourceCodeLocation)
{
    if !@typecmp(typeA, typeB, .CastAny)
    {
        var cxt = @getcontext()
        var str = &cxt.errors[0].msgBuf[0]
        cxt.errors[0].msgBuf[0] = 0

        if !typeB
        {
            __strcat(str, MaxLenErrorMsg, "[safety] 'any' value is null and cannot be casted")
            if typeA
            {
                __strcat(str, MaxLenErrorMsg, " ('")
                __strcat(str, MaxLenErrorMsg, typeA.name)
                __strcat(str, MaxLenErrorMsg, "' expected)")
            }
        }
        else
        {
            __strcat(str, Swag.MaxLenErrorMsg, "[safety] invalid dynamic cast from type 'any'")
            if typeA and typeB
            {
                __strcat(str, MaxLenErrorMsg, " ('")
                __strcat(str, MaxLenErrorMsg, typeA.name)
                __strcat(str, MaxLenErrorMsg, "' expected, '")
                __strcat(str, MaxLenErrorMsg, typeB.name)
                __strcat(str, MaxLenErrorMsg, "' provided)")
            }
        }

        @panic(@mkstring(str, @strlen(str)), loc)
    }
}

////////////////////////////////////////////////////////////

func __initStackTrace()
{
    var cxt = @getcontext()
    cxt.traceIndex = 0
}

func __stackTrace(loc: SourceCodeLocation)
{
    var cxt = @getcontext()
    if cxt.traceIndex >= MaxTraces
        return
    cxt.traces[cxt.traceIndex] = loc
    cxt.traceIndex += 1
}

////////////////////////////////////////////////////////////

// Raise a compiler error at the given source location
func @compilererror(message: string, loc: SourceCodeLocation)
{
    // If the compiler is around, just let it deal with the message
    var cxt = @getcontext()
    if cxt.flags & .ByteCode
    {
        __raiseException666(message, loc, .Error)
        return
    }
}

// Raise a compiler warning at the given source location
func @compilerwarning(message: string, loc: SourceCodeLocation)
{
    // If the compiler is around, just let it deal with the message
    var cxt = @getcontext()
    if cxt.flags & .ByteCode
    {
        __raiseException666(message, loc, .Warning)
        return
    }
}

// Stop the execution and panic
#[Swag.Safety("", false)]
func @panic(message: string, loc: SourceCodeLocation)
{
    var cxt = @getcontext()

    // User function
    if cxt.panic
    {
        cxt.panic(message, loc)
        return
    }

    // If we are running bytecode, then just let the runner deal with the message,
    // we will have more usefull information
    var contextFlags = cxt.flags
    if contextFlags & .ByteCode
    {
        __raiseException666(message, loc, .Panic)
        return
    }

    // Print the message first
    @print("\n");
    @print("########################", "\n")
    @print("! RUNTIME PANIC ATTACK !", "\n")
    @print("########################", "\n")
    @print("\n");

    @print("panic: ", message, "\n")
    @print("--> ")
    @print(loc.fileName, ":", loc.lineStart + 1, ":", loc.colStart + 1, "\n")
    @print("\n")

    // Print the error stack trace if available
    if cxt.traceIndex
    {
        @print("[error callstack]", "\n");
        for var i = cast(s32) cxt.traceIndex - 1; i >= 0; i -= 1
        {
            @print("error");
            @print(" --> ")
            @print(cxt.traces[i].fileName)
            @print(":");
            @print(cast(s64) cxt.traces[i].lineStart + 1);
            @print(":");
            @print(cast(s64) cxt.traces[i].colStart + 1);
            @print(":\n");
        }

        @print("\n")
    }

    // Print the calling stack trace
    @print("[runtime callstack]", "\n");
    var arr: [64] *void
    var nb = __captureStack(1, arr)
    __logStackTrace(@mkslice(&arr[0], nb))
    @print("\n")

    // During tests just raise an error without a dialog box
    if contextFlags & .Test
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
    var dstLine = &str[len]
    var cptLine = cast(u32) (__itoa(dstLine, cast(s64) loc.lineStart + 1) - dstLine)
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
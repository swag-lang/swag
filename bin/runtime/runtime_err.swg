using Swag

////////////////////////////////////////////////////////////

func __dropErr(err: any)
{
    if err == null do
        return
    let data = @dataof(err)
    let type = cast(const *Swag.TypeInfoStruct) @kindof(err)
    if type.opDrop do
        type.opDrop(data)
}

func __setErr(err: any)
{
    let cxt = @getcontext()
    if cxt.errorIndex >= MaxErrors do
        @panic("too many nested error levels!", #curlocation)

    __dropErr(cxt.errors[cxt.errorIndex].value)

    // We check that the contextual error allocator has been initialized.
    // In case an error is raised at a very very early stage (in #init)
    if !cxt.errorAllocator.capacity do
        cxt.errorAllocator.setCapacity(cast() __buildCfg.errorAllocatorCapacity, cxt.allocator)

    let data = @dataof(err)
    let type = cast(const *Swag.TypeInfoStruct) @kindof(err)
    let ptr  = cxt.errorAllocator.alloc(cast(u64) type.sizeof)
    @memcpy(ptr, data, cast(u64) type.sizeof)
    if type.opPostCopy do
        type.opPostCopy(ptr)
    cxt.errors[cxt.errorIndex].value = @mkany(ptr, type)
    cxt.hasError                     = 1
}

func __pushErr()
{
    let cxt = @getcontext()
    cxt.errors[cxt.errorIndex].pushTraceIndex = cast(u32) cxt.traceIndex
    cxt.errors[cxt.errorIndex].pushUsedAlloc  = cast(u32) cxt.errorAllocator.used
    cxt.errors[cxt.errorIndex].pushHasError   = cxt.hasError

    cxt.hasError = 0
    cxt.errorIndex += 1
}

func __popErr()
{
    let cxt = @getcontext()
    __dropErr(cxt.errors[cxt.errorIndex].value)
    cxt.errors[cxt.errorIndex].value = null

    cxt.errorIndex -= 1

    cxt.hasError            = cxt.errors[cxt.errorIndex].pushHasError
    cxt.curError            = cxt.errors[cxt.errorIndex].value
    cxt.traceIndex          = cxt.errors[cxt.errorIndex].pushTraceIndex
    cxt.errorAllocator.used = cxt.errors[cxt.errorIndex].pushUsedAlloc
}

func __catchErr()
{
    let cxt = @getcontext()
    if !cxt.hasError do
        cxt.curError = null
    else do
        cxt.curError = cxt.errors[cxt.errorIndex].value
    cxt.errorAllocator.used = cxt.errorIndex ? cxt.errors[cxt.errorIndex - 1].pushUsedAlloc : 0
    cxt.hasError            = 0
}

// Returns the current raised error, or null if none
#[Swag.Inline]
func @err()->any
{
    let cxt = @getcontext()
    return cxt.curError
}

func __failedAssume(loc: SourceCodeLocation)
{
    let cxt  = @getcontext()
    let type = @kindof(cxt.errors[cxt.errorIndex].value)

    var str: [512] u8
    var len: u64

    const U = "uncatched runtime error "
    @memcpy(&str[len], @dataof(U), @countof(U))
    len += @countof(U)
    var maxLen = @min(@countof(type.fullname), 128)
    str[len] = '['
    len += 1
    str[len] = '['
    len += 1
    @memcpy(&str[len], @dataof(type.fullname), maxLen)
    len += maxLen
    str[len] = ']'
    len += 1
    str[len] = ']'
    len += 1

    if type.kind == TypeInfoKind.Struct
    {
        let typeStruct = cast(const *Swag.TypeInfoStruct) type
        if (@countof(typeStruct.fields) > 0 and typeStruct.fields[0].pointedType == Swag.BaseError) or
           (type == Swag.BaseError)
        {
            let data = @dataof(cxt.errors[cxt.errorIndex].value)
            let val  = cast(const *Swag.BaseError) data
            maxLen = @min(@countof(val.message), 128)
            if maxLen
            {
                str[len] = ' '
                len += 1
                str[len] = '$'
                len += 1
                str[len] = ' '
                len += 1
                @memcpy(&str[len], @dataof(val.message), maxLen)
                len += maxLen
            }
        }
    }

    cxt.hasError = 0
    @assert(cxt.errorIndex == 0)

    str[len] = 0
    @panic(@mkstring(&str[0], len), loc)
}

////////////////////////////////////////////////////////////

func __stackTrace(loc: SourceCodeLocation)
{
    let cxt = @getcontext()
    if cxt.traceIndex >= MaxTraces do
        return
    cxt.traces[cxt.traceIndex] = loc
    cxt.traceIndex += 1
}

////////////////////////////////////////////////////////////

// Raise a compiler error at the given source location
func @compilererror(message: string, loc: SourceCodeLocation)
{
    // If the compiler is around, just let it deal with the message
    let cxt = @getcontext()
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
    let cxt = @getcontext()
    if cxt.flags & .ByteCode
    {
        __raiseException666(message, loc, .Warning)
        return
    }
}

// Stop the execution and panic
#[Swag.Safety(.All, .All, false)]
func @panic(message: string, loc: SourceCodeLocation)
{
    let cxt = @getcontext()

    // User function
    if cxt.panic
    {
        cxt.panic(message, loc)
        return
    }

    // If we are running bytecode, then just let the runner deal with the message,
    // we will have more usefull information
    let contextFlags = cxt.flags
    if contextFlags & .ByteCode
    {
        __raiseException666(message, loc, .Panic)
        return
    }

    // Print the message first
    @print("\n")
    @print("########################", "\n")
    @print("! RUNTIME PANIC ATTACK !", "\n")
    @print("########################", "\n")
    @print("\n")

    @print("panic: ", message, "\n")
    @print("--> ")
    @print(loc.fileName, ":", loc.lineStart + 1, ":", loc.colStart + 1, "\n")
    @print("\n")

    // Print the error stack trace if available
    if cxt.traceIndex
    {
        @print("[error callstack]", "\n")
        for var i = cast(s32) cxt.traceIndex - 1; i >= 0; i -= 1
        {
            @print("error")
            @print(" --> ")
            @print(cxt.traces[i].fileName)
            @print(":")
            @print(cast(s64) cxt.traces[i].lineStart + 1)
            @print(":")
            @print(cast(s64) cxt.traces[i].colStart + 1)
            @print(":\n")
        }

        @print("\n")
    }

    // Print the calling stack trace
    @print("[runtime callstack]", "\n")
    var arr: [64] *void
    let nb   = __captureStack(1, arr)
    __logStackTrace(@mkslice(&arr[0], nb))
    @print("\n")

    // During tests just raise an error without a dialog box
    if contextFlags & .Test do
        __exitError()

    // Build message
    var str: [1024] u8
    var len: u64

    if message != null
    {
        len = @countof(message)
        if len > 512 do
            len = 512
        @memcpy(&str[0], @dataof(message), len)
        str[len]     = '\n'
        str[len + 1] = '\n'
        len += 2
    }

    // Source location
    @memcpy(&str[len], @dataof(loc.fileName), @countof(loc.fileName))
    len += @countof(loc.fileName)
    @memcpy(&str[len], @dataof(", line "), 7)
    len += 7
    let dstLine = &str[len]
    let cptLine = cast(u32) (__itoa(dstLine, cast(s64) loc.lineStart + 1) - dstLine)
    len += cptLine
    @memcpy(&str[len], @dataof("\n\n"), 2)
    len += 2

    // Message to the user
    const title = "Swag Panic !"
    const info  = """- Press Cancel to exit
- Press Retry to debug
- Press Continue to ignore
"""

    @memcpy(&str[len], @dataof(info), @countof(info))
    len += @countof(info)

    str[len] = 0

    __panicBox(cast(string) str, title)
}

func __panic(file: const [*] u8, line: u32, col: u32, message: const [*] u8)
{
    var loc: SourceCodeLocation
    loc.fileName = @mkstring(file, @strlen(file))
    loc.lineStart, loc.lineEnd = line
    loc.colStart, loc.colEnd = col

    let msg = message ? @mkstring(message, @strlen(message)) : "panic"
    @panic(msg, loc)
}

func __compilerError(file: const [*] u8, line: u32, col: u32, message: const [*] u8)
{
    var loc: SourceCodeLocation
    loc.fileName = @mkstring(file, @strlen(file))
    loc.lineStart, loc.lineEnd = line
    loc.colStart, loc.colEnd = col

    let msg = message ? @mkstring(message, @strlen(message)) : "compiler error"
    @compilererror(msg, loc)
}

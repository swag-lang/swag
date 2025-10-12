/**
# 'Errors with throw/try/catch'

A function marked with 'throw' can return an error by using 'throw' followed
by an error value. The value is a struct that carries error details.

These are not traditional exceptions. Think of 'throw' as a specialized
'return' that carries an error value.
*/

/**
# 'Using throw with Custom Errors'
A function that may return an error must be annotated with 'throw'. When an
error is raised, it is a structured value (often a custom error struct).
*/

// Custom error extending the runtime base error.
struct MyError
{
    using base: Swag.BaseError
}

/**
When a function exits early due to 'throw', the *returned value* is the
default for the function return type (e.g., 0 for integers).
*/

// 'count' returns the length of 'name', or throws if 'name' is null.
// On error, it returns the default 'u64' value (0).
func count(name: string)->u64 throw
{
    if name == null
    {
        throw MyError{"null pointer"}
    }
    return @countof(name)
}

/**
# 'Handling Errors with catch and @err'
Use 'catch' at the call site to dismiss a raised error. The result becomes
the default value of the callee's return type. Check '@err' to see what was
caught (type 'any').
*/
func myFunc()
{
    let cpt = catch count("fileName")

    if @err != null
    {
        // '@err' is an 'any' representing the caught error.
        @assert(@err == MyError)
        @assert(cpt == 0)
        @print("An error was raised")
        return
    }

    // No error: continue normally.
}

/**
# 'Error Handling with trycatch'
'trycatch' dismisses the error and continues, returning the default value.
This contains errors within the current function.
*/
func myOtherFunc()
{
    var cpt1 = trycatch count("fileName")

    // Equivalent pattern using 'catch':
    var cpt2 = catch count("filename")
    if @err != null do
        return
}

/**
# 'Propagating Errors with try'
'try' halts execution and propagates the error to the caller. The callee
must be annotated with 'throw'.
*/
func myFunc1() throw
{
    var cpt = try count("filename") // Propagate on error
}

/**
Equivalent to catching and re-throwing the error explicitly.
*/
func myFunc2() throw
{
    var cpt = catch count("filename")
    if @err != null
    {
        throw @err
    }
}

/**
# 'Forcing Panic with assume'
'assume' forces a panic if an error occurs, instead of handling or propagating.
Useful when errors are not expected.
*/

/**
> NOTE:
> In 'release' builds, 'assume' may be disabled, which can lead to undefined
> behavior if an error occurs.
*/
func myFunc3()
{
    var cpt = assume count("filename") // Panic on error
}

/**
WARNING:
If an error is not caught, Swag will panic at runtime. The top-level caller
assumes safe execution and halts the program on unhandled errors.
*/

/**
# 'Blocks in Error Handling'
Blocks can be used with 'try', 'assume', 'catch', and 'trycatch' to apply
the handling strategy to multiple operations. These blocks do not create a
new scope.
*/
func myFunc4() throw
{
    // Propagate errors from several calls.
    try
    {
        var cpt0 = count("filename")
        var cpt1 = count("other filename")
    }

    // Panic if any call fails.
    assume
    {
        var cpt2 = count("filename")
        var cpt3 = count("other filename")
    }

    // Dismiss errors and continue (checking '@err' is irrelevant here).
    catch
    {
        var cpt4 = count("filename")
        var cpt5 = count("other filename")
    }

    // Dismiss errors and exit immediately without propagation.
    trycatch
    {
        var cpt6 = count("filename")
        var cpt7 = count("other filename")
    }
}

/**
# 'Implicit try'
Inside a function annotated with 'throw', calls to throwing functions are
implicitly treated as 'try' unless overridden.
*/
#test
{
    func mySubFunc2() throw
    {
        throw MyError{"error from mySubFunc2"}
    }

    func mySubFunc3() throw
    {
        throw MyError{"error from mySubFunc3"}
    }

    func mySubFunc1() throw
    {
        // Implicit 'try' because this function itself is 'throw'.
        mySubFunc2()

        // Still allowed to be explicit when desired.
        try mySubFunc3()
    }

    catch mySubFunc1()
    @assert(@err == MyError)
}

/**
# 'The error struct'
An error is a struct. You can add custom fields (e.g., line/column for a
syntax error).
*/
struct SyntaxError
{
    using base:     Swag.BaseError
    line, col:      u32
}

/**
WARNING:
Ensure references stored in an error (e.g., "string", "any") remain valid
for the error lifetime. Prefer heap or a context allocator when needed.
*/

/**
# 'Using defer for Controlled Cleanup'
'defer' schedules code to run on function exit (normal return or error). Since
'throw' behaves like 'return', 'defer' runs consistently in both cases.

You can filter execution with modes:
- 'defer #err'    -> run only when an error is raised
- 'defer #noerr'  -> run only when no error is raised
- 'defer'         -> run always
*/
var g_Defer = 0

func raiseError() throw
{
    throw MyError{"error"}
}

func testDefer(err: bool) throw
{
    defer #err g_Defer += 1 // Run on error only
    defer #noerr g_Defer += 2 // Run on success only
    defer g_Defer += 3 // Run always

    if err do
        raiseError()
}

#test
{
    // Error path: only '#err' and unconditional run.
    g_Defer = 0
    catch testDefer(true)
    @assert(g_Defer == 4) // 1 + 3

    // Success path: only '#noerr' and unconditional run.
    g_Defer = 0
    catch testDefer(false)
    @assert(g_Defer == 5) // 2 + 3
}

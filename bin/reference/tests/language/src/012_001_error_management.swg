/**
A function marked with 'throw' indicates that it can return an error by invoking the 'throw' keyword, followed by an error value. The error value is a struct, which encapsulates the error details. If an error occurs, the caller has the option to either halt execution and propagate the error using 'try', or handle the error explicitly using the '@err()' intrinsic.

> It's crucial to understand that these are **not** exceptions in the traditional sense. Consider 'throw' as a specialized form of 'return' that carries an error value.

# throw
A function capable of returning an error must be annotated with 'throw'. This enables the function to raise an error using the 'throw' keyword, passing an error value structured as a custom-defined struct.
*/

// Defines a custom error type by extending the default runtime base error.
struct MyError
{
    // The default runtime base error includes fields such as a 'message'.
    using base: Swag.BaseError
}

/**
When a function returns due to an error, the actual return value will always be the **default value** for the return type.
*/

// The 'count()' function may raise an error, so 'throw' is added at the end of its signature.
func count(name: string)->u64 throw
{
    if name == null
    {
        // Throwing a 'MyError' initialized with a compile-time string.
        // The return value will be 0, the default for 'u64'.
        throw MyError{"null pointer"}  // Error: null pointer
    }

    return @countof(name)  // Return the count of characters in the string
}

/**
# catch
The caller must handle the error appropriately.

The error can be 'caught' using the 'catch' keyword, and the value can be tested (or not) using the '@err()' intrinsic. If caught, the error is dismissed, and execution continues from the call site.
*/
func myFunc()
{
    // Dismiss any potential error using 'catch' and proceed with execution.
    let cpt = catch count("fileName")

    // Test the error using '@err()'. This should be done immediately after 'catch' to ensure
    // proper error handling.
    if @err() != null
    {
        // '@err()' returns an 'any', allowing direct type comparison with the error type.
        @assert(@err() == MyError)  // Ensure the error is of type 'MyError'

        // Verify that 'count()' returned the default value, which is 0.
        @assert(cpt == 0)

        @print("An error was raised")  // Inform the user that an error was encountered.
        return
    }
}

/**
# trycatch
Alternatively, the 'trycatch' construct can be used to dismiss the error and exit the current function, returning the default value if needed. This approach ensures that no error is propagated to the caller.
*/
func myOtherFunc()
{
    // If 'count()' throws an error, exit the function silently.
    var cpt1 = trycatch count("fileName")

    // Equivalent to the following block:
    var cpt2 = catch count("filename")
    if @err() != null:
        return  // Exit if an error is caught
}

/**
# try
The caller can also halt execution with 'try', propagating the error to its caller. The function must also be annotated with 'throw'.

In this example, the caller of 'myFunc1' will need to handle the error.
*/
func myFunc1() throw
{
    // If 'count()' raises an error, 'myFunc1' will return with the same error.
    var cpt = try count("filename")  // Propagate the error if one occurs
}

/**
This is equivalent to:
*/
func myFunc2() throw
{
    // If 'count()' raises an error, 'myFunc2' will return with the same error.
    var cpt = catch count("filename")
    if @err() != null:
        throw @err()  // Re-throw the caught error
}

/**
# assume
The caller can choose to panic on error using 'assume'.

> Note: This behavior can be disabled in release builds, leading to undefined behavior.
*/
func myFunc3()
{
    // The program will panic with an error message if 'count()' throws an error.
    var cpt = assume count("filename")  // Panic on error
}

/**
> If an error is not caught, Swag will panic at runtime, as the top-level caller always assumes a safe execution.

## Implicit assume
You can annotate the entire function with 'assume' instead of 'throw'. This treats the function as one large block where errors cause an automatic panic.
*/
func myFunc3A() assume
{
    // The program will panic if 'count()' throws an error.
    var cpt = count("filename")  // Implicit 'assume'

    // Repeated logic for another filename.
    var cpt1 = count("filename")  // Implicit 'assume'
}

/**
# Blocks
Blocks can be used instead of a single statement. This does not create a new scope but groups operations.
*/
func myFunc4() throw
{
    // Demonstrating 'try' with a block instead of a single call.
    try
    {
        var cpt0 = count("filename")
        var cpt1 = count("other filename")
    }

    assume
    {
        var cpt2 = count("filename")
        var cpt3 = count("other filename")
    }

    // Using 'catch' to dismiss errors without handling the error message.
    // In this case, '@err()' is irrelevant.
    catch
    {
        var cpt4 = count("filename")
        var cpt5 = count("other filename")
    }

    // Using 'trycatch' to exit immediately if an error is raised, without handling the error message.
    trycatch
    {
        var cpt6 = count("filename")
        var cpt7 = count("other filename")
    }
}

/**
# Implicit try
When a function is annotated with 'throw', the 'try' for function calls is implicit unless explicitly stated. This reduces verbosity when error handling is not required in specific scenarios.
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
        // No explicit 'try' needed before the call, as 'mySubFunc1' is marked with 'throw'.
        // This reduces verbosity when no specific error handling is required.
        mySubFunc2()  // Implicit 'try'
        try mySubFunc3()  // Explicit 'try'
    }

    catch mySubFunc1()
    @assert(@err() == MyError)  // Confirm the error is of type 'MyError'
}

/**
# The error struct
As discussed, the error value is a struct. This allows you to add specific error parameters, such as line and column numbers for syntax errors.
*/

struct SyntaxError
{
    using base:     Swag.BaseError
    line, col:      u32
}

/**
Note: Ensure that references to external values (e.g., 'string', 'any') remain valid throughout the error's lifecycle. The runtime will manage complex types, so it's recommended to store such values in the heap or a dedicated allocator within the current context.
*/

/**
# defer
Throwing an error is functionally equivalent to returning. Therefore, the 'defer' expression behaves similarly in both cases.

'defer' can be customized with a specific mode ('err' or 'noerr') to determine its execution based on the error state.

| `defer(err)`   | Executes when an error is raised via 'throw'
| `defer(noerr)` | Executes when the function returns normally without errors
| `defer`        | Executes regardless of how the function exits
*/
var g_Defer = 0

func raiseError() throw
{
    throw MyError{"error"}  // Raise a custom error
}

func testDefer(err: bool) throw
{
    defer(err) g_Defer += 1  // Increments if an error is raised
    defer(noerr) g_Defer += 2  // Increments if no error occurs
    defer g_Defer += 3  // Increments regardless of error state
    if err:
        raiseError()  // Raise an error if 'err' is true
}

#test
{
    g_Defer = 0
    catch testDefer(true)
    @assert(g_Defer == 4)  // Only 'defer(err)' and normal 'defer' executed

    g_Defer = 0
    catch testDefer(false)
    @assert(g_Defer == 5)  // Only 'defer(noerr)' and normal 'defer' executed
}

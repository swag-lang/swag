/**
A function marked with 'throw' indicates that it can return an error by invoking the 'throw' keyword, followed by an error value. The error value is a struct, which encapsulates the error details. If an error occurs, the caller has the option to either halt execution and propagate the error using 'try', or handle the error explicitly using the '@err' intrinsic.

> It's crucial to understand that these are **not** exceptions in the traditional sense. Consider 'throw' as a specialized form of 'return' that carries an error value.
*/

/**
# Using `throw` with Custom Errors
A function that has the potential to return an error must be annotated with the 'throw' keyword. This annotation signals that the function can raise an error using the 'throw' statement. When an error is thrown, it is represented as a structured value, typically using a custom-defined struct to encapsulate the error information.
*/

// Here, we define a custom error type named `MyError`. This custom error extends a default runtime base error provided by the language.
struct MyError
{
    // The custom error inherits from the base error type `Swag.BaseError`.
    // The base error type includes common fields such as an error 'message', which can be useful for debugging or error reporting.
    using base: Swag.BaseError
}

/**
When a function encounters an error and exits early, the actual return value is not what the function was originally intended to return. Instead, the function returns the **default value** for its return type. For example, if the return type is an integer, the default value would typically be 0.
*/

// The function 'count()' is defined to take a string parameter named 'name' and return an unsigned 64-bit integer (u64).
// The 'throw' keyword at the end of the function's signature indicates that this function can potentially raise an error.
func count(name: string)->u64 throw
{
    // Inside the function, we first check if the input parameter 'name' is null (i.e., it has no value assigned to it).
    if name == null
    {
        // If 'name' is null, we raise (throw) an error of type `MyError`.
        // The error is initialized with a string message "null pointer" to describe the problem.
        // When an error is thrown, the function will not proceed to return the intended value.
        // Instead, it will return 0, which is the default value for the u64 type.
        throw MyError{"null pointer"} // Error is thrown with the message: "null pointer"
    }

    // If 'name' is not null, the function proceeds to count the number of characters in the string.
    // The '@countof(name)' function is used to determine this count, and the result is returned.
    return @countof(name) // Return the count of characters in the string
}

/**
# Handling Errors with `catch` and `@err`
When calling a function that can throw an error, it's essential to handle the error appropriately. This is where the `catch` keyword comes into play. The error can be 'caught' using `catch`, and its value can be tested (or ignored) using the `@err` intrinsic. If an error is caught, it is dismissed, allowing the program to continue execution from the point where the error was handled.
*/
func myFunc()
{
    // Attempt to call the `count()` function. If `count()` throws an error,
    // `catch` will intercept it. The function will then return the default value
    // of the return type (in this case, 0) and assign it to `cpt`.
    let cpt = catch count("fileName")

    // Immediately after catching the error, use `@err` to check if an error occurred.
    if @err != null
    {
        // `@err` returns an 'any' type, representing the caught error.
        // Here, we assert that the error is of type `MyError` to ensure correct error handling.
        @assert(@err == MyError) // Validate that the error is indeed a `MyError`.

        // Also, assert that the value returned by `count()` is the default value (0 in this case),
        // which happens when an error is thrown.
        @assert(cpt == 0)

        // Inform the user that an error was encountered during the execution.
        @print("An error was raised")
        return // Exit the function early since an error was handled.
    }

    // If no error was caught, the function would proceed normally from this point.
}

/**
# Error Handling with `trycatch`
The `trycatch` construct provides an alternative way to handle errors. It allows the function to dismiss the error and exit gracefully, returning the default value if necessary. This approach ensures that no error is propagated to the caller, effectively containing the error within the current function.
*/
func myOtherFunc()
{
    // Attempt to call the `count()` function. If it throws an error,
    // `trycatch` catches the error, assigns the default value to `cpt1`,
    // and the function continues without propagating the error.
    var cpt1 = trycatch count("fileName")

    // The above `trycatch` block is equivalent to using `catch` followed by an error check:
    var cpt2 = catch count("filename")
    if @err != null do
        return // If an error is caught, exit the function silently
}

/**
# Propagating Errors with `try`
The `try` keyword allows a function to halt its execution and propagate any encountered errors to its caller. The function using `try` must be annotated with `throw`, indicating that it can pass errors up the call stack.

In this example, if the function `myFunc1` encounters an error, it will propagate that error to its caller, requiring the caller to handle it.
*/
func myFunc1() throw
{
    // Attempt to call the `count()` function. If it raises an error,
    // `try` will automatically propagate the error to the caller of `myFunc1`.
    var cpt = try count("filename") // Propagate the error if one occurs
}

/**
This behavior is equivalent to the following code, which uses `catch` to catch the error and then explicitly re-throws it.
*/
func myFunc2() throw
{
    // Call `count()` and catch any error that might be raised.
    var cpt = catch count("filename")
    if @err != null
    {
        // If an error is caught, re-throw the same error, propagating it to the caller.
        throw @err // Re-throw the caught error
    }
}

/**
# Forcing Panic with `assume`
The `assume` keyword allows the caller to force a panic if an error occurs, rather than handling or propagating the error. This is useful in scenarios where you expect the code to run without errors, and any error should result in an immediate halt.

> NOTE:
> In 'release' builds, the `assume` behavior can be disabled, which may lead to undefined behavior if an error occurs.
*/
func myFunc3()
{
    // If the `count()` function throws an error, the program will panic and terminate,
    // displaying an error message. The error is not propagated or handled.
    var cpt = assume count("filename") // Panic on error
}

/**
> WARNING:
> If an error is not caught, Swag will **panic** at runtime. This is because the top-level caller always assumes safe execution, ensuring that unhandled errors result in a program halt.
*/

/**
# Blocks in Error Handling
Blocks can be used in place of a single statement to group multiple operations together in error-handling constructs like `try`, `assume`, `catch`, and `trycatch`. These blocks do not create a new scope but allow you to execute several related operations under a single error-handling strategy.
*/
func myFunc4() throw
{
    // Using 'try' with a block to propagate errors from multiple operations.
    try
    {
        var cpt0 = count("filename")
        var cpt1 = count("other filename")
    }

    // Using 'assume' with a block to enforce a panic on any error occurring in the block.
    assume
    {
        var cpt2 = count("filename")
        var cpt3 = count("other filename")
    }

    // Using 'catch' with a block to catch and dismiss errors, proceeding with execution without handling or propagating the error.
    // In this context, checking '@err' is irrelevant as the errors are dismissed.
    catch
    {
        var cpt4 = count("filename")
        var cpt5 = count("other filename")
    }

    // Using 'trycatch' with a block to catch errors and exit immediately, without propagating or handling the error message.
    trycatch
    {
        var cpt6 = count("filename")
        var cpt7 = count("other filename")
    }
}

/**
# Implicit `try`
When a function is annotated with `throw`, any function calls within it that can raise errors will implicitly use `try` unless explicitly overridden. This implicit behavior reduces verbosity in scenarios where specific error handling is not required.
*/
#test
{
    // Define a function 'mySubFunc2' that throws an error.
    func mySubFunc2() throw
    {
        throw MyError{"error from mySubFunc2"} // Raise a custom error
    }

    // Define another function 'mySubFunc3' that also throws an error.
    func mySubFunc3() throw
    {
        throw MyError{"error from mySubFunc3"} // Raise a custom error
    }

    // Define 'mySubFunc1', which is marked with 'throw' indicating it can propagate errors.
    func mySubFunc1() throw
    {
        // When calling 'mySubFunc2()', there's no need for an explicit 'try' since 'mySubFunc1'
        // is marked with 'throw'. The 'try' is implicit, reducing the need for additional syntax.
        mySubFunc2() // Implicit 'try'

        // However, you can still use an explicit 'try' if specific error handling is desired.
        try mySubFunc3() // Explicit 'try'
    }

    // Catch the error raised by 'mySubFunc1' and confirm that it is of type 'MyError'.
    catch mySubFunc1()
    @assert(@err == MyError) // Ensure the error is of type 'MyError'
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
> WARNING:
> Ensure that references to external values (e.g., 'string', 'any') remain valid throughout the error's lifecycle. The runtime will manage complex types, so it's recommended to store such values in the heap or a dedicated allocator within the current context.
*/

/**
# Using `defer` for Controlled Cleanup
The `defer` statement schedules a block of code to be executed when the function exits, whether it's through a normal return or due to an error being thrown. Since throwing an error is functionally similar to returning, `defer` behaves consistently in both cases.

`defer` can be customized with specific modes (`#err` or `#noerr`) to control its execution based on the function's exit state:

| `defer #err`   | Executes only when an error is raised via `throw`.
| `defer #noerr` | Executes only when the function returns normally without errors.
| `defer`        | Executes regardless of how the function exits (either by returning normally or by throwing an error).
*/
var g_Defer = 0 // A global variable to track the execution of defer statements

// A function that raises an error based on the provided input
func raiseError() throw
{
    throw MyError{"error"} // Raise a custom error
}

// A function that demonstrates the use of defer with different modes
func testDefer(err: bool) throw
{
    // Schedules this block to execute only if an error is raised.
    defer #err g_Defer += 1 // Increment if an error occurs

    // Schedules this block to execute only if the function exits without an error.
    defer #noerr g_Defer += 2 // Increment if no error occurs

    // Schedules this block to execute regardless of whether an error occurs.
    defer g_Defer += 3 // Increment regardless of error state

    if err do
        raiseError() // If 'err' is true, raise an error
}

#test
{
    // Test case where an error is raised
    g_Defer = 0
    catch testDefer(true) // Execute the function with error condition
    @assert(g_Defer == 4) // Expect g_Defer to be 4 (1 + 3) since only 'defer #err' and the general 'defer' executed

    // Test case where no error is raised
    g_Defer = 0
    catch testDefer(false) // Execute the function without error condition
    @assert(g_Defer == 5) // Expect g_Defer to be 5 (2 + 3) since only 'defer #noerr' and the general 'defer' executed
}

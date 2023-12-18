/**
In a few words, a function marked with 'throw' can return an error by calling 'throw' followed by the error value. An error value is a struct. If an error has been raised, a caller can either stop its execution and return that same error with 'try', or it can 'catch' the error and deal with it with a dedicated intrinsic '@err()'.

So `throw Error{}` is equivalent to a return, and every rules when leaving a function are the same (call of 'defer', variables drop and so on).

> These are **not** exceptions ! You should consider 'throw' as a special 'return', with a specific value.

# throw
A function capable of returning an error must be annotated with 'throw'. This allows the function to raise an error with the same `throw` keyword, passing an error value in the form of a struct.
*/

// Defines one error.
struct MyError
{
    // The default runtime base error contains some default fields, like a 'message'
    using base: Swag.BaseError
}

/**
Note that when a function returns because of an error, the real return value will always be equal to the **default value** depending on the type.
*/

// The function 'count()' can raise an error, so we annotate it with 'throw', at the end of its signature.
func count(name: string)->u64 throw
{
    if name == null
    {
        // Throw a 'MyError' error, and initialize it with a compile-time string.
        // 'count()' will also return 0, because this is the default value for 'u64'.
        throw MyError{"null pointer"}
    }

    return @countof(name)
}

/**
# catch
The caller will then have to deal with the error in some way.

It can `catch` it, and test (or not) its value with the `@err()` intrinsic. In that case, the error is dismissed, and the execution will continue at the call site.
*/
func myFunc()
{
    // Dismiss the eventual error with 'catch' and continue execution
    let cpt = catch count("fileName")

    // We can test it with '@err()', which returns the 'throw' corresponding value as long as
    // another error is not raised. Consider this is a good idea to test it right
    // after the 'catch'.
    if @err() != null
    {
        // '@err()' returns an 'any', so you can test the type of the error directly with its name
        @assert(@err() == MyError)

        // The function 'count()' should have return the default value, so 0
        @assert(cpt == 0)

        @print("an error was raised")
        return
    }
}

/**
# trycatch
Instead of 'catch', you can use 'trycatch', which will dismiss the error and exit the current function, returning the default value if necessary. For the caller, no error has been raised.
*/
func myOtherFunc()
{
    // If count() throws an error, we just exit the function without further notice.
    var cpt1 = trycatch count("fileName")

    // This is equivalent to:
    var cpt2 = catch count("filename")
    if @err() != null return
}

/**
# try
The caller can also **stop the execution** with `try`, and return to its own caller with the same error raised. The function must then also be marked with `throw`.

Here, the caller of `myFunc1` will also have to deal with the error.
*/
func myFunc1() throw
{
    // If 'count()' raises an error, 'myFunc1' will return with the same error
    var cpt = try count("filename")
}

/**
This is equivalent to:
*/
func myFunc2() throw
{
    // If 'count()' raises an error, 'myFunc2' will return with the same error
    var cpt = catch count("filename")
    if @err() != null
        throw @err()
}

/**
# assume
The caller can also panic if an error is raised, with `assume`.

> This can be disabled in release builds (in that case the behaviour is undefined).

*/
func myFunc3()
{
    // Here the program will stop with a panic message if 'count()' throws an error
    var cpt = assume count("filename")
}

/**
> If an error is never catched, then Swag will panic at runtime, as the top level caller always have an 'assume'.
*/

/**
# Blocks
You can use a block instead of one single statement (this does not create a scope).
*/
func myFunc4() throw
{
    // This is not really necessary, see below, but this is just to show 'try' with a block
    // instead of one single call
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

    // Works also for 'catch' if you do not want to deal with the error message.
    // calling '@err()' in that case is not really relevant.
    catch
    {
        var cpt4 = count("filename")
        var cpt5 = count("other filename")
    }

    // Works also for 'trycatch' if you do not want to deal with the error message
    // and you want to return as soon as an error is raised.
    trycatch
    {
        var cpt6 = count("filename")
        var cpt7 = count("other filename")
    }
}

/**
# Implicit try
When a function is marked with `throw`, the `try` for a function call is implicit **if not specified**. That means that most of the time it's not necessary to specify it if you do not want to (if you do not want to be explicit about it).
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
        // There's no need to add a 'try' before the call because 'mySubFunc1' is marked with 'throw'.
        // This is less verbose when you do not want to do something special in case
        // of errors (with 'assume', 'catch' or 'trycatch')
        mySubFunc2()        // implicit
        try mySubFunc3()    // explicit
    }

    catch mySubFunc1()
    @assert(@err() == MyError)
}

/**
# The error struct
We have seen that the error value is a struct. This means that you can add some specific error parameters, like the line and column numbers in the case of a syntax error for example.
*/

struct SyntaxError
{
    using base: Swag.BaseError
    line, col: u32
}

/**
But be aware that a reference to an external value (like a 'string', an 'any' etc.) must remain valid all the time. The runtime will drop complexe types when needed, so you could store complex things in the heap, or in a dedicated allocator in the current context.

*/

/**
# defer
Throwing an error is equivalent to returning from the function. So a 'defer' expression works also in that case.

But `defer` can have specific parameters like `defer(err)` or `defer(noerr)` to control if it should be executed depending on the error status.

| `defer(err)`   | will be called for each 'throw', so only when an error is raised
| `defer(noerr)` | will be called only when the function returns in a normal way
| `defer`        | will always be called whatever the reason is (normal way or error)
*/
var g_Defer = 0

func raiseError() throw
{
    throw MyError{"error"}
}

func testDefer(err: bool) throw
{
    defer(err)      g_Defer += 1    // This will be called in case an error is raised, before exiting
    defer(noerr)    g_Defer += 2    // This will only be called in case an error is not raised
    defer           g_Defer += 3    // This will be called in both cases
    if err
        raiseError()
}

#test
{
    g_Defer = 0
    catch testDefer(true)
    @assert(g_Defer == 4)   // Will call only defer(err) and the normal defer

    g_Defer = 0
    catch testDefer(false)
    @assert(g_Defer == 5)   // Will call only defer(noerr) and the normal defer
}
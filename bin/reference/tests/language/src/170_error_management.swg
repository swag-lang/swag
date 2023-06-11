/**
Swag contains a **very** simple error system used to deal with function returning errors.
It will probably be changed/improved at some point.

> These are **not** exceptions !

A function that can return an error must be marked with `throw`. It can then raise an error with the `throw` keyword, passing an error message.

> When an error is raised by a function, the return value is always equal to the **default value**, depending on the return type.
*/
func count(name: string)->u64 throw
{
    if name == null
    {
        // This function will return 0 in case of an error, because this is the default
        // value for 'u64'.
        throw "null pointer"
    }

    return @countof(name)
}

/**
The caller will then have to deal with the error in some way. It can `catch` it, and test (or not) its value with the `@errmsg` intrinsic. The execution will continue at the call site.
*/
func myFunc()
{
    // Dismiss the eventual error with 'catch' and continue execution
    cpt := catch count("fileName")

    // And test it with @errmsg, which returns the 'throw' corresponding string
    if @errmsg
    {
        @assert(cpt == 0)
        @print(@errmsg)
        return
    }

    // You can also use 'trycatch', which will exit the current function in case
    // an error has been raised (returning the default value if necessary)
    cpt1 := trycatch count("fileName")
}

/**
The caller can stop the execution with `try`, and return to its own caller with the same error raised. The function must then also be marked with `throw`.

Here, the caller of `myFunc1` will have to deal with the error at its turn.
*/
func myFunc1() throw
{
    // If 'count()' raises an error, 'myFunc1' will return with the same error
    cpt := try count("filename")
}

/**
The caller can also panic if an error is raised, with `assume`.

> This can be disabled in release builds (in that case the behaviour is undefined).

*/
func myFunc2()
{
    // Here the program will stop with a panic message if 'count()' throws an error
    cpt := assume count("filename")
}

/**
Note that you can use a block instead of one single statement (this does not create a scope).
*/
func myFunc3() throw
{
    // This is not really necessary, see below, but this is just to show 'try' with a block
    // instead of one single call
    try
    {
        cpt0 := count("filename")
        cpt1 := count("other filename")
    }

    assume
    {
        cpt2 := count("filename")
        cpt3 := count("other filename")
    }

    // Works also for 'catch' if you do not want to deal with the error message.
    // '@errmsg' in that case is not really relevant.
    catch
    {
        cpt4 := count("filename")
        cpt5 := count("other filename")
    }

    // Works also for 'trycatch' if you do not want to deal with the error message
    // and you want to return as soon as an error is raised.
    trycatch
    {
        cpt6 := count("filename")
        cpt7 := count("other filename")
    }
}

/**
When a function is marked with `throw`, the `try` for a function call is automatic if not specified. That means that most of the time it's not necessary to specify it.
*/
#test
{
    func mySubFunc2() throw
    {
        throw "error"
    }

    func mySubFunc1() throw
    {
        // In fact there's no need to add a 'try' before the call because 'mySubFunc1' is
        // marked with 'throw'
        // This is less verbose when you do not want to do something special in case
        // of errors (with 'assume', 'catch' or 'trycatch')
        mySubFunc2()
    }

    catch mySubFunc1()
    @assert(@errmsg == "error")
}

/**
# defer
`defer` can have parameters like `defer(err)` or `defer(noerr)` to control if it should be executed depending on the error status.
*/
var g_Defer = 0

func raiseError() throw
{
    throw "error"
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
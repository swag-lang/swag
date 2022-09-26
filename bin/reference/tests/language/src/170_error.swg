// swag contains a *very* simple error system used to deal with function returning errors
// Will probably be changed/improved at some point
// This are *not* exceptions !

// A function that can return an error must be marked with 'throw'
// It can then raise an error with the 'throw' keyword, passing an error message.
// When an error is raised by a function, the return value is always equal to the default value,
// depending on the return type.
func count(name: string)->uint throw
{
    // This function will return 0 in case of an error.
    if name == null
        throw "null pointer"

    return @countof(name)
}

// The caller will then have to deal with the error
// It can 'catch' it, and test (or not) its value with the '@err' intrinsic
// Execution will continue at the call site.
func myFunc()
{
    // Dismiss the eventual error with 'catch'
    cpt := catch count("fileName")

    // And test it with @err, which returns the 'throw' corresponding string
    if @err
    {
        @assert(cpt == 0)
        @print(@err)
        return
    }
}

// The caller can stop the execution with 'try', and return to its own caller with the same error raised.
// The function must then also be marked with 'throw'.
// Here, the caller of 'myFunc1' will have to deal with the error at its turn.
func myFunc1() throw
{
    // If 'count()' raises an error, 'myFunc1' will return with the same error
    cpt := try count("filename")
}

// The caller can also panic if an error is raised, with 'assume'.
// This can be disabled in release builds (in that case the behaviour is undefined).
func myFunc2()
{
    // Here the program will stop with a panic message if 'count()' throws an error
    cpt := assume count("filename")
}

// Note that you can use a block instead of one single statement (this does not create a scope)
func myFunc3() throw
{
    // This is not really necessary, see below, but this is just de show 'try' with a block
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
    // '@err' in that case is not really relevant.
    catch
    {
        cpt4 := count("filename")
        cpt5 := count("other filename")
    }
}

// When a function is marked with "throw", the "try" for a function call is automatic
// if not specified.
// That means that most of the time it's not necessary to specify it.
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
        // of errors (with 'assume' or 'catch')
        mySubFunc2()
    }

    catch mySubFunc1()
    @assert(@err == "error")
}

// 'defer' can have parameters like defer(err) or defer(noerr) to control
// if it should be executed depending on the error status
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
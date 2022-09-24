// swag contains a *very* simple error system used to deal with function returning errors
// Will probably be changed at some point

// A function that can return an error must be marked with 'throw'
// It must then raise an error with the 'throw' keyword, passing an error message.
// Note that when an error is raised by a function, the return value is always equal to the default value,
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
    cpt := catch count("fileName")
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

// When a function is marked with "throw", the "try" for a subfunction call is automatic
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
        // No need to add a 'try' before the call because mySubFunction1 is
        // marked with "throw"
        // Less verbose when you do not want to do something special in case
        // of errors (with "assume" or "catch")
        mySubFunc2()
    }

    catch mySubFunc1()
    @assert(@err == "error")
}

// defer can also have parameters like defer(err) or defer(noerr) to control
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
    catch testDefer(true)
    @assert(g_Defer == 4)   // Will call only defer(err) and normal defer
    catch testDefer(false)
    @assert(g_Defer == 9)   // Will call only defer(noerror) and normal defer
}
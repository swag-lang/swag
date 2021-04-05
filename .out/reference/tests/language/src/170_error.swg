// swag contains a simple error system used to deal with function returning errors

// A function that can return an error must be marked with 'throw'
// It must then raise an error with the 'throw' keyword, passing an error message.
// Note that when an error is raised by a function, the return value is always equal to the default value, depending on the return type.
func count(name: string)->uint throw
{
    // This function will return 0 in case of an error.
    if name == null throw "null pointer"

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

// The caller can stop the execution with 'try', and return to its own caller with the same error raised. The function must then also be marked with 'throw'.
// Here the caller of 'myFunc1' will have to deal with the error at its turn.
func myFunc1() throw
{
    // If 'count()' raises an error, 'myFunc1' will return
    cpt := try count("filename")
}

// The caller can also panic if an error is raised, with 'assume'.
// This can be disabled in release builds (in that case behaviour is undefined).
func myFunc2()
{
    // Here the program will stop with a panic message if 'count()'
    // throws an error
    cpt := assume count("filename")
}
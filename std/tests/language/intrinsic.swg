#test
{
    // Intrinsics are special functions hardcoded in the compiler

    // @print(string)
    // @print(s64)
    // @print(f64)
    //
    // Writes to the console
    {
        //@print("a")
        //@print(cast(s64) 0)
        //@print(cast(f64) 0)
    }

    // @assert(bool)
    //
    // Evaluates an expression, and stops if it is false
    {
        @assert(true)
    }

    // @sizeof(expression)
    //
    // Returns the size, in bytes, of the type of the expression (if possible)
    {
        @assert(@sizeof(u32) == 4)
        @assert(@sizeof(f64) == 8)
        @assert(@sizeof(bool) == 1)
    }

    // @args()->[..] string
    //
    // Get the program arguments, as passed by the user.
    // When compiling, user arguments can be specified with the --user-args option.
    {
        args := @args()
        @assert(@typeof(args) == @typeof([..] string))
    }

    // @isbytecode()->bool
    //
    // Returns true during bytecode execution, false in native code
    {
        if @isbytecode()
        {
            //@print("this is it")
        }
    }
}

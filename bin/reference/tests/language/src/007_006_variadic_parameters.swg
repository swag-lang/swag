/**
# Introduction to Variadic Functions
Variadic functions accept a variable number of arguments using '...'.
They allow flexibility in cases where the number of arguments is not known in advance.
*/
#test
{
    func myFunction(value: bool, parameters: ...)
    {
        // This function can accept any number of extra arguments after 'value'.
    }

    myFunction(true, 4, "true", 5.6) // Passes extra arguments after 'value'
}

/**
# Working with Variadic Parameters as Slices
Variadic parameters are treated as slices of type 'any', allowing you to process mixed argument types dynamically.
*/
#test
{
    func myFunction(parameters: ...)
    {
        // Check the number of arguments
        @assert(@countof(parameters) == 3)

        // Initially, each parameter is of type 'any'
        #assert(#typeof(parameters[0]) == any)
        #assert(#typeof(parameters[1]) == any)
        #assert(#typeof(parameters[2]) == any)

        // Determine actual runtime types
        @assert(@kindof(parameters[0]) == s32)
        @assert(@kindof(parameters[1]) == string)
        @assert(@kindof(parameters[2]) == f32)
    }

    myFunction(4, "true", 5.6)
}

/**
# Forcing Variadic Parameters to a Specific Type
When all arguments are of the same type, you can enforce it using type annotations.
This prevents parameters from defaulting to 'any'.
*/
#test
{
    func myFunction(value: bool, parameters: s32...)
    {
        // All 'parameters' elements must be of type 's32'
        #assert(#typeof(parameters[0]).name == "s32")
        #assert(#typeof(parameters[1]).name == "s32")
        #assert(#typeof(parameters[2]) == s32)
        #assert(#typeof(parameters[3]) == s32)

        // Check values
        @assert(parameters[0] == 10)
        @assert(parameters[1] == 20)
        @assert(parameters[2] == 30)
        @assert(parameters[3] == 40)
    }

    myFunction(true, 10, 20, 30, 40)
}

/**
# Passing Variadic Parameters Between Functions
Variadic parameters can be forwarded between functions while preserving their types and values.
*/
#test
{
    func A(params: ...)
    {
        @assert(@countof(params) == 2)
        @assert(@kindof(params[0]) == string)
        @assert(@kindof(params[1]) == bool)
        @assert(cast(string) params[0] == "value")
        @assert(cast(bool) params[1] == true)
    }

    func B(params: ...)
    {
        A(params) // Forward the parameters
    }

    B("value", true)
}

/**
# Combining Fixed and Variadic Parameters
You can mix fixed parameters with variadic ones to make function calls more expressive.
*/
private func print()
{
    func logMessage(prefix: string, messages: ...)
    {
        foreach msg in messages
        {
            @print(prefix, " => ", cast(string) msg)
        }
    }

    logMessage("Error:", "File not found", "Access denied", "Disk full")
}

/**
# Handling Different Types in Variadic Parameters
Handle mixed-type parameters dynamically, performing type-specific actions.
*/
#test
{
    func processParameters(params: ...)->s32
    {
        var sum = 0

        foreach p in params
        {
            switch @kindof(p)
            {
            case s32:
                sum += 1
            case string:
                sum += 10
            }
        }

        return sum
    }

    let result = processParameters(1, 2, "Hello, ", 3, "World!")
    @assert(result == 1 + 1 + 10 + 1 + 10)
}

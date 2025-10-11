/**
# Introduction to Variadic Functions

Variadic functions accept a variable number of arguments using `...`. This capability enables
functions to handle a flexible number of parameters, making them more versatile in scenarios
where the exact number of arguments is not known in advance.
*/

#test
{
    func myFunction(value: bool, parameters: ...)
    {
        // This function can now accept any number of additional arguments after 'value'.
    }

    myFunction(true, 4, "true", 5.6) // Passes 4, "true", and 5.6 as additional parameters.
}

/**
# Working with Variadic Parameters as Slices

When a function takes a variadic parameter, the `parameters` variable is treated as a slice of type
`any`. This feature allows the function to flexibly handle different types of arguments at runtime.
*/

#test
{
    func myFunction(parameters: ...)
    {
        // Determine the number of parameters passed.
        @assert(@countof(parameters) == 3)

        // Each parameter is initially treated as 'any' type.
        #assert(#typeof(parameters[0]) == any)
        #assert(#typeof(parameters[1]) == any)
        #assert(#typeof(parameters[2]) == any)

        // Use '@kindof' to determine the actual type of each parameter at runtime.
        @assert(@kindof(parameters[0]) == s32)
        @assert(@kindof(parameters[1]) == string)
        @assert(@kindof(parameters[2]) == f32)
    }

    myFunction(4, "true", 5.6) // Passes an integer, string, and float.
}

/**
# Forcing Variadic Parameters to a Specific Type

If all variadic parameters are of the same type, you can enforce that type using type annotations.
This practice makes the parameters' type explicit, ensuring they are not treated as `any`.
*/

#test
{
    func myFunction(value: bool, parameters: s32...)
    {
        // All parameters in 'parameters' must be of type 's32'.
        #assert(#typeof(parameters[0]).name == "s32")
        #assert(#typeof(parameters[1]).name == "s32")
        #assert(#typeof(parameters[2]) == s32)
        #assert(#typeof(parameters[3]) == s32)

        // Verify that the parameters have been passed correctly.
        @assert(parameters[0] == 10)
        @assert(parameters[1] == 20)
        @assert(parameters[2] == 30)
        @assert(parameters[3] == 40)
    }

    myFunction(true, 10, 20, 30, 40) // Passes four integers.
}

/**
# Passing Variadic Parameters Between Functions

You can pass variadic parameters from one function to another, preserving their types and values.
This technique is useful when you need to delegate tasks to other functions without losing the
variadic nature of the arguments.
*/

#test
{
    func A(params: ...)
    {
        // Expecting a string and a boolean.
        @assert(@countof(params) == 2)
        @assert(@kindof(params[0]) == string)
        @assert(@kindof(params[1]) == bool)
        @assert(cast(string) params[0] == "value")
        @assert(cast(bool) params[1] == true)
    }

    func B(params: ...)
    {
        // Pass the variadic parameters from B to A.
        A(params)
    }

    B("value", true) // Passes the parameters to function A through B.
}

/**
# Advanced Example: Combining Variadic and Non-Variadic Parameters

This example demonstrates how to combine fixed parameters with variadic parameters and use them
together in a function.
*/

private func print()
{
    func logMessage(prefix: string, messages: ...)
    {
        // Print each message with the given prefix.
        foreach msg in messages
        {
            @print(prefix, " => ", cast(string) msg)
        }
    }

    logMessage("Error: ", "File not found", "Access denied", "Disk full")
}

/**
# Example: Handling Different Types in Variadic Parameters

This example shows how to handle different types within a variadic function, such as summing
integers and concatenating strings.
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

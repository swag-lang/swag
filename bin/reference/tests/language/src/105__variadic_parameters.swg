#test
{
    // A function can take a variable number of arguments with `...`.
    {
        func myFunction(value: bool, parameters: ...)
        {
        }

        myFunction(true, 4, "true", 5.6)
    }

    // In that case, `parameters` is a slice of `any`.
    {
        func myFunction(parameters: ...)
        {
            // We can know the number of parameters at runtime with '@countof'
            @assert(@countof(parameters) == 3)

            // Each parameter is a type 'any'
            #assert @typeof(parameters[0]) == any
            #assert @typeof(parameters[1]) == any
            #assert @typeof(parameters[2]) == any

            // But you can use '@kindof' to get the real type (at runtime) of the parameter
            @assert(@kindof(parameters[0]) == s32)
            @assert(@kindof(parameters[1]) == string)
            @assert(@kindof(parameters[2]) == f32)
        }

        myFunction(4, "true", 5.6)
    }

    // If all variadic parameters are of the same type, you can force it. Parameters then won't be of type `any`.
    {
        func myFunction(value: bool, parameters: s32...)
        {
            #assert @typeof(parameters[0]).name == "s32"
            #assert @typeof(parameters[1]).name == "s32"
            #assert @typeof(parameters[2]) == s32
            #assert @typeof(parameters[3]) == s32

            @assert(parameters[0] == 10)
            @assert(parameters[1] == 20)
            @assert(parameters[2] == 30)
            @assert(parameters[3] == 40)
        }

        myFunction(true, 10, 20, 30, 40)
    }

    // Variadic parameters can be passed from function to function.
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
            A(params)
        }

        B("value", true)
    }
}

/**
You can spread the content of an array or a slice to variadic parameters with `@spread`.
*/

#test
{
    {
        func sum(params: s32...)->s32
        {
            // Note that variadic parameters can be visited, as this is a slice
            var total = 0
            visit v: params
                total += v
            return total
        }

        var arr = [1, 2, 3, 4]
        var res = sum(@spread(arr)) // is equivalent to sum(1, 2, 3, 4)
        @assert(res == 1+2+3+4)

        var res1 = sum(@spread(arr[1..2])) // is equivalent to sum(2, 3)
        @assert(res1 == 2+3)
    }
}
#test
{
    // A function can take a variable number of arguments with '...'
    {
        func myFunction(value: bool, parameters: ...)
        {
        }

        myFunction(true, 4, "true", 5.6)
    }

    // In that case, 'parameters' is a slice of 'any'
    {
        func myFunction(parameters: ...)
        {
            @assert(@typeof(parameters[0]) == any)
            @assert(@typeof(parameters[1]) == any)
            @assert(@typeof(parameters[2]) == any)

            @assert(@countof(parameters) == 3)

            @assert(@kindof(parameters[0]) == s32)
            @assert(@kindof(parameters[1]) == string)
            @assert(@kindof(parameters[2]) == f32)
        }

        myFunction(4, "true", 5.6)
    }

    // If all variadic parameters are of the same type, you can force it
    // Parameters then won't be of type 'any'
    {
        func myFunction(value: bool, parameters: s32...)
        {
            @assert(@typeof(parameters[0]).name == "s32")
            @assert(@typeof(parameters[1]).name == "s32")
            @assert(@typeof(parameters[2]) == s32)
            @assert(@typeof(parameters[3]) == s32)

            @assert(parameters[0] == 10)
            @assert(parameters[1] == 20)
            @assert(parameters[2] == 30)
            @assert(parameters[3] == 40)
        }

        myFunction(true, 10, 20, 30, 40)
    }

    // Variadic parameters can be passed from function to function
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
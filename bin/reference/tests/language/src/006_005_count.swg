struct MyStruct {}

/**
# Implementing `opCount` for Iteration

The `opCount` method in Swag allows you to specify the number of iterations when looping over an instance of a struct.
By defining this method, you can control how many times a loop executes, effectively treating the struct as an iterable object.
*/

impl MyStruct
{
    // Define 'opCount' to return the number of iterations for loops involving this struct.
    // Returns 4 as the count value, meaning any loop over this struct will run 4 times.
    mtd opCount() => 4'u64  
}

/**
With `opCount` defined, an instance of `MyStruct` can be looped over similarly to an array or other iterable types.
This allows you to use the struct in a loop context, where the loop will execute a number of times based on the value returned by `opCount`.
*/

#test
{
    var v = MyStruct{}

    // '@countof' utilizes 'opCount' to determine the number of elements (iterations)
    @assert(@countof(v) == 4)  // Verifies that the struct is considered to have 4 elements

    // Loop through the struct 'v', with the loop running as many times as 'opCount' returns
    var cpt = 0
    for v:
        cpt += 1  // Increment the counter each iteration

    @assert(cpt == 4)  // Ensure the loop ran 4 times, as specified by 'opCount'
}

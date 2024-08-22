struct MyStruct {}

impl MyStruct
{
    // Define 'opCount' to return the number of iterations for loops involving this struct.
    mtd opCount() => 4'u64  // Returns 4 as the count value
}

/**
With `opCount` defined, you can loop over an instance of `MyStruct` just like you would with an array or other iterable type.
*/
#test
{
    var v = MyStruct{}

    // '@countof' utilizes 'opCount' to determine the number of elements
    @assert(@countof(v) == 4)

    // Loop through the struct, with the loop running as many times as 'opCount' returns
    var cpt = 0
    loop v:
        cpt += 1  // Increment the counter each iteration

    @assert(cpt == 4)  // Ensure the loop ran 4 times, as specified by 'opCount'
}

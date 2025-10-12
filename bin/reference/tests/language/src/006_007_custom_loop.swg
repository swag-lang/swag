struct MyStruct {}

/**
# Implementing 'opCount' for Iteration

The 'opCount' method in Swag specifies how many iterations a loop
performs when looping over a struct instance. By defining it, you
make the struct act like an iterable object with a controlled length.
*/

impl MyStruct
{
    // Return the number of iterations for loops using this struct
    mtd opCount() => 4'u64
}

/**
With 'opCount' defined, an instance of 'MyStruct' can be used in loops
just like arrays or other iterable types. The loop executes the number
of times returned by 'opCount'.
*/

#test
{
    let v = MyStruct{}

    @assert(@countof(v) == 4)

    var cpt = 0
    for v do
        cpt += 1

    @assert(cpt == 4)
}

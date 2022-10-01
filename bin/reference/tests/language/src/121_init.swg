// @init can be used to reinitialize a variable/array to the default value

#test
{
    // For a simple variable, default value is 0
    {
        x := 666
        @init(&x)
        @assert(x == 0)
    }

    // Work also for an array, as you can specify the number of elements
    // you want to initialize
    {
        x := @[1, 2]
        @init(&x, 2)    // Initialize 2 elements
        @assert(x[0] == 0)
        @assert(x[1] == 0)
    }

    // You can also specify a "value" to initialize, instead of the default one
    {
        x := 666'f32
        @init(&x)(3.14)  // Initialize to 3.14 instead of zero
        @assert(x == 3.14)
    }

    // Same for an array
    {
        x := @[1, 2]
        @init(&x, 2)(555)
        @assert(x[0] == 555)
        @assert(x[1] == 555)
    }
}
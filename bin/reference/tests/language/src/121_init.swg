/**
# @init
`@init` can be used to reinitialize a variable/array to the default value.
*/

#test
{
    // For a simple variable, default value is 0.
    {
        x := 666
        @init(&x)
        @assert(x == 0)
    }

    // Work also for an array, as you can specify the number of elements you want to initialize.
    {
        x := [1, 2]
        @init(&x, 2)    // Initialize 2 elements
        @assert(x[0] == 0)
        @assert(x[1] == 0)
    }

    // You can also specify a *value* to initialize, instead of the default one.
    {
        x := 666'f32
        @init(&x)(3.14)  // Initialize to 3.14 instead of zero
        @assert(x == 3.14)
    }

    // Same for an array.
    {
        x := [1, 2]
        @init(&x, 2)(555)
        @assert(x[0] == 555)
        @assert(x[1] == 555)
    }
}

/**
When called on a struct, the struct will be restored to the values defined in it.
*/
#test
{
    struct RGB
    {
        r = 1
        g = 2
        b = 3
    }

    {
        var rgb: RGB{10, 20, 30}
        @init(&rgb)
        @assert(rgb.r == 1)
        @assert(rgb.g == 2)
        @assert(rgb.b == 3)
    }

    // But you can also specified the values.
    {
        var rgb: RGB{10, 20, 30}
        @init(&rgb)(5, 6, 7)
        @assert(rgb.r == 5)
        @assert(rgb.g == 6)
        @assert(rgb.b == 7)
    }

    // And this works also for array.
    {
        var rgb: [4] RGB
        @init(&rgb, 4)(5, 6, 7)
        @assert(rgb[3].r == 5)
        @assert(rgb[3].g == 6)
        @assert(rgb[3].b == 7)
    }
}

/**
# @drop
Note that for a struct, this will **not** call `opDrop`, so this is mostly useful to initialize a plain old data.

But there is also `@drop`, which works the same, except that it will `drop` all the content by calling `opDrop` if it is defined.
*/
#test
{
    struct RGB
    {
        r = 1
        g = 2
        b = 3
    }

    {
        var rgb: [4] RGB

        // In fact this is a no op, as struct RGB is plain old data, without a defined 'opDrop'
        @drop(&rgb, 4)

        @init(&rgb, 4)(5, 6, 7)
        @assert(rgb[3].r == 5)
        @assert(rgb[3].g == 6)
        @assert(rgb[3].b == 7)
    }
}
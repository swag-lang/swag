/**
# @init
`@init` can be used to reinitialize a variable/array to the default value.
*/

/**
For a simple variable, the default value is 0.
*/
#test
{
    var x = 666
    @init(&x, 1)
    @assert(x == 0)
}

/**
Work also for an array, as you can specify the number of elements you want to initialize.
*/
#test
{
    var x = [1, 2]
    @init(&x, 2)    // Initialize 2 elements
    @assert(x[0] == 0)
    @assert(x[1] == 0)
}

/**
You can also specify a *value* to initialize, instead of the default one.
*/
#test
{
    var x = 666'f32
    @init(&x, 1)(3.14)  // Initialize to 3.14 instead of zero
    @assert(x == 3.14)
}

/**
Same for an array.
*/
#test
{
    var x = [1, 2]
    @init(&x, 2)(555)
    @assert(x[0] == 555)
    @assert(x[1] == 555)
}


/**
When called on a struct, the struct will be restored to the values defined in it.
*/
#test
{
    struct RGB{r = 1, g = 2, b = 3}

    var rgb: RGB{10, 20, 30}
    @init(&rgb, 1)
    @assert(rgb.r == 1)
    @assert(rgb.g == 2)
    @assert(rgb.b == 3)
}

/**
But you can also specified the values.
*/
#test
{
    struct RGB{r = 1, g = 2, b = 3}

    var rgb: RGB{10, 20, 30}
    @init(&rgb, 1)(5, 6, 7)
    @assert(rgb.r == 5)
    @assert(rgb.g == 6)
    @assert(rgb.b == 7)
}

/**
And this works also for array.
*/
#test
{
    struct RGB{r = 1, g = 2, b = 3}

    var rgb: [4] RGB
    @init(&rgb, 4)(5, 6, 7)
    @assert(rgb[3].r == 5)
    @assert(rgb[3].g == 6)
    @assert(rgb[3].b == 7)
}

/**
# @drop
For a struct, `@init` will **not** call `opDrop`, so this is mostly useful to initialize a plain old data.

But there is also `@drop` intrinsic, which works the same, except that it will `drop` all the content by calling `opDrop` if it is defined.
*/
#test
{
    struct RGB{r = 1, g = 2, b = 3}

    var rgb: [4] RGB

    // In fact this is a no op, as struct RGB is plain old data, without a defined 'opDrop'
    @drop(&rgb, 4)

    @init(&rgb, 4)(5, 6, 7)
    @assert(rgb[3].r == 5)
    @assert(rgb[3].g == 6)
    @assert(rgb[3].b == 7)
}
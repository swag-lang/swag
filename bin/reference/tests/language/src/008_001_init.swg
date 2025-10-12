/**
# '@init' Intrinsic
The '@init' intrinsic reinitializes a variable or memory region to either:
- its type default value, or
- a provided custom value (or tuple of values for aggregates).
*/

/**
# Scalars — Default Initialization
Reinitializes a single variable to its default value.
*/
#test
{
    var x = 666
    @init(x) // Reinitialize 'x' to its default value (0)
    @assert(x == 0)
}

/**
# Arrays — Count-Based Default Initialization
Reinitializes a specified number of elements within an array or memory block.
*/
#test
{
    var x = [1, 2]

    @init(&x, 2) // Reset first 2 elements to default (0)
    @assert(x[0] == 0)
    @assert(x[1] == 0)

    x[0] = 1
    x[1] = 2

    // Reset the entire array
    @init(x)
    @assert(x[0] == 0)
    @assert(x[1] == 0)
}

/**
# Scalars — Initialization with a Specific Value
Reinitializes a variable with a given custom value instead of its default.
*/
#test
{
    var x = 666'f32

    @init(x)(3.14) // Reinitialize to 3.14 instead of 0
    @assert(x == 3.14)
}

/**
# Arrays — Initialization with a Specific Value
Initializes all elements in an array to a given value.
*/
#test
{
    var x = [1, 2]

    @init(&x, 2)(555) // Initialize both elements to 555
    @assert(x[0] == 555)
    @assert(x[1] == 555)
}

/**
# Structs — Reset to Declared Defaults
Reinitializes a struct instance to its declared default field values.
*/
#test
{
    struct RGB { r = 1, g = 2, b = 3 }

    var rgb: RGB{10, 20, 30}
    @assert(rgb.r == 10)
    @assert(rgb.g == 20)
    @assert(rgb.b == 30)

    @init(rgb) // Reset to default struct values
    @assert(rgb.r == 1)
    @assert(rgb.g == 2)
    @assert(rgb.b == 3)
}

/**
# Structs — Initialization with Specific Field Values
Initializes a struct instance with custom values for its fields.
*/
#test
{
    struct RGB { r = 1, g = 2, b = 3 }

    var rgb: RGB{10, 20, 30}
    @assert(rgb.r == 10)
    @assert(rgb.g == 20)
    @assert(rgb.b == 30)

    @init(rgb)(5, 6, 7) // Reinitialize with new field values
    @assert(rgb.r == 5)
    @assert(rgb.g == 6)
    @assert(rgb.b == 7)
}

/**
# Arrays of Structs — Bulk Initialization and Reinitialization
Reinitializes all elements of an array of structs with specified field values.
*/
#test
{
    struct RGB { r = 1, g = 2, b = 3 }

    var rgb: [4] RGB

    @init(&rgb, 4)(5, 6, 7) // Initialize all 4 elements to (5, 6, 7)
    @assert(rgb[3].r == 5)
    @assert(rgb[3].g == 6)
    @assert(rgb[3].b == 7)

    @init(rgb)(50, 60, 70) // Reinitialize entire array with new values
    @assert(rgb[3].r == 50)
    @assert(rgb[3].g == 60)
    @assert(rgb[3].b == 70)
}

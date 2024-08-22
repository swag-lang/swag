/**
# `@init` Intrinsic
The `@init` intrinsic in Swag is used to reinitialize a variable or a memory block to its default value. This is particularly useful when you need to reset the state of variables or memory without manually setting each field or element.
*/

/**
# Reinitializing a Single Variable
You can reinitialize a single variable to its default value by simply passing the variable as an argument to `@init`.
*/

#test
{
    var x = 666
    @init(x)           // Reinitialize 'x' to its default value
    @assert(x == 0)    // The default value for a simple variable like 'x' is 0
}

/**
# Reinitializing Multiple Elements
You can also specify a pointer to a memory block and the count of elements to reinitialize a specific number of elements.
*/

#test
{
    var x = [1, 2]
    @init(&x, 2)       // Reinitialize the first 2 elements of the array 'x'
    @assert(x[0] == 0) // Both elements are reset to 0
    @assert(x[1] == 0)

    x[0] = 1
    x[1] = 2
    @init(x)           // You can also reinitialize the entire variable directly
    @assert(x[0] == 0)
    @assert(x[1] == 0)
}

/**
# Initializing with a Specific Value
Instead of reinitializing a variable to its default value, you can specify a value to initialize it with.
*/

#test
{
    var x = 666'f32
    @init(x)(3.14)      // Reinitialize 'x' to 3.14 instead of 0
    @assert(x == 3.14)
}

/**
# Initializing Arrays with a Specific Value
The `@init` intrinsic can also be used to reinitialize all elements of an array with a specific value.
*/

#test
{
    var x = [1, 2]
    @init(&x, 2)(555)  // Reinitialize both elements of the array to 555
    @assert(x[0] == 555)
    @assert(x[1] == 555)
}

/**
# Reinitializing Structs
When you use `@init` on a struct, it restores the struct to the values defined in its declaration.
*/

#test
{
    struct RGB { r = 1, g = 2, b = 3 }

    var rgb: RGB{10, 20, 30}

    @assert(rgb.r == 10)
    @assert(rgb.g == 20)
    @assert(rgb.b == 30)

    @init(rgb)          // Reset 'rgb' to its default values
    @assert(rgb.r == 1)
    @assert(rgb.g == 2)
    @assert(rgb.b == 3)
}

/**
# Specifying Initialization Values for Structs
You can also specify the values to initialize a struct directly using `@init`.
*/

#test
{
    struct RGB { r = 1, g = 2, b = 3 }

    var rgb: RGB{10, 20, 30}

    @assert(rgb.r == 10)
    @assert(rgb.g == 20)
    @assert(rgb.b == 30)

    @init(rgb)(5, 6, 7) // Initialize 'rgb' with specific values
    @assert(rgb.r == 5)
    @assert(rgb.g == 6)
    @assert(rgb.b == 7)
}

/**
# Reinitializing Arrays of Structs
The `@init` intrinsic also works with arrays of structs, allowing you to initialize each element with specific values.
*/

#test
{
    struct RGB { r = 1, g = 2, b = 3 }

    var rgb: [4] RGB
    @init(&rgb, 4)(5, 6, 7) // Initialize all elements of the array to (5, 6, 7)
    @assert(rgb[3].r == 5)
    @assert(rgb[3].g == 6)
    @assert(rgb[3].b == 7)

    @init(rgb)(50, 60, 70)  // Reinitialize the array with new values
    @assert(rgb[3].r == 50)
    @assert(rgb[3].g == 60)
    @assert(rgb[3].b == 70)
}

/**
# `@drop` Intrinsic
While `@init` resets the variable without calling any destructors, the `@drop` intrinsic works similarly, but it also calls `opDrop` if it is defined for the struct. This ensures that any necessary cleanup (like freeing resources) is performed before reinitializing.
*/

#test
{
    struct RGB { r = 1, g = 2, b = 3 }

    var rgb: [4] RGB

    // Calling `@drop` on the array. If `RGB` had an `opDrop` defined, it would be called here.
    @drop(&rgb, 4)

    @init(&rgb, 4)(5, 6, 7) // Reinitialize after dropping
    @assert(rgb[3].r == 5)
    @assert(rgb[3].g == 6)
    @assert(rgb[3].b == 7)
}

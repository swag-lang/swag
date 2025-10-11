/**
# `@init` Intrinsic
The `@init` intrinsic in Swag is used to reinitialize a variable or a memory block to its default value. This intrinsic is especially useful when you need to reset the state of variables or memory blocks without manually setting each field or element. The ability to reset variables to their default states or to a specific value simplifies state management in Swag applications.
*/

/**
# Reinitializing a Single Variable
To reinitialize a single variable to its default value, simply pass the variable as an argument to `@init`.
*/

#test
{
    var x = 666
    @init(x) // Reinitialize 'x' to its default value
    @assert(x == 0) // The default value for a simple variable like 'x' is 0
}

/**
# Reinitializing Multiple Elements
You can also specify a pointer to a memory block and the count of elements to reinitialize a specific number of elements within that memory block. This is useful for resetting arrays or parts of them.
*/

#test
{
    var x = [1, 2]
    @init(&x, 2) // Reinitialize the first 2 elements of the array 'x'
    @assert(x[0] == 0) // Both elements are reset to 0
    @assert(x[1] == 0)

    x[0] = 1
    x[1] = 2
    @init(x) // You can also reinitialize the entire variable directly
    @assert(x[0] == 0)
    @assert(x[1] == 0)
}

/**
# Initializing with a Specific Value
The `@init` intrinsic can also initialize a variable with a specific value instead of its default. This provides a flexible way to reset variables to any desired state.
*/

#test
{
    var x = 666'f32
    @init(x)(3.14) // Reinitialize 'x' to 3.14 instead of 0
    @assert(x == 3.14)
}

/**
# Initializing Arrays with a Specific Value
The `@init` intrinsic can be applied to arrays to reinitialize all elements with a specific value, ensuring consistency across the array.
*/

#test
{
    var x = [1, 2]
    @init(&x, 2)(555) // Reinitialize both elements of the array to 555
    @assert(x[0] == 555)
    @assert(x[1] == 555)
}

/**
# Reinitializing Structs
When applied to structs, `@init` restores the struct to its default state as defined in its declaration. This is particularly useful for resetting complex data structures.
*/

#test
{
    struct RGB { r = 1, g = 2, b = 3 }

    var rgb: RGB{10, 20, 30}

    @assert(rgb.r == 10)
    @assert(rgb.g == 20)
    @assert(rgb.b == 30)

    @init(rgb) // Reset 'rgb' to its default values
    @assert(rgb.r == 1)
    @assert(rgb.g == 2)
    @assert(rgb.b == 3)
}

/**
# Specifying Initialization Values for Structs
You can also initialize a struct with specific values directly using `@init`, providing a convenient way to set all fields at once.
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
The `@init` intrinsic can be used with arrays of structs, allowing you to reinitialize each element with specific values. This is particularly useful for resetting large data structures efficiently.
*/

#test
{
    struct RGB { r = 1, g = 2, b = 3 }

    var rgb: [4] RGB
    @init(&rgb, 4)(5, 6, 7) // Initialize all elements of the array to (5, 6, 7)
    @assert(rgb[3].r == 5)
    @assert(rgb[3].g == 6)
    @assert(rgb[3].b == 7)

    @init(rgb)(50, 60, 70) // Reinitialize the array with new values
    @assert(rgb[3].r == 50)
    @assert(rgb[3].g == 60)
    @assert(rgb[3].b == 70)
}

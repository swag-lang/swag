/**
# Understanding `union` in Swag

A `union` is a specialized type of struct where all fields share the same memory location, meaning
they all start at the same offset (0). This allows multiple fields to occupy the same space in memory,
making it possible to store different types of data in the same memory space at different times.
This feature is particularly useful for memory optimization in scenarios where only one of the
fields needs to be used at any given moment.
*/

#test
{
    // Define a union with three fields: x, y, and z, all of type f32.
    union MyUnion
    {
        x, y, z: f32                                              // Each field shares the same memory location
    }

    // Initialize the union, setting the value of the 'x' field to 666.
    let v = MyUnion{x: 666}                                       // 'x' is set, and by extension, 'y' and 'z' as well

    // Since all fields share the same memory, setting 'x' also sets 'y' and 'z' to the same value.
    @assert(v.y == 666)                                           // Assert that 'y' equals 666
    @assert(v.z == 666)                                           // Assert that 'z' equals 666
}

/**
# Explanation of Union Behavior

In this example, the `union` type allows you to define multiple fields (`x`, `y`, `z`), all of which
occupy the same memory location. Because these fields overlap in memory, changing the value of one
field automatically changes the others. This behavior is useful in situations where you need to
store and access different data types within the same memory space, albeit not simultaneously.
Here, `x`, `y`, and `z` are all of type `f32`, so when `x` is set to 666, both `y` and `z` reflect
this change, illustrating the shared memory concept inherent in unions.
*/

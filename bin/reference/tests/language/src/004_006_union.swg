/**
# The `union` Type in Swag

A `union` is a special kind of struct where all fields share the same memory location.
Each field starts at offset `0`, allowing multiple variables to overlap in memory.

This makes it possible to represent different data types using the same storage space,
which is particularly useful for optimizing memory when only one field is used at a time.
*/
#test
{
    // Define a union with three overlapping f32 fields: x, y, and z
    union MyUnion { x, y, z: f32 }

    // Initialize the union, setting the value of the 'x' field
    let v = MyUnion{x: 666}

    // All fields share the same memory location — updating one affects all
    @assert(v.y == 666)
    @assert(v.z == 666)
}

/**
# How Unions Work

In this example, the `MyUnion` type defines three fields (`x`, `y`, and `z`) that occupy the same
memory space. Writing to one field overwrites the others, since they are all stored at the same offset.

This design is ideal for cases where different data types (or values) are stored alternately in the
same memory area. Here, since all fields are of type `f32`, assigning a value to `x` automatically
updates `y` and `z`, demonstrating the shared-memory behavior of unions.
*/

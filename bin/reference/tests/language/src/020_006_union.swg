/**
An `union` is a special type of struct where all fields share the same memory location, meaning they all start at offset 0. This allows multiple fields to occupy the same space in memory.
*/

#test
{
    // Define a union with three fields: x, y, and z, all of type f32.
    union MyUnion
    {
        x, y, z: f32
    }

    // Initialize the union, setting the value of x to 666.
    var v = MyUnion{x: 666}

    // Since all fields share the same memory, setting x also sets y and z.
    @assert(v.y == 666)
    @assert(v.z == 666)
}

/**
In this example, the union type allows you to define multiple fields (x, y, z), but since they all occupy the same memory location, modifying one field will affect the others. This is useful when you need to store different types of data in the same memory space but at different times. Here, 'x', 'y', and 'z' are all of type 'f32', so changing one affects the others since they overlap in memory.
*/
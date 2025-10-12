/**
# Custom Field Layout with 'Swag.Offset'

You can force the layout of a field within a struct using the 'Swag.Offset' attribute.
This lets you manually specify the memory offset of a field — useful for custom memory
layouts, such as overlapping fields or sharing memory space between fields.
*/
#test
{
    struct MyStruct
    {
        x:   s32

        // 'y' is located at the same offset as 'x', so they share the same memory.
        // Changing one reflects in the other (overlay behavior).
        #[Swag.Offset("x")]
        y:   s32
    }

    // Even with two fields, they overlap, so the struct uses only 4 bytes.
    #assert(#sizeof(MyStruct) == 4)

    var v = MyStruct{}
    v.x = 666

    // Since 'x' and 'y' share memory, updating 'x' updates 'y'.
    @assert(v.y == 666)
}

/**
# Using 'Swag.Offset' for Indexed Field Access

Here, 'Swag.Offset' is used so an indexed array aliases multiple fields,
enabling indexed access to those fields via a single view.
*/
#test
{
    struct MyStruct
    {
        x, y, z: f32

        // 'idx' aliases the same memory as 'x', 'y', and 'z'.
        // Access 'x', 'y', and 'z' through indexed reads/writes on 'idx'.
        #[Swag.Offset("x")]
        idx: [3] f32
    }

    var v: MyStruct
    v.x = 10; v.y = 20; v.z = 30

    // Each index in 'idx' maps directly to x/y/z.
    @assert(v.idx[0] == v.x)
    @assert(v.idx[1] == v.y)
    @assert(v.idx[2] == v.z)
}

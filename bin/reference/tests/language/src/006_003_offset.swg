/**
# Custom Field Layout with `Swag.Offset`

You can force the layout of a field within a struct using the `Swag.Offset` attribute. 
This allows you to manually specify the memory offset of a field, which can be useful 
for creating custom memory layouts, such as overlapping fields or sharing memory space 
between different fields.
*/
#test
{
    struct MyStruct
    {
        x: s32

        // 'y' is located at the same offset as 'x', meaning they share the same memory space.
        // This effectively creates an overlay, where changes to one field affect the other.
        #[Swag.Offset("x")]
        y: s32
    }

    // Although there are two fields defined, they occupy the same space, so the struct only uses 4 bytes.
    #assert @sizeof(MyStruct) == 4

    var v = MyStruct{}
    v.x = 666

    // Since 'x' and 'y' share the same memory space, modifying 'x' also modifies 'y'.
    @assert(v.y == 666)
}

/**
# Using `Swag.Offset` for Indexed Field Access

In this example, `Swag.Offset` is used to reference a group of fields by index, 
enabling indexed access to multiple fields through a single array.
*/
#test
{
    struct MyStruct
    {
        x, y, z: f32

        // 'idx' is an array that references the same memory locations as 'x', 'y', and 'z'.
        // This allows you to access 'x', 'y', and 'z' via indexed access through 'idx'.
        #[Swag.Offset("x")]
        idx: [3] f32
    }

    var v: MyStruct
    v.x = 10; v.y = 20; v.z = 30

    // Each index in 'idx' directly references 'x', 'y', and 'z'.
    @assert(v.idx[0] == v.x)
    @assert(v.idx[1] == v.y)
    @assert(v.idx[2] == v.z)
}

/**
You can force the layout of a field with the `Swag.Offset` attribute.
*/
#test
{
    struct MyStruct
    {
        x: s32

        // 'y' is located at the same offset as 'x', so they share the same space
        #[Swag.Offset("x")]
        y: s32
    }

    // They are 2 fields in the struct but stored in only one s32, so the total size is 4 bytes.
    #assert @sizeof(MyStruct) == 4

    var v = MyStruct{}
    v.x = 666

    // As 'y' and 'x' are sharing the same space, affecting to 'x' also affects to 'y'.
    @assert(v.y == 666)
}

/**
An example to reference a field by index.
*/
#test
{
    struct MyStruct
    {
        x, y, z: f32

        #[Swag.Offset("x")]
        idx: [3] f32
    }

    var v: MyStruct
    v.x = 10; v.y = 20; v.z = 30
    @assert(v.idx[0] == v.x)
    @assert(v.idx[1] == v.y)
    @assert(v.idx[2] == v.z)
}

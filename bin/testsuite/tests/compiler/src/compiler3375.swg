//#global skip
struct(T) SoaTest
{
    #ast
    {
        return "x: s32"
    }
}

impl SoaTest
{
    mtd reserve(size: uint)
    {
    }
}

#test
{
    struct Test
    {
        x, y, z: f32
    }

    var v: SoaTest'Test
    v.reserve(10)
}
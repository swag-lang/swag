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
    mtd reserve(size: u64) {}
}

#test
{
    struct Test { x, y, z: f32 }

    var v: SoaTest'Test
    v.reserve(10)
}

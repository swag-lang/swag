struct A
{
    x: s32 = 666
}

impl A
{
    mtd opIndex(i: u64)->const ref s32
    {
        return &x
    }
}

//#[Swag.PrintBc]
#test
{
    var a: A
    var b = &a[0]
    @assert(@typeof(b).kind == .Pointer)
    @assert(dref b == 666)
}
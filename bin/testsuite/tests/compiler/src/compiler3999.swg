struct A
{
    x: s32 = 666
}

impl A
{
    mtd opIndex(i: uint)->const ref s32
    {
        return &x
    }
}

//#[Swag.PrintBc]
#test
{
    var a: A
    b := &a[0]
    @assert(@typeof(b).kind == .Pointer)
    @assert(dref b == 666)
}
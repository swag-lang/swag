struct C {}

impl C
{
    mtd opCast()->s32 { return 666; }
}

struct D
{
    x:   C
}

//#[Swag.PrintBc]
#test
{
    var x: D
    let v  = cast(s32) x.x
    @assert(v == 666)
}

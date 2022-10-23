struct B
{
    x: s32 = 42
}

impl B
{
    //#[Swag.Inline]
    mtd opData()->const *u8 => null
}

//#[Swag.PrintBc]
#test
{
    var c: B
    y := c orelse B{}
    @assert(y.x == 42)
}

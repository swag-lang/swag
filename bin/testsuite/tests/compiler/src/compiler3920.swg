#global testerror "Err0166"
struct B
{
    x: s32 = 42
}

impl B
{
    //#[Swag.Inline]
    mtd opData()->const *u8 => null

    mtd titi()->B => {}
}

//#[Swag.PrintBc]
#test
{
    var c: B
    var y  = c.titi() orelse c.titi()
    @assert(y == null)
}

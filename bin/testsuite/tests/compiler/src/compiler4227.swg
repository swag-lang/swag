struct A
{
    x:   u32 = 1
}

impl A
{
    mtd opDrop()
    {
        me.x = 666
    }
}

#[Swag.Inline]
func toto()->A
{
    var result: retval = A{55}
    return result
}

//#[Swag.PrintBc]
//#[Swag.Optimize(.All, false)]
#test
{
    let x = toto()
    //@print(x.x)
    @assert(x.x == 55)
}

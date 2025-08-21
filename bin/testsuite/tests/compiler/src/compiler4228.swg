struct A
{
    x: u32 = 1
}

impl A
{
    mtd opDrop()
    {
        x = 666
    }

    mtd(op:string) opBinary(other: A)->A
    {
        return {x+other.x}
    }
}

#[Swag.Inline]
func toto()->A
{
    var result: retval = A{55}
    return result
}

//#[Swag.PrintBc]
//#[Swag.Optimize("", false)]
#test
{
    var x = toto() + toto()
    //@print(x.x)
    @assert(x.x == 110)
}
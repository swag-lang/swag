struct XX
{
    x:   s32 = 666
}

struct AA
{
    x:             s32 = 555
    using xx0:     XX
}

struct BB
{
    xx:            f64
    using aa0:     AA
}

interface I
{
    func toto(me);
}

impl I for XX
{
    mtd impl toto()
    {
        @assert(me.x == 666)
    }
}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    var x:  BB
    let ptr = cast(I) x
    ptr.toto()
}

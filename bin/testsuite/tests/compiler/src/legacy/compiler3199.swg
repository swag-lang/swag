struct AA
{
    x:   s32 = 555
}

struct BB
{
    xx:           f64
    using aa:     AA
}

interface I
{
    func toto(me);
}

impl I for AA
{
    mtd impl toto()
    {
        @assert(me.x == 555)
    }
}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    var x:  BB
    let ptr = cast(I) x
    ptr.toto()
}

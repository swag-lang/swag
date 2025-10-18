#global testerror "Err0609"
#global testerror "Err0349"
struct AA
{
    x:   s32 = 555
}

struct BB
{
    xx:            f64
    using aa0:     AA
    using aa1:     AA
}

interface I
{
    func toto(me);
}

impl I for AA
{
    mtd toto()
    {
        @assert(me.x == 555)
    }
}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    var x:  BB
    var ptr = cast(I) x
    ptr.toto()
}

#global testerror "Err0434"
#global testerror "Err0019"
struct AA
{
    x: s32 = 555
}

struct BB
{
    xx: f64
    using aa0: AA
    using aa1: AA
}

interface I
{
    func toto(self);
}

impl I for AA
{
    mtd toto()
    {
        @assert(x == 555)
    }
}

//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    var x: BB
    var ptr = cast(I) x
    ptr.toto()
}
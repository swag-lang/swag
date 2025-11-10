struct Base
{
    x:   s32 = 555
}

struct BB
{
    using base: Base
}

interface I
{
    func init(me)->s32;
    func close(me)->s32;
}

impl I for Base
{
    mtd impl init()->s32  => 1
    mtd impl close()->s32 => 2
}

impl I for BB
{
    mtd impl init()->s32  => 10
    mtd impl close()->s32 => 20
}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    var x:  BB
    let ptr = cast(I) x
    @assert(ptr.init() == 10)
    @assert(ptr.close() == 20)
}

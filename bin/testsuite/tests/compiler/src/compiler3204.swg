struct Base
{
    x: s32 = 555
}

struct BB
{
    x: [4] bool
    using base: Base
}

interface I
{
    func init(self)->s32;
    func close(self)->s32;
}

impl I for Base
{
    mtd impl init()->s32 => 1
    mtd impl close()->s32 => 2
}

impl I for BB
{
    //#[Swag.PrintBc]
    mtd impl init() => base.I.init()

    mtd impl close()->s32 => 20
}

//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    var x: BB
    let ptr = cast(I) x
    @assert(ptr.init() == 1)
    @assert(ptr.close() == 20)
}
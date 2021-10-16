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
    init:  func(self)->s32
    close: func(self)->s32
}

impl I for Base
{
    mtd init()->s32 => 1
    mtd close()->s32 => 2
}

impl I for BB
{
    //#[Swag.PrintBc]
    mtd init() => base.I.init()

    mtd close()->s32 => 20
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var x: BB
    ptr := cast(I) x
    @assert(ptr.init() == 1)
    @assert(ptr.close() == 20)
}
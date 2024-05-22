struct XX
{
    x: s32 = 666
}

struct AA
{
    x: s32 = 555
    using xx0: XX
}

struct BB
{
    xx: f64
    using aa0: AA
}

interface I
{
    func toto(self);
}

impl I for XX
{
    mtd impl toto()
    {
        @assert(x == 666)
    }
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var x: BB
    let ptr = cast(I) x
    ptr.toto()
}
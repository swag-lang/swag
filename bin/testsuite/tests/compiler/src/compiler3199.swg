struct AA
{
    x: s32 = 555
}

struct BB
{
    xx: f64
    using aa: AA
}

interface I
{
    toto: func(self)
}

impl I for AA
{
    mtd impl toto()
    {
        @assert(x == 555)
    }
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var x: BB
    ptr := cast(I) x
    ptr.toto()
}
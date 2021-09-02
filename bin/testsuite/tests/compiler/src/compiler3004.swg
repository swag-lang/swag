struct R
{
    x: s32 = 10
}

impl R
{
    #[Swag.Inline]
    func Y(using self)->s32
    {
        return x + 1
    }
}

func toto(x: R)
{
    @assert(x.x == 11)
}

#test
{
    var v: R
    toto(@{v.Y()})
}
struct R
{
    x:   s32 = 10
}

impl R
{
    #[Swag.Inline]
    func Y(me)->s32
    {
        return me.x + 1
    }
}

func toto(x: R)
{
    @assert(x.x == 11)
}

#test
{
    var v: R
    toto({v.Y()})
}

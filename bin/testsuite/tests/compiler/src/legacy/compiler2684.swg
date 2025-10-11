struct AA
{
    x:   s64
}

impl AA
{
    #[Swag.Implicit, Swag.Complete]
    func opAffect(me, p: s64)
    {
        me.x = p
    }
}

func toto(v, v1: AA)
{
    @assert(v.x == 2)
    @assert(v1.x == 9)
}

func toto1()
{
    toto(2's64, 4 + 5's64)
}

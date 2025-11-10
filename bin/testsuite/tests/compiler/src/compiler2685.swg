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

    func toto(me, v: AA)
    {
        @assert(v.x == 2)
    }

    func toto1(me)
    {
        me.toto(2's64)
    }
}

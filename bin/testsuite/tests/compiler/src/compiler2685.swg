struct AA
{
    x: s64
}

impl AA
{
    #[Swag.Implicit, Swag.Complete]
    func opAffect(using me, p: s64)
    {
        x = p
    }

    func toto(using me, v: AA)
    {
        @assert(v.x == 2)
    }

    func toto1(using me)
    {
        toto(2's64)
    }
}
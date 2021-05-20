struct AA
{
    x: s64
}

impl AA
{
    #[Swag.implicit, Swag.complete]
    func opAffect(using self, p: s64)
    {
        x = p
    }

    func toto(using self, v: AA)
    {
        @assert(v.x == 2)
    }

    func toto1(using self)
    {
        toto(2's64)
    }
}
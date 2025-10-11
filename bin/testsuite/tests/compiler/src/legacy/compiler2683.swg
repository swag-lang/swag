struct AA
{
    x:   s64 = 2
}

func toto(v, v1: s64)
{
    @assert(v == 2)
    @assert(v1 == 2)
}

func toto1()
{
    var x, y: AA
    toto(x, y)
}

impl AA
{
    #[Swag.Implicit]
    func opCast(me)->s64
    {
        return me.x
    }
}

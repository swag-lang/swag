struct AA
{
    x: s64 = 2
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
    #[swag.implicit]
    func opCast(using self)->s64
    {
        return x
    }
}
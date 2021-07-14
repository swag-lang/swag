struct AA
{
    x: s64
}

impl AA
{
    #[Swag.Inline]
    func from(x: s64)->AA
    {
        return @{x}
    }
}

func toto(v1, v2: AA)
{
    @assert(v1.x == 1)
    @assert(v2.x == 2)
}

//#[Swag.PrintBc]//, Swag.Optim("", false)]
#test
{
    toto(AA.from(1), AA.from(2))
}
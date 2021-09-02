struct A
{
    x: s32
}

impl A
{
    #[Swag.Inline]
    func from(x: s32)->A
    {
        return @{x}
    }
}

func toto(x: A = A.from(10))
{
    @assert(x.x == 10)
}

#test
{
    toto()
}
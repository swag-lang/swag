/*struct A
{
    x: s32
}

impl A
{
    #[swag.inline]
    func from(x: s32)->A
    {
        return @{x}
    }
}

func toto(x, y: A = A.from(10))
{
    @assert(x.x == 10)
    @assert(y.x == 10)
}

#test
{
    toto()
}*/
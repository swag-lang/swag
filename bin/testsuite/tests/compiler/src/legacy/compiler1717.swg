#test
{
    func toto(x: func(s32)->s32)->s32
    {
        return x(4) * 4
    }

    @assert(toto(func(x) { return x * x; }) == 64)
}

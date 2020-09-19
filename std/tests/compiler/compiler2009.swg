var G = 0

func(T) toto(y: func(s32)->s32)
{
    @assert(y(6) == 6)
    G = y(5)
}

func(T) toto1()
{
    toto'T(@(x: s32) => x)
}

#test
{
    toto1's64()
    @assert(G == 5)
}
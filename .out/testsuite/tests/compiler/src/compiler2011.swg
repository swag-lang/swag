var G = 0's64

func(T) toto(y: func(T)->T)
{
    @assert(y(6) == 6)
    G = y(5)
}

func(T) toto1()
{
    toto'T(@(x) => x)
}

#test
{
    toto1's64()
    @assert(G == 5)
}
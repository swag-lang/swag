var G = 0

func(T) toto(y: T)
{
    func xx()
    {
        G = 1
    }

    xx()
}

#test
{
    toto(666's64)
    @assert(G == 1)
}
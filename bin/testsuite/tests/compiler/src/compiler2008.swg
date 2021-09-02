var G = 0

func(T) toto(y: T)
{
    {
        func xx(v: T)
        {
            G += 1
            @assert(v == 667)
        }

        xx(y + 1)
    }

    {
        func xx(v: T)
        {
            G += 2
            @assert(v == 666)
        }

        xx(y)
    }
}

#test
{
    toto(666's64)
    @assert(G == 3)
}
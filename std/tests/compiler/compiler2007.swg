func(T) toto(y: T)
{
    func xx(v: T)
    {
        @assert(v == 666)
    }

    xx(y)
}

#test
{
    toto(666's64)
    //@assert(G == 1)
}
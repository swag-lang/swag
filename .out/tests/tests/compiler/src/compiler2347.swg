var G = 0

func toto()
{
    loop x: 10
    {
        if x == 1
            return
        defer G += 1
    }
}

#test
{
    toto()
    @assert(G == 1)
}
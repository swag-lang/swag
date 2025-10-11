var G = 0

func toto()
{
    for x in 10
    {
        if x == 1 do
            return
        defer G += 1
    }
}

#test
{
    toto()
    @assert(G == 1)
}

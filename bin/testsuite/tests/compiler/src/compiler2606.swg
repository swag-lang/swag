var G = 0

func(T) titi()
{
    func toto()
    {
        func tata()
        {
            @assert(@typeof(T) == s32)
            G = 1
        }

        tata()
    }

    toto()
}

#test
{
    titi's32()
    @assert(G == 1)
}
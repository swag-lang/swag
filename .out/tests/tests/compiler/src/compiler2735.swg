var G = 0

func titi() throw
{
    throw "titi"
}

func toto() throw
{
    defer G = 1
    while true
    {
        try titi()
    }
}

#test
{
    catch toto()
    @assert(G == 1)
}
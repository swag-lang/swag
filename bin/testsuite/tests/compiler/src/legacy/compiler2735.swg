var G = 0

struct ErrTiti {}

func titi() throw
{
    throw ErrTiti{}
}

func toto() throw
{
    defer G = 1
    for
    {
        try titi()
    }
}

#test
{
    catch toto()
    @assert(G == 1)
}

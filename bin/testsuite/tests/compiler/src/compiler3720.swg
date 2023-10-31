func toto() throw
{
    throw "toto"
}

func tata() throw
{
    throw "tata"
}

func read() throw
{
    throw "read"
}

func titi() throw
{
    defer toto()
    defer tata()
    read()
}

#test
{
    catch titi()
    @assert(cast(string) @err() == "tata")
}
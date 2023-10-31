func tata() throw
{
    throw "error tata"
}

func toto()
{
    catch tata()
    @assert(cast(string) @err() == "error tata")
}

func titi() throw
{
    defer toto()
    throw "error titi"
}

#test
{
    catch titi()
    @assert(cast(string) @err() == "error titi")
}
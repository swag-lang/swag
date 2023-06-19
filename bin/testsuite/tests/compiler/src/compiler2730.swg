func tata() throw
{
    throw "error tata"
}

func toto()
{
    catch tata()
    @assert(@errmsg == "error tata")
}

func titi() throw
{
    defer toto()
    throw "error titi"
}

#test
{
    catch titi()
    @assert(@errmsg == "error titi")
}
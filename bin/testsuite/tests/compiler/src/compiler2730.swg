func tata() throw
{
    throw "error tata"
}

func toto()
{
    catch tata()
    @assert(@err == "error tata")
}

func titi() throw
{
    defer toto()
    throw "error titi"
}

#test
{
    catch titi()
    @assert(@err == "error titi")
}
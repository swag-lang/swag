func titi() throw
{
    throw "titi"
}

func tata() throw
{
    defer catch titi()
    throw "tata"
}

#test
{
    catch tata()
}
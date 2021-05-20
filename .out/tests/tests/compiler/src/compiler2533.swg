func toto() throw
{
    throw "toto"
}

func titi()->string throw
{
    try toto()
    return "666"
}

//#[Swag.printbc]
#test
{
    x := catch titi()
    @assert(x == null)
}

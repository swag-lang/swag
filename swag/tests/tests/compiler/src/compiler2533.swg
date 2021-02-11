func toto()
{
    throw "toto"
}

func titi()->string
{
    try toto()
    return "666"
}

//#[swag.printbc]
#test
{
    x := catch titi()
    @assert(x == null)
}
